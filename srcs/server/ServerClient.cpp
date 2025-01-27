/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2025/01/28 00:16:33 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "Server.hpp"

ServerClient::ServerClient(
    const int& client_socket_fd, const int& address_socket_fd, Server* server
)
    : _is_started(false), _client_socket_fd(client_socket_fd),
      _address_socket_fd(address_socket_fd), _server(server)
{
}

ServerClient::ServerClient(const ServerClient& client)
{
    *this = client;
}

ServerClient& ServerClient::operator=(const ServerClient& client)
{
    if (this == &client)
        return *this;
    _is_started        = client._is_started;
    _client_socket_fd  = client._client_socket_fd;
    _address_socket_fd = client._address_socket_fd;
    _epoll_ev          = client._epoll_ev;
    _server            = client._server;
    _request           = client._request;
    _responses_queue   = client._responses_queue;
    return *this;
}

ServerClient::~ServerClient() {}

void ServerClient::BindToClientSocket()
{
    assert(_is_started == false);
    _epoll_ev.data.ptr = this;
    _epoll_ev.events   = EPOLLIN | EPOLLOUT;
    try
    {
        IOMultiplexer::GetInstance().AddEvent(_epoll_ev, _client_socket_fd);
        std::cout << "New peer accepted on fd " << _client_socket_fd << ".\n";
        _is_started = true;
    }
    catch (const std::exception& e)
    {
        close(_client_socket_fd);
        std::cerr << "_IOmltplx->AddEvent() failed to add new peer fd: " << _client_socket_fd
                  << "Connection will terminate.\nReason: " << e.what() << std::endl;
    }
}
void ServerClient::ConsumeEvent(const epoll_event ev)
{
    assert(ev.data.fd == _client_socket_fd);
    if (ev.events & EPOLLIN)
        HandleEPOLLIN();
    if (ev.events & EPOLLOUT)
        HandleEPOLLOUT();
}
void ServerClient::HandleEPOLLIN()
{
    std::vector<uint8_t> buffer(RECV_CHUNK, 0);
    ssize_t              bytes = recv(_client_socket_fd, buffer.data(), RECV_CHUNK, MSG_DONTWAIT);
    if (bytes < 0)
    {
        std::cerr << "recv() failed. for client fd: " << _client_socket_fd << '\n';
        std::cerr << "Client will be kicked out." << std::endl;
        return Terminate();
    }
    if (bytes == 0)
        return Terminate();
    if (bytes < READ_CHUNK)
        buffer.erase(buffer.begin() + bytes, buffer.end());
    ReceiveRequest(buffer);
}
void ServerClient::HandleEPOLLOUT()
{
fetch_next_response:
    Response* response = _responses_queue.front();
    if (response->GetResponseBuffCount() == 0)
    {
        delete response;
        _responses_queue.pop();
        if (_responses_queue.size())
            goto fetch_next_response;
        else
            return;
    }
    const uint8_t* buff;
    try
    {
        buff = response->GetResponseBuff();
    }
    catch (const std::exception& e)
    {
        assert(!"NOT IMPLEMENTED");
        std::cerr << e.what() << '\n';
        return;
    }
    size_t bytes_to_send = response->GetResponseBuffCount();
    if (bytes_to_send > SEND_CHUNK)
        bytes_to_send = SEND_CHUNK;
    ssize_t bytes_sent = send(_client_socket_fd, buff, bytes_to_send, MSG_DONTWAIT);
    if (bytes_sent < 0)
    {
        std::cerr << "send() failed. for client fd: " << _client_socket_fd << '\n';
        std::cerr << "Client will be kicked out." << std::endl;
        return Terminate();
    }
    if ((size_t)bytes_sent != bytes_to_send)
        std::cerr << "Tried to send " << bytes_to_send << " but send() only sent " << bytes_sent
                  << "\nRemainder data will be sent on next epoll event call." << std::endl;
    response->IncrementResponseBuffBytesSent(bytes_sent);
    if (response->GetResponseBuffCount() == 0)
    {
        delete response;
        _responses_queue.pop();
    }
}
void ServerClient::Terminate()
{
    if (_is_started == false)
        throw ServerClientException("ServerClient::Terminate(): client not started.");
    for (; _responses_queue.size(); _responses_queue.pop())
        delete _responses_queue.front();
    try
    {
        IOMultiplexer::GetInstance().RemoveEvent(_epoll_ev, _client_socket_fd);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception caught while terminating client fd: " << _client_socket_fd
                  << std::endl;
        std::cerr << "Reason: " << e.what() << std::endl;
    }
    close(_client_socket_fd);
    _is_started = false;
    std::cout << "Client fd: " << _client_socket_fd << " disconnected." << std::endl;
}

void ServerClient::ReceiveRequest(const std::vector<uint8_t>& buff)
{
    _request += buff;

    if (_request.isCompleted())
    {
        HttpStatus status = _request.getStatus();
        if (status.code != STATUS_OK)
        {
            Response* response = new Response(_server);
            response->SetClientSocketFd(_client_socket_fd);
            response->SetVirtualServer(&_server->GetConfig().front());
            _request.clear();
            return ServerUtils::SendErrorResponse(status, response);
        }
        std::cerr << "<<< Request uri: " << _request.getUri()
                  << " | Request status after parsing: " << status.message << std::endl;
        Response* response = new Response(_request, _server);
        response->SetClientSocketFd(_client_socket_fd);
        response->SetVirtualServer(ServerUtils::GetRequestVirtualServer(
            _address_socket_fd, response->GetRequest(), _server->GetConfig()
        ));
        _request.clear();
        try
        {
            ProcessRequest(response);
        }
        catch (const std::exception& e)
        {
            ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
        }
    }
}

void ServerClient::ProcessRequest(Response* response)
{
    response->SetRequestFileLocation(
        ServerUtils::GetFileLocation(*response->GetVirtualServer(), response->GetRequest().getUri())
    );
    if (response->GetRequestFileLocation() == response->GetVirtualServer()->locations.end())
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_NOT_FOUND), response);
    HttpStatus check = CheckRequest(response);
    if (check.code == STATUS_HTTP_INTERNAL_IMPLEM_AUTO_INDEX)
        return auto_index(response);
    if (check.code != STATUS_OK)
        return ServerUtils::SendErrorResponse(check, response);
    const std::string& method = response->GetRequest().getMethod();
    // HTTP CGI response:
    if (response->GetRequestFileLocation()->cgi_path != "")
    {
        std::vector<std::string>::const_iterator it = std::find(
            response->GetRequestFileLocation()->cgi_extensions.begin(),
            response->GetRequestFileLocation()->cgi_extensions.end(),
            response->GetRequestFileExtension()
        );
        if (it != response->GetRequestFileLocation()->cgi_extensions.end())
        {
            if (method != "POST" && method != "GET" && method != "HEAD")
                return ServerUtils::SendErrorResponse(
                    HttpStatus(STATUS_METHOD_NOT_ALLOWED), response
                );
            Response* CGIresponse = new ResponseCGI(*response);
            delete response;
            return ProcessCGI(CGIresponse);
        }
    }
    // HTTP simple response:
    if (method == "GET")
    {
        return ProcessGET(response);
    }
    else if (method == "HEAD")
    {
        return ProcessHEAD(response);
    }
    else if (method == "DELETE")
    {
        return ProcessDELETE(response);
    }
    else
    {
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_METHOD_NOT_ALLOWED), response);
    }
}

/*Getters & Setters*/
int ServerClient::GetClientSocketfd() const
{
    return _client_socket_fd;
}
void ServerClient::SetClientSocketfd(int client_socket_fd)
{
    _client_socket_fd = client_socket_fd;
}
int ServerClient::GetAddressSocketfd() const
{
    return _address_socket_fd;
}
void ServerClient::SetAddressSocketfd(int address_socket_fd)
{
    _address_socket_fd = address_socket_fd;
}
bool ServerClient::isStarted() const
{
    return _is_started;
}

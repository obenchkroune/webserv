/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/28 15:19:09 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::vector<ServerConfig>& config)
    : _config(config), _is_started(false)
{
    _listen_socket_ev.events   = EPOLLIN;
    _listen_socket_ev.data.ptr = this;
}
Server::Server(const Server& server) : AIOEventListener(server)
{
    *this = server;
}
Server& Server::operator=(const Server& server)
{
    (void)server;
    return *this;
}
Server::~Server()
{
    Terminate();
}
bool Server::is_started() const
{
    return _is_started;
}
const std::vector<ServerConfig>& Server::GetConfig() const
{
    return _config;
}
const std::vector<int>& Server::GetListenSockets() const
{
    return _listen_socket_fds;
}

void Server::listen_on_addr(const sockaddr_in& _listen_addr)
{
    int _listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int _active_flag      = 1;

    if (_listen_socket_fd == -1)
        throw ServerException("socket(): failed.");
    if (setsockopt(_listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &_active_flag, sizeof(int)) == -1)
        close(_listen_socket_fd), throw ServerException("setsockopt(): failed.");
    if (bind(_listen_socket_fd, (sockaddr*)&_listen_addr, sizeof(_listen_addr)) == -1)
        close(_listen_socket_fd), throw ServerException("bind(): failed.");
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        close(_listen_socket_fd), throw ServerException("listen(): failed.");
    try
    {
        IOMultiplexer::GetInstance().AddEvent(_listen_socket_ev, _listen_socket_fd);
    }
    catch (const std::exception& e)
    {
        close(_listen_socket_fd);
        throw e;
    }
    _listen_socket_fds.insert(_listen_socket_fds.end(), _listen_socket_fd);
}
void Server::Start()
{
    if (_is_started)
        throw ServerException("Server is already started.");
    _is_started                            = true;
    std::vector<ServerConfig>::iterator it = _config.begin();
    it                                     = _config.begin();
    for (; it != _config.end(); it++)
    {
        try
        {
            sockaddr_in address = ServerUtils::GetListenAddr(*it);
            listen_on_addr(address);
            it->listen_address_fd = _listen_socket_fds.back();
            std::cout << "Server started listening on address: " << it->host << ':' << it->port
                      << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Server failed to listen on address: " << it->host << ':' << it->port
                      << '\n';
            std::cerr << "reason: " << e.what() << std::endl;
        }
    }
}
/*
void Server::QueueResponse(Response* response)
{
    _clients_responses[response->GetClientSocketFd()].push(response);
    std::cout << "Response queued for client socket fd: " << response->GetClientSocketFd()
              << std::endl;
}
*/
void Server::QueueCGIResponse(int pipe_fd, Response* response)
{
    _cgi_responses.insert(std::pair<int, Response*>(pipe_fd, response));
    epoll_event peer_ev;
    peer_ev.events   = EPOLLIN;
    peer_ev.data.ptr = this;
    IOMultiplexer::GetInstance().AddEvent(peer_ev, pipe_fd);
    std::cout << "CGI listening on pipe fd: " << pipe_fd
              << " for client fd: " << response->GetClientSocketFd() << std::endl;
}
void Server::ConsumeEvent(const epoll_event ev)
{
    // New peer connected:
    std::vector<int>::iterator listen_socket_it =
        std::find(_listen_socket_fds.begin(), _listen_socket_fds.end(), ev.data.fd);
    if (listen_socket_it != _listen_socket_fds.end())
        return AcceptNewPeerOnSocket(ev.data.fd);
    // CGI pipe I/O ready:
    std::map<int, Response*>::iterator cgi_it = _cgi_responses.find(ev.data.fd);
    assert(cgi_it != _cgi_responses.end());
    if (ev.events & (EPOLLIN | EPOLLHUP))
        HandleCGIEPOLLIN(ev, cgi_it->second);
}
void Server::AcceptNewPeerOnSocket(int socket_fd)
{
    sockaddr_in peer_address;
    socklen_t   peer_address_len = sizeof(peer_address);
    int         peer_fd          = accept(socket_fd, (sockaddr*)&peer_address, &peer_address_len);
    if (peer_fd == -1)
    {
        std::cerr << "accept(): failed to accept new peer connection." << std::endl;
        return;
    }
    std::vector<ServerClient>::iterator client = _clients_pool.begin();
    for (; client != _clients_pool.end() && client->isStarted(); client++)
        ;
    if (client == _clients_pool.end())
        client = _clients_pool.insert(client, ServerClient(peer_fd, socket_fd, this));
    else
        client->SetClientSocketfd(peer_fd), client->SetAddressSocketfd(socket_fd);
    client->BindToClientSocket();
}
void Server::HandleCGIEPOLLIN(const epoll_event& ev, Response* response)
{
    std::map<int, ServerClient>::iterator clients_it = _clients.find(response->GetClientSocketFd());
    if (clients_it == _clients.end())
        throw ServerException("HandleCGIEPOLLIN(): Client not found.");
    std::vector<uint8_t> buff(READ_CHUNK, 0);
    ssize_t              bytes = read(ev.data.fd, buff.data(), READ_CHUNK);
    if (bytes < 0)
        throw ServerException("HandleCGIEPOLLIN(): read() failed.");
    if (bytes == 0)
    {
        epoll_event event = ev;
        event.data.ptr    = this;
        _cgi_responses.erase(ev.data.fd);
        IOMultiplexer::GetInstance().RemoveEvent(event, ev.data.fd);
        close(ev.data.fd);
        if (response->GetContentSize() == 0)
            return ServerUtils::SendErrorResponse(
                HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response
            );
        response->FinishResponse();
        return QueueResponse(response);
    }
    if (bytes < READ_CHUNK)
        buff.erase(buff.begin() + bytes, buff.end());
    response->AppendContent(buff);
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    _is_started = false;
    // closing listen sockets
    {
        std::vector<int>::iterator it = _listen_socket_fds.begin();
        for (; it != _listen_socket_fds.end(); it++)
        {
            IOMultiplexer::GetInstance().RemoveEvent(_listen_socket_ev, *it);
            close(*it);
        }
    }
    // closing clients socket fd
    {
        std::map<int, Response*>::iterator it = _cgi_responses.begin();
        for (; it != _cgi_responses.end(); it++)
        {
            epoll_event ev;
            ev.events   = EPOLLIN;
            ev.data.ptr = this;
            IOMultiplexer::GetInstance().RemoveEvent(ev, it->first);
            close(it->first);
        }
    }
}
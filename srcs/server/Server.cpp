/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/01 23:02:49 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::vector<ServerConfig> &config)
    : _config(config), _is_started(false)
{
    _listen_socket_ev.events   = EPOLLIN;
    _listen_socket_ev.data.ptr = this;
}
Server::Server(const Server &server) : AIOEventListener(server)
{
    *this = server;
}
Server &Server::operator=(const Server &server)
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
const std::vector<ServerConfig> &Server::GetConfig() const
{
    return _config;
}
void Server::listen_on_addr(const sockaddr_in &_listen_addr)
{
    int _listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int _active_flag      = 1;

    if (_listen_socket_fd == -1)
        throw ServerException("socket(): failed.");
    if (setsockopt(_listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &_active_flag, sizeof(int)) == -1)
        close(_listen_socket_fd), throw ServerException("setsockopt(): failed.");
    if (bind(_listen_socket_fd, (sockaddr *)&_listen_addr, sizeof(_listen_addr)) == -1)
        close(_listen_socket_fd), throw ServerException("bind(): failed.");
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        close(_listen_socket_fd), throw ServerException("listen(): failed.");
    try
    {
        IOMultiplexer::GetInstance().AddEvent(_listen_socket_ev, _listen_socket_fd);
    }
    catch (const std::exception &e)
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
    _is_started                                  = true;
    std::vector<ServerConfig>::iterator it = _config.begin();
    it = _config.begin();
    for (; it != _config.end(); it++)
    {
        try
        {
            sockaddr_in address = ServerUtils::GetListenAddr(*it);
            listen_on_addr(address);
            it->listen_address_fd = _listen_socket_fds.back();
            std::cout << "Server started listening on address: " << it->host << ':' << it->port << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "Server failed to listen on address: " << it->host << ':' << it->port << '\n';
            std::cerr << "reason: " << e.what() << std::endl;
        }
    }
}
void Server::ConsumeEvent(const epoll_event ev)
{
    if (std::find(_listen_socket_fds.begin(), _listen_socket_fds.end(), ev.data.fd) != _listen_socket_fds.end())
        AcceptNewPeerOnSocket(ev.data.fd);
    else
    {
        std::map<int, ServerClient>::iterator it = _clients.find(ev.data.fd);
        assert(it != _clients.end());
        if (ev.events & EPOLLIN)
            HandlePeerEPOLLIN(ev, it->second);
        if (ev.events & EPOLLOUT)
            HandlePeerEPOLLOUT(ev, it->second);
    }
}
void Server::AcceptNewPeerOnSocket(int socket_fd)
{
    sockaddr_in peer_address;
    socklen_t   peer_address_len = sizeof(peer_address);
    int         peer_fd          = accept(socket_fd, (sockaddr *)&peer_address, &peer_address_len);
    if (peer_fd == -1)
    {
        std::cerr << "accept(): failed to accept new peer connection." << std::endl;
        return;
    }
    std::map<int, ServerClient>::iterator it = _clients.find(peer_fd);
    assert(it == _clients.end());
    _clients.insert(std::pair<int, ServerClient>(peer_fd, ServerClient(peer_fd, socket_fd, this)));
    epoll_event peer_ev;
    peer_ev.events   = EPOLLIN | EPOLLOUT;
    peer_ev.data.ptr = this;
    try
    {
        IOMultiplexer::GetInstance().AddEvent(peer_ev, peer_fd);
    }
    catch (const std::exception &e)
    {
        close(peer_fd);
        std::cerr << "_IOmltplx->AddEvent() failed to add new peer fd.\n" << e.what() << std::endl;
        return;
    }
    std::cout << "New peer accepted on fd " << peer_fd << ".\n";
}
void Server::HandlePeerEPOLLOUT(const epoll_event &ev, ServerClient &client)
{
    (void)client;
    std::map<int, Responses_queue>::iterator res_it = _responses_.find(ev.data.fd);
    if (res_it == _responses_.end() || res_it->second.size() == 0)
        return;
fetch_next_response:
    Response *response = res_it->second.front();
    if (response->ResponseCount() == 0)
    {
        delete response;
        res_it->second.pop();
        if (res_it->second.size())
            goto fetch_next_response;
        else
            return;
    }
    const uint8_t *buff          = response->GetResponseBuff();
    size_t         bytes_to_send = response->ResponseCount();
    if (bytes_to_send > SEND_CHUNK)
        bytes_to_send = SEND_CHUNK;
    int bytes_sent = send(ev.data.fd, buff, bytes_to_send, MSG_DONTWAIT);
    if (bytes_sent < 0)
    {
        std::cerr << "send() failed. for client fd: " << ev.data.fd << '\n';
        std::cerr << "Client will be kicked out." << std::endl;
        return RemoveClient(ev);
    }
    if (bytes_sent != (int)bytes_to_send)
        std::cerr << "Tried to send " << bytes_to_send << " but send() only sent " << bytes_sent
                  << "\nRemainder data will be sent on next call." << std::endl;
    response->ResponseSent(bytes_sent);
    if (response->ResponseCount() == 0)
    {
        delete response;
        res_it->second.pop();
    }
}
void Server::HandlePeerEPOLLIN(const epoll_event &ev, ServerClient &client)
{
    char    buff[RECV_CHUNK];
    ssize_t bytes = recv(ev.data.fd, buff, RECV_CHUNK - 1, MSG_DONTWAIT);
    if (bytes < 0)
    {
        std::cerr << "recv() failed. for client fd: " << ev.data.fd << '\n';
        std::cerr << "Client will be kicked out." << std::endl;
        return RemoveClient(ev);
    }
    if (bytes == 0)
        return RemoveClient(ev);
    buff[bytes] = 0;
    client.ReceiveRequest(buff);
}
void Server::QueueResponse(int socket_fd, Response *response)
{
    _responses_[socket_fd].push(response);
    std::cout << "Response queued for client socket fd: " << socket_fd << std::endl;
}
void Server::RemoveClient(const epoll_event ev)
{
    _clients.erase(ev.data.fd);
    std::map<int, Responses_queue>::iterator it = _responses_.find(ev.data.fd);
    if (it != _responses_.end())
    {
        for (; it->second.size(); it->second.pop())
            delete it->second.front();
    }
    IOMultiplexer::GetInstance().RemoveEvent(ev, ev.data.fd);
    close(ev.data.fd);
    std::cout << "Client fd: " << ev.data.fd << " disconnected." << std::endl;
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    _is_started = false;
    {
        std::vector<int>::iterator it = _listen_socket_fds.begin();
        for (; it != _listen_socket_fds.end(); it++)
            IOMultiplexer::GetInstance().RemoveEvent(_listen_socket_ev, *it);
        close(*it);
    }
    {
        std::map<int, ServerClient>::iterator it = _clients.begin();
        for (; it != _clients.end(); it = _clients.begin())
        {
            epoll_event ev;
            ev.events  = EPOLLIN | EPOLLOUT;
            ev.data.fd = it->first;
            RemoveClient(ev);
        }
    }
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/05 00:04:10 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

sockaddr_in Server::get_listen_addr(ServerConfig &_config)
{
    sockaddr_in  address;
    std::string &ip = _config.host;
    {
        for (size_t dot, pos = 0, i = 0; i < 4; i++, pos = dot + 1)
        {
            if ((dot = ip.find(".", pos)) == std::string::npos)
            {
                if (i < 3)
                    Terminate(), throw ServerException("Bad ip address string.", *this);
                else
                    dot = ip.length();
            }
            if (dot - pos > 3 || dot - pos <= 0)
                Terminate(), throw ServerException("Bad ip address string.", *this);
            std::string raw_byte = ip.substr(pos, dot);

            uint8_t  byte       = std::atoi(raw_byte.c_str());
            uint8_t *ip_address = (uint8_t *)&address.sin_addr.s_addr;
            ip_address[i]       = byte;
        }
    }
    address.sin_port   = htons(_config.port);
    address.sin_family = AF_INET;
    return address;
}

Server::Server(const ServerConfig &config, IOMultiplexer *IOmltplx, bool start /*= false*/)
    : _config(config), _is_started(false), _IOmltplx(IOmltplx)
{
    if (_IOmltplx == NULL)
        throw ServerException("I/O Multiplexer object is set to NULL\n", *this);
    _listen_addr             = get_listen_addr(_config);
    _listen_socket_ev.events = EPOLLIN;
    if (start)
        Start();
}
Server::Server(const Server &server) : AIOEventListener(server)
{
    *this = server;
}
Server &Server::operator=(const Server &server)
{
    if (this == &server)
        return *this;
    _clients          = server._clients;
    _config           = server._config;
    _is_started       = server._is_started;
    _listen_addr      = server._listen_addr;
    _listen_socket_fd = server._listen_socket_fd;
    _listen_socket_ev = server._listen_socket_ev;
    _IOmltplx         = server._IOmltplx;
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
const ServerConfig &Server::GetConfig() const
{
    return _config;
}
void Server::Start()
{
    if (_is_started)
        throw ServerException("Server is already started.", *this);
    _is_started       = true;
    _listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_socket_fd == -1)
        throw ServerException("socket(): failed.", *this);
    int active = 1;
    if (setsockopt(_listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &active, sizeof(int)) == -1)
        close(_listen_socket_fd), throw ServerException("setsockopt(): failed.", *this);
    if (bind(_listen_socket_fd, (sockaddr *)&_listen_addr, sizeof(_listen_addr)) == -1)
        close(_listen_socket_fd), throw ServerException("bind(): failed.", *this);
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        close(_listen_socket_fd), throw ServerException("listen(): failed.", *this);
    _listen_socket_ev.data.ptr = this;
    _IOmltplx->AddEvent(_listen_socket_ev, _listen_socket_fd);
    uint8_t *ip = (uint8_t *)&_listen_addr.sin_addr.s_addr;
    std::cout << "Server [" << (_config.server_names.size() ? _config.server_names.front() : "NO_NAME")
              << "] started listening on ";
    std::cout << "address: " << +ip[0] << '.' << +ip[1] << '.' << +ip[2] << '.' << +ip[3] << ':'
              << ntohs(_listen_addr.sin_port) << std::endl;
}
void Server::ConsumeEvent(const epoll_event ev)
{
    if (ev.data.fd == _listen_socket_fd)
        acceptNewPeer();
    else
        handlePeerEvent(ev);
}
void Server::acceptNewPeer()
{
    std::cout << "Listening socket event." << std::endl;
    sockaddr_in peer_address;
    socklen_t   peer_address_len = sizeof(peer_address);
    int         peer_fd          = accept(_listen_socket_fd, (sockaddr *)&peer_address, &peer_address_len);
    if (peer_fd == -1)
        Terminate(), throw ServerException("accept(): failed to accept new connection.", *this);
    std::map<int, ServerClient>::iterator it = _clients.find(peer_fd);
    if (it == _clients.end())
        _clients.insert(std::pair<int, ServerClient>(peer_fd, ServerClient(peer_fd, this, _IOmltplx)));
    else
        Terminate(), throw ServerException("fd already exists in clients.", *this);
    epoll_event p_ev;
    p_ev.events   = EPOLLIN | EPOLLOUT;
    p_ev.data.ptr = this;
    _IOmltplx->AddEvent(p_ev, peer_fd);
    uint8_t *peer_ip = (uint8_t *)&peer_address.sin_addr.s_addr;
    std::cout << "new peer accepted on fd " << peer_fd << ".\n";
    std::cout << "Address: " << +peer_ip[0] << '.' << +peer_ip[1] << '.' << +peer_ip[2] << '.' << +peer_ip[3] << ':'
              << ntohs(peer_address.sin_port) << std::endl;
}
void Server::handlePeerEvent(const epoll_event &ev)
{
    std::map<int, ServerClient>::iterator it = _clients.find(ev.data.fd);
    if (it == _clients.end())
        Terminate(), throw ServerException("Client not found.", *this);
    if (ev.events & EPOLLIN)
    {
        char    buff[RECV_CHUNK];
        ssize_t bytes = recv(ev.data.fd, buff, RECV_CHUNK - 1, MSG_DONTWAIT);
        if (bytes < 0)
            Terminate(), throw ServerException("recv() failed.", *this);
        if (bytes == 0)
            return RemoveClient(ev);
        buff[bytes] = 0;
        it->second.ReceiveRequest(buff);
    }
    if (ev.events & EPOLLOUT)
    {
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
            Terminate(), throw ServerException("send() failed.", *this);
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
    _IOmltplx->RemoveEvent(ev, ev.data.fd);
    close(ev.data.fd);
    std::cout << "Client fd: " << ev.data.fd << " disconnected." << std::endl;
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    _is_started = false;
    _IOmltplx->RemoveEvent(_listen_socket_ev, _listen_socket_fd);
    close(_listen_socket_fd);
    std::map<int, ServerClient>::iterator it = _clients.begin();
    for (; it != _clients.end(); it = _clients.begin())
    {
        epoll_event ev;
        ev.events  = EPOLLIN | EPOLLOUT;
        ev.data.fd = it->first;
        RemoveClient(ev);
    }
    std::cout << "Server [" << (_config.server_names.size() ? _config.server_names.front() : "NO_NAME")
              << "] terminated." << std::endl;
}
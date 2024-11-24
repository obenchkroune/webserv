/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/24 02:05:00 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

sockaddr_in Server::get_listen_addr(ServerConfig& _config)
{
    sockaddr_in  address;
    std::string& ip = _config.host;
    {
        for (size_t dot, pos = 0, i = 0; i < 4; i++, pos = dot + 1)
        {
            if ((dot = ip.find(".", pos)) == std::string::npos)
            {
                if (i < 3)
                    throw ServerException("Bad ip address string.", *this);
                else
                    dot = ip.length();
            }
            if (dot - pos > 3 || dot - pos <= 0)
                throw ServerException("Bad ip address string.", *this);
            std::string raw_byte = ip.substr(pos, dot);

            uint8_t  byte       = std::atoi(raw_byte.c_str());
            uint8_t* ip_address = (uint8_t*)&address.sin_addr.s_addr;
            ip_address[i]       = byte;
        }
    }
    address.sin_port   = htons(_config.port);
    address.sin_family = AF_INET;
    return address;
}

Server::Server(const ServerConfig& config, IOMultiplexer* IOmltplx, bool start /*= false*/)
    : _config(config), _is_started(false), _IOmltplx(IOmltplx)
{
    if (_IOmltplx == NULL)
        throw ServerException("I/O Multiplexer object is set to NULL\n", *this);
    _listen_addr               = Server::get_listen_addr(_config);
    _listen_socket_ev.events   = EPOLLIN;
    _listen_socket_ev.data.ptr = this;
    if (start)
        Start();
}
Server::Server(const Server& server) : AIOEventListener(server)
{
    *this = server;
}
Server& Server::operator=(const Server& server)
{
    if (this == &server)
        return *this;
    _clients_fd       = server._clients_fd;
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
const ServerConfig& Server::GetConfig() const
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
        Terminate(), throw ServerException("setsockopt(): failed.", *this);
    if (bind(_listen_socket_fd, (sockaddr*)&_listen_addr, sizeof(_listen_addr)) == -1)
        Terminate(), throw ServerException("bind(): failed.", *this);
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        Terminate(), throw ServerException("listen(): failed.", *this);
    _IOmltplx->AddEvent(_listen_socket_ev, _listen_socket_fd);
    uint8_t* ip = (uint8_t*)&_listen_addr.sin_addr.s_addr;
    std::cout << "Server started on listening succefully.\n";
    std::cout << "Address: " << +ip[0] << '.' << +ip[1] << '.' << +ip[2] << '.' << +ip[3] << ':'
              << ntohs(_listen_addr.sin_port) << '\n';
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    _is_started = false;
    _IOmltplx->RemoveEvent(_listen_socket_ev, _listen_socket_fd);
    if (_listen_socket_fd >= 0)
        close(_listen_socket_fd);
    std::cout << "Server terminated.\n";
}
void Server::ConsumeEvent(const epoll_event ev)
{
    std::cout << "[Server]: New event.\n";
    if (ev.data.fd == _listen_socket_fd)
    {
        std::cout << "Event is for listening socket.\n";
        sockaddr_in peer_address;
        socklen_t   peer_address_len = sizeof(peer_address);
        int peer_fd = accept(_listen_socket_fd, (sockaddr*)&peer_address, &peer_address_len);
        if (peer_fd == -1)
            throw ServerException("accept(): failed to accept new connection.", *this);
        std::vector<int>::iterator it =
            std::lower_bound(_clients_fd.begin(), _clients_fd.end(), peer_fd);
        if (it == _clients_fd.end())
        {
            _clients_fd.push_back(peer_fd);
            it = _clients_fd.end() - 1;
        }
        else
            throw ServerException("fd already exists in clients.", *this);
        epoll_event p_ev;
        p_ev.events   = EPOLLIN;
        p_ev.data.ptr = this;
        _IOmltplx->AddEvent(p_ev, peer_fd);
        uint8_t* peer_ip = (uint8_t*)&peer_address.sin_addr.s_addr;
        std::cout << "[Server] new peer accepted.\n";
        std::cout << "Address: " << +peer_ip[0] << '.' << +peer_ip[1] << '.' << +peer_ip[2] << '.'
                  << +peer_ip[3] << ':' << ntohs(peer_address.sin_port) << '\n';
        return;
    }
    else
    {
        std::vector<int>::iterator it =
            std::lower_bound(_clients_fd.begin(), _clients_fd.end(), ev.data.fd);

        if (it == _clients_fd.end())
            throw ServerException("Client not found.", *this);
        switch (ev.events)
        {
        case EPOLLIN: {
            std::cout << "Event is EPOLLIN on fd: " << ev.data.fd << '\n';
            char    buff[1024];
            ssize_t bytes = recv(ev.data.fd, buff, 1023, 0);
            if (bytes < 0)
                throw ServerException("recv() failed.", *this);
            buff[bytes] = 0;
            std::cout << "Content:\n[START OF CONTENT]\n" << buff << "[END OF CONTENT]\n";
            return;
        }

        default:
            throw ImpossibleToReach();
            break;
        }
    }
    std::cerr << "[Server]: Event fd uknown: " << ev.data.fd << " event ignored.\n";
}

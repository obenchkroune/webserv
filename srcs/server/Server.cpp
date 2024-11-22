/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/23 00:46:25 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static inline sockaddr_in get_listen_addr(const std::vector<Directive>& directives)
{
    std::vector<Directive>::const_iterator it = directives.begin();
    for (; it != directives.end(); it++)
    {
        if (it->name == "listen")
        {
            if (it->values.size() == 0)
                break;
            const std::string& raw = it->values[0];
            if (raw.find(":") == std::string::npos)
                throw ServerException("Bad listen address string.");
            std::string ip   = raw.substr(0, raw.find(":"));
            std::string port = raw.substr(raw.find(":") + 1);
            sockaddr_in address;
            {
                for (size_t dot, pos = 0, i = 0; i < 4; i++, pos = dot + 1)
                {
                    if ((dot = raw.find(".", pos)) == std::string::npos)
                    {
                        if (i < 3)
                            throw ServerException("Bad ip address string.");
                        else
                            dot = ip.length();
                    }
                    if (dot - pos > 3 || dot - pos <= 0)
                        throw ServerException("Bad ip address string.");
                    std::string raw_byte = ip.substr(pos, dot);

                    uint8_t  byte       = std::atoi(raw_byte.c_str());
                    uint8_t* ip_address = (uint8_t*)&address.sin_addr.s_addr;
                    ip_address[i]       = byte;
                }
            }
            address.sin_port   = htons(std::atoi(port.c_str()));
            address.sin_family = AF_INET;
            return address;
        }
    }
    throw ServerException(
        "Listen address not found in the global config directives of the server.");
}

Server::Server(const ServerConfig& config, IOMultiplexer* IOmltplx, bool start /*= false*/)
    : _config(config), _is_started(false), _IOmltplx(IOmltplx)
{
    if (_IOmltplx == NULL)
        throw ServerException("I/O Multiplexer object is set to NULL\n");
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
bool Server::is_started()
{
    return _is_started;
}
void Server::Start()
{
    if (_is_started)
        throw ServerException("Server is already started.");
    _listen_addr      = get_listen_addr(_config.directives);
    _listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_socket_fd == -1)
        throw ServerException("socket(): failed.");
    int active = 1;
    if (setsockopt(_listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &active, sizeof(int)) == -1)
        throw ServerException("setsockopt(): failed.");
    if (bind(_listen_socket_fd, (sockaddr*)&_listen_addr, sizeof(_listen_addr)) == -1)
        throw ServerException("bind(): failed.");
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        throw ServerException("listen(): failed.");
    _listen_socket_ev.events   = EPOLLIN;
    _listen_socket_ev.data.ptr = this;
    _IOmltplx->AddEvent(_listen_socket_ev, _listen_socket_fd);
    _is_started = true;
    uint8_t* ip = (uint8_t*)&_listen_addr.sin_addr.s_addr;
    std::cout << "Server started on listening succefully.\n";
    std::cout << "Address: " << +ip[0] << '.' << +ip[1] << '.' << +ip[2] << '.' << +ip[3] << ':'
              << ntohs(_listen_addr.sin_port) << '\n';
    _is_started = true;
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    _IOmltplx->RemoveEvent(_listen_socket_ev, _listen_socket_fd);
    close(_listen_socket_fd);
    _is_started = false;
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
            throw ServerException("accept(): failed to accept new connection.");
        std::vector<int>::iterator it =
            std::lower_bound(_clients_fd.begin(), _clients_fd.end(), peer_fd);
        if (it == _clients_fd.end())
        {
            _clients_fd.push_back(peer_fd);
            it = _clients_fd.end() - 1;
        }
        else
            throw ServerException("fd already exists in clients.");
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
            throw ServerException("Client not found.");
        switch (ev.events)
        {
        case EPOLLIN: {
            std::cout << "Event is EPOLLIN on fd: " << ev.data.fd << '\n';
            char    buff[1024];
            ssize_t bytes = recv(ev.data.fd, buff, 1023, 0);
            if (bytes < 0)
                throw ServerException("recv() failed.");
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

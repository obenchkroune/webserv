/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 16:22:21 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static inline sockaddr_in get_listen_addr(const std::vector<Directive>& directives)
{
    std::vector<Directive>::const_iterator it = directives.begin();
    for (; it != directives.end(); it++)
    {
        if (it->name == "listen" && it->values.size())
        {
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
                    ip_address[3 - i]   = byte;
                }
            }
            address.sin_port   = std::atoi(port.c_str());
            address.sin_family = AF_INET;
            return address;
        }
    }
    throw ServerException(
        "Listen address not found in the global config directives of the server.");
}

Server::Server(const ServerConfig& config, bool start /*= false*/)
    : _config(config), _is_started(false)
{
    if (start)
        Start();
}
Server::Server(const Server& server)
{
    *this = server;
}
Server& Server::operator=(const Server& server)
{
    if (this == &server)
        return *this;
    _clients          = server._clients;
    _config           = server._config;
    _is_started       = server._is_started;
    _listen_addr      = server._listen_addr;
    _listen_socket_fd = server._listen_socket_fd;
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
#include <cerrno>
void Server::Start()
{
    if (_is_started)
        throw ServerException("Server is already started.");
    _listen_addr      = get_listen_addr(_config.directives);
    _listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_socket_fd == -1)
        throw ServerException("socket() failed.");
    if (bind(_listen_socket_fd, (sockaddr*)&_listen_addr, sizeof(_listen_addr)) == -1)
        throw ServerException("bind() failed.");
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        throw ServerException("listen() failed.");
    _is_started = true;
    uint8_t* ip = (uint8_t*)&_listen_addr.sin_addr.s_addr;
    std::cout << "Server started on listening succefully.\n";
    std::cout << "Address: " << +ip[3] << '.' << +ip[2] << '.' << +ip[1] << '.' << +ip[0] << ':'
              << _listen_addr.sin_port << '\n';
    sockaddr_in peer_address;
    socklen_t   peer_address_len = sizeof(peer_address);
    if (accept(_listen_socket_fd, (sockaddr*)&peer_address, &peer_address_len) == -1)
        throw ServerException("accept() failed.");
    uint8_t* peer_ip = (uint8_t*)&peer_address.sin_addr.s_addr;
    std::cout << "Server accepted a peer succefully.\n";
    std::cout << "Address: " << +peer_ip[3] << '.' << +peer_ip[2] << '.' << +peer_ip[1] << '.'
              << +peer_ip[0] << ':' << peer_address.sin_port << '\n';
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    close(_listen_socket_fd);
    _is_started = false;
    std::cout << "Server terminated.\n";
}
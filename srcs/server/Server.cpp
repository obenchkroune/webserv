/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 12:05:23 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static inline sockaddr get_listen_addr(const std::vector<Directive>& directives)
{
    std::vector<Directive>::const_iterator it = directives.begin();
    for (; it != directives.end(); it++)
    {
        if (it->name == "listen")
        {
            const std::string& raw = it->values[0];
            sockaddr           address;
            address.sa_family = AF_INET | AF_INET6;
            ft_strlcpy(address.sa_data, raw.c_str(), sizeof(address.sa_data));
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
void Server::Start()
{
    if (_is_started)
        throw ServerException("Server is already started.");
    _listen_addr      = get_listen_addr(_config.directives);
    _listen_socket_fd = socket(AF_INET | AF_INET6, SOCK_STREAM, 0);
    if (_listen_socket_fd == -1)
        throw ServerException("socket() failed.");
    if (bind(_listen_socket_fd, &_listen_addr, sizeof(_listen_addr)) == -1)
        throw ServerException("bind() failed on address " + std::string(_listen_addr.sa_data));
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        throw ServerException("listen() failed.");
    _is_started = true;
    std::cout << "Server started on address " << _listen_addr.sa_data << " succefully.\n";
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    close(_listen_socket_fd);
    _is_started = false;
}
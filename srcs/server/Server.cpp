/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/21 22:20:32 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
static inline int get_listen_addr(const ServerConfig& config)
{
    std::vector<Directive>::const_iterator it = config.directives.begin();
    for (; it != config.directives.end(); it++)
    {
        if (it->name == "listen")
        {
            const std::string& raw = it->values[0];
            int                address;
            (void)raw, (void)address;
            /*
            Logic to extract listen ip addr
            From the raw string...
             */
            break;
        }
    }
    throw ServerExceptionListenAddrNotFound();
}

Server::Server(const ServerConfig& config, bool start /*= false*/)
    : _config(config), _is_started(false)
{
    if (start)
        Start();
}
Server::Server(const Server& server)
    : _clients(server._clients), _config(server._config), _is_started(server._is_started)
{
}
Server& Server::operator=(const Server& server)
{
    if (this == &server)
        return *this;
    _clients    = server._clients;
    _config     = server._config;
    _is_started = server._is_started;
    return *this;
}
Server::~Server() {}
bool Server::Start()
{
    if (_is_started)
        throw ServerExceptionAlreadyStarted();
    _listen_socket = socket(AF_INET | AF_INET6 , SOCK_STREAM , 0);
}
bool Server::Terminate() {}
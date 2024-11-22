/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:27:14 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 21:14:27 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifdef MAC_BUILD
#include <bsd/string.h>
#else
#include <cstring>
#endif
#include "ConfigParser.h"
#include "Exceptions.hpp"
#include "IOEventListener.hpp"
#include "IOMultiplexer.hpp"
#include "ServerClient.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Server : public AIOEventListener
{
private:
    std::vector<ServerClient> _clients;
    ServerConfig              _config;
    bool                      _is_started;
    sockaddr_in               _listen_addr;
    int                       _listen_socket_fd;
    epoll_event               _listen_socket_ev;
    IOMultiplexer*            _IOmltplx;

public:
    Server(const ServerConfig& config, IOMultiplexer* IOmltplx, bool start = false);
    Server(const Server& server);
    Server& operator=(const Server& server);
    ~Server();

public:
    void Start();
    void Terminate();
    bool is_started();

public:
    virtual void ConsumeEvent(const epoll_event ev);
};
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:27:14 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 12:01:26 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <bsd/string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "Exceptions.hpp"
#include "Utils.hpp"
#include "ConfigParser.h"
#include "ServerClient.hpp"

class Server
{
private:
    std::vector<ServerClient> _clients;
    ServerConfig        _config;
    bool                _is_started;
    sockaddr            _listen_addr;
    int                 _listen_socket_fd;

public:
    Server(const ServerConfig& config, bool start = false);
    Server(const Server& server);
    Server& operator=(const Server& server);
    ~Server();

public:
    void Start();
    void Terminate();
    bool is_started();
};
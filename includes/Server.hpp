/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:27:14 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/21 21:52:44 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Client.hpp"
#include "ConfigParser.h"
#include "Exceptions.hpp"
#include <sys/socket.h>

class Server
{
private:
    std::vector<Client> _clients;
    ServerConfig        _config;
    bool                _is_started;
    int                 _listen_addr;
    int                 _listen_socket;
public:
    Server(const ServerConfig& config, bool start = false);
    Server(const Server& server);
    Server& operator=(const Server& server);
    ~Server();

public:
    bool Start();
    bool Terminate();
};
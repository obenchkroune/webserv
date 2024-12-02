/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2024/12/02 20:41:31 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Exceptions.hpp"
#include "IOMultiplexer.hpp"
#include "IOEventListener.hpp"
#include "Response.hpp"
#include <iostream>
#include <string>

class ServerClient
{
private:
    int                     _socket_fd;
    std::string             _request_raw;
    Server                 *_server;

public:
    ServerClient(int socket_fd, Server *server, IOMultiplexer* IOmltplx);
    ServerClient(const ServerClient &client);
    ~ServerClient();

private:
    ServerClient &operator=(const ServerClient &client);

public:
    void ReceiveRequest(const std::string request);
    int  Getfd() const;
    void Setfd(const int fd);

private:
    void ProcessRequest(const Request &request);

};
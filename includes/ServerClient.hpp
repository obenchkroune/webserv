/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2024/12/05 02:23:26 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Exceptions.hpp"
#include "IOEventListener.hpp"
#include "Response.hpp"
#include <algorithm>
#include <iostream>
#include <string>

class ServerClient
{
private:
    int         _socket_fd;
    std::string _request_raw;
    Server     *_server;

public:
    ServerClient(int socket_fd, Server *server);
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
    void SendErrorResponse(const HttpStatus &status, Response *response);

private:
    void ProcessGET(const Request &request, Response *response, bool send_data = true);
    void ProcessHEAD(const Request &request, Response *response);
};
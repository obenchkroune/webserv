/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2024/12/08 17:15:57 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Exceptions.hpp"
#include "IOEventListener.hpp"
#include "Response.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

typedef std::vector<LocationConfig>::const_iterator LocationsIterator;

class ServerClient
{
private:
    const int   _socket_fd;
    const int   _address_fd;
    std::string _request_raw;
    Server     *_server;

public:
    ServerClient(const int &socket_fd, const int &address_fd, Server *server);
    ServerClient(const ServerClient &client);
    ~ServerClient();

private:
    ServerClient &operator=(const ServerClient &client);

public:
    void ReceiveRequest(const std::string request);
    int  Getfd() const;

private:
    void ProcessRequest(const Request &request);
    void SendErrorResponse(const HttpStatus &status, Response *response);

private:
    std::pair<HttpStatus, std::string> ProcessFilePermission(const Request &request, LocationsIterator file_location,
                                                             int permission);
    void                               ProcessGET(const Request &request, Response *response, bool send_data = true);
    void                               ProcessHEAD(const Request &request, Response *response);
    void                               ProcessPOST(const Request &request, Response *response);
    void                               ProcessPUT(const Request &request, Response *response);
    void                               ProcessDELETE(const Request &request, Response *response);
};
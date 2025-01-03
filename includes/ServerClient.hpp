/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2025/01/03 21:34:40 by simo             ###   ########.fr       */
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
    Server*     _server;

public:
    ServerClient(const int& socket_fd, const int& address_fd, Server* server);
    ServerClient(const ServerClient& client);
    ~ServerClient();

private:
    ServerClient& operator=(const ServerClient& client);

public:
    void ReceiveRequest(const std::string request);
    int  Getfd() const;

private:
    void ProcessRequest(const Request& request);
    void SendErrorResponse(const HttpStatus& status, Response* response);

private:
    void auto_index(Response* response);

private:
    std::pair<HttpStatus, std::string> CheckRequest(
        const Request& request, const LocationsIterator& file_location
    );
    void ProcessCGI(Response* response);
    void ProcessGET(Response* response, bool send_data = true);
    void ProcessHEAD(Response* response);
    void ProcessPOST(const Request& request, Response* response);
    void ProcessPUT(const Request& request, Response* response);
    void ProcessDELETE(const Request& request, Response* response);
};
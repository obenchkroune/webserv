/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2025/01/09 15:44:22 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Exceptions.hpp"
#include "IOEventListener.hpp"
#include "ResponseCGI.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

typedef std::vector<LocationConfig>::const_iterator LocationsIterator;

class ServerClient
{
private:
    const int   _client_socket_fd;
    const int   _address_socket_fd;
    std::string _request_raw;
    Server*     _server;

public:
    ServerClient(const int& client_socket_fd, const int& address_socket_fd, Server* server);
    ServerClient(const ServerClient& client);
    ~ServerClient();

private:
    ServerClient& operator=(const ServerClient& client);

public:
    void ReceiveRequest(const std::string request);
    int  GetClientSocketfd() const;

private:
    void ProcessRequest(const Request& request);

private:
    void       auto_index(Response* response);
    HttpStatus CheckRequest(Response* response);

private:
    void ProcessCGI(Response* response);
    void ProcessGET(Response* response, bool send_data = true);
    void ProcessHEAD(Response* response);
    void ProcessPOST(Response* response);
    void ProcessPUT(Response* response);
    void ProcessDELETE(Response* response);
};
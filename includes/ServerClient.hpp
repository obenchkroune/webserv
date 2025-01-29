/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2025/01/29 00:32:24 by simo             ###   ########.fr       */
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
#include <queue>

typedef std::vector<LocationConfig>::const_iterator LocationsIterator;

class ServerClient : AIOEventListener
{

private:
    bool                     _is_started;
    int                      _client_socket_fd;
    int                      _address_socket_fd;
    epoll_event              _epoll_ev;
    Server*                  _server;
    Request                  _request;
    std::queue<Response*>    _responses_queue;
    std::map<int, Response*> _cgi_responses;

public:
    ServerClient(const int& client_socket_fd, const int& address_socket_fd, Server* server);
    ServerClient(const ServerClient& client);
    ServerClient& operator=(const ServerClient& client);
    ~ServerClient();

private:

public:
    void BindToClientSocket();
    void ReceiveRequest(const std::vector<uint8_t>& buff);

    /*Getters & Setters*/
public:
    int  GetClientSocketfd() const;
    void SetClientSocketfd(int client_socket_fd);
    int  GetAddressSocketfd() const;
    void SetAddressSocketfd(int address_socket_fd);
    bool isStarted() const;

private:
    void ProcessRequest(Response* response);

private:
    void       auto_index(Response* response);
    HttpStatus CheckRequest(Response* response);
    void       SendErrorResponse(const HttpStatus& status, Response* response);

private:
    void ProcessCGI(Response* response);
    void ProcessGET(Response* response, bool send_data = true);
    void ProcessHEAD(Response* response);
    void ProcessDELETE(Response* response);

public:
    virtual void ConsumeEvent(const epoll_event ev);
    void         Terminate();

private:
    void HandleEPOLLOUT();
    void HandleEPOLLIN();
    void HandleCGIEPOLLIN(const epoll_event& ev, Response* response);
    void QueueCGIResponse(int output_pipe_fd, Response* response);
};
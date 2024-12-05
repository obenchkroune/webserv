/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:27:14 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/05 12:10:14 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifdef MAC_BUILD
#include <bsd/string.h>
#else
#include <cstring>
#endif
#include "Config.hpp"
#include "Exceptions.hpp"
#include "IOEventListener.hpp"
#include "IOMultiplexer.hpp"
#include "Response.hpp"
#include "ServerClient.hpp"
#include "ServerUtils.hpp"
#include <cassert>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <queue>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class Server : public AIOEventListener
{
public:
    typedef std::queue<Response *> Responses_queue;

private:
    std::map<int, ServerClient>     _clients;
    std::map<int, Responses_queue>  _responses_;
    const std::vector<ServerConfig> _config;
    bool                            _is_started;
    std::vector<sockaddr_in>        _listen_addrs;
    std::vector<int>                _listen_socket_fds;
    epoll_event                     _listen_socket_ev;
    IOMultiplexer                  *_IOmltplx;

public:
    Server(const std::vector<ServerConfig> &config, IOMultiplexer *IOmltplx);
    ~Server();

private:
    Server(const Server &server);
    Server &operator=(const Server &server);

public:
    void Start();
    void QueueResponse(int socket_fd, Response *response);

    /* Const */
public:
    bool                             is_started() const;
    const std::vector<ServerConfig> &GetConfig() const;

    /* Interface */
public:
    virtual void ConsumeEvent(const epoll_event ev);
    void         Terminate();

    /* Private Methods */
private:
    void listen_on_addr(const sockaddr_in &_listen_addr);
    void AcceptNewPeerOnSocket(int socket_fd);
    void HandlePeerEPOLLIN(const epoll_event &ev, ServerClient &client);
    void HandlePeerEPOLLOUT(const epoll_event &ev, ServerClient &client);
    void RemoveClient(const epoll_event ev);
};

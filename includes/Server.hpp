/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:27:14 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/25 18:26:22 by msitni           ###   ########.fr       */
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
#include "ServerClient.hpp"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <queue>
#include <sys/socket.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>

class Server : public AIOEventListener
{
private:
    std::map<int, ServerClient>        _clients;
    std::map<int, std::queue<Request> > _requests;
    ServerConfig                       _config;
    bool                               _is_started;
    sockaddr_in                        _listen_addr;
    int                                _listen_socket_fd;
    epoll_event                        _listen_socket_ev;
    IOMultiplexer                     *_IOmltplx;

public:
    Server(const ServerConfig &config, IOMultiplexer *IOmltplx, bool start = false);
    Server(const Server &server);
    Server &operator=(const Server &server);
    ~Server();

public:
    void Start();
    void Terminate();
    void AddRequest(const ServerClient &client, const Request &request);

    /* Const */
public:
    bool                is_started() const;
    const ServerConfig &GetConfig() const;

    /* Interface */
public:
    virtual void ConsumeEvent(const epoll_event ev);

    /* Private Methods */
private:
    sockaddr_in get_listen_addr(ServerConfig &_config);
    void        acceptNewPeer();
    void        handlePeerEvent(const epoll_event &ev);
    void        RemoveClient(epoll_event ev);
};

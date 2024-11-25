/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 17:49:16 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Exceptions.hpp"
#include "IOEventListener.hpp"
#include "Request.hpp"
#include <iostream>
#include <string>

class ServerClient
{
private:
    int         _fd;
    std::string _content;
    Server     *_server;

public:
    ServerClient(int fd, Server *server);
    ServerClient(const ServerClient &client);
    ServerClient &operator=(const ServerClient &client);
    ~ServerClient();

public:
    void PushContent(std::string buff);
    int  Getfd() const;
    void Setfd(const int fd);
};
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2024/11/24 12:49:27 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include "IOEventListener.hpp"

class ServerClient: public AIOEventListener
{
private:
    int _fd;
    std::string content;
public:
    ServerClient(int fd);
    ServerClient(const ServerClient& client);
    ServerClient& operator=(const ServerClient& client);
    ~ServerClient();
public:
    int Getfd() const;
    void Setfd(const int fd);
public:
    virtual void ConsumeEvent(const epoll_event ev);
};
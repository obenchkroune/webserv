/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOMultiplexer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:31:05 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/01 22:37:22 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Exceptions.hpp"
#include "IOEventListener.hpp"
#include <csignal>
#include <iostream>
#include <map>
#include <sys/epoll.h>
#include <unistd.h>

#define EPOLL_EVENTS_MAX 255

class IOMultiplexer
{
private:
    epoll_event                      _events[EPOLL_EVENTS_MAX];
    std::map<int, AIOEventListener*> _listeners;
    int                              _epoll_fd;
    bool                             _is_started;

private:
    IOMultiplexer();
    IOMultiplexer(const IOMultiplexer& IOM);
    IOMultiplexer& operator=(const IOMultiplexer& IOM);
    ~IOMultiplexer();

public:
    static IOMultiplexer& GetInstance();
    void AddEvent(epoll_event ev, int fd);
    void RemoveEvent(epoll_event ev, int fd);
    void StartEventLoop();
    void Terminate();
};
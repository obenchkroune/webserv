/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOMultiplexer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:31:05 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 19:16:56 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Exceptions.hpp"
#include "IOEvent.hpp"
#include <sys/epoll.h>
#include <unistd.h>

#define EPOLL_EVENTS_MAX 255

class IOMultiplexer
{
private:
    epoll_event _events[EPOLL_EVENTS_MAX];
    int         _epoll_fd;
    bool        _is_started;

public:
    IOMultiplexer();
    IOMultiplexer(const IOMultiplexer& IOM);
    IOMultiplexer& operator=(const IOMultiplexer& IOM);
    ~IOMultiplexer();

public:
    void AddEvent(epoll_event ev, int fd);
    void RemoveEvent(epoll_event ev, int fd);
    void StartEventLoop();
    void Terminate();
};
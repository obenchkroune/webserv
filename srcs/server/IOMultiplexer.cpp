/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOMultiplexer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:30:28 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 19:29:14 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IOMultiplexer.hpp"

IOMultiplexer::IOMultiplexer() : _is_started(false)
{
    _epoll_fd = epoll_create(__INT32_MAX__);
    if (_epoll_fd == -1)
        throw IOMultiplexerException("epoll(): failed.");
}
IOMultiplexer::IOMultiplexer(const IOMultiplexer& IOM)
{
    *this = IOM;
}
IOMultiplexer& IOMultiplexer::operator=(const IOMultiplexer& IOM)
{
    if (this == &IOM)
        return *this;
    for (size_t i = 0; i < sizeof(_events) / sizeof(epoll_event); i++)
        _events[i] = IOM._events[i];
    _epoll_fd   = IOM._epoll_fd;
    _is_started = IOM._is_started;
    return *this;
}
IOMultiplexer::~IOMultiplexer()
{
    Terminate();
}
#include <iostream>
void IOMultiplexer::AddEvent(epoll_event ev, int fd)
{
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw IOMultiplexerException("epoll_ctl() failed.");
}
void IOMultiplexer::RemoveEvent(epoll_event ev, int fd)
{
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1)
        throw IOMultiplexerException("epoll_ctl() failed.");
}
void IOMultiplexer::StartEventLoop()
{
    if (_is_started)
        throw IOMultiplexerException("Events loop already started.");
    _is_started = true;
    while (_is_started)
    {
        int events_count = epoll_wait(_epoll_fd, _events, EPOLL_EVENTS_MAX, -1);
        if (events_count == -1)
            throw IOMultiplexerException("epoll_wait(): failed.");
        std::cout << "Received " << events_count << " events.\n";
        for (int i = 0; i < events_count; i++)
            ((AIOEvent*)_events[i].data.ptr)->ConsumeEvent(_events[i]);
    }
}
void IOMultiplexer::Terminate()
{
    if (_is_started == false)
        return;
    close(_epoll_fd);
    _is_started = false;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOMultiplexer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:30:28 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/05 11:57:02 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IOMultiplexer.hpp"

int webserv_unix_signal = 0; // Global variable signal

IOMultiplexer::IOMultiplexer() : _is_started(false)
{
    _epoll_fd = epoll_create(__INT32_MAX__);
    if (_epoll_fd == -1)
        throw IOMultiplexerException("epoll(): failed.");
}
IOMultiplexer::IOMultiplexer(const IOMultiplexer &IOM)
{
    *this = IOM;
}
IOMultiplexer &IOMultiplexer::operator=(const IOMultiplexer &IOM)
{
    if (this == &IOM)
        return *this;
    for (size_t i = 0; i < sizeof(_events) / sizeof(epoll_event); i++)
        _events[i] = IOM._events[i];
    _listeners  = IOM._listeners;
    _epoll_fd   = IOM._epoll_fd;
    _is_started = IOM._is_started;
    return *this;
}
IOMultiplexer::~IOMultiplexer()
{
}
void IOMultiplexer::AddEvent(epoll_event ev, int fd)
{
    std::map<int, AIOEventListener *>::iterator it = _listeners.find(fd);
    if (it != _listeners.end())
        throw IOMultiplexerException("AddEvent() : Event listener already added.");
    AIOEventListener *listener = (AIOEventListener *)ev.data.ptr;
    _listeners.insert(std::pair<int, AIOEventListener *>(fd, listener));
    ev.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw IOMultiplexerException("epoll_ctl() failed.");
}
void IOMultiplexer::RemoveEvent(epoll_event ev, int fd)
{
    std::map<int, AIOEventListener *>::iterator it = _listeners.find(fd);
    if (it == _listeners.end())
        throw IOMultiplexerException("RemoveEvent() : Event listener not found.");
    ev.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1)
        throw IOMultiplexerException("epoll_ctl() failed.");
    _listeners.erase(fd);
}
void IOMultiplexer::StartEventLoop()
{
    if (_is_started)
        throw IOMultiplexerException("Events loop already started.");
    if (!_listeners.size())
        return;
    _is_started = true;
    std::cout << "IOMultiplexer loop started." << std::endl;
    while (_is_started)
    {
        int events_count = epoll_wait(_epoll_fd, _events, EPOLL_EVENTS_MAX, -1);
        if (events_count == -1)
        {
            if (webserv_unix_signal == SIGINT)
                break;
            Terminate();
            throw IOMultiplexerException("epoll_wait(): failed.");
        }
        for (int i = 0; i < events_count; i++)
        {
            std::map<int, AIOEventListener *>::iterator it = _listeners.find(_events[i].data.fd);
            if (it == _listeners.end())
                throw IOMultiplexerException("fd not found in [std::map<int, AIOEventListener*> _listeners] member.");
            it->second->ConsumeEvent(_events[i]);
        }
        if (webserv_unix_signal == SIGINT)
            break;
    }
    Terminate();
}
void IOMultiplexer::Terminate()
{
    if (_is_started == false)
        return;
    _is_started                                    = false;
    std::map<int, AIOEventListener *>::iterator it = _listeners.begin();
    for (; it != _listeners.end(); it = _listeners.begin())
        it->second->Terminate();
    close(_epoll_fd);
}
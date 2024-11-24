/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOMultiplexer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:30:28 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/24 11:16:43 by msitni           ###   ########.fr       */
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
    _listeners_in  = IOM._listeners_in;
    _listeners_out = IOM._listeners_out;
    _epoll_fd      = IOM._epoll_fd;
    _is_started    = IOM._is_started;
    return *this;
}
IOMultiplexer::~IOMultiplexer()
{
    Terminate();
}
void IOMultiplexer::AddEvent(epoll_event ev, int fd)
{
    std::map<int, AIOEventListener*>*          listner_map;
    std::map<int, AIOEventListener*>::iterator it;
    switch (ev.events)
    {
    case EPOLLIN:
        it          = _listeners_in.find(fd);
        listner_map = &_listeners_in;
        break;
    case EPOLLOUT:
        it          = _listeners_out.find(fd);
        listner_map = &_listeners_out;
        break;

    default:
        throw ImpossibleToReach();
        break;
    }
    if (it != listner_map->end())
        throw IOMultiplexerException("AddEvent() : Event listener already added.");
    AIOEventListener* listener = (AIOEventListener*)ev.data.ptr;
    listner_map->insert(std::pair<int, AIOEventListener*>(fd, listener));
    ev.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw IOMultiplexerException("epoll_ctl() failed.");
}
void IOMultiplexer::RemoveEvent(epoll_event ev, int fd)
{
    std::map<int, AIOEventListener*>*          listner_map;
    std::map<int, AIOEventListener*>::iterator it;
    switch (ev.events)
    {
    case EPOLLIN:
        it          = _listeners_in.find(fd);
        listner_map = &_listeners_in;
        break;
    case EPOLLOUT:
        it          = _listeners_out.find(fd);
        listner_map = &_listeners_out;
        break;

    default:
        throw ImpossibleToReach();
        break;
    }
    if (it == listner_map->end())
        return;
    ev.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1)
        throw IOMultiplexerException("epoll_ctl() failed.");
    listner_map->erase(fd);
}
void IOMultiplexer::StartEventLoop()
{
    if (_is_started)
        throw IOMultiplexerException("Events loop already started.");
    if (!_listeners_in.size() && !_listeners_out.size())
        return;
    _is_started = true;
    while (_is_started)
    {
        int events_count = epoll_wait(_epoll_fd, _events, EPOLL_EVENTS_MAX, -1);
        if (events_count == -1)
            throw IOMultiplexerException("epoll_wait(): failed.");
        for (int i = 0; i < events_count; i++)
        {
            std::map<int, AIOEventListener*>::iterator it;
            switch (_events[i].events)
            {
            case EPOLLIN:
                it = _listeners_in.find(_events[i].data.fd);
                if (it == _listeners_in.end())
                    throw IOMultiplexerException(
                        "fd not found in [std::map<int, AIOEventListener*> _listeners_in] member.");
                break;

            case EPOLLOUT:
                it = _listeners_out.find(_events[i].data.fd);
                if (it == _listeners_out.end())
                    throw IOMultiplexerException("fd not found in [std::map<int, "
                                                 "AIOEventListener*> _listeners_out] member.");
                break;

            default:
                throw ImpossibleToReach();
                break;
            }
            it->second->ConsumeEvent(_events[i]);
        }
    }
}
void IOMultiplexer::Terminate()
{
    if (_is_started == false)
        return;
    _is_started = false;
    if (_epoll_fd >= 0)
        close(_epoll_fd);
}
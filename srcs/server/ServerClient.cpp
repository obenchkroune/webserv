/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2024/11/24 13:33:24 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"

ServerClient::ServerClient(const int fd)
  : _fd(fd)
{
}
ServerClient::ServerClient(const ServerClient& client)
{
    *this = client;
}
ServerClient&
ServerClient::operator=(const ServerClient& client)
{
    if (this == &client)
        return *this;
    _fd = client._fd;
    return *this;
}
ServerClient::~ServerClient() {}
int
ServerClient::Getfd() const
{
    return _fd;
}
void
ServerClient::Setfd(const int fd)
{
    _fd = fd;
}
void
ServerClient::ConsumeEvent(const epoll_event ev)
{
    (void)ev;
}

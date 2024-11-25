/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 15:00:52 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "IOEventListener.hpp"
#include "Request.hpp"
#include <iostream>
#include <string>

class ServerClient
{
private:
    int         _fd;
    std::string _content;

public:
    ServerClient(int fd);
    ServerClient(const ServerClient &client);
    ServerClient &operator=(const ServerClient &client);
    ~ServerClient();

public:
    void PushContent(std::string buff);
    int  Getfd() const;
    void Setfd(const int fd);
};
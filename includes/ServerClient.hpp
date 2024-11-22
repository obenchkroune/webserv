/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:54:28 by msitni            #+#    #+#             */
/*   Updated: 2024/11/22 23:45:37 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class ServerClient
{
private:
    int _fd;
public:
    ServerClient(int fd);
    ServerClient(const ServerClient& client);
    ServerClient& operator=(const ServerClient& client);
    ~ServerClient();
public:
    int Getfd() const;
    void Setfd(const int fd);
};
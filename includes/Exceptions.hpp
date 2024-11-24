/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:42:12 by msitni            #+#    #+#             */
/*   Updated: 2024/11/24 01:04:11 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <exception>
#include <string>

class NotImplemented : public std::exception // is it that hard to just assert just that string heh?
                                             // instead of A whole created class
{
public:
    const char* what() const throw();
};

class ImpossibleToReach : public std::exception // is it that hard to just assert just that string
                                                // heh? instead of A whole created class
{
public:
    const char* what() const throw();
};

class Server;

class ServerException : public std::exception
{
private:
    std::string   _reason;
    const Server& _server;

public:
    ServerException(std::string reason, const Server& server) throw();
    const char* what() const throw();
    ~ServerException() throw();
};

class IOMultiplexerException : public std::exception
{
private:
    std::string _reason;

public:
    IOMultiplexerException(std::string reason) throw();
    const char* what() const throw();
    ~IOMultiplexerException() throw();
};

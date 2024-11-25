/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:42:12 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 17:12:47 by msitni           ###   ########.fr       */
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


class ServerClientException : public std::exception
{
private:
    std::string _reason;

public:
    ServerClientException(std::string reason) throw();
    const char* what() const throw();
    ~ServerClientException() throw();
};
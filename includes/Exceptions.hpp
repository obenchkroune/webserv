/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:42:12 by msitni            #+#    #+#             */
/*   Updated: 2024/12/05 02:33:54 by msitni1337       ###   ########.fr       */
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

public:
    ServerException(std::string reason) throw();
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/08 17:31:52 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include <ctime>
#include <exception>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

class ResponseException
{
private:
    std::string _reason;

public:
    ResponseException(std::string reason) throw();
    const char *what() const throw();
    ~ResponseException() throw();
};

class Response
{
private:
    std::string          _headers;
    std::vector<uint8_t> _content;
    size_t               _content_sent;
    const Request        _request;
    const ServerConfig  &_virtual_server;

public:
    Response(const Request &request, const ServerConfig &virtual_server);
    ~Response();

private:
    Response(const Response &response);
    Response &operator=(const Response &response);

public:
    const ServerConfig &GetVirtualServer() const;
    const uint8_t      *GetResponseBuff() const;
    void                ResponseSent(const size_t n);
    size_t              ResponseCount() const;

public:
    void SetStatusHeaders(const char *status_string);
    void AppendHeader(const HttpHeader &header);
    void ReadFile(const int fd);
    void FinishResponse(bool append_content_length);
};
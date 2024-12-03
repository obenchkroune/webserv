/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/03 19:35:07 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include <ctime>
#include <exception>
#include <ranges>
#include <sstream>
#include <string>
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

public:
    Response(const Request &request);
    ~Response();

private:
    Response(const Response &response);
    Response &operator=(const Response &response);

public:
    const uint8_t *GetResponseBuff() const;
    void           ResponseSent(const size_t n);
    size_t         ResponseCount() const;

public:
    void SetStatusHeaders(const char *status_string);
    void AppendHeader(const HttpHeader &header);
    void ReadFile(const int fd);
    void FinishResponse();
};
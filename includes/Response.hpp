/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/02 23:49:01 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include <exception>
#include <string>
#include <ctime>
#include <ranges>
#include <sstream>
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
    std::string    _response_string;
    std::string    _content_body;
    const Request  _request;

public:
    Response(const Request &request);
    Response(const Response &response);
    ~Response();

private:
    Response &operator=(const Response &response);

public:
    const std::string &GetResponseString() const;

public:
    void SetStatusHeaders(const char *status_string);
    void AppendHeader(const HttpHeader &header);
    void ReadFile(const int fd);
    void EndResponse();
};

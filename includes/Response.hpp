/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/02 22:25:16 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include <exception>
#include <string>
#include <ctime>
#include <ranges>

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
    void AppendContent(const char *buff);
    void EndResponse();
};

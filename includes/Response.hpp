/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/02 15:11:17 by msitni           ###   ########.fr       */
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

struct ResponseHeader
{
    std::string name;
    std::string value;

    ResponseHeader() {};
    ResponseHeader(const std::string& _name, const std::string& _value)
        : name(_name), value(_value) {};
};

class ResponseException
{
private:
    std::string _reason;

public:
    ResponseException(std::string reason) throw();
    const char* what() const throw();
    ~ResponseException() throw();
};

typedef std::vector<LocationConfig>::const_iterator LocationIterator;

class Response
{
private:
    int                  _client_socket_fd;
    std::string          _headers;
    std::vector<uint8_t> _content;
    size_t               _content_sent;
    const Request        _request;
    std::string          _file_name;
    std::string          _file_extension;
    LocationIterator     _file_location;
    struct stat          _file_stats;
    const ServerConfig&  _virtual_server;

public:
    Response(const Request& request, const ServerConfig& virtual_server);
    ~Response();

private:
    Response(const Response& response);
    Response& operator=(const Response& response);
    /**
     * getters & setters
     */
public:
    const int&              GetClientSocketFd() const;
    void                    SetClientSocketFd(const int& fd);
    const Request&          GetRequest() const;
    const std::string&      GetFileName() const;
    void                    SetFileName(const std::string& fname);
    const std::string&      GetFileExtension() const;
    void                    SetFileExtension(const std::string& ext);
    const LocationIterator& GetFileLocation() const;
    void                    SetFileLocation(const LocationIterator& location);
    struct stat&            GetFileStat();

public:
    const ServerConfig& GetVirtualServer() const;
    const uint8_t*      GetResponseBuff() const;
    void                ResponseSent(const size_t n);
    size_t              ResponseCount() const;

public:
    void SetStatusHeaders(const char* status_string);
    void AppendHeader(const ResponseHeader& header);
    void ReadFile(const int fd);
    void FinishResponse(bool append_content_length);
    void AppendContent(const std::vector<uint8_t>& content);
};

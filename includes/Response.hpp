/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/08 16:53:34 by simo             ###   ########.fr       */
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

class Server;

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
protected:
    int                  _client_socket_fd;
    std::string          _headers;
    std::vector<uint8_t> _content;
    size_t               _content_sent;
    std::string          _file_name;
    std::string          _file_extension;
    LocationIterator     _file_location;
    struct stat          _file_stats;
    const Request        _request;
    const ServerConfig&  _virtual_server;
    Server*              _server;

public:
    Response(const Request& request, const ServerConfig& virtual_server, Server* server);
    virtual ~Response();
    Response(const Response& response);

private:
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
    Server*                 GetServer() const;
    size_t            GetContentSize() const;

public:
    const ServerConfig& GetVirtualServer() const;
    const uint8_t*      GetResponseBuff() const;

public:
    void   ResponseSent(const size_t n);
    size_t ResponseCount() const;

public:
    void         SetStatusHeaders(const char* status_string);
    void         AppendHeader(const ResponseHeader& header);
    void         ReadFile(const int fd);
    virtual void FinishResponse(bool append_content_length = true);
    void         AppendContent(const std::vector<uint8_t>& content);
};

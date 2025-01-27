/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/28 00:43:09 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Request.hpp"
#include <ctime>
#include <exception>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdio.h>
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
    size_t               _bytes_sent_to_client;
    size_t               _content_lenght;
    std::string          _request_file_path;
    std::string          _request_file_extension;
    LocationIterator     _request_file_location;
    int                  _request_file_fd;
    struct stat          _request_file_stats;
    const Request        _request;
    std::vector<uint8_t> _response_buff;
    const ServerConfig*  _virtual_server;
    Server*              _server; // TODO: check if we can remove this?

public:
    Response(Server* server); // Tmp error response
    Response(const Request& request, Server* server);
    virtual ~Response();
    Response(const Response& response);

private:
    Response& operator=(const Response& response);
    /**
     * getters & setters
     */
public:
    int                     GetClientSocketFd() const;
    void                    SetClientSocketFd(const int& fd);
    const Request&          GetRequest() const;
    void                    SetRequest(const Request&);
    const std::string&      GetRequestFilePath() const;
    void                    SetRequestFilePath(const std::string& path);
    const std::string&      GetRequestFileExtension() const;
    void                    SetRequestFileExtension(const std::string& ext);
    const LocationIterator& GetRequestFileLocation() const;
    void                    SetRequestFileLocation(const LocationIterator& location);
    int                     GetRequestFileFd() const;
    void                    SetRequestFileFd(const int& file_fd);
    struct stat             GetRequestFileStat() const;
    void                    SetRequestFileStat(struct stat& stat);
    Server*                 GetServer() const;
    size_t                  GetContentLength() const;
    const ServerConfig*     GetVirtualServer() const;
    void                    SetVirtualServer(const ServerConfig* virtual_server);
    const uint8_t*          GetResponseBuff() const;
    size_t                  GetResponseBuffCount() const;

public:
    void IncrementResponseBuffBytesSent(const size_t n);

public:
    void         SetStatusHeaders(const char* status_string);
    void         AppendHeader(const ResponseHeader& header);
    void         AppendToResponseBuff(const std::vector<uint8_t>& content);
    virtual void FinishResponse();
};

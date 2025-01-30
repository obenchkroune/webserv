/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 21:56:34 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/29 16:47:17 by msitni           ###   ########.fr       */
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


class Response
{
protected:
    //int                  _client_socket_fd;
    std::string          _headers;
    size_t               _bytes_sent_to_client;
    size_t               _content_lenght;
    std::string          _request_file_path;
    std::string          _request_file_extension;
    int                  _request_file_fd;
    struct stat          _request_file_stats;
    const Request        _request;
    std::vector<uint8_t> _response_buff;

public:
    Response(); // Error response constructor
    Response(const Request& request);
    virtual ~Response();
    Response(const Response& response);

private:
    Response& operator=(const Response& response);
    /**
     * getters & setters
     */
public:
    //int                     GetClientSocketFd() const;
    //void                    SetClientSocketFd(const int& fd);
    const Request&          GetRequest() const;
    const std::string&      GetRequestFilePath() const;
    void                    SetRequestFilePath(const std::string& path);
    const std::string&      GetRequestFileExtension() const;
    void                    SetRequestFileExtension(const std::string& ext);
    int                     GetRequestFileFd() const;
    void                    SetRequestFileFd(const int& file_fd);
    struct stat             GetRequestFileStat() const;
    void                    SetRequestFileStat(struct stat& stat);
    size_t                  GetContentLength() const;
    const uint8_t*          GetResponseBuff();
    size_t                  GetResponseBuffCount() const;

public:
    void IncrementResponseBuffBytesSent(const size_t n);

public:
    void         SetStatusHeaders(const char* status_string);
    void         AppendHeader(const ResponseHeader& header);
    virtual void AppendToResponseBuff(const std::vector<uint8_t>& content);
    virtual void         FinishResponse();
};

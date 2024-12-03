/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 22:17:23 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/03 00:05:01 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const Request &request) : _request(request)
{
}
Response::Response(const Response &response) : _request(response._request)
{
    *this = response;
}
Response &Response::operator=(const Response &response)
{
    if (this == &response)
        return *this;
    _response_string = response._response_string;
    return *this;
}
Response::~Response()
{
}
const std::string &Response::GetResponseString() const
{
    return _response_string;
}
void Response::SetStatusHeaders(const char *status_string)
{
    _response_string = HTTP_VERSION_TOKEN " ";
    _response_string += status_string;
    _response_string += CRLF;
    _response_string += "Server: " PROGNAME "/" PROGVERSION CRLF;

    time_t t_now   = time(0);
    tm    *now     = gmtime(&t_now);
    size_t buff_sz = sizeof("aaa, dd bbb YYYY HH:MM:SS GMT");
    char   time_buff[buff_sz];
    size_t bytes = std::strftime(time_buff, buff_sz, "%a, %d %b %Y %H:%M:%S GMT", now);
    if (bytes == 0)
        throw ResponseException("strftime() failed.");
    time_buff[bytes] = 0;
    _response_string += "Date: ";
    _response_string += time_buff;
    _response_string += CRLF;
}
void Response::AppendHeader(const HttpHeader &header)
{
    _response_string += header.name + ": " + header.value + CRLF;
}
void Response::ReadFile(const int fd)
{
    for (;;)
    {
        char buff[READ_CHUNK];
        int  bytes = read(fd, buff, READ_CHUNK - 1);
        if (bytes < 0)
            throw ResponseException("read() failed for given fd.");
        buff[bytes] = 0;
        if (bytes == 0)
            break;
        _content_body += buff;
    }
    close(fd);
}
void Response::EndResponse()
{
    std::ostringstream content_length;
    content_length << _content_body.length() + 4;
    HttpHeader header;
    header.name = "Content-Length";
    header.value += content_length.str();
    AppendHeader(header);
    header.name  = "Connection";
    header.value = "keep-alive";
    AppendHeader(header);
    _response_string += CRLF;
    _response_string += _content_body;
    _response_string += CRLF CRLF;
    _content_body.clear();
}
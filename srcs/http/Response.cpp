/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 22:17:23 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/04 13:56:37 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const Request &request) : _content_sent(0), _request(request)
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
    _headers      = response._headers;
    _content_sent = response._content_sent;
    _content      = response._content;
    return *this;
}
Response::~Response()
{
}
const uint8_t *Response::GetResponseBuff() const
{
    return (uint8_t *)&_content[_content_sent];
}
void Response::ResponseSent(const size_t n)
{
    _content_sent += n;
}
size_t Response::ResponseCount() const
{
    if (_content_sent >= _content.size())
        return 0;
    return _content.size() - _content_sent;
}
void Response::SetStatusHeaders(const char *status_string)
{
    _headers = HTTP_VERSION_TOKEN " ";
    _headers += status_string;
    _headers += CRLF;
    _headers += "Server: " PROGNAME "/" PROGVERSION CRLF;

    time_t t_now   = time(0);
    tm    *now     = gmtime(&t_now);
    size_t buff_sz = sizeof("aaa, dd bbb YYYY HH:MM:SS GMT");
    char   time_buff[buff_sz];
    size_t bytes = std::strftime(time_buff, buff_sz, "%a, %d %b %Y %H:%M:%S GMT", now);
    if (bytes == 0)
        throw ResponseException("strftime() failed.");
    time_buff[bytes] = 0;
    _headers += "Date: ";
    _headers += time_buff;
    _headers += CRLF;
}
void Response::AppendHeader(const HttpHeader &header)
{
    _headers += header.name + ": " + header.value + CRLF;
}
void Response::ReadFile(const int fd)
{
    struct stat st;
    fstat(fd, &st);
    std::ostringstream content_length;
    content_length << st.st_size;
    HttpHeader header;
    header.name  = "Content-Length";
    header.value = content_length.str();
    AppendHeader(header);

    for (;;)
    {
        uint8_t buff[READ_CHUNK];
        int     bytes = read(fd, buff, READ_CHUNK);
        if (bytes < 0)
            throw ResponseException("read() failed for given fd.");
        if (bytes == 0)
            break;
        _content.insert(_content.end(), buff, (buff + bytes));
    }
    close(fd);
}
void Response::FinishResponse()
{
    HttpHeader header;
    header.name  = "Connection";
    header.value = "keep-alive";
    AppendHeader(header);
    _headers += CRLF;
    _content.insert(_content.begin(), _headers.begin(), _headers.end());
    std::cout << "[Response headers]     ============" << std::endl;
    std::cout << _headers << std::endl;
    std::cout << "[End Response headers] ============" << std::endl;
    _headers.erase();
}
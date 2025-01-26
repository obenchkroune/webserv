/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 22:17:23 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/26 20:12:55 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Server* server) : _content_sent(0), _server(server) {} // Temporary Response

Response::Response(const Request& request, Server* server)
    : _content_sent(0), _request(request), _server(server)
{
}
Response::Response(const Response& response)
    : _request(response._request), _virtual_server(response._virtual_server),
      _server(response._server)
{
    *this = response;
}
Response& Response::operator=(const Response& response)
{
    if (this == &response)
        return *this;
    _client_socket_fd = response._client_socket_fd;
    _headers          = response._headers;
    _content          = response._content;
    _content_sent     = response._content_sent;
    _file_path        = response._file_path;
    _file_extension   = response._file_extension;
    _file_location    = response._file_location;
    _file_stats       = response._file_stats;
    return *this;
}
Response::~Response() {}
/* getters & setters*/
const int& Response::GetClientSocketFd() const
{
    return _client_socket_fd;
}
void Response::SetClientSocketFd(const int& fd)
{
    _client_socket_fd = fd;
}
const Request& Response::GetRequest() const
{
    return _request;
}
const std::string& Response::GetFilePath() const
{
    return _file_path;
}
void Response::SetFilePath(const std::string& path)
{
    _file_path = path;
}
const std::string& Response::GetFileExtension() const
{
    return _file_extension;
}
void Response::SetFileExtension(const std::string& ext)
{
    _file_extension = ext;
}
const LocationIterator& Response::GetFileLocation() const
{
    return _file_location;
}
void Response::SetFileLocation(const LocationIterator& location)
{
    _file_location = location;
}
struct stat& Response::GetFileStat()
{
    return _file_stats;
}
const ServerConfig* Response::GetVirtualServer() const
{
    return _virtual_server;
}
void Response::SetVirtualServer(const ServerConfig* virtual_server)
{
    _virtual_server = virtual_server;
}
Server* Response::GetServer() const
{
    return _server;
}
size_t Response::GetContentSize() const
{
    return _content.size();
}
const uint8_t* Response::GetResponseBuff() const
{
    return (uint8_t*)&_content[_content_sent];
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
void Response::SetStatusHeaders(const char* status_string)
{
    _headers = HTTP_VERSION_TOKEN " ";
    _headers += status_string;
    _headers += CRLF;
    _headers += "Server: " PROGNAME "/" PROGVERSION CRLF;

    time_t       t_now   = time(0);
    tm*          now     = gmtime(&t_now);
    const size_t buff_sz = sizeof("aaa, dd bbb YYYY HH:MM:SS GMT");
    char         time_buff[buff_sz];
    size_t       bytes = std::strftime(time_buff, buff_sz, "%a, %d %b %Y %H:%M:%S GMT", now);
    if (bytes == 0)
        throw ResponseException("strftime() failed.");
    time_buff[bytes] = 0;
    _headers += "Date: ";
    _headers += time_buff;
    _headers += CRLF;
}
void Response::AppendHeader(const ResponseHeader& header)
{
    _headers += header.name + ": " + header.value + CRLF;
}
void Response::AppendContent(const std::vector<uint8_t>& content)
{
    _content.insert(_content.end(), content.begin(), content.end());
}
void Response::ReadFile(const int fd)
{
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
void Response::FinishResponse(bool append_content_length /* = true*/)
{
    ResponseHeader header;
    if (append_content_length)
    {
        std::ostringstream content_length;
        content_length << _content.size();
        header.name  = "Content-Length";
        header.value = content_length.str();
        AppendHeader(header);
    }
    header.name  = "Connection";
    header.value = "keep-alive";
    AppendHeader(header);
    _headers += CRLF;
    _content.insert(_content.begin(), _headers.begin(), _headers.end());
    std::cerr << "[Response headers]     ============" << std::endl;
    std::cerr << _headers << std::endl;
    std::cerr << "[End Response headers] ============" << std::endl;
    std::cerr << "[Response body]     ============" << std::endl;
    int snippet_size = std::min(100, (int)(_content.size() - _headers.size()));
    dprintf(
        2, "First %d bytes from body:\n%.*s\n", snippet_size, snippet_size,
        (char*)_content.data() + _headers.size()
    );
    std::cerr << "[End Response body] ============" << std::endl;
    _headers.erase();
}
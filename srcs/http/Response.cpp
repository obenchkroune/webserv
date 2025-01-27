/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 22:17:23 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/27 20:45:20 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <cassert>

Response::Response(Server* server)
    : _content_lenght(0), _bytes_sent_to_client(0), _request_file_fd(-1), _server(server)
{
} // Temporary Response

Response::Response(const Request& request, Server* server)
    : _content_lenght(0), _bytes_sent_to_client(0), _request_file_fd(-1), _request(request),
      _server(server)
{
}
Response::Response(const Response& response)
    : _request(response._request)
{
    *this = response;
}
Response& Response::operator=(const Response& response)
{
    if (this == &response)
        return *this;
    _client_socket_fd       = response._client_socket_fd;
    _headers                = response._headers;
    _bytes_sent_to_client   = response._bytes_sent_to_client;
    _content_lenght         = response._content_lenght;
    _request_file_path      = response._request_file_path;
    _request_file_extension = response._request_file_extension;
    _request_file_location  = response._request_file_location;
    _request_file_fd        = response._request_file_fd;
    _request_file_stats     = response._request_file_stats;
    _response_buff          = response._response_buff;
    _virtual_server         = response._virtual_server;
    _server                 = response._server;
    return *this;
}
Response::~Response() {}
/* getters & setters*/
int Response::GetClientSocketFd() const
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
const std::string& Response::GetRequestFilePath() const
{
    return _request_file_path;
}
void Response::SetRequestFilePath(const std::string& path)
{
    _request_file_path = path;
}
const std::string& Response::GetRequestFileExtension() const
{
    return _request_file_extension;
}
void Response::SetRequestFileExtension(const std::string& ext)
{
    _request_file_extension = ext;
}
const LocationIterator& Response::GetRequestFileLocation() const
{
    return _request_file_location;
}
void Response::SetRequestFileLocation(const LocationIterator& location)
{
    _request_file_location = location;
}
struct stat Response::GetRequestFileStat() const
{
    return _request_file_stats;
}
void Response::SetRequestFileStat(struct stat& stat)
{
    _request_file_stats = stat;
    _content_lenght     = stat.st_size;
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
const uint8_t* Response::GetResponseBuff() const
{
    if (_bytes_sent_to_client < _headers.size())
        return (const uint8_t*)_headers.c_str() + _bytes_sent_to_client;
    if (_bytes_sent_to_client - _headers.size() < _content_lenght)
    {
        if (_request_file_fd != -1)
        {
            size_t remaining_bytes = _content_lenght - (_bytes_sent_to_client - _headers.size());
            if (remaining_bytes && _response_buff.size() < SEND_CHUNK)
            {
                size_t read_size = std::min(remaining_bytes, SEND_CHUNK);
                _response_buff.insert(_response_buff.end(), read_size, 0);
                ssize_t bytes_read = read(
                    _request_file_fd, (uint8_t*)(_response_buff.data() + _response_buff.size()),
                    read_size
                );
                if (bytes_read < 0 || bytes_read != read_size)
                {
                    close(_request_file_fd);
                    throw ResponseException("Read() failed.");
                }
            }
        }
        return _response_buff.data();
    }
    assert(!"IMPOSSIBLE");
    return NULL;
}
void Response::IncrementResponseBuffBytesSent(const size_t n)
{
    _bytes_sent_to_client += n;
    if (_response_buff.size() && _bytes_sent_to_client > _headers.size())
        _response_buff.erase(_response_buff.begin(), _response_buff.begin() + n);
    if (_bytes_sent_to_client >= _headers.size() + _content_lenght)
    {
        // Todo: move to a clear() method..
        if (_request_file_fd != -1)
            close(_request_file_fd);
        _request_file_fd = -1;
    }
}
size_t Response::GetResponseBuffCount() const
{
    if (_bytes_sent_to_client < _headers.size())
        return _headers.size() - _bytes_sent_to_client;
    if (_bytes_sent_to_client - _headers.size() < _content_lenght)
        return _content_lenght - (_bytes_sent_to_client - _headers.size());
    return 0;
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
void Response::AppendToResponseBuff(const std::vector<uint8_t>& content)
{
    _response_buff.insert(_response_buff.end(), content.begin(), content.end());
    _content_lenght += content.size();
}
void Response::FinishResponse(bool append_content_length /* = true*/)
{
    ResponseHeader header;
    if (append_content_length)
    {
        std::ostringstream content_length;
        content_length << _content_lenght;
        header.name  = "Content-Length";
        header.value = content_length.str();
        AppendHeader(header);
    }
    header.name  = "Connection";
    header.value = "keep-alive";
    AppendHeader(header);
    _headers += CRLF;
    std::cerr << "[Response headers]     ============" << std::endl;
    std::cerr << _headers << std::endl;
    std::cerr << "[End Response headers] ============" << std::endl;
    std::cerr << "[Response body]     ============" << std::endl;
    std::cerr << _content_lenght << " Bytes of content will be sent to client." << std::endl;
    std::cerr << "[End Response body] ============" << std::endl;
    _headers.erase();
}
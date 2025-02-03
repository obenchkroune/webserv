/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 01:37:24 by simo              #+#    #+#             */
/*   Updated: 2025/02/03 13:52:50 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseCGI.hpp"
#include <cstdlib>

ResponseCGI::~ResponseCGI() {}
ResponseCGI::ResponseCGI(const Response& response) : Response(response)
{
    assert(_request_file_fd == -1);
    assert(_headers.size() == 0);
    assert(_response_buff.size() == 0);
    _content_lenght      = 0; // resetting from previous call to Response::SetRequestFilestat()
    _cgi_line_delimiter  = CRLF;
    _cgi_is_reading_body = false;
    _start_time          = time(0);
}
void ResponseCGI::AppendToResponseBuff(const std::vector<uint8_t>& content)
{
    if (_cgi_is_reading_body == false)
    {
        _headers.insert(_headers.end(), content.begin(), content.end());
        char* headers_end = utils::strnstr(_headers.c_str(), CRLF CRLF, _headers.size());
        if (headers_end == NULL)
        {
            headers_end = utils::strnstr(_headers.c_str(), "\n\n", _headers.size());
            if (headers_end)
                _cgi_line_delimiter = "\n";
        }
        if (headers_end)
        {
            size_t headers_length =
                (headers_end - _headers.c_str()) + 2 * std::strlen(_cgi_line_delimiter);
            if (headers_length < _headers.size())
            {
                _content_lenght = _headers.size() - headers_length;
                _response_buff.insert(
                    _response_buff.end(), _headers.begin() + headers_length, _headers.end()
                );
                _headers.erase(headers_length);
            }
            _cgi_is_reading_body = true;
        }
    }
    else
    {
        _content_lenght += content.size();
        _response_buff.insert(_response_buff.end(), content.begin(), content.end());
    }
}
void ResponseCGI::FinishResponse()
{
    // Inserting proper HTTP status line:
    {
        size_t status_found = _headers.find("Status:");
        if (status_found != std::string::npos)
        {
            int status_code = (int)std::strtol(_headers.c_str() + status_found, NULL, 10);
            _cgi_status     = HttpStatus((HttpStatusCode)status_code);
        }
        {
            std::string status_line = HTTP_VERSION_TOKEN " ";
            status_line += _cgi_status.message;
            status_line += _cgi_line_delimiter;
            _headers.insert(_headers.begin(), status_line.begin(), status_line.end());
        }
    }
    // Inserting content length:
    {
        size_t content_length_found = _headers.find("Content-Length:");
        if (content_length_found == std::string::npos)
        {
            std::string        content_length_line = "Content-Length: ";
            std::ostringstream content_length;
            content_length << _content_lenght;
            content_length_line += content_length.str();
            content_length_line += _cgi_line_delimiter;
            _headers.insert(
                _headers.end() - std::strlen(_cgi_line_delimiter), content_length_line.begin(),
                content_length_line.end()
            );
        }
    }
}
time_t ResponseCGI::GetStartTime() const
{
    return _start_time;
}
void ResponseCGI::SetCGIPID(const int pid)
{
    _cgi_pid = pid;
}
int ResponseCGI::GetCGIPID() const
{
    return _cgi_pid;
}
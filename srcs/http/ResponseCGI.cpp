/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 01:37:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/28 03:33:23 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseCGI.hpp"

ResponseCGI::~ResponseCGI() {}
ResponseCGI::ResponseCGI(const Response& response) : Response(response)
{
    assert(_request_file_fd == -1);
    assert(_headers.size() == 0);
    assert(_response_buff.size() == 0);
    _content_lenght      = 0; // resetting from previous call to Response::SetRequestFilestat()
    _cgi_line_delimiter  = CRLF;
    _cgi_is_reading_body = false;
}
void ResponseCGI::AppendToResponseBuff(const std::vector<uint8_t>& content)
{
    if (_cgi_is_reading_body)
    {
        _content_lenght += content.size();
        _response_buff.insert(_response_buff.end(), content.begin(), content.end());
    }
    else
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
                (headers_end - _headers.c_str()) + std::strlen(_cgi_line_delimiter);
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
}
void ResponseCGI::FinishResponse()
{
    // Inserting proper HTTP status line:
    {
        size_t status_found = _headers.find("Status:");
        if (status_found != std::string::npos)
        {
            int status_code = std::atoi(_headers.c_str() + status_found);
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
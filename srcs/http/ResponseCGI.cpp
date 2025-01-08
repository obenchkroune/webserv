/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 01:37:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/08 15:10:37 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseCGI.hpp"

ResponseCGI::ResponseCGI(const Request& request, const ServerConfig& virtual_server, Server* server)
    : Response(request, virtual_server, server)
{
}
ResponseCGI::~ResponseCGI() {}
ResponseCGI::ResponseCGI(const Response& responseCGI) : Response(responseCGI) {}
ResponseCGI& ResponseCGI::operator=(const ResponseCGI& responseCGI)
{
    if (this == &responseCGI)
        return *this;
    return *this;
}
void ResponseCGI::FinishResponse(bool append_content_length)
{
    (void)append_content_length;

    bool append_length = true;
    // char* headers_end_pos = std::strstr((char*)_content.data(), CRLF CRLF);
    // assert(headers_end_pos != NULL);
    if (_content.size())
    {
        char* header_line_start = (char*)_content.data();
        char* header_line_end   = std::strstr(header_line_start, CRLF);
        if (std::strncmp(header_line_start, "HTTP/", std::strlen("HTTP/")))
        {
            std::string status_header(HTTP_VERSION_TOKEN " " HTTP_STATUS_OK CRLF);
            _content.insert(_content.begin(), status_header.begin(), status_header.end());
            header_line_start = (char*)_content.data() + status_header.size();
            header_line_start = std::strstr(header_line_start, CRLF);
            if (header_line_start != NULL)
            {
                header_line_start += 2;
                header_line_end = std::strstr(header_line_start, CRLF);
            }
        }
        for (; header_line_end != NULL && header_line_end != header_line_start;
             header_line_start = header_line_end + 2,
             header_line_end   = std::strstr(header_line_start, CRLF))
        {
            char* header_name = std::strchr(header_line_start, ':');
            assert(header_name != NULL);
            if (!std::strncmp(header_line_start, "Content-Length", std::strlen("Content-Length")))
                append_length = false;
        }
        assert(header_line_end != NULL);
        if (append_length)
        {
            size_t content_length = 0;
            if (header_line_end != NULL)
                content_length = _content.size() - ((header_line_end + 2) - (char*)_content.data());
            std::stringstream content_length_header;
            content_length_header << "Content-Length: " << content_length << CRLF;
            std::string content_length_header_str = content_length_header.str();
            _content.insert(
                _content.end() - (content_length + 2), content_length_header_str.begin(),
                content_length_header_str.end()
            );
        }
    }
}
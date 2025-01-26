/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 01:37:24 by simo              #+#    #+#             */
/*   Updated: 2025/01/26 18:18:16 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseCGI.hpp"

ResponseCGI::~ResponseCGI() {}
ResponseCGI::ResponseCGI(const Response& response) : Response(response) {}
void ResponseCGI::FinishResponse(bool append_content_length /* = true*/)
{
    char*       header_line_start = (char*)_content.data();
    const char* header_line_delim = CRLF;
    if (utils::strnstr(header_line_start, CRLF, _content.size()) == NULL)
        header_line_delim = "\n";
    int   header_line_delim_len = std::strlen(header_line_delim);
    char* header_line_end       = utils::strnstr(
        header_line_start, header_line_delim,
        _content.size() - (header_line_start - (char*)_content.data())
    );
    if (std::strncmp(header_line_start, "HTTP/", std::strlen("HTTP/")))
    {
        std::string status_header(HTTP_VERSION_TOKEN " " HTTP_STATUS_OK);
        status_header += header_line_delim;
        _content.insert(_content.begin(), status_header.begin(), status_header.end());
        header_line_start = (char*)_content.data() + status_header.size();
        header_line_start = utils::strnstr(
            header_line_start, header_line_delim,
            _content.size() - (header_line_start - (char*)_content.data())
        );
        if (header_line_start != NULL)
        {
            header_line_start += header_line_delim_len;
            header_line_end = utils::strnstr(
                header_line_start, header_line_delim,
                _content.size() - (header_line_start - (char*)_content.data())
            );
        }
    }
    for (; header_line_end != NULL && header_line_end != header_line_start;
         header_line_start = header_line_end + header_line_delim_len,
         header_line_end   = utils::strnstr(
             header_line_start, header_line_delim,
             _content.size() - (header_line_start - (char*)_content.data())
         ))
    {
        char* header_name = std::strchr(header_line_start, ':');
        assert(header_name != NULL);
        if (!std::strncmp(header_line_start, "Content-Length", std::strlen("Content-Length")))
            append_content_length = false;
    }
    assert(header_line_end != NULL);
    if (append_content_length)
    {
        size_t content_length = 0;
        if (header_line_end != NULL)
            content_length = _content.size() -
                             ((header_line_end + header_line_delim_len) - (char*)_content.data());
        std::stringstream content_length_header;
        content_length_header << "Content-Length: " << content_length << header_line_delim;
        std::string content_length_header_str = content_length_header.str();
        _content.insert(
            _content.end() - (content_length + header_line_delim_len),
            content_length_header_str.begin(), content_length_header_str.end()
        );
    }
}
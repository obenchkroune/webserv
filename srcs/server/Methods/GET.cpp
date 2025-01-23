/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 13:33:03 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/09 15:45:43 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessGET(Response* response, bool send_data /* = true*/)
{
    size_t max_sz_limit = response->GetFileLocation()->max_body_size;
    if (response->GetFileStat().st_size > (long)max_sz_limit)
    {
        std::cerr << "GET request too large: " << std::endl;
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE), response
        );
    }
    response->SetStatusHeaders(HTTP_STATUS_OK);
    ResponseHeader header;
    header.name = "Content-Type";
    MimeTypes::const_iterator it =
        Config::getInstance().getMimeTypes().find(response->GetFileExtension());
    if (it != Config::getInstance().getMimeTypes().end())
        header.value = it->second;
    else
        header.value = "application/octet-stream";
    response->AppendHeader(header);
    int file_fd = open(response->GetFilePath().c_str(), O_RDONLY);
    if (file_fd < 0)
    {
        std::cerr << "open() failed for file: " << response->GetFilePath();
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    }
    if (send_data == false)
    {
        close(file_fd);
        std::ostringstream content_length;
        content_length << response->GetFileStat().st_size;
        header.name  = "Content-Length";
        header.value = content_length.str();
        response->AppendHeader(header);
        response->FinishResponse(false);
    }
    else
    {
        response->ReadFile(file_fd);
        response->FinishResponse();
    }
    _server->QueueResponse(_client_socket_fd, response);
}

void ServerClient::ProcessHEAD(Response* response)
{
    ProcessGET(response, false);
}

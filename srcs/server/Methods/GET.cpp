/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 13:33:03 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/08 16:17:04 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessGET(Response* response, bool send_data /* = true*/)
{
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
    int file_fd = open(response->GetFileName().c_str(), O_RDONLY);
    if (file_fd < 0)
    {
        std::cerr << "open() failed for file: " << response->GetFileName();
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response
        );
    }
    if (send_data == false)
    {
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
    _server->QueueResponse(_socket_fd, response);
}

void ServerClient::ProcessHEAD(Response* response)
{
    ProcessGET(response, false);
}

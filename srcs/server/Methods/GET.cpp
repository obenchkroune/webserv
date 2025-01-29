/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 13:33:03 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/29 02:14:40 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessGET(Response* response, bool send_data /* = true*/)
{
    // This shouldn't be checked for:
    // size_t max_sz_limit = response->GetFileLocation()->max_body_size;
    /*if (response->GetFileStat().st_size > (long)max_sz_limit)
    {
        std::cerr << "GET request too large: " << std::endl;
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE), response
        );
    }*/
    response->SetStatusHeaders(HTTP_STATUS_OK);
    ResponseHeader header;
    header.name = "Content-Type";
    MimeTypes::const_iterator it =
        Config::getInstance().getMimeTypes().find(response->GetRequestFileExtension());
    if (it != Config::getInstance().getMimeTypes().end())
        header.value = it->second;
    else
        header.value = "application/octet-stream";
    response->AppendHeader(header);
    int file_fd = open(response->GetRequestFilePath().c_str(), O_RDONLY);
    if (file_fd < 0)
    {
        std::cerr << "open() failed for file: " << response->GetRequestFilePath();
        return SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    }
    if (send_data == false)
    {
        close(file_fd);
        response->FinishResponse();
        struct stat tmp_stat;
        tmp_stat.st_size = 0;
        response->SetRequestFileStat(tmp_stat);
    }
    else
    {
        response->SetRequestFileFd(file_fd);
        response->FinishResponse();
    }
    _responses_queue.push(response);
}

void ServerClient::ProcessHEAD(Response* response)
{
    ProcessGET(response, false);
}

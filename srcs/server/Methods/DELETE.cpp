/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DELETE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 17:31:57 by msitni            #+#    #+#             */
/*   Updated: 2024/12/08 17:14:17 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessDELETE(const Request &request, Response *response)
{
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(response->GetVirtualServer(), request.getUri());
    if (file_location == response->GetVirtualServer().locations.end())
        return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    std::pair<HttpStatus, std::string> file =
        ProcessFilePermission(request, file_location, F_OK); // Should check if file's owner UID matches webserv's UID
    if (file.first.code != STATUS_OK)
        return SendErrorResponse(file.first, response);
    std::string &file_name = file.second;

    struct stat path_stat;
    stat(file_name.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode))
    {
        std::vector<std::string>::const_iterator index_it = file_location->index.begin();
        for (; index_it != file_location->index.end(); index_it++)
        {
            std::string index_file_name = file_name + '/' + *index_it;
            if (access(index_file_name.c_str(), F_OK) == 0)
            {
                if (access(index_file_name.c_str(), F_OK) ==
                    0) // Should check if file's owner UID matches webserv's UID
                {
                    file_name = index_file_name;
                    break;
                }
                return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
            }
        }
        if (index_it == file_location->index.end())
            return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
        stat(file_name.c_str(), &path_stat);
    }
    /*
        Here should we use unlink(), remove()..??
        Unfortunatly all of those not mentioned in the subject.
        Please dont tell me we need to execve() just to do `rm` Ugh!!.
    */
    if (unlink(file_name.c_str()) == -1)
        return SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response);
    response->SetStatusHeaders(HTTP_STATUS_OK);
    response->FinishResponse(true);
    _server->QueueResponse(_socket_fd, response);
}
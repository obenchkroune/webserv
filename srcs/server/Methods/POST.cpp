/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POST.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:54:42 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/01 01:47:36 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessPOST(const Request &request, Response *response)
{
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(response->GetVirtualServer(), request.getUri());
    if (file_location == response->GetVirtualServer().locations.end())
        return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    std::pair<HttpStatus, std::string> file = CheckRequest(request, file_location);
    if (file.first.code != STATUS_OK)
        return SendErrorResponse(file.first, response);
    struct stat  path_stat;
    std::string &file_name = file.second;
    stat(file_name.c_str(), &path_stat);

    if (S_ISDIR(path_stat.st_mode))
    {
        std::vector<std::string>::const_iterator index_it = file_location->index.begin();
        for (; index_it != file_location->index.end(); index_it++)
        {
            std::string index_file_name = file_name + '/' + *index_it;
            if (access(index_file_name.c_str(), F_OK) == 0)
            {
                if (access(index_file_name.c_str(), X_OK) == 0)
                {
                    file_name = index_file_name;
                    break;
                }
                return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
            }
        }
        if (index_it == file_location->index.end())
            return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    }

    size_t max_sz_limit = response->GetVirtualServer().max_body_size;
    if (file_location->max_body_size != response->GetVirtualServer().max_body_size)
        max_sz_limit = file_location->max_body_size;
    if (request.getBody().size() > max_sz_limit)
    {
        std::cerr << "POST request too large: " << std::endl;
        return SendErrorResponse(HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE, HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE),
                                 response);
    }
    /*
     ** Fork And buffer data to cgi server
     ** Or
     ** Upload data to upload dir
     */
    response->FinishResponse(true);
    _server->QueueResponse(_socket_fd, response);
}

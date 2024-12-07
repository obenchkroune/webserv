/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POST.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:54:42 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/07 12:48:17 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessPOST(const Request &request, Response *response)
{
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(response->GetVirtualServer(), request.getUri());
    std::pair<http_status_code, std::string> file = ProcessFilePermission(request, response, file_location, X_OK);
    if (file.first != STATUS_OK)
        return;
    struct stat path_stat;
    std::string& file_name = file.second;
    stat(file_name.c_str(), &path_stat);
    size_t max_sz_limit = response->GetVirtualServer().max_body_size;
    if (file_location->max_body_size != response->GetVirtualServer().max_body_size)
        max_sz_limit = file_location->max_body_size;
    if (request.getBody().size() > max_sz_limit)
    {
        std::cerr << "POST request too large: " << std::endl;
        return SendErrorResponse(HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE, HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE),
                                 response);
    }
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
    /*
     ** Fork And buffer data to cgi server
     ** Or
     ** Upload data to upload dir
     */
    response->FinishResponse();
    _server->QueueResponse(_socket_fd, response);
}

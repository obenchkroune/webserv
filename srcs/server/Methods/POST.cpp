/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POST.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:54:42 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/06 14:20:44 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessPOST(const Request &request, Response *response)
{
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(response->GetVirtualServer(), request.getUri());
    if (std::find(file_location->allow_methods.begin(), file_location->allow_methods.end(), request.getMethod()) ==
        file_location->allow_methods.end())
        return SendErrorResponse(HttpStatus(STATUS_METHOD_NOT_ALLOWED, HTTP_STATUS_METHOD_NOT_ALLOWED), response);
    std::string file_name = file_location->root + '/' + request.getUri().substr(file_location->path.length());
    if (ServerUtils::validateFileLocation(file_location->root, file_name) == false)
    {
        std::cerr << "Client fd: " << _socket_fd << " thinks himself a hacker." << std::endl;
        std::cerr << "Access for file: " << file_name << " is outside the location root, request is forbidden."
                  << std::endl;
        return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
    }
    if (access(file_name.c_str(), F_OK) == 0) // EXISTENCE ACCESS
    {
        if (access(file_name.c_str(), X_OK) != 0) // EXECUTE ACCESS
        {
            std::cerr << "X_OK failed for file: " << file_name << std::endl;
            return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
        }
    }
    std::cerr << "F_OK failed for file: " << file_name << std::endl;
    return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    


    // The code above should be refactored and used for all request methods...


    
    struct stat path_stat;
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

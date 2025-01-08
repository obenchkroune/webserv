/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PUT.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 17:32:02 by msitni            #+#    #+#             */
/*   Updated: 2025/01/08 16:16:29 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessPUT(const Request &request, Response *response)
{
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(response->GetVirtualServer(), request.getUri());
    if (file_location == response->GetVirtualServer().locations.end())
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    if (std::find(file_location->allow_methods.begin(), file_location->allow_methods.end(), request.getMethod()) ==
        file_location->allow_methods.end())
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_METHOD_NOT_ALLOWED, HTTP_STATUS_METHOD_NOT_ALLOWED), response);
    size_t max_sz_limit = response->GetVirtualServer().max_body_size;
    if (file_location->max_body_size != response->GetVirtualServer().max_body_size)
        max_sz_limit = file_location->max_body_size;
    if (request.getBody().size() > max_sz_limit)
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE, HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE),
                                 response);
    std::string file_name = file_location->root + '/' + request.getUri().substr(file_location->path.length());
    if (ServerUtils::validateFileLocation(file_location->root, file_name) == false)
    {
        std::cerr << "Client fd: " << _socket_fd << " thinks himself a hacker." << std::endl;
        std::cerr << "Access for file: " << file_name << " is outside the location root, request is forbidden."
                  << std::endl;
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
    }
    struct stat path_stat;
    stat(file_name.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode))
    {
        if (file_location->index.size() == 0)
            return ServerUtils::SendErrorResponse(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST),
                                     response); // TODO: check ngnix behaviour
        file_name += '/' + file_location->index.front();
    }
    int put_fd = open(file_name.c_str(), O_WRONLY);
    if (put_fd == -1)
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN),
                                 response); // TODO: check ngnix behaviour
    int bytes = write(put_fd, request.getBody().c_str(), request.getBody().size());
    close(put_fd);
    if (bytes < 0 || (size_t)bytes != request.getBody().size())
    {
        unlink(file_name.c_str());
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response);
    }
    response->SetStatusHeaders(HTTP_STATUS_OK);
    response->FinishResponse();
    _server->QueueResponse(_socket_fd, response);
}
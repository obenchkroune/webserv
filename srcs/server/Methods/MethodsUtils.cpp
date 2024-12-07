/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodsUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 12:27:55 by msitni            #+#    #+#             */
/*   Updated: 2024/12/07 12:45:54 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "ServerUtils.hpp"

std::pair<http_status_code, std::string> ServerClient::ProcessFilePermission(const Request &request, Response *response,
                                                                             LocationsIterator file_location,
                                                                             int               permission)
{
    if (std::find(file_location->allow_methods.begin(), file_location->allow_methods.end(), request.getMethod()) ==
        file_location->allow_methods.end())
    {
        SendErrorResponse(HttpStatus(STATUS_METHOD_NOT_ALLOWED, HTTP_STATUS_METHOD_NOT_ALLOWED), response);
        return std::pair<http_status_code, std::string>(STATUS_METHOD_NOT_ALLOWED, "");
    }
    std::string file_name = file_location->root + '/' + request.getUri().substr(file_location->path.length());
    if (ServerUtils::validateFileLocation(file_location->root, file_name) == false)
    {
        std::cerr << "Client fd: " << _socket_fd << " thinks himself a hacker." << std::endl;
        std::cerr << "Access for file: " << file_name << " is outside the location root, request is forbidden."
                  << std::endl;
        SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
        return std::pair<http_status_code, std::string>(STATUS_FORBIDDEN, "");
    }
    if (access(file_name.c_str(), F_OK) != 0) // EXISTENCE ACCESS
    {
        SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
        return std::pair<http_status_code, std::string>(STATUS_FORBIDDEN, "");
    }
    if (access(file_name.c_str(), permission) != 0) // PERMISSION ACCESS
    {
        SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
        return std::pair<http_status_code, std::string>(STATUS_FORBIDDEN, "");
    }
    return std::pair<http_status_code, std::string>(STATUS_OK, file_name);
}

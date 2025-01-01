/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodsUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 12:27:55 by msitni            #+#    #+#             */
/*   Updated: 2025/01/01 22:04:50 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryListing.hpp"
#include "Server.hpp"
#include "ServerClient.hpp"
#include "ServerUtils.hpp"

void ServerClient::auto_index(Response* response)
{
    response->SetStatusHeaders(HTTP_STATUS_OK);
    ResponseHeader header;
    header.name  = "Content-Type";
    header.value = "text/html";
    response->AppendHeader(header);

    std::string html = DirectoryListing::generate(response->GetFileName(), response->GetRequest().getUri());
    std::vector<uint8_t> content(html.begin(), html.end());
    response->AppendContent(content);
    response->FinishResponse(true);
    _server->QueueResponse(_socket_fd, response);
}

std::pair<HttpStatus, std::string> ServerClient::CheckRequest(
    const Request& request, const LocationsIterator& file_location
)
{
    if (std::find(
            file_location->allow_methods.begin(), file_location->allow_methods.end(),
            request.getMethod()
        ) == file_location->allow_methods.end())
        return std::pair<HttpStatus, std::string>(
            HttpStatus(STATUS_METHOD_NOT_ALLOWED, HTTP_STATUS_METHOD_NOT_ALLOWED), ""
        );

    std::string file_name =
        file_location->root + '/' + request.getUri().substr(file_location->path.length());
    if (ServerUtils::validateFileLocation(file_location->root, file_name) == false)
    {
        std::cerr << "Client fd: " << _socket_fd << " thinks himself a hacker." << std::endl;
        std::cerr << "Access for file: " << file_name
                  << " is outside the location root, request is forbidden." << std::endl;
        return std::pair<HttpStatus, std::string>(
            HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), ""
        );
    }
    if (access(file_name.c_str(), F_OK) != 0) // EXISTENCE ACCESS
        return std::pair<HttpStatus, std::string>(
            HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), ""
        );
    return std::pair<HttpStatus, std::string>(HttpStatus(STATUS_OK, HTTP_STATUS_OK), file_name);
}

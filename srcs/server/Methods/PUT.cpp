/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PUT.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 17:32:02 by msitni            #+#    #+#             */
/*   Updated: 2025/01/23 18:47:45 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessPUT(Response* response)
{
    size_t max_sz_limit = response->GetVirtualServer().max_body_size;
    if (max_sz_limit != response->GetFileLocation()
                            ->max_body_size) // TODO: this should check if location directive is set
        max_sz_limit = response->GetFileLocation()->max_body_size;
    if (response->GetRequest().getBody().size() > max_sz_limit)
    {
        std::cerr << "PUT request too large: " << std::endl;
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE), response
        );
    }
    int put_fd = open(response->GetFilePath().c_str(), O_WRONLY);
    if (put_fd == -1)
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_FORBIDDEN),
            response
        ); // TODO: check ngnix behaviour when permission denied
    int bytes = write(
        put_fd, response->GetRequest().getBody().data(), response->GetRequest().getBody().size()
    );
    close(put_fd);
    if (bytes < 0 || (size_t)bytes != response->GetRequest().getBody().size())
    {
        unlink(response->GetFilePath().c_str());
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    }
    response->SetStatusHeaders(HTTP_STATUS_OK);
    response->FinishResponse();
    _server->QueueResponse(_client_socket_fd, response);
}
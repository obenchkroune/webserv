/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DELETE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 17:31:57 by msitni            #+#    #+#             */
/*   Updated: 2025/01/09 15:44:57 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessDELETE(Response *response)
{
    if (unlink(response->GetFilePath().c_str()) == -1)
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR, HTTP_STATUS_INTERNAL_SERVER_ERROR), response);
    response->SetStatusHeaders(HTTP_STATUS_OK);
    response->FinishResponse();
    _server->QueueResponse(_client_socket_fd, response);
}
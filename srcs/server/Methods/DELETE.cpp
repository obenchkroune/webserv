/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DELETE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 17:31:57 by msitni            #+#    #+#             */
/*   Updated: 2025/01/26 17:33:21 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessDELETE(Response* response)
{
    if (unlink(response->GetFilePath().c_str()) == -1)
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    response->SetStatusHeaders(HTTP_STATUS_OK);
    response->FinishResponse();
    _server->QueueResponse(response);
}
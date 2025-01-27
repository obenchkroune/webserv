/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DELETE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 17:31:57 by msitni            #+#    #+#             */
/*   Updated: 2025/01/28 00:53:38 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessDELETE(Response* response)
{
    if (unlink(response->GetRequestFilePath().c_str()) == -1)
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    response->SetStatusHeaders(HTTP_STATUS_OK);
    std::string message = "File: " + response->GetRequest().getUri() + " Deleted successfuly.";
    std::vector<uint8_t> buff(message.begin(), message.end());
    response->AppendToResponseBuff(buff);
    response->FinishResponse();
    _server->QueueResponse(response);
}
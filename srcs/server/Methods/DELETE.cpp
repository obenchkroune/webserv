/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DELETE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 17:31:57 by msitni            #+#    #+#             */
/*   Updated: 2025/01/31 15:30:15 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessDELETE(Response* response)
{
    if (unlink(response->GetRequestFilePath().c_str()) == -1)
        return SendErrorResponse(HttpStatus(STATUS_INTERNAL_SERVER_ERROR), response);
    struct stat tmp;
    tmp.st_size = 0;
    response->SetRequestFileStat(tmp);
    close(response->GetRequestFileFd());
    response->SetRequestFileFd(-1);
    response->SetStatusHeaders(HTTP_STATUS_OK);
    std::string message = "File: " + response->GetRequest().getUri() + " Deleted successfuly.";
    std::vector<uint8_t> buff(message.begin(), message.end());
    response->AppendToResponseBuff(buff);
    response->FinishResponse();
    _responses_queue.push(response);
}
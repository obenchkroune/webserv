/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POST.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:54:42 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/26 18:20:54 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessPOST(Response* response)
{
    size_t max_body_sz_limit = response->GetVirtualServer()->max_body_size;
    if (response->GetFileStat().st_size > (long)max_body_sz_limit) // TODO: this should check for body size
    {
        std::cerr << "POST request too large: " << std::endl;
        return ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE), response
        );
    }
    /*
     ** Fork And buffer data to cgi server
     ** Or
     ** Upload data to upload dir
     */
    response->FinishResponse();
    _server->QueueResponse(response);
}

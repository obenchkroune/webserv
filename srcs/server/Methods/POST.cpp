/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POST.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:54:42 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/26 17:34:31 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessPOST(Response* response)
{
    size_t max_sz_limit = response->GetVirtualServer().max_body_size;
    if (max_sz_limit != response->GetFileLocation()
                            ->max_body_size) // TODO: this should check if location directive is set
        max_sz_limit = response->GetFileLocation()->max_body_size;
    if (response->GetFileStat().st_size > (long)max_sz_limit)
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodsUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 12:27:55 by msitni            #+#    #+#             */
/*   Updated: 2025/01/26 17:34:26 by msitni           ###   ########.fr       */
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

    std::string html =
        DirectoryListing::generate(response->GetFilePath(), response->GetRequest().getUri());
    std::vector<uint8_t> content(html.begin(), html.end());
    response->AppendContent(content);
    response->FinishResponse();
    _server->QueueResponse(response);
}

HttpStatus ServerClient::CheckRequest(Response* response)
{
    if (std::find(
            response->GetFileLocation()->allow_methods.begin(),
            response->GetFileLocation()->allow_methods.end(), response->GetRequest().getMethod()
        ) == response->GetFileLocation()->allow_methods.end())
        return HttpStatus(STATUS_METHOD_NOT_ALLOWED);
    std::string file_path =
        response->GetFileLocation()->root + "/" +
        response->GetRequest().getUri().substr(response->GetFileLocation()->path.length());
    if (ServerUtils::validateFileLocation(response->GetFileLocation()->root, file_path) == false)
    {
        std::cerr << "Client fd: " << _client_socket_fd << " thinks himself a hacker." << std::endl;
        std::cerr << "Access for file: " << file_path
                  << " is outside the location root, request is forbidden." << std::endl;
        return HttpStatus(STATUS_FORBIDDEN);
    }
    if (access(file_path.c_str(), F_OK) != 0) // EXISTENCE ACCESS
        return HttpStatus(STATUS_NOT_FOUND);
    response->SetFilePath(file_path);
    stat(response->GetFilePath().c_str(), &response->GetFileStat());
    if (S_ISDIR(response->GetFileStat().st_mode))
    {
        std::vector<std::string>::const_iterator index_it =
            response->GetFileLocation()->index.begin();
        for (; index_it != response->GetFileLocation()->index.end(); index_it++)
        {
            std::string index_page_fname = response->GetFilePath() + '/' + *index_it;
            if (access(index_page_fname.c_str(), F_OK) == 0)
            {
                response->SetFilePath(index_page_fname);
                break;
            }
        }
        if (index_it == response->GetFileLocation()->index.end())
        {
            if (response->GetFileLocation()->autoindex)
                return HttpStatus(STATUS_HTTP_INTERNAL_IMPLEM_AUTO_INDEX);
            return HttpStatus(STATUS_FORBIDDEN);
        }
        stat(response->GetFilePath().c_str(), &response->GetFileStat());
    }
    if (access(response->GetFilePath().c_str(), R_OK) != 0) // READ ACCESS
        return HttpStatus(STATUS_FORBIDDEN);
    std::string fname = basename(response->GetFilePath().c_str());
    if (fname.find_last_of(".") != std::string::npos)
        response->SetFileExtension(fname.substr(fname.find_last_of(".") + 1));
    return HttpStatus(STATUS_OK);
}

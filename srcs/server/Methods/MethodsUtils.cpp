/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodsUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 12:27:55 by msitni            #+#    #+#             */
/*   Updated: 2025/01/31 18:50:33 by msitni           ###   ########.fr       */
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
        DirectoryListing::generate(response->GetRequestFilePath(), response->GetRequest().getUri());
    std::vector<uint8_t> content(html.begin(), html.end());
    response->AppendToResponseBuff(content);
    response->FinishResponse();
    _responses_queue.push(response);
}

HttpStatus ServerClient::CheckRequest(Response* response)
{
    const Request&   request               = response->GetRequest();
    LocationIterator request_file_location = request.getRequestFileLocation();
    if (std::find(
            request_file_location->allow_methods.begin(),
            request_file_location->allow_methods.end(), request.getMethod()
        ) == request_file_location->allow_methods.end())
        return HttpStatus(STATUS_METHOD_NOT_ALLOWED);
    std::string file_path =
        request_file_location->root + "/" +
        response->GetRequest().getUri().substr(request_file_location->path.length());
    if (ServerUtils::validateFileLocation(request_file_location->root, file_path) == false)
    {
        std::cerr << "Client fd: " << _client_socket_fd << " thinks himself a hacker." << std::endl;
        std::cerr << "Access for file: " << file_path
                  << " is outside the location root, request is forbidden." << std::endl;
        return HttpStatus(STATUS_FORBIDDEN);
    }
    if (access(file_path.c_str(), F_OK) != 0) // EXISTENCE ACCESS
        return HttpStatus(STATUS_NOT_FOUND);
    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) == -1)
    {
        std::cerr << "stat(): failed for file: " << file_path << std::endl;
        return HttpStatus(STATUS_INTERNAL_SERVER_ERROR);
    }
    if (S_ISDIR(file_stat.st_mode))
    {
        std::vector<std::string>::const_iterator index_it = request_file_location->index.begin();
        for (; index_it != request_file_location->index.end(); index_it++)
        {
            std::string index_page_fname = file_path + "/" + *index_it;
            if (access(index_page_fname.c_str(), F_OK) == 0)
            {
                file_path = index_page_fname;
                break;
            }
        }
        if (index_it == request_file_location->index.end())
        {
            if (request_file_location->autoindex)
            {
                response->SetRequestFilePath(file_path);
                auto_index(response);
                return HttpStatus(STATUS_HTTP_INTERNAL_IMPLEM_AUTO_INDEX);
            }
            return HttpStatus(STATUS_FORBIDDEN);
        }
        if (stat(file_path.c_str(), &file_stat) == -1)
        {
            std::cerr << "stat(): failed for file: " << file_path << std::endl;
            return HttpStatus(STATUS_INTERNAL_SERVER_ERROR);
        }
    }
    if (access(file_path.c_str(), R_OK) != 0) // READ ACCESS
        return HttpStatus(STATUS_FORBIDDEN);
    response->SetRequestFilePath(file_path);
    response->SetRequestFileStat(file_stat);
    std::string fname = basename(file_path.c_str());
    if (fname.find_last_of(".") != std::string::npos)
        response->SetRequestFileExtension(fname.substr(fname.find_last_of(".") + 1));
    return HttpStatus(STATUS_OK);
}

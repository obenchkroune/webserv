/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 13:33:03 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/05 11:47:49 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::ProcessGET(const Request &request, Response *response, bool send_data /* = true*/)
{
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(response->GetVirtualServer(), request.getUri());
    if (std::find(file_location->allow_methods.begin(), file_location->allow_methods.end(), request.getMethod()) ==
        file_location->allow_methods.end())
        return SendErrorResponse(HttpStatus(STATUS_METHOD_NOT_ALLOWED, HTTP_STATUS_METHOD_NOT_ALLOWED), response);
    std::string file_name = file_location->root + '/' + request.getUri().substr(file_location->path.length());
    if (ServerUtils::validateFileLocation(file_location->root, file_name) == false)
    {
        std::cerr << "Client fd: " << _socket_fd << " thinks himself a hacker." << std::endl;
        std::cerr << "Access for file: " << file_name << " is outside the location root, request is forbidden."
                  << std::endl;
        return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
    }
    if (access(file_name.c_str(), F_OK) != 0) // EXISTENCE ACCESS
    {
        std::cerr << "F_OK failed for file: " << file_name << std::endl;
        return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    }
    if (access(file_name.c_str(), R_OK) != 0) // READ ACCESS
    {
        std::cerr << "R_OK failed for file: " << file_name << std::endl;
        return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
    }
    struct stat path_stat;
    stat(file_name.c_str(), &path_stat);
    size_t max_sz_limit = response->GetVirtualServer().max_body_size;
    if (file_location->max_body_size != response->GetVirtualServer().max_body_size)
        max_sz_limit = file_location->max_body_size;
    if ((size_t)path_stat.st_size > max_sz_limit)
    {
        std::cerr << "file too large: " << file_name << std::endl;
        return SendErrorResponse(HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE, HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE),
                                 response);
    }
    if (S_ISDIR(path_stat.st_mode))
    {
        std::vector<std::string>::const_iterator index_it = file_location->index.begin();
        for (; index_it != file_location->index.end(); index_it++)
        {
            std::string index_file_name = file_name + '/' + *index_it;
            if (access(index_file_name.c_str(), R_OK) == 0)
            {
                file_name = index_file_name;
                break;
            }
        }
        if (index_it == file_location->index.end())
        {
            if (file_location->autoindex)
            {
                std::cerr << "[Error] Directory listing is not yet implemented" << std::endl;
                std::cerr << "Try specifying a file path like /index.html" << std::endl;
                throw NotImplemented();
            }
            else
            {
                return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
            }
        }
    }
    response->SetStatusHeaders(HTTP_STATUS_OK);
    std::string fname = basename(file_name.c_str());
    std::string extension;
    if (fname.find_last_of(".") != std::string::npos)
        extension = fname.substr(fname.find_last_of(".") + 1);
    // todo : need to be imported from mime types
    HttpHeader header;
    header.name = "Content-Type";
    if (extension == "html" || extension == "htm")
        header.value = "text/html";
    else
        header.value = "application/octet-stream";
    response->AppendHeader(header);
    int file_fd = open(file_name.c_str(), O_RDONLY);
    if (file_fd < 0)
        throw ServerClientException("open() failed for file: " + file_name);
    if (send_data == false)
    {
        stat(file_name.c_str(), &path_stat);
        std::ostringstream content_length;
        content_length << path_stat.st_size;
        header.name  = "Content-Length";
        header.value = content_length.str();
        response->AppendHeader(header);
    }
    else
    {
        response->ReadFile(file_fd);
    }
    response->FinishResponse();
    _server->QueueResponse(_socket_fd, response);
}

void ServerClient::ProcessHEAD(const Request &request, Response *response)
{
    ProcessGET(request, response, false);
}

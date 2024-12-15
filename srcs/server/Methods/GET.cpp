/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 13:33:03 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/10 14:02:45 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoryListing.hpp"
#include "Server.hpp"
#include "ServerClient.hpp"

void ServerClient::
    ProcessGET(const Request& request, Response* response, bool send_data /* = true*/) {
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(response->GetVirtualServer(), request.getUri());
    if (file_location == response->GetVirtualServer().locations.end())
        return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    std::pair<HttpStatus, std::string> file = ProcessFilePermission(request, file_location, R_OK);
    if (file.first.code != STATUS_OK)
        return SendErrorResponse(file.first, response);
    std::string& file_name = file.second;

    struct stat path_stat;
    stat(file_name.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode)) {
        std::vector<std::string>::const_iterator index_it = file_location->index.begin();
        for (; index_it != file_location->index.end(); index_it++) {
            std::string index_file_name = file_name + '/' + *index_it;
            if (access(index_file_name.c_str(), F_OK) == 0) {
                if (access(index_file_name.c_str(), R_OK) == 0) {
                    file_name = index_file_name;
                    break;
                }
                return SendErrorResponse(
                    HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response
                );
            }
        }
        if (index_it == file_location->index.end()) {
            if (file_location->autoindex) {
                response->SetStatusHeaders(HTTP_STATUS_OK);
                ResponseHeader header;
                header.name  = "Content-Type";
                header.value = "text/html";
                response->AppendHeader(header);

                std::string          html = DirectoryListing::generate(file_name, request.getUri());
                std::vector<uint8_t> content(html.begin(), html.end());
                response->AppendContent(content);
                response->FinishResponse(true);
                _server->QueueResponse(_socket_fd, response);
                return;
            } else {
                return SendErrorResponse(
                    HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response
                );
            }
        }
        stat(file_name.c_str(), &path_stat);
    }

    size_t max_sz_limit = response->GetVirtualServer().max_body_size;
    if (file_location->max_body_size != response->GetVirtualServer().max_body_size)
        max_sz_limit = file_location->max_body_size;
    if ((size_t)path_stat.st_size > max_sz_limit) {
        std::cerr << "file too large: " << file_name << std::endl;
        return SendErrorResponse(
            HttpStatus(STATUS_REQUEST_ENTITY_TOO_LARGE, HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE),
            response
        );
    }

    response->SetStatusHeaders(HTTP_STATUS_OK);
    std::string fname = basename(file_name.c_str());
    std::string extension;
    if (fname.find_last_of(".") != std::string::npos)
        extension = fname.substr(fname.find_last_of(".") + 1);
    // todo : need to be imported from mime types
    ResponseHeader header;
    header.name = "Content-Type";
    if (extension == "html" || extension == "htm")
        header.value = "text/html";
    else
        header.value = "application/octet-stream";
    response->AppendHeader(header);
    int file_fd = open(file_name.c_str(), O_RDONLY);
    if (file_fd < 0)
        throw ServerClientException("open() failed for file: " + file_name);
    if (send_data == false) {
        stat(file_name.c_str(), &path_stat);
        std::ostringstream content_length;
        content_length << path_stat.st_size;
        header.name  = "Content-Length";
        header.value = content_length.str();
        response->AppendHeader(header);
        response->FinishResponse(false);
    } else {
        response->ReadFile(file_fd);
        response->FinishResponse(true);
    }
    _server->QueueResponse(_socket_fd, response);
}

void ServerClient::ProcessHEAD(const Request& request, Response* response) {
    ProcessGET(request, response, false);
}

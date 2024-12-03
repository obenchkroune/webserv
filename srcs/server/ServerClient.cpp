/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2024/12/03 18:57:36 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "Server.hpp"

ServerClient::ServerClient(const int socket_fd, Server *server, IOMultiplexer *IOmltplx)
    : _socket_fd(socket_fd), _server(server)
{
    if (server == NULL)
        throw ServerClientException("Can't have a null server ptr.");
    if (IOmltplx == NULL)
        throw ServerClientException("Can't have a null IOmltplx ptr.");
}
ServerClient::ServerClient(const ServerClient &client)
{
    *this = client;
}
ServerClient &ServerClient::operator=(const ServerClient &client)
{
    if (this == &client)
        return *this;
    _socket_fd   = client._socket_fd;
    _request_raw = client._request_raw;
    _server      = client._server;
    return *this;
}
ServerClient::~ServerClient()
{
}
void ServerClient::ReceiveRequest(const std::string buff)
{
    std::cout << "Client fd: " << _socket_fd << " adding a chunk of the request:\n";
    _request_raw += buff;
    if (_request_raw.find("\r\n\r\n") != std::string::npos)
    {
        std::cout << "Client fd: " << _socket_fd << " Got the full request:\n";
        std::cout << "Parsing request" << std::endl;
        Request req(_request_raw);
        try
        {
            req.Parse();
            std::cout << "Request parsed successfuly." << std::endl;
        }
        catch (const std::exception &e)
        {
            _request_raw.clear();
            std::cerr << "Request error for client on fd: " << _socket_fd << " reason: " << e.what() << '\n';
            std::cerr << "Request ignored." << std::endl;
            return;
        }
        req.print();
        _request_raw.clear();
        try
        {
            ProcessRequest(req);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Request not handled due to: " << e.what() << std::endl;
        }
    }
}
void ServerClient::SendErrorResponse(const HttpStatus &status, Response *response)
{
    response->SetStatusHeaders(status.name);
    const std::map<uint16_t, std::string>          &error_pages = _server->GetConfig().error_pages;
    std::map<uint16_t, std::string>::const_iterator it          = error_pages.find(status.code);
    if (it != error_pages.end())
    {
        std::vector<LocationConfig>::const_iterator file_location =
            ServerUtils::GetFileLocation(_server->GetConfig(), it->second);
        std::string file_name = file_location->root + '/' + it->second.substr(file_location->path.length());
        int         error_fd  = open(file_name.c_str(), O_RDONLY);
        if (error_fd >= 0)
        {
            response->ReadFile(error_fd);
        }
        else
        {
            std::cerr << "open() failed for error page file: " << file_name << " ignoring." << std::endl;
        }
    }
    response->FinishResponse();
    _server->QueueResponse(_socket_fd, response);
}
void ServerClient::ProcessRequest(const Request &request)
{
    if (request.getMethod() != HTTP_GET)
        throw NotImplemented();
    Response                                   *response = new Response(request);
    std::vector<LocationConfig>::const_iterator file_location =
        ServerUtils::GetFileLocation(_server->GetConfig(), request.getUri());
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
    response->ReadFile(file_fd);
    response->FinishResponse();
    _server->QueueResponse(_socket_fd, response);
}
int ServerClient::Getfd() const
{
    return _socket_fd;
}
void ServerClient::Setfd(const int fd)
{
    _socket_fd = fd;
}

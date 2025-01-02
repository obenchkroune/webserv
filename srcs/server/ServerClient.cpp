/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2025/01/02 15:12:00 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "Server.hpp"

ServerClient::ServerClient(const int& socket_fd, const int& address_fd, Server* server)
    : _socket_fd(socket_fd), _address_fd(address_fd), _server(server)
{
    if (server == NULL)
        throw ServerClientException("Can't have a null server ptr.");
}
ServerClient::ServerClient(const ServerClient& client)
    : _socket_fd(client._socket_fd), _address_fd(client._address_fd)
{
    *this = client;
}
ServerClient& ServerClient::operator=(const ServerClient& client)
{
    if (this == &client)
        return *this;
    _request_raw = client._request_raw;
    _server      = client._server;
    return *this;
}
ServerClient::~ServerClient() {}
void ServerClient::ReceiveRequest(const std::string buff)
{
    _request_raw += buff;
    if (_request_raw.find("\r\n\r\n") != std::string::npos)
    {
        std::cout << "Client fd: " << _socket_fd << " Got the full request:\n";
        std::cout << "Parsing request" << std::endl;
        Request req(_request_raw);
        _request_raw.clear();
        HttpStatus status = req.parse();
        if (status.code != STATUS_OK)
        {
            Response* response = new Response(req, _server->GetConfig().front());
            std::cerr << "HTTP_Request not accepted due to: " << status.name << std::endl;
            std::cerr << req;
            return SendErrorResponse(status, response);
        }
        std::cout << "Request parsed successfuly." << std::endl;
        std::cout << req;
        try
        {
            ProcessRequest(req);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Request not handled due to: " << e.what() << std::endl;
            std::cerr << req;
        }
    }
    else if (_request_raw.size() > REQUEST_HEADER_LIMIT)
    {
        std::cerr << "Request Headers exceeded the REQUEST_HEADER_LIMIT: " << REQUEST_HEADER_LIMIT
                  << std::endl;
        Request req("");
        _request_raw.clear();
        Response* response = new Response(req, _server->GetConfig().front());
        return SendErrorResponse(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST), response);
    }
}
void ServerClient::SendErrorResponse(const HttpStatus& status, Response* response)
{
    response->SetStatusHeaders(status.name);
    const std::map<uint16_t, std::string>& error_pages = response->GetVirtualServer().error_pages;
    std::map<uint16_t, std::string>::const_iterator it = error_pages.find(status.code);
    if (it != error_pages.end())
    {
        std::vector<LocationConfig>::const_iterator error_page_file_location =
            ServerUtils::GetFileLocation(response->GetVirtualServer(), it->second);
        if (error_page_file_location != response->GetVirtualServer().locations.end())
        {
            std::string file_name = error_page_file_location->root + '/' +
                                    it->second.substr(error_page_file_location->path.length());
            int error_fd = open(file_name.c_str(), O_RDONLY);
            if (error_fd >= 0)
            {
                response->ReadFile(error_fd);
            }
            else
            {
                std::cerr << "open() failed for error page file: " << file_name << " ignoring."
                          << std::endl;
            }
        }
    }
    response->FinishResponse(true);
    _server->QueueResponse(_socket_fd, response);
}
void ServerClient::ProcessRequest(const Request& request)
{
    const ServerConfig& virtualServer =
        ServerUtils::GetRequestVirtualServer(_address_fd, request, _server->GetConfig());
    Response* response = new Response(request, virtualServer);
    response->SetClientSocketFd(_socket_fd);
    response->SetFileLocation(
        ServerUtils::GetFileLocation(response->GetVirtualServer(), response->GetRequest().getUri())
    );
    if (response->GetFileLocation() == response->GetVirtualServer().locations.end())
        return SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    std::pair<HttpStatus, std::string> check =
        CheckRequest(response->GetRequest(), response->GetFileLocation());
    if (check.first.code != STATUS_OK)
        return SendErrorResponse(check.first, response);
    response->SetFileName(check.second);
    stat(response->GetFileName().c_str(), &response->GetFileStat());
    if (S_ISDIR(response->GetFileStat().st_mode))
    {
        std::vector<std::string>::const_iterator index_it =
            response->GetFileLocation()->index.begin();
        for (; index_it != response->GetFileLocation()->index.end(); index_it++)
        {
            std::string index_page_fname = response->GetFileName() + '/' + *index_it;
            if (access(index_page_fname.c_str(), F_OK) == 0)
            {
                response->SetFileName(index_page_fname);
                break;
            }
        }
        if (index_it == response->GetFileLocation()->index.end())
        {
            if (response->GetFileLocation()->autoindex)
                return auto_index(response);
            else
                return SendErrorResponse(
                    HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response
                );
        }
        stat(response->GetFileName().c_str(), &response->GetFileStat());
    }
    if (access(response->GetFileName().c_str(), R_OK) != 0)
        return SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
    std::string fname = basename(response->GetFileName().c_str());
    if (fname.find_last_of(".") != std::string::npos)
        response->SetFileExtension(fname.substr(fname.find_last_of(".") + 1));
    if (response->GetFileLocation()->cgi_path != "")
    {
        std::vector<std::string>::const_iterator it = std::find(
            response->GetFileLocation()->cgi_extensions.begin(),
            response->GetFileLocation()->cgi_extensions.end(), response->GetFileExtension()
        );
        if (it != response->GetFileLocation()->cgi_extensions.end())
        {
            return ProcessCGI(response);
        }
    }
    switch (request.getMethod())
    {
    case HTTP_GET: {
        ProcessGET(response);
        break;
    }
    case HTTP_HEAD: {
        ProcessHEAD(response);
        break;
    }
    case HTTP_PUT: {
        ProcessPUT(request, response);
        break;
    }
    case HTTP_DELETE: {
        ProcessDELETE(request, response);
        break;
    }
    default:
        SendErrorResponse(
            HttpStatus(STATUS_NOT_IMPLEMENTED, HTTP_STATUS_NOT_IMPLEMENTED), response
        );
    }
}
int ServerClient::Getfd() const
{
    return _socket_fd;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2025/01/08 15:12:37 by simo             ###   ########.fr       */
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
        std::cout << _request_raw << "\nParsing request" << std::endl;
        Request req(_request_raw);
        _request_raw.clear();
        HttpStatus status = req.parse();
        if (status.code != STATUS_OK)
        {
            Response* response = new Response(req, _server->GetConfig().front(), _server);
            std::cerr << "HTTP_Request not accepted due to: " << status.name << std::endl;
            std::cerr << req;
            return ServerUtils::SendErrorResponse(status, response);
        }
        std::cout << "Request parsed successfuly." << std::endl;
        std::cout << req.getUri() << std::endl;
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
        Response* response = new Response(req, _server->GetConfig().front(), _server);
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST), response);
    }
}
void ServerClient::ProcessRequest(const Request& request)
{
    const ServerConfig& virtualServer =
        ServerUtils::GetRequestVirtualServer(_address_fd, request, _server->GetConfig());
    Response* response = new Response(request, virtualServer, _server);
    response->SetClientSocketFd(_socket_fd);
    response->SetFileLocation(
        ServerUtils::GetFileLocation(response->GetVirtualServer(), response->GetRequest().getUri())
    );
    if (response->GetFileLocation() == response->GetVirtualServer().locations.end())
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_NOT_FOUND, HTTP_STATUS_NOT_FOUND), response);
    std::pair<HttpStatus, std::string> check =
        CheckRequest(response->GetRequest(), response->GetFileLocation());
    if (check.first.code != STATUS_OK)
        return ServerUtils::SendErrorResponse(check.first, response);
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
                return ServerUtils::SendErrorResponse(
                    HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response
                );
        }
        stat(response->GetFileName().c_str(), &response->GetFileStat());
    }
    if (access(response->GetFileName().c_str(), R_OK) != 0)
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_FORBIDDEN, HTTP_STATUS_FORBIDDEN), response);
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
            Response* CGIresponse = new ResponseCGI(*response);
            delete response;
            return ProcessCGI(CGIresponse);
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
        ServerUtils::SendErrorResponse(
            HttpStatus(STATUS_NOT_IMPLEMENTED, HTTP_STATUS_NOT_IMPLEMENTED), response
        );
    }
}
int ServerClient::Getfd() const
{
    return _socket_fd;
}
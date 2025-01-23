/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2025/01/23 18:50:19 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "Server.hpp"

ServerClient::ServerClient(
    const int& client_socket_fd, const int& address_socket_fd, Server* server
)
    : _client_socket_fd(client_socket_fd), _address_socket_fd(address_socket_fd), _server(server)
{
}

ServerClient::ServerClient(const ServerClient& client)
    : _client_socket_fd(client._client_socket_fd), _address_socket_fd(client._address_socket_fd)
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

int ServerClient::GetClientSocketfd() const
{
    return _client_socket_fd;
}

void ServerClient::ReceiveRequest(const std::vector<uint8_t>& buff)
{
    _request += buff;

    if (_request.isCompleted())
    {
        if (_request.getStatus().code != STATUS_OK)
        {
            Response* response = new Response(_request, _server->GetConfig().front(), _server);
            _request.clear();
            return ServerUtils::SendErrorResponse(_request.getStatus(), response);
        }

        try
        {
            ProcessRequest(_request);
        }
        catch (const std::exception& e)
        {
            Response* response = new Response(_request, _server->GetConfig().front(), _server);
            ServerUtils::SendErrorResponse(_request.getStatus(), response);
        }
        _request.clear();
    }
}

void ServerClient::ProcessRequest(const Request& request)
{
    const ServerConfig& virtualServer =
        ServerUtils::GetRequestVirtualServer(_address_socket_fd, request, _server->GetConfig());
    Response* response = new Response(request, virtualServer, _server);
    response->SetClientSocketFd(_client_socket_fd);
    response->SetFileLocation(
        ServerUtils::GetFileLocation(response->GetVirtualServer(), response->GetRequest().getUri())
    );
    if (response->GetFileLocation() == response->GetVirtualServer().locations.end())
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_NOT_FOUND), response);
    HttpStatus check = CheckRequest(response);
    if (check.code == STATUS_HTTP_INTERNAL_IMPLEM_AUTO_INDEX)
        return auto_index(response);
    if (check.code != STATUS_OK)
        return ServerUtils::SendErrorResponse(check, response);
    // HTTP CGI response:
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
    // HTTP response:
    const std::string& method = request.getMethod();
    if (method == "GET")
    {
        return ProcessGET(response);
    }
    else if (method == "HEAD")
    {
        return ProcessHEAD(response);
    }
    else if (method == "POST")
    {
        return ProcessPOST(response);
    }
    else if (method == "PUT")
    {
        return ProcessPUT(response);
    }
    else if (method == "DELETE")
    {
        return ProcessDELETE(response);
    }
    else
    {
        return ServerUtils::SendErrorResponse(HttpStatus(STATUS_NOT_IMPLEMENTED), response);
    }
}
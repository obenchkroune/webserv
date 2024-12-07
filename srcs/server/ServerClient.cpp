/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2024/12/07 16:59:51 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "Server.hpp"

ServerClient::ServerClient(const int &socket_fd, const int &address_fd, Server *server)
    : _socket_fd(socket_fd), _address_fd(address_fd), _server(server)
{
    if (server == NULL)
        throw ServerClientException("Can't have a null server ptr.");
}
ServerClient::ServerClient(const ServerClient &client) : _socket_fd(client._socket_fd), _address_fd(client._address_fd)
{
    *this = client;
}
ServerClient &ServerClient::operator=(const ServerClient &client)
{
    if (this == &client)
        return *this;
    _request_raw = client._request_raw;
    _server      = client._server;
    return *this;
}
ServerClient::~ServerClient()
{
}
void ServerClient::ReceiveRequest(const std::string buff)
{
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
    const std::map<uint16_t, std::string>          &error_pages = response->GetVirtualServer().error_pages;
    std::map<uint16_t, std::string>::const_iterator it          = error_pages.find(status.code);
    if (it != error_pages.end())
    {
        std::vector<LocationConfig>::const_iterator error_page_file_location =
            ServerUtils::GetFileLocation(response->GetVirtualServer(), it->second);
        if (error_page_file_location != response->GetVirtualServer().locations.end())
        {
            std::string file_name =
                error_page_file_location->root + '/' + it->second.substr(error_page_file_location->path.length());
            int error_fd = open(file_name.c_str(), O_RDONLY);
            if (error_fd >= 0)
            {
                response->ReadFile(error_fd);
            }
            else
            {
                std::cerr << "open() failed for error page file: " << file_name << " ignoring." << std::endl;
            }
        }
    }
    response->FinishResponse(true);
    _server->QueueResponse(_socket_fd, response);
}
void ServerClient::ProcessRequest(const Request &request)
{
    const ServerConfig &virtualServer =
        ServerUtils::GetRequestVirtualServer(_address_fd, request, _server->GetConfig());
    Response *response = new Response(request, virtualServer);
    switch (request.getMethod())
    {
    case HTTP_GET: {
        ProcessGET(request, response);
        break;
    }
    case HTTP_HEAD: {
        ProcessHEAD(request, response);
        break;
    }
    default:
        return SendErrorResponse(HttpStatus(STATUS_NOT_IMPLEMENTED, HTTP_STATUS_NOT_IMPLEMENTED), response);
    }
}
int ServerClient::Getfd() const
{
    return _socket_fd;
}
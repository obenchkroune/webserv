/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 18:51:11 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "Server.hpp"

ServerClient::ServerClient(const int fd, Server *server) : _fd(fd), _server(server)
{
    if (server == NULL)
        throw ServerClientException("Can't have a null server ptr.");
}
ServerClient::ServerClient(const ServerClient &client)
{
    *this = client;
}
ServerClient &ServerClient::operator=(const ServerClient &client)
{
    if (this == &client)
        return *this;
    _fd      = client._fd;
    _content = client._content;
    _server  = client._server;
    return *this;
}
ServerClient::~ServerClient()
{
}
void ServerClient::PushContent(std::string buff)
{
    std::cout << "Client fd: " << _fd << " adding a chunk of the request:\n";
    _content += buff;
    if (_content.find("\r\n\r\n") != std::string::npos)
    {
        std::cout << "Client fd: " << _fd << " Got the full request:\n";
        std::cout << "Parsing request" << std::endl;
        Request req(_content);
        try
        {
            req.Parse();
            std::cout << "Request parsed successfuly." << std::endl;
        }
        catch (const std::exception &e)
        {
            _content.clear();
            std::cerr << "Request error for client on fd: " << _fd << " reason: " << e.what() << '\n';
            std::cerr << "Request ignored." << std::endl;
            return;
        }
        req.print();
        _server->AddRequest(*this, req);
    }
}
int ServerClient::Getfd() const
{
    return _fd;
}
void ServerClient::Setfd(const int fd)
{
    _fd = fd;
}

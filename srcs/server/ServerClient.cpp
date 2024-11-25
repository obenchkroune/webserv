/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 15:43:04 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"

ServerClient::ServerClient(const int fd) : _fd(fd)
{
}
ServerClient::ServerClient(const ServerClient &client)
{
    *this = client;
}
ServerClient &ServerClient::operator=(const ServerClient &client)
{
    if (this == &client)
        return *this;
    _fd = client._fd;
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
        std::cout << "HTTP Method       : [" << req.getMethod() << ']' << std::endl;
        std::cout << "HTTP URI          : [" << req.getUri() << ']' << std::endl;
        std::cout << "HTTP Version      : [" << req.getVersion() << ']' << std::endl;
        const std::vector<HttpHeader>          &headers = req.getHeaders();
        std::vector<HttpHeader>::const_iterator it      = headers.begin();
        std::cout << "Request Headers   :" << std::endl;
        for (; it != headers.end(); it++)
        {
            std::cout << "  " << it->name << ": " << it->value << std::endl;
            std::vector<std::string>                 params    = it->parameters;
            std::vector<std::string>::const_iterator params_it = it->parameters.begin();
            for (; params_it != params.end(); params_it++)
                std::cout << "  param: " << *params_it << std::endl;
        }
        std::cout << "Request Body  : [" << req.getBody() << ']' << std::endl;
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

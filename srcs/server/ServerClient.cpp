/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:55:35 by msitni            #+#    #+#             */
/*   Updated: 2024/12/02 22:56:53 by msitni1337       ###   ########.fr       */
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
    _socket_fd     = client._socket_fd;
    _request_raw   = client._request_raw;
    _server        = client._server;
    return *this;
}
ServerClient::~ServerClient()
{}
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
        catch (const std::exception& e)
        {
            std::cerr << "Request not handled due to: " << e.what() << std::endl;
        }
    }
}
void ServerClient::ProcessRequest(const Request &request)
{
    if (request.getMethod() != HTTP_GET)
        throw NotImplemented();
    /*
    here should check the uri path
    to assign appropriate location.
    */
    std::string file_name = _server->GetConfig().locations.front().root;
    file_name += request.getUri();
    if (access(file_name.c_str(), R_OK))
    {
        std::cerr << "[Error] File not found " << file_name << std::endl;
        throw NotImplemented();
        // Todo return 404 response..
        return;
    }
    struct stat path_stat;
    stat(file_name.c_str(), &path_stat);
    if (S_ISREG(path_stat.st_mode) == 0)
    {
        std::cerr << "[Error] Directory listing is not yet implemented" << std::endl;
        std::cerr << "Try specifying a file path like /index.html" << std::endl;
        throw NotImplemented();
        return;
    }
    Response response(request);
    response.SetStatusHeaders(HTTP_STATUS_OK);
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
    response.AppendHeader(header);
    int file_fd = open(file_name.c_str(), O_RDONLY);
    if (file_fd < 0)
        throw ServerClientException("open() failed for file: " + file_name);
    for (;;)
    {
        char buff[READ_CHUNK];
        int  bytes = read(file_fd, buff, READ_CHUNK - 1);
        if (bytes < 0)
            throw ServerClientException("read() failed for given fd.");
        buff[bytes] = 0;
        if (bytes == 0)
            break;
        response.AppendContent(buff);
    }
    close(file_fd);
    response.EndResponse();
    _server->QueueResponse(_socket_fd, response.GetResponseString());
}
int ServerClient::Getfd() const
{
    return _socket_fd;
}
void ServerClient::Setfd(const int fd)
{
    _socket_fd = fd;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/25 18:26:47 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

sockaddr_in Server::get_listen_addr(ServerConfig &_config)
{
    sockaddr_in  address;
    std::string &ip = _config.host;
    {
        for (size_t dot, pos = 0, i = 0; i < 4; i++, pos = dot + 1)
        {
            if ((dot = ip.find(".", pos)) == std::string::npos)
            {
                if (i < 3)
                    Terminate(), throw ServerException("Bad ip address string.", *this);
                else
                    dot = ip.length();
            }
            if (dot - pos > 3 || dot - pos <= 0)
                Terminate(), throw ServerException("Bad ip address string.", *this);
            std::string raw_byte = ip.substr(pos, dot);

            uint8_t  byte       = std::atoi(raw_byte.c_str());
            uint8_t *ip_address = (uint8_t *)&address.sin_addr.s_addr;
            ip_address[i]       = byte;
        }
    }
    address.sin_port   = htons(_config.port);
    address.sin_family = AF_INET;
    return address;
}

Server::Server(const ServerConfig &config, IOMultiplexer *IOmltplx, bool start /*= false*/)
    : _config(config), _is_started(false), _IOmltplx(IOmltplx)
{
    if (_IOmltplx == NULL)
        throw ServerException("I/O Multiplexer object is set to NULL\n", *this);
    _listen_addr             = get_listen_addr(_config);
    _listen_socket_ev.events = EPOLLIN;
    if (start)
        Start();
}
Server::Server(const Server &server) : AIOEventListener(server)
{
    *this = server;
}
Server &Server::operator=(const Server &server)
{
    if (this == &server)
        return *this;
    _clients          = server._clients;
    _config           = server._config;
    _is_started       = server._is_started;
    _listen_addr      = server._listen_addr;
    _listen_socket_fd = server._listen_socket_fd;
    _listen_socket_ev = server._listen_socket_ev;
    _IOmltplx         = server._IOmltplx;
    return *this;
}
Server::~Server()
{
    Terminate();
}
bool Server::is_started() const
{
    return _is_started;
}
const ServerConfig &Server::GetConfig() const
{
    return _config;
}
void Server::Start()
{
    if (_is_started)
        throw ServerException("Server is already started.", *this);
    _is_started       = true;
    _listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_socket_fd == -1)
        throw ServerException("socket(): failed.", *this);
    int active = 1;
    if (setsockopt(_listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &active, sizeof(int)) == -1)
        Terminate(), throw ServerException("setsockopt(): failed.", *this);
    if (bind(_listen_socket_fd, (sockaddr *)&_listen_addr, sizeof(_listen_addr)) == -1)
        Terminate(), throw ServerException("bind(): failed.", *this);
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        Terminate(), throw ServerException("listen(): failed.", *this);
    _listen_socket_ev.data.ptr = this;
    _IOmltplx->AddEvent(_listen_socket_ev, _listen_socket_fd);
    uint8_t *ip = (uint8_t *)&_listen_addr.sin_addr.s_addr;
    std::cout << "Server started on listening succefully.\n";
    std::cout << "Address: " << +ip[0] << '.' << +ip[1] << '.' << +ip[2] << '.' << +ip[3] << ':'
              << ntohs(_listen_addr.sin_port) << '\n';
}
void Server::ConsumeEvent(const epoll_event ev)
{
    std::cout << "[Server: " << _config.host << ':' << _config.port << "]: New event." << std::endl;
    if (ev.data.fd == _listen_socket_fd)
        acceptNewPeer();
    else
        handlePeerEvent(ev);
}
void Server::acceptNewPeer()
{
    std::cout << "Listening socket event." << std::endl;
    sockaddr_in peer_address;
    socklen_t   peer_address_len = sizeof(peer_address);
    int         peer_fd          = accept(_listen_socket_fd, (sockaddr *)&peer_address, &peer_address_len);
    if (peer_fd == -1)
        Terminate(), throw ServerException("accept(): failed to accept new connection.", *this);
    std::map<int, ServerClient>::iterator it = _clients.find(peer_fd);
    if (it == _clients.end())
        _clients.insert(std::pair<int, ServerClient>(peer_fd, ServerClient(peer_fd, this)));
    else
        Terminate(), throw ServerException("fd already exists in clients.", *this);
    epoll_event p_ev;
    p_ev.events   = EPOLLIN;
    p_ev.data.ptr = this;
    _IOmltplx->AddEvent(p_ev, peer_fd);
    uint8_t *peer_ip = (uint8_t *)&peer_address.sin_addr.s_addr;
    std::cout << "new peer accepted on fd " << peer_fd << ".\n";
    std::cout << "Address: " << +peer_ip[0] << '.' << +peer_ip[1] << '.' << +peer_ip[2] << '.' << +peer_ip[3] << ':'
              << ntohs(peer_address.sin_port) << std::endl;
}
void Server::handlePeerEvent(const epoll_event &ev)
{
    std::map<int, ServerClient>::iterator it = _clients.find(ev.data.fd);
    if (it == _clients.end())
        Terminate(), throw ServerException("Client not found.", *this);
    std::cout << "Handling the peer event on fd " << ev.data.fd << std::endl;
    switch (ev.events)
    {
    case EPOLLIN: {
        std::cout << "Event type is EPOLLIN.\n";
        char    buff[1024];
        ssize_t bytes = recv(ev.data.fd, buff, 1023, 0);
        if (bytes < 0)
            Terminate(), throw ServerException("recv() failed.", *this);
        if (bytes == 0)
            return RemoveClient(ev);
        buff[bytes] = 0;
        it->second.PushContent(buff);
        break;
    }
    case EPOLLOUT: {
        std::map<int, std::queue<Request> >::iterator req_it = _requests.find(ev.data.fd);
        if (req_it == _requests.end())
            Terminate(), throw ServerException("Client request queue not found.", *this);
        /*
        The following code should go through IOmultplxr too.
        */
        /*======================================================*/
        {
            if (req_it->second.size() == 0)
                Terminate(), throw ServerException("Request queue for client is empty.", *this);
            Request req = req_it->second.front();
            req_it->second.pop();
            /*
            here should check the uri path
            to assign appropriate location.
            */
            std::string file_name = _config.locations.front().root;
            file_name += req.getUri();
            int file_fd = open(file_name.c_str(), O_RDONLY);
            if (file_fd < 0)
                Terminate(), throw ServerException("open() failed for file: " + file_name, *this);
            std::string content;
            char        buff[1024];
            while (1)
            {
                int bytes = read(file_fd, buff, 1023);
                if (bytes < 0)
                    Terminate(), throw ServerException("open() failed for file: " + file_name, *this);
                if (bytes == 0)
                    break;
                buff[bytes] = 0;
                content += buff;
            }
            send(ev.data.fd, content.c_str(), content.size(), 0);
        }
        /*======================================================*/
        break;
    }
    default:
        Terminate(), throw ImpossibleToReach();
        break;
    }
}
void Server::AddRequest(const ServerClient &client, const Request &request)
{
    assert(request.getMethod() == HTTP_GET);
    /* Need To be removed from all occurence
    the serching for the client below is now here just for debugging purposes
    it must be unneccesary to check if the client exist every time
    or should we?? huh
    */
    int                                   client_fd = client.Getfd();
    std::map<int, ServerClient>::iterator it        = _clients.find(client_fd);
    if (it == _clients.end())
        Terminate(), throw ServerException("Can't add request, Client not found.", *this);
    epoll_event ev;
    ev.data.ptr = this;
    ev.events   = EPOLLOUT;
    _IOmltplx->AddEvent(ev, client_fd);
    std::map<int, std::queue<Request> >::iterator req_it = _requests.find(client_fd);
    if (req_it == _requests.end())
    {
        std::queue<Request> queue;
        queue.push(request);
        _requests.insert(std::pair<int, std::queue<Request> >(client_fd, queue));
        return;
    }
    req_it->second.push(request);
    std::cout << "Request from client fd: " << client_fd << " added successfuly." << std::endl;
}
void Server::RemoveClient(epoll_event ev)
{
    std::map<int, ServerClient>::iterator it = _clients.find(ev.data.fd);
    if (it == _clients.end())
        Terminate(), throw ServerException("Can't remove Client, peer not found.", *this);
    _clients.erase(it);
    _IOmltplx->RemoveEvent(ev, ev.data.fd);
    if (ev.data.fd >= 0)
        close(ev.data.fd);
    std::cout << "Client fd: " << ev.data.fd << " disconnected." << std::endl;
}
void Server::Terminate()
{
    if (_is_started == false)
        return;
    _is_started = false;
    _IOmltplx->RemoveEvent(_listen_socket_ev, _listen_socket_fd);
    if (_listen_socket_fd >= 0)
        close(_listen_socket_fd);
    /*
    Loop through connected clients and disconnect them + remove them from
    the _clients map
    */
    std::cout << "Server terminated.\n";
}
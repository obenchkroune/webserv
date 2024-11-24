/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:26:41 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/24 13:31:43 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

sockaddr_in
Server::get_listen_addr(ServerConfig& _config)
{
    sockaddr_in  address;
    std::string& ip = _config.host;
    {
        for (size_t dot, pos = 0, i = 0; i < 4; i++, pos = dot + 1)
        {
            if ((dot = ip.find(".", pos)) == std::string::npos)
            {
                if (i < 3)
                    Terminate(),
                      throw ServerException("Bad ip address string.", *this);
                else
                    dot = ip.length();
            }
            if (dot - pos > 3 || dot - pos <= 0)
                Terminate(),
                  throw ServerException("Bad ip address string.", *this);
            std::string raw_byte = ip.substr(pos, dot);

            uint8_t  byte       = std::atoi(raw_byte.c_str());
            uint8_t* ip_address = (uint8_t*)&address.sin_addr.s_addr;
            ip_address[i]       = byte;
        }
    }
    address.sin_port   = htons(_config.port);
    address.sin_family = AF_INET;
    return address;
}

Server::Server(const ServerConfig& config,
               IOMultiplexer*      IOmltplx,
               bool                start /*= false*/)
  : _config(config)
  , _is_started(false)
  , _IOmltplx(IOmltplx)
{
    if (_IOmltplx == NULL)
        throw ServerException("I/O Multiplexer object is set to NULL\n", *this);
    _listen_addr             = get_listen_addr(_config);
    _listen_socket_ev.events = EPOLLIN;
    if (start)
        Start();
}
Server::Server(const Server& server)
  : AIOEventListener(server)
{
    *this = server;
}
Server&
Server::operator=(const Server& server)
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
bool
Server::is_started() const
{
    return _is_started;
}
const ServerConfig&
Server::GetConfig() const
{
    return _config;
}
void
Server::Start()
{
    if (_is_started)
        throw ServerException("Server is already started.", *this);
    _is_started       = true;
    _listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_socket_fd == -1)
        throw ServerException("socket(): failed.", *this);
    int active = 1;
    if (setsockopt(
          _listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &active, sizeof(int)) ==
        -1)
        Terminate(), throw ServerException("setsockopt(): failed.", *this);
    if (bind(_listen_socket_fd,
             (sockaddr*)&_listen_addr,
             sizeof(_listen_addr)) == -1)
        Terminate(), throw ServerException("bind(): failed.", *this);
    if (listen(_listen_socket_fd, __INT32_MAX__) == -1)
        Terminate(), throw ServerException("listen(): failed.", *this);
    _listen_socket_ev.data.ptr = this;
    _IOmltplx->AddEvent(_listen_socket_ev, _listen_socket_fd);
    uint8_t* ip = (uint8_t*)&_listen_addr.sin_addr.s_addr;
    std::cout << "Server started on listening succefully.\n";
    std::cout << "Address: " << +ip[0] << '.' << +ip[1] << '.' << +ip[2] << '.'
              << +ip[3] << ':' << ntohs(_listen_addr.sin_port) << '\n';
}
void
Server::ConsumeEvent(const epoll_event ev)
{
    std::cout << "[Server: " << _config.host << ':' << _config.port
              << "]: New event." << std::endl;
    if (ev.data.fd == _listen_socket_fd)
    {
        std::cout << "Listening socket event." << std::endl;
        sockaddr_in peer_address;
        socklen_t   peer_address_len = sizeof(peer_address);
        int         peer_fd          = accept(_listen_socket_fd,
                             (sockaddr*)&peer_address,
                             &peer_address_len);
        if (peer_fd == -1)
            Terminate(), throw ServerException(
                           "accept(): failed to accept new connection.", *this);
        std::map<int, ServerClient>::iterator it = _clients.find(peer_fd);
        if (it == _clients.end())
            _clients.insert(std::pair<int, ServerClient>(peer_fd, peer_fd));
        else
            Terminate(),
              throw ServerException("fd already exists in clients.", *this);
        epoll_event p_ev;
        p_ev.events   = EPOLLIN;
        p_ev.data.ptr = this;
        _IOmltplx->AddEvent(p_ev, peer_fd);
        uint8_t* peer_ip = (uint8_t*)&peer_address.sin_addr.s_addr;
        std::cout << "new peer accepted on fd " << peer_fd << ".\n";
        std::cout << "Address: " << +peer_ip[0] << '.' << +peer_ip[1] << '.'
                  << +peer_ip[2] << '.' << +peer_ip[3] << ':'
                  << ntohs(peer_address.sin_port) << std::endl;
        return;
    }
    else
    {
        std::map<int, ServerClient>::iterator it = _clients.find(ev.data.fd);
        if (it == _clients.end())
            Terminate(), throw ServerException("Client not found.", *this);
        std::cout << "Peer event on fd " << ev.data.fd << std::endl;
        switch (ev.events)
        {
        case EPOLLIN: {
            std::cout << "Event type is EPOLLIN.\n";
            char    buff[1024];
            ssize_t bytes = recv(ev.data.fd, buff, 1023, 0);
            if (bytes < 0)
                Terminate(), throw ServerException("recv() failed.", *this);
            if (bytes == 0)
                RemoveClient(ev);
            buff[bytes] = 0;
            std::cout << "Content:\n[START OF CONTENT]\n"
                      << buff << "[END OF CONTENT]\n";
            return;
        }

        case EPOLLOUT: {
            Terminate(), throw NotImplemented();
            break;
        }

        default:
            Terminate(), throw ImpossibleToReach();
            break;
        }
    }
    std::cerr << "[Server: " << _config.host << ':' << _config.port
              << "]: Warning: event has an uknown fd: " << ev.data.fd
              << " event is ignored." << std::endl;
}
void
Server::RemoveClient(epoll_event ev)
{
    std::map<int, ServerClient>::iterator it = _clients.find(ev.data.fd);
    if (it == _clients.end())
        Terminate(),
          throw ServerException("Can't remove Client, peer not found.", *this);
    _clients.erase(it);
    _IOmltplx->RemoveEvent(ev, ev.data.fd);
    if (ev.data.fd >= 0)
        close(ev.data.fd);
    std::cout << "Client fd: " << ev.data.fd << " disconnected." << std::endl;
}
void
Server::Terminate()
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
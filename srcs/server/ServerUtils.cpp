/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:15:54 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/29 23:07:08 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerUtils.hpp"
#include "Server.hpp"
#include "ServerClient.hpp"
#include <cassert>
#include <climits>
#include <iostream>
#include <libgen.h>

namespace ServerUtils
{
sockaddr_in GetListenAddr(const ServerConfig& _config)
{
    sockaddr_in address;
    std::string host = _config.host;

    address.sin_family = AF_INET;
    address.sin_port   = htons(_config.port);
    if (host.empty() || host == "0.0.0.0")
    {
        address.sin_addr.s_addr = INADDR_ANY;
        return address;
    }
    if (host == "localhost" || host == "127.0.0.1")
    {
        address.sin_addr.s_addr = INADDR_LOOPBACK;
        return address;
    }
    {
        for (size_t dot, pos = 0, i = 0; i < 4; i++, pos = dot + 1)
        {
            if ((dot = host.find(".", pos)) == std::string::npos)
            {
                if (i < 3)
                    throw ServerException("Bad host address string.");
                else
                    dot = host.length();
            }
            if (dot - pos > 3 || dot - pos <= 0)
                throw ServerException("Bad host address string.");
            std::string raw_byte = host.substr(pos, dot);

            uint8_t  byte       = std::atoi(raw_byte.c_str());
            uint8_t* ip_address = (uint8_t*)&address.sin_addr.s_addr;
            ip_address[i]       = byte;
        }
    }
    return address;
}
void PrintSocketIP(std::ostream& os, const sockaddr_in& address)
{
    const uint8_t* IP = (uint8_t*)&address.sin_addr.s_addr;
    os << +IP[0] << '.' << +IP[1] << '.' << +IP[2] << '.' << +IP[3] << ':'
       << ntohs(address.sin_port) << std::endl;
}
bool validateFileLocation(const std::string& location_root, const std::string& file_path)
{
    char buff[PATH_MAX];
    realpath(file_path.c_str(), buff);
    if (std::strncmp(location_root.c_str(), buff, std::strlen(location_root.c_str())) == 0)
    {
        const char last_char = buff[std::strlen(location_root.c_str())];
        return last_char == '\0' || last_char == '/';
    }
    return false;
}
VirtualServerIterator GetRequestVirtualServer(const int& address_fd, const Request& request)
{
    std::vector<VirtualServerIterator>        matched_servers;
    std::vector<ServerConfig>::const_iterator vservers_it =
        Server::GetInstance().GetConfig().begin();
    for (; vservers_it != Server::GetInstance().GetConfig().end(); vservers_it++)
    {
        if (vservers_it->listen_address_fd == address_fd)
            matched_servers.insert(matched_servers.end(), vservers_it);
    }
    assert(matched_servers.size() > 0 && "IMPOSSIBLE");
    if (matched_servers.size() == 1)
        return matched_servers.front();
    const HttpHeader* host_header = request.getHeader("Host");
    if (host_header == NULL)
        return matched_servers.front();
    std::vector<VirtualServerIterator>::const_iterator matched_servers_it = matched_servers.begin();
    for (; matched_servers_it != matched_servers.end(); matched_servers_it++)
    {
        std::vector<std::string>::const_iterator names_it =
            (*matched_servers_it)->server_names.begin();
        for (; names_it != (*matched_servers_it)->server_names.end(); names_it++)
            if (*names_it == host_header->values.front().value)
                return *matched_servers_it;
    }
    return matched_servers.front();
}
LocationIterator GetFileLocation(VirtualServerIterator config, const std::string& fname)
{
    LocationIterator matched_location = config->locations.end();
    LocationIterator location_it      = config->locations.begin();
    int              matching_record  = 0;
    for (; location_it != config->locations.end(); location_it++)
    {
        const std::string& loc_path = location_it->path;
        assert(fname[0] == '/' && loc_path[0] == '/');
        int matching_score = 0;
        for (size_t i = 1; i < loc_path.length() && i < fname.length(); i++)
        {
            if (loc_path[i] != fname[i])
                break;
            if (loc_path[i] == '/')
                matching_score++;
            else if (i == loc_path.length() - 1 && i == fname.length() - 1)
                matching_score++;
            else if (i == loc_path.length() - 1 && fname[i + 1] == '/')
                matching_score++;
        }
        if ((matching_record == 0 && loc_path == "/") || (matching_record < matching_score))
        {
            matched_location = location_it;
            matching_record  = matching_score;
        }
    }
    return matched_location;
}
} // namespace ServerUtils

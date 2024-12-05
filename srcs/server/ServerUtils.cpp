/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:15:54 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/05 02:36:33 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerClient.hpp"
#include "ServerUtils.hpp"
#include <cassert>
#include <iostream>
#include <libgen.h>
#include <netinet/in.h>

namespace ServerUtils
{
sockaddr_in GetListenAddr(const ServerConfig &_config)
{
    sockaddr_in address;
    std::string host = _config.host;

    address.sin_family = AF_INET;
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
            uint8_t *ip_address = (uint8_t *)&address.sin_addr.s_addr;
            ip_address[i]       = byte;
        }
    }
    address.sin_port = htons(_config.port);
    return address;
}
inline void PrintSocketIP(std::ostream &os, const sockaddr_in &address)
{
    const uint8_t *IP = (uint8_t *)&address.sin_addr.s_addr;
    os << +IP[0] << '.' << +IP[1] << '.' << +IP[2] << '.' << +IP[3] << ':' << ntohs(address.sin_port) << std::endl;
}
bool validateFileLocation(const std::string &location_root, const std::string &fname)
{
    if (fname.find("/..") == std::string::npos)
        return true;
    int curr_directory_relative_to_root = 0;
    int directory_name_len              = 0;
    int dots_count                      = 0;
    for (size_t i = location_root.length() + 1; i < fname.length(); i++)
    {
        if (i + 1 >= fname.length() || fname[i] == '/')
        {
            if (dots_count == 2 && directory_name_len == 0)
                curr_directory_relative_to_root--;
            else if (directory_name_len > 0)
                curr_directory_relative_to_root++;
            dots_count         = 0;
            directory_name_len = 0;
        }
        else if (fname[i] == '.')
            dots_count++;
        else
            directory_name_len++;
        if (curr_directory_relative_to_root < 0)
            return false;
    }
    return true;
}
std::vector<LocationConfig>::const_iterator GetFileLocation(const ServerConfig &config, const std::string &fname)
{
    std::vector<LocationConfig>::const_iterator matched_location = config.locations.begin();
    std::vector<LocationConfig>::const_iterator loc_it           = matched_location + 1;
    size_t                                      matching_record  = 0;
    for (; loc_it != config.locations.end(); loc_it++)
    {
        const std::string &loc_path = loc_it->path;
        assert(fname[0] == '/' && loc_path[0] == '/');
        size_t matching_score = 0;
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
        if (matching_record < matching_score)
        {
            matched_location = loc_it;
            matching_record  = matching_score;
        }
    }
    return matched_location;
}
} // namespace ServerUtils
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:16:12 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/07 15:30:11 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Config.hpp"
#include "Request.hpp"
#include <netinet/in.h>
#include <string>

namespace ServerUtils
{
sockaddr_in GetListenAddr(const ServerConfig &_config);
void        PrintSocketIP(std::ostream &os, const sockaddr_in &address);
bool        validateFileLocation(const std::string &location_root, const std::string &fname);
std::vector<LocationConfig>::const_iterator GetFileLocation(const ServerConfig &config, const std::string &fname);
const ServerConfig                         &GetRequestVirtualServer(const int &address_fd, const Request &request,
                                                                    const std::vector<ServerConfig> &config);
} // namespace ServerUtils
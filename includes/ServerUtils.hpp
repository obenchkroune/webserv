/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: simo <simo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:16:12 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/08 15:13:11 by simo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <string>

namespace ServerUtils
{
void        SendErrorResponse(const HttpStatus& status, Response* response);
sockaddr_in GetListenAddr(const ServerConfig& _config);
void        PrintSocketIP(std::ostream& os, const sockaddr_in& address);
bool        validateFileLocation(const std::string& location_root, const std::string& fname);
std::vector<LocationConfig>::const_iterator GetFileLocation(
    const ServerConfig& config, const std::string& fname
);
const ServerConfig& GetRequestVirtualServer(
    const int& address_fd, const Request& request, const std::vector<ServerConfig>& config
);
} // namespace ServerUtils
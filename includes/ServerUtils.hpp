/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:16:12 by msitni1337        #+#    #+#             */
/*   Updated: 2025/01/29 17:03:01 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Config.hpp"
#include "Response.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <string>

namespace ServerUtils
{
sockaddr_in GetListenAddr(const ServerConfig& _config);
void        PrintSocketIP(std::ostream& os, const sockaddr_in& address);
bool        validateFileLocation(const std::string& location_root, const std::string& file_path);
VirtualServerIterator GetRequestVirtualServer(const int& address_fd, const Request& request);
LocationIterator      GetFileLocation(VirtualServerIterator config, const std::string& fname);
} // namespace ServerUtils
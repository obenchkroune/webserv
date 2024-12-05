/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:16:12 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/05 11:51:43 by msitni           ###   ########.fr       */
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
void PrintSocketIP(std::ostream &os, const sockaddr_in &address);
bool        validateFileLocation(const std::string &location_root, const std::string &fname);
std::vector<LocationConfig>::const_iterator GetFileLocation(const ServerConfig &config, const std::string &fname);
const ServerConfig *GetRequestVServer(const Request &request, const std::vector<ServerConfig> &config);
} // namespace ServerUtils
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:27:14 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/20 23:31:36 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ConfigParser.h"
class Server
{
private:
    ConfigParser& _ConfigParser;
public:
    Server(const ConfigParser& config_parser);
    Server(const Server& server);
    Server& operator=(const Server& server);
    ~Server();
};
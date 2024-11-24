/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:43:52 by msitni            #+#    #+#             */
/*   Updated: 2024/11/24 02:10:03 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"
#include "Server.hpp"

ServerException::ServerException(std::string reason, const Server& server) throw()
    : _reason("ServerException::ServerException\n[Reason]: " + reason), _server(server)
{
    const ServerConfig& conf = _server.GetConfig();
    if (conf.server_names.size())
        _reason = _reason + std::string("\nserver name : ") + conf.server_names[0];
    else
        _reason = _reason + std::string("\nserver name : NO_NAME");
}
const char* ServerException::what() const throw()
{
    return _reason.c_str();
}
ServerException::~ServerException() throw() {}
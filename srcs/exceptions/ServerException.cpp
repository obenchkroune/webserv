/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:43:52 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 11:57:15 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"
#include "Server.hpp"

ServerException::ServerException(std::string reason, const Server& server) throw()
    : _reason("[Reason]: " + reason), _server(server)
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
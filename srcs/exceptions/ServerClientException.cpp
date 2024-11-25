/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClientException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:43:52 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 17:13:33 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"
#include "Server.hpp"

ServerClientException::ServerClientException(std::string reason) throw()
    : _reason("ServerClientException::ServerClientException : [Reason]: " + reason)
{}
const char* ServerClientException::what() const throw()
{
    return _reason.c_str();
}
ServerClientException::~ServerClientException() throw() {}
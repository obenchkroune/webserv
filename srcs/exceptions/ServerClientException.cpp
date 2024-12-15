/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClientException.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:43:52 by msitni            #+#    #+#             */
/*   Updated: 2024/12/05 02:35:37 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"
#include "Server.hpp"

ServerClientException::ServerClientException(std::string reason) throw()
    : _reason("[ServerClientException::ServerClientException] [Reason]: " + reason)
{}
const char* ServerClientException::what() const throw()
{
    return _reason.c_str();
}
ServerClientException::~ServerClientException() throw() {}
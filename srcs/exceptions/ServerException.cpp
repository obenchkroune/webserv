/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:43:52 by msitni            #+#    #+#             */
/*   Updated: 2024/12/05 02:34:49 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"
#include "Server.hpp"

ServerException::ServerException(std::string reason) throw()
    : _reason("[ServerException::ServerException] [Reason]: " + reason)
{
}
const char* ServerException::what() const throw()
{
    return _reason.c_str();
}
ServerException::~ServerException() throw() {}
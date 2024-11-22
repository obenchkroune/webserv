/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOMultiplexerException.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 18:16:15 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 18:17:32 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"

IOMultiplexerException::IOMultiplexerException(std::string reason) throw()
    : _reason("IOMultiplexerException::IOMultiplexerException\n[Reason]: " + reason)
{
}
const char* IOMultiplexerException::what() const throw()
{
    return _reason.c_str();
}
IOMultiplexerException::~IOMultiplexerException() throw() {}
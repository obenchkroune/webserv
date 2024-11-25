/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOMultiplexerException.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 18:16:15 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/25 11:57:04 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"

IOMultiplexerException::IOMultiplexerException(std::string reason) throw()
    : _reason("[Reason]: " + reason)
{
}
const char* IOMultiplexerException::what() const throw()
{
    return _reason.c_str();
}
IOMultiplexerException::~IOMultiplexerException() throw() {}
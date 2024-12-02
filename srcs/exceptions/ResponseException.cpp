/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseException.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 22:16:38 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/26 22:17:01 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

ResponseException::ResponseException(std::string reason) throw()
    : _reason("ResponseException::ResponseException : [Reason]: " + reason)
{}
const char* ResponseException::what() const throw()
{
    return _reason.c_str();
}
ResponseException::~ResponseException() throw() {}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerException.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:43:52 by msitni            #+#    #+#             */
/*   Updated: 2024/11/22 11:45:38 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exceptions.hpp"

ServerException::ServerException(std::string reason) throw()
    : _reason("ServerException::ServerException\n[Reason]: " + reason)
{
}
const char* ServerException::what() const throw()
{
    return _reason.c_str();
}
ServerException::~ServerException() throw() {}
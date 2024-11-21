/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 20:42:12 by msitni            #+#    #+#             */
/*   Updated: 2024/11/21 22:11:28 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <exception>

class NotImplemented : public std::exception  // is it that hard to just assert something heh? instead of 
{
public:
    const char* what() const throw();
};

class ServerExceptionAlreadyStarted : public std::exception
{
public:
    const char* what() const throw();
};

class ServerExceptionListenAddrNotFound : public std::exception
{
public:
    const char* what() const throw();
};

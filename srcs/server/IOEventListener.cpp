/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOEventListener.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:45:27 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 23:03:16 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IOEventListener.hpp"

AIOEventListener::AIOEventListener() {}
AIOEventListener::AIOEventListener(const AIOEventListener& event)
{
    (void)event;
}
AIOEventListener& AIOEventListener::operator=(const AIOEventListener& event)
{
    (void)event;
    return *this;
}
AIOEventListener::~AIOEventListener() {}
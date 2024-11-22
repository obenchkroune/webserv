/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOEvent.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:45:27 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 19:14:39 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IOEvent.hpp"

AIOEvent::AIOEvent() {}
AIOEvent::AIOEvent(const AIOEvent& event)
{
    (void)event;
}
AIOEvent& AIOEvent::operator=(const AIOEvent& event)
{
    (void)event;
    return *this;
}
AIOEvent::~AIOEvent() {}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOEvent.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:45:27 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 22:21:09 by msitni           ###   ########.fr       */
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
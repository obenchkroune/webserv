/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOEvent.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:37:18 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 19:14:45 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/epoll.h>

class AIOEvent
{
private:
public:
    AIOEvent();
    AIOEvent(const AIOEvent& event);
    AIOEvent& operator=(const AIOEvent& event);
    virtual ~AIOEvent();

public:
    virtual void ConsumeEvent(const epoll_event ev) = 0;
};
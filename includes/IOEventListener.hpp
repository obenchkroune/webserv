/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOEventListener.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:37:18 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/03 23:51:46 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/epoll.h>

class AIOEventListener
{
private:
public:
    AIOEventListener();
    AIOEventListener(const AIOEventListener& event);
    AIOEventListener& operator=(const AIOEventListener& event);
    virtual ~AIOEventListener();

public:
    virtual void ConsumeEvent(const epoll_event ev) = 0;
    virtual void Terminate()                        = 0;
};
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOEventListener.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 17:37:18 by msitni1337        #+#    #+#             */
/*   Updated: 2024/11/22 21:00:33 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/epoll.h>

class AIOEventListener
{
private:
public:
    AIOEventListener();
    AIOEventListener(const AIOEventListener &event);
    AIOEventListener &operator=(const AIOEventListener &event);
    virtual ~AIOEventListener();

public:
    virtual void ConsumeEvent(const epoll_event ev) = 0;
};
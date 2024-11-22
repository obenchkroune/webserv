/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:46:53 by msitni            #+#    #+#             */
/*   Updated: 2024/11/22 11:53:45 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

size_t ft_strlcpy(char* dst, const char* src, size_t sz)
{
    size_t i = 0;

    if (dst == NULL || src == NULL)
        return 0;
    if (!sz)
        return (std::strlen(src));
    while (src[i] && i < sz - 1)
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
    return (std::strlen(src));
}
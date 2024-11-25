/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:46:53 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 11:56:41 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include <sstream>
#include <vector>

namespace Utils
{

std::vector<std::string> ft_split(const std::string &str, char delim)
{
    std::vector<std::string> result;
    std::istringstream       iss(str);

    std::string token;
    while (std::getline(iss, token, delim))
        result.push_back(token);

    return result;
}

std::string ft_strtrim(const std::string &str)
{
    std::string::const_iterator         it  = str.begin();
    std::string::const_reverse_iterator rit = str.rbegin();

    while (it != str.end() && std::isspace(*it))
        it++;
    while (rit.base() > it && std::isspace(*rit))
        rit++;
    return std::string(it, rit.base());
}

size_t ft_strlcpy(char *dst, const char *src, size_t sz)
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

} // namespace Utils

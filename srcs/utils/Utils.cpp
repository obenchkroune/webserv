/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.cpp                                          :+:      :+:    :+:   */
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

namespace util {

std::string ft_strtrim(const std::string& str) {
    std::string::const_iterator         it  = str.begin();
    std::string::const_reverse_iterator rit = str.rbegin();

    while (it != str.end() && std::isspace(*it))
        it++;
    while (rit.base() > it && std::isspace(*rit))
        rit++;
    return std::string(it, rit.base());
}

} // namespace util

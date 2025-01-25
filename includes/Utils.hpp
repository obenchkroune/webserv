/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni <msitni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 11:47:32 by msitni            #+#    #+#             */
/*   Updated: 2024/11/25 11:36:41 by msitni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "Utils.tpp"

namespace utils
{

std::string strtrim(const std::string& str);
std::string quoted(const std::string& str, bool double_quotes = false);
char	*strnstr(const char *big, const char *little, size_t len);

} // namespace utils

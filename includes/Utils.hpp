/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
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

namespace Utils
{

std::string              ft_strtrim(const std::string &str);
std::vector<std::string> ft_split(const std::string &str, char delim);
size_t                   ft_strlcpy(char *dst, const char *src, size_t sz);

} // namespace Utils

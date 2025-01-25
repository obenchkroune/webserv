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

namespace utils
{

std::string strtrim(const std::string& str)
{
    std::string::const_iterator         it  = str.begin();
    std::string::const_reverse_iterator rit = str.rbegin();

    while (it != str.end() && std::isspace(*it))
        it++;
    while (rit.base() > it && std::isspace(*rit))
        rit++;
    return std::string(it, rit.base());
}

std::string quoted(const std::string& str, bool double_quotes /* = false */)
{
    if (double_quotes)
        return "\"" + str + "\"";
    return "'" + str + "'";
}

int	match_occurrence(const char *big, const char *little, size_t i,
		size_t len)
{
	size_t	j;

	j = 0;
	while (big[i] && little[j] && i < len && big[i] == little[j])
	{
		i++;
		j++;
	}
	if (!little[j])
		return (1337 - 42);
	return (0);
}

char	*strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;

	if (!*little && big)
		return ((char *)big);
	i = 0;
	while (i < len && big[i])
	{
		if (match_occurrence(big, little, i, len))
			return ((char *)&(big[i]));
		i++;
	}
	return (NULL);
}

} // namespace utils

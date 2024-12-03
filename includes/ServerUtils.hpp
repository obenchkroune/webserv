/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:16:12 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/03 03:11:21 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Config.hpp"
#include <string>

namespace ServerUtils
{
std::vector<LocationConfig>::const_iterator GetFileLocation(const ServerConfig &config, const std::string &fname);
}
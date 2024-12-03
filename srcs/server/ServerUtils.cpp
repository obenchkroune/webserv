/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msitni1337 <msitni1337@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 00:15:54 by msitni1337        #+#    #+#             */
/*   Updated: 2024/12/03 22:16:05 by msitni1337       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerUtils.hpp"
#include <cassert>
#include <libgen.h>

namespace ServerUtils
{
bool validateFileLocation(const std::string &location_root, const std::string &fname)
{
    assert(fname[location_root.length()] == '/');
    if (fname.find("/..") == std::string::npos)
        return true;
    int curr_directory_relative_to_root = 0;
    int directory_name_len              = 0;
    int dots_count                      = 0;
    for (size_t i = location_root.length() + 1; i < fname.length(); i++)
    {
        if (i + 1 >= fname.length() || fname[i] == '/')
        {
            if (dots_count == 2 && directory_name_len == 0)
                curr_directory_relative_to_root--;
            else if (directory_name_len > 0)
                curr_directory_relative_to_root++;
            dots_count         = 0;
            directory_name_len = 0;
        }
        else if (fname[i] == '.')
            dots_count++;
        else
            directory_name_len++;
        if (curr_directory_relative_to_root < 0)
            return false;
    }
    return true;
}
std::vector<LocationConfig>::const_iterator GetFileLocation(const ServerConfig &config, const std::string &fname)
{
    std::vector<LocationConfig>::const_iterator matched_location = config.locations.begin();
    std::vector<LocationConfig>::const_iterator loc_it           = matched_location + 1;
    size_t                                      matching_record  = 0;
    for (; loc_it != config.locations.end(); loc_it++)
    {
        const std::string &loc_path = loc_it->path;
        assert(fname[0] == '/' && loc_path[0] == '/');
        size_t matching_score = 0;
        for (size_t i = 1; i < loc_path.length() && i < fname.length(); i++)
        {
            if (loc_path[i] != fname[i])
                break;
            if (loc_path[i] == '/')
                matching_score++;
            else if (i == loc_path.length() - 1 && i == fname.length() - 1)
                matching_score++;
            else if (i == loc_path.length() - 1 && fname[i + 1] == '/')
                matching_score++;
        }
        if (matching_record < matching_score)
        {
            matched_location = loc_it;
            matching_record  = matching_score;
        }
    }
    return matched_location;
}
} // namespace ServerUtils
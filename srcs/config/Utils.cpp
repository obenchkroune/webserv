#include "./Utils.h"
#include <sstream>

namespace Utils
{

std::vector<std::string> ft_split(const std::string& str, char delim)
{
    std::vector<std::string> result;
    std::istringstream       iss(str);

    std::string token;
    while (std::getline(iss, token, delim))
        result.push_back(token);

    return result;
}

} // namespace Utils

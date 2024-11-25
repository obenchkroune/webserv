#include "Config.hpp"

const char *InvalidConfigException::what() const throw()
{
    return "Invalid Configuration";
}
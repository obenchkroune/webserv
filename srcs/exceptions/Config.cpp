#include "Config.hpp"

InvalidConfigException::InvalidConfigException(const std::string &message)
    : _message("invalid Configuration: invalid syntax near: " + message)
{
}

InvalidConfigException::~InvalidConfigException() throw()
{
}

const char *InvalidConfigException::what() const throw()
{
    return _message.c_str();
}

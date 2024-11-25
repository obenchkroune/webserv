#include "Request.hpp"
#include <exception>

RequestException::RequestException(const std::string &message) : _message(message)
{
}

RequestException::~RequestException() throw()
{
}

const char *RequestException::what() const throw()

{
    return _message.c_str();
}
#include "Request.hpp"
#include <exception>

RequestException::RequestException(const HttpStatus& error_code) : _error_code(error_code) {}

RequestException::~RequestException() throw() {}

const char* RequestException::what() const throw()
{
    return _error_code.message;
}

HttpStatus RequestException::getErrorCode() const
{
    return _error_code;
}

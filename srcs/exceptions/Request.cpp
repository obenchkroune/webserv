#include "Request.hpp"
#include <exception>

RequestException::RequestException(const HttpStatus& error_code, std::string line) : _error_code(error_code), _line(line) {}

RequestException::~RequestException() throw() {}

const char* RequestException::what() const throw()
{
    return _line.c_str();
}

HttpStatus RequestException::getErrorCode() const
{
    return _error_code;
}

#include "Request.hpp"
#include <exception>

RequestException::RequestException(const std::string& message) : _message(message) {
}

RequestException::RequestException(uint16_t error_code, const std::string& message) {
    std::stringstream ss;
    ss << error_code << ": " << message;
    _message = ss.str();
}

RequestException::~RequestException() throw() {
}

const char* RequestException::what() const throw() {
    return _message.c_str();
}

uint16_t RequestException::getErrorCode() const {
    return _error_code;
}

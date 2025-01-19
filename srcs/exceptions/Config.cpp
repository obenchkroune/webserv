#include "Config.hpp"
#include <sstream>

InvalidConfigException::InvalidConfigException(const std::string& token)
    : _message("invalid Configuration: invalid syntax near " + token)
{
}

InvalidConfigException::InvalidConfigException(std::size_t line)
{
    std::stringstream iss;

    iss << "invalid Configuration: unexpected token at line " << line;
    _message = iss.str();
}

InvalidConfigException::InvalidConfigException(ConfigLexer& lexer)
{
    std::stringstream iss;

    iss << "invalid Configuration: unexpected token at line " << lexer.getCurrentLine() << " near "
        << lexer.peek().value;
    _message = iss.str();
}

InvalidConfigException::~InvalidConfigException() throw() {}

const char* InvalidConfigException::what() const throw()
{
    return _message.c_str();
}

#include "ConfigLexer.hpp"
#include <fstream>
#include <iostream>
#include <limits>

ConfigLexer::ConfigLexer(const std::string &file) : _current_line(1)
{
    std::ifstream ifs(file.c_str());
    if (!ifs.is_open())
        throw std::runtime_error("could not open config file: " + file);

    while (ifs && !ifs.eof())
    {
        char c = ifs.peek();
        switch (c)
        {
        case '#':
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            _current_line++;
            break;
        case '{':
            _tokens.push_back(Token(T_BLOCK_START, ifs.get(), _current_line));
            break;
        case '}':
            _tokens.push_back(Token(T_BLOCK_END, ifs.get(), _current_line));
            break;
        case ';':
            _tokens.push_back(Token(T_SEMICOL, ifs.get(), _current_line));
            break;
        case '\n':
            ifs.ignore();
            _current_line++;
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\v':
        case '\f':
            ifs.ignore();
            break;
        default:
            std::string reserved("#{};");
            std::string value;
            while (ifs.good())
            {
                char c = ifs.peek();
                if (reserved.find(c) != std::string::npos || std::isspace(c))
                    break;
                value += ifs.get();
            }
            if (!value.empty())
            {
                _tokens.push_back(Token(T_WORD, value));
                _tokens.back().line_number = _current_line;
            }
        }
    }
    _tokens.push_back(Token(T_EOF));
    _tokens.back().line_number = _current_line;
    _current                   = _tokens.begin();
}

ConfigLexer::~ConfigLexer()
{
    //
}

const Token &ConfigLexer::peek() const
{
    return *_current;
}

const Token &ConfigLexer::getNext()
{
    if (_current->type != T_EOF && _current != _tokens.end())
        return *_current++;
    return *_current;
}

const Token &ConfigLexer::rpeek()
{
    if (_current == _tokens.begin())
        return *_current;
    return *(_current - 1);
}

const Token &ConfigLexer::expect(Token token)
{
    const Token &tok = this->getNext();
    if (token == tok)
        return tok;
    throw UnexpectedTokenException();
}

const Token &ConfigLexer::expect(TokenType type)
{
    const Token &tok = this->getNext();
    if (type == tok.type)
        return tok;
    throw UnexpectedTokenException();
}

std::size_t ConfigLexer::getCurrentLine() const
{
    return _current_line;
}

const char *ConfigLexer::UnexpectedTokenException::what() const throw()
{
    return "Unexpected token.";
}

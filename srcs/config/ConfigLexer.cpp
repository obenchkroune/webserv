#include "ConfigLexer.h"
#include <fstream>
#include <iostream>
#include <limits>

ConfigLexer::ConfigLexer(const std::string& file)
{
    std::ifstream ifs(file.c_str());
    if (!ifs.is_open())
        throw std::runtime_error("could not open config file: " + file);

    while (ifs >> std::ws && !ifs.eof())
    {
        char c = ifs.peek();
        switch (c)
        {
        case '#':
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        case '{':
            _tokens.push_back(Token(T_BLOCK_START, ifs.get()));
            break;
        case '}':
            _tokens.push_back(Token(T_BLOCK_END, ifs.get()));
            break;
        case ';':
            _tokens.push_back(Token(T_SEMICOL, ifs.get()));
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
            _tokens.push_back(Token(T_WORD, value));
        }
    }
    _tokens.push_back(Token(T_EOF));
    _current = _tokens.begin();
}

ConfigLexer::~ConfigLexer()
{
    //
}

const Token ConfigLexer::peek() const
{
    return *_current;
}

const Token ConfigLexer::getNext()
{
    if (_current->type != T_EOF && _current != _tokens.end())
        return *_current++;
    return *_current;
}

const Token ConfigLexer::expect(Token token)
{
    if (token == *_current)
        return this->getNext();
    throw UnexpectedTokenException();
}

const Token ConfigLexer::expect(TokenType type)
{
    if (type == _current->type)
        return this->getNext();
    throw UnexpectedTokenException();
}

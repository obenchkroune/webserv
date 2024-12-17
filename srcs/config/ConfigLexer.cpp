#include "ConfigLexer.hpp"
#include <fstream>
#include <iostream>
#include <limits>

ConfigLexer::ConfigLexer(const std::string& file) : _ifs(file.c_str()), _current_line(1) {
    if (!_ifs.is_open())
        throw std::runtime_error("could not open config file: " + file);
}

ConfigLexer::~ConfigLexer() {
}

const Token ConfigLexer::peek() {
    std::ifstream::pos_type pos   = _ifs.tellg();
    Token                   token = getNextToken();
    _ifs.seekg(pos);
    return token;
}

const Token ConfigLexer::getNextToken() {
    while (_ifs && !_ifs.eof()) {
        char c = _ifs.peek();
        switch (c) {
        case '#':
            _ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            _current_line++;
            break;
        case '{':
            return Token(T_BLOCK_START, _ifs.get(), _current_line);
        case '}':
            return Token(T_BLOCK_END, _ifs.get(), _current_line);
        case ';':
            return Token(T_SEMICOL, _ifs.get(), _current_line);
        case '\n':
            _ifs.ignore();
            _current_line++;
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\v':
        case '\f':
            _ifs.ignore();
            break;
        default:
            std::string reserved("#{};");
            std::string value;
            while (_ifs.good()) {
                char c = _ifs.peek();
                if (reserved.find(c) != std::string::npos || std::isspace(c))
                    break;
                value += _ifs.get();
            }
            if (!value.empty()) {
                return Token(T_WORD, value);
            }
        }
    }
    return Token(T_EOF);
}

const Token ConfigLexer::expect(Token token) {
    const Token& tok = this->getNextToken();
    if (token == tok)
        return tok;
    throw UnexpectedTokenException();
}

const Token ConfigLexer::expect(TokenType type) {
    const Token& tok = this->getNextToken();
    if (type == tok.type)
        return tok;
    throw UnexpectedTokenException();
}

std::size_t ConfigLexer::getCurrentLine() const {
    return _current_line;
}

const char* ConfigLexer::UnexpectedTokenException::what() const throw() {
    return "Unexpected token.";
}

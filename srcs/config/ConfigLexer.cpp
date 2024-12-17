#include "ConfigLexer.hpp"
#include "Utils.tpp"

#include <fstream>
#include <iostream>
#include <limits>

ConfigLexer::ConfigLexer(const std::string& file) : _current_line(1) {
    std::ifstream ifs(file.c_str());
    if (!ifs.is_open())
        throw std::runtime_error("could not open config file: " + file);
    _ss << ifs.rdbuf();
}

ConfigLexer::~ConfigLexer() {
}

const Token ConfigLexer::peek() {
    std::ifstream::pos_type pos   = _ss.tellg();
    Token                   token = getNextToken();
    _ss.seekg(pos);
    return token;
}

const Token ConfigLexer::getNextToken() {
    while (_ss && !_ss.eof()) {
        char c = _ss.peek();
        switch (c) {
        case '#':
            _ss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            _current_line++;
            break;
        case '{':
            return Token(T_BLOCK_START, _ss.get(), _current_line);
        case '}':
            return Token(T_BLOCK_END, _ss.get(), _current_line);
        case ';':
            return Token(T_SEMICOL, _ss.get(), _current_line);
        case '\n':
            _ss.ignore();
            _current_line++;
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\v':
        case '\f':
            _ss.ignore();
            break;
        default:
            std::string reserved("#{};");
            std::string value;
            while (_ss.good()) {
                char c = _ss.peek();
                if (reserved.find(c) != std::string::npos || std::isspace(c))
                    break;
                value += _ss.get();
            }
            if (value == "include") {
                std::string path = getNextToken().value;
                if (path.empty()) {
                    throw std::runtime_error(
                        "include directive must have a path." + utils::to_string(_current_line)
                    );
                }
                if (getNextToken().type != T_SEMICOL) {
                    throw std::runtime_error(
                        "unexpected token after include directive. in line " +
                        utils::to_string(_current_line)
                    );
                }
                includeFile(path);
                return getNextToken();
            }
            if (!value.empty()) {
                return Token(T_WORD, value);
            }
        }
    }
    return Token(T_EOF);
}

void ConfigLexer::includeFile(const std::string& path) {
    std::ifstream ifs(path.c_str());
    if (!ifs.is_open())
        throw std::runtime_error("could not open included file: " + path);
    std::stringstream included;
    included << ifs.rdbuf();

    std::stringstream::pos_type current_pos = _ss.tellg();
    _ss.str(_ss.str().insert(current_pos, included.str()));
    _ss.seekg(current_pos);
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

#pragma once

#include <string>
#include <vector>

enum TokenType
{
    T_EOF = 0,
    T_WORD,
    T_BLOCK_START,
    T_BLOCK_END,
    T_SEMICOL
};

struct Token
{
    Token(TokenType t) : type(t), line_number(1) {};
    Token(TokenType t, const std::string &v) : type(t), value(v) {};
    Token(TokenType t, const std::string &v, std::size_t l) : type(t), value(v), line_number(l) {};
    Token(TokenType t, char c) : type(t), value(1, c) {};
    Token(TokenType t, const char c, std::size_t l) : type(t), value(1, c), line_number(l) {};
    ~Token() {};

    bool operator==(const Token &t)
    {
        return t.value == this->value && t.type == this->type;
    }

    TokenType   type;
    std::string value;
    std::size_t line_number;
};

class ConfigLexer
{
public:
    ConfigLexer(const std::string &file);
    ~ConfigLexer();

    const Token &peek() const;
    const Token &getNext();
    const Token &rpeek();
    const Token &expect(Token token);
    const Token &expect(TokenType type);
    std::size_t  getCurrentLine() const;

    class UnexpectedTokenException : public std::exception
    {
    public:
        const char *what() const throw();
    };

private:
    ConfigLexer(const ConfigLexer &other);
    ConfigLexer &operator=(const ConfigLexer &other);

    std::vector<Token>                 _tokens;
    std::vector<Token>::const_iterator _current;
    std::size_t                        _current_line;
};

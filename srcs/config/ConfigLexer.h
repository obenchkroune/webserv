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
    Token(TokenType t) : type(t){};
    Token(TokenType t, const std::string& v) : type(t), value(v){};
    Token(TokenType t, char c) : type(t), value(1, c){};
    ~Token(){};

    bool operator==(const Token& t)
    {
        return t.value == this->value && t.type == this->type;
    }

    TokenType   type;
    std::string value;
};

class ConfigLexer
{
public:
    ConfigLexer(const std::string& file);
    ~ConfigLexer();

    const Token peek() const;
    const Token getNext();
    const Token expect(Token token);
    const Token expect(TokenType type);

    class UnexpectedTokenException : public std::exception
    {
    public:
        const char* what() const throw()
        {
            return "unexpected token";
        }
    };

private:
    ConfigLexer(const ConfigLexer& other);
    ConfigLexer& operator=(const ConfigLexer& other);

    std::vector<Token>                 _tokens;
    std::vector<Token>::const_iterator _current;
};

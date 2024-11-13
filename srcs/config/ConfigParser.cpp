#include "ConfigParser.h"

/*
TODO: add default values before parasing
in case theres a missing value ( except required ones like listen)
we use the default as fallback.
*/

ConfigParser::ConfigParser(const std::string& file) : _lexer(file)
{
    //
}

ConfigParser::~ConfigParser()
{
    //
}

std::vector<ServerConfig> ConfigParser::parse()
{
    std::vector<ServerConfig> result;

    while (_lexer.peek().type != T_EOF)
    {
        result.push_back(parseServerBlock());
    }

    _lexer.expect(T_EOF);
    return result;
}

Directive ConfigParser::parseDirective()
{
    Directive directive;
    directive.name = _lexer.expect(T_WORD).value;

    while (_lexer.peek().type == T_WORD)
    {
        directive.values.push_back(_lexer.getNext().value);
    }
    _lexer.expect(T_SEMICOL);
    return directive;
}

ServerConfig ConfigParser::parseServerBlock()
{
    _lexer.expect(Token(T_WORD, "server"));
    _lexer.expect(T_BLOCK_START);

    ServerConfig result;

    while (_lexer.peek().type != T_BLOCK_END)
    {
        if (_lexer.peek().value == "location")
            result.routes.insert(parseRouteBlock());
        else
            result.directives.push_back(parseDirective());
    }

    _lexer.expect(T_BLOCK_END);
    return result;
}

std::pair<std::string, std::vector<Directive> > ConfigParser::parseRouteBlock()
{
    std::string            route_path;
    std::vector<Directive> directives;

    _lexer.expect(Token(T_WORD, "location"));

    route_path = _lexer.expect(T_WORD).value;

    _lexer.expect(T_BLOCK_START);

    while (_lexer.peek().type != T_BLOCK_END)
    {
        directives.push_back(parseDirective());
    }

    _lexer.expect(T_BLOCK_END);
    return std::make_pair(route_path, directives);
}

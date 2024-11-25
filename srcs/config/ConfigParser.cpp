#include "ConfigParser.hpp"
#include "./Validate.hpp"
#include <cstdlib>
#include <stdexcept>

LocationConfig::LocationConfig(const ServerConfig &server)
{
    root          = server.root;
    index         = server.index;
    max_body_size = server.max_body_size;
    allow_methods = server.allow_methods;
    autoindex     = server.autoindex;
    redirect      = false;
    redirect_code = 0;
    upload        = false;
    error_pages   = server.error_pages;
}

ServerConfig::ServerConfig()
{
    port          = 0;
    root          = "/var/www/html";
    max_body_size = 1024 * 10;
    autoindex     = false;
    allow_methods.push_back(HTTP_ANY);
    index.push_back("index.html");
}

ConfigParser::ConfigParser(const std::string &file) : _lexer(file)
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

    do
    {
        directive.values.push_back(_lexer.expect(T_WORD).value);
    } while (_lexer.peek().type == T_WORD);

    _lexer.expect(T_SEMICOL);
    return directive;
}

void ConfigParser::parseServerDirective(ServerConfig &server)
{
    Directive directive = parseDirective();

    if (directive.name == "listen")
    {
        std::pair<std::string, uint16_t> listen = Validate::listenDirective(directive);

        server.host = listen.first;
        server.port = listen.second;
    }
    else if (directive.name == "error_page")
        server.error_pages.insert(Validate::errorPageDirective(directive));
    else if (directive.name == "server_name")
        server.server_names.swap(directive.values);
    else if (directive.name == "root")
        server.root = Validate::rootDirective(directive);
    else if (directive.name == "index")
        server.index = Validate::indexDirective(directive);
    else if (directive.name == "client_max_body_size")
        server.max_body_size = Validate::maxSizeDirective(directive);
    else if (directive.name == "allow_methods")
        server.allow_methods = Validate::allowMethodsDirective(directive);
    else if (directive.name == "autoindex")
        server.autoindex = Validate::autoindexDirective(directive);
    else
        throw std::runtime_error("unknown directive: " + directive.name);
}

ServerConfig ConfigParser::parseServerBlock()
{
    _lexer.expect(Token(T_WORD, "server"));
    _lexer.expect(T_BLOCK_START);

    ServerConfig server;

    while (_lexer.peek().type != T_BLOCK_END)
    {
        if (_lexer.peek().value == "location")
            server.locations.push_back(parseLocationBlock(server));
        else
            parseServerDirective(server);
    }

    _lexer.expect(T_BLOCK_END);
    if (server.host.empty())
        throw std::runtime_error("missing listen directive");
    return server;
}

void ConfigParser::parseLocationDirective(LocationConfig &location)
{
    Directive directive = parseDirective();

    if (directive.name == "root")
        location.root = Validate::rootDirective(directive);
    else if (directive.name == "error_page")
        location.error_pages.insert(Validate::errorPageDirective(directive));
    else if (directive.name == "index")
        location.index.swap(directive.values);
    else if (directive.name == "client_max_body_size")
        location.max_body_size = Validate::maxSizeDirective(directive);
    else if (directive.name == "allow_methods")
        location.allow_methods = Validate::allowMethodsDirective(directive);
    else if (directive.name == "autoindex")
        location.autoindex = Validate::autoindexDirective(directive);
    else if (directive.name == "return")
    {
        std::pair<std::string, uint16_t> redirect = Validate::redirectDirective(directive);
        location.redirect                         = true;
        location.redirect_path                    = redirect.first;
        location.redirect_code                    = redirect.second;
    }
    else if (directive.name == "upload_dir")
    {
        location.upload      = true;
        location.upload_path = directive.values[0];
    }
    else
        throw std::runtime_error("unknown directive: " + directive.name);
}

LocationConfig ConfigParser::parseLocationBlock(const ServerConfig &server)
{
    _lexer.expect(Token(T_WORD, "location"));

    LocationConfig location(server);

    location.path = _lexer.expect(T_WORD).value;

    _lexer.expect(T_BLOCK_START);

    while (_lexer.peek().type != T_BLOCK_END)
        parseLocationDirective(location);

    _lexer.expect(T_BLOCK_END);
    return location;
}

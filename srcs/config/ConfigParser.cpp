#include "ConfigParser.hpp"
#include "./ConfigUtils.hpp"

#include <cstdlib>
#include <stdexcept>

LocationConfig::LocationConfig(const ServerConfig& server) {
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

ServerConfig::ServerConfig() {
    port          = 0;
    root          = "/var/www/html";
    max_body_size = 1024 * 1024;
    autoindex     = false;
    index.push_back("index.html");
}

ConfigParser::ConfigParser(const std::string& file) : _lexer(file) {
}

ConfigParser::~ConfigParser() {
    //
}

std::pair<std::vector<ServerConfig>, MimeTypes> ConfigParser::parse() {
    std::vector<ServerConfig> result;
    MimeTypes                 mime_types;

    while (_lexer.peek().type != T_EOF) {
        if (_lexer.peek().value == "server") {
            result.push_back(parseServerBlock());
        } else if (_lexer.peek().value == "types") {
            MimeTypes types = parseMimeTypesBlock();
            mime_types.insert(types.begin(), types.end());
        } else {
            throw InvalidConfigException(_lexer);
        }
    }

    _lexer.expect(T_EOF);

    return std::make_pair(result, mime_types);
}

MimeTypes ConfigParser::parseMimeTypesBlock() {
    _lexer.expect(Token(T_WORD, "types"));
    _lexer.expect(T_BLOCK_START);

    MimeTypes mime_types;

    while (_lexer.peek().type != T_BLOCK_END) {
        Directive directive        = parseDirective();
        mime_types[directive.name] = directive.values;
    }

    _lexer.expect(T_BLOCK_END);
    return mime_types;
}

Directive ConfigParser::parseDirective() {
    Directive directive;
    directive.name = _lexer.expect(T_WORD).value;

    do {
        directive.values.push_back(_lexer.expect(T_WORD).value);
    } while (_lexer.peek().type == T_WORD);

    _lexer.expect(T_SEMICOL);
    return directive;
}

void ConfigParser::parseServerDirective(ServerConfig& server) {
    Directive directive = parseDirective();

    if (directive.name == "listen") {
        std::pair<std::string, uint16_t> listen = ConfigUtils::listenDirective(directive);

        server.host = listen.first;
        server.port = listen.second;
    } else if (directive.name == "error_page")
        server.error_pages.insert(ConfigUtils::errorPageDirective(directive));
    else if (directive.name == "server_name")
        server.server_names.swap(directive.values);
    else if (directive.name == "root")
        server.root = ConfigUtils::rootDirective(directive);
    else if (directive.name == "index")
        server.index = ConfigUtils::indexDirective(directive);
    else if (directive.name == "client_max_body_size")
        server.max_body_size = ConfigUtils::maxSizeDirective(directive);
    else if (directive.name == "allow_methods")
        server.allow_methods = ConfigUtils::allowMethodsDirective(directive);
    else if (directive.name == "autoindex")
        server.autoindex = ConfigUtils::autoindexDirective(directive);
    else
        throw std::runtime_error("unknown directive: " + directive.name);
}

ServerConfig ConfigParser::parseServerBlock() {
    _lexer.expect(Token(T_WORD, "server"));
    _lexer.expect(T_BLOCK_START);

    ServerConfig server;

    while (_lexer.peek().type != T_BLOCK_END) {
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

void ConfigParser::parseLocationDirective(LocationConfig& location) {
    Directive directive = parseDirective();

    if (directive.name == "root")
        location.root = ConfigUtils::rootDirective(directive);
    else if (directive.name == "error_page")
        location.error_pages.insert(ConfigUtils::errorPageDirective(directive));
    else if (directive.name == "index")
        location.index.swap(directive.values);
    else if (directive.name == "client_max_body_size")
        location.max_body_size = ConfigUtils::maxSizeDirective(directive);
    else if (directive.name == "allow_methods")
        location.allow_methods = ConfigUtils::allowMethodsDirective(directive);
    else if (directive.name == "autoindex")
        location.autoindex = ConfigUtils::autoindexDirective(directive);
    else if (directive.name == "return") {
        std::pair<std::string, uint16_t> redirect = ConfigUtils::redirectDirective(directive);
        location.redirect                         = true;
        location.redirect_path                    = redirect.first;
        location.redirect_code                    = redirect.second;
    } else if (directive.name == "upload_dir") {
        location.upload      = true;
        location.upload_path = directive.values[0];
    } else
        throw std::runtime_error("unknown directive: " + directive.name);
}

LocationConfig ConfigParser::parseLocationBlock(const ServerConfig& server) {
    _lexer.expect(Token(T_WORD, "location"));

    LocationConfig location(server);

    location.path = _lexer.expect(T_WORD).value;

    _lexer.expect(T_BLOCK_START);

    while (_lexer.peek().type != T_BLOCK_END)
        parseLocationDirective(location);

    _lexer.expect(T_BLOCK_END);
    return location;
}

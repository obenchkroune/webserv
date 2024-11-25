#pragma once

#include "Config.hpp"
#include "ConfigLexer.hpp"
#include <stdint.h>
#include <string>
#include <vector>

struct Directive
{
    std::string              name;
    std::vector<std::string> values;
};

class ConfigParser
{
public:
    ConfigParser(const std::string &file);
    ConfigParser(const ConfigParser &other);
    ConfigParser &operator=(const ConfigParser &other);
    ~ConfigParser();

    std::vector<ServerConfig> parse();

private:
    ConfigLexer    _lexer;
    ServerConfig   parseServerBlock();
    LocationConfig parseLocationBlock(const ServerConfig &server);
    Directive      parseDirective();
    void           parseServerDirective(ServerConfig &server);
    void           parseLocationDirective(LocationConfig &location);
};

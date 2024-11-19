#pragma once

#include "ConfigLexer.h"
#include <map>
#include <string>
#include <vector>

struct Directive
{
    std::string              name;
    std::vector<std::string> values;
};

//  TODO: ??????
// struct RouteConfig
// {
// 	std::string			   url;
// 	std::vector<Directive> directives;
// };

struct ServerConfig
{
    std::vector<Directive>                         directives;
    std::map<std::string, std::vector<Directive> > routes;
};

class ConfigParser
{
public:
    ConfigParser(const std::string& file);
    ConfigParser(const ConfigParser& other);
    ConfigParser& operator=(const ConfigParser& other);
    ~ConfigParser();

    std::vector<ServerConfig> parse();

private:
    ConfigLexer _lexer;

    ServerConfig                                    parseServerBlock();
    std::pair<std::string, std::vector<Directive> > parseRouteBlock();
    Directive                                       parseDirective();
};

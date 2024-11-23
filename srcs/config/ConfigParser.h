#pragma once

#include "ConfigLexer.h"
#include <stdint.h>
#include <string>
#include <vector>

enum HttpMethod
{
    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_PATCH
};

struct Directive
{
    std::string              name;
    std::vector<std::string> values;
};

struct ServerConfig;

struct LocationConfig
{
    LocationConfig(const ServerConfig& server);

    std::string              path;
    std::string              root;
    std::vector<std::string> index;
    std::size_t              max_body_size;
    std::vector<HttpMethod>  allow_methods;
    bool                     autoindex;
    bool                     redirect;
    uint16_t                 redirect_code;
    std::string              redirect_path;
    bool                     upload;
    std::string              upload_path;
};

struct ServerConfig
{
    ServerConfig();

    std::string                 host;
    uint16_t                    port;
    std::vector<std::string>    server_names;
    std::string                 root;
    std::vector<std::string>    index;
    std::size_t                 max_body_size;
    std::vector<HttpMethod>     allow_methods;
    bool                        autoindex;
    std::vector<LocationConfig> locations;
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

    ServerConfig   parseServerBlock();
    LocationConfig parseLocationBlock(const ServerConfig& server);
    Directive      parseDirective();
    void           parseServerDirective(ServerConfig& server);
    void           parseLocationDirective(LocationConfig& location);
};

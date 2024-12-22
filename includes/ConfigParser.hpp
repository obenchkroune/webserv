#pragma once

#include "ConfigLexer.hpp"
#include "Http.hpp"
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

class InvalidConfigException : public std::exception {
public:
    InvalidConfigException(const std::string& token);
    InvalidConfigException(std::size_t line);
    InvalidConfigException(ConfigLexer& lexer);
    ~InvalidConfigException() throw();
    const char* what() const throw();

private:
    std::string _message;
};

struct ServerConfig;

struct LocationConfig {
    LocationConfig(const ServerConfig& server);

    std::string                     path;
    std::string                     root;
    std::vector<std::string>        index;
    std::size_t                     max_body_size;
    std::vector<HttpMethod>         allow_methods;
    bool                            autoindex;
    bool                            redirect;
    uint16_t                        redirect_code;
    std::string                     redirect_path;
    bool                            upload;
    std::string                     upload_path;
    std::map<uint16_t, std::string> error_pages;
    std::string                     cgi_path;
};

struct ServerConfig {
    ServerConfig();

    int                             listen_address_fd;
    std::string                     host;
    uint16_t                        port;
    std::vector<std::string>        server_names;
    std::string                     root;
    std::vector<std::string>        index;
    std::size_t                     max_body_size;
    std::vector<HttpMethod>         allow_methods;
    bool                            autoindex;
    std::vector<LocationConfig>     locations;
    std::map<uint16_t, std::string> error_pages;
};

struct Directive {
    std::string              name;
    std::vector<std::string> values;
};

typedef std::map<std::string, std::string> MimeTypes;

class ConfigParser {
public:
    ConfigParser(const std::string& file);
    ConfigParser(const ConfigParser& other);
    ConfigParser& operator=(const ConfigParser& other);
    ~ConfigParser();

    std::pair<std::vector<ServerConfig>, MimeTypes> parse();

private:
    ConfigLexer    _lexer;
    MimeTypes      parseMimeTypesBlock();
    ServerConfig   parseServerBlock();
    LocationConfig parseLocationBlock(const ServerConfig& server);
    Directive      parseDirective();
    void           parseServerDirective(ServerConfig& server);
    void           parseLocationDirective(LocationConfig& location);
};

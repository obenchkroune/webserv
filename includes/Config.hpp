#pragma once

#include "Http.hpp"
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

class InvalidConfigException : public std::exception
{
public:
    InvalidConfigException(const std::string &message);
    ~InvalidConfigException() throw();
    const char *what() const throw();

private:
    std::string _message;
};

struct ServerConfig;

struct LocationConfig
{
    LocationConfig(const ServerConfig &server);

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
};

struct ServerConfig
{
    ServerConfig();

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

class Config
{
public:
    static Config                   &getInstance();
    void                             loadConfig(const std::string &configFile);
    const ServerConfig              &getServer(const std::string &host, uint16_t port) const;
    const std::vector<ServerConfig> &getServers() const;

    class ServerNotFound : public std::exception
    {
    public:
        virtual const char *what() const throw()
        {
            return "Server not found";
        }
    };

private:
    Config();
    Config(const Config &other);
    Config &operator=(const Config &other);
    ~Config();

    std::vector<ServerConfig> _servers;
};

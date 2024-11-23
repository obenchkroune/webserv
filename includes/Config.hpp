#pragma once

#include "srcs/config/ConfigParser.hpp"
#include <stdint.h>
#include <string>

class Config
{
public:
    static Config&      getInstance();
    void                loadConfig(const std::string& configFile);
    const ServerConfig& getServer(const std::string& host, uint16_t port) const;
    const std::vector<ServerConfig>& getServers() const;

    class ServerNotFound : public std::exception
    {
    public:
        virtual const char* what() const throw()
        {
            return "Server not found";
        }
    };

private:
    Config();
    Config(const Config& other);
    Config& operator=(const Config& other);
    ~Config();

    std::vector<ServerConfig> _servers;
};

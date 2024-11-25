#include "Config.hpp"
#include "ConfigParser.hpp"

Config::Config()
{
}

Config::~Config()
{
}

Config &Config::getInstance()
{
    static Config instance;
    return instance;
}

void Config::loadConfig(const std::string &configFile)
{
    ConfigParser parser(configFile);
    _servers = parser.parse();
}

const ServerConfig &Config::getServer(const std::string &host, uint16_t port) const
{
    std::vector<ServerConfig>::const_iterator it = _servers.begin();
    for (; it != _servers.end(); it++)
    {
        if (it->host == host && it->port == port)
            return *it;
    }
    throw ServerNotFound();
}

const std::vector<ServerConfig> &Config::getServers() const
{
    return _servers;
}

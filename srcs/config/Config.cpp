#include "Config.hpp"
#include "ConfigParser.hpp"

Config::Config() {
}

Config::~Config() {
}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

void Config::loadConfig(const std::string& configFile) {
    ConfigParser                                    parser(configFile);
    std::pair<std::vector<ServerConfig>, MimeTypes> cfg = parser.parse();
    _servers                                            = cfg.first;
    _mimeTypes                                          = cfg.second;
}

const ServerConfig& Config::getServer(const std::string& host, uint16_t port) const {
    std::vector<ServerConfig>::const_iterator it = _servers.begin();
    for (; it != _servers.end(); it++) {
        if (it->host == host && it->port == port)
            return *it;
    }
    throw ServerNotFound();
}

const MimeTypes& Config::getMimeTypes() const {
    return _mimeTypes;
}

const std::vector<ServerConfig>& Config::getServers() const {
    return _servers;
}

std::ostream& operator<<(std::ostream& os, const Config& cfg) {
    std::vector<ServerConfig>::const_iterator it = cfg.getServers().begin();
    for (; it != cfg.getServers().end(); it++) {
        os << "Server: " << it->host << ":" << it->port << std::endl;
        os << "  root: " << it->root << std::endl;
        os << "  index: ";
        std::vector<std::string>::const_iterator index = it->index.begin();
        for (; index != it->index.end(); index++) {
            os << *index << " ";
        }
        os << std::endl;
        os << "  max_body_size: " << it->max_body_size << std::endl;
        os << "  allow_methods: ";
        std::vector<HttpMethod>::const_iterator method = it->allow_methods.begin();
        for (; method != it->allow_methods.end(); method++) {
            os << *method << " ";
        }
        os << std::endl;
        os << "  autoindex: " << it->autoindex << std::endl;
        os << "  locations: " << std::endl;
        std::vector<LocationConfig>::const_iterator location = it->locations.begin();
        for (; location != it->locations.end(); location++) {
            os << "    Location: " << location->path << std::endl;
            os << "      root: " << location->root << std::endl;
            os << "      index: ";
            std::vector<std::string>::const_iterator index = location->index.begin();
            for (; index != location->index.end(); index++) {
                os << *index << " ";
            }
            os << std::endl;
            os << "      max_body_size: " << location->max_body_size << std::endl;
            os << "      allow_methods: ";
            std::vector<HttpMethod>::const_iterator method = location->allow_methods.begin();
            for (; method != location->allow_methods.end(); method++) {
                os << *method << " ";
            }
            os << std::endl;
            os << "      autoindex: " << location->autoindex << std::endl;
            os << "      redirect: " << location->redirect << std::endl;
            os << "      redirect_path: " << location->redirect_path << std::endl;
            os << "      redirect_code: " << location->redirect_code << std::endl;
        }
    }
    MimeTypes::const_iterator mime = cfg.getMimeTypes().begin();
    for (; mime != cfg.getMimeTypes().end(); mime++) {
        os << "Mime type: " << mime->first << " -> ";
        for (std::size_t i = 0; i < mime->second.size(); i++) {
            os << mime->second[i] << " ";
        }
        os << std::endl;
    }
    return os;
}

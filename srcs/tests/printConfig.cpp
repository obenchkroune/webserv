#include "Test.hpp"
#include <iostream>

void printServerConfig(const ServerConfig& config) {
    std::cout << "\n=== Server Configuration ===" << std::endl;
    std::cout << "Host: " << config.host << std::endl;
    std::cout << "Port: " << config.port << std::endl;

    std::cout << "Server Names: ";
    for (std::vector<std::string>::const_iterator it = config.server_names.begin();
         it != config.server_names.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Root: " << config.root << std::endl;
    std::cout << "Index: ";
    for (std::vector<std::string>::const_iterator it = config.index.begin();
         it != config.index.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "Max Body Size: " << config.max_body_size << std::endl;

    std::cout << "Allowed Methods: ";
    for (std::vector<HttpMethod>::const_iterator it = config.allow_methods.begin();
         it != config.allow_methods.end(); ++it) {
        switch (*it) {
        case HTTP_GET:
            std::cout << "GET ";
            break;
        case HTTP_HEAD:
            std::cout << "HEAD ";
            break;
        case HTTP_POST:
            std::cout << "POST ";
            break;
        case HTTP_PUT:
            std::cout << "PUT ";
            break;
        case HTTP_DELETE:
            std::cout << "DELETE ";
            break;
        case HTTP_PATCH:
            std::cout << "PATCH ";
            break;
        }
    }
    std::cout << std::endl;

    std::cout << "Autoindex: " << (config.autoindex ? "on" : "off") << std::endl;

    std::cout << "\nLocations:" << std::endl;
    for (std::vector<LocationConfig>::const_iterator loc = config.locations.begin();
         loc != config.locations.end(); ++loc) {
        std::cout << "\n  Location " << loc->path << ":" << std::endl;
        std::cout << "    Root: " << loc->root << std::endl;
        std::cout << "    Index: ";
        for (std::vector<std::string>::const_iterator it = loc->index.begin();
             it != loc->index.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
        std::cout << "    Max Body Size: " << loc->max_body_size << std::endl;

        std::cout << "    Allowed Methods: ";
        for (std::vector<HttpMethod>::const_iterator it = loc->allow_methods.begin();
             it != loc->allow_methods.end(); ++it) {
            switch (*it) {
            case HTTP_GET:
                std::cout << "GET ";
                break;
            case HTTP_HEAD:
                std::cout << "HEAD ";
                break;
            case HTTP_POST:
                std::cout << "POST ";
                break;
            case HTTP_PUT:
                std::cout << "PUT ";
                break;
            case HTTP_DELETE:
                std::cout << "DELETE ";
                break;
            case HTTP_PATCH:
                std::cout << "PATCH ";
                break;
            }
        }
        std::cout << std::endl;

        std::cout << "    Autoindex: " << (loc->autoindex ? "on" : "off") << std::endl;

        if (loc->redirect) {
            std::cout << "    Redirect: " << loc->redirect_code << " -> " << loc->redirect_path
                      << std::endl;
        }

        if (loc->upload) {
            std::cout << "    Upload Path: " << loc->upload_path << std::endl;
        }

        std::cout << "    Error Pages:" << std::endl;
        std::map<uint16_t, std::string>::const_iterator it = loc->error_pages.begin();
        for (; it != loc->error_pages.end(); ++it) {
            std::cout << "      " << it->first << " -> " << it->second << std::endl;
        }
    }
    std::cout << "==========================\n" << std::endl;
}

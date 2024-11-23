#include "./config/ConfigParser.h"
#include <iostream>

// TODO: remove this function
void printServerConfig(const ServerConfig& config)
{
    std::cout << "\n=== Server Configuration ===" << std::endl;
    std::cout << "Host: " << config.host << std::endl;
    std::cout << "Port: " << config.port << std::endl;

    std::cout << "Server Names: ";
    for (std::vector<std::string>::const_iterator it = config.server_names.begin();
         it != config.server_names.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Root: " << config.root << std::endl;
    std::cout << "Index: ";
    for (std::vector<std::string>::const_iterator it = config.index.begin();
         it != config.index.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "Max Body Size: " << config.max_body_size << std::endl;

    std::cout << "Allowed Methods: ";
    for (std::vector<std::string>::const_iterator it = config.allow_methods.begin();
         it != config.allow_methods.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Autoindex: " << (config.autoindex ? "on" : "off") << std::endl;

    std::cout << "\nLocations:" << std::endl;
    for (std::vector<LocationConfig>::const_iterator loc = config.locations.begin();
         loc != config.locations.end(); ++loc)
    {
        std::cout << "\n  Location " << loc->path << ":" << std::endl;
        std::cout << "    Root: " << loc->root << std::endl;
        std::cout << "    Index: ";
        for (std::vector<std::string>::const_iterator it = loc->index.begin();
             it != loc->index.end(); ++it)
        {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
        std::cout << "    Max Body Size: " << loc->max_body_size << std::endl;

        std::cout << "    Allowed Methods: ";
        for (std::vector<std::string>::const_iterator it = loc->allow_methods.begin();
             it != loc->allow_methods.end(); ++it)
        {
            std::cout << *it << " ";
        }
        std::cout << std::endl;

        std::cout << "    Autoindex: " << (loc->autoindex ? "on" : "off") << std::endl;

        if (loc->redirect)
        {
            std::cout << "    Redirect: " << loc->redirect_code << " -> " << loc->redirect_path
                      << std::endl;
        }

        if (loc->upload)
        {
            std::cout << "    Upload Path: " << loc->upload_path << std::endl;
        }
    }
    std::cout << "==========================\n" << std::endl;
}

int main(int ac, char** av)
{
    if (ac > 2)
    {
        std::cout << "Usage: " << av[0] << " [config_file]" << std::endl;
        return 1;
    }

    std::string config_path = "example.conf";
    if (ac == 2)
        config_path = av[1];

    try
    {
        ConfigParser                              parser(config_path);
        std::vector<ServerConfig>                 servers = parser.parse();
        std::vector<ServerConfig>::const_iterator it      = servers.begin();
        for (; it != servers.end(); it++)
        {
            printServerConfig(*it);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

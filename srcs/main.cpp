#include "ConfigParser.h"
#include <iostream>

// TODO: remove this function
void printServerConfig(const ServerConfig& config)
{
    std::cout << "Server Configuration:" << std::endl;

    // Print main server directives
    std::cout << "Global directives:" << std::endl;
    std::vector<Directive>::const_iterator dir_it;
    for (dir_it = config.directives.begin(); dir_it != config.directives.end(); ++dir_it)
    {
        std::cout << "  " << dir_it->name << ": ";
        std::vector<std::string>::const_iterator val_it;
        for (val_it = dir_it->values.begin(); val_it != dir_it->values.end(); ++val_it)
        {
            std::cout << *val_it << " ";
        }
        std::cout << std::endl;
    }

    // Print routes and their directives
    std::cout << "\nRoutes:" << std::endl;
    std::map<std::string, std::vector<Directive> >::const_iterator route_it;
    for (route_it = config.routes.begin(); route_it != config.routes.end(); ++route_it)
    {
        std::cout << "Location " << route_it->first << " {" << std::endl;
        std::vector<Directive>::const_iterator route_dir_it;
        for (route_dir_it = route_it->second.begin(); route_dir_it != route_it->second.end();
             ++route_dir_it)
        {
            std::cout << "    " << route_dir_it->name << ": ";
            std::vector<std::string>::const_iterator val_it;
            for (val_it = route_dir_it->values.begin(); val_it != route_dir_it->values.end();
                 ++val_it)
            {
                std::cout << *val_it << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "}" << std::endl;
    }
}

int main(void)
{
    try
    {
        ConfigParser                              parser("example.conf");
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

#include "./config/ConfigParser.hpp"
#include "tests/printConfig.hpp"
#include <iostream>

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

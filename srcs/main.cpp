#include "Config.hpp"
#include "tests/printConfig.hpp"
#include <iostream>

int
main(int ac, char** av)
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
        Config::getInstance().loadConfig(config_path);

        const std::vector<ServerConfig>& servers =
          Config::getInstance().getServers();

        for (std::size_t i = 0; i < servers.size(); i++)
        {
            printServerConfig(servers[i]);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

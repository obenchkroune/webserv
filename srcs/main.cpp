#include "Server.hpp"
#include "Test.hpp"

#define DEFAULT_CONFIG_PATH "config/default.conf"
#define USAGE(program_name) "Usage: " + std::string(program_name) + " [path to config file]"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << USAGE(av[0]) << '\n';
        return 1;
    }
    try
    {
        Config::getInstance().loadConfig(ac == 2 ? av[1] : DEFAULT_CONFIG_PATH);
        std::vector<ServerConfig> configs = Config::getInstance().getServers();

        std::vector<Server>                 servers;
        IOMultiplexer                       IOmltplxr;
        std::vector<ServerConfig>::iterator conf_it = configs.begin();
        for (; conf_it != configs.end(); conf_it++)
        {
            printServerConfig(*conf_it);
            servers.push_back(Server(*conf_it, &IOmltplxr));
        }
        std::vector<Server>::iterator it = servers.begin();
        for (; it != servers.end(); it++)
        {
            try
            {
                it->Start();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Server Error: " << e.what() << '\n';
            }
        }
        try
        {
            IOmltplxr.StartEventLoop();
        }
        catch (const std::exception &e)
        {
            std::cerr << "I/O Error: " << e.what() << '\n';
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal Error: " << e.what() << '\n';
    }

    return 0;
}

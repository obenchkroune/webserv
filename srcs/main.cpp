#include "Server.hpp"
#include "Test.hpp"
#include <csignal>

#define DEFAULT_CONFIG_PATH "config/default.conf"
#define USAGE(program_name) "Usage: " + std::string(program_name) + " [path to config file]"

extern int webserv_unix_signal;

void handle_sigint(int signal)
{
    webserv_unix_signal = signal;
    std::cout << std::endl;
}
void handle_signals()
{
    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
        throw std::runtime_error("signal() failed.");
    if (signal(SIGINT, &handle_sigint) == SIG_ERR)
        throw std::runtime_error("signal() failed.");
}
int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << USAGE(av[0]) << std::endl;
        return 1;
    }
    try
    {
        handle_signals();
        Config::getInstance().loadConfig(ac == 2 ? av[1] : DEFAULT_CONFIG_PATH);
        std::vector<ServerConfig> configs = Config::getInstance().getServers();

        std::vector<Server>                 servers;
        IOMultiplexer                       IOmltplxr;
        std::vector<ServerConfig>::iterator conf_it = configs.begin();
        for (; conf_it != configs.end(); conf_it++)
        {
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
                std::cerr << "Server Error: " << e.what() << std::endl;
            }
        }
        try
        {
            IOmltplxr.StartEventLoop();
        }
        catch (const std::exception &e)
        {
            std::cerr << "I/O Error: " << e.what() << std::endl;
        }
        std::cout << PROGNAME "/" PROGVERSION " exited." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }
    return 0;
}

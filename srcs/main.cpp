#include "Server.hpp"
#include "ConfigParser.hpp"
#include "Test.hpp"

int main(int c, char** v, char** envp)
{
    (void)envp;
    if (c != 2)
    {
        std::cerr << "Error: No configuration file provided.\n";
        std::cerr << "Usage: " << v[0] << " [path to config file]\n";
        return 1;
    }
    try
    {
        ConfigParser                        parser(v[1]);
        std::vector<ServerConfig>           configs = parser.parse();
        IOMultiplexer                       IOmltplxr;
        std::vector<ServerConfig>::iterator conf_it = configs.begin();
        std::vector<Server>                 servers;
        for (; conf_it != configs.end(); conf_it++)
            servers.push_back(Server(*conf_it, &IOmltplxr));
        std::vector<Server>::iterator it = servers.begin();
        for (; it != servers.end(); it++)
        {
            try
            {
                it->Start();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Server Error:\nException caught:\n" << e.what() << '\n';
            }
        }
        try
        {
            IOmltplxr.StartEventLoop();
        }
        catch (const std::exception& e)
        {
            std::cerr << "I/O Error:\nException caught:\n" << e.what() << '\n';
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal Error:\nException caught:\n" << e.what() << '\n';
    }

    return 0;
}

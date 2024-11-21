#include "ConfigParser.h"
#include "Test.hpp"

int main(int c, char**v, char**envp)
{
    (void) envp;
    try
    {
        if (c != 2)
        {
            std::cerr << "Error: No configuration file provided.\n";
            std::cerr << "Usage: " << v[0] << " [path to config file]\n";
            return 1;
        }
        ConfigParser                              parser(v[1]);
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
        return 1;
    }
    return 0;
}

#include "Server.hpp"
#include "Test.hpp"

int main(int c, char** v, char** envp)
{
    (void)envp;
    try
    {
        if (c != 2)
        {
            std::cerr << "Error: No configuration file provided.\n";
            std::cerr << "Usage: " << v[0] << " [path to config file]\n";
            return 1;
        }
        ConfigParser                  parser(v[1]);
        std::vector<ServerConfig>     configs = parser.parse();
        std::vector<Server>           servers(configs.begin(), configs.end());
        std::vector<Server>::iterator it = servers.begin();
        for (; it != servers.end(); it++)
        {
            try
            {
                it->Start();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Warning: " << e.what() << '\n';
            }
        }
        std::cout << "Press enter to exit program.\n";
        char tmp;
        std::cin.get(tmp);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

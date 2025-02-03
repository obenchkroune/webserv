#include "./ConfigUtils.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include <arpa/inet.h>
#include <climits>
#include <cstdlib>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace ConfigUtils
{

std::string cgiPathDirective(const Directive& directive)
{
    if (directive.values.size() != 1)
        throw InvalidConfigException(directive.name);
    // check if the executable exists and valid
    struct stat buffer;
    bool isvalid = stat(directive.values[0].c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode) &&
                   buffer.st_mode & S_IXUSR;
    if (!isvalid)
        throw InvalidConfigException(directive.values[0]);
    return directive.values[0];
}
std::vector<std::string> cgiExtensionsDirective(const Directive& directive)
{
    if (directive.values.size() == 0)
        throw InvalidConfigException(directive.name);
    return directive.values;
}
std::string cgiConfDirective(const Directive& directive)
{
    if (directive.values.size() == 0)
        throw InvalidConfigException(directive.name);
    return directive.values.front();
}
std::size_t maxSizeDirective(const Directive& directive)
{
    std::size_t        res;
    char               size_multiplier = 'B';
    std::istringstream iss(directive.values[0]);

    if (directive.values.size() != 1 || !(iss >> res))
        throw InvalidConfigException(directive.name);
    size_multiplier = iss.get();
    switch (size_multiplier)
    {
    case 'B':
        break;
    case 'K':
        res *= 1024;
        break;
    case 'M':
        res *= 1024 * 1024;
        break;
    case 'G':
        res *= 1024 * 1024 * 1024;
        break;
    default:
        throw InvalidConfigException(directive.name);
    }
    return res;
}

static inline bool isValidIpAddr(const std::string& ip)
{
    struct sockaddr_in sa;

    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) == 1;
}

std::pair<std::string, uint16_t> listenDirective(const Directive& directive)
{
    if (directive.values.size() != 1)
        throw InvalidConfigException(directive.name);

    std::string host;
    uint16_t    port;

    if ((std::istringstream(directive.values[0]) >> port).eof())
        return std::make_pair("0.0.0.0", port);

    host = directive.values[0];
    port = 80;

    std::size_t semicol = directive.values[0].find(':');
    if (semicol != std::string::npos)
    {
        host = directive.values[0].substr(0, semicol);
        std::istringstream iss(directive.values[0].substr(semicol + 1));
        iss >> port;

        if (iss.fail() || !iss.eof())
            throw InvalidConfigException(directive.name);
    }

    if (!isValidIpAddr(host))
        throw InvalidConfigException(host);

    return std::make_pair(host, port);
}

std::string rootDirective(const Directive& directive)
{
    if (directive.values.size() != 1)
        throw InvalidConfigException(directive.name);
    struct stat buffer;
    if (stat(directive.values[0].c_str(), &buffer) != 0)
        throw InvalidConfigException(directive.values[0]);
    char buff[PATH_MAX];
    if (realpath(directive.values[0].c_str(), buff) == NULL)
        throw InvalidConfigException("realpath(): failed in line" + directive.values[0]);
    return buff;
}

std::vector<std::string> indexDirective(const Directive& directive)
{
    if (directive.values.empty())
        throw InvalidConfigException(directive.name);
    return directive.values;
}

bool autoindexDirective(const Directive& directive)
{
    if (directive.values.size() != 1)
        throw InvalidConfigException(directive.name);
    return directive.values[0] == "on";
}

static inline bool isValidRedirectCode(uint16_t code)
{
    return (code >= 300 && code <= 305) || (code >= 307 && code <= 308);
}

std::pair<std::string, uint16_t> redirectDirective(const Directive& directive)
{
    if (directive.values.size() != 2)
        throw InvalidConfigException(directive.name);

    uint16_t           code;
    std::istringstream iss(directive.values[0]);
    iss >> code;

    if (iss.fail() || !isValidRedirectCode(code))
        throw InvalidConfigException(directive.name);

    return std::make_pair(directive.values[1], code);
}

std::vector<std::string> allowMethodsDirective(const Directive& directive)
{
    return directive.values;
}

static inline bool isValidErrorCode(uint16_t code)
{
    return ((code >= 400 && code <= 417) || (code >= 500 && code <= 505));
}

std::pair<uint16_t, std::string> errorPageDirective(const Directive& directive)
{
    std::pair<uint16_t, std::string> error_page;

    if (directive.values.size() != 2)
        throw InvalidConfigException(directive.name);

    std::istringstream iss(directive.values[0]);
    iss >> error_page.first;
    error_page.second = directive.values[1];
    if (iss.fail() || !iss.eof())
        throw InvalidConfigException(directive.name);

    if (!isValidErrorCode(error_page.first))
        throw InvalidConfigException(directive.name);
    return error_page;
}

} // namespace ConfigUtils

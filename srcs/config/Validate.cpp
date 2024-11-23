#include "./Validate.hpp"
#include "Utils.hpp"
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

namespace Validate
{

std::size_t maxSizeDirective(const Directive& directive)
{
    std::size_t        res;
    char               size_multiplier = 'B';
    std::istringstream iss(directive.values[0]);

    if (directive.values.size() != 1 || !(iss >> res))
        throw std::runtime_error("invalid client_max_body_size directive");
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
        throw std::runtime_error("invalid client_max_body_size directive");
    }
    return res;
}

std::pair<std::string, uint16_t> listenDirective(const Directive& directive)
{
    if (directive.values.size() != 1)
        throw std::runtime_error("invalid listen directive");

    std::vector<std::string> values = Utils::ft_split(directive.values[0], ':');

    if (values.size() != 2)
        throw std::runtime_error("invalid listen directive");

    uint16_t port;
    if (!(std::istringstream(values[1]) >> port))
        throw std::runtime_error("invalid listen directive");

    return std::make_pair(values[0], port);
}

std::string rootDirective(const Directive& directive)
{
    if (directive.values.size() != 1)
        throw std::runtime_error("invalid root directive");
    // struct stat buffer;
    // if (stat(directive.values[0].c_str(), &buffer) != 0)
    //     throw std::runtime_error("root path does not exist");
    return directive.values[0];
}

std::vector<std::string> indexDirective(const Directive& directive)
{
    if (directive.values.empty())
        throw std::runtime_error("invalid index directive");
    return directive.values;
}

bool autoindexDirective(const Directive& directive)
{
    if (directive.values.size() != 1)
        throw std::runtime_error("invalid autoindex directive");
    return directive.values[0] == "on";
}

std::pair<std::string, uint16_t> redirectDirective(const Directive& directive)
{
    if (directive.values.size() != 2)
        throw std::runtime_error("invalid return directive");

    uint16_t code;
    if (!(std::istringstream(directive.values[0]) >> code))
        throw std::runtime_error("invalid return directive");

    return std::make_pair(directive.values[1], code);
}

std::vector<HttpMethod> allowMethodsDirective(const Directive& directive)
{
    std::vector<HttpMethod> res;

    for (std::vector<std::string>::const_iterator it = directive.values.begin();
         it != directive.values.end(); ++it)
    {
        if (*it == "GET")
            res.push_back(HTTP_GET);
        else if (*it == "HEAD")
            res.push_back(HTTP_HEAD);
        else if (*it == "POST")
            res.push_back(HTTP_POST);
        else if (*it == "PUT")
            res.push_back(HTTP_PUT);
        else if (*it == "DELETE")
            res.push_back(HTTP_DELETE);
        else if (*it == "PATCH")
            res.push_back(HTTP_PATCH);
        else
            throw std::runtime_error(
                "invalid method in allow_methods directive");
    }

    return res;
}

} // namespace Validate

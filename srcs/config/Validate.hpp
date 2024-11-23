#pragma once

#include "./ConfigParser.hpp"
#include <stdint.h>

namespace Validate
{

std::pair<std::string, uint16_t>
listenDirective(const Directive& directive);
std::pair<std::string, uint16_t>
redirectDirective(const Directive& directive);

std::string
rootDirective(const Directive& directive);
std::size_t
maxSizeDirective(const Directive& directive);
std::vector<std::string>
indexDirective(const Directive& directive);
bool
autoindexDirective(const Directive& directive);
std::vector<HttpMethod>
allowMethodsDirective(const Directive& directive);

} // namespace Validate

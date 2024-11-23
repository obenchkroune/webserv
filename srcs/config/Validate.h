#pragma once

#include "./ConfigParser.h"
#include <stdint.h>

namespace Validate
{

std::pair<std::string, uint16_t> listenDirective(const Directive& directive);
std::string                      rootDirective(const Directive& directive);
uint16_t                         maxSizeDirective(const Directive& directive);
std::vector<std::string>         indexDirective(const Directive& directive);
bool                             autoindexDirective(const Directive& directive);
std::pair<std::string, uint16_t> redirectDirective(const Directive& directive);

} // namespace Validate

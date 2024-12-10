#pragma once

#include <sstream>
#include <string>

namespace utils {

template <typename Tp>
std::string to_string(Tp value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

} // namespace utils

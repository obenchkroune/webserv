#pragma once

#include "Http.hpp"

class HttpHeader
{
public:
    std::string                             name;
    std::string                             raw_value;
    std::vector<std::string>                values;
    std::multimap<std::string, std::string> parameters;

    HttpHeader();
    HttpHeader(const std::string& key, const std::string& value);
    ~HttpHeader();

    HttpHeader& operator=(const HttpHeader& other);

private:
    std::vector<std::string> _tokens;

    void tokenize();
    void parse();
};

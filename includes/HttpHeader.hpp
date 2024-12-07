#pragma once

#include "Http.hpp"

class HttpHeader
{
public:
    HttpHeader();
    HttpHeader(const std::string& key, const std::string& value);
    ~HttpHeader();

    HttpHeader& operator=(const HttpHeader& other);

    std::string                             name;
    std::string                             raw_value;
    std::vector<std::string>                values;
    std::multimap<std::string, std::string> parameters;

    std::vector<std::string> _tokens;

private:
    void tokenize();
    void parse();
};

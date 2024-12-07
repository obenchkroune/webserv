#pragma once

#include "Http.hpp"

struct HttpHeaderValue
{
    std::string                             value;
    std::multimap<std::string, std::string> parameters;
    float                                   quality_factor;

    static bool compare(const HttpHeaderValue& a, const HttpHeaderValue& b);

    HttpHeaderValue();
    HttpHeaderValue(const std::string& value);
    ~HttpHeaderValue();

    HttpHeaderValue& operator=(const HttpHeaderValue& other);
};

struct HttpHeader
{
public:
    std::string                  name;
    std::string                  raw_value;
    std::vector<HttpHeaderValue> values;

    HttpHeader();
    HttpHeader(const std::string& key, const std::string& value);
    ~HttpHeader();

    HttpHeader& operator=(const HttpHeader& other);

private:
    std::vector<std::string> _tokens;

    std::vector<std::string>            stripParameters(const std::string& str);
    std::pair<std::string, std::string> parseParameter(const std::string& str);

    void tokenize();
    void parse();
};

std::ostream& operator<<(std::ostream& os, const HttpHeader& header);

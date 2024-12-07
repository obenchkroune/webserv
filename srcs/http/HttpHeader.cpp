#include "HttpHeader.hpp"
#include <algorithm>
#include <stdexcept>

HttpHeader::HttpHeader()
{
}

HttpHeader::HttpHeader(const std::string& name, const std::string& value)
    : name(name), raw_value(value)
{
    this->tokenize();
}

HttpHeader::~HttpHeader()
{
}

HttpHeader& HttpHeader::operator=(const HttpHeader& other)
{
    if (this != &other)
    {
        this->name      = other.name;
        this->raw_value = other.raw_value;
        this->values    = other.values;
    }
    return *this;
}

void HttpHeader::tokenize()
{
    std::map<char, char> sep_pairs;
    sep_pairs['{'] = '}';
    sep_pairs['['] = ']';
    sep_pairs['<'] = '>';
    sep_pairs['('] = ')';
    sep_pairs['"'] = '"';

    if (raw_value.empty())
    {
        return;
    }

    std::string::iterator it       = raw_value.begin();
    std::string::iterator start    = it;
    bool                  in_quote = false;
    for (; it != raw_value.end(); it++)
    {
        switch (*it)
        {
        case '"':
            in_quote = !in_quote;
            break;
        case ',':
        case ' ':
        case '\t':
            if (!in_quote)
            {
                std::string tok = Utils::ft_strtrim(std::string(start, it));
                if (!tok.empty())
                    _tokens.push_back(tok);
                start = it + 1;
            }
            break;
        case '{':
        case '[':
        case '<':
        case '(':
            if (sep_pairs.find(*it) != sep_pairs.end())
            {
                std::string::iterator pair = std::find(it, raw_value.end(), sep_pairs[*it]);
                if (pair == raw_value.end())
                {
                    throw std::runtime_error(
                        "invalid header field-value: missing closing separator '" +
                        std::string(1, sep_pairs[*it]) + "'");
                }
                std::string tok = Utils::ft_strtrim(std::string(start, it));
                if (!tok.empty())
                    _tokens.push_back(tok);
                start = pair + 1;
                it    = pair;
            }
            break;
        case '}':
        case ']':
        case '>':
        case ')':
            if (sep_pairs.find(*it) == sep_pairs.end())
            {
                throw std::runtime_error(
                    "invalid header field-value: unexpected closing separator '" +
                    std::string(1, *it) + "'");
            }
            break;
        default:
            break;
        }
    }

    if (start != raw_value.end())
    {
        _tokens.push_back(std::string(start, raw_value.end()));
    }
}

void HttpHeader::parse()
{
}

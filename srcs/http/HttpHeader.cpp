#include "HttpHeader.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

HttpHeaderValue::HttpHeaderValue() : quality_factor(1.0) {}

HttpHeaderValue::HttpHeaderValue(const std::string& val) : quality_factor(1.0)
{
    if (val.size() >= 2 && val[0] == '"' && val[val.size() - 1] == '"')
    {
        value = val.substr(1, val.size() - 2);
    }
    else
    {
        value = val;
    }
}

HttpHeaderValue::~HttpHeaderValue() {}

/**
 * @brief Compares two HttpHeaderValues based on their quality factor.
 */
bool HttpHeaderValue::compare(const HttpHeaderValue& a, const HttpHeaderValue& b)
{
    return a.quality_factor > b.quality_factor;
}

HttpHeaderValue& HttpHeaderValue::operator=(const HttpHeaderValue& other)
{
    if (this != &other)
    {
        this->value          = other.value;
        this->parameters     = other.parameters;
        this->quality_factor = other.quality_factor;
    }
    return *this;
}

HttpHeader::HttpHeader() {}

HttpHeader::HttpHeader(const std::string& name, const std::string& value)
    : name(name), raw_value(value)
{
    if (!isValidFieldName(name))
    {
        throw std::runtime_error("Invalid header field name");
    }
    this->tokenize();
    this->parse();
}

HttpHeader::~HttpHeader() {}

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

bool HttpHeader::isTokenChar(char c)
{
    const std::string separators = "()<>@,;:\\\"/[]?={} \t";
    return (c >= '!' && c <= '~') && separators.find(c) == std::string::npos;
}

bool HttpHeader::isValidFieldName(const std::string& name)
{
    if (name.empty())
        return false;
    for (std::string::size_type i = 0; i < name.size(); ++i)
    {
        if (!isTokenChar(name[i]))
            return false;
    }
    return true;
}

std::string HttpHeader::unquote(const std::string& str)
{
    if (str.size() < 2 || str[0] != '"' || str[str.size() - 1] != '"')
    {
        return str;
    }
    std::string result;
    for (std::string::size_type i = 1; i < str.size() - 1; ++i)
    {
        if (str[i] == '\\' && i + 1 < str.size() - 1)
        {
            result += str[++i];
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}

void HttpHeader::skipOWS(std::string::size_type& pos, const std::string& str)
{
    while (pos < str.size() && (str[pos] == ' ' || str[pos] == '\t'))
    {
        ++pos;
    }
}

std::string::size_type HttpHeader::findCommentEnd(
    const std::string& str, std::string::size_type pos
)
{
    int  depth   = 1;
    bool escaped = false;

    while (pos < str.size())
    {
        if (escaped)
        {
            escaped = false;
            ++pos;
            continue;
        }

        if (str[pos] == '\\')
        {
            escaped = true;
            ++pos;
            continue;
        }

        if (str[pos] == '(')
        {
            ++depth;
        }
        else if (str[pos] == ')')
        {
            --depth;
            if (depth == 0)
            {
                return pos + 1;
            }
        }
        ++pos;
    }
    throw std::runtime_error("Unterminated comment in header value");
}

void HttpHeader::tokenize()
{
    _tokens.clear();
    std::string            token;
    std::string::size_type pos      = 0;
    bool                   in_quote = false;

    while (pos < raw_value.size())
    {
        skipOWS(pos, raw_value);
        if (pos >= raw_value.size())
            break;

        if (raw_value[pos] == '"')
        {
            in_quote = true;
            token += raw_value[pos++];
            while (pos < raw_value.size() && (in_quote || raw_value[pos] != ','))
            {
                if (raw_value[pos] == '"' && (pos == 0 || raw_value[pos - 1] != '\\'))
                {
                    in_quote = false;
                }
                token += raw_value[pos++];
            }
        }
        else if (raw_value[pos] == '(')
        {
            std::string::size_type comment_end = findCommentEnd(raw_value, pos + 1);
            pos                                = comment_end;
            continue;
        }
        else
        {
            while (pos < raw_value.size() && raw_value[pos] != ',')
            {
                if (raw_value[pos] == '(')
                {
                    std::string::size_type comment_end = findCommentEnd(raw_value, pos + 1);
                    pos                                = comment_end;
                    continue;
                }

                // Check if we hit a space that's not within a comment or quote
                if (raw_value[pos] == ' ' && !token.empty())
                {
                    std::string cleaned = utils::strtrim(token);
                    if (!cleaned.empty())
                    {
                        _tokens.push_back(cleaned);
                    }
                    token.clear();
                    ++pos;
                    continue;
                }

                token += raw_value[pos++];
            }
        }

        token = utils::strtrim(token);
        if (!token.empty())
        {
            _tokens.push_back(token);
        }
        token.clear();
        if (pos < raw_value.size() && raw_value[pos] == ',')
            ++pos;
    }
}

std::vector<std::string> HttpHeader::stripParameters(const std::string& str)
{
    std::vector<std::string> tokens;

    std::string::const_iterator it    = str.begin();
    std::string::const_iterator start = it;

    for (; it != str.end(); ++it)
    {
        if (*it == ';')
        {
            std::string tok = utils::strtrim(std::string(start, it));
            if (!tok.empty())
                tokens.push_back(tok);
            start = it + 1;
        }
    }
    if (start != str.end())
    {
        std::string tok = utils::strtrim(std::string(start, str.end()));
        if (!tok.empty())
            tokens.push_back(tok);
    }
    return tokens;
}

std::pair<std::string, std::string> HttpHeader::parseParameter(const std::string& str)
{
    std::string::const_iterator it = str.begin();
    std::string                 key, value;

    for (; it != str.end(); ++it)
    {
        if (*it == '=')
        {
            key   = unquote(utils::strtrim(std::string(str.begin(), it)));
            value = unquote(utils::strtrim(std::string(it + 1, str.end())));
            break;
        }
    }
    return std::make_pair(key, value);
}

void HttpHeader::parse()
{
    std::vector<std::string>::iterator it = _tokens.begin();

    for (; it != _tokens.end(); it++)
    {
        std::vector<std::string> tokens = stripParameters(*it);
        if (tokens.size() > 0)
        {
            HttpHeaderValue metadata(tokens[0]);
            for (size_t i = 1; i < tokens.size(); i++)
            {
                if (tokens[i].substr(0, 2) == "q=")
                {
                    std::istringstream iss(tokens[i].substr(2));
                    iss >> metadata.quality_factor;
                    if (iss.fail())
                        throw std::runtime_error("invalid quality factor " + iss.str());
                }
                else
                {
                    metadata.parameters.insert(parseParameter(tokens[i]));
                }
            }
            values.push_back(metadata);
        }
    }
    _tokens.clear();
    std::sort(values.begin(), values.end(), HttpHeaderValue::compare);
}

std::ostream& operator<<(std::ostream& os, const HttpHeader& header)
{
    os << header.name << ": " << header.raw_value << std::endl;
    os << "* Values: " << std::endl;
    std::vector<HttpHeaderValue>::const_iterator it = header.values.begin();
    for (; it != header.values.end(); it++)
    {
        os << " - " << it->value;
        std::map<std::string, std::string>::const_iterator it2 = it->parameters.begin();
        if (it2 != it->parameters.end())
        {
            os << " [";
            for (; it2 != it->parameters.end(); it2++)
            {
                os << " " << it2->first << " = " << it2->second << " ";
            }
            os << "]";
        }
        os << std::endl;
    }

    return os;
}

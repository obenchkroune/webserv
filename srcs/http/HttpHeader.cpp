#include "HttpHeader.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

HttpHeaderValue::HttpHeaderValue() : quality_factor(1.0) {
}

HttpHeaderValue::HttpHeaderValue(const std::string& value) : value(value), quality_factor(1.0) {
}

HttpHeaderValue::~HttpHeaderValue() {
}

/**
 * @brief Compares two HttpHeaderValues based on their quality factor.
 */
bool HttpHeaderValue::compare(const HttpHeaderValue& a, const HttpHeaderValue& b) {
    return a.quality_factor > b.quality_factor;
}

HttpHeaderValue& HttpHeaderValue::operator=(const HttpHeaderValue& other) {
    if (this != &other) {
        this->value          = other.value;
        this->parameters     = other.parameters;
        this->quality_factor = other.quality_factor;
    }
    return *this;
}

HttpHeader::HttpHeader() {
}

HttpHeader::HttpHeader(const std::string& name, const std::string& value)
    : name(name), raw_value(value) {
    this->tokenize();
    this->parse();
}

HttpHeader::~HttpHeader() {
}

HttpHeader& HttpHeader::operator=(const HttpHeader& other) {
    if (this != &other) {
        this->name      = other.name;
        this->raw_value = other.raw_value;
        this->values    = other.values;
    }
    return *this;
}

void HttpHeader::tokenize() {
    std::map<char, char> sep_pairs;
    sep_pairs['{'] = '}';
    sep_pairs['['] = ']';
    sep_pairs['<'] = '>';
    sep_pairs['"'] = '"';

    if (raw_value.empty()) {
        return;
    }

    std::string::iterator it       = raw_value.begin();
    std::string::iterator start    = it;
    bool                  in_quote = false;
    for (; it != raw_value.end(); it++) {
        switch (*it) {
        case '"':
            in_quote = !in_quote;
            break;
        case ',':
        case ' ':
        case '\t': {
            if (!in_quote) {
                std::string tok = util::ft_strtrim(std::string(start, it));
                if (!tok.empty())
                    _tokens.push_back(tok);
                start = it + 1;
            }
            break;
        }
        case '(': {
            std::string tok = util::ft_strtrim(std::string(start, it));
            if (!tok.empty())
                _tokens.push_back(tok);

            std::string::iterator pair = std::find(it, raw_value.end(), ')');
            if (pair == raw_value.end())
                throw std::runtime_error(
                    "invalid header field-value: missing closing separator ')'");
            it    = pair;
            start = pair + 1;

            break;
        }
        case '{':
        case '[':
        case '<': {
            if (it != start) {
                std::string tok = util::ft_strtrim(std::string(start, it));
                if (!tok.empty())
                    _tokens.push_back(tok);
                start = it;
            }

            std::string::iterator pair = std::find(it, raw_value.end(), sep_pairs[*it]);
            if (pair == raw_value.end())
                throw std::runtime_error("invalid header field-value: missing closing separator '" +
                                         std::string(1, sep_pairs[*it]) + "'");

            std::string tok = util::ft_strtrim(std::string(start, pair + 1));
            if (!tok.empty())
                _tokens.push_back(tok);
            start = pair + 1;
            it    = pair;
            break;
        }
        case '}':
        case ']':
        case '>':
        case ')':
            throw std::runtime_error("invalid header field-value: unexpected closing separator '" +
                                     std::string(1, *it) + "'");
            break;
        default:
            break;
        }
    }

    if (start != raw_value.end()) {
        _tokens.push_back(std::string(start, raw_value.end()));
    }
}

std::vector<std::string> HttpHeader::stripParameters(const std::string& str) {
    std::vector<std::string> tokens;

    std::string::const_iterator it    = str.begin();
    std::string::const_iterator start = it;

    for (; it != str.end(); ++it) {
        if (*it == ';') {
            std::string tok = util::ft_strtrim(std::string(start, it));
            if (!tok.empty())
                tokens.push_back(tok);
            start = it + 1;
        }
    }
    if (start != str.end()) {
        std::string tok = util::ft_strtrim(std::string(start, str.end()));
        if (!tok.empty())
            tokens.push_back(tok);
    }
    return tokens;
}

std::pair<std::string, std::string> HttpHeader::parseParameter(const std::string& str) {
    std::string::const_iterator it = str.begin();
    std::string                 key, value;

    for (; it != str.end(); ++it) {
        if (*it == '=') {
            key   = util::ft_strtrim(std::string(str.begin(), it));
            value = util::ft_strtrim(std::string(it + 1, str.end()));
            break;
        }
    }
    return std::make_pair(key, value);
}

void HttpHeader::parse() {
    std::vector<std::string>::iterator it = _tokens.begin();

    for (; it != _tokens.end(); it++) {
        std::vector<std::string> tokens = stripParameters(*it);
        if (tokens.size() > 0) {
            HttpHeaderValue metadata(tokens[0]);
            for (size_t i = 1; i < tokens.size(); i++) {
                if (tokens[i].substr(0, 2) == "q=") {
                    std::istringstream iss(tokens[i].substr(2));
                    iss >> metadata.quality_factor;
                    if (iss.fail())
                        throw std::runtime_error("invalid quality factor " + iss.str());
                } else {
                    metadata.parameters.insert(parseParameter(tokens[i]));
                }
            }
            values.push_back(metadata);
        }
    }
    _tokens.clear();
    std::sort(values.begin(), values.end(), HttpHeaderValue::compare);
}

std::ostream& operator<<(std::ostream& os, const HttpHeader& header) {
    os << header.name << ": " << header.raw_value << std::endl;
    os << "* Values: " << std::endl;
    std::vector<HttpHeaderValue>::const_iterator it = header.values.begin();
    for (; it != header.values.end(); it++) {
        os << " - " << it->value;
        std::map<std::string, std::string>::const_iterator it2 = it->parameters.begin();
        if (it2 != it->parameters.end()) {
            os << " [";
            for (; it2 != it->parameters.end(); it2++) {
                os << " " << it2->first << " = " << it2->second << " ";
            }
            os << "]";
        }
        os << std::endl;
    }

    return os;
}

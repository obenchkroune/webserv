#include "Request.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>

Request::Request(const std::string& request) : _buffer(request)
{
}

Request::Request(const Request& other)
{
    *this = other;
}

Request& Request::operator=(const Request& other)
{
    if (this == &other)
        return *this;
    _body         = other._body;
    _headers      = other._headers;
    _http_version = other._http_version;
    _method       = other._method;
    _uri          = other._uri;
    return *this;
}

Request::~Request()
{
}

void Request::appendBody(const std::string& body)
{
    _body += body;
    _buffer << body;
}

uint16_t Request::Parse()
{
    try
    {
        this->parseRequestLine();
        this->ParseHeaders();
        this->ParseBody();
        return 200;
    }
    catch (const std::exception& e)
    {
        return 400;
    }
}

HttpMethod Request::getMethod() const
{
    return _method;
}

std::string Request::getUri() const
{
    return _uri;
}

std::string Request::getVersion() const
{
    return _http_version;
}

const HttpHeader* Request::getHeader(const std::string& key) const
{
    std::vector<HttpHeader>::const_iterator it = _headers.begin();
    for (; it != _headers.end(); it++)
    {
        if (it->name == key)
            return it.base();
    }
    return NULL;
}

const std::vector<HttpHeader>& Request::getHeaders() const
{
    return _headers;
}

std::string Request::getBody() const
{
    return _body;
}

const std::map<std::string, std::string>& Request::getQueryParams() const
{
    return _query_params;
}

void Request::setMethod(std::string method)
{
    if (method == "GET")
        _method = HTTP_GET;
    else if (method == "HEAD")
        _method = HTTP_HEAD;
    else if (method == "POST")
        _method = HTTP_POST;
    else if (method == "DELETE")
        _method = HTTP_DELETE;
    else if (method == "PUT")
        _method = HTTP_PUT;
    else
        throw RequestException("Invalid HTTP method.");
}

void Request::setUri(const std::string& uri)
{
    if (uri.empty() || uri[0] != '/')
        throw RequestException("Invalid URI.");
    _uri = uri;
}

void Request::setVersion(const std::string& version)
{
    if (version.substr(0, 5) != "HTTP/")
        throw RequestException("Invalid HTTP version.");

    int major, minor;
    if (std::sscanf(version.c_str() + 5, "%d.%d", &major, &minor) != 2 || major != 1 || minor != 1)
        throw RequestException("Invalid HTTP version.");
    _http_version = version;
}

void Request::setHeader(const HttpHeader& header)
{
    _headers.push_back(header);
}

void Request::setBody(const std::string& body)
{
    _body = body;
}

std::string Request::getHeaderLine(std::istream& iss) const
{
    std::string line;

    std::getline(iss, line, '\n');
    if (line.empty() || line[line.size() - 1] != '\r')
        throw RequestException("Invalid line ending.");
    line.erase(line.size() - 1);

    if (std::string(" \t").find(iss.peek()) != std::string::npos)
    {
        iss.ignore();
        line += ' ';
        while (std::string(" \t").find(iss.peek()) != std::string::npos)
            iss.ignore();
        line += Request::getHeaderLine(iss);
    }
    return line;
}

void Request::parseRequestLine()
{
    std::string method, uri, version;

    if (!std::getline(_buffer, method, ' ') || !std::getline(_buffer, uri, ' '))
        throw RequestException("invalid start line.");

    if (!std::getline(_buffer, version, '\n') || version[version.size() - 1] != '\r')
        throw RequestException("invalid start line.");

    version.erase(version.size() - 1);

    this->setMethod(method);
    this->setUri(uri);
    this->setVersion(version);
    this->parseQueryParams();
}

void Request::parseQueryParams()
{
    size_t pos = _uri.find('?');
    if (pos == std::string::npos)
        return;
    std::string query = _uri.substr(pos + 1);
    _uri              = _uri.substr(0, pos);

    std::vector<std::string> params = Utils::ft_split(query, '&');
    for (size_t i = 0; i < params.size(); i++)
    {
        if (params[i].empty())
            continue;
        if (params[i].find('=') == std::string::npos)
            throw RequestException("Invalid query parameter.");
        std::vector<std::string> param = Utils::ft_split(params[i], '=');
        if (param.size() == 2)
            _query_params[param[0]] = param[1];
        else
            _query_params[param[0]] = "";
    }
}

void Request::ParseHeaders()
{
    std::string line;
    while (!(line = Request::getHeaderLine(_buffer)).empty())
    {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
            throw RequestException("Invalid header.");
        std::string name  = Utils::ft_strtrim(line.substr(0, pos)),
                    value = Utils::ft_strtrim(line.substr(pos + 1));
        HttpHeader header(name, value);
        this->setHeader(header);
    }
    this->ValidateHeaders();
}

void Request::ParseBody()
{
    std::string body;
    std::string line;
    while (std::getline(_buffer, line, '\n'))
        body += line + '\n';

    this->setBody(body);

    HttpMethod method     = this->getMethod();
    bool       isBodyless = (method == HTTP_GET || method == HTTP_DELETE || method == HTTP_HEAD);
    if (isBodyless && !this->getBody().empty())
        throw RequestException("Body not allowed for this method.");
}

void Request::ValidateHeaders()
{
    if (this->getHeader("Host") == NULL)
        throw RequestException("Host header is required.");
}

std::ostream& operator<<(std::ostream& os, const Request& request)
{
    os << "Method: ";
    switch (request.getMethod())
    {
    case HTTP_GET:
        std::cout << "GET";
        break;
    case HTTP_POST:
        std::cout << "POST";
        break;
    case HTTP_DELETE:
        std::cout << "DELETE";
        break;
    case HTTP_PUT:
        std::cout << "PUT";
        break;
    case HTTP_ANY:
        std::cout << "ANY";
        break;
    default:
        std::cout << "UNKNOWN";
    }
    os << std::endl;
    os << "URI: " << request.getUri() << std::endl;
    os << "Query params: " << std::endl;
    for (size_t i = 0; i < request.getQueryParams().size(); i++)
    {
        os << "- " << request.getQueryParams().begin()->first << " = "
           << request.getQueryParams().begin()->second << std::endl;
    }
    os << "HTTP version: " << request.getVersion() << std::endl;
    os << "Headers:\n";

    size_t max_length = 0;

    std::vector<HttpHeader>::const_iterator it;
    for (it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
    {
        max_length = std::max(max_length, it->name.length());
    }

    for (it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
    {
        os << std::setw(max_length) << std::left << it->name << ": " << it->raw_value << std::endl;
        os << "* Tokens: " << std::endl;
        std::vector<std::string>::const_iterator it2;
        for (it2 = it->_tokens.begin(); it2 != it->_tokens.end(); ++it2)
        {
            os << "  - " << *it2 << std::endl;
        }
    }
    os << "Body: ";
    if (request.getBody().empty())
        os << "<empty>\n";
    else
        os << request.getBody() << '\n';
    return os;
}

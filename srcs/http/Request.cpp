#include "Request.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include <iostream>
#include <sstream>

Request::Request(const std::string &request) : _request(request)
{
}

Request::Request(const Request &other)
{
    *this = other;
}

Request &Request::operator=(const Request &other)
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

void Request::Parse()
{
    std::istringstream iss(_request);

    std::string              line       = Request::getline(iss);
    std::vector<std::string> start_line = Utils::ft_split(line, ' ');
    if (start_line.size() != 3)
        throw RequestException(HTTP_STATUS_BAD_REQUEST);
    setMethod(start_line[0]);
    setUri(start_line[1]);
    setVersion(start_line[2]);

    /**
     * TODO : parse header parameters and secondary values
     */
    while (!iss.eof() && !(line = Request::getline(iss)).empty())
    {
        HttpHeader  header;
        std::size_t pos = line.find(": ");

        if (pos == std::string::npos)
            throw RequestException(HTTP_STATUS_BAD_REQUEST);
        header.name  = line.substr(0, pos);
        header.value = Utils::ft_strtrim(line.substr(pos + 1));
        setHeader(header);
    }

    if (getHeader("Host") == NULL)
        throw RequestException(HTTP_STATUS_BAD_REQUEST);

    if ((iss >> std::ws).eof())
        return;
    std::string body = Request::getline(iss);

    HttpMethod method = getMethod();
    if ((method == HTTP_GET || method == HTTP_DELETE || method == HTTP_HEAD) && !body.empty())
    {
        throw RequestException(HTTP_STATUS_BAD_REQUEST);
    }
    setBody(body);
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

const HttpHeader *Request::getHeader(const std::string &key) const
{
    std::vector<HttpHeader>::const_iterator it = _headers.begin();
    for (; it != _headers.end(); it++)
    {
        if (it->name == key)
            return it.base();
    }
    return NULL;
}

const std::vector<HttpHeader> &Request::getHeaders() const
{
    return _headers;
}

std::string Request::getBody() const
{
    return _body;
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

void Request::setUri(const std::string &uri)
{
    if (uri.empty() || uri[0] != '/')
        throw RequestException("Invalid URI.");
    _uri = uri;
}

void Request::setVersion(const std::string &version)
{
    if (version != "HTTP/1.1")
        throw RequestException(HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED);
    _http_version = version;
}

void Request::setHeader(const HttpHeader &header)
{
    _headers.push_back(header);
}

void Request::setBody(const std::string &body)
{
    _body = body;
}

std::string Request::getline(std::istream &iss) const
{
    std::string line;
    std::getline(iss, line, '\n');
    if (line.empty() || line[line.size() - 1] != '\r')
        throw RequestException(HTTP_STATUS_BAD_REQUEST);
    line.erase(line.size() - 1);
    return line;
}

std::ostream &operator<<(std::ostream &os, const Request &request)
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
        os << std::setw(max_length) << std::left << it->name << ": " << it->value << std::endl;
    }
    os << "Body: ";
    if (request.getBody().empty())
        os << "<empty>\n";
    else
        os << request.getBody() << '\n';
    return os;
}

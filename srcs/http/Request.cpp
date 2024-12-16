#include "Request.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>

Request::Request(const std::string& request) : _buffer(request) {
}

Request::Request(const Request& other) {
    *this = other;
}

Request& Request::operator=(const Request& other) {
    if (this == &other)
        return *this;
    _body         = other._body;
    _headers      = other._headers;
    _http_version = other._http_version;
    _method       = other._method;
    _uri          = other._uri;
    _query_params = other._query_params;

    _buffer.clear();
    _buffer << other._buffer.rdbuf();
    return *this;
}

Request::~Request() {
}

void Request::appendBody(const std::string& body) {
    _body += body;
}

HttpStatus Request::parse() {
    try {
        parseRequestLine();
        parseHeaders();
        return HttpStatus(STATUS_OK, HTTP_STATUS_OK);
    } catch (const RequestException& e) {
        return e.getErrorCode();
    }
}

HttpMethod Request::getMethod() const {
    return _method;
}

std::string Request::getUri() const {
    return _uri;
}

std::string Request::getVersion() const {
    return _http_version;
}

const HttpHeader* Request::getHeader(const std::string& key) const {
    std::vector<HttpHeader>::const_iterator it = _headers.begin();
    for (; it != _headers.end(); it++) {
        if (it->name == key)
            return it.base();
    }
    return NULL;
}

const std::vector<HttpHeader>& Request::getHeaders() const {
    return _headers;
}

std::string Request::getBody() const {
    return _body;
}

const std::map<std::string, std::string>& Request::getQueryParams() const {
    return _query_params;
}

void Request::setMethod(std::string method) {
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
    else if (method == "PATCH")
        _method = HTTP_PATCH;
    else
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));
}

void Request::setUri(const std::string& uri) {
    if (uri.empty() || uri[0] != '/')
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));
    _uri = uri;
}

void Request::setVersion(const std::string& version) {
    if (version.substr(0, 5) != "HTTP/")
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));

    int major, minor;
    if (std::sscanf(version.c_str() + 5, "%d.%d", &major, &minor) != 2)
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));
    if (major != 1 || minor != 1)
        throw RequestException(
            HttpStatus(STATUS_HTTP_VERSION_NOT_SUPPORTED, HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)
        );
    _http_version = version;
}

void Request::setHeader(const HttpHeader& header) {
    _headers.push_back(header);
}

void Request::setBody(const std::string& body) {
    _body = body;
}

std::string Request::getHeaderLine() {
    std::string line;

    std::getline(_buffer, line, '\n');
    if (line.empty() || line[line.size() - 1] != '\r')
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));
    line.erase(line.size() - 1);

    if (std::string(" \t").find(_buffer.peek()) != std::string::npos) {
        _buffer.ignore();
        line += ' ';
        while (std::string(" \t").find(_buffer.peek()) != std::string::npos)
            _buffer.ignore();
        line += Request::getHeaderLine();
    }
    return line;
}

void Request::parseRequestLine() {
    std::string method, uri, query, version;

    if (!std::getline(_buffer, method, ' ') || !std::getline(_buffer, uri, ' ')) {
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));
    }

    if (uri.find('?') != std::string::npos) {
        query         = uri.substr(uri.find('?') + 1);
        uri           = uri.substr(0, uri.find('?'));
        _query_params = parseQueryParams(query);
    }

    if (!std::getline(_buffer, version, '\n') || version[version.size() - 1] != '\r') {
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));
    }

    version.erase(version.size() - 1);

    setMethod(method);
    setUri(uri);
    setVersion(version);
}

std::map<std::string, std::string> Request::parseQueryParams(const std::string& query) {
    std::map<std::string, std::string> params;

    std::string::const_iterator start = query.begin();
    std::string::const_iterator it    = query.begin();
    std::string                 key, value;
    for (; it != query.end(); ++it) {
        if (*it == '=' && start != it) {
            key   = util::strtrim(std::string(start, it));
            start = it + 1;
        } else if (*it == '&' && !key.empty()) {
            value       = util::strtrim(std::string(start, it));
            params[key] = value;
            start       = it + 1;
            key.clear();
            value.clear();
        }
    }
    return params;
}

void Request::parseHeaders() {
    std::string line;
    while (!(line = Request::getHeaderLine()).empty()) {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
            throw RequestException(HttpStatus(STATUS_BAD_REQUEST, HTTP_STATUS_BAD_REQUEST));
        std::string name  = util::strtrim(line.substr(0, pos)),
                    value = util::strtrim(line.substr(pos + 1));
        HttpHeader header(name, value);
        setHeader(header);
    }
    // TODO: check for the required host headers (the server instance is needed)
}

std::ostream& operator<<(std::ostream& os, const Request& request) {
    os << "===========================================================\n";
    os << "Method: ";
    switch (request.getMethod()) {
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
    case HTTP_HEAD:
        std::cout << "HEAD";
        break;
    case HTTP_PATCH:
        std::cout << "PATCH";
        break;
    }
    os << '\n';
    os << "URI: " << request.getUri() << '\n';
    os << "Query params: " << '\n';
    for (size_t i = 0; i < request.getQueryParams().size(); i++) {
        os << "- " << request.getQueryParams().begin()->first << " = "
           << request.getQueryParams().begin()->second << '\n';
    }
    os << "HTTP version: " << request.getVersion() << '\n';
    os << "Headers:\n";

    size_t max_length = 0;

    std::vector<HttpHeader>::const_iterator it;
    for (it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it) {
        max_length = std::max(max_length, it->name.length());
    }

    for (it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it) {
        os << *it << '\n';
    }
    os << "Body: ";
    if (request.getBody().empty())
        os << "<empty>\n";
    else
        os << request.getBody() << '\n';
    os << "===========================================================" << std::endl;
    return os;
}

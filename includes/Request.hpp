#pragma once

#include "Config.hpp"
#include <iomanip>

class RequestException : public std::exception
{
public:
    RequestException(const std::string& message);
    ~RequestException() throw();
    const char* what() const throw();

private:
    std::string _message;
};

class Request
{
public:
    Request(const std::string& request);
    Request(const Request& other);
    Request& operator=(const Request& other);
    ~Request();

    void Parse();

    // getters
    HttpMethod                                getMethod() const;
    std::string                               getUri() const;
    std::string                               getVersion() const;
    const HttpHeader*                         getHeader(const std::string& key) const;
    const std::vector<HttpHeader>&            getHeaders() const;
    std::string                               getBody() const;
    const std::map<std::string, std::string>& getQueryParams() const;

    // setters
    void setMethod(std::string method);
    void setUri(const std::string& uri);
    void setVersion(const std::string& version);
    void setHeader(const HttpHeader& header);
    void setBody(const std::string& body);

    std::string getline(std::istream& iss) const;

private:
    std::string                        _request;
    std::map<std::string, std::string> _query_params;
    HttpMethod                         _method;
    std::string                        _uri;
    std::string                        _http_version;
    std::string                        _body;
    std::vector<HttpHeader>            _headers;

    void ValidateHeaders();

    void ParseRequestLine(std::istringstream& iss);
    void parseQueryParams();
    void ParseHeaders(std::istringstream& iss);
    void ParseBody(std::istringstream& iss);
};

std::ostream& operator<<(std::ostream& os, const Request& request);

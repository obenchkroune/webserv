#pragma once

#include "Config.hpp"
#include "HttpHeader.hpp"
#include <iomanip>
#include <sstream>

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

    uint16_t Parse();
    void     appendBody(const std::string& body);

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

private:
    std::stringstream                  _buffer;
    std::map<std::string, std::string> _query_params;
    HttpMethod                         _method;
    std::string                        _uri;
    std::string                        _http_version;
    std::string                        _body;
    std::vector<HttpHeader>            _headers;

    void        ValidateHeaders(); // TODO: refactor
    std::string getHeaderLine(std::istream& iss) const;
    void        parseRequestLine();
    void        parseQueryParams();
    void        parseHeaders();
    void        parseBody();
};

std::ostream& operator<<(std::ostream& os, const Request& request);

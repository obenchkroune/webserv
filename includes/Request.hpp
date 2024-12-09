#pragma once

#include "Config.hpp"
#include "HttpHeader.hpp"
#include <iomanip>
#include <sstream>

class RequestException : public std::exception {
public:
    RequestException(const std::string& message);
    RequestException(uint16_t error_code, const std::string& message);
    ~RequestException() throw();
    const char* what() const throw();

    uint16_t getErrorCode() const;

private:
    std::string _message;
    uint16_t    _error_code;
};

class Request {
public:
    Request(const std::string& request);
    Request(const Request& other);
    Request& operator=(const Request& other);
    ~Request();

    uint16_t parse();
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

    void                               parseRequestLine();
    std::map<std::string, std::string> parseQueryParams(const std::string& query);
    std::string                        getHeaderLine();
    void                               parseHeaders();
};

std::ostream& operator<<(std::ostream& os, const Request& request);

#pragma once

#include "Config.hpp"

class RequestException : public std::exception
{
public:
    RequestException(const std::string &message);
    ~RequestException() throw();
    const char *what() const throw();

private:
    std::string _message;
};

class Request
{
public:
    Request();
    Request(const std::string &request);
    Request(const Request &other);
    Request &operator=(const Request &other);
    ~Request();

    // getters
    HttpMethod        getMethod() const;
    std::string       getUri() const;
    std::string       getVersion() const;
    const HttpHeader *getHeader(const std::string &key) const;
    std::string       getBody() const;

    // setters
    void setMethod(std::string method);
    void setUri(const std::string &uri);
    void setVersion(const std::string &version);
    void setHeader(const HttpHeader &header);
    void setBody(const std::string &body);

    std::string getline(std::istream &iss) const;
    // TODO: remove this
    void print() const;

private:
    HttpMethod              _method;
    std::string             _uri;
    std::string             _http_version;
    std::string             _body;
    std::vector<HttpHeader> _headers;
};

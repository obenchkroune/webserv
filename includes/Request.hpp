#pragma once

#include "Config.hpp"
#include "HttpHeader.hpp"
#include <iomanip>
#include <sstream>

class RequestException : public std::exception
{
public:
    RequestException(const HttpStatus& error_code);
    ~RequestException() throw();
    const char* what() const throw();

    HttpStatus getErrorCode() const;

private:
    HttpStatus _error_code;
};

class Request
{
public:
    Request();
    Request(const Request& other);
    ~Request();

    Request&   operator=(const Request& other);
    Request&   operator+=(const std::vector<uint8_t>& bytes);
    HttpStatus parse();

    // getters
    const std::string                         getMethod() const;
    std::string                               getUri() const;
    std::string                               getVersion() const;
    const HttpHeader*                         getHeader(const std::string& key) const;
    const std::vector<HttpHeader>&            getHeaders() const;
    const HttpHeader*                         getContentTypeHeader() const;
    const std::vector<uint8_t>&               getBody() const;
    const std::map<std::string, std::string>& getQueryParams() const;
    const std::string&                        getQueryParamsString() const;
    const std::stringstream&                  getRawBuffer() const;
    const HttpStatus&                         getStatus() const;
    bool                                      isCompleted() const;

    // setters
    void setMethod(const std::string& method);
    void setUri(const std::string& uri);
    void setVersion(const std::string& version);
    void setHeader(const HttpHeader& header);

    void clear();

private:
    bool                               _is_headers_completed;
    bool                               _is_body_completed;
    bool                               _is_chunked;
    std::vector<uint8_t>               _raw_buffer;
    std::stringstream                  _stream_buf;
    std::map<std::string, std::string> _query_params;
    std::string                        _query_params_string;
    std::string                        _method;
    std::string                        _uri;
    std::string                        _http_version;
    std::vector<uint8_t>               _body;
    size_t                             _body_length;
    const HttpHeader*                  _content_type_header;
    const HttpHeader*                  _transfer_encoding_header;
    std::vector<HttpHeader>            _headers;
    HttpStatus                         _status;

    void                               parseRequestLine();
    std::map<std::string, std::string> parseQueryParams(const std::string query);
    std::string                        getHeaderLine();
    void                               parseHeaders();
    HttpStatus                         ValidateMultipart();
};

std::ostream& operator<<(std::ostream& os, const Request& request);

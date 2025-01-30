#pragma once

#include "Config.hpp"
#include "HttpHeader.hpp"
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <unistd.h>

typedef std::vector<LocationConfig>::const_iterator LocationIterator;
typedef std::vector<ServerConfig>::const_iterator   VirtualServerIterator;
class Server;
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
    Request(const int& client_address_socket_fd);
    Request(const Request& other);
    Request& operator=(const Request& other);
    ~Request();

public:
    Request& operator+=(const std::vector<uint8_t>& bytes);
    void     parse();

    // getters
    const std::string                         getMethod() const;
    std::string                               getUri() const;
    std::string                               getVersion() const;
    const HttpHeader*                         getHeader(const std::string& key) const;
    const std::vector<HttpHeader>&            getHeaders() const;
    int                                       getBodyFd() const;
    size_t                                    getBodySize() const;
    const std::map<std::string, std::string>& getQueryParams() const;
    const std::string&                        getQueryParamsString() const;
    const std::stringstream&                  getRawBuffer() const;
    const HttpStatus&                         getStatus() const;
    bool                                      isCompleted() const;
    bool                                      isChunked() const;
    VirtualServerIterator                     getRequestVirtualServer() const;
    LocationIterator                          getRequestFileLocation() const;

    void clear();

private:
    // setters
    void setMethod(const std::string& method);
    void setUri(const std::string& uri);
    void setVersion(const std::string& version);
    void setHeader(const HttpHeader& header);

private:
    int                                _client_address_socket_fd;
    bool                               _is_headers_completed;
    bool                               _is_body_completed;
    bool                               _is_chunked;
    bool                               _remove_chunk_data_trailing_crlf;
    size_t                             _chunk_size;
    size_t                             _remaining_chunk_size;
    std::vector<uint8_t>               _raw_buffer;
    std::stringstream                  _headers_raw_buf;
    std::map<std::string, std::string> _query_params;
    std::string                        _query_params_string;
    std::string                        _method;
    std::string                        _uri;
    std::string                        _http_version;
    int                                _body_fd;
    size_t                             _body_size;
    std::vector<uint8_t>               _body_buff;
    size_t                             _body_received;
    std::vector<HttpHeader>            _headers;
    HttpStatus                         _status;
    VirtualServerIterator              _request_virtual_server;
    LocationIterator                   _request_file_location;

    void                               parseRequestLine();
    std::map<std::string, std::string> parseQueryParams(const std::string query);
    std::string                        getHeaderLine();
    void                               parseHeaders();
    void                               writeBodyToFile();
    void                               writeChunkToFile(size_t& offset);
    void                               writeChunked();
};

std::ostream& operator<<(std::ostream& os, const Request& request);

#include "Request.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>

Request::Request()
    : _is_headers_completed(false), _is_body_completed(false), _is_chunked(false),
      _remove_chunk_data_trailing_crlf(false), _remaining_chunk_size(0), _body_size(0),
      _content_type_header(NULL), _transfer_encoding_header(NULL)
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
    _is_headers_completed            = other._is_headers_completed;
    _is_body_completed               = other._is_body_completed;
    _is_chunked                      = other._is_chunked;
    _remove_chunk_data_trailing_crlf = other._remove_chunk_data_trailing_crlf;
    _chunk_size                      = other._chunk_size;
    _remaining_chunk_size            = other._remaining_chunk_size;
    _raw_buffer                      = other._raw_buffer;
    _body                            = other._body;
    _body_length                     = other._body_length;
    _body_size                       = other._body_size;
    _body_fd                         = other._body_fd;
    _content_type_header             = other._content_type_header;
    _transfer_encoding_header        = other._transfer_encoding_header;
    _headers                         = other._headers;
    _http_version                    = other._http_version;
    _method                          = other._method;
    _uri                             = other._uri;
    _query_params_string             = other._query_params_string;
    _query_params                    = other._query_params;

    _stream_buf.clear();
    _stream_buf << other._stream_buf.rdbuf();
    return *this;
}

Request& Request::operator+=(const std::vector<uint8_t>& bytes)
{
    if (_is_headers_completed == false)
    {
        _raw_buffer.insert(_raw_buffer.end(), bytes.begin(), bytes.end());
        const char* headers_end = CRLF CRLF;
        const char*                    found =
            utils::strnstr((const char*)_raw_buffer.data(), headers_end, _raw_buffer.size());

        if (found != NULL)
        {
            size_t headers_end_pos =
                (found - (const char*)_raw_buffer.data()) + strlen(headers_end);
            if (headers_end_pos < _raw_buffer.size())
            {
                _body.insert(_body.end(), _raw_buffer.begin() + headers_end_pos, _raw_buffer.end());
                _raw_buffer.erase(_raw_buffer.begin() + headers_end_pos, _raw_buffer.end());
            }
            _raw_buffer.insert(_raw_buffer.end(), 0);
            _stream_buf << _raw_buffer.data();
            _raw_buffer.clear();
            _status = parse();
        }
    }
    else if (_is_body_completed == false)
    {
        _body.insert(_body.end(), bytes.begin(), bytes.end());
        if (_is_chunked)
        {
            writeChunked();
        }
        else if (_body.size() == _body_length)
        {
            _is_body_completed = true;
            _status            = ValidateMultipart();
        }
        else if (_body.size() > _body_length)
            assert(!"There is some overflow that need to be carried on to next request..");
    }
    else
    {
        assert(!"IMPOSSIBLE TO REACH!");
    }

    return *this;
}

Request::~Request() {}
void Request::writeChunkToFile(size_t& offset)
{
    size_t to_write = std::min(_remaining_chunk_size, _body.size() - offset);
    if (to_write)
    {
        ssize_t bytes_written = write(_body_fd, _body.data() + offset, to_write);
        if (bytes_written < 0 || to_write != (size_t)bytes_written)
        {
            std::cerr << "Error writing received request body." << std::endl;
            close(_body_fd);
            throw RequestException(HttpStatus(STATUS_INTERNAL_SERVER_ERROR));
        }
        offset += bytes_written;
        _body_size += bytes_written;
        _remaining_chunk_size -= bytes_written;
    }
}
void Request::writeChunked()
{
    size_t offset = 0;
    if (_remaining_chunk_size)
    {
        writeChunkToFile(offset);
        if (!_remaining_chunk_size)
        {
            _remove_chunk_data_trailing_crlf = true;
            if (offset + 2 <= _body.size())
            {
                offset += 2;
                _remove_chunk_data_trailing_crlf = false;
            }
        }
    }
    if (!_remaining_chunk_size && _remove_chunk_data_trailing_crlf)
    {
        if (_body.size() < 2)
            return;
        offset += 2;
        _remove_chunk_data_trailing_crlf = false;
    }
    if (offset < _body.size())
    {
        const char* chunk_size_ln =
            utils::strnstr((const char*)(_body.data() + offset), CRLF, _body.size() - offset);
        if (chunk_size_ln)
        {
            _chunk_size           = std::strtoul((const char*)(_body.data() + offset), NULL, 16);
            _remaining_chunk_size = _chunk_size;
            offset                = (chunk_size_ln + 2) - ((const char*)_body.data());
            for (; offset < _body.size() && chunk_size_ln && _chunk_size;)
            {
                writeChunkToFile(offset);
                if (!_remaining_chunk_size)
                {
                    if (offset + 2 <= _body.size())
                        offset += 2;
                    else
                    {
                        _remove_chunk_data_trailing_crlf = true;
                        break;
                    }
                }
                chunk_size_ln = utils::strnstr(
                    (const char*)(_body.data() + offset), CRLF, _body.size() - offset
                );
                if (chunk_size_ln)
                {
                    _chunk_size = std::strtoul((const char*)(_body.data() + offset), NULL, 16);
                    _remaining_chunk_size = _chunk_size;
                    offset                = (chunk_size_ln + 2) - ((const char*)_body.data());
                }
            }
        }
    }
    if (offset)
    {
        if (offset == _body.size())
            _body.clear();
        else
            _body.erase(_body.begin(), _body.begin() + offset);
    }
    if (_chunk_size == 0)
    {
        _is_body_completed = true;
    }
}

HttpStatus Request::parse()
{
    _is_headers_completed = true;

    try
    {
        parseRequestLine();
        parseHeaders();
        _content_type_header      = getHeader("Content-Type");
        _transfer_encoding_header = getHeader("Transfer-Encoding");
        if (_transfer_encoding_header != NULL && _transfer_encoding_header->values.front().value ==
                                                     "chunked") // this should be case insensitive
        {
            _is_chunked = true;
            _body_fd    = open("/tmp/", O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR);
            if (_body_fd < 0)
                return HttpStatus(STATUS_INTERNAL_SERVER_ERROR);
            if (_body.size())
                writeChunked();
        }
        else
        {
            const HttpHeader* lenght_header = getHeader("Content-Length");
            if (lenght_header == NULL)
            {
                _body_length       = 0;
                _is_body_completed = true;
            }
            else
            {
                _body_length = strtoul(lenght_header->raw_value.c_str(), NULL, 10);
                if (_body.size() == _body_length)
                {
                    _is_body_completed = true;
                    return ValidateMultipart();
                }
                else if (_body.size() > _body_length)
                    assert(!"There is some overflow that need to be carried on to next request..");
            }
        }
        return HttpStatus(STATUS_OK);
    }
    catch (const RequestException& e)
    {
        return e.getErrorCode();
    }
}

void Request::clear()
{
    _status                          = HttpStatus(STATUS_OK);
    _is_headers_completed            = false;
    _is_body_completed               = false;
    _is_chunked                      = false;
    _remove_chunk_data_trailing_crlf = false;
    _chunk_size                      = 0;
    _remaining_chunk_size            = 0;
    _content_type_header             = NULL;
    _transfer_encoding_header        = NULL;
    _body_fd                         = -1;
    _body_size                       = 0;
    _body.clear();
    _headers.clear();
    _http_version.clear();
    _method.clear();
    _uri.clear();
    _query_params_string.clear();
    _query_params.clear();
    _stream_buf.str("");
    _stream_buf.seekg(0);
}

const std::string Request::getMethod() const
{
    return _method;
}

const HttpStatus& Request::getStatus() const
{
    return _status;
}

bool Request::isCompleted() const
{
    return _status.code != STATUS_OK || (_is_headers_completed && _is_body_completed);
}
bool Request::isChunked() const
{
    return _is_chunked;
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

const HttpHeader* Request::getContentTypeHeader() const
{
    return _content_type_header;
}

const std::vector<uint8_t>& Request::getBody() const
{
    return _body;
}
int Request::getBodyFd() const
{
    return _body_fd;
}
size_t Request::getBodySize() const
{
    return _body_size;
}
const std::map<std::string, std::string>& Request::getQueryParams() const
{
    return _query_params;
}
const std::string& Request::getQueryParamsString() const
{
    return _query_params_string;
}

const std::stringstream& Request::getRawBuffer() const
{
    return _stream_buf;
}

void Request::setMethod(const std::string& method)
{
    _method = method;
}

void Request::setUri(const std::string& uri)
{
    if (uri.empty() || uri[0] != '/')
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST));
    _uri = uri;
}

void Request::setVersion(const std::string& version)
{
    if (version.substr(0, 5) != "HTTP/")
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST));

    int major, minor;
    if (std::sscanf(version.c_str() + 5, "%d.%d", &major, &minor) != 2)
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST));
    if (major != 1 || minor != 1)
        throw RequestException(HttpStatus(STATUS_HTTP_VERSION_NOT_SUPPORTED));
    _http_version = version;
}

void Request::setHeader(const HttpHeader& header)
{
    _headers.push_back(header);
}

std::string Request::getHeaderLine()
{
    std::string line;

    std::getline(_stream_buf, line, '\n');
    if (line.empty() || line[line.size() - 1] != '\r')
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST));
    line.erase(line.size() - 1);

    if (std::string(" \t").find(_stream_buf.peek()) != std::string::npos)
    {
        _stream_buf.ignore();
        line += ' ';
        while (std::string(" \t").find(_stream_buf.peek()) != std::string::npos)
            _stream_buf.ignore();
        line += Request::getHeaderLine();
    }
    return line;
}

void Request::parseRequestLine()
{
    std::string method, uri, version;

    if (!std::getline(_stream_buf, method, ' ') || !std::getline(_stream_buf, uri, ' '))
    {
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST));
    }

    if (uri.find('?') != std::string::npos)
    {
        _query_params_string = uri.substr(uri.find('?') + 1);
        uri                  = uri.substr(0, uri.find('?'));
        _query_params        = parseQueryParams(_query_params_string);
    }

    if (!std::getline(_stream_buf, version, '\n') || version[version.size() - 1] != '\r')
    {
        throw RequestException(HttpStatus(STATUS_BAD_REQUEST));
    }

    version.erase(version.size() - 1);

    setMethod(method);
    setUri(uri);
    setVersion(version);
}

std::map<std::string, std::string> Request::parseQueryParams(const std::string query)
{
    std::map<std::string, std::string> params;

    std::string::const_iterator start = query.begin();
    std::string::const_iterator it    = query.begin();
    std::string                 key, value;
    for (; it != query.end(); ++it)
    {
        if (*it == '=' && start != it)
        {
            key   = utils::strtrim(std::string(start, it));
            start = it + 1;
        }
        else if ((*it == '&' || it + 1 == query.end()) && !key.empty())
        {
            value       = utils::strtrim(std::string(start, it));
            params[key] = value;
            start       = it + 1;
            key.clear();
            value.clear();
        }
    }
    return params;
}

void Request::parseHeaders()
{
    // check the size of the header fields
    size_t current_pos = _stream_buf.tellg();
    _stream_buf.seekg(0, std::ios::end);
    if ((size_t)_stream_buf.tellg() - current_pos > REQUEST_HEADER_LIMIT)
    {
        throw RequestException(HttpStatus(STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE));
    }
    _stream_buf.seekg(current_pos);

    std::string line;
    while (!(line = Request::getHeaderLine()).empty())
    {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
            throw RequestException(HttpStatus(STATUS_BAD_REQUEST));
        std::string name  = utils::strtrim(line.substr(0, pos)),
                    value = utils::strtrim(line.substr(pos + 1));
        HttpHeader header(name, value);
        setHeader(header);
    }
    // TODO: check for the required host headers (the server instance is needed)
}

HttpStatus Request::ValidateMultipart()
{
    if (_content_type_header != NULL && _content_type_header->values.size() &&
        _content_type_header->values.front().value == "multipart/form-data")
    {
        if (_content_type_header->values.size() < 2)
            return HttpStatus(STATUS_BAD_REQUEST);
    }
    return HttpStatus(STATUS_OK);
}

std::ostream& operator<<(std::ostream& os, const Request& request)
{
    os << "===========================================================\n";
    os << "Method: " << request.getMethod() << '\n';
    os << "URI: " << request.getUri() << '\n';
    os << "Query string: " << request.getQueryParamsString() << '\n';
    os << "Query params: " << '\n';
    for (size_t i = 0; i < request.getQueryParams().size(); i++)
    {
        os << "- " << request.getQueryParams().begin()->first << " = "
           << request.getQueryParams().begin()->second << '\n';
    }
    os << "HTTP version: " << request.getVersion() << '\n';
    os << "Headers:\n";

    size_t max_length = 0;

    std::vector<HttpHeader>::const_iterator it;
    for (it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
    {
        max_length = std::max(max_length, it->name.length());
    }

    for (it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
    {
        os << *it << '\n';
    }
    os << "Body: ";
    if (request.getBody().empty())
        os << "<empty>\n";
    else
        os << request.getBody().data() << '\n';
    os << "===========================================================" << std::endl;
    return os;
}

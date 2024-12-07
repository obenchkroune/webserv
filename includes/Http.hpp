#pragma once

#include "Utils.hpp"
#include "shared.hpp"
#include <map>
#include <stdint.h>
#include <string>

enum HttpMethod
{
    HTTP_ANY = 0,
    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_PATCH,
};

struct HttpHeader
{
    std::string              name;
    std::string              value;
    std::vector<std::string> parameters;
};

#define HTTP_VERSION_TOKEN "HTTP/1.1"
#define CRLF               "\r\n"

// 100 -> 102
#define HTTP_STATUS_CONTINUE            "100 Continue"
#define HTTP_STATUS_SWITCHING_PROTOCOLS "101 Switching Protocols"
#define HTTP_STATUS_PROCESSING          "102 Processing"

// 200 -> 206
#define HTTP_STATUS_OK                            "200 OK"
#define HTTP_STATUS_CREATED                       "201 Created"
#define HTTP_STATUS_ACCEPTED                      "202 Accepted"
#define HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION "203 Non-Authoritative Information"
#define HTTP_STATUS_NO_CONTENT                    "204 No Content"
#define HTTP_STATUS_RESET_CONTENT                 "205 Reset Content"
#define HTTP_STATUS_PARTIAL_CONTENT               "206 Partial Content"

// 300 -> 305 , 307 -> 308
#define HTTP_STATUS_MULTIPLE_CHOICES   "300 Multiple Choices"
#define HTTP_STATUS_MOVED_PERMANENTLY  "301 Moved Permanently"
#define HTTP_STATUS_FOUND              "302 Found"
#define HTTP_STATUS_SEE_OTHER          "303 See Other"
#define HTTP_STATUS_NOT_MODIFIED       "304 Not Modified"
#define HTTP_STATUS_USE_PROXY          "305 Use Proxy"
#define HTTP_STATUS_TEMPORARY_REDIRECT "307 Temporary Redirect"
#define HTTP_STATUS_PERMANENT_REDIRECT "308 Permanent Redirect"

// 400 -> 417
#define HTTP_STATUS_BAD_REQUEST                     "400 Bad Request"
#define HTTP_STATUS_UNAUTHORIZED                    "401 Unauthorized"
#define HTTP_STATUS_PAYMENT_REQUIRED                "402 Payment Required"
#define HTTP_STATUS_FORBIDDEN                       "403 Forbidden"
#define HTTP_STATUS_NOT_FOUND                       "404 Not Found"
#define HTTP_STATUS_METHOD_NOT_ALLOWED              "405 Method Not Allowed"
#define HTTP_STATUS_NOT_ACCEPTABLE                  "406 Not Acceptable"
#define HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED   "407 Proxy Authentication Required"
#define HTTP_STATUS_REQUEST_TIMEOUT                 "408 Request Timeout"
#define HTTP_STATUS_CONFLICT                        "409 Conflict"
#define HTTP_STATUS_GONE                            "410 Gone"
#define HTTP_STATUS_LENGTH_REQUIRED                 "411 Length Required"
#define HTTP_STATUS_PRECONDITION_FAILED             "412 Precondition Failed"
#define HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE        "413 Request Entity Too Large"
#define HTTP_STATUS_REQUEST_URI_TOO_LONG            "414 Request-URI Too Long"
#define HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE          "415 Unsupported Media Type"
#define HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE "416 Requested Range Not Satisfiable"
#define HTTP_STATUS_EXPECTATION_FAILED              "417 Expectation Failed"

// 500 -> 505
#define HTTP_STATUS_INTERNAL_SERVER_ERROR      "500 Internal Server Error"
#define HTTP_STATUS_NOT_IMPLEMENTED            "501 Not Implemented"
#define HTTP_STATUS_BAD_GATEWAY                "502 Bad Gateway"
#define HTTP_STATUS_SERVICE_UNAVAILABLE        "503 Service Unavailable"
#define HTTP_STATUS_GATEWAY_TIMEOUT            "504 Gateway Timeout"
#define HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED "505 HTTP Version Not Supported"

enum http_status_code
{
    // 100 - 102
    STATUS_CONTINUE = 100,
    STATUS_SWITCHING_PROTOCOLS,
    STATUS_PROCESSING,
    // 200 - 206
    STATUS_OK = 200,
    STATUS_CREATED,
    STATUS_ACCEPTED,
    STATUS_NON_AUTHORITATIVE_INFORMATION,
    STATUS_NO_CONTENT,
    STATUS_RESET_CONTENT,
    STATUS_PARTIAL_CONTENT,
    // 300 - 305
    STATUS_MULTIPLE_CHOICES = 300,
    STATUS_MOVED_PERMANENTLY,
    STATUS_FOUND,
    STATUS_SEE_OTHER,
    STATUS_NOT_MODIFIED,
    STATUS_USE_PROXY,
    // 307 - 308
    STATUS_TEMPORARY_REDIRECT = 307,
    STATUS_PERMANENT_REDIRECT,
    // 400 - 417
    STATUS_BAD_REQUEST = 400,
    STATUS_UNAUTHORIZED,
    STATUS_PAYMENT_REQUIRED,
    STATUS_FORBIDDEN,
    STATUS_NOT_FOUND,
    STATUS_METHOD_NOT_ALLOWED,
    STATUS_NOT_ACCEPTABLE,
    STATUS_PROXY_AUTHENTICATION_REQUIRED,
    STATUS_REQUEST_TIMEOUT,
    STATUS_CONFLICT,
    STATUS_GONE,
    STATUS_LENGTH_REQUIRED,
    STATUS_PRECONDITION_FAILED,
    STATUS_REQUEST_ENTITY_TOO_LARGE,
    STATUS_REQUEST_URI_TOO_LONG,
    STATUS_UNSUPPORTED_MEDIA_TYPE,
    STATUS_REQUESTED_RANGE_NOT_SATISFIABLE,
    STATUS_EXPECTATION_FAILED,
    // 500 - 505
    STATUS_INTERNAL_SERVER_ERROR = 500,
    STATUS_NOT_IMPLEMENTED,
    STATUS_BAD_GATEWAY,
    STATUS_SERVICE_UNAVAILABLE,
    STATUS_GATEWAY_TIMEOUT,
    STATUS_HTTP_VERSION_NOT_SUPPORTED,
};

struct HttpStatus
{
    http_status_code code;
    const char      *name;
    HttpStatus(const http_status_code &_code, const char *_name) : code(_code), name(_name) {};
    HttpStatus(const HttpStatus &_status) : code(_status.code), name(_status.name) {};
};
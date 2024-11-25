#pragma once

#include "Utils.hpp"
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

// 100 -> 102
#define HTTP_STATUS_CONTINUE            "Continue"
#define HTTP_STATUS_SWITCHING_PROTOCOLS "Switching Protocols"
#define HTTP_STATUS_PROCESSING          "Processing"

// 200 -> 206
#define HTTP_STATUS_OK                            "OK"
#define HTTP_STATUS_CREATED                       "Created"
#define HTTP_STATUS_ACCEPTED                      "Accepted"
#define HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION "Non-Authoritative Information"
#define HTTP_STATUS_NO_CONTENT                    "No Content"
#define HTTP_STATUS_RESET_CONTENT                 "Reset Content"
#define HTTP_STATUS_PARTIAL_CONTENT               "Partial Content"

// 300 -> 305 , 307 -> 308
#define HTTP_STATUS_MULTIPLE_CHOICES   "Multiple Choices"
#define HTTP_STATUS_MOVED_PERMANENTLY  "Moved Permanently"
#define HTTP_STATUS_FOUND              "Found"
#define HTTP_STATUS_SEE_OTHER          "See Other"
#define HTTP_STATUS_NOT_MODIFIED       "Not Modified"
#define HTTP_STATUS_USE_PROXY          "Use Proxy"
#define HTTP_STATUS_TEMPORARY_REDIRECT "Temporary Redirect"
#define HTTP_STATUS_PERMANENT_REDIRECT "Permanent Redirect"

// 400 -> 417
#define HTTP_STATUS_BAD_REQUEST                     "Bad Request"
#define HTTP_STATUS_UNAUTHORIZED                    "Unauthorized"
#define HTTP_STATUS_PAYMENT_REQUIRED                "Payment Required"
#define HTTP_STATUS_FORBIDDEN                       "Forbidden"
#define HTTP_STATUS_NOT_FOUND                       "Not Found"
#define HTTP_STATUS_METHOD_NOT_ALLOWED              "Method Not Allowed"
#define HTTP_STATUS_NOT_ACCEPTABLE                  "Not Acceptable"
#define HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED   "Proxy Authentication Required"
#define HTTP_STATUS_REQUEST_TIMEOUT                 "Request Timeout"
#define HTTP_STATUS_CONFLICT                        "Conflict"
#define HTTP_STATUS_GONE                            "Gone"
#define HTTP_STATUS_LENGTH_REQUIRED                 "Length Required"
#define HTTP_STATUS_PRECONDITION_FAILED             "Precondition Failed"
#define HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE        "Request Entity Too Large"
#define HTTP_STATUS_REQUEST_URI_TOO_LONG            "Request-URI Too Long"
#define HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE          "Unsupported Media Type"
#define HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE "Requested Range Not Satisfiable"
#define HTTP_STATUS_EXPECTATION_FAILED              "Expectation Failed"

// 500 -> 505
#define HTTP_STATUS_INTERNAL_SERVER_ERROR      "Internal Server Error"
#define HTTP_STATUS_NOT_IMPLEMENTED            "Not Implemented"
#define HTTP_STATUS_BAD_GATEWAY                "Bad Gateway"
#define HTTP_STATUS_SERVICE_UNAVAILABLE        "Service Unavailable"
#define HTTP_STATUS_GATEWAY_TIMEOUT            "Gateway Timeout"
#define HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED "HTTP Version Not Supported"

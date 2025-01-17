#pragma once

#include "Utils.hpp"
#include "shared.hpp"
#include <map>
#include <stdint.h>
#include <string>

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

// 400 -> 418
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
#define HTTP_STATUS_IM_A_TEAPOT                     "418 I'm a teapot"
// 421 -> 429
#define HTTP_STATUS_MISDIRECTED_REQUEST   "421 Misdirected Request"
#define HTTP_STATUS_UNPROCESSABLE_ENTITY  "422 Unprocessable Entity"
#define HTTP_STATUS_LOCKED                "423 Locked"
#define HTTP_STATUS_FAILED_DEPENDENCY     "424 Failed Dependency"
#define HTTP_STATUS_TOO_EARLY             "425 Too Early"
#define HTTP_STATUS_UPGRADE_REQUIRED      "426 Upgrade Required"
#define HTTP_STATUS_PRECONDITION_REQUIRED "428 Precondition Required"
#define HTTP_STATUS_TOO_MANY_REQUESTS     "429 Too Many Requests"

// 431, 451
#define HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE "431 Request Header Fields Too Large"
#define HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS   "451 Unavailable For Legal Reasons"

// 500 -> 505
#define HTTP_STATUS_INTERNAL_SERVER_ERROR      "500 Internal Server Error"
#define HTTP_STATUS_NOT_IMPLEMENTED            "501 Not Implemented"
#define HTTP_STATUS_BAD_GATEWAY                "502 Bad Gateway"
#define HTTP_STATUS_SERVICE_UNAVAILABLE        "503 Service Unavailable"
#define HTTP_STATUS_GATEWAY_TIMEOUT            "504 Gateway Timeout"
#define HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED "505 HTTP Version Not Supported"

enum HttpStatusCode
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
    // 400 - 418
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
    STATUS_IM_A_TEAPOT,
    // 421 - 429
    STATUS_MISDIRECTED_REQUEST = 421,
    STATUS_UNPROCESSABLE_ENTITY,
    STATUS_LOCKED,
    STATUS_FAILED_DEPENDENCY,
    STATUS_TOO_EARLY,
    STATUS_UPGRADE_REQUIRED,
    STATUS_PRECONDITION_REQUIRED,
    STATUS_TOO_MANY_REQUESTS,

    // 431, 451
    STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE,
    STATUS_UNAVAILABLE_FOR_LEGAL_REASONS,

    // 500 - 505
    STATUS_INTERNAL_SERVER_ERROR = 500,
    STATUS_NOT_IMPLEMENTED,
    STATUS_BAD_GATEWAY,
    STATUS_SERVICE_UNAVAILABLE,
    STATUS_GATEWAY_TIMEOUT,
    STATUS_HTTP_VERSION_NOT_SUPPORTED,
    // INTERNAL WEBSERV IMPLEM
    STATUS_HTTP_INTERNAL_IMPLEM_AUTO_INDEX = 1000,
};

struct HttpStatus
{
    HttpStatusCode code;
    const char*    message;

    HttpStatus();
    HttpStatus(const HttpStatus& _status);
    HttpStatus(const HttpStatusCode code);
};

const char* get_status_message(HttpStatusCode status);
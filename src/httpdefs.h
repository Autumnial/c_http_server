#ifndef HTTPDEFS_H
#define HTTPDEFS_H
#include "include/LinkedList.h"

typedef enum {
    CONTINUE = 100,
    SWITCHING_PROTOCOF = 101,
    PROCESSING = 102,
    EARLY_HINTS = 103,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTI_STATUS = 207,
    ALREADY_REPORTED = 208,
    IM_USED = 226,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    USE_PROXY = 305,
    TEMPORARY_REDIRECT = 307,
    PERMANENT_REDIRECT = 308,
    BAD_REQUEST = 400,
    UNAUTHORISED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_AVAILABLE = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAIL = 412,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    IM_A_TEAPOT = 418,
    MISDIRECTED_REQUEST = 421,
    UNPROCESSABLE_CONTENT = 422,
    LOCKED = 423,
    FAILED_DEPENDENCY = 424,
    TOO_EARLY = 425,
    UPGRADE_REQUIRED = 426,
    PRECONDITION_REQUIRED = 428,
    TOO_MANY_REQUESTS = 429,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    UNAVAILABLE_FOR_LEGAL_REASONS = 451,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    INSUFFICIENT_STORAGE = 507,
    LOOP_DETECTED = 508,
    NETWORK_AUTHENTICATION_REQUIRED = 511
} StatusCode;

typedef enum {
    HTTP_CONNECT,
    HTTP_DELETE,
    HTTP_GET,
    HTTP_HEAD,
    HTTP_OPTIONS,
    HTTP_PATCH,
    HTTP_POST,
    HTTP_PUT,
    HTTP_TRACE
} Method;

// represents a HTTP header
typedef struct {
    char *key;
    char *value;
} Header;

typedef struct {
    char  *route;
    Method method;

} Path;
// represents a HTTP request.
typedef struct {
    //  INFO: should only be used with `Header` structs;
    LinkedList *headers;
    char       *body;
    Path        path;
} Request;

typedef struct {
    StatusCode  status;
    char       *message;
    LinkedList *headers;
    char       *body;
} Response;

typedef struct {
    char    *route;
    Method   method;
    Response (*exec)(Request);
} Route;

char    *build_response(Response r);
Path    *parse_path(char *line);
Response not_found();
Response ok();
int      cmp_headers(void *, void *);
int      cmp_routes(void *, void *);
#endif // !HTTPDEFS_H

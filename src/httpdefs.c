#include "httpdefs.h"
#include "include/LinkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Path *parse_path(char *line) {
    char *method_str = line;

    while (*line != '\0') {
        if (*line == ' ') {
            *line = '\0';
            line++;
            break;
        }
        line++;
    }

    Method method;

    if (strcmp(method_str, "CONNECT") == 0) {
        method = HTTP_CONNECT;
    } else if (strcmp(method_str, "DELETE") == 0) {
        method = HTTP_DELETE;
    } else if (strcmp(method_str, "GET") == 0) {
        method = HTTP_GET;
    } else if (strcmp(method_str, "HEAD") == 0) {
        method = HTTP_HEAD;
    } else if (strcmp(method_str, "OPTIONS") == 0) {
        method = HTTP_OPTIONS;
    } else if (strcmp(method_str, "PATCH") == 0) {
        method = HTTP_PATCH;
    } else if (strcmp(method_str, "POST") == 0) {
        method = HTTP_POST;
    } else if (strcmp(method_str, "PUT") == 0) {
        method = HTTP_PUT;
    } else if (strcmp(method_str, "TRACE") == 0) {
        method = HTTP_TRACE;
    } else {
        return NULL;
    }

    char *route = line;

    while (*line != '\0') {
        if (*line == ' ') {
            *line = '\0';
            line++;
            break;
        }
        line++;
    }

    Path *path = malloc(sizeof(Path));
    path->method = method;
    path->route = route;

    return path;
}

Response not_found() {

    LinkedList *headers = ll_new_linked_list(cmp_headers);

    Response resp = {.status = NOT_FOUND,
                     .headers = headers,
                     .body = " ",
                     .message = "Not Found"};
    return resp;
}

Response ok() {
    LinkedList *h = ll_new_linked_list(cmp_headers);

    Header* content = malloc(sizeof(Header));

    content->key = "Content-Type"; 
    content->value = "text/html"; 

    ll_push(h, content);

    Response resp = {.status = OK, .headers = h, .body = NULL, .message = "OK"};

    return resp;
}

char *build_response(Response r) {

    char *message = calloc(1, 4096);

    strcat(message, "HTTP/1.1 ");

    // first line, with version, status, and message.
    char *status_code_str = malloc(5);
    snprintf(status_code_str, 5, "%d ", r.status);

    strcat(message, status_code_str);
    strcat(message, r.message);
    strcat(message, "\n");

    // headers

    if (r.headers->length != 0) {

        _ll_Node *node = r.headers->_head;
        while (node != NULL) {

            Header head = (*(Header *)node->data);

            int keylen = strlen(head.key);
            int valuelen = strlen(head.key);
            // + 2 for ': ', one more for '\n', and a last one for null
            // terminator
            int totallen = keylen + valuelen + 4;

            char *header_str = malloc(totallen);
            snprintf(header_str, totallen, "%s: %s\n", head.key, head.value);
            strcat(message, header_str);

            node = node->next;
        }
    }

    // Content-Length header + body.

    int body_len = strlen(r.body);
    int body_len_str_len = snprintf(NULL, 0, "%d", body_len);
    // + 2 for '\n\n', + 1 for null terminator.
    char *body_len_str = malloc(body_len_str_len + 3);

    snprintf(body_len_str, body_len_str_len + 3, "%d\n\n", body_len);

    strcat(message, "Content-Length: ");
    strcat(message, body_len_str);
    strcat(message, r.body);

    return message;
}

// a: header.
// b: char ptr to start of a string containing headers name.
int cmp_headers(void *a, void *b) {
    if (strcmp((*(Header *)a).key, (char *)b) == 0) {
        return 1;
    } else {
        return 0;
    }
}

// a: route.
// b: char ptr to start of a string containing requested route.
int cmp_routes(void *a, void *b) {
    if (strcmp((*(Route *)a).route, (char *)b) == 0) {
        return 1;
    } else {
        return 0;
    }
}

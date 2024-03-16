#include <WinSock2.h>
#include <handleapi.h>
#include <iso646.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <oleauto.h>
#include <processthreadsapi.h>
#include <rpcdce.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <synchapi.h>
#include <winbase.h>
#include <windows.h>
#include <winnt.h>

#include "httpdefs.h"
#include "include/LinkedList.h"
#include "include/queue.h"

#define PORT 9876
#define THREAD_COUNT 8

HANDLE             threadpool[THREAD_COUNT] = {0};
CONDITION_VARIABLE new_connection_ready;
CRITICAL_SECTION   socket_section;
CRITICAL_SECTION   routes_section;

LinkedList *routes;
Queue      *connection_queue;

void         handle_connection(int);
Request      parse_request(char *);
Response     handle_request(Request);
LinkedList  *parse_headers(char *);
DWORD WINAPI fetch_task(LPVOID);
Header      *get_header(char *header_key, LinkedList *);
int          cmp_headers(void *a, void *);
int          cmp_routes(void *a, void *);
Response     say_hi();

int main(void) {
    int sockfd;
    InitializeConditionVariable(&new_connection_ready);
    InitializeCriticalSection(&socket_section);
    InitializeCriticalSection(&routes_section);

    routes = ll_new_linked_list(cmp_routes);
    connection_queue = q_new_queue();

    Route index = {
        .route = "/",
        .exec = say_hi,
        .method = HTTP_GET,
    };

    ll_push(routes, &index);

    for (int i = 0; i < THREAD_COUNT; i++) {
        threadpool[i] = CreateThread(NULL, 0, fetch_task, NULL, 0, NULL);
    }

    WSADATA wsdata;

    if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_addr.S_un.S_addr = ADDR_ANY;
    ;
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        return -2;
    }

    printf("Ready for connection!\n");

    listen(sockfd, 50);
    while (1) {

        int *cli_fd = malloc(sizeof(int));
        *cli_fd = accept(sockfd, 0, 0);

        EnterCriticalSection(&socket_section);
        q_push(connection_queue, cli_fd);
        LeaveCriticalSection(&socket_section);

        WakeConditionVariable(&new_connection_ready);
    }

    return 0;
}

DWORD WINAPI fetch_task(LPVOID param) {

    while (1) {
        int client_fd;

        EnterCriticalSection(&socket_section);

        while (connection_queue->size == 0) {
            SleepConditionVariableCS(&new_connection_ready, &socket_section,
                                     INFINITE);
        }
        int *cli_point = (int *)q_pop(connection_queue);

        if (cli_point != NULL) {
            client_fd = *cli_point;
        }
        LeaveCriticalSection(&socket_section);

        // do shit with the client

        handle_connection(client_fd);
        free(cli_point);
    }

    return 0;
};

void handle_connection(int client_fd) {

    while (1) {
        char recieved[4096] = {0};
        int  res = recv(client_fd, recieved, 1024, 0);

        if (res == 0) {
            printf("dropped connection\n");
            closesocket(client_fd);
            return;
        }

        if (strcmp(recieved, "") == 0) {
            continue;
        }

        Request req = parse_request(&recieved[0]);

        Response resp = handle_request(req);

        char *message = build_response(resp);

        send(client_fd, message, strlen(message), 0);

        memset(message, 0, 4096);
        memset(recieved, 0, 4096);
    }
}

Request parse_request(char *recieved) {

    char *body = strstr(recieved, "\r\n\r\n");
    body = &body[4];

    char  r = '\r';
    char *_ = strtok(recieved, &r);

    char *path = &recieved[0];

    Path *path_obj = parse_path(path);

    if (path_obj == NULL) {
        // TODO: Generate 400

        exit(-1);
    }

    LinkedList *headers = parse_headers(recieved);

    Request req = {.headers = headers, .body = body, .path = *path_obj};

    return req;
}

Response handle_request(Request req) {
    Response resp;
    Route   *route = NULL;

    EnterCriticalSection(&routes_section);

    _ll_Node *route_node = routes->_head;

    while (route_node != NULL) {
        Route *check_route = (Route *)route_node->data;

        if (req.path.method != check_route->method) {
            route_node = route_node->next;
            continue;
        };

        if (strcmp(req.path.route, check_route->route) == 0) {
            route = check_route;
            break;
        }

        route_node = route_node->next;
    }

    if (route == NULL) {
        return not_found();
    }

    LeaveCriticalSection(&routes_section);

    resp = route->exec(req);

    return resp;
}

LinkedList *parse_headers(char *buff) {
    LinkedList *headers = ll_new_linked_list(cmp_headers);

    char r = '\r';

    char *header;
    header = strtok(NULL, &r);

    while (header != NULL && header[2] != '\n') {

        char *key = header;
        char *value = header;

        // loop over the line until it ends or we find :,
        // once we find :, replace it with END-OF-TEXT character,
        // move over one more char so that we actually get the value
        while (*value != '\0') {
            value++;
            if (*value == ':') {
                *value = '\0';
                value++;
                break;
            }
        }

        Header *h = malloc(sizeof(Header));
        h->key = key;
        h->value = value;

        ll_push(headers, h);

        header = strtok(NULL, &r);
    }

    return headers;
}

Header *get_header(char *header_key, LinkedList *headers) {
    return ll_get_by_value(headers, header_key);
}

Response say_hi() {
    Response response = ok();
    response.body = "Haiii :3 ";

    return response;
}

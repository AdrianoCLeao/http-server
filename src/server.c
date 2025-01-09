#include "../include/server.h"
#include "../include/html_handler.h"
#include "../include/responses/not_implemented.h"
#include "../include/log.h"
#include "../include/metrics.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSESOCKET closesocket
    #define inet_ntop InetNtopA 
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #define CLOSESOCKET close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 4000
#define BUFFER_SIZE 4096

void handle_request(int client_socket, const char *request, const char *client_ip) {
    char method[16], route[256];
    sscanf(request, "%s %s", method, route);

    write_log(method, route, client_ip);

    if (strcmp(method, "GET") != 0) {
        NotImplemented(client_socket); 
        return;
    }

    if (strcmp(route, "/hello") == 0) {
        const char *response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 13\r\n\r\n"
                               "Hello, World!";
        send(client_socket, response, strlen(response), 0);
    } else {
        const char *file_name = route[1] ? route + 1 : "index.html";
        serve_html(client_socket, file_name);
    }
}

void start_server() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
#endif

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket creation failed");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        CLOSESOCKET(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        CLOSESOCKET(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    printf("Server running on http://localhost:%d\n", PORT);

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        char client_ip[INET_ADDRSTRLEN];

        int new_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        if (inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip)) == NULL) {
            perror("Failed to retrieve client IP");
            strcpy(client_ip, "Unknown");
        }

#ifdef _WIN32
        int bytes_read = recv(new_socket, buffer, BUFFER_SIZE, 0);
#else
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
#endif

        if (bytes_read > 0) {
            handle_request(new_socket, buffer, client_ip);
        }

        CLOSESOCKET(new_socket);
    }

    CLOSESOCKET(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
}

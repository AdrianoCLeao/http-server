#include "../../include/responses/not_found.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <unistd.h>
#endif

#define BUFFER_SIZE 4096
#define NOT_FOUND_PAGE "assets/responses/404.html"

void NotFoundException(int client_socket) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen(NOT_FOUND_PAGE, "r");

    if (!file) {
        const char *fallback_response = "HTTP/1.1 404 Not Found\r\n"
                                        "Content-Type: text/plain\r\n"
                                        "Content-Length: 35\r\n\r\n"
                                        "404 Not Found - Custom page missing";
        send(client_socket, fallback_response, strlen(fallback_response), 0);
        return;
    }

    struct stat file_stat;
    if (stat(NOT_FOUND_PAGE, &file_stat) < 0) {
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 21\r\n\r\n"
                                     "Internal Server Error";
        send(client_socket, error_response, strlen(error_response), 0);
        fclose(file);
        return;
    }

    const char *header_template = "HTTP/1.1 404 Not Found\r\n"
                                  "Content-Type: text/html\r\n"
                                  "Content-Length: %ld\r\n\r\n";
    snprintf(buffer, sizeof(buffer), header_template, file_stat.st_size);
    send(client_socket, buffer, strlen(buffer), 0);

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

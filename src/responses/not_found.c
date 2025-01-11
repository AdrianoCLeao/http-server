#include "../../include/responses/not_found.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <unistd.h>
#endif

#define BUFFER_SIZE 4096
#define NOT_FOUND_PAGE "static/responses/404.html"

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

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size <= 0) {
        fclose(file);
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 21\r\n\r\n"
                                     "Internal Server Error";
        send(client_socket, error_response, strlen(error_response), 0);
        return;
    }

    memset(buffer, 0, sizeof(buffer)); 
    snprintf(buffer, sizeof(buffer),
             "HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %ld\r\n\r\n",
             file_size);
    send(client_socket, buffer, strlen(buffer), 0);

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        memset(buffer + bytes_read, 0, sizeof(buffer) - bytes_read); 
        size_t total_sent = 0;

        while (total_sent < bytes_read) {
            int sent = send(client_socket, buffer + total_sent, bytes_read - total_sent, 0);
            if (sent < 0) {
                fclose(file);
                return;
            }
            total_sent += sent;
        }
    }

    fclose(file);
}

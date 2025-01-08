#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <winsock2.h>
    #define CLOSESOCKET closesocket
#else
    #include <unistd.h>
    #define CLOSESOCKET close
#endif

#define BUFFER_SIZE 4096
#define DOCUMENT_ROOT "../assets" 

void serve_html(int client_socket, const char *file_name) {
    char file_path[BUFFER_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/%s", DOCUMENT_ROOT, file_name);

    FILE *file = fopen(file_path, "r");
    if (!file) {
        const char *not_found_response = "HTTP/1.1 404 Not Found\r\n"
                                         "Content-Type: text/plain\r\n"
                                         "Content-Length: 13\r\n\r\n"
                                         "404 Not Found";
        send(client_socket, not_found_response, strlen(not_found_response), 0);
        return;
    }

    struct stat file_stat;
    if (stat(file_path, &file_stat) < 0) {
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 21\r\n\r\n"
                                     "Internal Server Error";
        send(client_socket, error_response, strlen(error_response), 0);
        fclose(file);
        return;
    }

    const char *header_template = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/html\r\n"
                                  "Content-Length: %ld\r\n\r\n";
    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header), header_template, file_stat.st_size);
    send(client_socket, header, strlen(header), 0);

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

#ifdef _WIN32
    #include <winsock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSESOCKET closesocket
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #define CLOSESOCKET close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SERVER_PORT 4000
#define SERVER_ADDR "127.0.0.1"
#define BUFFER_SIZE 1024

int connect_to_server();
void test_connection();
void test_server_response();

int main()
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }
#endif

    printf("Running unit and integration tests...\n");
    test_connection();
    test_server_response();
    printf("All tests passed successfully!\n");

#ifdef _WIN32
    WSACleanup();
#endif

    return EXIT_SUCCESS;
}

// Function to connect to the server
int connect_to_server()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDR, &server_address.sin_addr) <= 0)
    {
        perror("Invalid address or address not supported");
        CLOSESOCKET(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection to server failed");
        CLOSESOCKET(sock);
        return -1;
    }

    return sock;
}

// Unit test: Verify connection to the server
void test_connection()
{
    printf("Testing server connection...\n");
    int sock = connect_to_server();
    assert(sock >= 0); 
    printf("Connection test passed!\n");
    CLOSESOCKET(sock);
}

// Integration test: Send a request and validate the response
void test_server_response()
{
    printf("Testing server response...\n");
    int sock = connect_to_server();
    assert(sock >= 0);

    const char *request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    char buffer[BUFFER_SIZE] = {0};

    if (send(sock, request, strlen(request), 0) < 0)
    {
        perror("Failed to send request");
        CLOSESOCKET(sock);
        assert(0); 
    }

    int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0)
    {
        perror("Failed to receive response");
        CLOSESOCKET(sock);
        assert(0);
    }

    buffer[bytes_received] = '\0'; 
    printf("Server response:\n%s\n", buffer);
    assert(strstr(buffer, "HTTP/1.1") != NULL); 

    printf("Response test passed!\n");
    CLOSESOCKET(sock);
}

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

#define SERVER_PORT 4000
#define SERVER_ADDR "127.0.0.1"
#define BUFFER_SIZE 1024

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

    int sock;
    struct sockaddr_in server_address;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
#ifdef _WIN32
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDR, &server_address.sin_addr) <= 0)
    {
        perror("Invalid address or address not supported");
        CLOSESOCKET(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection to server failed");
        CLOSESOCKET(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    printf("Server is up and running!\n");

    CLOSESOCKET(sock);
#ifdef _WIN32
    WSACleanup();
#endif

    return EXIT_SUCCESS;
}

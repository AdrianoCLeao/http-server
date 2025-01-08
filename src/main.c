#include <stdio.h>
#include <stdlib.h>
#include "../include/server.h"

int main()
{
    printf("Starting server...\n");
    start_server();
    printf("Server stopped.\n");

    return 0;
}

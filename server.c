#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define PORT 4761
#define MAX_CLIENTS 10

//Client structure
typedef struct Client {
    struct sockaddr_in address;
    int sock_fd;
    char name[30];
} Client;


int main(int argc, char **argv) {
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    unsigned int listen_fd, connection_fd;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { 
        printf("Error:socket()");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if ((bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) != 0) {
        printf("Error:bind()");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully bound");

    if ((listen(listen_fd, 10)) < 0) {
        printf("Error:listen()");
        exit(EXIT_FAILURE);
    }
    printf("Server listening");
    printf("   -> CChat <-   ");

    while (1) {
        socklen_t client_length = sizeof(client_addr);
        connection_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_length);

        Client *client = (Client *) malloc(sizeof(Client));
        client->address = client_addr;
        client->sock_fd = connection_fd;
    }
}

//gcc -Wall -o server server.c
// ./server

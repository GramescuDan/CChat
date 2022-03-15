#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define PORT 5000
#define PORT_CLIENT 5001
#define MAX_CLIENTS 5
#define BUFMAX 4096

typedef enum {True, False} Boolean;

//Client structure
typedef struct Connection {
    struct sockaddr_in address;
    int sock_fd;
} Connection;

typedef struct Client {
    char *username;
    char *password;
    Connection *connection;
} Client;

Client clientsDB[MAX_CLIENTS] = {
    {"admin", "admin"},
    {"dragos", "dragos"},
    {"dan", "dan"},
    {"ioana", "ioana"},
    {"august", "august"},
};

void *custom_alloc(int size) {
    char mem = (char)malloc(size);
    if(mem == NULL) {
        fprintf(stderr, "Error memory allocation");
        exit(EXIT_FAILURE);
    }
    return (void*)mem;
}

Connection *connected_clients[MAX_CLIENTS];

Client *verifyCredentials(char *credentials) {
    char *token = strtok(credentials," ");
    for(int i =0; i < MAX_CLIENTS; i++) {
        if (strcmp(clientsDB[i].username, token) == 0) {
            token = strtok(NULL, " ");
            if (strcmp(token, clientsDB[i].password) == 0) {
                // lock on 
                Client client = (Client)custom_alloc(sizeof(Client));
                client->username = (char*)custom_alloc(sizeof(strlen(clientsDB[i].username) + 1));
                client->password = (char*)custom_alloc(sizeof(strlen(clientsDB[i].password) + 1));

                client->connection = (Connection*)custom_alloc(sizeof(Connection));

                strcpy(client->username, clientsDB[i].username);
                strcpy(client->password, clientsDB[i].password);

                // lock off
                return client;
            }
        }
    }
    return NULL;
}

void clear_buffer(char *buffer, int size) {
    for(int i = 0; i < size; i++) {
        buffer[i] = 0;
    }
}

Boolean send_login_confirmation(Client *client) {
    char *login_message = "1";
    return send(client->connection->sock_fd, login_message, sizeof(login_message), 0) > 0 ? True : False;
}

// aloca o zona de memorie cu dimensiune specificata ca parametru
int main(int argc, char **argv) {

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    unsigned int listen_fd, connection_fd;
    char buff[BUFMAX];

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { 
        printf("Error:socket()");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT_CLIENT);

    if ((bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) != 0) {
        printf("Error:bind()");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully bound\n");

    if ((listen(listen_fd, MAX_CLIENTS)) < 0) {
        printf("Error:listen()");
        exit(EXIT_FAILURE);
    }
    printf("Server listening\n");
    printf("   -> CChat <-   \n");

    while (1) {
        socklen_t client_length = sizeof(client_addr);

        connection_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_length);
        if(connection_fd < 0) {
            printf("Error:accept1");
            exit(EXIT_FAILURE);
        }
        printf("Accept succes\n");

        clear_buffer(buff, BUFMAX);

        int read_message = read(connection_fd, buff, BUFMAX);
        if(read_message < 0) {
            printf("Error:read");
            exit(EXIT_FAILURE);
        }
        printf("Read success\n");

        char op = buff[0];
        for( int i = 1; i< strlen(buff)+1; i++) {
            buff[i-1] = buff[i];
        }

        if(op == '~') {
            
            Client *authorized_client = verifyCredentials(buff);

            if(authorized_client == NULL) {
                // send error message to client

                // int login_failed_fd = accept(connection_fd, (struct sockaddr *) &client_addr, &client_length);

                // if(login_failed_fd < 0) {
                //     printf("Error:accept2");
                //     exit(EXIT_FAILURE);
                // }

                clear_buffer(buff,BUFMAX);
                strcat(buff, "Loggin failed");

                int login_failed_write = write(connection_fd, &buff, strlen(buff));
                if(login_failed_write < 0){
                    printf("Error:Read()");
                    exit(EXIT_FAILURE);
                }

                if (send(connection_fd, buff, strlen(buff), 0) <0 ) {
                    printf("Error:send()");
                    exit(EXIT_FAILURE);
                }
                printf("Verificare invalida\n");

            } else {

                printf("User %s logged in\n", authorized_client->username);

                printf("After verify\n");

                authorized_client->connection->address = client_addr;
                authorized_client->connection->sock_fd = connection_fd;

                printf("Before send\n");

                send_login_confirmation(authorized_client);

                printf("After send\n");
            }
        } else if(op == '!') {
            printf("Am primit mesaj");
            strcat(buff,buff+1);
        } else {
            printf("Nici logare, nici mesaj");
            exit(EXIT_FAILURE);
        }

        // verifici cu gramescu daca merge serveru -> client , client <- server
        // 
        // listen on port (socket) -> buffer contains "message" -> message and send to all auth clients

        // threads
        // every thread has its own buffer to avoid racing condition
        // every intercepted listened message starts a thread with functions that does auth + message reception + message send to all clients
        // use mutexes on clientDB when write into global DB
        // no need for locks if you use isolated buffers.

        // NU UITA SA DEALOCI TOATA MEMORIA
    }
}

//gcc -Wall -o server server.c
// ./server
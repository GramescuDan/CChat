#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define PORT 4761
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
    char *mem = (char*)malloc(size);
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
            if(strcmp(clientsDB[i].password, token) == 0) {
                // lock on 

                printf("TESTARE1\n");

                Client *client = (Client*)custom_alloc(sizeof(Client));
                client->username = (char*)custom_alloc(sizeof(strlen(clientsDB[i].username) + 1));
                client->password = (char*)custom_alloc(sizeof(strlen(clientsDB[i].password) + 1));

                client->connection = (Connection*)custom_alloc(sizeof(Connection));

                printf("TESTARE2\n");
                strcpy(client->username, clientsDB[i].username);
                strcpy(client->password, clientsDB[i].password);

                // lock off

                return client;
            }
        }
    }
    return NULL;
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
    server_addr.sin_port = htons(PORT);

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
            printf("Error:accept");
            exit(EXIT_FAILURE);
        }
        printf("Accept succesfull\n");

        int read_message = read(connection_fd, buff, BUFMAX);
        if(read_message < 0) {
            printf("Error:read");
            exit(EXIT_FAILURE);
        }
        printf("%s\n",buff);
        printf("Read success\n");
        

        Client *authorized_client = verifyCredentials(buff);

        if(authorized_client == NULL) {
            // send error message to client
            // remetea's homework
        }
        printf("Verify success\n");

        authorized_client->connection->address = client_addr;
        authorized_client->connection->sock_fd = connection_fd;

        printf("Before send\n");

        send_login_confirmation(authorized_client);

        printf("After send\n");


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

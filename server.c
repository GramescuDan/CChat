#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_PORT 5000
#define MAX_CLIENTS 5
#define BUFFER_SIZE 512

typedef struct Client{
    int socket_fd;
    struct sockaddr client_addr;
    socklen_t client_length;
} Client;

typedef struct {
    char username[BUFFER_SIZE / 8];
    char password[BUFFER_SIZE / 8];
    Client *client; // NULL - unauthorized | !NULL - authorized
} Database;

Database database[MAX_CLIENTS] = {
    {"admin", "admin", NULL},
    {"dragos", "dragos", NULL},
    {"dan", "dan", NULL},
    {"ioana", "ioana", NULL},
    {"august", "august", NULL},
};

pthread_mutex_t database_mutex = PTHREAD_MUTEX_INITIALIZER;

// ~ login
// ! mesaj
void *client_handler(void *client_data) {
    Client client = (Client)client_data;
    char buffer[BUFFER_SIZE] = {0};

    if(read(client.socket_fd, buffer, BUFFER_SIZE) < 0) {
        printf("Error:read()\n");
        exit(EXIT_FAILURE);
    }


    const char op = buffer[0];
    for(int i = 1; i <= strlen(buffer); i++) {
        buffer[i] = buffer[i + 1];
    }
    switch(op) {
        case '!':
            for(int i = 0; i < MAX_CLIENTS; i++) {
                if(database[i].client != NULL) {
                    write(client.socket_fd, buffer, strlen(buffer));
                    //send
                }
            }
            //send message to all
            break;

        case '~':;
            char *token = strtok(buffer, " ");
            for(int i = 0; i < MAX_CLIENTS; i++) {
                if(!strcmp(database[i].username, token)){
                    token = strtok(NULL, " ");
                    if(!strcmp(database[i].password, token)){
                        pthread_mutex_lock(&database_mutex);
                        database[i].client = &client;
                        pthread_mutex_unlock(&database_mutex);
                        break;
                    } else {
                        //sent password is incorrect
                    }
                }
                if (i == MAX_CLIENTS - 1) { 
                    //sent user not found
                } 
            }    
            //send succesful logging in
            bzero(buffer, BUFFER_SIZE);
            strcat(buffer, "Succesful log in");
            write(client.socket_fd, buffer, strlen(buffer));
            // send       
            break;
    }




    close(client.socket_fd);
    free(client_data);
    return NULL;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr = {0};

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf("Error:socket()\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if(bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error:bind()\n");
        exit(EXIT_FAILURE);
    }

    if(listen(socket_fd, MAX_CLIENTS) < 0){
        printf("Error:listen()\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        Client new_client = (Client)malloc(sizeof(Client));
        if(new_client == NULL) {
            printf("Error:malloc()");
            exit(EXIT_FAILURE);
        }

        int new_socket_fd = accept(socket_fd, (struct sockaddr*)&new_client->client_addr, (socklen_t*)&new_client->client_length);
        if(new_socket_fd < 0) {
            printf("Error:accept()\n");
            exit(EXIT_FAILURE);
        }
        new_client->socket_fd = new_socket_fd;
        //pthread_create
        client_handler(new_client);
    }
    return 0;
}

//gcc -Wall -O2 -lpthread -o server server.c && ./server
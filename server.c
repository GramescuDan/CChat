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
    {"admin", "admin",      NULL},
    {"dragos", "dragos",    NULL},
    {"dan", "dan",          NULL},
    {"ioana", "ioana",      NULL},
    {"august", "august",    NULL},
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// ~ login
// ! mesaj
void *client_handler(void *client_data) {
    Client* client = (Client*)client_data;
    char buffer[BUFFER_SIZE] = {0};
    bzero(buffer, BUFFER_SIZE);
    if(read(client->socket_fd, buffer, 16) < 0) {
        printf("Error:read()\n");
        exit(EXIT_FAILURE);
    }
    
    const char op = buffer[0];
    for(int i = 1; i < strlen(buffer); i++) {
        buffer[i - 1] = buffer[i];
    }
    buffer[strlen(buffer) - 1] = '\0';
    printf("*%s*\n", buffer);

    if (op == '!') {
        printf("Message incoming\n");
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(database[i].client != NULL) {
                pthread_mutex_lock(&mutex);
                if(write(database[i].client->socket_fd, buffer, strlen(buffer))<0) {
                    printf("Error:write()");
                    exit(EXIT_FAILURE);
                }
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    if (op == '~') {
        char *token = strtok(buffer, " ");
        printf("Login attempt\n");
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(!strcmp(database[i].username, token)){
                token = strtok(NULL, " ");
                if(!strcmp(database[i].password, token)){
                    printf("User %s logged in successfully\n", database[i].username);
                    pthread_mutex_lock(&mutex);

                    database[i].client = (Client*)client_data;
                    bzero(buffer, BUFFER_SIZE);
                    strcat(buffer, ">Hello ");
                    strcat(buffer, database[i].username);
                    strcat(buffer, ", welcome to Cchat!");
                    if (write(client->socket_fd, buffer, strlen(buffer)) < 0) {
                        printf("Error:write()");
                        exit(EXIT_FAILURE);
                    }

                    pthread_mutex_unlock(&mutex);

                    break;
                } else {
                    bzero(buffer, BUFFER_SIZE);
                    strcat(buffer, "Failed log in\n");

                    pthread_mutex_lock(&mutex);

                    if (write(client->socket_fd, buffer, strlen(buffer)) < 0) {
                        printf("Error:write()");
                        exit(EXIT_FAILURE);
                    }

                    pthread_mutex_unlock(&mutex);
                    break;
                }
            }
            if (i == MAX_CLIENTS - 1) {
                printf("User not found\n");
                bzero(buffer, BUFFER_SIZE);
                pthread_mutex_lock(&mutex);
                strcat(buffer, "Invalid user provided\n");
                if (write(client->socket_fd, buffer, strlen(buffer)) < 0) {
                    printf("Error:write()");
                    exit(EXIT_FAILURE);
                }
                pthread_mutex_unlock(&mutex);
            }
        }    
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t recv_msg;
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
    printf(">Hello server, welcome to Cchat!\n");

    if(listen(socket_fd, MAX_CLIENTS) < 0){
        printf("Error:listen()\n");
        exit(EXIT_FAILURE);
    }

    pthread_t t[MAX_CLIENTS] = {0};
    int thread_counter = 0;
    while(1) {
        Client* new_client = (Client*)malloc(sizeof(Client));
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

        // if(pthread_create((pthread_t*)&t[thread_counter], NULL, client_handler, (void*)(new_client)) != 0) {
        //     printf("Error:pthread_create()\n");
        //     exit(EXIT_FAILURE);
        // }
        // pthread_join(t[thread_counter], NULL);

        if(pthread_create(&recv_msg, NULL, client_handler, (void*)(new_client)) < 0) {
            printf("Error:thread()");
            exit(EXIT_FAILURE);
        }
        pthread_join(recv_msg, NULL);
    }

    return 0;
}

//gcc -Wall -O2 -o server server.c -lpthread && ./server
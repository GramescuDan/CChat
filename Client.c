#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_PORT 5000
#define BUFFER_SIZE 512

const char SERVER_ADDR[]="127.0.0.1";
char temp[BUFFER_SIZE / 2];
char buffer[BUFFER_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *send_message(void *arg) {
    int socket_fd = *(int*)arg;
    char local_buffer[BUFFER_SIZE] = {0};
    local_buffer[0] = '!';
    while(1) {
        for(int i = 1; i < BUFFER_SIZE - 1; i++) {
            char c = getchar();
            if(c == '\n') break;
            local_buffer[i] = c;
        }

        pthread_mutex_lock(&mutex);
        if(send(socket_fd, local_buffer, strlen(local_buffer), 0) < 0) {
            printf("Error:write()\n");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *recv_message(void* arg) {
    int socket_fd = *(int*)arg;
    char local_buffer[BUFFER_SIZE] = {0};

    while(1) {
        if(read(socket_fd, local_buffer, BUFFER_SIZE) < 0) {
            printf("Error:read()\n");
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf(">>>%s\n", local_buffer);
    }
    return NULL;
}

int main() {
    pthread_t recv_msg;
    pthread_t send_msg;

    struct sockaddr_in server_addr = {0};

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf("Error:socket()\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    // int flag = 1;
    // setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));

    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Error:connect()\n");
        exit(EXIT_FAILURE);
    }

    // get credentials
    strcat(buffer, "~");
    printf("Username: ");
    scanf("%s", temp);

    strcat(buffer, temp);
    strcat(buffer, " ");
    bzero(temp, BUFFER_SIZE / 2);

    printf("Password: ");
    scanf("%s", temp);
    strcat(buffer, temp);

    if(write(socket_fd, buffer, strlen(buffer)) < 0) {
        printf("Error:write()");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&recv_msg, NULL, recv_message, &socket_fd) < 0) {
        printf("Error:thread()");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&send_msg, NULL, send_message, &socket_fd) < 0) {
        printf("Error:thread()");
        exit(EXIT_FAILURE);
    }

    pthread_join(recv_msg, NULL);

    close(socket_fd);
    return 0;
}

///gcc -Wall -O2 -o client Client.c -lpthread && ./client
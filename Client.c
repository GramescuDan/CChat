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
#define BUFFER_SIZE 512

const char SERVER_ADDR[]="127.0.0.1";
char temp[BUFFER_SIZE / 2];
char buffer[BUFFER_SIZE];

void *send_message(void arg) {
    int socket_fd = (int)arg;
    char local_buffer[BUFFER_SIZE] = {0};
    printf("-> ");
    fgets(local_buffer, BUFFER_SIZE - 1, stdin);
    local_buffer[strlen(local_buffer) - 1] = '\0';

    //send to socket_fd

    return NULL;
}



int main() {
    struct sockaddr_in server_addr = {0};

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf("Error:socket()\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    if(connect(socket_fd, (struct sockaddr)&server_addr, sizeof(server_addr)) < 0){
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

    int n_write, n_read;

    n_write = write(socket_fd, buffer, strlen(buffer));
    if(n_write < 0) {
        printf("Error:write()");
        exit(EXIT_FAILURE);
    }

    //enter in infinite loop of reading and writting
    while(1) {
        //pthread_create
        send_message(&socket_fd);
        //accept and read to buffer then display

    }

    close(socket_fd);
    return 0;
}

///gcc -Wall -O2 -lpthread -o client client.c && ./client
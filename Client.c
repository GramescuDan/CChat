#include <stdio.h> 
#include <stdlib.h>
#include <string.h>  
#include <sys/socket.h>  
#include <unistd.h>
#include <pthread.h>
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

#define PORT 5001
#define PORT_SERVER 5000
#define BUFFER_SIZE 512

typedef enum {READ, WRITE} State;

typedef enum {True, False} Boolean;

const char *IP= "127.0.0.1";\
char buffer[BUFFER_SIZE];

void *custom_alloc(int size) {
    void mem = (void)malloc(size);
    if(mem == NULL) {
        fprintf(stderr, "Error memory allocation");
        exit(EXIT_FAILURE);
    }
    return mem;
}

void clear_buffer(char *buffer, int size) {
    for(int i = 0; i < size; i++) {
        buffer[i] = 0;
    }
}

int main(int argc, char* argv[]) {

    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];

    printf("Username: ");
    scanf("%s", username);

    printf("Password: ");
    scanf("%s", password);

    clear_buffer(buffer, BUFFER_SIZE);

    strcat(buffer, "~");
    strcat(buffer, username);
    strcat(buffer, " ");
    strcat(buffer, password);

    struct sockaddr_in client_addr;

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(PORT);

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT_SERVER);

    int socket_sender_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_sender_fd < 0){
        printf("Error:Socket()");
        exit(EXIT_FAILURE);
    }

    int socket_reciever_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_reciever_fd < 0){
        printf("Error:Socket()");
        exit(EXIT_FAILURE);
    }

    if(connect(socket_sender_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        printf("Error: Connect()");
        exit(EXIT_FAILURE);
    }

    printf("\n%s\n", buffer);
    if (send(socket_sender_fd, buffer, strlen(buffer), 0) <0 ) {
        printf("Error:send()");
        exit(EXIT_FAILURE);
    }

    if((listen(socket_reciever_fd, 1)) <0) {
        printf("Error:listen");
        exit(EXIT_FAILURE);
    }

    int response_fd = accept(socket_reciever_fd, (struct sockaddr )&server_addr,(socklen_t)sizeof(server_addr));
    if(response_fd < 0) {
        printf("Error:accept");
        exit(EXIT_FAILURE);
    }

    clear_buffer(buffer, BUFFER_SIZE);
    int message_read = read(response_fd, buffer, sizeof(char));
    if(message_read < 0){
        printf("Error:Read()");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buffer);



    // int state = READ;
    // while(1) {
    //     switch (state){
    //         case READ:
    //             state = WRITE;
    //             break;
    //         case WRITE:
    //             state = READ;
    //             break;
    //     }
    // }
}
// gcc -Wall -o client Client.c -lws2_32
//./client
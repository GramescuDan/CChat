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

#define PORT 4761
const char *IP= "127.0.0.1";

typedef struct Client{
    char *Username;
    char *Password;
}Client;

void *custom_alloc(int size) {
    char mem = (char)malloc(size);
    if(mem == NULL) {
        fprintf(stderr, "Error memory allocation");
        exit(EXIT_FAILURE);
    }
    return (void*)mem;
}

Client *login(int *socket) {
    
    char *username;
    username = (char*)custom_alloc(15*sizeof(char));

    char *password;
    password = (char*)custom_alloc(15*sizeof(char));
    printf("Username:");
    scanf("%s",username);
    
    char *message;
    message = (char*)custom_alloc(50*sizeof(char));

    strcat(message,username);
    strcat(message + strlen(username)-1," ");

    printf("Password:");
    scanf("%s", password);

    strcat(message,password);
    send(*socket, message, strlen(message)+1, 0);

    if(listen(*socket,1)<0){
        printf("Error: listen()");
        exit(EXIT_FAILURE);
    }
    char buffer;
    int message_read = read(*socket, &buffer,sizeof(char));
    if(message_read <0){
        printf("Error:Read()");
        exit(EXIT_FAILURE);
    }
    if(strcmp(buffer,"0")){
        Client *client = (Client*)custom_alloc(sizeof(Client));
        client->Username = (char*)custom_alloc(sizeof(strlen(username)));
        client->Password = (char*)custom_alloc(sizeof(strlen(password)));

        strcpy(client->Username,username);
        strcpy(client->Password,password);

    return client;
    }
    return NULL;
}

int main(int argc, char* argv[]) {

    struct sockaddr_in server;
    int skt_fd;

    skt_fd=socket(AF_INET,SOCK_STREAM,0);
    if(skt_fd<0){
        printf("Error:Socket()");
        exit(EXIT_FAILURE);
    }

    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if(connect(skt_fd,(struct sockaddr *)&server,sizeof(server))<0){
        printf("Error: Connect()");
        exit(EXIT_FAILURE);
    }

    printf("Welcome to CChat!\n---Log in---\n");
    while (1) {
        if (login(&skt_fd) == NULL) {
            printf("Login Failed!\nPlease try again!\n");
            continue;
        } else {
            break;
        }
    }

    printf("--CHAT--");

}
// gcc -Wall -o client Client.c -lws2_32
//./client
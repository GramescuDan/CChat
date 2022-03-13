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

typedef struct User{
    int port;
    char *ip;
    char *Username;
    char *Password;
}User;

int login(User user, int *socket) {

    printf("Username:");
    scanf("%s", user.Username);
    printf("Password:");
    scanf("%s", user.Password);

    int messagelength = strlen(user.Username) + strlen(user.Password);
    char *message;
    message = malloc(messagelength*sizeof(char));

    if(message == NULL){
        printf("Error:message malloc()");
        exit(EXIT_FAILURE);
    }

    strcat(message,user.Username);
    strcat(message+strlen(user.Username)-1," ");
    strcat(message,user.Password);

    return send(*socket, message, messagelength, 0);
}

int main(int argc, char* argv[]) {

    User *user;
    struct sockaddr_in server;
    int skt_fd;

    user = malloc(sizeof(User));
    if (user == NULL) {
        printf("User:malloc()");
        exit(EXIT_FAILURE);
    }

    user->Username = malloc(50 * sizeof(char));
    if (user->Username == NULL) {
        printf("User->Username:malloc()");
        exit(EXIT_FAILURE);
    }
    user->Password = malloc(50 * sizeof(char));
    if (user->Password == NULL) {
        printf("User->Password:malloc()");
        exit(EXIT_FAILURE);
    }
    user->ip = malloc(9 * sizeof(char));
    if (user->ip == NULL) {
        printf("User->Password:malloc()");
        exit(EXIT_FAILURE);
    }
    user->ip = "127.0.0.1";
    user->port = 4761;

    skt_fd=socket(AF_INET,SOCK_STREAM,0);
    if(skt_fd<0){
        printf("Error:Socket()");
        exit(EXIT_FAILURE);
    }

    server.sin_addr.s_addr = inet_addr(user->ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(user->port);

    if(connect(skt_fd,(struct sockaddr *)&server,sizeof(server))<0){
        printf("Error: Connect()");
        exit(EXIT_FAILURE);
    }

    printf("Welcome to CChat!\n---Log in---\n");
    while (1) {
        if (login(*user,&skt_fd) == 1) {
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
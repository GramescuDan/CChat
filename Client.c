#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intrin.h>
#include <winsock.h>

typedef struct User{
    int port;
    char *ip;
    char *Username;
    char *Password;
    bool status;
}User;

int login(User user) {
    printf("Username:");
    scanf("%s", user.Username);
    printf("Password:");
    scanf("%s", user.Password);
    //trebuie facuta legautra cu serverul
    return 0;
}

int main(int argc, char* argv[]) {

    User *user;
    struct sockaddr_in server;
    unsigned int skt=socket(AF_INET,SOCK_STREAM,0);

    if(skt<0){
        printf("Error:Socket()");
        exit(EXIT_FAILURE);
    }

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
    user->port = 8888;

    server.sin_addr.s_addr = inet_addr(user->ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(user->port);


    printf("Welcome to CChat!\n---Log in---\n");
    while (1) {
        if (login(*user) == 1) {
            printf("Login Failed!\n Please try again");
            continue;
        } else {
            user->status = true;
            break;
        }
    }

    printf("--CHAT--");

}

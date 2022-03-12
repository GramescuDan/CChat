
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intrin.h>

typedef struct User{
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

    printf("Welcome to CChat!\n");
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

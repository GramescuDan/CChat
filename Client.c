
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

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

int main(int argc, char* argv[]){
    User *user;
    user = malloc(sizeof(User));
    if(user==NULL){
        printf("User:malloc()");
        exit(EXIT_FAILURE);
    }
    user->Username = malloc(50*sizeof(char));
    if(user->Username==NULL){
        printf("User->Username:malloc()");
        exit(EXIT_FAILURE);
    }
    user->Password = malloc(50*sizeof(char));
    if(user->Password==NULL){
        printf("User->Password:malloc()");
        exit(EXIT_FAILURE);
    }
    int choice;
    user->status = true;
    printf("Hello World!\n Welcome to our messaging App!\n");
    while(1){
        if(login(*user)==1){
            printf("Login Failed!\n Please try again");

        }else{
            break;
        }
    }
   // while(1){
        printf("0.Exit\n1.See Who is Online\n2.");
        switch(choice) {
            case 0 :
                user->status =false;
                exit(EXIT_SUCCESS);
            case 1 :
                break;
        }
        //}

    }




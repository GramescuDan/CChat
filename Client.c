
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

    printf("Hello World!\nWelcome to our messaging App!\n");
    while(1){
        if(login(*user)==1){
            printf("Login Failed!\n Please try again");

        }else{
            user->status = true;
            break;
        }
    }
    int choice = 0;
    char yesno[5];
    char message[256];
    while(1){
        printf("1.See Who is Online\n2.Write Message to Chat\n0.LogOut\nYour Choice:");
        scanf("%d",&choice);
        switch(choice) {
            case 0 :
                printf("\nAre you sure you want to log out?(yes/no)");
                while(1) {
                    scanf("%s", &yesno);
                    if (strcmp(yesno, "yes") == 0) {
                        user->status = false;
                        exit(EXIT_SUCCESS);
                    } else if (strcmp(yesno, "no") == 0) {
                        break;
                    } else {
                        printf("Choice not recognized!\nTry again!\nYour Choice(yes/no):");
                        continue;
                    }
                }
                break;
            case 1:
                //trimitem request catre server ca dorim sa vedem cine e on
                printf("status:%d",user->status);
                break;
            case 2:
                printf("Your Message:");
                scanf("%s",&message);
                //Mesajul scris e trimis catre server.
                break;

            default :
                break;
        }
        }

    }




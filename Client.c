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
#include <signal.h>

#include "getch.c"

#define SERVER_PORT 5000
#define BUFFER_SIZE 512
#define  BACK_SPACE   0x08

// close la socket cand clientul trimite mesajul "exit"
int flag;
const char SERVER_ADDR[]="127.0.0.1";
char temp[BUFFER_SIZE / 2];
char buffer[BUFFER_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void encrypt (char password[], int key){

	
	for(int i = 0; i < strlen(password); i++){
		password[i] = password[i] - key;
	}
}

void maskPass (char password[]){
	char ch;
	char buff1[] = "*";
	char buff2[] = "\b \b";
	int i = 0;
	while((ch = getch())!= '\n'){ //until user press enter
	   if(ch != BACK_SPACE){
		   write(STDOUT_FILENO, buff1, sizeof(buff1) - 1);
			password[i] = ch;
			i++;
			} else {
				if (i > 0) {
					write(STDOUT_FILENO, buff2, sizeof(buff2) - 1);
					password[i] = '\0';
					i--;	
				}
			}
	   }
	password[i] = '\0';
}

void *send_message(void *arg) {

    int socket_fd = *(int*)arg;
    char local_buffer[BUFFER_SIZE] = {0};
    memset(local_buffer,0,sizeof(local_buffer));
    strcat(local_buffer,"!");
    while(1) {

        for(int i = 1; i < BUFFER_SIZE - 1; i++) {
            char c = getchar();
            if(c == '\n') {
                local_buffer[i]='\0';
                break;}
            local_buffer[i] = c;
        }

        //printf("*%s*\n", local_buffer);

        pthread_mutex_lock(&mutex);

        if(send(socket_fd, local_buffer, strlen(local_buffer),0) < 0) {
            printf("Error:send()\n");
            exit(EXIT_FAILURE);
        }
	    if(strcmp(local_buffer, "!exit\0") == 0){
			
			exit(EXIT_FAILURE);
			break;
	    }
		else if(strcmp(local_buffer, "exit") != 0){
        		//printf("S-a trimis mesajul\n");			
		}

        pthread_mutex_unlock(&mutex);

    }
    flag = 1;

    return NULL;
}

void *recv_message(void* arg) {
    int socket_fd = *(int*)arg;
    char local_buffer[BUFFER_SIZE] = {0};
       while(1) {
       
		  int receive = recv(socket_fd, local_buffer, BUFFER_SIZE, 0);  	
          if (receive > 0) {
                printf("%s\n", local_buffer);
          	}
          	else if (receive < 0) {
          		break;
          	}
          	bzero(local_buffer,BUFFER_SIZE); //adaugat stergerea ultimului mesaj
		
	

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


    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Error:connect()\n");
        exit(EXIT_FAILURE);
    }
int tryAgain = 0;
     char string[BUFFER_SIZE];
do {
	
// get credentials
	bzero(temp, BUFFER_SIZE);
	
    strcat(buffer, "~");
    printf("Username: ");
    scanf("%s", temp);

    strcat(buffer, temp);
    strcat(buffer, " ");
    bzero(temp, BUFFER_SIZE / 2);

    printf("Password: ");
	maskPass(temp);
	encrypt(temp,0xAED);
	printf("\n");
 
 
    strcat(buffer, temp);
    bzero(string, BUFFER_SIZE);
    if(write(socket_fd, buffer, strlen(buffer)+1) < 0) {
        printf("Error:send()");
        exit(EXIT_FAILURE);
    }

	if(read(socket_fd, string, BUFFER_SIZE) < 0) {
            		printf("Error:read()\n");
            		perror("read");
            		exit(EXIT_FAILURE);
    }
	printf("%s\n",string);
    if (strcmp(string, "Failed log in\n") == 0 || strcmp(string, "Invalid user provided\n") == 0 || strstr(string, " already signed in") != NULL) {
        tryAgain = 1;
		bzero(buffer,BUFFER_SIZE);
    
    } else {
		tryAgain = 0;
	}
    
}while (tryAgain);

	if(pthread_create(&send_msg, NULL, send_message, &socket_fd) < 0) {
        printf("Error:thread()");
        exit(EXIT_FAILURE);
    }
	
    if(pthread_create(&recv_msg, NULL, recv_message, &socket_fd) < 0) {
        printf("Error:thread()");
        exit(EXIT_FAILURE);
    }


    pthread_join(recv_msg, NULL);
    
        while (1) {
        if (flag) {
            break;
        }
    }
	close(socket_fd);
    return 0;
}



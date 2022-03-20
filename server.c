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
#define MAX_CLIENTS 5
#define BUFFER_SIZE 512

int flag = 0;

//close la socket cand serverul primeste exit de la client

typedef struct Client{
    int socket_fd;
    struct sockaddr client_addr;
    socklen_t client_length;
    char client_name[20];
} Client;

typedef struct {
    char username[BUFFER_SIZE / 8];
    char password[BUFFER_SIZE / 8];
    Client *client; // NULL - unauthorized | !NULL - authorized
} Database;

Database database[MAX_CLIENTS] = {
    {"admin", "admin",      NULL},
    {"dragos", "dragos",    NULL},
    {"dan", "dan",          NULL},
    {"ioana", "ioana",      NULL},
    {"august", "august",    NULL},
};


void decrypt (char password[], int key){

	
	for(int i = 0; i < strlen(password); i++){
		password[i] = password[i] + key;
	}
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// ~ login
// ! mesaj


void *client_handler(void *client_data) {
	
	while(1) { //while pentru mentinere in permanenta a legaturii dintre client si server
    Client* client = (Client*)client_data;
    char buffer[BUFFER_SIZE] = {0};
    bzero(buffer, BUFFER_SIZE);
    if(read(client->socket_fd, buffer, BUFFER_SIZE) < 0) {
        printf("Error:read()\n");
        exit(EXIT_FAILURE);
    }

    const char op = buffer[0];
	
	
    for(int i = 1; i < strlen(buffer); i++) {
        buffer[i - 1] = buffer[i];
    }
    buffer[strlen(buffer) - 1] = '\0';
    if(strcmp(buffer, "exit") == 0){
	printf("%s left the chat\n", client->client_name);
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(strcmp(client->client_name, database[i].username) == 0){
			free(database[i].client);
			database[i].client = NULL;
		}
	}
	break;
    } else {
	flag = 0;
    }
	

	
    if (strcmp(buffer, "") != 0 && strcmp(client->client_name, "") !=0 ){
    	printf("%s says: %s\n", client->client_name, buffer);
    }	 
    if (op == '!') {
		char local[BUFFER_SIZE] ={0};
        //printf("Message incoming\n");
        for(int i = 0; i < MAX_CLIENTS; i++) {
		  bzero(local, BUFFER_SIZE);
            if(database[i].client != NULL) {
                pthread_mutex_lock(&mutex);
				if ((strcmp(database[i].client->client_name, client->client_name) != 0) && (strcmp(buffer, "") != 0) ) {
					
					strcat(local,client->client_name);
					strcat(local, ": ");
					strcat(local, buffer);
					//strcat(local, "\n");
				 if(write(database[i].client->socket_fd, local, strlen(local))<0) {
                    printf("Error:write()");
                    exit(EXIT_FAILURE);
                }
				}
			

                pthread_mutex_unlock(&mutex);
				

            }
        }
    }
    if (op == '~') {
        char *token = strtok(buffer, " ");
        printf("Login attempt\n");
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(!strcmp(database[i].username, token)){
				if(database[i].client != NULL){
					char local[BUFFER_SIZE] = {0};
					strcat(local, database[i].username);
					strcat(local, " already signed in");
					//printf("%s already signed in\n", database[i].username);
						if (write(client->socket_fd, local, strlen(local)) < 0) {
							printf("Error:write()");
							exit(EXIT_FAILURE);
							}
						break;
						
				} 
				token = strtok(NULL, " ");
				decrypt(token, 0xAED);				
                if(!strcmp(database[i].password, token)){
                    printf("User %s logged in successfully\n", database[i].username);
                    pthread_mutex_lock(&mutex);

                    database[i].client = (Client*)client_data;
                    
                    strcpy(database[i].client->client_name, database[i].username);
                    
                    bzero(buffer, BUFFER_SIZE);
                    strcat(buffer, ">Hello ");
                    strcat(buffer, database[i].username);
                    strcat(buffer, ", welcome to Cchat!");
                    if (write(client->socket_fd, buffer, strlen(buffer)) < 0) {
                        printf("Error:write()");
                        exit(EXIT_FAILURE);
                    }

                    pthread_mutex_unlock(&mutex);

                    break;
                } else {
                    bzero(buffer, BUFFER_SIZE);
                    strcat(buffer, "Failed log in\n");
                    pthread_mutex_lock(&mutex);

                    if (write(client->socket_fd, buffer, strlen(buffer)) < 0) {
                        printf("Error:write()");
                        exit(EXIT_FAILURE);
                    }
                    pthread_mutex_unlock(&mutex);
                    break;
                }
            }
            if (i == MAX_CLIENTS - 1) {
                printf("User not found\n");
                bzero(buffer, BUFFER_SIZE);
                pthread_mutex_lock(&mutex);
                strcat(buffer, "Invalid user provided\n");
                if (write(client->socket_fd, buffer, strlen(buffer)) < 0) {
                    printf("Error:write()");
                    exit(EXIT_FAILURE);
                }
                pthread_mutex_unlock(&mutex);
            }
        }    
    }
}
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t recv_msg;
    struct sockaddr_in server_addr = {0};

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf("Error:socket()\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if(bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error:bind()\n");
        exit(EXIT_FAILURE);
    }

    printf(">Hello server, welcome to Cchat!\n");

    if(listen(socket_fd, MAX_CLIENTS) < 0){
        printf("Error:listen()\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        
        Client* new_client = (Client*)malloc(sizeof(Client));
        if(new_client == NULL) {
            printf("Error:malloc()");
            exit(EXIT_FAILURE);
        }

        new_client->client_length = sizeof(new_client->client_addr);

        int new_socket_fd = accept(socket_fd, (struct sockaddr*)&new_client->client_addr, (socklen_t*)&new_client->client_length);
        if(new_socket_fd < 0) {
            printf("Error:accept()\n");
            exit(EXIT_FAILURE);
        }
        
        new_client->socket_fd = new_socket_fd;

        if(pthread_create(&recv_msg, NULL, client_handler, (void*)(new_client)) < 0) {
            printf("Error:thread()");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}


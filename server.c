/* 
    Protocolos  
    creado: 2018
    Conecta a dos nodos por medio de sockets
    */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 8080
#define MAX_USERS 100
void *createUser(void *usr);
int main(int argc, char const *argv[]);

//GLOBAL VARIABLES

// user structure
struct user
{
    /* 
    "id": "ASdbjkxclz+asd?",
	"name": "<nombre>",
	"status": "<status>" 
    */

    int userID;
    char *name;
    char *status;
};

// creates user with his pthread
void *createUser(void *usr)
{
    // getting info from the user recived
    struct user *userInfo;
    userInfo = (struct user *)usr;
    fflush(stdout);
    
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    bool running = true; // if the program is running
    int server_fd, new_socket, valread, rc, counter;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    // char *acceptedMssg = "{\n  	\"status\": \"OK\",\n  	\"user\": {\n		\"id\": \"ASdbjkxclz+asd?\",\n		\"name\": \"<nombre>\",\n		\"status\": \"<status>\"\n	}\n}";
    char *acceptedMssg = "Abxhlfadjsaf$312"; //TODO cambiar
    //Threads
    pthread_t threads[MAX_USERS];
    // Aqui es donde se van a guardar los usuairos, va a ser en la RAM
    struct user users[MAX_USERS];
    fflush(stdout); // printf will not work without this
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof(buffer));
    fflush(stdout);
    memset(buffer, 0, sizeof(buffer));
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("failed");
        exit(EXIT_FAILURE);
    }
    while (running)
    {
        // COMUNICATION NEW USER
        bzero(buffer, sizeof(buffer));
        // reads recived message
        valread = read(new_socket, buffer, sizeof(buffer));
        if (valread > 0)
        {
            // USER INFO
            users[counter].userID = counter;
            users[counter].name = (char *)buffer;
            users[counter].status = "active";
            // users[counter].message = buffer;
            printf("userID: %d\n", users[counter].userID);
            printf("name: %s\n", users[counter].name);
            printf("status: %s\n", users[counter].status);
            // create user with socket
            rc = pthread_create(&threads[counter], NULL, createUser, (void *)&users[counter]);
            bzero(buffer, sizeof(buffer));
            // Sending Confirmation
            write(new_socket, acceptedMssg, strlen(acceptedMssg));
            counter = counter + 1;
            // pthread_join(threads[counter],NULL);
            // close(new_socket);
        }
        else
        {
            printf("ERROR socket reading\n");
            write(new_socket, acceptedMssg, strlen(acceptedMssg));
            bzero(buffer, sizeof(buffer));
        }
    }

    return 0;
}

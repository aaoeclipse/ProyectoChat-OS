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
void *createUser(void *usr);
int main(int argc, char const *argv[]);

// user structure
struct user
{
    int userID;
    char const *message;
};

// creates user with his pthread
void *createUser(void *usr)
{
    // getting info from the user recived
    struct user *userInfo;
    userInfo = (struct user *) usr;
    printf("ID : %d\n", userInfo->userID);
    fflush(stdout);
    printf("Message : %s\n", userInfo->message);
    fflush(stdout);
    // usr->message
    pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
    bool running = true;                                // if the program is running
    int server_fd, new_socket, valread, rc, counter;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    //Threads
    pthread_t threads[100];
    struct user users[100];

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
    while (running)
    {
        fflush(stdout);

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        users[counter].userID = counter;
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        users[counter].message = buffer;
        rc = pthread_create(&threads[counter], NULL, createUser, (void *)&users[counter]);
        send(new_socket, hello, strlen(hello), 0);
        counter = counter + 1;
    }


    printf("Hello message sent\n");
    return 0;
}

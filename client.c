// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 8080
#define NETWORKADDRESS 127.0.0.1

int sendMessage(char* message);

int main(int argc, char const *argv[])
{
    bool wantToExit = false;
    bool connected = true;
    struct sockaddr_in address;
    char *exitcommand = "q";
    int sock = 0, valread, n;
    struct sockaddr_in serv_addr;
    char *hello = (char *)"Hello from client";
    char buffer[1024] = {0};
    // Bienvenida
    fflush(stdout);
    printf("============ ChatRoom 2018 ==============\n");
    printf("Press 'q' to quit\n ");
    fflush(stdout);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    while (wantToExit == false)
    {

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nConnection Failed \n");
        }
        printf("You: ");
        fgets(buffer, 256, stdin);

        while (1)
        {
            n = write(sock, buffer, strlen(buffer));
        }
        if (n < 0)
            printf("error writing to socket\n");

        n = read(sock, buffer, 255);
        if (strcmp(exitcommand,buffer) != 0){
            printf("Exiting Program...");
            wantToExit = true;
            return -1;
        } else {
            printf("it is not q");
        }
        if (n < 0){
            printf("error reading from socket\n");
            wantToExit = true;
            }
        else {
        send(sock, buffer, strlen(buffer), 0);
        valread = read(sock, buffer, 1024);
        }

    }

    printf("%s\n", buffer);

    return 0;
}


// void welcomeMessage(){
//     print
// }
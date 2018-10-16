// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ifaddrs.h>

#define PORT 8080
#define NETWORKADDRESS 127.0.0.1

// methods
/* Manda mensajes de una persona a otra, hay que cambiar los parametros para
    que acepte a un id */
int sendMessage(char *message, int successfulSocket);
/* Este metodo va a sacar el mensaje de bienvenida y se va a enfocar en conseguir los datos del usuario */
void welcomeMessage();
/*  */
int commandFunctions(char *command);
void conn();
// global variables
int ID, lengthOfString, sock = 0, valread;
struct sockaddr_in serv_addr;
bool connected;

int main(int argc, char const *argv[])
{
    bool wantToExit = false;
    struct sockaddr_in address;
    char exitcommand = 'q';
    int successfulSocket, instruction, writeCounter;
    char buffer[1024] = {0}, recvBuffer[1024] = {0};
    // Bienvenida
    welcomeMessage();

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error\n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }
    // CHECK CONNECTION
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed\n");
        fflush(stdout);
        connected = false;
    }
    else
    {
        connected = true;
    }
    while (wantToExit == false)
    {
        if (!connected)
        {
            printf("Trying to reconnect...\n");
            fflush(stdout);
            sleep(3);
            conn();
        }
        // If connection is successsful
        else
        {
            // restart the buffer
            memset(buffer, 0, 1024);
            printf("You: ");
            // ask for input
            fgets(buffer, 256, stdin);
            fflush(stdin);
            // check if its a command
            if (buffer[0] == '\\')
            {
                switch (commandFunctions(buffer))
                {
                case 0:
                    return -1;
                    break;

                case 1:
                    // Private message
                    break;

                default:
                    printf("invalid statement");
                    break;
                }
            }
            // writeCounter = 0;
            // while (writeCounter < 1024)
            // {
            //     successfulSocket = write(sock, buffer, strlen(buffer));
            //     writeCounter = ++writeCounter;
            // }
            // if (successfulSocket < 0)
            // {
            //     printf("error writing to socket\n");
            //     fflush(stdout);
            // }

            successfulSocket = read(sock, buffer, 255);

            if (successfulSocket < 0)
            {
                printf("error reading from socket\n");
                wantToExit = true;
            }
            printf("%s\n", buffer);
        }
    }

    printf("%s\n", buffer);

    return 0;
}
void welcomeMessage()
{
    char *username, *ipAddress;
    struct ifaddrs *id;
    int val;
    val = getifaddrs(&id);

    printf("============ ChatRoom 2018 ==============\n");

    // USERNAME
    printf("Please enter your username: ");
    fflush(stdout);
    fgets(username, 256, stdin);
    fflush(stdin);

    // Gettting origin ip
    ipAddress = (char *)id->ifa_addr;
    printf("Your IP: %s", ipAddress);
    printf("\n");

    printf("Press '\\q' to quit\n");
    fflush(stdout);
}
int commandFunctions(char *command)
{
    lengthOfString = (unsigned)strlen(command);
    // printf("lenghtofstring = %u", lengthOfString );
    // CHECK IF ITS Q
    if (lengthOfString == 3)
    {
        if ('q' == command[1])
        {
            printf("Exiting Program...\n");
            fflush(stdout);
            return 0;
        }
    }
    return -99;
}
int sendMessage(char *message, int successfulSocket)
{
    /* 
    "action": "SEND_MESSAGE",
	"from": "<idusuario>",
	"to": "<idusuario>",
	"message": "Hola"
     */

    int writeCounter = 0;
    writeCounter = 0;
    while (writeCounter < 1024)
    {
        successfulSocket = write(sock, message, strlen(message));
        writeCounter = ++writeCounter;
    }
    if (successfulSocket < 0)
    {
        printf("error writing to socket\n");
        fflush(stdout);
    }
    return successfulSocket;
}

void conn()
{
    // CHECK CONNECTION
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed \n");
        fflush(stdout);
        connected = false;
    }
    else
    {
        connected = true;
    }
}

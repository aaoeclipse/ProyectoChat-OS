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
#include <pthread.h>
#include "cJSON.c"

#define PORT 8080
// #define NETWORKADDRESS 127.0.0.1

// METHODS
/* Manda mensajes de una persona a otra, hay que cambiar los parametros para
    que acepte a un id */
int sendMessage(char *message, int successfulSocket);
/* Este metodo va a sacar el mensaje de bienvenida y se va a enfocar en conseguir los datos del usuario */
void welcomeMessage();
/* cuando alguien escribe \[algo] es considerado un command, esta funcion identifica cual es */
int commandFunctions(char *command);
/* Este connection se utiliza al principio para conectar al server y si no logra, sigue intentando */
void conn();
char *LastcharDel(char *name);
void *listening(void *sock);

// GLOBAL VARIABLES
int ID, lengthOfString, sock = 0, valread;
struct sockaddr_in serv_addr;
bool connected;
char username[256];
char buffer[1024] = {0}, recvBuffer[1024] = {0}, *ipAddress;
pthread_t thread;
int rc;
bool wantToExit;

// user structure
struct user
{
    /* 
    "id": "ASdbjkxclz+asd?",
	"name": "<nombre>",
	"status": "<status>" 
    */
    int userID;
    char name[100];
    char status[100];
};

struct user usuario;

int main(int argc, char const *argv[])
{
    wantToExit = false;
    struct sockaddr_in address;
    char exitcommand = 'q';
    int successfulSocket, instruction, writeCounter, sendSuccessful;

    // Checks if arguments are correct
    if (argc != 2)
    {
        fprintf(stderr, "[-] Incorrect format!\n");
        fprintf(stderr, "$ ./[client] [ip of host]\n");
        return EXIT_FAILURE;
    }
    // gets IP address of HOST
    ipAddress = (char *)argv[1];

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
    if (inet_pton(AF_INET, ipAddress, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    // CONNECTION
    conn(sock);
    rc = pthread_create(&thread, NULL, listening, (void *)&sock);

    while (!wantToExit)
    {
        if (!connected)
        {
            printf("Trying to reconnect...\n");
            fflush(stdout);
            sleep(3);
            conn(sock);
        }

        // If connection is successsful
        else
        {
            // restart the buffer
            memset(buffer, 0, sizeof(buffer));
            printf("%s: ", username);
            // ask for input
            fgets(buffer, sizeof(buffer), stdin);
            // check if its a command
            if (buffer[0] == '\\')
            {
                switch (commandFunctions(buffer))
                {
                case 0:
                    sendSuccessful = sendMessage("BYE", sock);
                    exit(EXIT_SUCCESS);
                    break;

                case 1:
                    // Private message
                    break;
                case 2:
                    // Broadcast message
                    break;
                case 3:
                    //c 1 o //c 2 o //c 3
                    //Change status
                    int status;
                    char *num;
                    int len;                    
                    int dec;
                    num = buffer[(strlen(buffer)-1)];
                    len = strlen(num);
                    for(int i=0; i<len; i++){
		                dec = dec * 10 + ( num[i] - '0' );
	                }
                    printf("prueba para status %d",dec);
                    char *a= ChangeStatus(dec,sock);
                    printf("%s",a);
                    break;
                default:
                    printf("======== Help:\n========  \\q - quit\n========  \\m [user] - change user target\n========  \\b [mssg] - broadcast message\n========  \\l - List\n========  \\c [1-3]- change status: 1 active, 2 busy, 3 inactive");
                    break;
                }
            }
            else
            {
                sendSuccessful = sendMessage(buffer, sock);
                if (sendSuccessful < 0)
                    printf("ERROR writing to socket\n");
                memset(recvBuffer, 0, sizeof(recvBuffer));
            }
        }
    }

    printf("%s\n", buffer);

    return 0;
}
void welcomeMessage()
{
    printf("============ ChatRoom 2018 ==============\n");
    // USERNAME
    printf("Please enter your username: ");
    fflush(stdout);
    fgets(username, 256, stdin);
    LastcharDel(username);
    fflush(stdin);
    printf("Press '\\h' for help or \\q to quit\n");
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
        if ('m' == command[1])
        {
            printf("Changing User Target\n");
            fflush(stdout);
            return 1;
        }
        if ('b' == command[1])
        {
            printf("Boradcasting Message\n");
            fflush(stdout);
            return 2;
        }
        if ('c' == command[1])
        {
            printf("Change Status\n");
            fflush(stdout);
            return 3;
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
	"message": "message"
     */
    successfulSocket = write(sock, message, strlen(message));

    if (successfulSocket < 0)
    {
        printf("error writing to socket\n");
        fflush(stdout);
    }
    return successfulSocket;
}

char * ChangeStatus(int status, int successfulSocket){
/* 
    {
	"action": "CHANGE_STATUS",
	"user": "<id_del_usuario>",
	"status": "<status>"
    }


    cJSON *RequestCambioEstado= cJSON_CreateObject();
    cJSON_AddStringToObject(RequestCambioEstado,"action","CHANGE_STATUS");
    cJSON_AddStringToObject(RequestCambioEstado,"user","<id_del_usuario>");
    cJSON_AddStringToObject(RequestCambioEstado,"status","<status>");
    out = cJSON_Print(RequestCambioEstado);
    printf("%s\n\n", out);
    cJSON_Delete(RequestCambioEstado);
    free(out);


     */

    if (status == 1){

        cJSON *RequestCambioEstado= cJSON_CreateObject();
        cJSON_AddStringToObject(RequestCambioEstado,"action","CHANGE_STATUS");
        cJSON_AddStringToObject(RequestCambioEstado,"user",usuario.userID);
        cJSON_AddStringToObject(RequestCambioEstado,"status","active");
        char *out = cJSON_Print(RequestCambioEstado);
        //printf("%s\n\n", out);
        successfulSocket = write(sock, out, strlen(out));
        cJSON_Delete(RequestCambioEstado);
        free(out);
        return "Estado cambiado a active";
    }
    if (status == 2){

        cJSON *RequestCambioEstado= cJSON_CreateObject();
        cJSON_AddStringToObject(RequestCambioEstado,"action","CHANGE_STATUS");
        cJSON_AddStringToObject(RequestCambioEstado,"user",usuario.userID);
        cJSON_AddStringToObject(RequestCambioEstado,"status","busy");
        char *out = cJSON_Print(RequestCambioEstado);
        //printf("%s\n\n", out);
        successfulSocket = write(sock, out, strlen(out));
        cJSON_Delete(RequestCambioEstado);
        free(out);

        return "Estado cambiado a busy";
    }
    if (status == 3){

        cJSON *RequestCambioEstado= cJSON_CreateObject();
        cJSON_AddStringToObject(RequestCambioEstado,"action","CHANGE_STATUS");
        cJSON_AddStringToObject(RequestCambioEstado,"user",usuario.userID);
        cJSON_AddStringToObject(RequestCambioEstado,"status","inactive");
        char *out = cJSON_Print(RequestCambioEstado);
        //printf("%s\n\n", out);
        successfulSocket = write(sock, out, strlen(out));
        cJSON_Delete(RequestCambioEstado);
        free(out);

        return "Estado cambiado a inactive";
    }else{



        return "No se pudo realizar el cambio";
    }

}

void conn(int successfulSocket)
{
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
        sendMessage(username, successfulSocket);
        successfulSocket = read(sock, recvBuffer, sizeof(recvBuffer));
        printf("%s\n", recvBuffer);


        //Aqui tengo que guardar la informaicon del usuario
        //es en donde tengo que parsear y guardar en usuario

        //buffer es lo que contiene el string donde viene lo que es la estructura del json
        //se crea un objeto json, donde lo parseado se asigna
        cJSON *json = cJSON_Parse(recvBuffer);
        cJSON *pruebaid = cJSON_GetObjectItem(json, "user");
        cJSON *pruebaidreal = cJSON_GetObjectItem(pruebaid,"id");
        cJSON *pruebanamereal = cJSON_GetObjectItem(pruebaid,"name");
        cJSON *pruebastatusreal = cJSON_GetObjectItem(pruebaid,"status");
        char *idusuario = cJSON_Print(pruebaidreal);
        char *nameusuario = cJSON_Print(pruebaidreal);
        char *statususuario = cJSON_Print(pruebaidreal);
        strcpy (usuario.userID,idusuario);
        strcpy (usuario.name,nameusuario);
        strcpy (usuario.status,statususuario);
        //printf("%s\n", outtt);
        printf("este es el primer id o\n\n");

    }
}

char *LastcharDel(char *name)
{
    int i = 0;
    while (name[i] != '\0')
    {
        i++;
    }
    name[i - 1] = '\0';
    return name;
}

void *listening(void *sock)
{
    int newSock = *((int *)sock);
    char bufferUser[1024];
    while (1)
    {
        // COMUNICATION USER
        memset(bufferUser, 0, sizeof(bufferUser));
        // bzero(bufferUser, sizeof(bufferUser));
        // reads recived message
        valread = read(newSock, bufferUser, sizeof(bufferUser));

        if (valread > 0)
        {
            // TODO:
            //  * parsear de texto a JSON
            //  * sacar quien lo mando o si es broadcast
            //  * sacar quien lo mando

            printf("\nUser: %s\n%s: ", bufferUser, username);

            memset(bufferUser, 0, sizeof(bufferUser));
            // bzero(bufferUser, sizeof(bufferUser));
            fflush(stdout);
        }
        else
        {
            printf("ERROR Server Disconnected withouth HandShake\n");
            memset(bufferUser, 0, sizeof(bufferUser));
            // bzero(bufferUser, sizeof(bufferUser));
            wantToExit = true;
            break;
        }
    }
    pthread_exit(NULL);
}

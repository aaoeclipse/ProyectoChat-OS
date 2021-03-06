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
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>

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
char *ChangeStatus(char status[1024], int successfulSocket);
int sendPrivateMessage(char *message, int successfulSocket);
char *noQuotes(char *withQuotes);
char *ListUsersMultiple(int successfulSocket);
char *ListUsersUnico(char *id, int successfulSocket);

// GLOBAL VARIABLES
char *HOST;
char *ORIGIN;
int ID, lengthOfString, sock = 0, valread;
char idParaMandar[25];
char *withoutQuotes;
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
    char userID[25];
    char name[100];
    char status[100];
};

struct user usuario;

int main(int argc, char const *argv[])
{
    int status;
    char *num;
    int len;
    int dec;
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
    HOST = ipAddress;

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
            printf("\n%s: ", username);
            // ask for input
            fgets(buffer, sizeof(buffer), stdin);
            fflush(stdin);
            // check if its a command
            if (buffer[0] == '\\')
            {
                switch (commandFunctions(buffer))
                {
                case 0:
                    // sendSuccessful = sendMessage("BYE", sock);
                    write(sock, "BYE", strlen("BYE"));
                    exit(EXIT_SUCCESS);
                    break;

                case 1:
                    // Private message
                    printf("");
                    char prueba[1024];

                    //char *prueba;
                    //strcpy(prueba,buffer);
                    //printf("prueba %c",prueba);
                    //prueba++;
                    //prueba++;
                    //prueba++;

                    //printf("Llegue al solicitar un usuario\n");
                    //printf("prueba %s",buffer);
                    // printf("Paso la asignacion %s",pruebalist);
                    // strcpy(pruebalist,buffer);
                    strncpy(prueba, buffer + 3, sizeof(buffer) - 3);
                    //printf("prueba %s",prueba);
                    LastcharDel(prueba);
                    //printf("Se va a mandar al id ...%s...\n",prueba);
                    strcpy(idParaMandar, prueba);
                    //printf("EL ID PARA MANDAR ES ...%s...\n",idParaMandar);
                    fflush(stdout);

                    break;
                case 2:
                    // List users
                    printf("List users");
                    char *listusers;
                    listusers = ListUsersMultiple(sock);
                    break;
                case 3:
                    //c 1 o //c 2 o //c 3

                    //pruebast++;
                    //pruebast++;
                    //pruebast++;

                    //                    LastcharDel(pruebast);
                    //printf("lo que trae es %s",pruebast);

                    //len = strlen(buffer[3]);

                    //for (int i = 0; i < len; i++)
                    //{
                    //dec = dec * 10 + (buffer[i] - '0');
                    //}
                    //printf("prueba para status %d", pruebaparaestatus[0]);
                    //char *a = ChangeStatus(buffer[3], sock);
                    //printf("%s", a);
                    //printf("Llegue al solicitar un usuario\n");
                    //printf("prueba %s",buffer);
                    printf("Cambio de status");
                    char pruebastatus[1024];
                    // printf("Paso la asignacion %s",pruebalist);
                    // strcpy(pruebalist,buffer);
                    strncpy(pruebastatus, buffer + 3, sizeof(buffer) - 3);
                    LastcharDel(pruebastatus);
                    //printf("prueba ...%s...",pruebastatus);
                    fflush(stdout);
                    char *statuschange = ChangeStatus(pruebastatus, sock);
                    break;
                case 4:
                    printf("Busqueda de usuario en especifico\n");
                    //printf("prueba %s",buffer);
                    char pruebalist[1024];
                    // printf("Paso la asignacion %s",pruebalist);
                    // strcpy(pruebalist,buffer);
                    strncpy(pruebalist, buffer + 3, sizeof(buffer) - 3);
                    LastcharDel(pruebalist);
                    ///printf("prueba ...%s...",pruebalist);
                    fflush(stdout);
                    char *unsolousuario = ListUsersUnico(pruebalist, sock);
                    break;

                default:
                    printf("======== Help:\n========  \\q - quit\n========  \\m [user] - change user target\n========  \\b [mssg] - broadcast message\n========  \\l - List\n========  \\c [1-3]- change status: 1 active, 2 busy, 3 inactive \\u [id] - search specific user");
                    break;
                }
            }
            else
            {
                sendSuccessful = sendPrivateMessage(buffer, sock);
                if (sendSuccessful < 0)
                    printf("ERROR writing to socket\n");
                memset(recvBuffer, 0, sizeof(recvBuffer));
            }
        }
    }

    // printf("%s\n", buffer);

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
    if ('l' == command[1])
    {
        printf("List users\n");
        fflush(stdout);
        return 2;
    }
    if ('c' == command[1])
    {
        printf("Change Status\n");
        fflush(stdout);
        return 3;
    }
    if ('u' == command[1])
    {
        printf("List One user\n");
        fflush(stdout);
        return 4;
    }
    return -99;
}

void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostbyname");
    }
}
// Returns host information corresponding to host name
void checkHostEntry(struct hostent *hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}
int sendMessage(char *message, int successfulSocket)
{
    /* 
    "host": "192.168.1.2",
	"origin": "192.168.1.3",
	"user": "<nombre>"
     */
    // printf("message: %s\n", message);

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);

    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr *)
                               host_entry->h_addr_list[0]));
    ORIGIN = IPbuffer;
    //printf("Hostname: %s\n", hostbuffer);
    //printf("Host IP: %s", IPbuffer);
    // HOST = "1.1.1.1";
    char *out;
    //cJSON *usr = NULL;
    cJSON *usr = cJSON_CreateObject();
    cJSON_AddStringToObject(usr, "host", HOST);
    cJSON_AddStringToObject(usr, "origin", ORIGIN);
    cJSON_AddStringToObject(usr, "user", message);
    out = cJSON_Print(usr);
    successfulSocket = write(sock, out, strlen(out));
    cJSON_Delete(usr);
    free(out);

    if (successfulSocket < 0)
    {
        printf("error writing to socket\n");
        fflush(stdout);
    }
}

int sendPrivateMessage(char *message, int successfulSocket)
{
    /* 
    "action": "SEND_MESSAGE",
	"from": "<idusuario>",
	"to": "<idusuario>",
	"message": "message"

    //Estructura de JSON de mensaje
    printf("Estructura del json de mensaje\n");
    cJSON *Mensaje= cJSON_CreateObject();
    cJSON_AddStringToObject(Mensaje,"action","SEND_MESSAGE");
    cJSON_AddStringToObject(Mensaje,"from","<idusuario>");
    cJSON_AddStringToObject(Mensaje,"to","<idusuario>");
    cJSON_AddStringToObject(Mensaje,"message","Hola");
    out = cJSON_Print(Mensaje);
    printf("%s\n\n", out);
    cJSON_Delete(Mensaje);
    free(out);

     */

    //Estructura de JSON de mensaje
    //printf("Estructura del json de mensaje\n");
    char *paramensaje;
    cJSON *Mensaje = cJSON_CreateObject();
    LastcharDel(message);
    //LastcharDel(usuario.userID);/////////////////
    //idParaMandar = LastcharDel(idParaMandar);
    ////////////////////////strcpy (idParaMandar, LastcharDel( idParaMandar));
    noQuotes(usuario.userID);
    //noQuotes(idParaMandar);
    //prueba

    //usuario.userID[i-1]='\0';
    //prueba
    //printf("id from: %s...\n",usuario.userID);
    //strcpy (usuario.userID, noQuotes( usuario.userID));
    //strcpy (usuario.userID, noQuotes( usuario.userID));
    //strcpy (idParaMandar, LastcharDel( idParaMandar));
    //strcpy (idParaMandar, LastcharDel( idParaMandar));
    cJSON_AddStringToObject(Mensaje, "action", "SEND_MESSAGE");
    cJSON_AddStringToObject(Mensaje, "from", usuario.userID);
    cJSON_AddStringToObject(Mensaje, "to", idParaMandar);
    cJSON_AddStringToObject(Mensaje, "message", message);
    paramensaje = cJSON_Print(Mensaje);
    //printf("%s\n\n", paramensaje);
    cJSON_Delete(Mensaje);
    //printf("id para mandar: %s\n",idParaMandar);
    //printf("UserID: %s\n",usuario.userID);
    //printf("mensaje: %s\n",message);

    //printf("MENSAJE: %s\n", paramensaje);
    successfulSocket = write(sock, paramensaje, strlen(paramensaje));
    free(paramensaje);

    if (successfulSocket < 0)
    {
        printf("error writing to socket\n");
        fflush(stdout);
    }
    return successfulSocket;
}

char *ListUsersUnico(char *id, int successfulSocket)
{

    //Estructura del Json para listar usuarios (server response) un usuario
    //printf("Estructura del json de listar usuarios (Request) un usuario\n");
    cJSON *ListUsersRequestUnico = cJSON_CreateObject();
    cJSON_AddStringToObject(ListUsersRequestUnico, "action", "LIST_USER");
    cJSON_AddStringToObject(ListUsersRequestUnico, "user", id);
    char *out;
    out = cJSON_Print(ListUsersRequestUnico);
    successfulSocket = write(sock, out, strlen(out));
    //    printf("%s\n\n", out);
    cJSON_Delete(ListUsersRequestUnico);
    free(out);
}

char *ListUsersMultiple(int successfulSocket)
{

    /*
    //Estructura del Json para listar usuarios
    printf("Estructura del json de listar usuarios\n");
    cJSON *ListUsers= cJSON_CreateObject();
    cJSON_AddStringToObject(ListUsers,"action","LIST_USER");
    out = cJSON_Print(ListUsers);
    printf("%s\n\n", out);
    cJSON_Delete(ListUsers);
    free(out);
*/
    char *out;
    //Estructura del Json para listar usuarios
    //printf("Estructura del json de listar usuarios\n");
    cJSON *ListUsers = cJSON_CreateObject();
    cJSON_AddStringToObject(ListUsers, "action", "LIST_USER");
    out = cJSON_Print(ListUsers);
    successfulSocket = write(sock, out, strlen(out));
    //printf("%s\n\n", out);
    cJSON_Delete(ListUsers);
    free(out);
}

char *ChangeStatus(char status[1024], int successfulSocket)
{
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

    if (strcmp(status, "1"))
    {

        cJSON *RequestCambioEstado = cJSON_CreateObject();
        cJSON_AddStringToObject(RequestCambioEstado, "action", "CHANGE_STATUS");
        cJSON_AddStringToObject(RequestCambioEstado, "user", usuario.userID);
        cJSON_AddStringToObject(RequestCambioEstado, "status", "active");
        char *out = cJSON_Print(RequestCambioEstado);
        //printf("%s\n\n", out);
        successfulSocket = write(sock, out, strlen(out));
        cJSON_Delete(RequestCambioEstado);
        free(out);
        return "Estado cambiado a active";
    }
    if (strcmp(status, "2"))
    {

        cJSON *RequestCambioEstado = cJSON_CreateObject();
        cJSON_AddStringToObject(RequestCambioEstado, "action", "CHANGE_STATUS");
        cJSON_AddStringToObject(RequestCambioEstado, "user", usuario.userID);
        cJSON_AddStringToObject(RequestCambioEstado, "status", "busy");
        char *out = cJSON_Print(RequestCambioEstado);
        //printf("%s\n\n", out);
        successfulSocket = write(sock, out, strlen(out));
        cJSON_Delete(RequestCambioEstado);
        free(out);

        return "Estado cambiado a busy";
    }
    if (strcmp(status, "3"))
    {

        cJSON *RequestCambioEstado = cJSON_CreateObject();
        cJSON_AddStringToObject(RequestCambioEstado, "action", "CHANGE_STATUS");
        cJSON_AddStringToObject(RequestCambioEstado, "user", usuario.userID);
        cJSON_AddStringToObject(RequestCambioEstado, "status", "inactive");
        char *out = cJSON_Print(RequestCambioEstado);
        //printf("%s\n\n", out);
        successfulSocket = write(sock, out, strlen(out));
        cJSON_Delete(RequestCambioEstado);
        free(out);

        return "Estado cambiado a inactive";
    }
    else
    {

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
        //printf("%s\n", recvBuffer);
        // printf("buffer size: %ld\n", sizeof(recvBuffer));
        // fflush(stdout);

        //Aqui tengo que guardar la informaicon del usuario
        //es en donde tengo que parsear y guardar en usuario

        //buffer es lo que contiene el string donde viene lo que es la estructura del json
        //se crea un objeto json, donde lo parseado se asigna
        cJSON *json = cJSON_Parse(recvBuffer);

        cJSON *pruebaid = cJSON_GetObjectItem(json, "user");
        cJSON *pruebaidreal = cJSON_GetObjectItem(pruebaid, "id");
        cJSON *pruebanamereal = cJSON_GetObjectItem(pruebaid, "name");
        cJSON *pruebastatusreal = cJSON_GetObjectItem(pruebaid, "status");
        char *idusuario = cJSON_Print(pruebaidreal);
        char *nameusuario = cJSON_Print(pruebanamereal);
        char *statususuario = cJSON_Print(pruebastatusreal);
        //printf("%s\n%s\n%s\n",idusuario,nameusuario,statususuario);
        strcpy(usuario.userID, idusuario);
        strcpy(usuario.name, nameusuario);
        strcpy(usuario.status, statususuario);
        // noQuotes(usuario.userID);
        //printf("ESTE ES EL ID MIO: %s",usuario.userID);
        strcpy(idParaMandar, usuario.userID);

        fflush(stdout);
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
    bool listBool = false;
    while (1)
    {
        // COMUNICATION USER
        memset(bufferUser, 0, sizeof(bufferUser));
        // reads recived message
        valread = read(newSock, bufferUser, sizeof(bufferUser));
        if (valread > 0)
        {
            // printf("\n");
            cJSON *json = cJSON_Parse(bufferUser);
            cJSON *pruebaprint = cJSON_GetArrayItem(json, 0);
            char *pruebasprint = cJSON_Print(pruebaprint);
            noQuotes(pruebasprint);
            int tammao = cJSON_GetArraySize(json);
            int validateok;
            validateok = strncmp(pruebasprint, "OK", 2);
            if (validateok == 0)
            {
                listBool = true;
                if (tammao == 1)
                {

                    //PARSEAR PARA DEVOLVER SOLO EL OK
                    cJSON *MENSAJESOLOOK = cJSON_GetObjectItem(json, "status");
                    char *mensajeunicok = cJSON_Print(MENSAJESOLOOK);
                    noQuotes(mensajeunicok);
                    // printf("Server said: %s",mensajeunicok);
                }
                else
                {
                    //PARSEAR PARA DEVOLVER EL ID
                    cJSON *MENSAJESOLOOK = cJSON_GetObjectItem(json, "status");
                    char *mensajeunicok = cJSON_Print(MENSAJESOLOOK);
                    noQuotes(mensajeunicok);
                    printf("Server said: %s\n", mensajeunicok);
                    cJSON *userfromok = cJSON_GetObjectItem(json, "user");
                    cJSON *iduserfromok = cJSON_GetObjectItem(userfromok, "id");
                    char *currentuserid = cJSON_Print(iduserfromok);
                    noQuotes(currentuserid);
                    printf("Your ID is: %s\n", currentuserid);
                }
            }
            int validateError;
            validateError = strcmp(pruebasprint, "ERROR");
            if (validateError == 0)
            {
                listBool = true;
                cJSON *MensajeError = cJSON_GetObjectItem(json, "message");
                char *Textoerror = cJSON_Print(MensajeError);
                noQuotes(Textoerror);
                printf("Server detected error with message: %s\n", Textoerror);
            }
            int ValidateMessageRecive;
            ValidateMessageRecive = strcmp(pruebasprint, "RECEIVE_MESSAGE");
            if (ValidateMessageRecive == 0)
            {
                listBool = true;

                cJSON *MensajeRecibido = cJSON_GetObjectItem(json, "message");
                cJSON *From = cJSON_GetObjectItem(json, "from");
                char *TextoMensaje = cJSON_Print(MensajeRecibido);
                char *textoFrom = cJSON_Print(From);
                noQuotes(textoFrom);
                noQuotes(TextoMensaje);
                printf("\n%s: ", textoFrom);
                printf("%s\n", TextoMensaje);
            }
            int ValidateUserConnected;
            ValidateUserConnected = strcmp(pruebasprint, "USER_CONNECTED");
            if (ValidateUserConnected == 0)
            {
                listBool = true;

                cJSON *Userconectado = cJSON_GetObjectItem(json, "user");
                cJSON *iddelusuario = cJSON_GetObjectItem(Userconectado, "id");
                cJSON *nombredelusuairoconectado = cJSON_GetObjectItem(Userconectado, "name");
                char *textoidusuario = cJSON_Print(iddelusuario);
                char *textonombreusuairo = cJSON_Print(nombredelusuairoconectado);
                noQuotes(textoidusuario);
                noQuotes(textonombreusuairo);
                noQuotes(textonombreusuairo);
                noQuotes(textonombreusuairo);
                printf("\n");
                printf("El usuario con id %s", textoidusuario);
                printf(" y nombre %s se conecto", textonombreusuairo);
                printf("\n");
            }
            int ValidateUserDisConnected;
            ValidateUserDisConnected = strcmp(pruebasprint, "USER_DISCONNECTED");
            if (ValidateUserDisConnected == 0)
            {
                listBool = true;

                cJSON *Userdesconectado = cJSON_GetObjectItem(json, "user");
                cJSON *iddelusuario = cJSON_GetObjectItem(Userdesconectado, "id");
                cJSON *nombredelusuairoconectado = cJSON_GetObjectItem(Userdesconectado, "name");
                char *textoidusuario = cJSON_Print(iddelusuario);
                char *textonombreusuairo = cJSON_Print(nombredelusuairoconectado);
                noQuotes(textoidusuario);
                noQuotes(textonombreusuairo);
                noQuotes(textonombreusuairo);
                noQuotes(textonombreusuairo);
                printf("\n");
                printf("El usuario con id %s", textoidusuario);
                printf(" y nombre %s se desconecto", textonombreusuairo);
                printf("\n");
            }
            if (listBool == false)
            {
                printf("mensaje: %s", bufferUser);
            }

            memset(bufferUser, 0, sizeof(bufferUser));
            fflush(stdout);
        }
        else
        {
            printf("ERROR Server Disconnected withouth HandShake\n");
            memset(bufferUser, 0, sizeof(bufferUser));
            wantToExit = true;
            break;
        }
        // printf("\n");
        printf("%s: ", username);


    }
    pthread_exit(NULL);
}

char *noQuotes(char *withQuotes)
{
    // char str[25]="hello";
    int i, len = strlen(withQuotes);
    for (i = 1; i < len - 1; i++)
    {
        withQuotes[i - 1] = withQuotes[i];
    }
    withQuotes[i - 1] = '\0';
    // printf(withQuotes);
    return withoutQuotes;
}
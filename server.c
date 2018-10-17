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
#include "cJSON.c"

#define PORT 8080
#define MAX_USERS 100

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
    // Extra
    char address[100];
    int fileDescriptor;
};
//GLOBAL VARIABLES
//Threads
pthread_t threads[MAX_USERS];
char globalString[1024];
// Aqui es donde se van a guardar los usuairos, va a ser en la RAM
struct user users[MAX_USERS];
struct user availableUsers[MAX_USERS];
// METHODS
void *createUser(void *usr);
int main(int argc, char const *argv[]);
void sendMessage(char message[1024], int userSocket, char *fromUser, int toUser);
// @params struct user
// @return el usern en forma de JSON
char *JSONfromUser(struct user givenUser);
// void sendMessage(char *message, int successfulSocket, char *fromUser, int toUser);
int broadcastMessage(char *message);
// muestra toda la lista de
char *listUsers();
char *LastcharDel(char *message);
void initializeUsers();
char *withSemi(char *stringWithoutSemi);
void GetAvailableUsers();
char *respuestaDeJSON(struct user createUser);
void mensajeJSON(struct user *userSender, struct user userResiver, char *message);
void firstJSONRecieve(char *informacionInutil, struct user newUser);

// creates user with his pthread
void *createUser(void *usr)
{
    // getting info from the user recived
    struct user *userInfo;
    char bufferUser[1024] = {0};
    char *accepted = "accpeted";
    int valread, i;
    userInfo = (struct user *)usr;
    fflush(stdout);

    while (1)
    {
        // COMUNICATION USER
        memset(bufferUser, 0, sizeof(bufferUser));
        // reads recived message
        valread = read(userInfo->fileDescriptor, bufferUser, sizeof(bufferUser));
        if (valread > 0)
        {
            i = strncmp("BYE", bufferUser, 3);
            if (i == 0)
            {
                printf("%d:%s BYE\n", userInfo->userID, userInfo->name);
                fflush(stdout);
                break;
            }
            // TODO:
            //    1. String bufferUser -> JSON
            //    2. JSON -> que es? LIST_USER, BYE
            //       2.1 HACER ESAS 5 FUNCIONES

            /* // Sending Confirmation
            sendMessage(bufferUser, userInfo->fileDescriptor, userInfo->name, 1); */
            recivedJSON(bufferUser, userInfo);

            // TODO: PARSEAR
            // mensajeJSON( userInfo, users[0], bufferUser)

            // write(userInfo->fileDescriptor, accepted, strlen(accepted));
            sleep(0.1);
        }
        else
        {
            printf("ERROR user %d:%s disconnected without handshake\n", userInfo->userID, userInfo->name);
            bzero(bufferUser, sizeof(bufferUser));
            // printf("%d BYE\n", userInfo->userID);
            fflush(stdout);
            break;
        }
    }
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
    char *acceptedMssg = "fixthis"; //FIXME: hacer un menajse con el id
    fflush(stdout);                 // printf will not work without this
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
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    counter = 0;
    while (running)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("failed");
            exit(EXIT_FAILURE);
        }
        // COMUNICATION NEW USER
        memset(buffer, 0, sizeof(buffer));

        // reads recived message
        valread = read(new_socket, buffer, sizeof(buffer));
        if (valread >= 0)
        {
            // USER INFO
            users[counter].userID = counter;
            strcpy(users[counter].name, buffer);
            strcpy(users[counter].status, "active");
            users[counter].fileDescriptor = new_socket;

            // create user with socket
            rc = pthread_create(&threads[counter], NULL, createUser, (void *)&users[counter]);

            if (rc < 0)
                printf("ERROR in pthread");
            bzero(buffer, sizeof(buffer));
            // JSONfromUser(users[counter]);
            respuestaDeJSON(users[counter]);
            counter = counter + 1;
        }
        else
        {
            printf("ERROR socket reading\n");
            bzero(buffer, sizeof(buffer));
        }
    }

    return 0;
}
void sendMessage(char message[1024], int userSocket, char *fromUser, int toUser)
{
    /* 
    "action": "SEND_MESSAGE",
	"from": "<idusuario>",
	"to": "<idusuario>",
	"message": "message"
     */
    // TODO:
    //      1. Mandar el mensaje a esa persona
    //      2. No se sabe como el usuario va a estar escuchando

    message = LastcharDel(message);
    // scanf("test: %s", users[0].name);
    printf("user: %s\n", users[0].name);
    fflush(stdout);
    printf("send message: %s\n", message);
    fflush(stdout);
    int resultSocket;
    resultSocket = write(users[toUser].fileDescriptor, message, strlen(message));
    // sendBuffer = message;
    if (resultSocket < 0)
    {
        printf("error writing to socket\n");
        fflush(stdout);
    }
    printf("end of send mssg\n");
    fflush(stdout);
    // return resultSocket;
}

char *listUsers() {
    FILE *f = fopen("file.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        //break;
    }
else{
    const char *intro [200];
    *intro = "{\n\"action\": \"LIST_USER\",\n\t\"users\": [";
    fputs(f, *intro);

    for (int i = 0; i < MAX_USERS; i++) {
        if (availableUsers[i].userID < 0) {
            break;
        }
        /*Sirve para imprimir y guardar en el file*/
        fputs(f, createUserJson(availableUsers[i].userID, availableUsers[i].name, availableUsers[i].status));
        fputs(f,",");
    }
    /* TODO:
        1. sacar todos los users
        2. de users -> JSON
        3. Mandar a la persona que pidio
    */
    
    FILE *fp = fopen("file.txt","r");
    char *c = fgetc(fp);
    printf("%c", c);
    fclose(fp);
}
}

char createUserJson(char id, char name, char status){
    //char userText [1000];
    char idtext []= "{\"id\": ";
    char nametext = "\n\"name\": ";
    char statustext = "\n\"status\": ";
    char endtext = "}";
    strcat(idtext, id);
    strcat(nametext, name);
    strcat(statustext, status);
    strcat(idtext, nametext);
    strcat(idtext, statustext);
    strcat(idtext, endtext);
    return idtext;
}
char* withSemi(char *stringWithoutSemi){
    memset(globalString, 0, sizeof(globalString));
    char semi[50] = "\"";
    fflush(stdout);
    strcat(globalString, semi);
    strcat(globalString, stringWithoutSemi);
    strcat(globalString, "\"");
    return (char *)globalString;
}
char *respuestaDeJSON(struct user currUser)
{
    char *out;
    char id[10];
    char name[100];
    char status[100];
    // id
    snprintf(id, sizeof(id), "%d", currUser.userID);
    // name
    strcpy(name, currUser.name);
    // status
    strcpy(status, currUser.status);

    //Response del server del cliente
    cJSON *usr = NULL;

    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente, "status", "ok");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "user", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr, "id", id);
    cJSON_AddStringToObject(usr, "name", name);
    cJSON_AddStringToObject(usr, "status", status);
    out = cJSON_Print(Response_del_server_del_cliente);
    write(currUser.fileDescriptor, out, strlen(out));

    // printf("%s\n\n", out);
    cJSON_Delete(Response_del_server_del_cliente);
    free(out);
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

void initializeUsers()
{
    for (int i = 0; i < MAX_USERS; i++)
    {
        users[i].userID = -1;
    }
}

void GetAvailableUsers()
{
    int availableUsersCounter = 0;
    memset(availableUsers, 0, sizeof(availableUsers));
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (users[i].userID != -1)
        {
            availableUsers[availableUsersCounter] = users[i];
        }
    }
}

char *JSONfromUser(struct user givenUser)
{
    char *out;
    char id[10];
    char name[100];
    char status[100];
    char *infoOfUser;
    // id
    snprintf(id, sizeof(id), "%d", givenUser.userID);
    // name
    strcpy(name, givenUser.name);
    // status
    strcpy(status, givenUser.status);

    //Response del server del cliente
    cJSON *usr = NULL;
    cJSON_CreateArray;
    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente, "action", "LIST_USER");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "user", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr, "id", id);
    cJSON_AddStringToObject(usr, "name", name);
    cJSON_AddStringToObject(usr, "status", status);
    // id
    snprintf(id, sizeof(id), "%d", givenUser.userID);
    // name
    strcpy(name, givenUser.name);
    // status
    strcpy(status, givenUser.status);
    cJSON_AddItemToObject(Response_del_server_del_cliente, "asdf", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr, "id", "something");
    cJSON_AddStringToObject(usr, "name", "name");
    cJSON_AddStringToObject(usr, "status", "status");

    out = cJSON_Print(Response_del_server_del_cliente);
    infoOfUser = out;
    printf("%s\n\n", out);
    cJSON_Delete(Response_del_server_del_cliente);
    free(out);
    return infoOfUser;
}

void errorResponder(char *message)
{
    char *out;
    //En caso de un error
    cJSON *Error = cJSON_CreateObject();
    cJSON_AddStringToObject(Error, "status", "ERROR");
    cJSON_AddStringToObject(Error, "message", message);
    out = cJSON_Print(Error);
    printf("%s\n\n", out);
    cJSON_Delete(Error);
    free(out);
}

void mensajeJSON(struct user *userSender, struct user userResiver, char *message)
{
    char *out;
    cJSON *Mensaje = cJSON_CreateObject();
    cJSON_AddStringToObject(Mensaje, "action", "RECEIVE_MESSAGE");
    cJSON_AddStringToObject(Mensaje, "from", userSender->userID);
    cJSON_AddStringToObject(Mensaje, "to", userResiver.userID);
    cJSON_AddStringToObject(Mensaje, "message", message);
    out = cJSON_Print(Mensaje);
    write(userResiver.fileDescriptor, out, strlen(out));
    // printf("%s\n\n", out);
    cJSON_Delete(Mensaje);
    free(out);
}

void recivedJSON(char *info, struct user *actualUser)
{
    char *reciver;
    int compare;
    char *SEND = "SEND_MESSAGE";
    char *LIST = "LIST_USER";
    char *CHANGE = "CHANGE_STATUS";
    cJSON *json = cJSON_Parse(info);
    cJSON *getAction = cJSON_GetObjectItem(json, "action");
    char *action = cJSON_Print(getAction);
    compare = strncmp(SEND,action, sizeof(SEND));
    if (compare == 0){
        printf("SENT MESSAGE");
        // SEND MESSAGE
        cJSON *getID = cJSON_GetObjectItem(json, "to");
        // test char
        char id =  cJSON_Print(getID);
        int idInt = id - '0';
        cJSON *getMessage = cJSON_GetObjectItem(json, "message");
        char *message =  cJSON_Print(getMessage);
        mensajeJSON(actualUser, users[idInt], message);
    }
    compare = strncmp(LIST,action, sizeof(LIST));
    if (compare == 0){
        // LIST USER
        printf("LIST USER");
    }
    compare = strncmp(CHANGE,action, sizeof(CHANGE));
    if (compare == 0){
        // CHANGE USER MODE
        printf("Change User");
        
    }

    /* cJSON *pruebaidreal = cJSON_GetObjectItem(pruebaid, "id");
    cJSON *pruebanamereal = cJSON_GetObjectItem(pruebaid, "name");
    cJSON *pruebastatusreal = cJSON_GetObjectItem(pruebaid, "status");
    char *idusuario = cJSON_Print(pruebaidreal);
    char *nameusuario = cJSON_Print(pruebaidreal);
    char *statususuario = cJSON_Print(pruebaidreal);
    strcpy(usuario.userID, idusuario);
    strcpy(usuario.name, nameusuario);
    strcpy(usuario.status, statususuario); */
}

void firstJSONRecieve(char *informacionInutil, struct user newUser)
{
    printf("wooo");
    respuestaDeJSON(newUser);
}

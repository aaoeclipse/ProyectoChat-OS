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
#include <time.h>
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
    char userID[100];
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
char arr[5 + 1];
// Aqui es donde se van a guardar los usuairos, va a ser en la RAM
struct user users[MAX_USERS];
struct user availableUsers[MAX_USERS];

// METHODS
char *disconnectedUser(struct user *currUser);
void successfullMessage(int socketMessage);
int indexFromID(char *id);
void recivedJSON(char *info, struct user *actualUser);
void *createUser(void *usr);
int main(int argc, char const *argv[]);
char *nuevaConnexionAviso(struct user currUser);
void sendMessage(char message[1024], int userSocket, char *fromUser, int toUser);
char *noQuotes(char *withQuotes);
// @params struct user
// @return el usern en forma de JSON
char *JSONfromUser(struct user givenUser);
char *changeStatusUserMessage(struct user *currUser);
// void sendMessage(char *message, int successfulSocket, char *fromUser, int toUser);
int broadcastMessage(char *message);
// muestra toda la lista de
void listUsers();
char *LastcharDel(char *message);
void initializeUsers();
char *withSemi(char *stringWithoutSemi);
void GetAvailableUsers();
char *respuestaDeJSON(struct user createUser);
void mensajeJSON(struct user *userSender, struct user userResiver, char *message);
void firstJSONRecieve(char *informacionInutil, struct user newUser);
char *createUserJson(char *id, char *name, char *status);
char *randomStringFunction();
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
                printf("%s:%s BYE\n", userInfo->userID, userInfo->name);
                fflush(stdout);
                disconnectedUser(userInfo);
                break;
            }
            recivedJSON(bufferUser, userInfo);
        }
        else
        {
            printf("ERROR user %s:%s disconnected without handshake\n", userInfo->userID, userInfo->name);
            bzero(bufferUser, sizeof(bufferUser));
            disconnectedUser(userInfo);
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
    fflush(stdout); // printf will not work without this

    initializeUsers();

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
            char *randomString = randomStringFunction();
            /* 
                cJSON *json = cJSON_Parse(informacionInutil);
                cJSON *getIpDelUsuario = cJSON_GetObjectItem(json, "origin");
                char *ipDelUsuario = cJSON_Print(getIpDelUsuario);
                cJSON *getUser = cJSON_GetObjectItem(json, "user");
                char *username = cJSON_Print(getUser);
            */
            // Parse JSON and create USER
            strcpy(users[counter].userID, randomString);
            strcpy(users[counter].name, buffer);
            strcpy(users[counter].status, "active");
            // strcpy(newUser.address, ipDelUsuario);
            users[counter].fileDescriptor = new_socket;

            firstJSONRecieve(buffer, users[counter]);

            printf("Usuario: %s\n", users[counter].name);
            printf("ID: %s\n", users[counter].userID);
            printf("FileD.: %d\n\n", users[counter].fileDescriptor);

            // create user with socket
            rc = pthread_create(&threads[counter], NULL, createUser, (void *)&users[counter]);

            if (rc < 0)
                printf("ERROR in pthread");
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

void listUsers(int socketFromUser)
{
    GetAvailableUsers();
    FILE *f;
    f = fopen("file.txt", "w");
    if (!f)
    {
        printf("Error opening file!\n");
        fflush(stdout);
        //break;
    }
    else
    {

        char *intro;
        intro = "{ \n\"action\": \"LIST_USER\",\n\t\"users\":[";
        printf("list users");
        fflush(stdout);
        //fprintf(f,"%s",intro);
        fputs(intro, f);
        fclose(f);
        //f = fopen("file.txt", "a");
        for (int i = 0; i < MAX_USERS; i++)
        {
            f = fopen("file.txt", "a");
            int equals = strncmp("-1", availableUsers[i].userID, 10);
            if (equals == 0)
            {
                break;
            }
            fflush(stdout);
            /*Sirve para imprimir y guardar en el file*/

            createUserJson(availableUsers[i].userID, availableUsers[i].name, availableUsers[i].status);
            fputs(",", f);
            fclose(f);
        }
        fputs("]\n}", f);
        fclose(f);
        printf("antes de cat\n");
        fflush(stdout);
        FILE *fp = fopen("file.txt", "r");
        char c[1024];
        char messageToSend[1024];
        while (fgets(c, 1024, fp) != NULL)
        {
            strncat(messageToSend, c, sizeof(c));
        }

        fclose(fp);
        // printf("MESSAGE TO SENT: %s\n", messageToSend);
        
        fflush(stdout);
        write(socketFromUser, messageToSend, strlen(messageToSend));
        
        // return messageToSend;
        //remove(fp);
    }
}

char *createUserJson(char *id, char *name, char *status)
{
    //char userText [1000];
    char *idtext = "{\"id\": ";
    char *nametext = "\t\n\"name\": ";
    char *statustext = "\t\n\"status\": ";
    char *endtext = "}";
    char *coma = ",";
    char text[1000];
    strcpy(text, idtext);
    strcat(text, id);
    strcat(text, coma);
    strcat(text, nametext);
    strcat(text, name);
    strcat(text, coma);
    strcat(text, statustext);
    strcat(text, status);
    strcat(text, endtext);
    fflush(stdout);
    FILE *f;
    f = fopen("file.txt", "a");
    fputs(text, f);
    fclose(f);
    return text;
}
char *respuestaDeJSON(struct user currUser)
{
    char *out;
    char id[25];
    char name[100];
    char status[100];

    // address
    // char address[100];
    // strcpy(status, currUser.address);

    // id
    strcpy(id, currUser.userID);
    // name
    strcpy(name, currUser.name);
    // status
    strcpy(status, currUser.status);

    //Response del server del cliente
    cJSON *usr = NULL;

    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente, "status", "OK");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "user", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr, "id", id);
    cJSON_AddStringToObject(usr, "name", name);
    cJSON_AddStringToObject(usr, "status", status);
    out = cJSON_Print(Response_del_server_del_cliente);

    write(currUser.fileDescriptor, out, strlen(out));

    cJSON_Delete(Response_del_server_del_cliente);
    free(out);
}
// Funciones de mandar a los sockets para todos
char *nuevaConnexionAviso(struct user currUser)
{
    char *out;
    char id[25];
    char name[100];
    char status[100];
    // address
    // char address[100];
    // strcpy(status, currUser.address);
    // id
    strcpy(id, currUser.userID);
    // name
    strcpy(name, currUser.name);
    // status
    strcpy(status, currUser.status);
    //Response del server del cliente
    cJSON *usr = NULL;
    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente, "action", "USER_CONNECTED");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "user", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr, "id", id);
    cJSON_AddStringToObject(usr, "name", name);
    cJSON_AddStringToObject(usr, "status", status);
    out = cJSON_Print(Response_del_server_del_cliente);
    // AVAILABLE USERS
    GetAvailableUsers();
    int i = 0;
    int copmare;
    for (i = 0; i < MAX_USERS; i++)
    {
        copmare = strncmp("-1", availableUsers[i].userID, 10);
        if (copmare == 0)
            break;
        copmare = strncmp(currUser.userID, availableUsers[i].userID, 10);
        if (copmare != 0)
        {
            write(availableUsers[i].fileDescriptor, out, strlen(out));
        }
    }
    cJSON_Delete(Response_del_server_del_cliente);
    free(out);
}
char *disconnectedUser(struct user *currUser)
{
    char *out;
    char id[25];
    char name[100];
    char status[100];
    // address
    // char address[100];
    // strcpy(status, currUser.address);
    // id
    strcpy(id, currUser->userID);
    // name
    strcpy(name, currUser->name);
    // status
    strcpy(status, currUser->status);
    //Response del server del cliente
    cJSON *usr = NULL;
    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente, "action", "USER_DISCONNECTED");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "user", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr, "id", id);
    cJSON_AddStringToObject(usr, "name", name);
    cJSON_AddStringToObject(usr, "status", status);
    out = cJSON_Print(Response_del_server_del_cliente);
    // AVAILABLE USERS
    GetAvailableUsers();
    int i = 0;
    int copmare;
    for (i = 0; i < MAX_USERS; i++)
    {
        copmare = strncmp("-1", availableUsers[i].userID, 10);
        if (copmare == 0)
            break;
        copmare = strncmp(currUser->userID, availableUsers[i].userID, 10);
        if (copmare != 0)
        {
            write(availableUsers[i].fileDescriptor, out, strlen(out));
        }
    }
    cJSON_Delete(Response_del_server_del_cliente);
    free(out);
}
char *changeStatusUserMessage(struct user *currUser)
{
    char *out;
    char id[25];
    char name[100];
    char status[100];
    // address
    // char address[100];
    // strcpy(status, currUser.address);
    // id
    strcpy(id, currUser->userID);
    // status
    strcpy(status, currUser->status);
    //Response del server del cliente
    cJSON *usr = NULL;
    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente, "action", "USER_DISCONNECTED");
    cJSON_AddItemToObject(Response_del_server_del_cliente, "user", usr = cJSON_CreateObject());
    cJSON_AddStringToObject(usr, "user", id);
    cJSON_AddStringToObject(usr, "status", status);
    out = cJSON_Print(Response_del_server_del_cliente);
    // AVAILABLE USERS
    GetAvailableUsers();
    int i = 0;
    int copmare;
    for (i = 0; i < MAX_USERS; i++)
    {
        copmare = strncmp("-1", availableUsers[i].userID, 10);
        if (copmare == 0)
            break;
        copmare = strncmp(currUser->userID, availableUsers[i].userID, 10);
        if (copmare != 0)
        {
            write(availableUsers[i].fileDescriptor, out, strlen(out));
        }
    }
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
    int i = 0;
    // strcpy(users[0].userID, "-1");
    // printf("user[%d] = %s", users[0].userID);
    // fflush(stdout);
    for (i = 0; i < MAX_USERS; i++)
    {
        strcpy(users[i].userID, "-1");
        // users[i].userID = "-1";
    }
}

void GetAvailableUsers()
{
    int availableUsersCounter = 0;
    int negative;
    memset(availableUsers, 0, sizeof(availableUsers));

    for (int i = 0; i < MAX_USERS; i++)
    {
        strcpy(availableUsers[i].userID, "-1");
    }
    for (int i = 0; i < MAX_USERS; i++)
    {
        negative = strncmp("-1", users[i].userID, 10);
        if (negative != 0)
        {
            availableUsers[availableUsersCounter] = users[i];
            availableUsersCounter = ++availableUsersCounter;
        }
    }
}

char *JSONfromUser(struct user givenUser)
{
    char *out;
    char id[25];
    char name[100];
    char status[100];
    char *infoOfUser;
    // id
    strcpy(id, givenUser.userID);
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
    // snprintf(id, sizeof(id), "%d", givenUser.userID);
    strcpy(id, givenUser.userID);
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
    printf("(557~) JSON FROM USER: %s\n\n", out);
    cJSON_Delete(Response_del_server_del_cliente);
    free(out);
    return infoOfUser;
}

void errorResponder(char *message, int socket)
{
    char *out;
    //En caso de un error
    cJSON *Error = cJSON_CreateObject();
    cJSON_AddStringToObject(Error, "status", "ERROR");
    cJSON_AddStringToObject(Error, "message", message);
    out = cJSON_Print(Error);
    write(socket, out, strlen(out));

    // printf("%s\n\n", out);
    cJSON_Delete(Error);
    free(out);
}
// MANDAR
void mensajeJSON(struct user *userSender, struct user userResiver, char *message)
{
    char *out;
    // printf("Mensaje: %s", message);
    printf("Reciver: %s", userResiver.name);
    fflush(stdout);

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
    // printf("info: %s\n", info);
    char *reciver;
    int compare;
    char *SEND = "SEND_MESSAGE";
    char *LIST = "LIST_USER";
    char *CHANGE = "CHANGE_STATUS";
    cJSON *json = cJSON_Parse(info);
    cJSON *getAction = cJSON_GetObjectItem(json, "action");
    char *action = cJSON_Print(getAction);
    strcpy(action, noQuotes(action));
    printf("action: %s", action);
    fflush(stdout);
    compare = strncmp(SEND, action, sizeof(SEND));
    if (compare == 0)
    {
        printf("SENT MESSAGE\n");
        // SEND MESSAGE
        cJSON *getID = cJSON_GetObjectItem(json, "to");
        char *id = cJSON_Print(getID);
        strcpy(id, noQuotes(id));
        cJSON *getMessage = cJSON_GetObjectItem(json, "message");
        char *message = cJSON_Print(getMessage);
        // QUITAR LAS COMILLAS
        strcpy(message, noQuotes(message));
        // printf("id para mandar: %s\n", id);
        int idDelUsuario = indexFromID(id);
        // printf("index: %d\n", idDelUsuario);
        if (idDelUsuario >= 0)
        {
            fflush(stdout);
            mensajeJSON(actualUser, availableUsers[idDelUsuario], message);
            successfullMessage(actualUser->fileDescriptor);
        }
        else
        {
            errorResponder("No existe usuario", actualUser->fileDescriptor);
        }
    }
    compare = strncmp(LIST, action, sizeof(LIST));
    if (compare == 0)
    {
        // LIST USER
        printf("LIST USER\n");
        int tammao = cJSON_GetArraySize(json);
        printf("Tamano: %d\n", tammao);
        if (tammao == 1)
        {
            listUsers(actualUser->fileDescriptor);
            // printf("RESULT: %s", getListUsers);
            fflush(stdout);
            // successfullMessage(actualUser->fileDescriptor);
            // write(actualUser->fileDescriptor, getListUsers, strlen(getListUsers));
        }
        else
        {
            cJSON *getID = cJSON_GetObjectItem(json, "user");
            char *usersID = cJSON_Print(getID);
            int indexUsuario = indexFromID(usersID);
            if (indexUsuario >= 0)
            {
                fflush(stdout);
                char *jsonInfoUser = JSONfromUser(availableUsers[indexUsuario]); 
                write(actualUser->fileDescriptor,jsonInfoUser, strlen(jsonInfoUser) );
                // mensajeJSON(actualUser, users[idDelUsuario], message);
                successfullMessage(actualUser->fileDescriptor);
            }
            else
            {
                errorResponder("No existe usuario", actualUser->fileDescriptor);
            }
        }
    }
    compare = strncmp(CHANGE, action, sizeof(CHANGE));
    if (compare == 0)
    {
        // CHANGE USER MODE
        printf("Change Status");
        cJSON *getStatus = cJSON_GetObjectItem(json, "status");
        char *status = cJSON_Print(getStatus);
        // QUITAR LAS COMILLAS
        strcpy(status, noQuotes(status));
        strcpy(actualUser->status, status);
        printf("User %s, new status: %s", actualUser->name, actualUser->status);
        changeStatusUserMessage(actualUser);
        // successfull MSSG
        successfullMessage(actualUser->fileDescriptor);
    }
}

void firstJSONRecieve(char *informacionInutil, struct user newUser)
{
    // FIXME:
    //  * usuario tiene que mandar el JSON con ORIGIN, HOST y USERNAME
    //  * Descomentar el chunck!
    /* 
    cJSON *json = cJSON_Parse(informacionInutil);
    cJSON *getIpDelUsuario = cJSON_GetObjectItem(json, "origin");
    char *ipDelUsuario = cJSON_Print(getIpDelUsuario);
    cJSON *getUser = cJSON_GetObjectItem(json, "user");
    char *username = cJSON_Print(getUser);
 */
    // USER INFO FIXME: dsecomment y quitar el informacion Inutil
    /* char *randomString = randomStringFunction();
    strcpy(newUser.userID, randomString);
    // strcpy(newUser.name, username);
    // strcpy(newUser.name, informacionInutil);
    strcpy(newUser.status, "active");
    // strcpy(newUser.address, ipDelUsuario);
    newUser.fileDescriptor = new_socket; */

    respuestaDeJSON(newUser); // Responder a usuario
    // mandar mensaje a todos de conexion
    nuevaConnexionAviso(newUser);
}
void successfullMessage(int socketMessage)
{
    char *out;
    cJSON *usr = NULL;
    cJSON *Response_del_server_del_cliente = cJSON_CreateObject();
    cJSON_AddStringToObject(Response_del_server_del_cliente, "status", "OK");
    out = cJSON_Print(Response_del_server_del_cliente);
    write(socketMessage, out, strlen(out));
    cJSON_Delete(Response_del_server_del_cliente);
    free(out);
}
int indexFromID(char *id)
{
    GetAvailableUsers();
    int equals;
    int negative;
    for (int i = 0; i < MAX_USERS; i++)
    {
        // printf("id del user: %s\n", availableUsers[i].userID);
        negative = strncmp("-1", availableUsers[i].userID, 10);
        if (negative == 0)
        {
            break;
        }
        fflush(stdout);

        equals = strncmp(id, availableUsers[i].userID, 10);
        if (equals == 0)
        {
            printf("%s = %s Y tiene de socket %d", id, availableUsers[i].userID, availableUsers[i].fileDescriptor);
            printf("retornando!\n");
            fflush(stdout);
            return i;
        }
    }
    return -1;
}
char *randomStringFunction()
{
    int i, n, rnd;
    srand(time(NULL));
    for (i = 0; i < 5; ++i)
    {
        rnd = rand();
        n = (rnd >> 4) & 0xF;
        *(arr + i) = (rnd & 0xF) & 1
                         ? (n % 10) + '0'
                         : (n % 26) + 'A';
    }
    arr[5] = 0;
    return arr;
}
char *noQuotes(char *withQuotes)
{
    int i, len = strlen(withQuotes);
    for (i = 1; i < len - 1; i++)
    {
        withQuotes[i - 1] = withQuotes[i];
    }
    withQuotes[i - 1] = '\0';
    return withQuotes;
}
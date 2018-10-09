#include <pthread.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8080
#define NETWORKADDRESS 127.0.0.1 


int connectionVerification = 1;
/* connection busca verificar si hay conexion */
void *connection(void *null)
{
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        { 
            printf("\n Socket creation error \n"); 
            connectionVerification = 0; 
        } 
        memset(&serv_addr, '0', sizeof(serv_addr)); 
    
        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_port = htons(PORT); 

        // Convert IPv4 and IPv6 addresses from text to binary form 
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
        { 
            connectionVerification = 0;
        } 
    while (1 == 1){
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        { 
            printf("\nConnection Failed \n"); 
            connectionVerification = 0; 
        } else{
            connectionVerification = 1; 
            printf("\nGood Conecction");
        }

        sleep(5);
    }
        return NULL; 
}

int main()
{
    pthread_t connectionThread;
    /* create a second thread which executes inc_x(&x) */
    if (pthread_create(&connectionThread, NULL, connection, NULL)){
        fprintf(stderr, "Error creating thread\n");
        return 1;}
    /* wait for the second thread to finish */
    if (pthread_join(connectionThread, NULL)){
        fprintf(stderr, "Error joining thread\n");
        return 2;}
    if (connectionVerification = 0)
        fprintf(stderr, "Se desconecto");
    return 0;
}
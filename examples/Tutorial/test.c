#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char globalString[50];

char* withSemi(char *stringWithoutSemi){
    memset(globalString, 0, sizeof(globalString));
    char semi[50] = "\"";
    printf("withsemi\n");
    fflush(stdout);
    strcat(globalString, semi);
    strcat(globalString, stringWithoutSemi);
    printf("strcatonation\n");
    strcat(globalString, "\"");
    return (char *) globalString;
}

int main(){
    char name[50];
    char result[50];
    printf("hola\n");
    fflush(stdout);
    strcpy(name,"hello my name is santy");
    strcpy(result, withSemi(name));
    // result = withSemi(name);
    printf("%s",result);


    return -1;
}
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
int main(){
    char a[17] = "hello world";
    size_t n = sizeof(a) / sizeof(char);
    printf("size of a: %zu\n", n);  
    printf ("The sentence entered is %u characters long.\n",(unsigned)strlen(a));
    printf("char: %c",a[0]);
    return 0;
}
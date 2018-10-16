#include <stdio.h>

#include <sys/socket.h>

#include <stdlib.h>

#include <netinet/in.h>

#include <string.h>

int sfd;

void my_isr(int n)

{

    printf("In Isr...\n");

    write(sfd, "***********CLIENT TERMINATED !!!****************", 50);

    close(sfd);

    printf("SOCKET closed\n");

    kill(getpid(), 9);
}

int main(int argc, char *argv[])

{

    if (argc != 3)

    {

        printf("Usage:./a.out serverip portnumber\n");

        return;
    }

    struct sockaddr_in cln;

    char rdbuf[128];
    char wrbuf[128];

    sfd = socket(PF_INET, SOCK_STREAM, 0);

    memset(&cln, 0, sizeof(cln));

    if (sfd < 0)

    {

        perror("socket");

        exit(1);
    }

    cln.sin_family = PF_INET;

    cln.sin_addr.s_addr = inet_addr(argv[1]);

    cln.sin_port = htons(atoi(argv[2]));

    if (connect(sfd, (struct sockaddr *)&cln, sizeof(cln)) == 0)

        printf("Connection Established Successfully!!!\n");

    else

    {

        perror("connect");

        return;
    }

    memset(rdbuf, 0, 128);

    memset(wrbuf, 0, 128);

    signal(2, my_isr);

    if (fork() == 0)

    {

        while (1)

        {

            if (read(sfd, rdbuf, 128) != 0)

                puts(rdbuf);
        }
    }

    else

    {

        while (1)

        {

            gets(wrbuf);

            write(sfd, wrbuf, 128);
        }
    }
}
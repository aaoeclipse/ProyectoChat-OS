#include <stdio.h>

#include <sys/socket.h>

#include <stdlib.h>

#include <netinet/in.h>

#include <string.h>

int sfd, newsfd;

void my_isr(int n)

{

    printf("in isr...\n");

    write(newsfd, "***********SERVER TERMINATED!!!**********", 50);

    close(sfd);

    printf("SOCKET closed\n");

    kill(getpid(), 9);
}

int main(int argc, char *argv[])

{

    if (argc != 2)

    {

        printf("./a.out portnumber\n");

        return;
    }

    struct sockaddr_in srv, cln;

    char rdbuf[128];
    char wrbuf[128];

    sfd = socket(PF_INET, SOCK_STREAM, 0);

    memset(&srv, 0, sizeof(srv));

    memset(rdbuf, 0, 128);

    memset(wrbuf, 0, 128);

    if (sfd < 0)

    {

        perror("socket");

        exit(1);
    }

    srv.sin_family = PF_INET;

    //srv.sin_addr.s_addr=inet_addr(argv[1]);

    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    srv.sin_port = htons(atoi(argv[1]));

    if (bind(sfd, (struct sockaddr *)&srv, sizeof(srv)) == 0)

        printf("Bind Sucess...\n");

    else

    {

        perror("bind");

        exit(1);
    }

    listen(sfd, 5);

    int len = sizeof(cln);

    newsfd = accept(sfd, (struct sockaddr *)&cln, &len);

    if (newsfd < 0)

    {

        perror("accept");

        exit(1);
    }

    else

        printf("New Connection established...\n");

    signal(2, my_isr);

    if (fork() == 0)

    {

        while (1)

        {

            gets(wrbuf);

            write(newsfd, wrbuf, 128);
            
    }

    else

    {

        while (1)

        {

            if (read(newsfd, rdbuf, 128) != 0)

                puts(rdbuf);
        }
    }
}
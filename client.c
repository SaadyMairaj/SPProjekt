#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <error.h>
#include <stdlib.h>

// Socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


int main(int argc, char const *argv[])
{
    char buffer[500];
    int answer = 1;
    char output[500];
    int line = 1;

    // Socket Client Code
    int sock;
    struct sockaddr_in server;
    struct hostent *hp;

    // Creating a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Opening Sock Stream");
        exit(1);
    }

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if(hp == 0){
        // fprintf(stderr, "%s: Unknown host\n", argv[1] );
        write(STDOUT_FILENO, "Unknown Host\n", sizeof("Unknown Host\n"));
        exit(2);
    }

    bcopy(hp->h_addr, &server.sin_addr, hp->h_length);

    server.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Connecting Stream Socket");
        exit(1);
    }

    write(STDOUT_FILENO, "Welcome to the Client Console!!\n", sizeof("Welcome to the Client Console!!\n"));

    while(line > 0){
        write(STDOUT_FILENO, "\n>> ", sizeof("\n>> "));
        line = read(STDIN_FILENO, buffer, sizeof(buffer));
        if(line < 2){
            continue;
        }

        char testBuff[1024];

        strcpy(testBuff, buffer);

        char *token = strtok(testBuff, " ");

        if(strcasecmp(token, "exit")==0){
            write(sock, buffer, line);
            break;
        }

        write(sock, buffer, line);

        answer = read(sock, output, sizeof(output));
        write(STDOUT_FILENO, output, answer);
    }
    close(sock);
    return 0;
}
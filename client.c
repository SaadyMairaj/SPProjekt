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

// Threads Includes
#include <pthread.h>

void *ReaderFunction(void *ptr){
    char buffer[500];
    int sock = *((int *) ptr);
    // printf("In Reader: %d\n", sock);
    // printf("In Yo\n");
    while(1){
        // printf("In reader whiles\n");
        int reading = read(sock, buffer, sizeof(buffer));
        if(reading < 0){
            perror("Socket Read");
            pthread_exit(0);
        }
        else if (reading == 0)
        {
            // write(STDOUT_FILENO, "In Reading == 0", sizeof("In Reading == 0"));
            continue;
        }

        write(STDOUT_FILENO, buffer, reading);
        write(STDOUT_FILENO, "\n>> ", sizeof("\n>> "));
        // printf("Yo5\n");
    }


}

int SocketConnection(char *socketInfo[]){
     // Socket Client Code

    // for (int i = 0; i < 3; ++i)
    // {
    //     printf("%s\n", socketInfo[i] );
    // }

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
    hp = gethostbyname(socketInfo[1]);
    if(hp == 0){
        // fprintf(stderr, "%s: Unknown host\n", argv[1] );
        write(STDOUT_FILENO, "Unknown Host\n", sizeof("Unknown Host\n"));
        exit(2);
    }

    bcopy(hp->h_addr, &server.sin_addr, hp->h_length);

    server.sin_port = htons(atoi(socketInfo[2]));

    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Connecting Stream Socket");
        return -1;
    }
    // printf("%d\n", sock);

    write(STDOUT_FILENO, "Welcome to the Client Console!!\n", sizeof("Welcome to the Client Console!!\n"));

    return sock;

}

int NotConnectedToServer(){
    char buffer[500];
    int line = 0;
    // printf("you\n");
    write(STDOUT_FILENO, "\n>> ", sizeof("\n>> "));
    line = read(STDIN_FILENO, buffer, sizeof(buffer));
    buffer[line-1] = 0;
    int flag = 0;
    int sock = 0;
    char *space = strchr( buffer, ' ' );
    if (space){
        char *savePtr;
        char *token = strtok_r(buffer, " ", &savePtr);
        if(strcasecmp(token, "connect")==0){
            char* args[3];
            args[0] = "./client";
            for (int i = 1; i < 3; ++i)
            {
                char *partialToken = strtok_r(NULL, " ", &savePtr);
                if(partialToken != NULL){
                    args[i] = partialToken;
                }
                else{
                    if(i != 2){
                        write(STDOUT_FILENO, "Error Detected\n", sizeof("Error Detected\n"));
                        flag = 1;
                        break;
                    }
                }

            }

            if (flag == 0)
            {
                sock = SocketConnection(args);
            }
        }

        else{
            write(STDOUT_FILENO, "Unable to Process. Not Connected to Server\n", sizeof("Unable to Process. Not Connected to Server\n"));
        }
    }

    else{

        write(STDOUT_FILENO, "Unable to Process. Not Connected to Server\n", sizeof("Unable to Process. Not Connected to Server\n"));
    }

    return sock;
}


int main(int argc, char const *argv[])
{
    pthread_t reader;
    char buffer[500];
    int answer = 1;
    char output[500];
    int line = 1;
    int sock = 0;


    write(STDOUT_FILENO, "Welcome!!\n", sizeof("Welcome!!\n"));
    int arguments = argc;

    while(1){

        if(arguments == 3){
            char *args[3];
            memcpy(args, argv, sizeof(char *) * 3);
            sock = SocketConnection(args);
            if(sock == -1){
                arguments = 1;
                continue;
            }
            else{
                break;
            }
        }

        else if(arguments <= 1){
            sock = NotConnectedToServer();
            if(sock != -1){
                break;
            }
            else{
                continue;
            }
        }

        else{
            write(STDOUT_FILENO, "Error Detected...\nPlease Try Again\n", sizeof("Error Detected...\nPlease Try Again\n"));
            continue;
        }
    }


    // int *ptr = &sock;
    // *ptr = sock;
    while(1){

        if(sock == 0){
            // printf("sock == -1\n");
            sock = NotConnectedToServer();
            continue;
        }

        else if(sock != -1){
            pthread_create(&reader, NULL, ReaderFunction, &sock);
            while(line > 0){
                // sock = NotConnectedToServer();
                write(STDOUT_FILENO, "\n>> ", sizeof("\n>> "));
                // printf("Yo4\n");
                line = read(STDIN_FILENO, buffer, sizeof(buffer));
                if(line < 3){
                    line = 2;
                    // printf("Yo1\n");
                    continue;
                }
                char testBuff[1024];
                strcpy(testBuff, buffer);
                testBuff[line-1] = 0;

                char *space = strchr( testBuff, ' ' );
                if (space){
                    // char *savePtr;
                    // char *token = strtok_r(testBuff, " ", &savePtr);
                    // if(strcasecmp(token, "connect")==0){
                    //     char* args[3];
                    //     int flag = 0;
                    //     args[0] = "./client";
                    //     for (int i = 1; i < 3; ++i)
                    //     {
                    //         char *partialToken = strtok_r(NULL, " ", &savePtr);
                    //         if(partialToken != NULL){
                    //             args[i] = partialToken;
                    //         }
                    //         else{
                    //             if(i != 2){
                    //                 write(STDOUT_FILENO, "Error Detected\n", sizeof("Error Detected\n"));
                    //                 flag = 1;
                    //                 break;
                    //             }
                    //         }

                    //     }

                    //     if (flag == 0)
                    //     {
                    //                 /* code */
                    //         sock = SocketConnection(args);
                    //         // if(sock != -1){
                    //         //     break;
                    //         // }
                    //         // else{
                    //         //     continue;
                    //         // }

                    //     }

                    // write(sock, buffer, line);
                    // write(STDOUT_FILENO, "Disconnected From Server..", sizeof("Disconnected From Server.."));
                    // close(sock);



                        // sock = SocketConnection();
                        // printf("%d\n", sock);

                    // }
                }

                else{
                    char *word  = testBuff;
                    if(strcasecmp(word, "exit")==0){
                        write(sock, "disconnect", sizeof("disconnect"));
                        write(STDOUT_FILENO, "Bye!", sizeof("Bye!"));
                        close(sock);
                        exit(-1);
                    }
                    else if (strcasecmp(word, "disconnect")==0)
                    {
                        write(sock, "disconnect", sizeof("disconnect"));
                    // close(sock);
                        // sock = NotConnectedToServer();
                        // if(sock != -1){
                        //     break;
                        // }
                        // else{
                        //     continue;
                        // printf("Yo2\n");
                        // }
                        pthread_cancel(reader);
                        break;


                    }

               }

               write(sock, buffer, line);



            // answer = read(sock, output, sizeof(output));
            // write(STDOUT_FILENO, output, answer);

           }
           pthread_join(reader, NULL);

           close(sock);

           sock = 0;
           while(sock <= 0){
                sock = NotConnectedToServer();
           }
        }
   }

   return 0;
}
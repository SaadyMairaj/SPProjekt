#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>

// Server Includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define TRUE 1

#define OUTPUT 200
#define MAX_PRINT 10000
#define BUFF_SIZE 1024
#define TIME_SIZE 26
#define PROCESS_LIST_SIZE 128
#define FD_SIZE 2

struct Table {
    int pid;
    time_t begin;
    time_t end;
    char name[200];
};

int tableIndex = 0;
struct Table processList[PROCESS_LIST_SIZE];


void AddIntegers(char arr[], int msgsock){
    int Number = 0;
    char msg[OUTPUT];
    char *savePtr;
    char *token = strtok_r(arr, " ", &savePtr);
    while(token != NULL){
        int value = atoi(token);
        Number += value;
        token = strtok_r(NULL, " ", &savePtr);
    }
    // sleep(5);
    int toWrite = sprintf(msg, "The Answer is : %d", Number);
    write(msgsock, msg, toWrite);
}

void SubtractIntegers(char arr[], int msgsock){
    int Number = 0;
    char msg[OUTPUT];
    char *savePtr;
    char *token = strtok_r(arr, " ", &savePtr);
    token = strtok_r(NULL, " ", &savePtr);
    Number = atoi(token);
    token = strtok_r(NULL, " ", &savePtr);
    while(token != NULL){

        Number = Number - atoi(token);
        token = strtok_r(NULL, " ", &savePtr);
    }
    int toWrite = sprintf(msg, "The Answer is : %d", Number);
    write(msgsock, msg, toWrite);

}

void MultiplyIntegers(char arr[], int msgsock){
    int Number = 0;
    char msg[OUTPUT];
    char *savePtr;
    char *token = strtok_r(arr, " ", &savePtr);
    token = strtok_r(NULL, " ", &savePtr);
    Number = 1;
    while(token != NULL){

        int value = atoi(token);
        Number *= value;
        token = strtok_r(NULL, " ", &savePtr);
    }
    int toWrite = sprintf(msg, "The Answer is : %d", Number);
    write(msgsock, msg, toWrite);
}

void DivideIntegers(char arr[], int msgsock){
    int flag = 0;
    int Number = 0;
    char msg[OUTPUT];
    char *savePtr;
    char *token = strtok_r(arr, " ", &savePtr);
    token = strtok_r(NULL, " ", &savePtr);
    Number = atoi(token);
    token = strtok_r(NULL, " ", &savePtr);
    while(token != NULL){
        int value = atoi(token);
        if (value == 0)
        {
            write(msgsock,"Division By 0 not Allowed\n", sizeof("Division By 0 not Allowed\n"));
            flag = 1;
            break;
        }

        Number /= value;
        token = strtok_r(NULL, " ", &savePtr);
    }
    if(flag == 1){

    }
    else{
        int toWrite = sprintf(msg, "The Answer is : %d", Number);
        write(msgsock, msg, toWrite);
    }

}

void RunProgram(char arr[], int msgsock){
    char *arga[BUFF_SIZE];
    char *savePtr;
    char *token = strtok_r(arr, " ", &savePtr);
    token = strtok_r(NULL, " ", &savePtr);
    int index = 0;

    while(token != NULL){
        arga[index] = token;
        index++;
        token = strtok_r(NULL, " ", &savePtr);
    }

    arga[index] = 0;

    int fd[2];
    pipe(fd);

    pid_t pid;
    pid = fork();

    if(pid < 0){
        perror("Error, Child Not Created");
        write(msgsock, "Failure", sizeof("Failure"));
        return;
    }
    else if(pid > 0){ // In Parent
        processList[tableIndex].pid = pid;
        strcpy(processList[tableIndex].name, *arga);
        char timer[26];
        time(&processList[tableIndex].begin);
        tableIndex++;

        close(fd[1]);
        char buff[10];

        int reading = read(fd[0], buff, sizeof(buff));
        if(reading == 0){
            write(msgsock, "Success!!", sizeof("Success!!"));
        }
        else{
            write(msgsock, "Failure!!", sizeof("Failire"));
        }
    }

    else if(pid == 0){ // In Child
        close(fd[0]);
        fcntl(fd[1], F_SETFD, FD_CLOEXEC);

        int result = execvp(*arga, arga);
        if(result < 0){
            perror("Error");
            write(fd[1], "Failure", sizeof("Failure"));
            kill(getppid(), SIGUSR2);
            raise(SIGTERM);
        }
    }
}

void KillProgram(char arr[], int msgsock){
    char *savePtr;
    char *token = strtok_r(arr, " ", &savePtr);
    token = strtok_r(NULL, " ", &savePtr);
    char *program;
    if (program = strtok_r(NULL, " ", &savePtr))
    {
        if(strcasecmp(program, "ALL")==0){
            char *ptr;
            long ret;
            int processId;

            ret = strtol(token, &ptr, 10);

            for (int i = 0; i < tableIndex; i++){
                if(strcasecmp(processList[i].name, ptr)==0 && processList[i].end == 0){
                    processId = processList[i].pid;

                    int returnValueKill = kill(processId, SIGTERM);
                    if(returnValueKill < 0){
                        perror("Kill");
                        write(msgsock, "Failure to Terminate", sizeof("Failure to Terminate"));

                    }
                    time(&processList[i].end);
                }
            }
            write(msgsock, "Successfully Terminated!", sizeof("Successfully Terminated!"));
        }
        else{
            write(msgsock, "Failure to Terminate", sizeof("Failure to Terminate"));
        }

    }

    else{
        char *ptr;
        long ret;
        int processId;

        ret = strtol(token, &ptr, 10);

        if(ret > 0){
            int pidToKill = 0;
            for (int i = 0; i < tableIndex; ++i){
                if(processList[i].pid == ret){
                    break;
                    pidToKill = i;
                }
            }
            int returnValueKill = kill(ret, SIGTERM);
            if(returnValueKill < 0){
                perror("Kill");
                write(msgsock, "Failure to Terminate", sizeof("Failure to Terminate"));
                return;
            }

            time(&processList[pidToKill].end);
            write(msgsock, "Successfully Terminated!", sizeof("Successfully Terminated!"));
        }

        else{
            int pidToKill = 0;
            for (int i = 0; i < tableIndex; ++i){

                if(strcasecmp(processList[i].name, ptr)==0 && processList[i].end == 0){
                    processId = processList[i].pid;
                    pidToKill = i;
                    break;
                }
            }

            int returnValueKill = kill(processId, SIGTERM);
            if(returnValueKill < 0){
                perror("Kill");
                write(msgsock, "Failure to Terminate", sizeof("Failure to Terminate"));
                return;
            }
            time(&processList[pidToKill].end);
            write(msgsock, "Successfully Terminated!", sizeof("Successfully Terminated!"));
        }
    }
}

void ListProgram(char arr[], int msgsock){
    char *savePtr;
    char *token = strtok_r(arr, " ", &savePtr);
    if(token = strtok_r(NULL, " ", &savePtr)){
        if(strcasecmp(token, "ALL")==0){
            char toPrint[MAX_PRINT];
            strcpy(toPrint, "Name\t\t\tProcess Id\t\t\tStart Time\t\t\tEnd Time\n");
            char output[OUTPUT];
            int i = 0;
            for (i = 0; i < tableIndex; i++){
                char timerStart[TIME_SIZE];
                struct tm* tmInfoStart;
                tmInfoStart = localtime(&processList[i].begin);

                strftime(timerStart, 26, "%Y-%m-%d %H:%M:%S", tmInfoStart);

                char timerEnd[TIME_SIZE];

                if(processList[i].end == 0){
                    timerEnd[0] = 0;
                }

                else{
                    struct tm* tmInfoEnd;
                    tmInfoEnd = localtime(&processList[i].end);
                    strftime(timerEnd, 26, "%Y-%m-%d %H:%M:%S", tmInfoEnd);
                }
                int out = sprintf(output, "%s\t\t\t%d\t\t\t%s\t\t\t%s\n", processList[i].name, processList[i].pid, timerStart, timerEnd);
                strcat(toPrint, output);
            }

            write(msgsock, toPrint, strlen(toPrint));
        }
        // write(msgsock, "Error", sizeof("Error"));
    }
    else{
        char toPrint[MAX_PRINT];
        char output[OUTPUT];
        int i = 0;
        strcpy(toPrint, "Name\t\t\tProcess Id\t\t\tStart Time\n");
        for (i = 0; i < tableIndex; i++){
            if(processList[i].end == 0){
                char timer[26];
                struct tm* tm_info;
                tm_info = localtime(&processList[i].begin);

                strftime(timer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

                int out = sprintf(output, "%s\t\t\t%d\t\t\t%s\n", processList[i].name, processList[i].pid, timer);
                strcat(toPrint, output);
            }
        }
        write(msgsock, toPrint, strlen(toPrint));
    }
}


void signalHandlerExec(int signo){
    if(signo == SIGCHLD){
        // TODO, reduce tableIndex...
        int status = -15;
        for (int i = 0; i < tableIndex; i++){
            int k = waitpid(processList[i].pid, &status, WNOHANG);
            if(WIFEXITED(status)){
                time(&processList[i].end);
            }
        }
    }
}

void signalHandlerFailExec(int signo){
    if(signo == SIGUSR2){
        tableIndex--;
    }
}


int main(int argc, char const *argv[])
{

    char buffer[BUFF_SIZE];
    char testBuff[BUFF_SIZE];
    char *token;
    int line = 2;
    int Number = 0;
    char sockInfo[BUFF_SIZE];

    // Socket Code..
    int sock, length;
    struct sockaddr_in server;
    int msgsock;

     if(signal(SIGCHLD, signalHandlerExec)==SIG_ERR){
                write(STDOUT_FILENO, "Error in Signal Handler\n", sizeof("Error in Signal Handler\n"));
    }

    if(signal(SIGUSR2, signalHandlerFailExec)==SIG_ERR){
        write(STDOUT_FILENO, "Error in Signal Handler\n", sizeof("Error in Signal Handler\n"));
    }

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Opening Sock Stream");
        exit(0);
    }

    // Name socket using wildcards
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = 12129;

    if(bind(sock, (struct sockaddr *)&server, sizeof(server))){
        perror("Binding Stream Socket");
        exit(0);
    }

    // Find out assigned port and print it out
    length = sizeof(server);
    if(getsockname(sock, (struct sockaddr *)&server, (socklen_t *)&length )){
        perror("Getting Sock Name");
        exit(0);
    }

    int sizeofSock = sprintf(sockInfo, "Server is now Listening for Connections on PORT : %d\n", ntohs(server.sin_port));

    // Start listening for connections
    listen(sock, 5);
    write(STDOUT_FILENO, sockInfo, sizeofSock);

    do{
        msgsock = accept(sock, 0, 0);

        if(msgsock == -1){
            perror("Accept");
        }
        else{
            line = 2;
            write(STDOUT_FILENO, "\nThe Server Has Accepted a Connection Successfully...\nYou may send arguments via the child terminal... Cheers\n", sizeof("\nThe Server Has Accepted a Connection Successfully...\nYou may send arguments via the child terminal... Cheers\n"));

            while(line > 1){
                Number = 0;
                line = read(msgsock, buffer, sizeof(buffer));

                if(line == 1){
                    line = 2;
                    continue;
                }

                char *newline = strchr( buffer, '\n' );
                if (newline){
                    *newline = 0;
                }

                strcpy(testBuff, buffer);

                char *savePtr;
                token = strtok_r(testBuff, " ", &savePtr);

                if(strcasecmp(token, "add")==0){
                    AddIntegers(buffer, msgsock);
                }

                else if(strcasecmp(token, "sub")==0){
                    SubtractIntegers(buffer, msgsock);
                }

                else if(strcasecmp(token, "mul")==0){
                    MultiplyIntegers(buffer, msgsock);
                }

                else if(strcasecmp(token, "div")==0){
                    DivideIntegers(buffer, msgsock);
                }

                else if(strcasecmp(token, "RUN")==0){
                    RunProgram(buffer, msgsock);
                }

                else if(strcasecmp(token, "LIST")==0){
                    ListProgram(buffer, msgsock);
                }

                else if(strcasecmp(token, "kill")==0){
                    KillProgram(buffer, msgsock);
                }

                else if(strcasecmp(token, "exit")==0){
                    // int exitKill = kill(getpid(), SIGTERM);
                    // if(exitKill < 0){
                    //     write(msgsock, "Could Not Exit! :(", sizeof("Could Not Exit! :("));
                    //     continue;
                    // }

                    write(STDOUT_FILENO, "Thank You For Using This Software!\n\n", sizeof("Thank You For Using This Software!\n\n"));
                    exit(0);
                }

                else if(strcasecmp(token, "HELP")==0){
                    write(msgsock, "\nThe shell has the following basic commands:\n\tADD: ADD <params[]>:Can pass multiple parameters after the keyword ADD 4 5\n\tSUB: SUB <params>: Can pass multiple parameters after the keyword SUB e.g SUB 4 5 6\n\tMUL: MUL <params> : Can pass multiple parameters after the keyword MUL e.g MUL 4 5 6\n\tDIV: DIV <params>: Can pass multiple parameters after the keyword DIV e.g DIV 4 5 6\n\tRUN : RUN <filename>: Creates a new Process of given filename\n\tHELP: Shows the help", sizeof("\nThe shell has the following basic commands:\n\tADD: ADD <params[]>:Can pass multiple parameters after the keyword ADD 4 5\n\tSUB: SUB <params>: Can pass multiple parameters after the keyword SUB e.g SUB 4 5 6\n\tMUL: MUL <params> : Can pass multiple parameters after the keyword MUL e.g MUL 4 5 6\n\tDIV: DIV <params>: Can pass multiple parameters after the keyword DIV e.g DIV 4 5 6\n\tRUN : RUN <filename>: Creates a new Process of given filename\n\tHELP: Shows the help"));
                }

                else{
                    write(msgsock, ":Command Not Recognized\n", sizeof(":Command Not Recognized\n"));
                }
            }
        }
        write(STDOUT_FILENO, "Client Closed", sizeof("Client Closed"));
        close(msgsock);
    } while(TRUE);

    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>

#define MAX 50
#define SIZE sizeof(int);

// Helper functions
char *int_to_str(int num, char *str)
{
    if(str == NULL)
    {
        return NULL;
    }
    sprintf(str, "%d", num);
    return str;
}

char *slice(char *str, int start, int end)
{

    int i;
    int size = (end - start) + 1;
    char *output = (char *)malloc(size * sizeof(char));

    for (i = 0; start <= end; start++, i++)
    {
        output[i] = str[start];
    }

    output[size] = '\0';

    return output;
}

// bloqueio de envio da token
bool lock(float probability) {
    return rand() % 100 < (probability * 100);
}
//   Create pipes (Temporary files in ./tmp)
bool createPipes(int n, char* fifos[n][MAX]){
    char filepath[MAX];
    int j = 0;
    char str[5];
    for(int i = 1; i <= n ; i++){
        strcpy(filepath, "./tmp/pipe");
        strcpy(str, "");
        strcat(filepath,int_to_str(i, str));
        strcat(filepath, "to");
        if(i == n){
            strcat(filepath, "1");
        }else{
            strcat(filepath, int_to_str(i+1, str));
        }
        strcpy(fifos[j], filepath);
        j++;
    }

    for(int i =0 ; i < n ; i++){
        char fileToCreate = fifos[i];
        if ((mkfifo(fileToCreate,0666)) != 0) {
            if(errno == 17){ // If a file with the same name exists, this overwrites it
                unlink(fileToCreate);
                mkfifo(fileToCreate,S_IRWXU);
            }else{
                printf("Unable to create a fifo; errno=%d\n",errno);
                return false;
            }
        }
    }
    return true;

}


int main(int argc, char** argv) {
    if(argc < 4){
        printf("Usage: ./tokenring n_processes probability time\n\r");
        return EXIT_FAILURE;
    }

    //                 Input validation, definitions

    // If the user doesn't comply with the ranges it'll use the default values ./tokenring 5 0.25 2

    // Nr of processes -- n > 1
    int n = atoi(argv[1]) > 1 ? atoi(argv[1]) : 5;
    // Probabilty -- 100% >= p >= 1%
    float probability =  (atof(argv[2]) >= 0.001 && atof(argv[2]) <= 1.0) ? atof(argv[2]) : 0.25;
    // Time -- time > 0;
    int time = atoi(argv[3]) > 0 ? atoi(argv[3]) : 2;
    //File descriptors and token
    int fd[2], token = 0;
    pid_t  pid[n];
    // Pipes path array
    char* fifos[n][MAX];
    //Filepaths for pipes i and i+1
    char file1=(char) malloc(MAX*sizeof(char)) , file2=(char) malloc(MAX*sizeof(char));

    //If there's any error while creating the fifos it should quit
    if(!createPipes(n, *fifos)){
        return 2;
    }


    for (int i=1; i <= n; i++) {

        if((pid[i-1] = fork())== -1) {
            fprintf(stderr, "./tokenring: fork errno=%d\n",errno);
            return 2;

            //  Child process created successfuly
        } else if(pid[i-1] == 0) {


            if(i == n) {
                sprintf(file1, "./tmp/pipe%dto1", i);
                sprintf(file2, "./tmp/pipe%dto%d", i-1, i);
            } else if(i == 1) {
                sprintf(file1, "./tmp/pipe%dto%d", i, i+1);
                sprintf(file2, "./tmp/pipe%dto1", n);
            } else {
                sprintf(file1, "./tmp/pipe%dto%d", i, i + 1);
                sprintf(file2, "./tmp/pipe%dto%d", i - 1, i);

            }


            if(i==1) {
                if ((fd[1] = open(file1, O_WRONLY)) == -1) {
                    fprintf(stderr, "./tokenring: pipe opening errno=%d\n", errno);
                    return 2;
                }

                if (write(fd[1], &token, sizeof(token)) == -1) {
                    fprintf(stderr, "./tokenring: write errno=%d\n",errno);
                    return 2;
                }

                close(fd[1]);
            }


            while (true) {
                // Opening and reading from previous pipe
                if ((fd[0] = open(file2, O_RDONLY)) == -1) {
                    fprintf(stderr, "./tokering: pipe opening errno=%d\n",errno);
                    return 2;
                }
                if (read(fd[0], &token, sizeof(token)) == -1) {
                    fprintf(stderr, "./tokenring: read errno=%d\n", errno);
                    return 2;
                }
                close(fd[0]);


                token++;

                if (lock(probability)) {
                    printf("[p%s] lock on token (val = %d)\n", slice(file2, 10, 10), token);
                    sleep(time);
                    printf("[p%s] unlock on token\n", slice(file2, 10, 10));
                }

                // Opening and writing on current pipe
                if((fd[1] = open(file1, O_WRONLY)) == -1) {
                    fprintf(stderr, "./tokenring: pipe opening errno=%d\n", errno);
                    return 2;
                }

                if(write(fd[1], &token, sizeof(token)) == -1) {
                    fprintf(stderr, "./tokenring: write errno=%d\n",errno);
                    return 2;
                }
                close(fd[1]);

            }
            return 0;
        }

    }

    // Parent process has to wait for all of its children to end, otherwise we'd have zombie/orphan processes

    for(int i =0 ; i < n ; i++){
        if(waitpid(pid[i], NULL, 0)  == -1) {
            fprintf(stderr, "./tokenring: waitpid error: %d\n",errno);
            return 2;
        }
    }
}

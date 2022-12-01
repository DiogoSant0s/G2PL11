#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>


int main (int argc, char *argv[]) {
    /* check number of args */
    if(argc != 4) {
        printf("usage: ./tokenring number_process probability number_seconds\n");
        return EXIT_FAILURE;
    }
    
    int n = atoi(argv[1]);
    int p = atoi(argv[2]);
    int t = atoi(argv[3]);

    /* check if probablitity is between zero and a hundred */
    if(p > 100 || p < 0) {
        printf("probability must be between 0 and 100\n");
        return EXIT_FAILURE;
    }

    /* check if number of processes is more than one */
    if(n < 2) {
        printf("must have more than one process\n");
        return EXIT_FAILURE;
    }

    /* check if number of seconds is a positive number */
    if(t <= 0) {
        printf("number of seconds must be positive\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

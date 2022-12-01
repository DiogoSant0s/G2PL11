#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, const char *argv[]) {

    char epubs[argc-1];
    for (int i = 1; i < argc; i++) {

        pid_t pid;

        if (( pid = fork() ) == -1) {           //fork error
            fprintf(stderr, "%s: can't fork command: %s\n", argv[0], strerror(errno));
            continue;
        }
        else if (pid == 0) {            //child process
            //pid_t n;
            const char * filename_txt = argv[i];
            char filename[strlen(filename_txt)];

            strcpy(filename, filename_txt);
            filename[strlen(filename) - 4] = '\0';
            strcat(filename, ".epub");

            epubs[i-1] = malloc( (strlen(filename)+1) * sizeof(char) );
            strcpy(epubs[i-1], filename);

            printf("[pid%i] converting %s ... \n", getpid(), filename_txt);
            execlp("pandoc", "pandoc", filename_txt, "-o", filename, (char *)0);

        } 
        else {
            if (waitpid(pid, NULL, 0) == -1) {
                perror("wait");
                return EXIT_FAILURE;
            }
        }

        //remove(argv[i]);
    }

   char *zip[argc + 4];
    zip[0] = malloc((strlen("zip") + 1) * sizeof(char));
    strcpy(zip[0], "zip");
    zip[1] = malloc((strlen("ebooks.zip") + 1) * sizeof(char));
    strcpy(zip[1], "ebooks.zip");
    for(int k = 0; k < argc - 1; k++){
        char temp[sizeof(argv[k + 1]+1)] = {}, *dot;
        strcpy(temp, argv[k + 1]);
        dot = strchr(temp, '.');
        if(dot != 0){*dot = 0;}
        strcat(temp, ".epub");
        zip[k + 2] = malloc((strlen(temp) + 1) * sizeof(char));
        strcpy(zip[k+2], temp);
    }
    zip[argc + 2] = "--quiet";
    zip[argc + 3] = NULL;

    execvp(zip[0], zip);

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/wait.h>

void pandoc_converter(char *fname) {
    char *command = "pandoc";
    char *setting = "-o";
    char *setting1 = "--quiet"; //no warnings
    char *arg1 = fname;
    //file output
    char *output_fname = strdup(fname);
    int len = strlen(arg1);
    // tirar .txt e pôr .epub
    output_fname[len] = 'b';
    output_fname[len - 1] = 'u';
    output_fname[len - 2] = 'p';
    output_fname[len - 3] = 'e';
    output_fname[len - 4] = '.';

    //o execlp executa o comando que lhe passo em primeiro lugar com o argv que lhe passo no resto dos argumentos
    execlp(command, command, fname, setting, output_fname, setting1, (char *)0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: ./txt2epub f1.txt f2.txt ... fn.txt\n");
    return EXIT_FAILURE;
    }
    
    //char *file1 = argv[1];
    // a cada ficheiro, um filho
    for (int i = 1; i < argc; i++) {
        int pid;  // para saber se é o pai ou o filho
        
        // fork processo
        pid = fork();
        if ((pid) < 0) {
            // Se o pid for menor que 0 então dá erro
            fprintf(stderr, "Fork failed");
            exit(EXIT_FAILURE);
    	} else if (pid == 0) {
    	    // se o pid for 0 então este é o processo filho
            pandoc_converter(fname);
    	} else {
    	    // se o pid for maior que 0, então este é o processo pai
        	// esperar que o processo filho acabe
    	    printf("[pid%i] converting %s ... \n", getpid(), fname);
    	}
    }

    // esperar pelos filhos
     for (int i = 1; i < argc; i++) {
    /*
        int status = 0;
        pid_t childpid = wait(&status);*/
        wait(0);
    }
    
    system("zip ebooks *.epub");
    return (EXIT_SUCCESS);
}

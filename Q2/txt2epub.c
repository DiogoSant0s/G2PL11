#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/wait.h>

void Filename_converter(char *filename_txt) {
    char *arg1 = filename_txt;
    char *output_fname = strdup(filename_txt); // filename.epub
    int len = strlen(arg1);
    // tirar .txt e pôr .epub
    output_filename[len] = 'b';
    output_filename[len - 1] = 'u';
    output_filename[len - 2] = 'p';
    output_filename[len - 3] = 'e';
    output_filename[len - 4] = '.';

    //executa o pandoc mudando o valor do filename.txt para o filename.epub
    execlp((char *)"pandoc", (char *)"pandoc", (char *)filename_txt, (char *)"-o", (char *)output_filename, (char *)"--quiet", (char *)0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: ./txt2epub f1.txt f2.txt ... fn.txt\n");
    return EXIT_FAILURE;
    }
    
    // char *file1 = argv[1];
    // a cada ficheiro, um filho
    for (int i = 1; i < argc; i++) {
        int pid;  // para saber se é o pai ou o filho
        
        pid = fork();
        if ((pid) < 0) { 
            // fork error se pid menor que 0
            fprintf(stderr, "Fork failed");
            exit(EXIT_FAILURE);
    	} else if (pid == 0) {
    	    // processo filho
            pandoc_converter(argv[i]);
    	} else {
    	    // processo pai
        	// esperar que o processo filho acabe
    	    printf("[pid%i] converting %s ... \n", getpid(), argv[i]);
    	}
    }

    // esperar pelos filhos
     for (int i = 1; i < argc; i++) {
        wait(0);
    }
    
    system("zip ebooks *.epub");
    return (EXIT_SUCCESS);
}

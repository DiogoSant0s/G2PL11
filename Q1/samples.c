#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
  FILE* fp = fopen(argv[1], "r");
  int n = atoi(argv[2]);
  int m = atoi(argv[3]);
  srandom(0);

  if (argc != 4) {
    printf("usage: ./samples filename numberfrags maxfragsize\n");
    return EXIT_FAILURE;
  }
  
  struct stat s;
  if (stat(argv[1], &s) == -1) {
    puts("There was an error getting file info");
    return EXIT_FAILURE;
  }
  int file_size = s.st_size;
  
  
  for (int i = 0; i < n; i++) {
    fseek(fp, random() % (file_size - m), SEEK_SET);
    printf(">");
    for (int j = 0; j < m; j++) {
      int c = fgetc(fp);
      if (c == '\n') {printf("%c", ' ');}
      else {printf("%c", c);}
    }
    printf("<\n");
  }
  fclose(fp);
  
  return EXIT_SUCCESS;
}

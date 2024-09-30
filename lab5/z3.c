#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#define FIFO "potok"
#define SIZE 2

int main(int argc, char *argv[])
{
    int potok;
    char wiadomosc[SIZE];

    if(argc < 2)
    {
        fprintf(stderr, "Nie podano plikow\nUzycie: %s <nazwa_pliku>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    if (mkfifo(FIFO, 0777) == -1)
    {
        fprintf(stderr, "Blad przy tworzeniu potoku");
        exit(EXIT_FAILURE);
    }


	potok = open(FIFO, O_WRONLY);

 	 for (int i = 1; i < argc; i++) {
   	 FILE * file = fopen(argv[i], "r");

    	while (fgets(wiadomosc, SIZE, file) != NULL) {

      	write(potok, wiadomosc, SIZE);

    	}
    fclose(file);
    sleep(5);

  }
  close(potok);
  

  return 0;


}

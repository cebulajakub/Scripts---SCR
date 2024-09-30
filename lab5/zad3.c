#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ROZMIAR_BUFORA 7    
#define FIFO_path "/tmp/fifo"

int main(int argc, char *argv[])
{
    struct stat result;
    
    if (argc < 2)
    {
        printf("Podano bledna liczbe argumentow. Przy wywolaniu podaj nazwe pliku.\n");
    }

   /* Tworzenie potoku FIFO*/
    mkfifo(FIFO_path, 0666);

    int fd = open(FIFO_path, O_WRONLY);
    
    char bufor[ROZMIAR_BUFORA];
    size_t dlugosc;

    for(int i = 1; i < argc; ++i)
    {
        if (stat(argv[i], &result) == 0)
            printf("File size: %ld\n", result.st_size);
        else
            {
                printf("Blad przy pobieraniu rozmiaru pliku.\n");
                return -3;
            }

        FILE *file = fopen(argv[i], "r");
       
        if(file == NULL){
            printf("Blad przy otwieraniu pliku. \n");
            return -5;
        }

        while(fgets(bufor, ROZMIAR_BUFORA, file) != NULL)
        {
            dlugosc = strlen(bufor);

            if(bufor[dlugosc - 1] == '\n'){
                bufor[dlugosc - 1] == ' ';
            }
            write(fd, bufor, dlugosc);
        }        

        fclose(file);
        sleep(5);
    }
close(fd);
/*Usuniecie pliku FIFO*/
unlink(FIFO_path);
return 0;
}
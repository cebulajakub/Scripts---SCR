#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int N; /* liczba liczb pierwszych do znalezienia (arg.wywolania) */
int K;
int M = 2; /* nastepna liczba do sprawdzania */
int znalezione = 0; /* liczba znalezionych i zsumowanych l.pierwszych */
long suma = 0; /* to bedzie obliczona suma liczb pierwszych */
pthread_mutex_t mutex_sumowanie = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_sprawdzanie = PTHREAD_MUTEX_INITIALIZER;



bool liczba_pierwsza(int liczba)
{

  	if(liczba<2)
   	return false;

	for(int i=2;i*i<=liczba;i++){
	if(liczba%i==0)
	return false;} 
 return true;

}


void *dodawanie(void *arg)
{


    long N_ptr = (long )arg;

    while (1)
    {
        int current_sprawdz;
        pthread_mutex_lock(&mutex_sprawdzanie); 
        current_sprawdz = M;
        M++;
        pthread_mutex_unlock(&mutex_sprawdzanie); 

        if (liczba_pierwsza(current_sprawdz))
        {
            pthread_mutex_lock(&mutex_sumowanie); 
            suma += current_sprawdz;
            znalezione++;
            pthread_mutex_unlock(&mutex_sumowanie); 
        }

        if (znalezione >= N_ptr)
            break;
    }

    pthread_exit(NULL);
}


int main(int argc, char *argv[]){



    if (argc != 3)
    {
        fprintf(stderr, "Użycie: %s <N> <K>\n", argv[0]);
        exit(EXIT_FAILURE);
    }



    N = atoi(argv[1]);
    K = atoi(argv[2]);

    pthread_t tids[K];

    for (int i = 0; i < K; i++)
    {
        if (pthread_create(&tids[i], NULL, dodawanie, (void *)N) != 0)
        {
            perror("Błąd przy tworzeniu wątku");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < K; i++)
    {
        if (pthread_join(tids[i], NULL) != 0)
        {
            perror("Błąd przy oczekiwaniu na zakończenie wątku");
            exit(EXIT_FAILURE);
        }
    }

    printf("Suma liczb pierwszych: %ld\n", suma);

}

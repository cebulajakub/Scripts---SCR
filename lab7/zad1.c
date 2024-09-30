#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


pthread_barrier_t bariera;


void *wypisanie(void *id1) {
    long id = (long)id1;
    int wynik;
    pthread_barrier_wait(&bariera);

    for (int i=0; i<1000; ++i){
    wynik += log(i);}
    printf("Hello OpSys. Written by thread ID %ld\n", id);
    pthread_exit(NULL);
}


int main()
{
 pthread_t watki[5];
 pthread_barrier_init(&bariera,NULL, 5);


    for (long i = 0; i < 5; i++){
        printf("Tworzenie watku %ld\n", i);
        pthread_create(&watki[i], NULL, wypisanie, (void *)i);

    }

     for (long i = 0; i < 5; i++){
        pthread_join(watki[i], NULL);
    }



pthread_barrier_destroy(&bariera);

pthread_exit(NULL);

}

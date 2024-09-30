#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_X 8
#define MAX_Y 9
#define MAX_WATKI 8
#define MAX_SZANSE 3

int niezajete_terytorium=0;
int zajete_terytorium=0;
int terytorium[MAX_X][MAX_Y];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
int S;
int WATKI;
int X;
int Y;

int syn_id_ter;
int ilosc_syna=0;

void inicjalizacja_terytorium(int X, int Y){
	for(int i=0;i<X;i++){
	  for(int j=0;j<Y;j++){
		terytorium[i][j]=0;


  }
 }
}

void* zajmowanie_terytorium(void* id_void)
{
long id = (long)id_void;
int zajete_przez_syna=0;
    for (int i = 0; i < S; i++) {
        int random_x = rand() % X;
        int random_y = rand() % Y;

	pthread_mutex_lock(&mutex);
        if (terytorium[random_x][random_y]==0) {
            terytorium[random_x][random_y] = 1;
            zajete_terytorium++;
	    zajete_przez_syna++;
	    pthread_mutex_unlock(&mutex);

            printf("Syn %d zajal terytorium (%d, %d)\n", id, random_x, random_y);
        } else {
            printf("Syn %d nie zajal terytorium (%d, %d)\n", id, random_x, random_y);
            pthread_mutex_unlock(&mutex);

        }
        }
	pthread_mutex_lock(&mutex);
	syn_id_ter=id;
	ilosc_syna=zajete_przez_syna;
	pthread_cond_broadcast(&condition);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void* rejent_zarzadzanie(void* arg) {


    for (int i = 0; i < WATKI; i++) {
        pthread_mutex_lock(&mutex);

        pthread_cond_wait(&condition, &mutex);
	printf("Syn %d zajal terytorium %d \n", syn_id_ter, ilosc_syna);
	ilosc_syna=0;
        pthread_mutex_unlock(&mutex);
    }

    printf("\nNiezajete terytorium: %d\n", niezajete_terytorium-zajete_terytorium);

    return NULL;
}


int main() {
srand((unsigned int)time(NULL));
//int X;
//int Y;

do{
X = rand() % MAX_X + 1;
Y = rand() % MAX_Y + 1;
WATKI = rand() % MAX_WATKI + 1;
S = rand() % MAX_SZANSE + 1;
}while(S*WATKI<X*Y);
niezajete_terytorium=X*Y;
int terytorium[X][Y];
int id_syn[MAX_WATKI];
pthread_t syn[WATKI];
pthread_t rejent;


inicjalizacja_terytorium(X,Y);


printf("X= %d\n",X);
printf("Y= %d\n ",Y);
printf("Watki= %d\n ",WATKI);
printf("Terytorium= %d\n ",niezajete_terytorium);
printf("Szanse= %d\n ",S);



	pthread_create(&rejent, NULL, rejent_zarzadzanie, NULL);

	for(long i=0;i<WATKI;i++)
	{
	id_syn[i]=i+1;
	pthread_create(&syn[i],NULL,zajmowanie_terytorium,(void *)id_syn[i]);
	}

        for (int i = 0; i < WATKI; i++) {
          pthread_join(syn[i], NULL);
        }

         pthread_join(rejent, NULL);

}

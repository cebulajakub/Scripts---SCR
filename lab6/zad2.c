#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define BABBLE_NAME "kuba"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

struct babblespace {
  pthread_mutex_t babble_mutex;
  pthread_cond_t babble_cond;
  int babble_first, babble_total;
  char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

int main (){
int shm;
struct babblespace *shared_memory;
char buf[BABBLE_LENGTH];


shm=shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
if(shm == -1){
        fprintf(stderr, "Nie dziala funkcja shm_open\n");
        exit(-1);
}

shared_memory = (struct babblespace *)mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
if(shared_memory == MAP_FAILED){
        fprintf(stderr, "Nie dziala mmap\n");
        exit(-1);
}

int start = shared_memory->babble_first;
int koniec = (shared_memory->babble_first+shared_memory->babble_total);

pthread_mutex_trylock(&(shared_memory->babble_mutex));

for( int i=start; i<koniec; ++i){
        printf("%s\n", shared_memory->babbles[i%BABBLE_LIMIT]);

}

pthread_mutex_unlock(&(shared_memory->babble_mutex));


do{
 pthread_mutex_lock(&(shared_memory -> babble_mutex));
 printf("Podaj tekst do przesłania: ");
 fgets(buf, BABBLE_LENGTH, stdin);
 buf[strcspn(buf, "\n")] = '\0';
if(strncmp(buf,"1",1) != 0){
 int index = (shared_memory->babble_first + shared_memory->babble_total) % BABBLE_LIMIT;
 strncpy(shared_memory->babbles[index], buf,BABBLE_LENGTH);
 shared_memory->babble_total++;
}
pthread_mutex_unlock(&(shared_memory -> babble_mutex));
}while(strncmp(buf,"1234",1) != 0 );





 if (munmap(shared_memory, sizeof(struct babblespace)) == -1) {
        printf(stderr,"Blad munmap");
    }

    if (close(shm) == -1) {
        printf(stderr, "Blad zamkniecia");
    }

}

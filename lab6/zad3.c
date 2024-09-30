#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define BABBLE_NAME "/kk"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

struct babblespace {
  pthread_mutex_t babble_mutex;
  pthread_cond_t babble_cond;
  int babble_first, babble_total;
  char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

struct babblespace *shared_memory;
int sigint_received = 0;

void sigint_handler(int signo) {
    printf("Otrzymano sygnał SIGINT w procesie potomnym. Zamykanie...\n");
    pthread_mutex_unlock(&(shared_memory->babble_mutex));
    sigint_received = 1;
    pthread_cond_signal(&(shared_memory->babble_cond));
    exit(0);
}

int main (){



int shm;
pid_t potomek;
//struct babblespace *shared_memory;
char buf[BABBLE_LENGTH];
int wyjscie;

shm=shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
if(shm == -1){
        fprintf(stderr, "Nie dziala funkcja shm_open\n");
        exit(-1);
}

shared_memory = (struct babblespace *)mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, shm,0);
if(shared_memory == MAP_FAILED){
        fprintf(stderr, "Nie dziala mmap\n");
        exit(-1);
}


potomek=fork();


if (potomek<0)
{
        fprintf(stderr, "Blad potomka\n");
        exit(-1);
}

if (potomek>0)
{

 do{
  printf("Podaj tekst do przesłania: ");
  fgets(buf, BABBLE_LENGTH, stdin);
  pthread_mutex_lock(&(shared_memory -> babble_mutex));
  buf[strcspn(buf, "\n")] = '\0';
 if(strncmp(buf,"1",1) != 0){
  int index = (shared_memory->babble_first + shared_memory->babble_total) % BABBLE_LIMIT;
  strncpy(shared_memory->babbles[index], buf,BABBLE_LENGTH);
  shared_memory->babble_total++;
}
pthread_mutex_unlock(&(shared_memory -> babble_mutex));
pthread_cond_broadcast(&(shared_memory-> babble_cond));

}while(strncmp(buf,"1234",1) !=0);

kill(potomek,SIGINT);
}

if (potomek==0)
{

 signal(SIGINT, sigint_handler);
 pthread_mutex_lock(&(shared_memory->babble_mutex));

 while(1)
 {
   if (sigint_received) {
    pthread_mutex_unlock(&(shared_memory->babble_mutex));
    break;
 }


 pthread_cond_wait(&(shared_memory->babble_cond), &(shared_memory->babble_mutex));

 int start = shared_memory->babble_first;
 int koniec = (shared_memory->babble_first+shared_memory->babble_total);


 for( int i=start; i<koniec; ++i){
     printf("%d. %s\n",(i%BABBLE_LIMIT)+1,shared_memory->babbles[i%BABBLE_LIMIT]);
 }

}
//shared_memory->babble_total = 0;

pthread_mutex_unlock(&(shared_memory->babble_mutex));

}


 if (munmap(shared_memory, sizeof(struct babblespace)) == -1) {
        printf(stderr,"Blad munmap");
    }

    if (close(shm) == -1) {
        printf(stderr, "Blad zamkniecia");
    }

return 0;

}

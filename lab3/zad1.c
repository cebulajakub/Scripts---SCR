#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>



void obsluga(int signum) {
    if (signum == SIGALRM) {
        signal(SIGALRM, SIG_IGN);
    } else if (signum == SIGTERM) {
        printf("Przechwycono sygnał SIGTERM. Zamykanie programu.\n");
        exit(1);
    } else if (signum == SIGUSR1) {
        printf("Przechwycono sygnał SIGUSR1. Kontynuuję pracę.\n");
    } else if (signum == SIGUSR2) {
	printf("Sygnal dotarl.\n");
    }
}


int main() {
    int i = 0;


struct sigaction sygnal;
sygnal.sa_handler=obsluga;
sigemptyset(&sygnal.sa_mask); //pusta maska bez sygnału
sygnal.sa_flags=0; //brak flag
sigaddset(&sygnal.sa_mask, SIGUSR2);
sigprocmask(SIG_BLOCK, &sygnal.sa_mask, NULL);


sigaction(SIGALRM, &sygnal,NULL);
sigaction(SIGTERM, &sygnal,NULL);
sigaction(SIGUSR1, &sygnal,NULL);
sigaction(SIGUSR2, &sygnal,NULL);




    while(1) {
        i++;

	if(i%1000==0){
	printf("Wznowienie odnierania sygnalu USR2.\n");
	sigprocmask(SIG_UNBLOCK, &sygnal.sa_mask, NULL);
	}
	else
	{
	printf("Wstrzymywanie odnierania sygnalu USR2.\n")
	sigprocmask(SIG_BLOCK, &sygnal.sa_mask, NULL);

	}
        usleep(10000);
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>

#define BABBLE_NAME "/kuba"
#define BABBLE_MODE 0666
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

struct babblespace {
    pthread_mutex_t babble_mutex;
    pthread_cond_t babble_cond;
    int babble_first, babble_total;
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

int main() {
    shm_unlink(BABBLE_NAME);
    umask(0);

    int babble_fd = shm_open(BABBLE_NAME, O_RDWR | O_CREAT, BABBLE_MODE);
    ftruncate(babble_fd, sizeof(struct babblespace));

    struct babblespace *babble_ptr = (struct babblespace *)mmap(NULL, sizeof(struct babblespace),PROT_READ|PROT_WRITE, MAP_SHARED,babble_fd, 0);

    // Inicjalizacja mutexu
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&babble_ptr->babble_mutex, &attr);

    // Inicjalizacja zmiennej warunkowej
    pthread_condattr_t attr2;
    pthread_condattr_init(&attr2);
    pthread_condattr_setpshared(&attr2, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&babble_ptr->babble_cond, &attr2);

    /*// Inicjalizacja pozostałych pól struktury
    babble_ptr->babble_first = 0;
    babble_ptr->babble_total = 0; // Tutaj możesz wstępnie zainicjować inne pola struktury, jeśli to konieczne

    // Przykładowa inicjalizacja tablicy babbles
    for (int i = 0; i < BABBLE_LIMIT; ++i) {
        snprintf(babble_ptr->babbles[i], BABBLE_LENGTH, "Default Message");
    }*/

    // Zwalnianie zasobów
   // munmap((char *)babble_ptr, sizeof(struct babblespace));

    return 0;
}

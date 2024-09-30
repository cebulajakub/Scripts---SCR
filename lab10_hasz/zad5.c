#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include<unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_naprawiacz = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t sygnal_mutex = PTHREAD_COND_INITIALIZER;
pthread_cond_t sygnal_mutex_naprawiacz = PTHREAD_COND_INITIALIZER;



struct Baza {
    char mail[50];
    char hash[33];

    int watek;
    int haslo;

    char zlamane_haslo[33];
    char hasz_zlamane_haslo[33];
};
int liczba_linii = 0;
int dlugosc;
char **slownik, **slownik_hash;
struct Baza dane[1000];
int uzytkownik = 0;
int uzytkownik_konsument=0;





void bytes2md5(const char *data, int len, char *md5buf) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, len);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);
    for (i = 0; i < md_len; i++) {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
    }
}

void wczytanie_slownika(char ***slownik1, int *ilosc_wierszy, char *nazwa) {
    FILE *plik;
    int linie = 1;
    char znak;
    plik = fopen(nazwa, "r");

    if (plik == NULL) {
        printf("Blad otwarcia slownika.\n");
        exit(1);
    }
    while ((znak = fgetc(plik)) != EOF) {
        if (znak == '\n' || znak=='\0') {
            linie++;
        }
    }
    rewind(plik);
    printf("liczba linii %d\n", linie);

    *slownik1 = (char **)malloc(linie * sizeof(char *));
    for (int i = 0; i < linie; i++)
        (*slownik1)[i] = (char *)malloc((33) * sizeof(char));

    for (int i = 0; i < linie; i++) {
        fgets((*slownik1)[i], 32, plik);
        size_t len = strlen((*slownik1)[i]);
        if (len > 0 && (*slownik1)[i][len - 1] == '\n') {
            (*slownik1)[i][len - 1] = '\0';
        }
    }
    *ilosc_wierszy = linie;
    fclose(plik);
}


void wyswietlanie_sygnal(){
    for(int i=0;i<uzytkownik;i++)
    {
        if(dane[i].watek!=-1)
        printf("Password for %s is %s\n", dane[i].mail ,dane[i].zlamane_haslo);
    }
}


void wczytaj_baze(char *name) {
    FILE *plik2;
    char dlugosc_linii[200];


    plik2 = fopen(name, "r");
    if (plik2 == NULL) {
        printf("Blad otwarcia .\n");
        exit(1);
    }

    while (fgets(dlugosc_linii, sizeof(dlugosc_linii), plik2)) {
        char *id = strtok(dlugosc_linii, " \t");
        char *haslo = strtok(NULL, " \t");
        char *mail = strtok(NULL, " \t");
        char *nazwa = strtok(NULL, " \t");

        if (haslo != NULL && mail != NULL) {
            strncpy(dane[uzytkownik].hash, haslo, 32);
            strncpy(dane[uzytkownik].mail, mail, 50);
            dane[uzytkownik].watek=-1;
            uzytkownik++;
        }

        if (uzytkownik == 1000) {
            printf("Tablica zapełniona");
            break;
        }
    }
    fclose(plik2);
}

void hashowanie_hasel(char **tablica_slownik, int linie, char **tablica_hash) {
    for (int i = 0; i < linie; i++) {
        bytes2md5(tablica_slownik[i], strlen(tablica_slownik[i]), tablica_hash[i]);
        //printf("%s ======================> %s\n", tablica_slownik[i], tablica_hash[i]);
    }
}

void zalokuj_tablice(int linie, char ***hashowana) {
    *hashowana = (char **)malloc(linie * sizeof(char *));
    for (int i = 0; i < linie; i++)
        (*hashowana)[i] = (char *)malloc((33) * sizeof(char));
}

void zwolnij_tablice(char **tablica, int linie) {
    for (int i = 0; i < linie; i++) {
        free(tablica[i]);
    }
    free(tablica);
}

void porownaj( char **tablica_hash, int liczba_hasel, char **dobre_hasla, int ktory_watek) {

    for (int i = 0; i < uzytkownik; i++){
        for (int j = 0; j < liczba_hasel; j++) {
            if (strcmp(dane[i].hash, tablica_hash[j]) == 0) {
                pthread_mutex_lock(&mutex);
                dane[i].watek=ktory_watek;
                uzytkownik_konsument=i;
                //printf("Password for %s is %s by %d thread\n",dane[i].mail ,dobre_hasla[j], dane[i].watek);
                strcpy(dane[i].zlamane_haslo,dobre_hasla[j]);
                strcpy(dane[i].hasz_zlamane_haslo,dane[i].hash);
                pthread_cond_signal(&sygnal_mutex);
                pthread_mutex_unlock(&mutex);

            }
        }
    }
}



void *sam_slownik(void *id_watek1){

int w=1;
char **slownik_sameslowa_hash;

    zalokuj_tablice(liczba_linii,&slownik_sameslowa_hash);
    hashowanie_hasel(slownik, liczba_linii, slownik_sameslowa_hash);
    porownaj(slownik_sameslowa_hash, liczba_linii,slownik,w);
    zwolnij_tablice(slownik_sameslowa_hash,liczba_linii);

    pthread_exit(NULL);

}

void *slownik_liczby_z_przodu(void *id_watek2) {

    int w=2;
    char **slownik_liczby_przod_hash;
    char **slownik_liczby_przod;
    int rozmiar_hasz = liczba_linii * 100;  //tylko liczby z przodu

    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_przod_hash);
    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_przod);

    int indeks = 0;

    for (int i = 0; i < liczba_linii; i++) {

        for (int j = 0; j < 100; j++) {
            strcpy(slownik_liczby_przod[indeks], slownik[i]);
            sprintf(slownik_liczby_przod[indeks], "%d%s", j, slownik[i]);
            indeks++;
        }
    }

    hashowanie_hasel(slownik_liczby_przod, rozmiar_hasz, slownik_liczby_przod_hash);
    porownaj(slownik_liczby_przod_hash, rozmiar_hasz, slownik_liczby_przod,w);
    zwolnij_tablice(slownik_liczby_przod_hash, rozmiar_hasz);
    zwolnij_tablice(slownik_liczby_przod, rozmiar_hasz);
    pthread_exit(NULL);
}

void *slownik_liczby_z_tylu(void *id_watek3) {

    int w=3;
    char **slownik_liczby_tyl_hash;
    char **slownik_liczby_tyl;
    int rozmiar_hasz = liczba_linii * 100;  //tylko liczby z tylu

    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_tyl_hash);
    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_tyl);

    int indeks = 0;

    for (int i = 0; i < liczba_linii; i++) {

        for (int j = 0; j < 100; j++) {
            strcpy(slownik_liczby_tyl[indeks], slownik[i]);
            sprintf(slownik_liczby_tyl[indeks], "%s%d",slownik[i],j);
            indeks++;
        }
    }

    hashowanie_hasel(slownik_liczby_tyl, rozmiar_hasz, slownik_liczby_tyl_hash);
    porownaj(slownik_liczby_tyl_hash, rozmiar_hasz, slownik_liczby_tyl,w);
    zwolnij_tablice(slownik_liczby_tyl_hash, rozmiar_hasz);
    zwolnij_tablice(slownik_liczby_tyl, rozmiar_hasz);
    pthread_exit(NULL);
}

void *slownik_liczby_przod_tyl(void *id_watek4) {

    int w=4;
    char **slownik_liczby_hash;
    char **slownik_liczby;
    int rozmiar_hasz = liczba_linii * 10000;  //tylko liczby z tylu

    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_hash);
    zalokuj_tablice(rozmiar_hasz, &slownik_liczby);

    int indeks = 0;

    for (int i = 0; i < liczba_linii; i++) {
        for (int j = 0; j < 100; j++) {
            for (int k = 0; k < 100; k++) {
                strcpy(slownik_liczby[indeks], slownik[i]);
                sprintf(slownik_liczby[indeks], "%d%s%d", j, slownik[i], k);
                indeks++;
            }
        }
    }

    hashowanie_hasel(slownik_liczby, rozmiar_hasz, slownik_liczby_hash);
    porownaj(slownik_liczby_hash, rozmiar_hasz, slownik_liczby,w);
    zwolnij_tablice(slownik_liczby_hash, rozmiar_hasz);
    zwolnij_tablice(slownik_liczby, rozmiar_hasz);
    pthread_exit(NULL);
}

void *liczby_z_przodu_slowa(void *id_watek5) {

    int w=5;
    char **slownik_liczby_przodslowa_hash;
    char **slownik_liczby_przodslowa;
    int rozmiar_hasz = liczba_linii * 100 * 2;

    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_przodslowa_hash);
    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_przodslowa);

    int indeks = 0;

    for (int i = 0; i < liczba_linii; i++) {

        for (int j = 0; j < 100; j++) {
            strcpy(slownik_liczby_przodslowa[indeks], slownik[i]);
            sprintf(slownik_liczby_przodslowa[indeks], "%d%s", j, slownik[i]);
            if (slownik_liczby_przodslowa[indeks][1] >= 48 && slownik_liczby_przodslowa[indeks][1] <= 57) {
                slownik_liczby_przodslowa[indeks][2] = toupper((unsigned char) slownik_liczby_przodslowa[indeks][2]);
            } else if (slownik_liczby_przodslowa[indeks][0] >= 48 && slownik_liczby_przodslowa[indeks][0] <= 57) {
                slownik_liczby_przodslowa[indeks][1] = toupper((unsigned char) slownik_liczby_przodslowa[indeks][1]);
            } else {
                slownik_liczby_przodslowa[indeks][0] = toupper((unsigned char) slownik_liczby_przodslowa[indeks][0]);
            }

            indeks++;
        }

    }

    for (int i = 0; i < liczba_linii*100; i++) {
        strcpy(slownik_liczby_przodslowa[indeks], slownik_liczby_przodslowa[i]);
        for (int j = 0; slownik_liczby_przodslowa[indeks][j] != '\0'; j++) {
            slownik_liczby_przodslowa[indeks][j] = toupper((unsigned char) slownik_liczby_przodslowa[indeks][j]);
        }
        indeks++;
    }
    hashowanie_hasel(slownik_liczby_przodslowa, rozmiar_hasz, slownik_liczby_przodslowa_hash);
    porownaj(slownik_liczby_przodslowa_hash, rozmiar_hasz,slownik_liczby_przodslowa,w);
    zwolnij_tablice(slownik_liczby_przodslowa_hash,rozmiar_hasz);
    zwolnij_tablice(slownik_liczby_przodslowa,rozmiar_hasz);
    pthread_exit(NULL);

}

void *liczby_z_tylu_slowa(void *id_watek6) {

    int w=6;
    char **slownik_liczby_tylslowa_hash;
    char **slownik_liczby_tylslowa;
    int rozmiar_hasz = liczba_linii * 100 * 2;

    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_tylslowa_hash);
    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_tylslowa);

    int indeks = 0;

    for (int i = 0; i < liczba_linii; i++) {

        for (int j = 0; j < 100; j++) {
            strcpy(slownik_liczby_tylslowa[indeks], slownik[i]);
            sprintf(slownik_liczby_tylslowa[indeks], "%s%d", slownik[i],j);
            if (slownik_liczby_tylslowa[indeks][1] >= 48 && slownik_liczby_tylslowa[indeks][1] <= 57) {
                slownik_liczby_tylslowa[indeks][2] = toupper((unsigned char) slownik_liczby_tylslowa[indeks][2]);
            } else if (slownik_liczby_tylslowa[indeks][0] >= 48 && slownik_liczby_tylslowa[indeks][0] <= 57) {
                slownik_liczby_tylslowa[indeks][1] = toupper((unsigned char) slownik_liczby_tylslowa[indeks][1]);
            } else {
                slownik_liczby_tylslowa[indeks][0] = toupper((unsigned char) slownik_liczby_tylslowa[indeks][0]);
            }

            indeks++;
        }

    }

    for (int i = 0; i < liczba_linii*100; i++) {
        strcpy(slownik_liczby_tylslowa[indeks], slownik_liczby_tylslowa[i]);
        for (int j = 0; slownik_liczby_tylslowa[indeks][j] != '\0'; j++) {
            slownik_liczby_tylslowa[indeks][j] = toupper((unsigned char) slownik_liczby_tylslowa[indeks][j]);
        }
        indeks++;
    }
    hashowanie_hasel(slownik_liczby_tylslowa, rozmiar_hasz, slownik_liczby_tylslowa_hash);
    porownaj(slownik_liczby_tylslowa_hash, rozmiar_hasz,slownik_liczby_tylslowa,w);
    zwolnij_tablice(slownik_liczby_tylslowa_hash,rozmiar_hasz);
    zwolnij_tablice(slownik_liczby_tylslowa,rozmiar_hasz);
    pthread_exit(NULL);

}

void *liczby_slowa(void *watek7){

    int w=7;
    char **slownik_liczby_slowa_hash;
    char **slownik_liczby_slowa;
    int rozmiar_hasz = liczba_linii * 10000 * 2;

    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_slowa_hash);
    zalokuj_tablice(rozmiar_hasz, &slownik_liczby_slowa);

    int indeks = 0;


    for (int i = 0; i < liczba_linii; i++) {
        for (int j = 0; j < 100; j++) {
            for (int k = 0; k < 100; k++) {
                strcpy(slownik_liczby_slowa[indeks], slownik[i]);
                sprintf(slownik_liczby_slowa[indeks], "%d%s%d", j, slownik[i], k);
                if (slownik_liczby_slowa[indeks][1] >= 48 && slownik_liczby_slowa[indeks][1] <= 57) {
                    slownik_liczby_slowa[indeks][2] = toupper((unsigned char) slownik_liczby_slowa[indeks][2]);
                } else if (slownik_liczby_slowa[indeks][0] >= 48 && slownik_liczby_slowa[indeks][0] <= 57) {
                    slownik_liczby_slowa[indeks][1] = toupper((unsigned char) slownik_liczby_slowa[indeks][1]);
                } else {
                    slownik_liczby_slowa[indeks][0] = toupper((unsigned char) slownik_liczby_slowa[indeks][0]);
                }
                indeks++;
            }

        }
    }


    for (int i = 0; i < liczba_linii*10000; i++) {
        strcpy(slownik_liczby_slowa[indeks], slownik_liczby_slowa[i]);
        for (int j = 0; slownik_liczby_slowa[indeks][j] != '\0'; j++) {
            slownik_liczby_slowa[indeks][j] = toupper((unsigned char) slownik_liczby_slowa[indeks][j]);
        }
        indeks++;
    }

    hashowanie_hasel(slownik_liczby_slowa, rozmiar_hasz, slownik_liczby_slowa_hash);
    porownaj(slownik_liczby_slowa_hash, rozmiar_hasz,slownik_liczby_slowa,w);
    zwolnij_tablice(slownik_liczby_slowa_hash,rozmiar_hasz);
    zwolnij_tablice(slownik_liczby_slowa,rozmiar_hasz);
    pthread_exit(NULL);
}


void *slowa(void * id_watek8) {

    int w=8;
    char **slownik_slowa_hash;
    char **slownik_slowa;
    int rozmiar_hasz = liczba_linii * 2;

    zalokuj_tablice(rozmiar_hasz, &slownik_slowa_hash);
    zalokuj_tablice(rozmiar_hasz, &slownik_slowa);

    int indeks=0;

    for (int i = 0; i < liczba_linii; i++) {

        strcpy(slownik_slowa[indeks], slownik[i]);
        slownik_slowa[indeks][0] = toupper((unsigned char) slownik_slowa[indeks][0]);
        indeks++;
    }

    for (int i = 0; i < liczba_linii; i++) {
        strcpy(slownik_slowa[indeks], slownik_slowa[i]);
        for (int j = 0; slownik_slowa[indeks][j] != '\0'; j++) {
            slownik_slowa[indeks][j] = toupper((unsigned char) slownik_slowa[indeks][j]);
        }
        indeks++;
    }

    hashowanie_hasel(slownik_slowa, rozmiar_hasz, slownik_slowa_hash);
    porownaj(slownik_slowa_hash, rozmiar_hasz,slownik_slowa,w);
    zwolnij_tablice(slownik_slowa_hash,rozmiar_hasz);
    zwolnij_tablice(slownik_slowa,rozmiar_hasz);
    pthread_exit(NULL);

}

void *konsument(void *id_watek9){

    while(1){
        pthread_mutex_lock(&mutex);

        pthread_cond_wait(&sygnal_mutex, &mutex);
        printf("Password %s is %s by %d thread\n",dane[uzytkownik_konsument].mail ,dane[uzytkownik_konsument].zlamane_haslo, dane[uzytkownik_konsument].watek);
        pthread_mutex_unlock(&mutex);
    }
}

void obsluga(int signal){
    if(signal == SIGHUP){
        printf("Otrzymano sygnal SIGHUP\n");
        wyswietlanie_sygnal();
    }
}

void *niszczyciel(void *id_watek10){
    printf("uzytkownik: %d\n", uzytkownik);
    int i=0;
  while(i<3)
  {
  if(uzytkownik>0){
    pthread_mutex_lock(&mutex_naprawiacz);
    int random_index= rand() % uzytkownik;
    printf("Przed podmiana: %s\n", dane[random_index].hasz_zlamane_haslo);
    strcpy(dane[random_index].hasz_zlamane_haslo, "9abcd23jsasdfsadf124234kuyfdakji");
    printf("Podmieniony hasz: %s\n", dane[random_index].hasz_zlamane_haslo);
    i++;
    pthread_cond_signal(&sygnal_mutex_naprawiacz);
    pthread_mutex_unlock(&mutex_naprawiacz);
  }
  }

    pthread_exit(NULL);

}

void *naprawiacz(void *id_watek11){
    int i=0;
  while(i<3)
  {
  if(uzytkownik>0){
    pthread_mutex_lock(&mutex_naprawiacz);
    pthread_cond_wait(&sygnal_mutex, &mutex_naprawiacz);
    for(int i=0;i<uzytkownik;i++){
         if (strcmp(dane[i].hash, dane[i].hasz_zlamane_haslo) != 0){
            printf("Zamaiana Hasla\n");
            strcpy(dane[i].hasz_zlamane_haslo,dane[i].hash );
            printf("Naprawione haslo: %s\n", dane[i].hasz_zlamane_haslo);
         }
         else{
            printf("Haslo niezmienione.\n");
            printf("Stare Haslo: %s\n", dane[i].hasz_zlamane_haslo);
         }

    }
    pthread_mutex_unlock(&mutex_naprawiacz);
  }
}
}

int main(int argc, char *argv[]) {

signal(SIGHUP, obsluga);


    pthread_t watek1;
    pthread_t watek2;
    pthread_t watek3;
    pthread_t watek4;
    pthread_t watek5;
    pthread_t watek6;
    pthread_t watek7;
    pthread_t watek8;
    pthread_t watek9;
    pthread_t watek10;
    pthread_t watek11;

    wczytanie_slownika(&slownik, &liczba_linii, argv[1]);
    printf("liczba po wczytaniu: %d\n", liczba_linii);
    wczytaj_baze(argv[2]);
    printf("liczba linii do hash: %d\n", liczba_linii);

    pthread_create(&watek9,NULL,konsument,NULL);
    pthread_create(&watek1,NULL,sam_slownik,NULL);
    // pthread_create(&watek2,NULL,slownik_liczby_z_przodu,NULL);
    // pthread_create(&watek3,NULL,slownik_liczby_z_tylu,NULL);
    // pthread_create(&watek4,NULL,slownik_liczby_przod_tyl,NULL);
    // pthread_create(&watek5,NULL,liczby_z_przodu_slowa,NULL);
    // pthread_create(&watek6,NULL,liczby_z_tylu_slowa,NULL);
    // pthread_create(&watek7,NULL,liczby_slowa,NULL);
    // pthread_create(&watek8,NULL,slowa,NULL);
    pthread_create(&watek10,NULL,niszczyciel,NULL);
    pthread_create(&watek11,NULL,naprawiacz,NULL);
    pthread_join(watek1, NULL);
    // pthread_join(watek2, NULL);
    // pthread_join(watek3, NULL);
    // pthread_join(watek4, NULL);
    // pthread_join(watek5, NULL);
    // pthread_join(watek6, NULL);
    // pthread_join(watek7, NULL);
    // pthread_join(watek8,NULL);
    pthread_join(watek9,NULL);
    pthread_join(watek10,NULL);
    pthread_join(watek11,NULL);



    return 0;
}


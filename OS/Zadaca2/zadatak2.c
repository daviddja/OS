#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
// Zastavica nam govori moze li proces uci u kriticni dio 
// pravo pokazuje tko ima prednost ako zele uci istovremeno
// Pokazivaci i ID-ovi za zajednicku memoriju
// medjusobno iskljucivanje izmedju dva procesa
int *PRAVO, *ZASTAVICA, pravoId, zastavicaId;

// Funkcija za ulazak u kriticni odsjek koristeci Petersonov algoritam
void udi_u_kriticni_odsjecak(int i, int j){
    ZASTAVICA[i] = 1; // Proces i zeli uci
    while (ZASTAVICA[j] == 1) { // Dok proces j zeli uci
        if (*PRAVO == j) { // Ako je pravo na strani procesa j
            ZASTAVICA[i] = 0; // Proces i odustaje privremeno
            while (*PRAVO == j) {
                // Ceka dok se pravo ne promijeni
            }
            ZASTAVICA[i] = 1; // Ponovno postavlja svoju zastavicu
        }
    }
}

// Funkcija za izlazak iz kriticnog odsjeka
void izadji_iz_kriticnog_odsjecka(int i, int j){
    ZASTAVICA[i] = 0; // Proces i vise ne zeli uci
    *PRAVO = j;       // Daje pravo procesu j
}

// Funkcija koju izvrsava svaki proces (simulacija rada)
void proc(int i, int j){
    int k, m;
    for(k = 1; k < 5; k++){
        udi_u_kriticni_odsjecak(i, j); // Ulazak u kriticni odsjek

        for(m = 1; m < 5; m++){
            // Kriticna sekcija — ispisuje trenutne vrijednosti
            printf("i: %d, k: %d, m: %d\n", i, k, m);
            sleep(1); // Simulacija rada
        }

        izadji_iz_kriticnog_odsjecka(i, j); // Izlazak iz kriticnog odsjeka
    }
}

// Funkcija koja se poziva kada se primi SIGINT (Ctrl+C) — brise memoriju
void brisi(){
    shmdt(PRAVO);       // Odspaja se od zajednicke memorije PRAVO
    shmdt(ZASTAVICA);   // Odspaja se od zajednicke memorije ZASTAVICA

    shmctl(pravoId, IPC_RMID, NULL);       // Brise segment PRAVO
    shmctl(zastavicaId, IPC_RMID, NULL);   // Brise segment ZASTAVICA

    exit(0); // Gasi program
}

void main(){
    // Alokacija zajednicke memorije
    pravoId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0660);
    zastavicaId = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0660);

    PRAVO = (int*)shmat(pravoId, NULL, 0);            // Spajanje na PRAVO
    ZASTAVICA = (int*)shmat(zastavicaId, NULL, 0);    // Spajanje na ZASTAVICA

    // Pokretanje dva procesa
    if (fork() == 0) {
        proc(1, 0); // Prvi proces (proces 1)
    }
    if (fork() == 0) {
        proc(0, 1); // Drugi proces (proces 0)
    }

    signal(SIGINT, brisi); // Signal handler za prekid (Ctrl+C)

    wait(NULL); // Ceka prvi child proces
    wait(NULL); // Ceka drugi child proces

    brisi(); // Nakon sto su oba procesa gotova — brise memoriju
}


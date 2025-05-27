#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// Globalne varijable
int *ULAZ, *BROJ, brSt, brDr, *REZ;

// Funkcija koja pronalazi maksimalni broj u nizu BROJ
int max() {
    int najv = BROJ[0];
    for (int i = 0; i < brSt; i++) {
        if (BROJ[i] > najv) {
            najv = BROJ[i];
        }
    }
    return najv;
}

// Funkcija koja provjerava jesu li svi stolovi zauzeti
int sve_zauzeto() {
    for (int i = 0; i < brSt; i++) {
        if (REZ[i] == -1) {
            return 0; // Ima barem jedan slobodan
        }
    }
    return 1; // Svi su zauzeti
}

// Funkcija za ulazak dretve u kriticni odsjek (Lamportov algoritam)
void udji_u_kriticni_odsjecak(int i) {
    ULAZ[i] = 1; // Oznaci da dretva ulazi
    BROJ[i] = max() + 1; // Dodijeli broj veci od svih postojecih
    ULAZ[i] = 0;

    for (int j = 0; j < brSt - 1; j++) {
        // Cekaj dok druga dretva odreduje svoj broj
        while (ULAZ[j] != 0) {}
        // Cekaj dok druga dretva ima prednost
        while (BROJ[j] != 0 && (BROJ[j] < BROJ[i] || (BROJ[j] == BROJ[i] && j < i))) {}
    }
}

// Funkcija za izlazak iz kriticnog odsjeka
void izadji_iz_kriticnog_odsjecka(int i) {
    BROJ[i] = 0; // Ponisti svoj broj
}

// Funkcija dretve — pokusava rezervirati nasumican stol
void *provjeri_stol(void *pdr) {
    if (sve_zauzeto()) {
        pthread_exit("svi su stolovi zauzeti");
    }

    int dr = *((int *)pdr); // Broj dretve
    int rand_tbl = rand() % brSt; // Nasumican broj stola

    printf("Dretva %d: pokusavam rez stol %d\n", dr + 1, rand_tbl + 1);

    udji_u_kriticni_odsjecak(rand_tbl); // Ulazak u kriticni odsjek

    char stanje[brSt];
    for (int i = 0; i < brSt; i++) {
        stanje[i] = (REZ[i] == -1) ? '-' : REZ[i] + 1 + '0';
    }
    stanje[brSt] = '\0';

    if (REZ[rand_tbl] == -1) {
        // Ako je stol slobodan — rezerviraj ga
        REZ[rand_tbl] = dr;
        for (int i = 0; i < brSt; i++) {
            stanje[i] = (REZ[i] == -1) ? '-' : REZ[i] + 1 + '0';
        }
        stanje[brSt] = '\0';
        printf("Dretva %d: rezerviram stol %d, stanje: %s\n", dr + 1, rand_tbl + 1, stanje);
    } else {
        // Stol je vec zauzet
        printf("Dretva %d: neuspjela rezervacija stola %d, stanje: %s\n", dr + 1, rand_tbl + 1, stanje);
    }

    izadji_iz_kriticnog_odsjecka(rand_tbl); // Izlazak iz kriticnog odsjeka
    pthread_exit(NULL); // Zavrsi dretvu
}

void main() {
    printf("br dretvi: ");
    scanf("%d", &brDr); // Unesi broj dretvi (korisnika)
    printf("br st: ");
    scanf("%d", &brSt); // Unesi broj stolova

    pthread_t thread_id;

    // Alociraj memoriju za sve potrebne nizove
    REZ = (int*)malloc(sizeof(int) * brSt);     // Rezervacije stolova
    BROJ = (int*)malloc(sizeof(int) * brSt);    // Brojevi za Lamportov algoritam
    ULAZ = (int*)malloc(sizeof(int) * brSt);    // Ulazak dretvi

    // Inicijalizacija svih nizova
    for (int i = 0; i < brSt; i++) {
        REZ[i] = -1; // -1 oznacava da je stol slobodan
        BROJ[i] = 0;
        ULAZ[i] = 0;
    }

    // Dok nisu svi stolovi zauzeti — stvaraj dretve
    while (!sve_zauzeto()) {
        for (int i = 0; i < brDr; i++) {
            int *pdr = (int *)malloc(sizeof(int));
            *pdr = i; // Proslijedi broj dretve
            pthread_create(&thread_id, NULL, provjeri_stol, pdr);
        }
    }

    // Cekaj zavrsetak svih dretvi
    for (int i = 0; i < brDr; i++) {
        pthread_join(thread_id, NULL);
    }

    // Oslobodi alociranu memoriju
    free(REZ);
    free(BROJ);
    free(ULAZ);
}


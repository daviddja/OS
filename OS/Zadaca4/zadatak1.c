#include <stdio.h>      
#include <semaphore.h>  
#include <sys/types.h>  
#include <unistd.h>     
#include <stdlib.h>     
#include <sys/wait.h>   
#include <sys/stat.h>   
#include <sys/shm.h>    
#include <fcntl.h>     

#define N 5             // Broj sjedalica na vrtuljku
#define M 15            // Broj posjetitelja
#define SEMNAME_MJESTA "mjesta"  // Ime semafora

// Funkcija koju pozivaju posjetitelji
void posjetitelj(int i){
	// Otvaranje postoje�eg semafora
	sem_t* mjesta = sem_open(SEMNAME_MJESTA, 0);

	while(1){
		// Posjetitelj ceka slobodno mjesto na vrtuljku
		sem_wait(mjesta);
		printf("proces %d je usao u vrtuljak\n", i);

		// Simulira voznju vrtuljkom
		sleep(3);
	}
}

// Funkcija koju pokre�e proces vrtuljka
void vrtuljak(){
	int br = 0;  // Broj voznji

	// Otvaranje postojeceg semafora
	sem_t* mjesta = sem_open(SEMNAME_MJESTA, 0);
	int *value = malloc(sizeof(int));  // Za dohvat vrijednosti semafora

	while(1){
		// Cekanje dok sva mjesta ne budu popunjena
		do {
			sem_getvalue(mjesta, value);  // Dohvat trenutnog broja slobodnih mjesta
			printf("broj slobodnih mjesta: %d\n", *value);
			sleep(1);  // Pauza izmedju provjera
		} while(*value != 0);  // Dok ima slobodnih mjesta, vrtuljak ne krece

		sleep(1);
		printf("vrtuljak se vrti %d\n", br++);
		sleep(3);  // Simulacija trajanja voznje
		printf("Vrtuljak se zaustavio\n");

		// Otpustanje N mjesta (nakon sto svi posjetitelji sidju)
		for(int i = 0; i < N; i++){
			sem_post(mjesta);  // Oslobadjanje jednog mjesta
			printf("Sjedalo br %d na vrtuljku se ispraznilo!\n", i+1);
		}
	}
}

// Glavna funkcija
void main(){
	// Kreiranje semafora s pocetnom vrijednosti N (sva mjesta slobodna)
	sem_t* mjesta = sem_open(SEMNAME_MJESTA, O_CREAT, 0644, N);

	// Dodatno otpustanje N mjesta (ovo je visak jer je vec postavljeno na N)
	for(int i = 0; i < N; i++){
		sem_post(mjesta);
	}

	// Kreiranje procesa za vrtuljak
	if(fork() == 0){
		vrtuljak();  // Dijete postaje vrtuljak
	}

	sleep(1);  // Pauza prije pokretanja posjetitelja

	// Kreiranje M procesa posjetitelja
	for(int i = 0; i < M; i++){
		if(fork() == 0){
			posjetitelj(i);  // Svaki posjetitelj ima svoj proces
		}
	}

	// Cekanje svih M + 1 child procesa (1 vrtuljak + M posjetitelja)
	for(int i = 0; i < M + 1; i++){
		wait(NULL);  // Cekaj da dijete zavr�i
	}

	// Unistavanje semafora (nece se dostici jer su procesi u beskonacnoj petlji)
	sem_destroy(mjesta);
}


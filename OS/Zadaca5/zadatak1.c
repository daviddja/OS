#include<stdio.h>       
#include<stdlib.h>      
#include<semaphore.h>    
#include<pthread.h>      
#include<unistd.h>       
#define N 5              // Broj filozofa

// Identifikatori filozofa (0 do 4)
int fil_br[5] = {0,1,2,3,4}; 

sem_t sem[N];            // N semafora, po jedan za svakog filozofa
sem_t mutex;             // Globalni mutex semafor za zaštitu kritiène sekcije
int state[N];            // Niz koji cuva stanje svakog filozofa (1 = gladan, 2 = jede, 3 = misli)

// Proverava da li filozof moze da uzme vilice
void test(int br){
    // Ako je filozof gladan i susjedi ne jedu, može da jede
	if(state[br] == 1 && state[(br+4)%N] != 2 && state[(br+1)%N] != 2 ){
		state[br] = 2; // Prelazi u stanje "jede"
		printf("filozof %d uzima vilice i jede\n", br +1);
		fflush(stdout);  // Isprazni izlazni bafer (stdout, ne stdin)
		sleep(1);       // Simulira jedenje
		sem_post(&sem[br]); // Signalizuje da filozof može da nastavi
	}
}

// Filozof pokusava da uzme vilice (postaje gladan)
void uzima_vilicu(int br){
	sem_wait(&mutex);         // Ulazi u kriticnu sekciju
	state[br] = 1;            // Postaje gladan
	printf("filozof %d je gladan\n", br +1 );
	test(br);                // Pokusava da uzme vilice
	sem_post(&mutex);        // Izlazi iz kriticne sekcije

	sem_wait(&sem[br]);      // Ceka dok ne dobije signal da moze da jede
	fflush(stdout);
	sleep(1);                // Simulira vreme cekanja
}

// Filozof vraca vilice i dozvoljava susjedima da jedu ako mogu
void vrati_vilice(int br){
	sem_wait(&mutex);             // Ulazi u kriticnu sekciju
	printf("filozof %d ostavlja vilice i ide misliti\n", br +1);
	fflush(stdout);
	sleep(1);                     // Simulira razmisljanje
	state[br] = 3;                // Vraca se u stanje "misli"
	
	// Proverava da li suskedi sada mogu da jedu
	test((br+4)%N);               // Ljevi sused
	test((br+1)%N);               // Desni sused
	sem_post(&mutex);            // Izlazi iz kriticne sekcije
}

// Funkcija niti koju svaki filozof izvrsava
void* filozof(void* pdr){
	int temp = *((int *)pdr);  // Dobija identifikator filozofa

	while(1){                  // Beskonasna petlja
		sleep(2);              // Razmislja
		uzima_vilicu(temp);    // Postaje gladan i pokusava da jede
		vrati_vilice(temp);    // Vraca vilice i nastavlja da misli
	}
}

// Glavna funkcija
int main(){
	pthread_t thread_id[N];      // Niti za svakog filozofa
	sem_init(&mutex, 0, 1);      // Inicijalizuje mutex semafor
	
	// Inicijalizuje sve semafore i stanja filozofa
	for(int i=0; i<N; i++){
		sem_init(&sem[i], 0, 0); // Svaki filozof ceka na svoj semafor
		state[i] = 3;            // Svi pocinju u stanju "misli"
	}

	// Kreira niti za svakog filozofa
	for(int i=0; i<N; i++){
		int *br = (int *) malloc(sizeof(int)); // Dinamicki alocira indeks
		*br = i;
		pthread_create(&thread_id[i], NULL, filozof, br); // Pokrece nit
		printf("Filozof %d misli\n", i+1);
	}

	// Ceka da se sve niti zavrse (nikad se ne zavrse zbog while(1))
	for(int i=0; i<N; i++){
		pthread_join(thread_id[i], NULL);
	}

	return 0; // Ispravno zavrsavanje programa
}


#include <math.h>     
#include <stdio.h>    
#include <sys/types.h> 
#include <signal.h>    
#include <stdlib.h>    
#include <unistd.h>   

int zavrsi = 0;        // Kontrolna varijabla za zavrsetak petlje
int statusBr;          // Trenutni status procitan iz status.txt

// Za SIGINT (prekid s tipkovnice - npr. Ctrl+C)
void sigint_funkcija ( int sig ){
	printf("\nOdaberite radnju:\n1) Prekid programa (bez dovrsavanja radnje)\n2) Zavrsi program\n3) Ispisi trenutni status\n4) Nastavi\n");
	int input;
	scanf("%d", &input);
	switch(input){
		case 1:
			printf("\nPrekid programa\n");
			exit(0); // Trenutni izlaz iz programa
			break;
		case 2:
			kill(getpid(), SIGTERM); // Sam sebi salje signal za zavrsetak
			break;
		case 3:
			kill(getpid(), SIGUSR1); // Sam sebi salje signal za ispis statusa
			break;
		default:
			break; // Nastavlja s radom ako se unese 4 ili nesto drugo
	}
}

// Za SIGTERM (signal za zavrsetak)
void sigterm_funkcija ( int sig ){
	printf("\nSIGTERM primljen, zavrsavanje programa\n");
	zavrsi = 1;  // Postavlja kontrolnu varijablu da prekine petlju
	exit(0);     // Zavrsava program
}

// Za SIGUSR1 (koristi se za ispis trenutnog statusa)
void sigusr1_funkcija ( int sig ){
	printf("\nTrenutni status: %d\n", statusBr);
}

// Funkcija koja obradjuje podatke
void proces(FILE *status, FILE *obrada){

	if(statusBr == 0){
		// Ako je status 0, procitaj zadnji broj iz obrada.txt
		int obradaBr;
		while(!feof(obrada)){
			fscanf(obrada, "%d", &obradaBr);
		}
		// Izracunaj status kao zaokruzeni korijen zadnjeg broja
		statusBr = round(sqrt(obradaBr));
	}

	// Oznaka da je obrada u tijeku - zapisuje 0 u status.txt
	fseek(status, 0, SEEK_SET);
	fprintf(status, "%d", 0 );
	fflush(status);
	fflush(obrada);
	sleep(3); // Simulacija obrade - pauza

	// Azurira status i dodaje novi kvadrat u obrada.txt
	fseek(status, 0, SEEK_SET);
	fseek(obrada, 0, SEEK_END); // Idi na kraj obrada.txt
	statusBr++; // Inkrementuj status
	fprintf(status, "%d", statusBr); // Upisi novi status
	fprintf(obrada, "%d\n", statusBr*statusBr); // Upisi kvadrat tog broja

	fflush(status);
    fflush(obrada);
	sleep(2); // Pauza za simulaciju
}

// Glavna funkcija
void main(){

	FILE  *status, *obrada;

	// Otvori status.txt za citanje
	status = fopen("status.txt", "r");
	// Otvori obrada.txt za citanje i pisanje
	obrada = fopen("obrada.txt", "r+");

	// Ucitaj status iz status.txt
	fscanf(status, "%d", &statusBr);

	// Registriraj signalne funkcije
	signal(SIGTERM, sigterm_funkcija);
	signal(SIGINT, sigint_funkcija);
	signal(SIGUSR1, sigusr1_funkcija);

	// Glavna petlja - radi dok zavrsi nije 1
	while(zavrsi == 0){
		// Ponovno otvori status.txt u nacinu za pisanje i citanje
		status = freopen("status.txt", "w+", status);
		proces(status, obrada); // Obradi sljedeæi broj
	}

	// Zatvori datoteke nakon zavrsetka
	fclose(status);
	fclose(obrada);
}


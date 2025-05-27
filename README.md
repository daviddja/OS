Zadaca 1 - Neka program simulira neki dugotrajni posao (slično servisima) koji koristi dvije datoteke:
u jednu dodaje do sada izračunate vrijednosti (npr. kvadrati slijednih brojeva), a u drugu
podatak do kuda je stigao. Npr. u obrada.txt zapisuje 1 4 9 … (svaki broj u novi red) a u
status.txt informaciju o tome gdje je stao ili kako nastaviti. Npr. ako je zadnji broj u
obrada.txt 100 u status.txt treba zapisati 10 tako da u idućem pokretanju može nastaviti
raditi i dodavati brojeve.
 Prije pokretanja te je datoteke potrebno ručno napraviti i inicijalizirati. Početne vrijednosti
mogu biti iste – broj 1 u obje datoteke.
 Pri pokretanju programa on bi trebao otvoriti obje datoteke, iz status.txt, pročitati tamo
zapisanu vrijednost. Ako je ona veća od nule program nastavlja s proračunom s prvom
idućom vrijednošću i izračunate vrijednosti nadodaje u obrada.txt. Prije nastavka rada u
status.txt upisuje 0 umjesto prijašnjeg broja, što treba označavati da je obrada u tijeku, da
program radi.
 Na signal (npr. SIGUSR1) program treba ispisati trenutni broj koji koristi u obradi. Na
signal SIGTERM otvoriti status.txt i tamo zapisati zadnji broj (umjesto nule koja je tamo)
te završiti s radom.
 Na SIGINT samo prekinuti s radom, čime će u status.txt ostati nula (čak se taj signal niti
ne mora maskirati – prekid je pretpostavljeno ponašanje!). To će uzrokovati da iduće
pokretanje detektira prekid – nula u status.txt, te će za nastavak rada, tj. Određivanje
idućeg broja morati „analizirati“ datoteku obrada.txt i od tamo zaključiti kako nastaviti
(pročitati zadnji broj i izračunati korijen). Operacije s datotekama, radi jednostavnosti,
uvijek mogu biti u nizu open+fscanf/fprintf+close, tj. ne držati datoteke otvorenima da se
izbjegnu neki drugi problemi. Ali ne mora se tako. U obradu dodati odgodu (npr. sleep(5))
da se uspori izvođenje.

Zadaca 2 - Međusobno isključivanje ostvariti za dva procesa/dretve
međusobnim isključivanjem po Dekkerovom algoritmu.

Zadaca 3 - program koji simulira tijek rezervacije stolova u nekom
restoranu. Program na početku treba stvoriti određeni broj dretvi koji
se zadaje u naredbenom retku. Svaka dretva/proces nakon isteka
jedne sekunde provjerava ima li slobodnih stolova te slučajno
odabire jedan od njih. Nakon odabira dretva ulazi u kritični odsječak
te ponovo provjerava je li odabrani stol slobodan. Ako jest, označava
stol zauzetim i izlazi iz kritičnog odsječka. U oba slučaja, nakon
obavljene operacije ispisuje trenutno stanje svih stolova te podatke o
obavljenoj rezervaciji. Prilikom ispisa za svaki stol mora biti vidljivo
je li slobodan ili broj dretve/procesa koja je taj stol rezervirala. Broj
stolova se također zadaje u naredbenom retku. Svaka dretva
ponavlja isti postupak sve dok više nema slobodnih stolova.
Program završava kada sve dretve završe.

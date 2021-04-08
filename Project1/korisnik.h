#pragma once

typedef struct korisnik
{
	char *ime, *prezime, *korIme, aktivan, korGrupa;
	int pin;
}KORISNIK;

typedef struct lista
{
	KORISNIK *clan;
	struct lista *sljedeci, *prethodni;
}LISTA;

int valuta;

//formiranje liste korisnika, brisanje, ispis

void ucitavanjeKorisnika(KORISNIK *, char *, char *, char *, char, char, int);	//alocira i ucitava podatke o jednom korisniku
void dodajKorisnika(LISTA **, LISTA **, KORISNIK *);		//dodaje korisnika u listu
void listaKorisnika(LISTA **, LISTA **);					//formira listu od ucitanih korisnika iz fajla "Korisnici.txt"
KORISNIK *pretraga(LISTA *, LISTA *, char *, int);			//pretrazuje listu
void brisanjeKorisnika(LISTA **, LISTA **, char *);			//brisanje korisnika s obzirom na sve uslove
void upisKorisnikaUFajl(LISTA **, LISTA**);					//upis korisnika u fajl "Korsinici.txt" pri zavrsetku rada programa
void pisi(LISTA *);											//ispisuje listu korisnika sistema

//pomocne funkcije
void uspjesnaPrijava(LISTA **, LISTA **, char *);			//obavjestava da li se korisnik prijavio na sistem
void prijavaSuperKorisnika(LISTA **, LISTA **);				//otvara meni za super korisnika
char biranjeOpcija(int);									//provjeravanje ispravnosti prilikom biranje opcija
void ispisProzora();										//ispisivanje menija
void uspjesnoKreiranjeNaloga(KORISNIK);						//obavjestava o uspjesnom kreiranju naloga i ispisuje podatke novog korisnika
int verifikacijaPina(LISTA **, LISTA **, char*, int);		//pomocna funkcija za verifikaciju pina prilikom brisanja, promjene informacija ili blokiranja korisnika
void pomocPriBiranjuOpcija(LISTA *, LISTA *, char);			//pomocna funkcija koja samo prosljedjuje na prijavu ili kreiranje naloga
void podesavanjeValute();									//funkcija za podesavanje valute aplikacije	
void oslobadjanjeKorisnika(KORISNIK *);						//oslobadjanje korisnika nakon zavrsetka programa

//provjere vezane za (korisnicko) ime, prezime
int provjeraRazmaka(char *);								//vraca informaciju o tome da li postoji razmak u imenu
char *utvrdjivanjeImena(char *);							//vraca ucitani format imena
char *provjeraIspravnosti(LISTA *, LISTA *, char *);		//provjerava sve uslove ispravnosti korisnickog imena
char* provjeraImena(char *, int);							//vraca isti format imena korisnika za upis u fajl

//provjere vezane za pin kod
int provjeraKaraktera(char *);								//provjerava da li pin kod sadrzi slova ili znakove
char* provjeraPina(char *);	                                //provjerava potpuno ispravnost pin koda
char *kodovanjePina(int,char *);							//koduje pin korisnika koji se unosi u fajl "Korisnici.txt"
int dekodovanjePina(char*);									//dekoduje pin korisnika koji se nalazi u fajlu "Korisnici.txt"
void sakrivanjePina(char *);								//ispisuje '*' prilikom unosa pin koda

//prijava i kreiranje naloga, promjena informacija
void noviNalog(LISTA **, LISTA **, int);					//ucitava podatke o novom korisniku u listu korisnika
void prijava(LISTA *, LISTA *);								//omogucava postojecem korisniku prijavu na sistem
void dobrodosli(LISTA *, LISTA *);							//omogucava korisniku da izabere zeljenu opciju
void promjenaInformacija(LISTA *, LISTA *, char **);		//promjena informacija na korisnickom nalogu
int blokiranjeKorisnika(LISTA *, LISTA *, char*);			//funkija kojom super korisnik blokira/aktivira druge korisnike


#pragma once
#include <stdio.h>

typedef struct proizvod
{
	char *sifraProizvoda;
	double kolicina, cijena, vrijednost;
}PROIZVOD;

typedef struct racun
{
	char *kupac, datum[11];
	double PDVvrijednost, ukupanIznos;
	PROIZVOD *proizvodi;
	int brojProizvoda;
}RACUN;

void citajRacun();
void ucitavanjePodatakaSaRacunaTxt(char*);
void ucitavanjePodatakaSaRacunaCsv(char*);
int citanjeRijeciIzFajla(FILE *, char *);
void ucitavanjeProizvodaFormat1(FILE *, PROIZVOD *, RACUN *, char *);
void ucitavanjeProizvodaFormat24(FILE *, PROIZVOD *, RACUN *, char *);
void ucitavanjeProizvodaFormat3(FILE *, PROIZVOD *, RACUN *, char *);
void rasporedjivanjeRacuna(RACUN *);
void smjestanjeUArhivu(char *);

//pomocne funkcije
void ucitavanjeProizvoda(RACUN *, PROIZVOD *, char *, int);
void utvrdjivanjeProizvoda(PROIZVOD *, char *);
void odredjivanjePodatakaNaRacunu(RACUN *, char *);
void odredjivanjeNazivaFajla(char *, char *, char *);
char *odredjivanjeMjeseca(char*);
void kreiranjePotrebnihFajlova(RACUN *, char *, char *);

//verifikacija racuna
int verifikacijaRacuna(RACUN *, char *);
int verifikacijaProizvoda(PROIZVOD *);
int	verifikacijaPDVa(double, double);
int verifikacijaUkupnogIznosa(double, double, double);

void upisKupca(FILE *, RACUN *);
void upisProizvoda(FILE *, RACUN *, char *);
void upisMjeseca(FILE *, RACUN *);

void prikazFajla(char *, int);
void pregledKupaca(FILE *);
void pregledProizvoda(FILE *);
void mjesecniPregled(FILE *);




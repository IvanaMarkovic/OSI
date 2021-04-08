#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "korisnik.h"
#include "racun.h"
#define SUPER_USER "igord12"
#define SUPER_CODE 2241
#define BACKSPACE 8

valuta = 1;

/*		***************************************************************   
		**********FORMIRANJE LISTE KORISNIKA, BRISANJE, ISPIS**********
		*************************************************************** */

void ucitavanjeKorisnika(KORISNIK *korisnik, char *ime, char *prezime, char *korIme, char grupa, char aktivan, int pin)
{
	korisnik->ime = (char*)calloc(strlen(ime)+1, sizeof(char));
	strcpy(korisnik->ime, ime);
	korisnik->prezime = (char*)calloc(strlen(prezime)+1, sizeof(char));
	strcpy(korisnik->prezime, prezime);
	korisnik->korIme = (char*)calloc(strlen(korIme)+1, sizeof(char));
	strcpy(korisnik->korIme, korIme);
	korisnik->aktivan = aktivan;
	korisnik->korGrupa = grupa;
	korisnik->pin = pin;
}

void dodajKorisnika(LISTA **pglava, LISTA **prep, KORISNIK *korisnik)
{
	LISTA *noviClan = (LISTA*)malloc(sizeof(LISTA));
	noviClan->clan = korisnik;
	noviClan->sljedeci = 0;
	noviClan->prethodni = 0;
	if (*pglava == 0)
	{
		*pglava = *prep = noviClan;

	}
	else if (strcmp((*pglava)->clan->korIme, noviClan->clan->korIme) > 0)
	{
		noviClan->sljedeci = *pglava;
		(*pglava)->prethodni = noviClan;
		*pglava = noviClan;
	}
	else if (strcmp((*prep)->clan->korIme, noviClan->clan->korIme) < 0)
	{
		noviClan->prethodni = *prep;
		(*prep)->sljedeci = noviClan;
		*prep = noviClan;
	}
	else
	{
		LISTA *p = (*pglava);
		while (strcmp(p->sljedeci->clan->korIme, noviClan->clan->korIme) < 0)
			p = p->sljedeci;
		noviClan->sljedeci = p->sljedeci;
		noviClan->prethodni = p;
		p->sljedeci->prethodni = noviClan;
		p->sljedeci = noviClan;
	}
}

void listaKorisnika(LISTA **pglava, LISTA **prep)
{
	FILE *f;
	char ime[30] = { 0 }, prezime[30] = { 0 }, korIme[20] = { 0 }, pin[9] = { 0 }, a, grupa;
	int pinKod, n = 0, kapacitet = 10;
	KORISNIK *korisnik;
	korisnik = (KORISNIK*)calloc(kapacitet, sizeof(KORISNIK));
	if ((f = fopen("Korisnici.txt", "r")) != NULL)
	{
		while ((fscanf(f, "%s %s %s %c %c %s\n", korIme, ime, prezime, &grupa, &a, pin)) == 6)
		{

			if (n == kapacitet)
				korisnik = (KORISNIK*)realloc(korisnik, (kapacitet *= 2) * sizeof(KORISNIK));
			else
			{
				pinKod = dekodovanjePina(pin);
				ucitavanjeKorisnika(korisnik + n, ime, prezime, korIme, grupa, a, pinKod);
				dodajKorisnika(pglava, prep, korisnik + n);
				n++;
			}
		}
		fclose(f);
	}
	else
	{
		f = fopen("Korisnici.txt", "w");
		fclose(f);
	}
}

KORISNIK *pretraga(LISTA *glava, LISTA *rep, char *korIme, int pin)
{
	if (glava == 0) return 0;
	while ((strcmp(glava->clan->korIme, korIme) < 0) && (strcmp(rep->clan->korIme, korIme) > 0))
	{
		glava = glava->sljedeci;
		rep = rep->prethodni;
	}
	if ((strcmp(glava->clan->korIme, korIme)) == 0 && (glava->clan->pin == pin || pin == SUPER_CODE || pin == -1))
			return glava->clan;
	if (strcmp(rep->clan->korIme, korIme) == 0 && (rep->clan->pin == pin || pin == SUPER_CODE || pin == -1))
			return rep->clan;
	return 0;
}

void brisanjeKorisnika(LISTA **pglava, LISTA **prep, char *korIme)
{
	LISTA *p;
	if ((strcmp(korIme, (*pglava)->clan->korIme) == 0) && (strcmp(korIme, (*prep)->clan->korIme) == 0))
	{
		p = *pglava;
		*pglava = *prep = 0;
	}
	else if ((strcmp(korIme, (*pglava)->clan->korIme) == 0))
	{
		p = *pglava;
		*pglava = (*pglava)->sljedeci;
		(*pglava)->prethodni = 0;
	}
	else if ((strcmp(korIme, (*prep)->clan->korIme) == 0))
	{
		p = *prep;
		*prep = (*prep)->prethodni;
		(*prep)->sljedeci = 0;
	}
	else
	{
		p = *pglava;
		for (p = p->sljedeci; (strcmp(korIme, p->clan->korIme) != 0) && p->sljedeci; p = p->sljedeci);
		p->prethodni->sljedeci = p->sljedeci;
		p->sljedeci->prethodni = p->prethodni;
	}
	oslobadjanjeKorisnika(p->clan);
	free(p);
}

void upisKorisnikaUFajl(LISTA **pglava, LISTA **prep)
{
	FILE *fajlZaUpis = fopen("Korisnici.txt", "w");
	while (*pglava != NULL)
	{
		LISTA *pomocniCvor = (*pglava)->sljedeci;
		char pin[9] = { 0 };
		kodovanjePina((*pglava)->clan->pin, pin);
		fprintf(fajlZaUpis, "%s %s %s %c %c %s\n", (*pglava)->clan->korIme, provjeraImena((*pglava)->clan->ime, 1),
			provjeraImena((*pglava)->clan->prezime, 1), (*pglava)->clan->korGrupa, (*pglava)->clan->aktivan, pin);
		oslobadjanjeKorisnika((*pglava)->clan);
		free(*pglava);
		*pglava = pomocniCvor;
	}
	*prep = NULL;
	fclose(fajlZaUpis);
}

int verifikacijaPina(LISTA **pglava, LISTA **prep, char *korIme, int opcija)
{
	int j = 0, pin;
	char *pinKod = (char*)calloc(50, sizeof(char));
	KORISNIK *kljuc = pretraga(*pglava, *prep, korIme, -1);
	if (strcmp(korIme, SUPER_USER) == 0)
	{
		printf("\nNije moguce %s super korisnika!\n",
			opcija == 1 ? "izbrisati nalog" : (opcija == 2 ? "promijeniti ingormacije" : "blokirati nalog"));
		return 0;
	}
	else if (kljuc == 0)
	{
		printf("Unijeto korisnicko ime ne postoji na sistemu!\n");
		return 0;
	}
	do
	{
		j++;
		printf("Unesite pin kod za verifikaciju naloga: ");
		sakrivanjePina(pinKod);
		pin = atoi(pinKod);
	} while ((kljuc->pin != pin && pin != SUPER_CODE) && j < 3);
	free(pinKod);
	if (j < 3)
	{
		switch(opcija)
		{
		case 1:
			brisanjeKorisnika(pglava, prep, korIme);
			break;
		case 2:
			promjenaInformacija(*pglava, *prep, &korIme);
			break;
		case 3:
			if (blokiranjeKorisnika(*pglava, *prep, korIme))
				printf("\nKorisnicki nalog %s je blokiran!\n", korIme);
			else
				printf("\nKorisnicki nalog %s je ponovno aktivan!\n", korIme);
			break;
		}
		return 1;
	}
	else
	{
		printf("\nDozvoljeno je tri puta unijeti pogresan pin kod!\nImate mogucnost da ponovo birate: ");
		return 0;
	}
}

void pisi(LISTA *glava)
{
	printf("=== ==================== ========================= ========================= ================\n");
	printf("RB. KORISNICKO IME       IME                       PREZIME                   KORISNICKA GRUPA\n");
	printf("=== ==================== ========================= ========================= ================\n");
	int i = 1;
	while (glava)
	{
		strcpy(glava->clan->prezime, provjeraImena(glava->clan->prezime, 1));
		strcpy(glava->clan->ime, provjeraImena(glava->clan->ime, 1));
		printf("%02d. %-20s %-25s %-25s %-18s\n", i++, glava->clan->korIme, glava->clan->ime, glava->clan->prezime,
			(glava->clan->korGrupa=='1'?"administrator":"analiticar"));
		glava = glava->sljedeci;
	}
	printf("=== ==================== ========================= ========================= ================\n");
}

/*		************************************
		**********POMOCNE FUNKCIJE**********
		************************************ */

void uspjesnaPrijava(LISTA **pglava, LISTA **prep, char *korIme)
{
	printf("\n\n");
	static int k = 0, p = 0;
	KORISNIK *kljuc = pretraga(*pglava, *prep, korIme, -1);
	char *opcija = 0;
	int korGrupa = atoi(&kljuc->korGrupa);
	if (korGrupa == 1 && k == 0)
	{
		podesavanjeValute();
		++k;
	}
	else if (korGrupa == 2 && p == 0)
	{
		citajRacun();
		++p;
	}
	for (int i = 0, j = 0; i < 11 || j < 9;)
	{
		if (i == 0 || i == 10 || j == 8)
			printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n"), ++i, ++j;
		if (i == 1 || i == 9 || j == 7)
			printf("*                                                               *\n"), ++i, ++j;
		if (korGrupa == 1 && j == 2)
			printf("*     Pregled svih korisnika sistema (opcija %d)                 *\n", (j++) - 1);
		else if (korGrupa == 2 && i == 2)
		{
			printf("*     Pregled svih podataka za odredjenog kupca (opcija 1)      *\n");
			printf("*     Pregled svih podataka za odredjeni proizvod (opcija 2)    *\n");
			printf("*     Pregled ukupne prodaje za odredjeni mjesec (opcija 3)     *\n");
			i += 3;
		}
		if ((i < 6 && j == 2) || (j < 4 && i == 2))
		{
			printf("*     Promjena podataka na korisnickom nalogu (opcija %d)        *\n", korGrupa == 1 ? 2 : 4);
			printf("*     Brisanje korisnickog naloga (opcija %d)                    *\n", korGrupa == 1 ? 3 : 5);
			printf("*     Odjava sa korisnikog naloga (opcija %d)                    *\n", korGrupa == 1 ? 4 : 6);
			printf("*     Izlaz iz aplikacije (opcija %d)                            *\n", korGrupa == 1 ? 5 : 7);
			(korGrupa == 1) ? (j += 4) : (i += 4);
		}
		if ((i == 11 && j == 4) || (i == 4 && j == 9))
			break;
	}
	char op;
	if (korGrupa == 1)
		op = biranjeOpcija(5);
	else
		op = biranjeOpcija(7);
	switch (op)
	{
	case '1':
		if (korGrupa == 1)
		{
			printf("     Svi korisnici sistema\n\n");
			Sleep(1000);
			pisi(*pglava);
		}
		else
		{
			opcija = (char*)calloc(50, sizeof(char));
			printf("Unesite naziv kupca: ");
			scanf("%s", opcija);
			prikazFajla(opcija,1);
			free(opcija);
		}
		uspjesnaPrijava(pglava, prep, korIme);
		break;
	case '2':
		if (korGrupa == 1)
			verifikacijaPina(pglava, prep, korIme, 2);
		else
		{
			opcija = (char*)calloc(50, sizeof(char));
			printf("Unesite naziv proizvoda: ");
			scanf("%s", opcija);
			prikazFajla(opcija, 2);
			free(opcija);
		}
		uspjesnaPrijava(pglava, prep, korIme);
		break;
	case '3':
		if (korGrupa == 1 && verifikacijaPina(pglava, prep, korIme, 1))
		{
			printf("Uspjesno ste izbrisali korisnicki nalog!\n\n");
			dobrodosli(*pglava, *prep);
		}
		else if (korGrupa == 2)
		{
			opcija = (char*)calloc(50, sizeof(char));
			printf("Unesite naziv mjeseca: ");
			scanf("%s", opcija);
			prikazFajla(opcija, 3);
			free(opcija);
			uspjesnaPrijava(pglava, prep, korIme);
		}
		break;
	case '4':
		if (korGrupa == 1)
		{
			printf("\nUspjesno ste se odjavili sa sistema!\n\n");
			dobrodosli(*pglava, *prep);
		}
		else if(korGrupa ==2)
		{
			verifikacijaPina(pglava, prep, korIme, 2);
			uspjesnaPrijava(pglava, prep, korIme);
		}
		break;
	case '5':
		if (korGrupa == 1)
			return;
		else if (korGrupa == 2 && verifikacijaPina(pglava, prep, korIme, 1))
		{
			printf("Uspjesno ste izbrisali korisnicki nalog!\n\n");
			dobrodosli(*pglava, *prep);
		}
		else uspjesnaPrijava(pglava, prep, korIme);
		break;
	case '6':
		printf("\nUspjesno ste se odjavili sa sistema!\n\n");
		dobrodosli(*pglava, *prep);
		break;
	case '7':
		return;
	}
}

void prijavaSuperKorisnika(LISTA **pglava, LISTA **prep)
{
	printf("\n\n");
	char korIme[50];
	for (int i = 0; i < 11;)
	{
		if (i == 0 || i == 10)
			printf("|~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~|\n"), ++i;
		if (i == 1 || i == 9)
			printf("|                                                               |\n"), ++i;
		if (i < 3)
		{
			printf("|     Registracija novog korisnika (opcija 1)                   |\n");
			printf("|     Blokiranje/aktiviranje naloga korisnika (opcija 2)        |\n");
			printf("|     Brisanje naloga korisnika (opcija 3)                      |\n");
			printf("|     Promjena informacija na korisnickim nalozima (opcija 4)   |\n");
			printf("|     Podesavanje valute aplikacije (opcija 5)                  |\n");
			printf("|     Odjava sa korisnickog naloga (opcija 6)                   |\n");
			printf("|     Izlaz iz aplikacije (opcija 7)                            |\n");
			i += 7;
		}
	}
	char op = biranjeOpcija(7);
	switch (op)
	{
	case '1':
		noviNalog(pglava, prep, 2);
		break;
	case '2':
	case '3':
	case '4':
		Sleep(1000);
		pisi(*pglava);
		printf("Unesite korisnicko ime korisnika ciji nalog zelte %s: ", op == '2' ? "blokirati/aktivirati" : (op == '3' ? "izbrisati" : "promijeniti"));
		strcpy(korIme, utvrdjivanjeImena(korIme));
		if (op == '2')
			verifikacijaPina(pglava, prep, korIme, 3);
		else if (op == '3')
			verifikacijaPina(pglava, prep, korIme, 1);
		else verifikacijaPina(pglava, prep, korIme, 2);
		prijavaSuperKorisnika(pglava, prep);
		break;
	case '5':
		podesavanjeValute();
		prijavaSuperKorisnika(pglava, prep);
		break;
	case '6':
		printf("\nUspjesno ste se odjavili sa sistema!\n\n");
		dobrodosli(*pglava, *prep);
		break;
	case '7':
		return;
	}

}


char biranjeOpcija(int n)
{
	char *opcija = (char*)calloc(50, sizeof(char)), op;
	int k = 0;
	printf("\nUnesite zeljenu opciju: ");
	scanf("%s", opcija);
	while(k == 0)
	{
		if (strlen(opcija) == 1)
			for (int i = 1; i <= n; i++)
				if (atoi(opcija) == i)
					k = 1;
		if (k == 0)
		{
			printf("\nOpcija - %s ne postoji!\nUnesite ponovo zeljenu opciju: ", opcija);
			scanf("%s", opcija);
		}
	}
	op = opcija[0];
	free(opcija);
	return op;
}

void ispisProzora()
{
	for (int i = 0; i < 7;)
	{
		if (i == 0 || i == 6)
			printf("* * * * * * * * * * * * * * * * * * * * * * * * * *\n"), ++i;
		if (i == 1 || i == 5)
			printf("*                                                 *\n"), ++i;
		if (i < 3)
		{	
			printf("*            Prijavi se (opcija 1)                *\n");
			printf("*    Kreiraj novi korisnicki nalog (opcija 2)     *\n");
			printf("*         Izlaz iz aplikacije (opcija 3)          *\n");
			i += 3;
		}
	}
}

void uspjesnoKreiranjeNaloga(KORISNIK korisnik)
{
	printf("\n       USPJESNO STE SE PRIJAVILI NA SISTEM!       \n\n");
	printf(" Podaci o novom korisniku:\n\n");
	for (unsigned i = 0; i < 9;)
	{
		if (i == 0 || i == 8)
			printf("* * * * * * * * * * * * * * * * * * * * * * * * * *\n"),++i;
		if(i==1 || i==7)
			printf(" *\n"), ++i;
		if (i < 3)
		{
			printf("  *   Ime: %s\n", korisnik.ime);
			printf("   *  Prezime: %s\n", korisnik.prezime);
			printf("    * Korisnicko ime: %s\n", korisnik.korIme);
			printf("   *  Korisnicka grupa: %s\n", (korisnik.korGrupa == '1') ? "administrator" : "analiticar");
			printf("  *   Pin kod: ****\n");
			i += 5;
		}
	}
}

void pomocPriBiranjuOpcija(LISTA *glava, LISTA *rep, char opcija)
{
	if (opcija == '1')
		prijava(glava, rep);
	else if( opcija == '2')
		noviNalog(&glava, &rep, 1);
	else if(opcija == '3')
		return;
}

void podesavanjeValute()
{
	printf("	  Meni za izbor valute aplikacije\n\n");
	for (int i = 0; i < 7;)
	{
		if (i == 0 || i == 6)
			printf("* * * * * * * * * * * * * * * * * * * * * * * * * *\n"), ++i;
		if (i == 1 || i == 5)
			printf("*                                                 *\n"), ++i;
		if (i < 3)
		{
			printf("*     Konvertibilna marka - BAM (opcija 1)        *\n");
			printf("*     Evro - EUR (opcija 2)                       *\n");
			printf("*     Americki dolar - USD (opcija 3)             *\n");
			i += 3;
		}
	}
	char  op = biranjeOpcija(3);
	if (op == '1')
		valuta = 1;
	else if (op == '2')
		valuta = 2;
	else
		valuta = 3;
}

void oslobadjanjeKorisnika(KORISNIK *korisnik)
{
	free(korisnik->korIme);
	free(korisnik->ime);
	free(korisnik->prezime);
}

/*		*****************************************************
		**********PROVJERE VEZANE ZA KORISNICKO IME**********
		***************************************************** */

int provjeraRazmaka(char *ime)
{
	for (unsigned i = 0; i < strlen(ime); i++)
		if (ime[i] == ' ')
			return 1;
	return 0;
}

char* utvrdjivanjeImena(char *korisnik)
{
	int i;
	char c;
	scanf("%s", korisnik);
	i = strlen(korisnik);
	do
	{
		scanf("%c", &c);
		korisnik[i++] = c;
	} while (c != '\n');
	korisnik[i - 1] = 0;
	return korisnik;
}

char *provjeraIspravnosti(LISTA *glava, LISTA *rep, char *korIme)
{
	KORISNIK *kljuc1 = pretraga(glava, rep, korIme, -1);
	int kljuc2;
	kljuc2 = provjeraRazmaka(korIme);
	if (kljuc1 == 0 && kljuc2 == 0)
		return korIme;
	do
	{
		if (kljuc1 != 0 && kljuc2 == 0)
			printf("Uneseno korisnicko ime je vec zauzeto!\nMolimo Vas da unesete ponovo: ");
		else if (kljuc1 == 0 && kljuc2 == 1)
			printf("Uneseno korisnicko ime sadrzi razmak!\nMolimo Vas da unesete ponovo: ");
		strcpy(korIme, utvrdjivanjeImena(korIme));
		kljuc1 = pretraga(glava, rep, korIme,-1);
		kljuc2 = provjeraRazmaka(korIme);
	} while (kljuc1 != 0 || kljuc2 != 0);
	return korIme;
}

char* provjeraImena(char *korisnik, int n)
{
	if (n == 1)
	{
		for (unsigned i = 0; i < strlen(korisnik); i++)
		{
			if (korisnik[i] == '-')
				korisnik[i] = ' ';
			else if (korisnik[i] == ' ')
				korisnik[i] = '-';
		}
	}
	else if (n == 2)
	{
		strcpy(korisnik, utvrdjivanjeImena(korisnik));
		if (korisnik[0] >= 'a' && korisnik[0] <= 'z')
			korisnik[0] -= 0x20;
		for (unsigned i = 1; i < strlen(korisnik); i++)
		{
			if ((korisnik[i] == ' ' || korisnik[i] == '-') && korisnik[i + 1] >= 'a' &&  korisnik[i + 1] <= 'z')
			{
				korisnik[i + 1] -= 0x20;
				++i;
			}
			else if (korisnik[i] >= 'A' &&  korisnik[i] <= 'Z')
				korisnik[i] += 0x20;
		}
	}
	return korisnik;
}

/*		**********************************************
		**********PROVJERE VEZANE ZA PIN KOD**********
		********************************************** */

int provjeraKaraktera(char *pin)
{
	int j = 0;
	for (unsigned i = 0; i < strlen(pin); i++)
		if ((pin[i] >= '0') && (pin[i] <= '9'))
			j++;
	if (j == strlen(pin) && strlen(pin) == 4)
		return 0;
	else if (j == strlen(pin) && (strlen(pin) != 4))
		return -1;
	else	
		return 1;
}

char* provjeraPina(char *pin)
{
	int kljuc;
	kljuc = provjeraKaraktera(pin);
	if (kljuc == 0)
		return pin;
	do
	{
		if (strlen(pin) > 4 && kljuc == 1)
			printf("\nPin ima vise of 4 karaktera!Takodje, sadrzi nedozvoljene karaktere!\nMolimo Vas da unestete ponovo:");
		else if (strlen(pin) > 4 && (kljuc == -1 || kljuc == 0))
			printf("\nPin ima vise of 4 karaktera!\nMolimo Vas da unestete ponovo: ");
		else if (strlen(pin) < 4 && kljuc == 1)
			printf("\nPin ima manje od 4 karaktera!Takodje, pin sadrzi nedozvoljene karaktere!\nMolimo Vas da uneste ponovo: ");
		else if (strlen(pin) < 4 && (kljuc == -1 || kljuc == 0))
			printf("\nPin ima manje od 4 karaktera!\nMolimo Vas da unesete ponovo: ");
		else
			printf("\nPin sadrzi nedozvoljene karaktere!\nMolimo Vas da unesete ponovo: ");
		sakrivanjePina(pin);
		kljuc = provjeraKaraktera(pin);
	} while (strlen(pin) != 4 || kljuc != 0);
	return pin;
}

char* kodovanjePina(int pin, char *kod)
{
	int p[4], i=0;
	char k[9] = { 0 };
	while (pin)
	{
		p[i++]=pin%10;
		pin /= 10;
	}
	for (unsigned i = 4, j = 0; i > 0; i--)
	{
		switch (p[i - 1])
		{
			case 1:
				k[j++] = '*', k[j++] = '>';
				break;
			case 2:
				k[j++] = '@', k[j++] = '+';
				break;
			case 3:
				k[j++] = '{', k[j++] = '-';
				break;
			case 4:
				k[j++] = '?', k[j++] = '=';
				break;
			case 5:
				k[j++] = '~', k[j++] = '$';
				break;
			case 6:
				k[j++] = '.', k[j++] = '<';
				break;
			case 7:
				k[j++] = '^', k[j++] = '_';
				break;
			case 8:
				k[j++] = '/', k[j++] = '!';
				break;
			case 9:
				k[j++] = '[', k[j++] = ',';
				break;
			case 0:
			default:
				k[j++] = '-', k[j++] = '#';
				break;
		}
	}
	strcpy(kod, k);
	return kod;
}

int dekodovanjePina(char *kod)
{
	int pin=0, p[4];
	for (unsigned i = 0, j=0; i < strlen(kod); i+=2)
	{
		switch (kod[i])
		{
			case '-':
				if (kod[i + 1] == '#')
					p[j++] = 0;
				break;
			case '*':
				if (kod[i + 1] == '>')
					p[j++] = 1;
				break;
			case '@':
				if (kod[i + 1] == '+')
					p[j++] = 2;
				break;
			case '{':
				if (kod[i + 1] == '-')
					p[j++] = 3;
				break;
			case '?':
				if (kod[i + 1] == '=')
					p[j++] = 4;
				break;
			case '~':
				if (kod[i + 1] == '$')
					p[j++] = 5;
				break;
			case '.':
				if (kod[i + 1] == '<')
					p[j++] = 6;
					break;
			case '^':
				if (kod[i + 1] == '_')
					p[j++] = 7;
				break;
			case '/':
				if (kod[i + 1] == '!')
					p[j++] = 8;
				break;
			default:
				if (kod[i] == '[' && kod[i + 1] == ',')
					p[j++] = 9;
				break;
		} 
	}
	for (unsigned i = 0, k = 1000; i < 4; ++i, k /= 10)
		pin += (k*p[i]);
	return pin;
}

void sakrivanjePina(char *pin)
{
	int i = 0;
	while ((pin[i] = _getch()) != '\r')
	{
		if (pin[i] == BACKSPACE)
		{
			printf("\b \b");
			i--;
		}
		else
		{
			printf("*");
			i++;
		}
	}
	pin[strlen(pin) - 1] = '\0';
	printf("\n");
}

/*		***************************************************
		**********PRIJAVA I KRIRANJE NOVOG NALOGA**********
		*************************************************** */

void noviNalog(LISTA **pglava, LISTA **prep, int opcija)
{
	KORISNIK *noviKorisnik=(KORISNIK*)malloc(sizeof(KORISNIK));
	char *ime, *prezime, *korIme, *pinKod, korGrupa;
	ime = (char*)calloc(50, sizeof(char));
	prezime = (char*)calloc(50, sizeof(char));
	korIme = (char*)calloc(50, sizeof(char));
	pinKod = (char*)calloc(50, sizeof(char));
	int pin;

	printf("\nKreiranje novog korisnickog naloga!\n");
	printf("	Ime: ");
	strcpy(ime, provjeraImena(ime,2));

	printf("	Prezime: ");
	strcpy(prezime, provjeraImena(prezime,2));

	printf("	Korisnicko ime(unesite bez razmaka): ");
	strcpy(korIme, utvrdjivanjeImena(korIme));
	strcpy(korIme, provjeraIspravnosti(*pglava, *prep, korIme));

	printf("	Korisnicka grupa:\n");
	printf("	    *administrator (opcija 1)\n");
	printf("	    *analiticar (opcija 2)\n");
	korGrupa = biranjeOpcija('2');

	printf("	Pin kod(sadrzi 4 cifre): ");
	sakrivanjePina(pinKod);
	strcpy(pinKod, provjeraPina(pinKod));
	pin = atoi(pinKod);

	ucitavanjeKorisnika(noviKorisnik, ime, prezime, korIme, korGrupa, 'a', pin);
	dodajKorisnika(pglava, prep, noviKorisnik);
	free(ime), free(prezime), free(pinKod);
	uspjesnoKreiranjeNaloga(*noviKorisnik);
	if (opcija == 1)
		uspjesnaPrijava(pglava, prep, korIme);
	else if (opcija == 2)
		prijavaSuperKorisnika(pglava, prep);
	free(korIme);
}

void prijava(LISTA *glava, LISTA *rep)
{
	char *korIme = (char*)calloc(50, sizeof(char));
	char *pinKod = (char*)calloc(50, sizeof(char));
	int pin;
	KORISNIK *kljuc;
	printf("Unesite Vase korisniko ime: ");
	strcpy(korIme, utvrdjivanjeImena(korIme));
	printf("Unesite Vas pin kod: ");
	sakrivanjePina(pinKod);
	pin = atoi(pinKod);
	kljuc = pretraga(glava, rep, korIme, -1);
	if (kljuc != 0 && kljuc->aktivan == 'b')
	{
		printf("\n\nNe mozete se prijaviti, korisnicki nalog je blokiran!\n");
		Sleep(1000);
		dobrodosli(glava, rep);
	}
	else
	{
		if (kljuc != 0 && kljuc->pin == pin)
			uspjesnaPrijava(&glava, &rep, korIme);
		else if ((strcmp(korIme, SUPER_USER) == 0 && pin == SUPER_CODE))
			prijavaSuperKorisnika(&glava, &rep);
		else if (strcmp(korIme, SUPER_USER) != 0 && kljuc == 0)
		{
			printf("Trazeni nalog ne postoji na sistemu!\n\n");
			ispisProzora();
			char op = biranjeOpcija(3);
			pomocPriBiranjuOpcija(glava, rep, op);

		}
		else if (pin != SUPER_CODE || (kljuc != 0 && kljuc->pin != pin))
		{
			short i = 0;
			do
			{
				i++;
				printf("Unijeli ste pogresan pin kod!\nUnesite ponovo: ");
				sakrivanjePina(pinKod);
				pin = atoi(pinKod);
			} while ((pin != SUPER_CODE || (kljuc != 0 && kljuc->pin != pin)) && i < 3);
			if (i < 3 && kljuc !=0 && kljuc->pin == pin)
				uspjesnaPrijava(&glava, &rep, korIme);
			else if (i < 3 && pin == SUPER_CODE)
				prijavaSuperKorisnika(&glava, &rep);
			else
			{
				printf("\nDozvoljeno je unijeti pogresan pin kod tri puta!\nImate mogucnost da birate ponovo:\n");
				ispisProzora();
				char op = biranjeOpcija(3);
				pomocPriBiranjuOpcija(glava, rep, op);
			}
		}
	}
	free(pinKod);
	free(korIme);
}

void dobrodosli(LISTA *glava, LISTA *rep)
{
	printf("              DOBRODOSLI NA SISTEM ZA\n");
	printf("                 ANALIZU TROSKOVA\n\n");
	ispisProzora();
	char op = biranjeOpcija(3);
	pomocPriBiranjuOpcija(glava,rep,op);
}

void promjenaInformacija(LISTA *glava, LISTA *rep, char **korIme)
{
	KORISNIK *kljuc = pretraga(glava, rep, *korIme, -1);
	char *novoIme = (char*)calloc(50, sizeof(char));
	if (kljuc == 0)
	{
		printf("Trazeni korisnicki nalog ne postoji!\n");
		return;
	}
	else
	{
		char *pin = (char *)calloc(50, sizeof(char));
		for (int i = 0; i < 7;)
		{
			if (i == 0 || i == 6)
				printf("* * * * * * * * * * * * * * * * * * * * * * * * * *\n"), ++i;
			if (i == 1 || i == 5)
				printf("*                                                 *\n"), ++i;
			if (i < 3)
			{
				printf("*     Promjena imena (opcija 1)                   *\n");
				printf("*     Promjena korisnickog imena (opcija 2)       *\n");
				printf("*     Promjena pin koda (opcija 3)                *\n");
				i += 3;
			}
		}
		char op = biranjeOpcija(3);
		switch (op)
		{
		case '1':
			printf("Unesite novo ime: ");
			free(kljuc->ime), free(kljuc->prezime);
			strcpy(novoIme, provjeraImena(novoIme,2));
			kljuc->ime = (char*)calloc(strlen(novoIme) + 1, sizeof(char));
			strcpy(kljuc->ime, novoIme);
			printf("Unesite novo prezime: ");
			strcpy(novoIme, provjeraImena(novoIme, 2));
			kljuc->prezime = (char*)calloc(strlen(novoIme) + 1, sizeof(char));
			strcpy(kljuc->prezime, novoIme);
			printf("Novo ime korisnika: %s %s\n", kljuc->ime, kljuc->prezime);
			free(novoIme);
			break;
		case '2':
			printf("Unesite novo korisnicko ime: ");
			free(kljuc->korIme);
			strcpy(novoIme, utvrdjivanjeImena(novoIme));
			strcpy(*korIme, provjeraIspravnosti(glava, rep, novoIme));
			kljuc->korIme = (char*)calloc(strlen(*korIme) + 1, sizeof(char));
			strcpy(kljuc->korIme,*korIme);
			free(novoIme);
			printf("Novo korisnicko ime: %s\n", *korIme);
			break;
		case '3':
			printf("Unesite novi pin kod: ");
			sakrivanjePina(pin);
			strcpy(pin, provjeraPina(pin));
			kljuc->pin = atoi(pin);
			free(pin);
			printf("Uspjesno ste promijenitili pin kod!\n");
			break;
		}
	}
}

int blokiranjeKorisnika(LISTA *glava, LISTA *rep, char *korIme)
{
	KORISNIK *kljuc = pretraga(glava, rep, korIme, -1);
	if (strcmp(korIme, SUPER_USER) == 0)
		return -1;
	else if (kljuc->aktivan == 'a')
	{
		kljuc->aktivan = 'b';
		return 1;
	}
	else
	{
		kljuc->aktivan = 'a';
		return 0;
	}
}

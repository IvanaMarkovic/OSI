#include "racun.h"
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#define _NO_CRT_STDIO_INLINE

int maxBrojProizvoda = 20;

void citajRacun()
{
	DIR *folderRacuni;
	struct dirent *racun;
	folderRacuni = opendir("Racun");

	if (folderRacuni != NULL)
	{
		while ((racun = readdir(folderRacuni)) != NULL)
		{
			if ((strcmp(racun->d_name, ".") == 0 || strcmp(racun->d_name, "..") == 0) || racun->d_name[0] == '-')
			{}
			else
			{
				char *noviRacun=(char*)calloc(50,sizeof(char));
				strcpy(noviRacun, "Racun");
				strcat(noviRacun, "\\");
				strcat(noviRacun, racun->d_name);
				if(noviRacun[strlen(noviRacun)-1] == 't')
					ucitavanjePodatakaSaRacunaTxt(noviRacun);
				else
					ucitavanjePodatakaSaRacunaCsv(noviRacun);
				free(noviRacun);
			}
		}
		closedir(folderRacuni);
	}
}

void ucitavanjePodatakaSaRacunaTxt(char *nazivRacuna)
{
	FILE *noviRacun;
	RACUN racun;
	racun.proizvodi = (PROIZVOD*)calloc(maxBrojProizvoda, sizeof(PROIZVOD));
	char pomocnaRijec[50] = { 0 }, *nazivProizvoda, *kupac, datum[11];
	nazivProizvoda = (char*)calloc(50, sizeof(char));
	kupac = (char*)calloc(50, sizeof(char));
	PROIZVOD pomocniProizvod;
	pomocniProizvod.sifraProizvoda = (char*)calloc(50, sizeof(char));
	double pdv, ukupno;
	int ucitaniPodaci = 0;
	if ((noviRacun = fopen(nazivRacuna, "r")) != NULL)
	{
		char pomocnaRijec[50] = { 0 };
		while (citanjeRijeciIzFajla(noviRacun, pomocnaRijec) != EOF)
		{
			if (strcmp(pomocnaRijec, "Kupac") == 0)
			{
				fscanf(noviRacun, " %s", kupac);
				racun.kupac = (char*)calloc(strlen(kupac) + 1, sizeof(char));
				strcpy(racun.kupac, kupac);
				++ucitaniPodaci;
			}
			else if (strcmp(pomocnaRijec, "Datum") == 0)
			{
				fscanf(noviRacun, " %s", datum);
				strcpy(racun.datum, datum);
				++ucitaniPodaci;
			}
			else if (strcmp(pomocnaRijec, "PDV") == 0)
			{
				fscanf(noviRacun, " %lf", &pdv);
				racun.PDVvrijednost = pdv;
				++ucitaniPodaci;
			}
			else if (strcmp(pomocnaRijec, "placanje") == 0)
			{
				fscanf(noviRacun, " %lf", &ukupno);
				racun.ukupanIznos = ukupno;
				++ucitaniPodaci;
			}
			else if (strcmp(pomocnaRijec, "Racun") == 0 && ucitaniPodaci == 2)
				ucitavanjeProizvodaFormat1(noviRacun, &pomocniProizvod, &racun, nazivProizvoda);
			else if ((strcmp(pomocnaRijec, "Proizvod") == 0 && ucitaniPodaci == 1) ||
				(strcmp(pomocnaRijec, "Market") == 0 && ucitaniPodaci == 2))
				ucitavanjeProizvodaFormat24(noviRacun, &pomocniProizvod, &racun, nazivProizvoda);
			else if (strcmp(pomocnaRijec, "racun") == 0 && ucitaniPodaci == 2)
				ucitavanjeProizvodaFormat3(noviRacun, &pomocniProizvod, &racun, nazivProizvoda);
		}
		fclose(noviRacun);
		if (!verifikacijaRacuna(&racun, nazivRacuna))
		{
			smjestanjeUArhivu(nazivRacuna);
			rasporedjivanjeRacuna(&racun);
		}
	}
	else
		printf("Greska pri otvaranju racuna!\n");
}

void ucitavanjePodatakaSaRacunaCsv(char *nazivRacuna)
{
	FILE *noviRacun;
	char pomocnaRijec[50] = { 0 }, proizvod[50] = { 0 };
	PROIZVOD pomocniProizvod;
	pomocniProizvod.sifraProizvoda = (char*)calloc(50, sizeof(char));
	RACUN racun;
	racun.proizvodi = (PROIZVOD*)calloc(maxBrojProizvoda, sizeof(PROIZVOD));
	odredjivanjePodatakaNaRacunu(&racun, nazivRacuna);
	racun.brojProizvoda = 0;
	double ukupno = 0;
	if ((noviRacun = fopen(nazivRacuna, "r")) != NULL)
	{
		while(citanjeRijeciIzFajla(noviRacun,pomocnaRijec)!=EOF)
			if (strcmp(pomocnaRijec, "Ukupno") == 0)
			{
				while (fscanf(noviRacun, "%s\n", proizvod) == 1)
				{
					utvrdjivanjeProizvoda(&pomocniProizvod, proizvod);
					ucitavanjeProizvoda(&racun, &pomocniProizvod,"", racun.brojProizvoda++);
				}
			}
		fclose(noviRacun);
	}
	for (int i = 0; i < racun.brojProizvoda; i++)
		ukupno += racun.proizvodi[i].vrijednost;
	racun.PDVvrijednost = (ukupno * 17) / 100;
	racun.ukupanIznos = racun.PDVvrijednost + ukupno;
	if (!verifikacijaRacuna(&racun, nazivRacuna))
	{
		smjestanjeUArhivu(nazivRacuna);
		rasporedjivanjeRacuna(&racun);
	}
}

int citanjeRijeciIzFajla(FILE *racun, char *rijec)
{
	int i = 0;
	char c;
	while ((c = fgetc(racun)) != EOF)
	{
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c=='-')
			rijec[i++] = c;
		else if (i > 0)
			break;
	}
	rijec[i] = '\0';
	return i > 0 ? 0 : EOF;
}

void ucitavanjeProizvodaFormat1(FILE *noviRacun, PROIZVOD *pomocniProizvod, RACUN *racun, char *nazivProizvoda)
{
	racun->brojProizvoda = 0;
	char pomocnaRijec[50] = { 0 };
	while (strcmp(pomocnaRijec, "ukupno") != 0)
		citanjeRijeciIzFajla(noviRacun, pomocnaRijec);
	while ((fscanf(noviRacun, "%s %s  -    %lf    -    %lf   -   %lf\n", nazivProizvoda, pomocniProizvod->sifraProizvoda,
		&pomocniProizvod->kolicina, &pomocniProizvod->cijena, &pomocniProizvod->vrijednost) == 5))
		ucitavanjeProizvoda(racun, pomocniProizvod, nazivProizvoda, racun->brojProizvoda++);
}

void ucitavanjeProizvodaFormat24(FILE *noviRacun, PROIZVOD *pomocniProizvod, RACUN *racun, char *nazivProizvoda)
{
	racun->brojProizvoda = 0;
	char pomocnaRijec[50] = { 0 };
	while (strcmp(pomocnaRijec, "---------------------------------------") != 0)
		citanjeRijeciIzFajla(noviRacun, pomocnaRijec);
	while ((fscanf(noviRacun, "%s %s  -    %lf    -    %lf   -   %lf\n", nazivProizvoda, pomocniProizvod->sifraProizvoda,
		&pomocniProizvod->kolicina, &pomocniProizvod->cijena, &pomocniProizvod->vrijednost) == 5))
		ucitavanjeProizvoda(racun, pomocniProizvod, nazivProizvoda, racun->brojProizvoda++);
}

void ucitavanjeProizvodaFormat3(FILE *noviRacun, PROIZVOD *pomocniProizvod, RACUN *racun, char *nazivProizvoda)
{
	racun->brojProizvoda = 0;
	char pomocnaRijec[50] = { 0 }, proizvod[50] = { 0 };
	while (strcmp(pomocnaRijec, "----------------------------------") != 0)
		citanjeRijeciIzFajla(noviRacun, pomocnaRijec);
	while ((fscanf(noviRacun, "%s %s\n", nazivProizvoda, proizvod) == 2) && strcmp(nazivProizvoda,"----------------------------------")!=0)
	{
		utvrdjivanjeProizvoda(pomocniProizvod, proizvod);
		ucitavanjeProizvoda(racun, pomocniProizvod, nazivProizvoda, racun->brojProizvoda++);
	}
}

void rasporedjivanjeRacuna(RACUN *racun)
{
	char *folder=(char*)calloc(10,sizeof(char));
	strcpy(folder, "Kupci");
	kreiranjePotrebnihFajlova(racun, folder, racun->kupac);
	strcpy(folder, "Proizvodi");
	for (int i = 0; i < racun->brojProizvoda; i++)
		kreiranjePotrebnihFajlova(racun, folder, racun->proizvodi[i].sifraProizvoda);
	strcpy(folder, "Mjesec");
	kreiranjePotrebnihFajlova(racun, folder, odredjivanjeMjeseca(racun->datum));
}

void smjestanjeUArhivu(char *nazivRacuna)
{
	char noviNaziv[50] = { 0 }, nazivArhiva[50] = { 0 }, *naziv=(char*)calloc(10,sizeof(char));
	strcpy(naziv,"Racun");
	odredjivanjeNazivaFajla(nazivRacuna, noviNaziv, naziv);
	strcpy(naziv, "Arhiva");
	odredjivanjeNazivaFajla(nazivRacuna, nazivArhiva, naziv);
	rename(nazivRacuna, noviNaziv);
	FILE *racun=NULL, *arhiva=NULL;
	if ((racun = fopen(noviNaziv, "r")) != NULL)
	{
		if ((arhiva = fopen(nazivArhiva, "w")) != NULL)
		{
			char *pomocniNiz = (char*)calloc(50, sizeof(char));
			while (fgets(pomocniNiz, 50, racun))
				fputs(pomocniNiz, arhiva);
			free(pomocniNiz);
			fclose(arhiva);
		}
		fclose(racun);
	}
}

void ucitavanjeProizvoda(RACUN *racun, PROIZVOD *pomocniProizvod, char *nazivProizvoda, int brojElemenata)
{
	if (brojElemenata == maxBrojProizvoda)
		racun->proizvodi = (PROIZVOD*)realloc(racun->proizvodi, (2 * maxBrojProizvoda) * sizeof(PROIZVOD));
	racun->proizvodi[brojElemenata].sifraProizvoda = (char*)calloc(strlen(nazivProizvoda) + strlen(pomocniProizvod->sifraProizvoda) + 1, sizeof(char));
	strcpy(racun->proizvodi[brojElemenata].sifraProizvoda, nazivProizvoda);
	strcat(racun->proizvodi[brojElemenata].sifraProizvoda, pomocniProizvod->sifraProizvoda);
	racun->proizvodi[brojElemenata].kolicina = pomocniProizvod->kolicina;
	racun->proizvodi[brojElemenata].cijena = pomocniProizvod->cijena;
	racun->proizvodi[brojElemenata].vrijednost = pomocniProizvod->vrijednost;
}

void utvrdjivanjeProizvoda(PROIZVOD *noviProizvod, char *proizvod)
{
	char kolicina[10] = { 0 }, cijena[10] = { 0 }, vrijednost[10] = { 0 };
	for (unsigned i = 0, j = 0, k = 0; i < strlen(proizvod); i++)
	{
		if (proizvod[i] != '=' && proizvod[i]!= ',')
			switch (j)
			{
			case 0:
				noviProizvod->sifraProizvoda[k++] = proizvod[i];
				break;
			case 1:
				kolicina[k++] = proizvod[i];
				break;
			case 2:
				cijena[k++] = proizvod[i];
				break;
			case 3:
				vrijednost[k++] = proizvod[i];
				break;
			}
		else if ((proizvod[i] == '=' && proizvod[i - 1] >= '0' && proizvod[i - 1] <= '9') || proizvod[i]==',')
			++j, k = 0;

	}
	noviProizvod->kolicina = atof(kolicina);
	noviProizvod->cijena = atof(cijena);
	noviProizvod->vrijednost = atof(vrijednost);
}

void odredjivanjePodatakaNaRacunu(RACUN *racun, char *nazivRacuna)
{
	racun->kupac = (char*)calloc(50,sizeof(char));
	char datum[11] = { 0 };
	for (unsigned i = 0, j = 0; i < strlen(nazivRacuna)-4; i++)
	{
		if (nazivRacuna[i] == '\\')
			while (nazivRacuna[i+1] != '_')
				racun->kupac[j++] = nazivRacuna[++i];
		if (nazivRacuna[i] == '_')
		{
			j = 0;
			while (i < strlen(nazivRacuna) - 5)
			{
				datum[j] = nazivRacuna[++i];
				if (datum[j++] == '.')
					datum[j-1] = '/';
			}
		}
	}
	strcpy(racun->datum, datum);
}

void odredjivanjeNazivaFajla(char *nazivRacuna, char *nazivFajla, char *nazivFoldera)
{
	strcpy(nazivFajla, nazivFoldera);
	int j = strlen(nazivFoldera)+1;
	if (strcmp(nazivFoldera, "Racun") == 0)
	{
		strcat(nazivFajla, "\\-");
		++j;
	}
	else
		strcat(nazivFajla, "\\");
	for (unsigned i = 0; i < strlen(nazivRacuna); i++)
		if (nazivRacuna[i] == '\\')
			while (i < strlen(nazivRacuna))
				nazivFajla[j++] = nazivRacuna[++i];
	if (j == strlen(nazivFoldera) + 1)
	{
		strcat(nazivFajla, nazivRacuna);
		strcat(nazivFajla, ".txt");
	}
}

char* odredjivanjeMjeseca(char *datum)
{
	char brojMjeseca[3] = { 0 };
	int mjesec;
	for (unsigned i = 0, j = 0; i < strlen(datum); i++)
		if (datum[i++] == '/')
			while (datum[i] != '/')
				brojMjeseca[j++] = datum[i++];
	mjesec = atoi(brojMjeseca);
	switch (mjesec)
	{
	case 1: return "januar";
	case 2: return "februar";
	case 3: return "mart";
	case 4: return "april";
	case 5: return "maj";
	case 6: return "juni";
	case 7: return "juli";
	case 8: return "avgust";
	case 9: return "septembar";
	case 10: return "oktobar";
	case 11: return "novembar";
	case 12: return "decembar";
	default: return "";
	}
}

void kreiranjePotrebnihFajlova(RACUN *racun, char *nazivFoldera, char *nazivFajla)
{
	char *fajl = (char*)calloc(50, sizeof(char));
	strcpy(fajl, nazivFoldera);
	strcat(fajl, "\\");
	strcat(fajl, nazivFajla);
	strcat(fajl, ".txt");
	FILE *fajlZaUpis = fopen(fajl, "a");
	if(nazivFoldera[0]=='K')
		upisKupca(fajlZaUpis, racun);
	else if(nazivFoldera[0] == 'P')
		upisProizvoda(fajlZaUpis, racun, nazivFajla);
	else if (nazivFoldera[0] == 'M')
		upisMjeseca(fajlZaUpis, racun);
	fclose(fajlZaUpis);
}

int verifikacijaRacuna(RACUN *racun, char *nazivRacuna)
{
	char *fajlZaGreske=(char*)calloc(50, sizeof(char));
	odredjivanjeNazivaFajla(nazivRacuna, fajlZaGreske, "Arhiva");
	fajlZaGreske[strlen(fajlZaGreske) - 4] = '\0';
	strcat(fajlZaGreske, "_error.txt");
	FILE *greska=NULL;
	double ukupnoBezPDVa = 0;
	for (int i = 0; i < racun->brojProizvoda; i++)
	{
		ukupnoBezPDVa += racun->proizvodi[i].vrijednost;
		if (verifikacijaProizvoda(&racun->proizvodi[i]) && (greska = fopen(fajlZaGreske, "a")) != NULL)
			fprintf(greska, "Ukupna vrijednost proizvoda (%s) je pogresna! Ispravna vrijednost je %8.3lf!\n",
				racun->proizvodi[i].sifraProizvoda, racun->proizvodi[i].kolicina*racun->proizvodi[i].cijena);
	}
	if (verifikacijaPDVa(ukupnoBezPDVa, racun->PDVvrijednost) && (greska != NULL || (greska = fopen(fajlZaGreske, "a")) != NULL))
		fprintf(greska, "Ukupna PDV vrijednost na racunu je pogresna! Ispravna vrijednost je %8.3lf!\n",
			ukupnoBezPDVa * 17 / 100);
	else if (verifikacijaUkupnogIznosa(ukupnoBezPDVa, racun->PDVvrijednost, racun->ukupanIznos) && (greska != NULL || (greska = fopen(fajlZaGreske, "a")) != NULL))
		fprintf(greska, "Ukupna vrijednost za placanje je pogresna! Ispravna vrijednost je %8.3lf!\n",
			ukupnoBezPDVa + racun->PDVvrijednost);
	if (greska == NULL)
		return 0;
	else
	{
		fclose(greska);
		return 1;
	}
}

int verifikacijaProizvoda(PROIZVOD *proizvod)
{
	if (proizvod->kolicina * proizvod->cijena == proizvod->vrijednost)
		return 0;
	return 1;
}

int verifikacijaPDVa(double ukupno, double pdv)
{
	if ((ukupno * 17) / 100 == pdv)
		return 0;
	return 1;
}

int verifikacijaUkupnogIznosa(double ukupno, double pdv, double ukupanIznos)
{
	if ((ukupno + pdv) == ukupanIznos)
		return 0;
	return 1;
}

void upisKupca(FILE *fajlZaUpis, RACUN *racun)
{
	fprintf(fajlZaUpis, "(%d %s %lf %lf)\n", racun->brojProizvoda, racun->datum, racun->PDVvrijednost, racun->ukupanIznos);
	for (int i = 0; i < racun->brojProizvoda; i++)
		fprintf(fajlZaUpis, "%s %lf %lf %lf\n", racun->proizvodi[i].sifraProizvoda,
			racun->proizvodi[i].kolicina, racun->proizvodi[i].cijena, racun->proizvodi[i].vrijednost);
}

void upisProizvoda(FILE *fajlZaUpis, RACUN *racun, char *proizvod)
{
	for (int j = 0; j < racun->brojProizvoda; j++)
		if (strcmp(racun->proizvodi[j].sifraProizvoda, proizvod) == 0)
			fprintf(fajlZaUpis, "%s %s %lf %lf %lf %lf\n", racun->kupac, racun->datum,
				racun->proizvodi[j].kolicina, racun->proizvodi[j].cijena, racun->proizvodi[j].vrijednost,
				(racun->proizvodi[j].vrijednost + (racun->proizvodi[j].vrijednost * 17 / 100)));
}

void upisMjeseca(FILE *fajlZaUpis, RACUN *racun)
{
	fprintf(fajlZaUpis, "%s %s %lf %lf %lf\n", racun->kupac, racun->datum,
		racun->ukupanIznos - racun->PDVvrijednost, racun->PDVvrijednost, racun->ukupanIznos);
}

void prikazFajla(char *fajl, int opcija)
{
	DIR *folder=NULL;
	struct dirent *pomocniFajl;
	char *nazivFajla = (char*)calloc(50, sizeof(char));
	if (opcija == 1)
		folder = opendir("Kupci");
	else if(opcija == 2)
		folder = opendir("Proizvodi");
	else if (opcija == 3)
		folder = opendir("Mjesec");
	odredjivanjeNazivaFajla(fajl, nazivFajla, (opcija == 1 ? "Kupci" : (opcija == 2 ? "Proizvodi" : "Mjesec")));
	int n = 0, brojFajlova = 20;
	char **sviFajlovi = (char**)calloc(brojFajlova, sizeof(char*));
	if (folder != NULL)
	{
		while ((pomocniFajl = readdir(folder)) != NULL)
		{
			if ((strcmp(pomocniFajl->d_name, ".") == 0 || strcmp(pomocniFajl->d_name, "..") == 0))
			{}
			else
			{
				if (n == brojFajlova)
					sviFajlovi = (char**)realloc(sviFajlovi, (brojFajlova *= 2) * sizeof(char*));
				pomocniFajl->d_name[strlen(pomocniFajl->d_name) - 4] = '\0';
				sviFajlovi[n] = (char*)calloc(strlen(pomocniFajl->d_name) + 1, sizeof(char));
				strcpy(sviFajlovi[n++], pomocniFajl->d_name);
			}
		}
		closedir(folder);
	}
	for (int i = 0, j=0, brojac = 0; i < n; i++)
	{
		if (strcmp(sviFajlovi[i], fajl) == 0)
		{
			FILE *pregled = fopen(nazivFajla, "r");
			opcija == 1 ? pregledKupaca(pregled) : (opcija == 2 ? pregledProizvoda(pregled) : mjesecniPregled(pregled));
			break;
		}
		else brojac++;
		if (brojac == n && j<3)
		{
			printf("Trazeni fajl ne postoji na sistemu!\nMolimo Vas da uneste ponovo: ");
			scanf("%s", fajl);
			odredjivanjeNazivaFajla(fajl, nazivFajla, (opcija == 1 ? "Kupci" : (opcija == 2 ? "Proizvodi" : "Mjesec")));
			brojac = 0;
			j++;
			i = -1;
			if (j == 3)
				printf("Moguce je unijeti pogresan naziv tri puta!\n");
		}
	}
}

void pregledKupaca(FILE *kupac)
{
	printf("\n");
	char *sifra = (char*)calloc(50, sizeof(char)), datum[11] = { 0 };
	double pdv, ukupanIznos, kolicina, cijena, vrijednost;
	int i = 1, brojProizvoda;
	while (fscanf(kupac, "(%d %s %lf %lf)\n", &brojProizvoda, datum, &pdv, &ukupanIznos) == 4)
	{
		printf("Datum: %s", datum);
		printf("\n=== =============== ========== ========== ==========\n");
		printf("RB. SIFRA PROIZVODA  KOLICINA    CIJENA   VRIJEDNOST\n");
		printf("=== =============== ========== ========== ==========\n");
		while (i <= brojProizvoda)
		{
			fscanf(kupac, "%s %lf %lf %lf\n", sifra, &kolicina, &cijena, &vrijednost);
			printf("%02d. %-15s %10.3lf %10.3lf %10.3lf\n", i++, sifra, kolicina, cijena, vrijednost);
		}
		printf("=== =============== ========== ========== ==========\n");
		printf("Ukupno bez PDV-a: %7.3lf\n", ukupanIznos-pdv);
		printf("Ukupna vrijednost PDV-a: %7.3lf\n", pdv);
		printf("Ukupno za placanje: %7.3lf\n\n", ukupanIznos);
		i = 1;
	}
}

void pregledProizvoda(FILE *proizvod)
{
	char *kupac = (char*)calloc(50, sizeof(char)), datum[11] = { 0 };
	double vrijednost, ukupanIznos, kolicina, cijena;
	int i = 1;
	printf("=== =========== ========== ========== ========== ========== ==========\n");
	printf("RB     KUPAC      DATUM     KOLICINA    CIJENA   VRIJEDNOST   UKUPNO\n");
	printf("=== =========== ========== ========== ========== ========== ==========\n");
	while(fscanf(proizvod,"%s %s %lf %lf %lf %lf\n", kupac, datum, &kolicina, &cijena, &vrijednost, &ukupanIznos)==6)
		printf("%02d. %-11s %-10s %10.3lf %10.3lf %10.3lf %10.3lf\n", i++, kupac, datum, kolicina, cijena, vrijednost, ukupanIznos);
	printf("=== =========== ========== ========== ========== ========== ==========\n");
}

void mjesecniPregled(FILE *mjesec)
{
	char *kupac = (char*)calloc(50, sizeof(char)), datum[11] = { 0 };
	double ukupnoBezPDVa, pdv, ukupanIznos;
	int i = 1;
	printf("=== =========== ========== ========== ========== ================\n");
	printf("RB     KUPAC      DATUM      UKUPNO   UKUPNO PDV UKUPNO SA PDV-om\n");
	printf("=== =========== ========== ========== ========== ================\n");
	while (fscanf(mjesec, "%s %s %lf %lf %lf\n", kupac, datum, &ukupnoBezPDVa, &pdv, &ukupanIznos) == 5)
		printf("%02d. %-10s  %-10s %10.3lf %10.3lf %16.3lf\n", i++, kupac, datum, ukupnoBezPDVa, pdv, ukupanIznos);
	printf("=== =========== ========== ========== ========== ================\n");
}

 #include <stdlib.h>  
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "korisnik.h"
#include "racun.h"

int main()
{
	LISTA *glava=0, *rep=0;
	listaKorisnika(&glava,&rep);
	dobrodosli(glava,rep);
	upisKorisnikaUFajl(&glava, &rep);
	printf("Hvala Vam za koristenje sistema za analizu troskova!\n");
	Sleep(2000);
	return 0;
}
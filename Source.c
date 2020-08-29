#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//NIZ KOJI CUVA KALORIJSKI CILJ I KILAZU
static double niz[2];

//LISTA OBROKA
typedef struct listaObroka* PLISTAOBROKA;

typedef struct listaObroka {
	int kalorije;
	char nazivObroka[20];
	PLISTAOBROKA sledeci;
}LISTAOBROKA;

//PARAMETRIZACIJA FUNKCIJA
void unesiUListu(PLISTAOBROKA *glava, char nazivObroka[20], int kalorije);
void prikaziListuiUpisiUDokument(PLISTAOBROKA glava);
void unesiObrok(PLISTAOBROKA* glava);
void glavniMeni();
void datum();
void prikaziTabelu();
void kreirajDnevnik();
void izbrisiTotal();
int ukupnoCeoDan();
double kilaza();
double kalorijskiCilj();
FILE* fajlZaKilazuIKalorijskiCilj();
void nizTezinaKalorije(double niz[]);

int main(void) {

	glavniMeni();

	return 0;
}

//Funkcija za unos obroka u listu
void unesiUListu(PLISTAOBROKA *glava, char nazivObroka[20], int kalorije) {
	PLISTAOBROKA novi = malloc(sizeof(LISTAOBROKA));
	strcpy(novi->nazivObroka, nazivObroka);
	novi->kalorije = kalorije;
	novi->sledeci = NULL;
	if (*glava == NULL) {
		*glava = novi;
		return;
	}
	PLISTAOBROKA pom = *glava;
	while (pom->sledeci != NULL) {
		pom = pom->sledeci;
	}
	pom->sledeci = novi;
}

//Otvaranje fajla
FILE *otvoriDokument() {
	FILE * fajl = fopen("dnevnikIshrane.txt", "a");

	if (fajl == NULL) {
		printf("Greska");
		return NULL;
	}
	return fajl;
}

//Citanje fajla
FILE *citajDokument() {
	FILE * fajl = fopen("dnevnikIshrane.txt", "r");
	if (fajl == NULL) {
		printf("Greska");
		return NULL;
	}
	return fajl;
}

//Prikazivanje unesenih obroka tokom dana i upisivanje u dnevnik ishrane
void prikaziListuiUpisiUDokument(PLISTAOBROKA glava) {
	int x;
	FILE * fajl = otvoriDokument();
	FILE * total = fopen("total.txt", "a+");
	if (fajl == NULL || total == NULL) {
		printf("Greska sa fajlom.");
		exit(1);
	}
	printf("\nObrok%-14sKalorije\n", " ");
	printf("----------------------------\n");
	PLISTAOBROKA pom = glava;
	while (pom != NULL) {
		fprintf(fajl, "%-19s%d\n", pom->nazivObroka, pom->kalorije);
		printf("%-19s%d\n", pom->nazivObroka, pom->kalorije);
		pom = pom->sledeci;
	}
	int ukupno = izracunajUkupno(glava);

	fprintf(stdout, "----------------------------\n", " ");
	fprintf(stdout, "Ukupno uneto: %-5s%d\n", " ", ukupno);
	
	printf("Da li ste zavrsili sa danom (1 - da, 0 - ne): ");
	scanf("%d", &x);
	if (x != 1) {
		fprintf(total, "%d\n", ukupno);
	}
	if (x == 1) {
		double suma = ukupnoCeoDan()+ukupno;
		nizTezinaKalorije(niz);
		double ocekivanaKilaza = niz[0] - ((2500.0 - suma) / 1000.0);
		fprintf(fajl, "---------------------------\n", " ");
		fprintf(fajl, "Ukupno uneto: %-5s%.0lf kcal\n", " ", suma);
		fprintf(fajl, "---------------------------\n", " ");
		fprintf(fajl, "Kalorijski cilj: %.0lf kcal\nTrenutna tezina: %.1lf kg\nOcekivana tezina za 7 dana: %.1lf kg\n", niz[1], niz[0], ocekivanaKilaza);
		izbrisiTotal();
		datum();
		return;
	}

	printf("\n");
	fclose(fajl);
	fclose(total);
}

//Brisanje ukupnih kalorija na kraju dana
void izbrisiTotal() {
	FILE * total = fopen("total.txt", "w");
	if (total == NULL) {
		printf("Greska sa fajlom!");
		return;
	}
	fclose(total);
}

//Racunanje ukupnog unosa obroka u nekom delu dana preko liste
int izracunajUkupno(PLISTAOBROKA glava) {
	
	int ukupno = 0;	
	FILE * fajl = otvoriDokument();

	while (glava != NULL) {
		ukupno = ukupno + glava->kalorije;
		glava = glava->sledeci;
	}
	return ukupno;
}

//Ubacivanje kalorijskog cilja i tezine u niz
void nizTezinaKalorije(double niz[]) {
	FILE * fajl = fopen("kalorije_kilaza.txt", "r+");
	if (fajl == NULL) {
		printf("Greska sa fajlom!");
		return;
	}

	for (int i = 0; i < 2; i++) {
		fscanf(fajl, "%lf", &niz[i]);
	}

	fclose(fajl);
}

//Citanje ukupnih kalorija unesenih obroka tokom dana iz fajla
int ukupnoCeoDan() {
	FILE * total = fopen("total.txt", "r+");
	
	if (total == NULL) {
		return;
	}

	int suma = 0;
	int niz[8];
	
	for (int i = 0; i < 8; i++) {
		fscanf(total, "%d", &niz[i]);
	}
	for (int j = 0; j < 8; j++) {
		if (niz[j] > 0) {
			suma = suma + niz[j];
		}
	}

	fclose(total);

	return suma;
}

//Generisanje upita za unost obroka u programu i pozivanje f-je za unos obroka u listu
void unesiObrok(PLISTAOBROKA *glava) {
	int x;
	char nazivObroka[20];
	int kalorije;
	do {
		printf("\nUpisite obrok: ");
		gets(nazivObroka);
		printf("\nUpisite kalorije upisane hrane: ");
		scanf("%d", &kalorije);
		unesiUListu(glava, nazivObroka, kalorije);
		printf("\nDa li zelite da nastavite sa unosom hrane (1 - da, 0 - ne): ");
		scanf("%d", &x);
		getchar();
	} while (x != 0);

	prikaziListuiUpisiUDokument(*glava);
}

//Dobijanje datuma za unesene obroke tokom datog dana
void datum() {
	
	time_t trenutnoVreme;
	char* konvertovanoVreme;

	trenutnoVreme = time(NULL);

	konvertovanoVreme = ctime(&trenutnoVreme);

	FILE * fajl = otvoriDokument();
	if (fajl == NULL) {
		exit(1);
	}
	fprintf(fajl, "---------------------------\n", " ");
	fprintf(fajl, "%-13s\n", konvertovanoVreme);
	fprintf(fajl, "----------------------------\n");
	fprintf(fajl, "Obrok%-14sKalorije\n", " ");
	fprintf(fajl, "----------------------------\n");
}

//Prikazivanje tabele unesenih obroka, kalorija i kilaze
void prikaziTabelu() {
	FILE *fajl = citajDokument();
	if (fajl == NULL) {
		printf("Fajl je prazan!\n");
		return NULL;
	}
	char x;
	while (!feof(fajl)) {
		x = fgetc(fajl);
		printf("%c", x);
	}
	fclose(fajl);
}

//Generisanje i brisanje dnevnik fajla
void kreirajDnevnik() {
	int x;

	printf("\nUkoliko imate vec kreirani dnevnik, ova opcija ce Vam obrisati "
		"sve unete podatke do sada!\n\n");
	printf("Da li zelite da nastavite (1 - da, 0 - ne): ");
	scanf("%d", &x);

	if (x == 1) {
		FILE *fajl = fopen("dnevnikIshrane.txt", "w");
		if (fajl == NULL) {
			printf("Greska!\n");
			return;
		}
		fprintf(fajl, "Obrok%-14sKalorije\n", " ");
		fprintf(fajl, "----------------------------\n");

		printf("\nUspesno ste kreirali novi fajl!\n");

		fclose(fajl);
	}
}

//Unos kilaze i vracanje double vrednosti
double kilaza() {	
	double x;
	
	printf("\nUpisite Vasu kilazu na danasnji dan: ");
	scanf("%lf", &x);
	getchar();

	return x;
}

//Unos kalorijskog cilja i vracanje double vrednosti
double kalorijskiCilj() {
	double x;

	printf("\nUpisite Vas dnevni kalorijski cilj: ");
	scanf("%lf", &x);
	getchar();

	return x;
}

//Generisanje fajla za kalorijski cilj i kilazu
FILE* fajlZaKilazuIKalorijskiCilj() {
	FILE * fajl = fopen("kalorije_kilaza.txt", "w+");
	
	if (fajl == NULL) {
		printf("Greska sa fajlom!");
		return;
	}

	double x = kilaza();
	double y = kalorijskiCilj();

	fprintf(fajl, "%lf\n", x);
	fprintf(fajl, "%lf\n", y);

	fclose(fajl);

	return fajl;
}

//Funkcija za generisanje glavnog menija
void glavniMeni() {

	
	PLISTAOBROKA glava = NULL;
	int x;

	do {
		printf("--------------------------------------------\n");
		printf("DOBRODOSLI U GLAVNI MENI DNEVNIKA ISHRANE\n");
		printf("--------------------------------------------\n");
		printf("0. Kreiraj/obrisi dnevnik\n");
		printf("1. Unesite obrok\n");
		printf("2. Upisite Vasu kilazu i kalorijski cilj\n");
		printf("3. Prikazite tabelu ishrane po danima\n");
		printf("4. Izadjite iz programa\n");
		printf("\nUnesite opciju: ");
		scanf("%d", &x);
		getchar();
		switch (x) {
		case 0: kreirajDnevnik(); printf("\n");
			break;
		case 1: unesiObrok(&glava); printf("\n");
			break;
		case 2: fajlZaKilazuIKalorijskiCilj(); printf("\n");
			break;
		case 3: printf("\n"); prikaziTabelu(); printf("////////KRAJ TABELE////////\n\n");
			break;
		case 4: printf("\nIzasli ste iz programa...\n\n");
			break;
		default: printf("\nNiste uneli validnu opciju!\n");
		}

	} while (x != 4);
}

#ifndef FILESYS_H
#define FILESYS_H

#include "comon.h"


typedef unsigned char uint8_t;
typedef short int uint16_t;
typedef long int uint32_t;


typedef struct idxt idxt;
typedef struct iofile iofile;

struct idxt
{
	uint8_t type;		// type des données 
	uint8_t dim;		// nombre de dimensions
	int *dimt;			// tableau de nombre de valeurs de chaque dimension
	
	int num;			// nombre de data
	
	uint8_t *array;		// table des labels (tout est rangé en un seul vecteur ici)
	
};

struct iofile
{
	int pop;			// taille de la population
	int idim;			// dimension du vecteur des entrées
	int odim;			// dimension du vecteur des sorties
	
	
	double **iv;		// vecteur des entrées
	double **ov;		// vecteur des sorties
	
	
};


idxt *load_idxfile(char fname[60]);			// charge un fichier idx
double **vectorize_idxfile(idxt *iv);		// retourne une matrice de double

iofile *create_iofile(char ifn[60], char ofn[60]);

void free_idxf(idxt *ix);
void free_iovf(iofile *ix);



// ATTENTION // issu d'une anciène implémentation.
//Les deux fonctions ci-dessous ne sont absolument généralisables

double *classify_table(double val);		// transforme un entier en vecteur 
double get_real_val(double *tab);		// transforme un vecteur en double

void printa_test(double *tab, int wi, int he, int o);
void print_test(idxt *iv, idxt *ov, int o);





#endif

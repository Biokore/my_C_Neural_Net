#ifndef NETBUILD_H
#define NETBUILD_H

#include "comon.h"


typedef struct network network;
typedef struct layer layer;

struct network
{
	int ivs;			// input vector size
	int ovs;			// output vector size
	int lnb;			// nombre de couches
	
	layer *lay;			// tableau de layer
	
	double *niv;		// network input values
	double *nov;		// network output vector
	
	double alfa;		// valeur alfa
	double err;			// erreur de test du réseau
};

struct layer
{
	int ivs;
	int knb;
	
	int aft;			// agregation function type
	int pft;			// propagation function type
	
	double bias;		// biais de la couche
	
	double **win;		// tableau de poids par noyau
	double *afv;		// tableau de valeurs d'agrégation
	double *pfv;		// tableau de valeurs de propagation
	double *delta;		// tableau de deltas
};


network *build_network(int ivs, int ovs);
void add_layer(network *n, int knb, int aft, int pft, double bias);
void activate_network(network *n);

void free_layer(layer *l);
void free_network(network *net);


/* ATTENTION : Anciène implémentation de 
 * chargement et sauvegarde du réseau */

network *load_network(char netname[60]);
void save_network(network *n, char name[32]);




#endif

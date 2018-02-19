#include "netbuild.h"




network *build_network(int ivs, int ovs)
{
	network *n = calloc(1, sizeof(network));
	if(!n) exit(EXIT_FAILURE);
	
	n->ivs = ivs;
	n->ovs = ovs;
	
	n->lnb = 0;
	
	n->niv = calloc(ivs, sizeof(double));
	if(!n->niv) exit(EXIT_FAILURE);
	
	n->nov = calloc(ovs, sizeof(double));
	if(!n->nov) exit(EXIT_FAILURE);
	
	n->err = 0.0;
	
	return (n);
}


void add_layer(network *n, int knb, int aft, int pft, double bias)
{
	layer *l = calloc(1, sizeof(layer));
	if(!l) exit(EXIT_FAILURE);
	
	int lnb = n->lnb;
	
	l->knb = knb;
	l->aft = aft;
	l->pft = pft;
	l->bias = bias;
	
	
	l->win = (double**) calloc(knb, sizeof(double*));
	if(!l->win) exit(EXIT_FAILURE);
	
	l->afv = calloc(knb, sizeof(double));
	if(!l->afv) exit(EXIT_FAILURE);
	
	l->pfv = calloc(knb, sizeof(double));
	if(!l->pfv) exit(EXIT_FAILURE);
	
	l->delta = calloc(knb, sizeof(double));
	if(!l->delta) exit(EXIT_FAILURE);
	
	
	if(lnb == 0)
	{
		l->ivs = n->ivs;
		
		n->lay = calloc(lnb+1, sizeof(layer));
		if(!n->lay) exit(EXIT_FAILURE);
	}
	else
	{
		l->ivs = n->lay[lnb-1].knb;
		
		n->lay = realloc(n->lay, (lnb+1) * sizeof(layer));
		if(!n->lay) exit(EXIT_FAILURE);
	}
	
	int ivs = l->ivs;
	
	for(int k = 0; k < knb; k++)
	{
		l->win[k] = calloc(ivs+1, sizeof(double));			// +1 pour le poids lié au biais
		if(!l->win[k]) exit(EXIT_FAILURE);
	}
	
	n->lay[lnb] = *l;
	n->lnb = lnb+1;
	
	
	
	free(l);
}


void activate_network(network *n)
{
	
	double sw = 0.0;		// = sqrt(3/k);
	int ivs = 0;
	int knb = 0;
	int lnb = n->lnb;
	
	
	for(int l = 0; l < lnb; l++)
	{
		
		ivs = n->lay[l].ivs;
		knb = n->lay[l].knb;
		sw = sqrt(3.0/(double)ivs);
		
		for(int k = 0; k < knb; k++)
		{
			for(int i = 0; i < ivs+1; i++)
			{
				n->lay[l].win[k][i] = randomize(-sw, sw);
			}
		}
	}
}


void free_network(network *n)
{
	int lnb = n->lnb;
	int knb = 0;
	
	for(int l = lnb-1; l >= 0; l--)
	{
		
		knb = n->lay[l].knb;
		
		for(int k = knb-1; k >= 0; k--)
		{
			free(n->lay[l].win[k]);
			n->lay[l].win[k] = NULL;
		}
		
		free(n->lay[l].win);
		n->lay[l].win = NULL;
		
		free(n->lay[l].afv);
		n->lay[l].afv = NULL;
		
		free(n->lay[l].pfv);
		n->lay[l].pfv = NULL;
		
	}
	
	free(n->lay);
	n->lay = NULL;
	
	free(n->niv);
	n->niv = NULL;
	
	free(n->nov);
	n->nov = NULL;
	
	free(n);
	n = NULL;
	
}


void free_layer(layer *l)			//ATTENTION : buguée
{
	int knb = l->knb;
	
	
	for(int k = knb-1; k >= 0; k--)
	{
		free(l->win[k]);
		l->win[k] = NULL;
	}
	
	free(l->win);
	l->win = NULL;
	
	free(l->afv);
	l->afv = NULL;
	
	free(l->pfv);
	l->pfv = NULL;
	
	free(l->delta);
	l->delta = NULL;
	
// 	free(l);
// 	l = NULL;
	
}




/* ATTENTION : Anciène implémentation de 
 * chargement et sauvegarde du réseau
 * Fonctions à réimplémenter par la suite. */


network *load_network(char netname[60])
{
	
	FILE *f = NULL;
	
	f= fopen(netname, "r+");
	if(!f) exit(EXIT_FAILURE);
	
	int nivs = 0;
	int novs = 0;
	int lnb = 0;
	
	int ivs = 0;
	int knb = 0;
	int aft = 0;
	int pft = 0;
	double bias = 0.0;
	
	int t;
	t = t;
	
	
	t=fscanf(f, "IVS : %i\n", &nivs);
	t=fscanf(f, "OVS : %i\n", &novs);
	t=fscanf(f, "LNB : %i\n", &lnb);
	t=fscanf(f, "\n");
	
	network *n = build_network(nivs, novs);
	
	for (int l = 0; l < lnb; l++)
	{
		t=fscanf(f, "LAYER %i :\n", &l);
		t=fscanf(f, "IVS : %i\n", &ivs);
		t=fscanf(f, "KNB : %i\n", &knb);
		t=fscanf(f, "BIAS : %lf\n", &bias);
		t=fscanf(f, "AFT : %i\n", &aft);
		t=fscanf(f, "PFT : %i\n", &pft);
		
		
		add_layer(n, knb, aft, pft, bias);
		n->lay[l].ivs = ivs;
		
		for(int k = 0; k < knb; k++)
		{
			t=fscanf(f, "KERNEL %i : \n", &k);
			
			for(int i = 0; i < ivs+1; i++)
			{
				t=fscanf(f, "WEIGHT %i = %lf \n", &i, &(n->lay[l].win[k][i]));
			}
		}
		t=fscanf(f, "\n");
		
	}
	
	fclose(f);
	
	return n;
}


void save_network(network *n, char name[32])
{
	
	FILE *f = NULL;
	
	f = fopen(name, "w+");
	if(!f)
	{
		printf("ERROR SAVING FILE...\n");
		exit(EXIT_FAILURE);
	}
	
	int lnb = n->lnb;
	int knb = 0;
	int ivs = 0;
	
// 	fprintf(f, "NAME : %s\n", n->name);
	fprintf(f, "IVS : %i\n", n->ivs);
	fprintf(f, "OVS : %i\n", n->ovs);
	fprintf(f, "LNB : %i\n", n->lnb);
	fprintf(f, "\n");
	
	for (int l = 0; l < lnb; l++)
	{
		fprintf(f, "LAYER %i :\n", l);
		fprintf(f, "IVS : %i\n", n->lay[l].ivs);
		fprintf(f, "KNB : %i\n", n->lay[l].knb);
		fprintf(f, "BIAS : %lf\n", n->lay[l].bias);
		fprintf(f, "AFT : %i\n", n->lay[l].aft);
		fprintf(f, "PFT : %i\n", n->lay[l].pft);
		
		ivs = n->lay[l].ivs;
		knb = n->lay[l].knb;
		
		for(int k = 0; k < knb; k++)
		{
			fprintf(f, "KERNEL %i : \n", k);
			
			for(int i = 0; i < ivs; i++)
			{
				fprintf(f, "WEIGHT %i = %lf \n", i, n->lay[l].win[k][i]);
			}
		}
		fprintf(f, "\n");
	}
	
	fclose(f);
}
















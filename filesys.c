#include "filesys.h"
#include "trsys.h"


/* NOTE : Le code ci-dessous est issu d'une anciène implémentation.
 * seules certaines fonctions ont été modifiées / adaptées, mais
 * globalement, tout est à reprendre en ce qui concerne la gestion
 * des fichiers... */


idxt *load_idxfile(char fname[60])
{
	idxt *ip = calloc(1, sizeof(idxt));
	
	FILE *f = fopen(fname, "r+");
	if(!f)
	{
		printf("ERROR OPENING LABELS FILE\n\n");
		exit(EXIT_FAILURE);
	}
	
	ip->type = 0x00;		// type des data
	ip->dim = 0x00;		// nombre de dimensions
	ip->num = 0;			// nombre d'items
	
	uint8_t zero = 0x00;		// Toujours à 0
	uint8_t num_hhb = 0x00;		// MSB
	uint8_t num_hlb = 0x00;		// 
	uint8_t num_lhb = 0x00;		// 
	uint8_t num_llb = 0x00;		// LSB
	
	uint8_t t;
	t = t;
	
	t=fscanf(f,"%c", &zero);
	t=fscanf(f,"%c", &zero);
	t=fscanf(f,"%c",&ip->type);
	t=fscanf(f,"%c",&ip->dim);
	
	ip->dimt = calloc(ip->dim, sizeof(int));
	if(!ip->dim) exit(EXIT_FAILURE);
	
	for (int d = 0; d < (int)ip->dim; d++)
	{
		t=fscanf(f,"%c", &num_hhb);
		t=fscanf(f,"%c", &num_hlb);
		t=fscanf(f,"%c", &num_lhb);
		t=fscanf(f,"%c", &num_llb);
		
		ip->dimt[d] = 256*256*256*(int)num_hhb + 256*256*(int)num_hlb + 256*(int)num_lhb + (int)num_llb;
		
		if(d == 0)
		{
			ip->num = ip->dimt[d];
		}
		else
		{
			ip->num *= ip->dimt[d];
		}
	}
	
	ip->array = calloc(ip->num, sizeof(uint8_t));
	if(!ip->array)
	{
		printf("\nERROR ALLOCATING LABEL'S VECTOR\n");
		exit(EXIT_FAILURE);
	}
	
	
	for(int i = 0; i < ip->num; i++)
	{
		t=fscanf(f,"%c",&(ip->array[i]));
	}
	
	fclose(f);
	
	return (ip);
}


double **vectorize_idxfile(idxt *iv)
{
	int inb = (int)iv->dimt[0];
	int vnb = (int) ((int)iv->dimt[1] * (int)iv->dimt[2]);
	int o = 0;
	
	double **fv = calloc(inb, sizeof(double*));
	if(!fv) exit(EXIT_FAILURE);
	
	for (int i = 0; i < inb; i++)
	{
		fv[i] = calloc(vnb, sizeof(double));
		if(!fv[i]) exit(EXIT_FAILURE);
		
		o = i * vnb;
		
		for (int j = 0; j < vnb; j++)
		{
			fv[i][j] = (double)iv->array[o+j]/255;			// ATTENTION : modifié pour linéarisation des entrées.
		}
	}
	
	return(fv);
}


iofile *create_iofile(char ifn[60], char ofn[60])
{
	idxt *ix = load_idxfile(ifn);
	if(!ix) exit(EXIT_FAILURE);
	
	idxt *ox = load_idxfile(ofn);
	if(!ox) exit(EXIT_FAILURE);
	
	iofile *iof = calloc(1, sizeof(iofile));
	if(!iof) exit(EXIT_FAILURE);
	
	
	iof->pop = ox->dimt[0];
	iof->idim = ix->dimt[1]*ix->dimt[2];
	iof->odim = 10;
	
	iof->iv = vectorize_idxfile(ix);
	if(!iof->iv) exit(EXIT_FAILURE);
	
	iof->ov = calloc(iof->pop, sizeof(double*));
	if(!iof->ov) exit(EXIT_FAILURE);
	
	for (int i = 0; i < iof->pop; i++)
	{
		iof->ov[i] = classify_table((double)ox->array[i]);
	}
	
	
	return iof;
}



// void free_idxf(idxt *ix)
// {
// 	
// 	
// }


void free_iovf(iofile *ix)
{
	int pop = ix->pop;
	
	for(int p = pop-1; p > 0; p--)
	{
		free(ix->iv[p]);
		ix->iv[p] = NULL;
		
		free(ix->ov[p]);
		ix->ov[p] = NULL;
	}
	
	free(ix->iv);
	ix->iv = NULL;
	
	free(ix->ov);
	ix->iv = NULL;
	
	free(ix);
	ix = NULL;
}






/////// ATTENTION /////// Code issu des mes anciennes implémentations ! 
// Il  a très certainement des améliorations à y apporter !!!! ////

double *classify_table(double val)
{
	int osize = 10;
	
	double *tab = calloc(osize, sizeof(double));
	
	for (int i = 0; i < osize; i++)
	{
		if ((double)i == val)
		{
			tab[i] = 1.0;
		}
		else
		{
			tab[i] = 0.0;
		}
	}
	
	return (tab);
}
		


double get_real_val(double *tab)
{
	double r = 0.0;
	double max = 0.0;
	
	int osize = 10;
	
	for(int i = 0; i < osize; i++)
	{
		if(tab[i] > max)
		{
			max = tab[i];
			r = i;
		}
	}
	
	return r;
}


void printa_test(double *tab, int wi, int he, int o)
{
	printf("index = %i\n", o);
	
	
	for (int h = 0; h < he; h++)
	{
		
		for (int w = 0; w < wi; w++)
		{
			if((float)tab[w+(h*wi)] > 0.0)
			{
				printf(" x");
			}
			else
			{
				printf(" .");
			}
		}
		printf("\n");
	}
	printf("\n");
}


void print_test(idxt *iv, idxt *ov, int o)
{
	
	int he = (int)iv->dimt[1];
	int wi = (int)iv->dimt[2];
	
	printf("img dim (W * H) : %i x %i \n", wi, he);
	
	
	printf("%i value = %i\n", o, (int)ov->array[o]);
	
	o *= he*wi;
	
	
	for (int h = 0; h < he; h++)
	{
		
		for (int w = 0; w < wi; w++)
		{
			if((float)iv->array[w+(h*wi)+o] > 0.0)
			{
				printf(" x");
			}
			else
			{
				printf(" .");
			}
		}
		printf("\n");
	}
	printf("\n");
}



















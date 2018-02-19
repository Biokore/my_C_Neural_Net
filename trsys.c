#include "trsys.h"




trfile *load_trv(char tvf[60])
{
	trfile *trf = calloc(1, sizeof(trfile));
	if(!trf) exit(EXIT_FAILURE);
	
	FILE *f = fopen(tvf, "r+");
	if(!f)
	{
		printf("ERROR OPENING TRAIN CFG FILE\n\n");
		exit(EXIT_FAILURE);
	}
	
	int t = 0;
	t = t;
	
	int type;
	double alfa;
	double decay;
	double mk;
	
	int rot;
	int it;
	int btype;
	int bsize;
	
	t=fscanf(f,"LTYPE = %i\n", &type);
	t=fscanf(f,"ALFA = %lf\n", &alfa);
	t=fscanf(f,"DECAY_RATE = %lf\n", &decay);
	t=fscanf(f,"MOMENTUM_K = %lf\n", &mk);
	t=fscanf(f,"EPOCH = %i\n", &rot);
	t=fscanf(f,"ITERATIONS = %i\n", &it);
	t=fscanf(f,"BATCH_TYPE = %i\n", &btype);
	t=fscanf(f,"BATCH_SIZE = %i\n", &bsize);
	
	trf->type = type;
	trf->alfa = alfa;
	trf->decay = decay;
	trf->mk = mk;
	trf->rot = rot;
	trf->it = it;
	trf->btype = btype;
	trf->bsize = bsize;
	
	fclose(f);
	
	printf("type = %i\n", trf->type);
	printf("alfa = %lf\n", trf->alfa);
	printf("decay = %lf\n", trf->decay);
	printf("mk = %lf\n", trf->mk);
	printf("rot = %i\n", trf->rot);
	printf("iter = %i\n", trf->it);
	printf("btype = %i\n", trf->btype);
	printf("bsize = %i\n", trf->bsize);
	
	printf("\n");
	
	return (trf);
}


void train_mngr(network *n, trfile *trv, iofile *trf, iofile *tef)
{
	list_network(n);
	n->err = test_network(n, tef);
	printf("Base network error = %2.2lf%c\n\n\n", n->err*100, 0x25);
	
	if(trv->type == 0)
	{
		printf("Standard SGD training...\n");
		sgd_train(n, trv, trf, tef);
	}
	else if(trv->type == 1)
	{
		printf("Mini-batch training mode...\n");
		mini_batch(n, trv, trf, tef);
	}
	else
	{
		printf("Bad training mode, exiting...\n");
		exit(EXIT_FAILURE);
	}
	
}


void sgd_train(network *n, trfile *trv, iofile *trf, iofile *tef)
{
	int rot = trv->rot;
	int it = trv->it;
	int pop = trf->pop;
	
	double decay = trv->decay;
	double d = 0.019;
	
	clock_t start;
	clock_t end;
	
	double elapsed = 0.0;
	
	double err = 0.0;
	double sav = n->err;
	
	
	for(int e = 0; e < rot; e++)
	{
		for(int i = 0; i < it; i++)
		{
			start = clock();
			for(int p = 0; p < pop; p++)
			{
				network_propagate(n, trf->iv[p]);
				calculate_delta(n, trv, trf->ov[p]);
				actualize_weights(n, trv);
			}
			end = clock();
			
			elapsed = ((double)end - start) / CLOCKS_PER_SEC;
			
			err = test_network(n, tef);
			printf("Epoch n°%i, iteration n°%i : Error = %2.2lf%c (time = %.2lfsec)\n", (e+1), (i+1), err*100, 0x25, elapsed);
			
			if(err < sav)
			{
				if(err < d)
				{
					trv->alfa *= decay;
					d *= 0.9;
					printf("changing alfa : %.4lf\n", trv->alfa);
				}
				sav = err*0.999;
				save_network(n, "networks/current/saved.txt");
			}
		}
		trv->alfa *= decay;
		d *= decay;
		printf("changing alfa : %.4lf\n", trv->alfa);
	}
	
}


void mini_batch(network *n, trfile *trv, iofile *trf, iofile *tef)	// ATTENTION: implémentation à finaliser...
{
	int rot = trv->rot;
	int it = trv->it;
	int pop = 0;
	
	clock_t start;
	clock_t end;
	double elapsed = 0.0;
	
	double err = 0.0;
	
	iofile *mbf = NULL;

	
	for(int e = 0; e < rot; e++)
	{
		for(int i = 0; i < it; i++)
		{
			start = clock();
			
			mbf = create_batch(trv, trf);
			pop = mbf->pop;
			
			for(int p = 0; p < pop; p++)
			{
				network_propagate(n, trf->iv[p]);
				calculate_delta(n, trv, trf->ov[p]);
				actualize_weights(n, trv);
			}
			
			end = clock();
			elapsed = ((double)end - start) / CLOCKS_PER_SEC;
			
			err = test_network(n, tef);
			printf("Epoch n°%i, iteration n°%i : Error = %2.2lf%c (time = %.2lfsec)\n", (e+1), (i+1), err*100, 0x25, elapsed);
		}
	}
	
	free_iovf(mbf);
}


void calculate_delta(network *n, trfile *trv, double *ov)
{
	int lnb = n->lnb;
	int fl = lnb-1;
	int knb = 0;
	int jnb = 0;
	int pft = 0;
	
	double kav = 0.0;
	double kpv = 0.0;
	double err = 0.0;
	double deriv = 0.0;
	
	double mk = trv->mk;
	
	
	pft = n->lay[fl].pft;
	knb = n->lay[fl].knb;
	
	for(int k = 0; k < knb; k++)
	{
		kav = n->lay[fl].afv[k];
		kpv = n->lay[fl].pfv[k];
		
		err = (ov[k] - kpv);
		deriv = (*getDerivate[pft])(kav);		// ATTENTION : utilisation d'un pointeur de fonction !
		
		n->lay[fl].delta[k] = (1 - mk) * err * deriv + (mk * n->lay[fl].delta[k]);
	}
	
	for(int l = fl-1; l >= 0; l--)
	{
		pft = n->lay[l].pft;
		knb = n->lay[l].knb;
		jnb = n->lay[l+1].knb;
		
		for(int k = 0; k < knb; k++)
		{
			err = 0.0;				// ne pas oublier...
			kav = n->lay[l].afv[k];
			
			deriv = (*getDerivate[pft])(kav);		// ATTENTION : utilisation d'un pointeur de fonction !
			
			for(int i = 0; i < jnb; i++)
			{
				err += n->lay[l+1].delta[i] * n->lay[l+1].win[i][k+1];
			}
			
			n->lay[l].delta[k] = (1 - mk) * err * deriv + (mk * n->lay[l].delta[k]);
		}
	}
	
	
}

void actualize_weights(network *n, trfile *trv)
{
	int lnb = n->lnb;
	int knb = 0;
	int ivs = 0;
	
	double alfa = trv->alfa;
	double delta = 0.0;
	double bias = 0.0;
	double win = 0.0;
	
	
	ivs = n->ivs;
	knb = n->lay[0].knb;
	bias = n->lay[0].bias;
	
	for(int k = 0; k < knb; k++)
	{
		delta = n->lay[0].delta[k];
		win = n->lay[0].win[k][0];
		
		n->lay[0].win[k][0] = win + alfa * delta * bias;
		
		for(int i = 0; i < ivs; i++)
		{
			win = n->lay[0].win[k][i+1];
			n->lay[0].win[k][i+1] = win + alfa * delta * n->niv[i];
		}
	}
	
	
	for(int l = 1; l < lnb; l++)
	{
		ivs = n->lay[l].ivs;
		knb = n->lay[l].knb;
		bias = n->lay[l].bias;
		
		for(int k = 0; k < knb; k++)
		{
			delta = n->lay[l].delta[k];
			win = n->lay[l].win[k][0];
			
			n->lay[l].win[k][0] = win + alfa * delta * bias;
			
			for(int i = 0; i < ivs; i++)
			{
				win = n->lay[l].win[k][i+1];
				n->lay[l].win[k][i+1] = win + alfa * delta * n->lay[l-1].pfv[i];
			}
		}
	}
	
	
}


double test_network(network *n, iofile *tef)
{
	double err = 0.0;
	
	int pop = tef->pop;
	
	for(int p = 0; p < pop; p++)
	{
		network_propagate(n, tef->iv[p]);
		
		if(get_real_val(n->nov) == get_real_val(tef->ov[p]))
		{
			err += 0.0;
		}
		else
		{
			err += 1.0/pop;
		}
	}
	
	return (err);
	
}



iofile *create_batch(trfile *trv, iofile *iof)				///// ATTENTION : FONCTION A TERMINER !!!!
{
	iofile *oof = calloc(1, sizeof(iofile));
	if(!oof) exit(EXIT_FAILURE);
	
	int btype = trv->btype;
	int bsize = trv->bsize;
	int pop = iof->pop;
	int opop = 0;
	
	int idim = iof->idim;
	int odim = iof->odim;
	
	oof->idim = idim;
	oof->odim = odim;
	
	if(pop % bsize == 0)
	{
		oof->pop = pop / bsize;
	}
	else
	{
		oof->pop = pop / bsize + pop % bsize;
	}
	
	opop = oof->pop;
	
	
	oof->iv = calloc(opop, sizeof(double*));
	if(!oof->iv) exit(EXIT_FAILURE);
	
	oof->ov = calloc(opop, sizeof(double*));
	if(!oof->ov) exit(EXIT_FAILURE);
	
	for(int b = 0; b < opop; b++)
	{
		oof->iv[b] = calloc(idim, sizeof(double));
		if(!oof->iv[b]) exit(EXIT_FAILURE);
			
		oof->ov[b] = calloc(odim, sizeof(double));
		if(!oof->ov[b]) exit(EXIT_FAILURE);
	}
	
	
	int x = 0;
	
	for(int p = 0; p < pop; p += bsize)
	{
		
		if(p + bsize > pop) opop = pop % bsize;
		
		for(int b = 0; b < opop; b++)
		{
			
			if(btype == 0)
			{
				x = p+b;
			}
			else if(btype == 1)
			{
				x = rand()%pop;
			}
			
			for(int i = 0; i < idim; i++)
			{
				oof->iv[b][i] += iof->iv[x][i]/bsize;
			}
			
			for(int o = 0; o < odim; o++)
			{
				oof->ov[b][o] += iof->ov[x][o]/bsize;
			}
		}
	}
	
	return (oof);
	
}





















































































































































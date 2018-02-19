#ifndef TRSYS_H
#define TRSYS_H

#include "comon.h"
#include "netsys.h"


typedef struct trfile trfile;

struct trfile
{
	int type;
	double alfa;
	double decay;
	double mk;
	
	int rot;
	int it;
	int btype;
	int bsize;
	
};

trfile *load_trv(char tvf[60]);

void train_mngr(network *n, trfile *trv, iofile *trf, iofile *tef);
iofile *create_batch(trfile *trv, iofile *iof);

void sgd_train(network *n, trfile *trv, iofile *trf, iofile *tef);
void mini_batch(network *n, trfile *trv, iofile *trf, iofile *tef);

void calculate_delta(network *n, trfile *trv, double *ov);
void actualize_weights(network *n, trfile *trv);

double test_network(network *n, iofile *tef);		// teste le réseau sur le document test




#endif

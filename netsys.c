#include "netsys.h"
#include "comon.h"





/**************** Fonctions d'activation ************************
 * 
 * #define LINEAR		0		// fonction identité
 * #define THRESHOLD	1		// 1 si x > 0, 0 sinon
 * #define RELU			4		// rectification linéaire
 * #define SIGMOID		2		// sigmoide
 * #define HTAN			3		// tangeante hyperbolique
 * 
 ***************************************************************/

double (*getPropVal[6])(double) = {&linear, &threshold, &relu, &relus, &sigmoid, &htan};	// ATTENTION : pointeur de fonction !!!
double (*getDerivate[6])(double) = {&linear_d, &threshold_d, &relu_d, &relus_d, &sigmoid_d, &htan_d};		// ATTENTION : pointeur de fonction !!!

double linear (double x)
{
	return x;
}

double threshold (double x)
{
	if (x>0.0)
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

double relu (double x)
{
	if (x>0.0)
	{
		return x;
	}
	else
	{
		return 0;
	}
}

double relus (double x)
{
	if (x>0.0)
	{
		if(x>1)
		{
			return 1;
		}
		else
		{
			return x;
		}
	}
	else
	{
		return 0;
	}
}

double sigmoid (double x)
{
	return (1/(1+exp(-x)));
}

double htan (double x)
{
	return ((2/(1+exp(-2*x)))-1);
}



// DERIVEES //

double linear_d (double x)
{
	x = x;
	return 1;
}

double threshold_d (double x)		// voir quoi retourner exactement si x <= 0
{
	if (x>0.0)
	{
		return 0.0;
	}
	else
	{
		return 0.5;
	}
}

double relu_d (double x)
{
	if (x>0.0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

double relus_d (double x)
{
	if (x<1.0 && x > 0.0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

double sigmoid_d (double x)
{
	double xd = sigmoid(x);
	return (xd*(1-xd));
}

double htan_d (double x)
{
	double xd = htan(x);
	return (1-(xd*xd));
}

// double get_pft(int pft, double val)		// ATTENTION : modifié par pointeur de fonction !!!
// {	
// 	return (*getPropVal[pft])(val);
// }


// double derivate(int pft, double val)
// {
// 	return (*getDerivate[pft])(val);
// }

void network_propagate(network *n, double *iv)
{
	int lnb = n->lnb;
	int pft = 0;
	int knb = 0;
	int ivs = 0;
	int ovs = 0;
	double bias = 0.0;
	
	pft = n->lay[0].pft;
	knb = n->lay[0].knb;
	ivs = n->lay[0].ivs;
	bias = n->lay[0].bias;
	
	for(int k = 0; k < knb; k++)
	{
		n->lay[0].afv[k] = n->lay[0].win[k][0] * bias;
		for(int i = 0; i < ivs; i++)
		{
			n->lay[0].afv[k] += n->lay[0].win[k][i+1] * iv[i];
		}
		
		n->lay[0].pfv[k] = (*getPropVal[pft])(n->lay[0].afv[k]);	// ATTENTION : Utilisation d'un pointeur de fonction !!!
	}
	
	for(int l = 1; l < lnb; l++)
	{
		pft = n->lay[l].pft;
		knb = n->lay[l].knb;
		ivs = n->lay[l].ivs;
		bias = n->lay[l].bias;
		
		for(int k = 0; k < knb; k++)
		{
			n->lay[l].afv[k] = n->lay[l].win[k][0] * bias;
			for(int i = 0; i < ivs; i++)
			{
				n->lay[l].afv[k] += n->lay[l].win[k][i+1] * n->lay[l-1].pfv[i];
			}
			n->lay[l].pfv[k] = (*getPropVal[pft])(n->lay[l].afv[k]);	// ATTENTION : Utilisation d'un pointeur de fonction !!!
		}
	}
	
	ivs = n->ivs;
	ovs = n->ovs;
	
	for(int i = 0; i < ivs; i++)
	{
		n->niv[i] = iv[i];
	}
	for(int o = 0; o < ovs; o++)
	{
		n->nov[o] = n->lay[lnb-1].pfv[o];
	}
}


void list_network(network *n)
{
	
	int lnb = n->lnb;
	int knb = 0;
	int syn = 0;
	int pft = 0;
	
	char *ltype = NULL;
	
	for(int l = 0; l < lnb; l++)
	{
		pft = n->lay[l].pft;
		knb = n->lay[l].knb;
		syn = n->lay[l].ivs * knb;
		
		switch(pft)
		{
			case RELU :
				ltype = "RELU";
				break;
			case RELUS :
				ltype = "RELUS";
				break;
			case LINEAR:
				ltype = "LINEAR";
				break;
			case SIGMOID:
				ltype = "SIGMOID";
				break;
			case HTAN:
				ltype = "HTAN";
				break;
			case THRESHOLD:
				ltype = "THRESHOLD";
				break;
			default:
				ltype = "LINEAR";
				break;
		};
		
		printf("LAYER N°%i : %s, %i KERNELS, %i SYNAPSES\n", l, ltype, knb, syn);
	}
	
	printf("\n");
	
	ltype = NULL;
}

















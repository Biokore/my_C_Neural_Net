#ifndef NETSYS_H
#define NETSYS_H

#include "comon.h"
#include "netbuild.h"

#define LINEAR		0
#define THRESHOLD	1
#define RELU		2
#define RELUS		3
#define SIGMOID 	4
#define HTAN		5

double (*getPropVal[6])(double);		// ATTENTION : Pointeur de fonction !!!
double (*getDerivate[6])(double);		// ATTENTION : Pointeur de fonction !!!

double linear (double x);
double threshold (double x);
double relu (double x);
double relus (double x);
double sigmoid (double x);
double htan (double x);

double linear_d (double x);
double threshold_d (double x);
double relu_d (double x);
double relus_d (double x);
double sigmoid_d (double x);
double htan_d (double x);

// double get_pft(int pft, double val);
// double derivate(int pft, double val);

void network_propagate(network *n, double *iv);

void list_network(network *n);




#endif

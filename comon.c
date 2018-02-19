#include "comon.h"




void clear_screen(void)
{
	if (system("clear") == 1)
	{
		printf("ERROR_CLEAR_SCREEN\n");
		exit(EXIT_FAILURE);
	}
}

char get_key(void)	//récupération d'un caractère clavier
{
	char c = '0';
	
	c = getchar();
	while(getchar() != '\n');

	return c;
}

double randomize(double min, double max)
{
	double val = 0.0;
	
	int imax = max * 100000;
	int imin = min * 100000;
	
	val = (double)(rand()%(imax - imin))+imin;
	
	val /= 100000;
	
	
	
	return val;
}





















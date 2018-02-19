
#include "main.h"
#include "comon.h"
#include "netbuild.h"

#include "trsys.h"


int main (/*int argc, char **argv*/)
{
	clear_screen();
	srand(time(NULL));
	printf("==== BIONET ====\n\n");
	
	trfile *trf = load_trv("conf/trcfg.txt");
	
	// Digits
	iofile *iotrainf = create_iofile("database/train-images.idx3-ubyte", "database/train-labels.idx1-ubyte");
	iofile *iotestf = create_iofile("database/t10k-images.idx3-ubyte", "database/t10k-labels.idx1-ubyte");
	iofile *iovalidf = create_iofile("database/train-images.idx3-ubyte", "database/train-labels.idx1-ubyte");
	
	
// 	network *net = load_network("networks/saved_00154.txt");
	network *net = net_build();
	
	testOn(net, iotestf, rand()%10000);
	
	
	// TEST // TEST // TEST // TEST // TEST // TEST 
	
	
	train_mngr(net, trf, iotrainf, iotestf);
	
	printf("\n");
	printf("Initiate validation...\n");
	printf("Final error = %.2lf%c\n\n", test_network(net, iovalidf), 0x25);
	
	
	
	// TEST // TEST // TEST // TEST // TEST // TEST 
	
	free_iovf(iotrainf);
	free_iovf(iotestf);
	free_iovf(iovalidf);
	
	free_network(net);
	
	printf("Good-bye !\n");
	
	printf("\n");
	return(EXIT_SUCCESS);
};



network *net_build()
{
	
	network *net = build_network(784,10);
	
	add_layer(net, 150, LINEAR, RELU, -0.5);
	add_layer(net, 50, LINEAR, HTAN, -0.5);
// 	add_layer(net, 20, LINEAR, RELU, +1.0);
	add_layer(net, 10, LINEAR, SIGMOID, +0.5);
	
	activate_network(net);
	
	save_network(net, "networks/current/base.txt");
	
	return (net);
}


void testOn(network *n, iofile *iof, int index)
{
	
	printa_test(iof->iv[index], 28, 28, index);
	
	network_propagate(n, iof->iv[index]);
	
	printf("computed value : %lf\n", get_real_val(n->nov));
	printf("real value : %lf\n", get_real_val(iof->ov[index]));
}





















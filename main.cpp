/*
 * main.cpp
 *
 *  Created on: 14.07.2008
 *      Author: raido
 */

#include "CABP.h"

int main(int argc, char **argv) {
	CABP algo(10,20);

	printf("Adding nodes\n");
	algo.addNonCommNode(6,100,100,50);
	algo.addCommNode(1,100,100,54);
	algo.addCommNode(2,100,150,55);
	algo.addCommNode(3,150,100,53);
	algo.addNonCommNode(4,150,150,50);
	algo.addNonCommNode(5,75,75,50);
	algo.addCommNode(6,250,100,71);
	algo.addCommNode(7,250,150,60);
	algo.addCommNode(8,250,150,55);
	printf("Calculating\n");
	algo.position(NULL, NULL, NULL);


	algo.clear();
	printf("Adding nodes\n");
	//algo.addNonCommNode(6,100,100,50);
	algo.addCommNode(1,100,100,75);
	algo.addCommNode(2,100,200,75);
	algo.addCommNode(3,200,100,75);
	//algo.addNonCommNode(4,150,150,50);
	//algo.addNonCommNode(5,75,75,50);
	printf("Calculating\n");
	algo.position(NULL, NULL, NULL);

	return 1;
}

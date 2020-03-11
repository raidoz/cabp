/*
 * main.cpp
 *
 *  Created on: 14.07.2008
 *      Author: raido
 */

#include "CABP.h"
#include <stdio.h>
//#include <string.h>
#include <stdlib.h>



int openFile(const char *fileName, FILE **f) {
    if(*f == NULL) {
    	printf("Opening %s ... ", fileName);
        if ((*f = fopen(fileName, "r")) == NULL) {
        	printf("FAILED\n");
        	return FAIL;
        }
        else printf("OK\n");
    }
    return SUCCESS;
}

int insertValues(FILE *f, CABP *algo) {
	char line[80];
	int values[4];
	int count = 0;
	int valueCount = 0;
	int start;

	while(fgets(line, 80, f) != NULL) { // Read lines until EOF - fgets returns NULL
		count++;
		switch(line[0]) {
			case 'C':
			case 'N':
				if(line[1] == ',') {
					valueCount = 0;
					start = 2;
					for(int i=2; i<80; i++) {
						if(line[i] == ',' || line[i] == '\n') {
							line[i] = '\0';
							values[valueCount] = atoi(&line[start]);
							valueCount++;
							if(valueCount >= 4) break;
							start = i+1;
						}
					}
					if(valueCount >= 4) {
						printf("Node %i (%i, %i) r=%i\n", values[0],values[1],values[2],values[3]);
						if(line[0] == 'C') algo->addCommNode(values[0],values[1],values[2],values[3]);
						else algo->addNonCommNode(values[0],values[1],values[2],values[3]);
					}
					else printf("Bad input line: %i\n", count);
				}
				else printf("Bad input line: %i\n", count);
				break;
			case ' ':
			case '\n':
				return SUCCESS;
			case '#': // Comment
				break;
			default:
				printf("Bad input line: %i\n", count);
				break;
		}
	}
	return SUCCESS;
}

int writeToFile(int x, int y, int r) {
	FILE *f = fopen("result.txt", "w");
		if(f != NULL) {
			char data[80];
			sprintf(data, "%i,%i,%i\n", x, y ,r);
			fputs(data, f);
			fclose(f);
		}

    return SUCCESS;
}

int main(int argc, char **argv) {
	FILE *f = NULL;

	int16_t x = 0;
	int16_t y = 0;
	uint16_t r = 0;

	CABP algo(10,20);
	algo.enableCompensating(25);

	if(argc == 1) openFile("testset.txt", &f);
	else openFile(argv[1], &f);

	printf("Reading input:\n");
	insertValues(f, &algo);

	if(f != NULL) fclose(f);

	printf("Calculating:\n");
	algo.position(&x,&y,&r);

	writeToFile(x,y,r);

	return 1;
}

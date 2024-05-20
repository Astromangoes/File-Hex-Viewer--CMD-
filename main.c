#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024 //sets the file read buffer size

int columns = 16; //default column value

void printColHead(int numCol) {
	printf("| OFFSET |");
	for(int i = 0; i < numCol; i++) {
		printf("%.2X|", i);
	}
	printf("\n");
}

void printHexTable(unsigned char* string, int length, int numCol) {
	int i = 0;
	unsigned char* str_pos;
	printColHead(numCol);
	while(i < length) {
		str_pos = string + i;
		i++;
		if(!(i%numCol) || (i == length)) {
			printf("%.2X\n|%.8X|", *str_pos, i);
		} else if(i==1) {
			printf("|%.8X|%.2X ", i-1,*str_pos);
		} else {
			printf("%.2X ", *str_pos);
		}
	}
}

void readFile(FILE *toRead) {
	unsigned char* str = (char*)malloc(BUFFER_SIZE);
	int buffer_mult = 1;
	int str_size = 0;
	unsigned char* str_pos;
	int i = 0;
	while(1 || i < BUFFER_SIZE) {
		if(i < BUFFER_SIZE) {
			str_pos = str+str_size;
			*str_pos = fgetc(toRead);
		} else {
			str = realloc(str, BUFFER_SIZE * buffer_mult);
			printf("%s\n", str);
			i = 0;
			//buffer_mult++;
			str_pos = str+str_size;
			*str_pos = fgetc(toRead);
		}
		if(feof(toRead)) {
			printf("EOF");
			break;
		}
		str_size++;
		i++;
	}
	printHexTable(str, str_size, columns);
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("No input file provided.\n");
	} else if(argc > 3) {
		printf("Too many arguments provided\n");
	}else if(argc == 2) {
		FILE* doc;
		doc = fopen(argv[1], "rb");
		if(doc == NULL) {
			perror("ERROR, FOPEN: ");
			return (-1);
		} else {
			printf("file opened.\n");
			readFile(doc);
			fclose(doc);
		}
	} else if(argc == 3) {
		char* pointy;
		columns = strtol(argv[2], &pointy, 10);
		FILE* doc;
		doc = fopen(argv[1], "rb");
		if(doc == NULL) {
			perror("ERROR, FOPEN: ");
			return (-1);
		} else {
			printf("file opened.\n");
			readFile(doc);
			fclose(doc);
		}
	}
	printf("Program exiting.\n");
	return 0;
}
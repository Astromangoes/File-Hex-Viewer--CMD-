#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE sizeof(char)*1024 //sets the file read buffer size

int columns = 16; //default column value
char* fileBuff;
unsigned long fileLen;
char* allocated;
char* allocated_pos;
unsigned long numAllocated;

char* allocate(unsigned long memSize) {
	char* output = malloc(memSize);
	allocated = output;
	allocated_pos += sizeof(char*);
	return output;
}

char* reallocate(char* ptr, unsigned long memSize) {
	char* output = realloc(ptr, memSize);
	return output;
}

void deallocateAll() {
	char* pos = allocated+(numAllocated*sizeof(char*));
	for(int i = numAllocated; i > 0; i--) {
		free(pos);
		pos -= sizeof(char*);
	}
}

unsigned long sizeOfHead(unsigned long col) {
	unsigned long size = (10*sizeof(char)) + (col*(3*sizeof(char))) + (1*sizeof(char)); //address col = 10 chars, each col = 3 chars, null character = 1 char
	return size;
}

unsigned long sizeOfTable(unsigned long col, unsigned long length) {
	unsigned long rows;
	unsigned long size = sizeOfHead(col);
	if(length%col) {
		rows = ((length - (length%col))/col) + 1;
	} else {
		rows = ((length - (length%col))/col);
	}
	size = size + 
		   ((3*sizeof(char)) * length) +
		   ((11*sizeof(char)) * rows);	
	return size;
}

char* formatHead(int numCol) {
	char* output = allocate(sizeOfHead(numCol));
	char* out_pos;
	out_pos = output + 10*sizeof(char);
	sprintf(output, "| OFFSET |");
	for(int i = 0; i < numCol; i++) {
		sprintf(out_pos, "%.2X|", i);
		out_pos += 3*sizeof(char);
	}
	sprintf(out_pos, "\n");
	return output;
}

char* formatFirstVal(char* chr) {
	char* output = allocate(sizeof(char)*12);
	sprintf(output, "|%.8X|%.2X ", 0, *chr);
	return output;
}

char* formatInnerVal(char* chr) {
	char* output = allocate(sizeof(char)*3);
	sprintf(output, "%.2X ", *chr);
	return output;
}

char* formatEndVal(char* chr, unsigned long rowVal) {
	char* output = allocate(sizeof(char)*13);
	sprintf(output, "%.2X\n|%.8X|", *chr, rowVal);
	return output;
}

char* formatFinalVal(char* chr) {
	char* output = allocate(sizeof(char)*3);
	sprintf(output, "%.2X\n", *chr);
	return output;
}

void printColHead(int numCol) {
	printf("%s", formatHead(numCol));
}

void printFirstVal(char* chr) {
	printf("%s", formatFirstVal(chr));
}

void printInnerVal(char* chr) {
	printf("%s", formatInnerVal(chr));
}

void printEndVal(char* chr, unsigned long rowVal) {
	printf("%s", formatEndVal(chr, rowVal));
}

void printFinalVal(char* chr) {
	printf("%s", formatFinalVal(chr));
}

char* formatHexTable(unsigned char* data, unsigned long length, int numCol) {
	unsigned long i = 0;
	unsigned char* output = allocate(sizeOfTable(numCol, length));
	unsigned char* str_pos = output;
	unsigned char* data_pos = data;
	char* str = formatHead(numCol);
	sprintf(str_pos, "%s", str);
	str_pos = str_pos + sizeOfHead(columns);
	while(i < length) {
		data_pos = data + i*sizeof(char);
		i++;
		if(!(i%numCol)) {
			sprintf(str_pos, "%s", formatEndVal(data_pos, i));
			str_pos = str_pos + 13*sizeof(char);
		} else if(i==1) {
			sprintf(str_pos, "%s", formatFirstVal(data_pos));
			str_pos = str_pos + 13*sizeof(char);
		} else if(i == length) {
			sprintf(str_pos, "%s", formatFinalVal(data_pos));
			str_pos = str_pos + 3*sizeof(char);
		} else {
			sprintf(str_pos, "%s", formatInnerVal(data_pos));
			str_pos = str_pos + 3*sizeof(char);
		}
	}
	return output;
}

FILE* openFileRB(char* fileName) {
	FILE* fileReturn;
	fileReturn = fopen(fileName, "rb");
	if(fileReturn == NULL) {
			perror("ERROR, FOPEN: ");
			return fileReturn;
		} else {
			return fileReturn;
		}
}

FILE* openFileWB(char* fileName) {
	FILE* fileReturn;
	fflush(stdout);
	fileReturn = fopen(fileName, "w");
	if(fileReturn == NULL) {
			perror("ERROR, FOPEN: ");
			return fileReturn;
		} else {
			return fileReturn;
		}
}

void readFile(char* toRead) {
	FILE* doc;
	doc = openFileRB(toRead);
	if(doc) {
		unsigned char* str = (char*)malloc(BUFFER_SIZE);
		int buffer_mult = 1;
		unsigned long str_size = 0;
		unsigned char* str_pos;
		int i = 0;
		while(1 || i < BUFFER_SIZE) {
			if(i < BUFFER_SIZE) {
				str_pos = str+str_size;
				*str_pos = fgetc(doc);
			} else {
				buffer_mult++;
				str = (char*)realloc(str, BUFFER_SIZE * buffer_mult);
				i = 0;
				str_pos = str+str_size;
				*str_pos = fgetc(doc);
			}
			if(feof(doc)) {
				break;
			}
			str_size = str_size + sizeof(char);
			i++;
		}
		fileBuff = (char*)realloc(str, BUFFER_SIZE * buffer_mult);
		fileLen = str_size/sizeof(char);
		//free(str); // this frees the same memory as fileBuff, because pointers so remember to free up fileBuff
		fclose(doc);
	}
}

bool fileExists(char* filename) {
	FILE* test;
	test = fopen(filename, "r");
	if(test) {
		fclose(test);
		return true;
	} else {
		return false;
	}
	fclose(test);
}

void writeFile(char* toWrite, char* data, unsigned long length, int col) {
	FILE* output;
	output = openFileWB(toWrite);
	if(output) {
		fprintf(output, "%s", formatHexTable(data, length, col));
	}
	fclose(output);
}

/*
* Asks the user a yes or no question and returns true (yes) or false (no).
*/
bool askQuestion(char* question) {
	char* answer = (char*)malloc(sizeof(char)*2);
	while(1) {
		printf("%s [Y/N]: ", question);
		scanf("%1c", answer);
		if(*answer == 'y' || *answer == 'Y') {
			fflush(stdin);
			free(answer);
			return true;
			break;
		} else if(*answer == 'n' || *answer == 'N') {
			fflush(stdin);
			free(answer);
			return false;
		} else {
			fflush(stdin); //flushes the input buffer, so that no excess characters exist
			printf("Please answer Y or N.\n");
		}
	}
	free(answer); //just incase :-)
}

char* saveQuestion() {
	char* answer = (char*)malloc(sizeof(char)*200);
	while(1) {
		printf("Please enter filename: ");
		scanf("%s", answer);
		if(fileExists(answer)) {
			char* q = (char*)malloc(sizeof(char)*280); //needs to have mem space for answer and the question to ask (in this case 200 + 80 is plenty)
			sprintf(q, "File '%s' already exits, would you like to overwrite?", answer);
			fflush(stdin);
			if(askQuestion(q)) {
				openFileWB(answer);
				printf("overwrite\n");
				writeFile(answer, fileBuff, fileLen, columns);
				free(q);
				break;
			} else {
				free(q);
				printf("no write\n");
			}
		} else {
			printf("writing to file: %s\n", answer);
			writeFile(answer, fileBuff, fileLen, columns);
			break;
		}
	}
	fflush(stdin);
	free(answer);
}

/*
* Function to ask user whether they would like to
* print the data or save it to a text file.
*/
void askData() {
	if(askQuestion("Print data to text file?")) {
		saveQuestion();
	}
	if(askQuestion("Print data to terminal?")) {
		printf("%s", formatHexTable(fileBuff, fileLen, columns));
	}
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("No input file provided.\n");
	} else if(argc > 3) {
		printf("Too many arguments provided\n");
	}else if(argc == 2) {
		readFile(argv[1]);
		askData();
	} else if(argc == 3) {
		char* pointy;
		columns = strtol(argv[2], &pointy, 10);
		readFile(argv[1]);
		askData();
	}
	deallocateAll();
	printf("Program exiting.\n");
	return 0;
}
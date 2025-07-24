#include "fileops.h"
#include <stdbool.h>



int memfilepath(char* inputarray[MAX_FILEPATH_RECORDED]) {
	for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
		inputarray[i] = (char*)calloc(MAX_FILE_SIZE, 1);
		if (inputarray[i] == NULL) {
			fprintf(stderr, "Memory Allocator Failed (MemFilepath)\n");
			return -1;
		}
	}

	return 0;
}



int Mem_filepath_free(char* inputarray[MAX_FILEPATH_RECORDED]) {
	for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
		if (inputarray[i] != NULL) {
			free(inputarray[i]);
			inputarray[i] = NULL; 
		}
	}
	return 0;
}

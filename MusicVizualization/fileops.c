#include "fileops.h"


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
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


int Create_File(char* filename, char* inputarray[MAX_FILEPATH_RECORDED]) {
	FILE* fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Doesn't work bitch");
		return 1;
	}

	int count = 0;
	char FILENAME[MAX_FILE_CHAR_LENGTH];
	while (fgets(filename, sizeof(filename), fp) && count <= MAX_FILEPATH_RECORDED) {
		if (FileExists(filename) && IsFileExtension(FILENAME, ".wav")) {
			//int TextCopy(char playlist[MAX_FILE_CHAR_LENGTH], const char* filename);
			//fopen("playlist.txt", "w");
			//count++;
		}

		exit(EXIT_FAILURE);
	}
}

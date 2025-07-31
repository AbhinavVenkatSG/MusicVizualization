#define MAX_PATH_LENGTH 256
#include "fileops.h"
#include <stdbool.h>






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

void Drag_N_Drop(char filearray[MAX_FILEPATH_RECORDED][MAX_PATH_LENGTH], int* filepathcounter) {
	if (IsFileDropped()) {
		fprintf(stdout, "Files dropped detected! (DragnDrop)\n");

		FilePathList droppedFiles = LoadDroppedFiles();

		fprintf(stdout, "Number of files dropped: %d\n", droppedFiles.count);

		for (int i = 0; i < droppedFiles.count; i++) {
			if (*filepathcounter < MAX_FILEPATH_RECORDED) {
				TextCopy(filearray[*filepathcounter], droppedFiles.paths[i]);
				printf("Added dropped file: %s\n", filearray[*filepathcounter]);
				(*filepathcounter)++;
			}
			else {
				fprintf(stderr, "File limit reached. Cannot add more. (DragnDrop) \n");
				break;
			}
		}
		UnloadDroppedFiles(droppedFiles);
	}
}
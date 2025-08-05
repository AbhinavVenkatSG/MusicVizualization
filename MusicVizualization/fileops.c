#define MAX_PATH_LENGTH 256
#include "fileops.h"
#include <stdbool.h>






int Create_File(const char* filename, char filearray[MAX_FILEPATH_RECORDED][MAX_PATH_LENGTH], int filepathcount) {
	FILE* fp = fopen(filename, "w");
	if (!fp) {
		fprintf(stderr, "Error: Could not create file '%s'\n", filename);
		return 1;
	}

	for (int i = 0; i < filepathcount; i++) {
		if (FileExists(filearray[i]) && IsFileExtension(filearray[i], ".wav")) {
			fprintf(fp, "%s\n", filearray[i]);
		}
	}

	fclose(fp);
	return 0;
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
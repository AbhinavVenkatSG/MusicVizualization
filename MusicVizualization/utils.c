#define CRT_SECURE_NO_WARNINGS

#include "raylib.h"
#include "utils.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>


#define ARGMAX 10

bool IsArgValid(int argc) {

	if (argc == 2) return true;

	else {
		return false;
	}
}

char* convertArgToLower(char* argv) {
	char arg1_lower[ARGMAX];
	strcpy_s(arg1_lower, argv[1], sizeof(arg1_lower));
	for (int i = 0; i < sizeof(arg1_lower); i++) {
		arg1_lower[i] = tolower(arg1_lower[i]);
	}		
	arg1_lower[ARGMAX] = '\0';
}

char* getFileExtension(char* inputFile) {
	char* fileExtension = strchr(inputFile, ".");
	return fileExtension;
}

bool validateFilePath(char* inputFile) {

}








//extra stuff
int invalidFilePath() {
	fprintf(stderr, "Invalid File Path.");
	return 1;
}
int invalidFileFormat() {
	fprintf(stderr, "invalid file format");
	return 1;
}
int invalidInstruction() {
	fprintf(stderr, "instruction invalid.");
	return 1;
}
int usagePrompt() {
	fprintf(stderr, "usage: porgrame name instruction. ");
	return 1;
}

int filepathcounter = 0;

int DragnDrop(char* filearray[MAX_FILEPATH_RECORDED], int* filepathcounter) {
	if (!IsFileDropped()) {
		fprintf(stderr, "Files dropped are not recognized (DragnDrop");
		return 1;
	}
	FilePathList droppedFiles = LoadDroppedFiles();
	int filesAdded = 0;

	for (int i = 0; i < droppedFiles.count; i++) {
		if (*filepathcounter >= MAX_FILEPATH_RECORDED) {
			printf("File limit reached. Cannot add more.\n");
			break;
		}
		TextCopy(filearray[*filepathcounter], droppedFiles.paths[i]);
		printf("Added dropped file: %s\n", filearray[*filepathcounter]);

		(*filepathcounter)++;
		filesAdded++;
	}
	return filesAdded;
}




void DrawWaveform(Wave wave, float timePlayed, float duration, int screenWidth, int screenHeight) {
	if (wave.frameCount == 0 || wave.data == NULL) return;

	int samplesPerChannel = wave.frameCount / wave.channels;
	short* samples = (short*)wave.data;
	int currentSample = (int)((timePlayed / duration) * samplesPerChannel);
	if (currentSample + SAMPLES_PER_FRAME >= samplesPerChannel) {
		currentSample = samplesPerChannel - SAMPLES_PER_FRAME;
	}
	if (currentSample < 1) currentSample = 1;

	float centerY = screenHeight / 2.0f;
	float scale = screenHeight / 2.5f;

	for (int i = 1; i < SAMPLES_PER_FRAME; i++) {
		int index1 = (currentSample + i - 1) * wave.channels;
		int index2 = (currentSample + i) * wave.channels;

		float x1 = (float)(i - 1) / SAMPLES_PER_FRAME * screenWidth;
		float y1 = centerY - (samples[index1] / 32768.0f) * scale;

		float x2 = (float)i / SAMPLES_PER_FRAME * screenWidth;
		float y2 = centerY - (samples[index2] / 32768.0f) * scale;

		DrawLine((int)x1, (int)y1, (int)x2, (int)y2, DARKBLUE);
	}
}
#include "utils.h"
#include "stdio.h"
#include "string.h"

#define ARGMAX 10

bool IsArgValid(int argc) {

	if (argc == 2) return true;

	else {
		usagePrompt();
		return false;
	}
}

char* convertArgToLower(char* argv) {
	char arg1_lower[ARGMAX];
	strncpy(arg1_lower, argv[1], sizeof(arg1_lower));
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
	fprint(stderr, "Invalid File Path.");
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

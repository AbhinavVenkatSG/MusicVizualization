#include "utils.h"
#include "stdio.h"
#define ARGMAX 10
bool IsArgValid(int argc) {

	if (argc == 2) return true;

	else {
		fprintf(stderr, "usage: porgrame name instruction. ");
		return false;
	}
}

char* convertArgToLower(char* argv) {
	char arg1_lower[ARGMAX];
	int i = 0;
	while (argv[1][i] && i < sizeof(arg1_lower) - 1) {
		arg1_lower[i] = tolower((char)argv[1][i]);
		i++;
	}
	arg1_lower[i] = '\0';
}

#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


#define FFT_SIZE 1024            
#define SAMPLE_RATE 44100        
#define MAX_FILEPATH_RECORDED 10  
#define MAX_FILE_SIZE 1024       
#define ARGMAX 128




int main(int argc, char* argv[]) {
    char arg1_lower[ARGMAX];

    if (!IsArgValid(argc)) {
        fprintf(stderr, "Exiting Program. No Command Line Argument Passed (Main)\n");
        exit(EXIT_FAILURE);
    }

    convertArgToLower(argv[1], arg1_lower);  

    printf("Command: %s\n", arg1_lower);
    printf("This is a test\n");

    return 0;
}
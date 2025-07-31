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
#define MAX_PATH_LENGTH 256




int main(int argc, char* argv[]) {
    char filearray[MAX_FILEPATH_RECORDED][MAX_PATH_LENGTH];
    int filepathcounter = 0;

    if (!Is_Arg_Valid(argc)) {
        fprintf(stderr, "Exiting Program. No Command Line Argument Passed (Main)\n");
        exit(EXIT_FAILURE);
    }

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Music Visualizer");

    SetTargetFPS(60); 

    while (!WindowShouldClose()) {
        Drag_N_Drop(filearray, &filepathcounter);


        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Drag and drop files here", 10, 10, 20, DARKGRAY);
        DrawText("Press ESC to quit.", 10, 40, 20, DARKGRAY);


        for (int i = 0; i < filepathcounter; i++) {
            DrawText(filearray[i], 10, 70 + (i * 20), 20, DARKGREEN);
        }

        EndDrawing();

    }

    CloseWindow(); 

    return 0;
}

#define CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "fileops.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILEPATH_RECORDED 10
#define ARGMAX 10
#define MAX_FILE_SIZE 1024
#define SAMPLES_PER_FRAME 1024
#define SAMPLES_PER_FRAME 1024




int main(int argc, char* argv[]){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <play|create>\n", argv[0]);
        return 1;
    }

    char* filepath[MAX_FILEPATH_RECORDED] = { 0 };
    int filepathCounter = 0;

    if (memfilepath(filepath) != 0) {
        return 1;
    }
    const int w_width = 800;
    const int w_height = 450;

    InitAudioDevice();
    SetTargetFPS(60);


    const char* instruction = argv[1];

    if (strcmp(instruction, "play") == 0) {

        InitWindow(w_width, w_height, "Music Visualizer");


        Music music = { 0 };
        Wave wave = { 0 };
        bool musicLoaded = false;
        float duration = 0.0f;

        while (!WindowShouldClose()) {
            if (IsFileDropped()) {
                DragnDrop(filepath, &filepathCounter);
            }
        }
        return 0;
    }


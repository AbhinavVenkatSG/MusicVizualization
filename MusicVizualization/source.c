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

    const char* instruction = argv[1];

    if (strcmp(instruction, "play") == 0) {
        const int w_width = 800;
        const int w_height = 450;

        InitWindow(w_width, w_height, "Music Visualizer");
        InitAudioDevice();
        SetTargetFPS(60);

        Music music = { 0 };
        Wave wave = { 0 };
        bool musicLoaded = false;
        float duration = 0.0f;

        while (!WindowShouldClose()) {
            if (IsFileDropped()) {
                DragnDrop(filepath, &filepathCounter);

                if (filepathCounter > 0 && FileExists(filepath[filepathCounter - 1])) {
                    wave = LoadWave(filepath[filepathCounter - 1]);
                    music = LoadMusicStream(filepath[filepathCounter - 1]);
                    duration = GetMusicTimeLength(music);

                    if (wave.frameCount > 0) {
                        PlayMusicStream(music);
                        musicLoaded = true;
                        printf("Loaded and playing: %s\n", filepath[filepathCounter - 1]);
                    }
                    else {
                        printf("Invalid or empty audio file.\n");
                    }
                }
            }

            if (musicLoaded) {
                UpdateMusicStream(music);
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            if (filepathCounter == 0) {
                DrawText("Drag and drop a .wav file", 20, 20, 20, DARKGRAY);
            }
            else if (musicLoaded) {
                float timePlayed = GetMusicTimePlayed(music);
                DrawWaveform(wave, timePlayed, duration, w_width, w_height);
            }

            EndDrawing();
        }

        if (musicLoaded) {
            UnloadMusicStream(music);
            UnloadWave(wave);
        }

        CloseAudioDevice();
        CloseWindow();
    }
    else if (strcmp(instruction, "create") == 0) {
        printf("Create mode not implemented yet.\n");
    }
    else {
        fprintf(stderr, "Invalid instruction: %s\n", instruction);
        return 1;
    }

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        free(filepath[i]);
    }

    return 0;
}

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILEPATH_RECORDED 2
#define MAX_FILE_SIZE 1024
#define SAMPLES_PER_FRAME 1024

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


int main(void) {
    const int w_width = 800;
    const int w_height = 450;

    InitWindow(w_width, w_height, "Music Visualizer");
    InitAudioDevice();
    //SetTargetFPS(60);

    int filepathCounter = 0;
    char* filepath[MAX_FILEPATH_RECORDED] = { 0 };

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        filepath[i] = (char*)calloc(MAX_FILE_SIZE, 1);
        if (filepath[i] == NULL) {
            printf("Memory allocation failed for filepath[%d]\n", i);
            return -1;
        }
    }

    Music music = { 0 };
    Wave wave = { 0 };
    bool musicLoaded = false;
    float duration = 0.0f;

    while (!WindowShouldClose()) {
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (filepathCounter >= MAX_FILEPATH_RECORDED) {
                printf("Too many files dropped.\n");
            }
            else {
                TextCopy(filepath[filepathCounter], droppedFiles.paths[0]);

                if (FileExists(filepath[filepathCounter])) {
                    music = LoadMusicStream(filepath[filepathCounter]);
                    wave = LoadWave(filepath[filepathCounter]);
                    duration = GetMusicTimeLength(music);

                    if (wave.frameCount == 0) {
                        printf("ERROR: Failed to load music from %s\n", filepath[filepathCounter]);
                    }
                    else {
                        printf("SUCCESS: Loaded music: %s\n", filepath[filepathCounter]);
                        PlayMusicStream(music);
                        musicLoaded = true;
                    }

                    filepathCounter++;
                }
                else {
                    printf("ERROR: File does not exist: %s\n", filepath[filepathCounter]);
                }
            }

            UnloadDroppedFiles(droppedFiles);
        }

        if (musicLoaded) {
            UpdateMusicStream(music);
        }

        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        if (filepathCounter == 0) {
            DrawText("Drag and drop your music file here", 100, 40, 20, BLACK);
        }
        else {
            DrawText("File dropped. Playing music...", 100, 40, 20, BLACK);

            if (musicLoaded) {
                float timePlayed = GetMusicTimePlayed(music);
                DrawWaveform(wave, timePlayed, duration, w_width, w_height);
            }
        }

        EndDrawing();
    }

    if (musicLoaded) {
        UnloadMusicStream(music);
        UnloadWave(wave);
    }

    CloseAudioDevice();
    CloseWindow();

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        free(filepath[i]);
    }

    return 0;
}
#include "raylib.h"
#include "fileops.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILEPATH_RECORDED 2
#define ARGMAX 10
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

int main(int argc, char* argv[2]) {
    if (argc < 2) {
        return 1;
    }
    char arg1_lower[ARGMAX];
    int i = 0;
    while (argv[1][i] && i < sizeof(arg1_lower) - 1) {
        arg1_lower[i] = tolower((unsigned char)argv[1][i]);
        i++;
    }
    arg1_lower[i] = '\0';

    if (strcmp(arg1_lower, "play") == 0) {
        //isfilevalid();
        //checkfilepath();
        //musicprocessing();
    }
    else if (strcmp(arg1_lower, "create") == 0) {
        //draganddrop();
        //isfilevalid();
        //verifyarray();
        //musicprocessing();
    }

    return 0;
}

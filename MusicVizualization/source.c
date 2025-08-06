#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <raylib.h>
#include "plug.h"

Plug plug = { 0 };

int main(int argc, char** argv)
{
    const char* file_path = "C:\\Users\\abhianu\\OneDrive\\Desktop\\SKAL.wav";

    InitWindow(800, 600, "Music Visualizer");
    SetTargetFPS(60);
    InitAudioDevice();

    plug_init(&plug, file_path);
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            // re-initialize plug if needed
            plug_init(&plug, file_path);
        }
        plug_update(&plug);
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

#define M_PI 3.14159265358979323846
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include <raylib.h>

#include "plug.h"

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

char* shift_args(int* argc, char*** argv)
{
    assert(*argc > 0);
    char* result = (**argv);
    (*argv) += 1;
    (*argc) -= 1;
    return result;
}

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
            // re-initialize plug if issue (just so i dont forgor)
            plug_init(&plug, file_path);
        }
        plug_update(&plug);
    }

    return 0;
}

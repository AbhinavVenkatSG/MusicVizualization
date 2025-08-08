#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plug.h"

#define MAX_SONGS 100
#define MAX_PATH_LEN 512

const char* playlist[MAX_SONGS] = { 0 };
int playlist_size = 0;
int currentSongIndex = 0;
int shuffleMode = 0;

Plug plug = { 0 };

int load_playlist_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    char line[MAX_PATH_LEN];
    while (fgets(line, sizeof(line), file) && playlist_size < MAX_SONGS) {
        line[strcspn(line, "\r\n")] = '\0';

        if (IsFileExtension(line, ".wav")) {
            char* path = (char*)malloc(strlen(line) + 1);
            if (path) {
                strcpy(path, line);
                playlist[playlist_size++] = path;
            }
        }
    }

    fclose(file);
    return playlist_size;
}

int build_playlist_from_dropped_files(FilePathList dropped) {
    playlist_size = 0;

    for (int i = 0; i < dropped.count && playlist_size < MAX_SONGS; i++) {
        const char* path = dropped.paths[i];

        if (IsFileExtension(path, ".txt")) {
            load_playlist_from_file(path);
        }
        else if (IsFileExtension(path, ".wav")) {
            char* copy = (char*)malloc(TextLength(path) + 1);
            if (copy) {
                TextCopy(copy, path);
                playlist[playlist_size++] = copy;
            }
        }
    }

    return playlist_size;
}

void free_playlist() {
    for (int i = 0; i < playlist_size; i++) {
        free((void*)playlist[i]);
    }
    playlist_size = 0;
}

bool WaitForUserToDropFiles() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Drop .wav file(s) or a .txt playlist", 100, 200, 20, RAYWHITE);
        EndDrawing();

        if (IsFileDropped()) {
            FilePathList dropped = LoadDroppedFiles();
            bool success = build_playlist_from_dropped_files(dropped) > 0;
            UnloadDroppedFiles(dropped);
            return success;
        }
    }
    return false;
}

void play_next_song() {
    currentSongIndex = (currentSongIndex + 1) % playlist_size;
    plug_init(&plug, playlist[currentSongIndex]);
}

int main(int argc, char* argv[]) {
    if (argc != 2 || strcmp(argv[1], "play") != 0) {
        printf("Usage: %s play\n", argv[0]);
        return 1;
    }

    InitWindow(800, 600, "Music Visualizer");
    SetTargetFPS(60);

    if (!WaitForUserToDropFiles()) {
        CloseWindow();
        return 1;
    }

    InitAudioDevice();
    plug_init(&plug, playlist[0]);

    while (!WindowShouldClose()) {
        plug_update(&plug);

        if (IsKeyPressed(KEY_N)) {
            StopMusicStream(plug.music);
            play_next_song();
        }

        if (IsKeyPressed(KEY_ESCAPE)) break;
    }

    UnloadMusicStream(plug.music);
    CloseAudioDevice();
    CloseWindow();
    free_playlist();

    return 0;
}

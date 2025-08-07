#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <raylib.h>
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
    if (!file) {
        fprintf(stderr, "Failed to open playlist file: %s\n", filename);
        return 0;
    }

    char line[MAX_PATH_LEN];
    playlist_size = 0;
    while (fgets(line, sizeof(line), file) && playlist_size < MAX_SONGS) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '\0') continue;

        char* path = malloc(strlen(line) + 1);
        if (!path) {
            fprintf(stderr, "Memory allocation failed\n");
            fclose(file);
            return 0;
        }
        strcpy(path, line);
        playlist[playlist_size++] = path;
    }

    fclose(file);
    return playlist_size;
}

void free_playlist() {
    for (int i = 0; i < playlist_size; i++) {
        free((void*)playlist[i]);
        playlist[i] = NULL;
    }
    playlist_size = 0;
}

void play_next_song() {
    if (shuffleMode) {
        if (playlist_size > 1) {
            int next;
            do {
                next = rand() % playlist_size;
            } while (next == currentSongIndex);
            currentSongIndex = next;
        }
    }
    else {
        currentSongIndex = (currentSongIndex + 1) % playlist_size;
    }
    plug_init(&plug, playlist[currentSongIndex]);
}

bool WaitForPlaylistDrop(char* outPath, int maxLen) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Drop a playlist .txt file to start", 100, 250, 20, RAYWHITE);
        EndDrawing();

        if (IsFileDropped()) {
            FilePathList dropped = LoadDroppedFiles();
            if (dropped.count > 0) {
                const char* droppedFile = dropped.paths[0];

                if (strstr(droppedFile, ".txt") != NULL) {
                    strncpy(outPath, droppedFile, maxLen - 1);
                    outPath[maxLen - 1] = '\0';
                    UnloadDroppedFiles(dropped);
                    return true;
                }
                else {
                    fprintf(stderr, "Dropped file is not a .txt file.\n");
                }
            }
            UnloadDroppedFiles(dropped);
        }
    }

    return false;
}

int main(void) {
    srand((unsigned int)time(NULL));
    char playlistFilePath[MAX_PATH_LEN] = { 0 };

    InitWindow(800, 600, "Music  Visualizer");
    SetTargetFPS(60);

    if (!WaitForPlaylistDrop(playlistFilePath, MAX_PATH_LEN)) {
        CloseWindow();
        return 1;
    }

    if (!load_playlist_from_file(playlistFilePath) || playlist_size == 0) {
        fprintf(stderr, "Failed to load valid playlist. Exiting.\n");
        CloseWindow();
        return 1;
    }

    InitAudioDevice();
    currentSongIndex = 0;
    plug_init(&plug, playlist[currentSongIndex]);
    shuffleMode = 1;

    bool songEnded = false;

    while (!WindowShouldClose()) {
        plug_update(&plug);

        float played = GetMusicTimePlayed(plug.music);
        float length = GetMusicTimeLength(plug.music);
        bool done = (length > 0.0f) && (played >= length - 0.01f);

        if (done && !songEnded) {
            StopMusicStream(plug.music);
            play_next_song();
            songEnded = true;
        }
        else if (!done) {
            songEnded = false;
        }

        if (IsKeyPressed(KEY_N)) {
            StopMusicStream(plug.music);
            play_next_song();
            songEnded = false;
        }
        if (IsKeyPressed(KEY_R)) {
            StopMusicStream(plug.music);
            plug_init(&plug, playlist[currentSongIndex]);
            songEnded = false;
        }
        if (IsKeyPressed(KEY_S)) {
            shuffleMode = !shuffleMode;
            printf("Shuffle mode %s\n", shuffleMode ? "ON" : "OFF");
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }
    }

    UnloadMusicStream(plug.music);
    CloseAudioDevice();
    CloseWindow();
    free_playlist();

    return 0;
}

#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "plug.h"
#include "utils.h"

#define MAX_SONGS 100
#define MAX_PATH_LEN 512
#define ARGMAX 10

const char* playlist[MAX_SONGS] = { 0 };
int playlist_size = 0;
int currentSongIndex = 0;
int shuffleMode = 0;
Plug plug = { 0 };

int build_playlist_from_dropped_files(FilePathList dropped) {
    playlist_size = 0;

    for (int i = 0; i < dropped.count && playlist_size < MAX_SONGS; i++) {
        const char* path = dropped.paths[i];
        if (IsFileExtension(path, ".wav")) {
            char* stored = malloc(strlen(path) + 1);
            if (!stored) continue;
            strcpy(stored, path);
            playlist[playlist_size++] = stored;
        }
    }

    return playlist_size;
}

void save_playlist_to_file(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Could not write playlist: %s\n", filename);
        return;
    }

    for (int i = 0; i < playlist_size; i++) {
        fprintf(f, "%s\n", playlist[i]);
    }

    fclose(f);
    printf("Playlist saved as: %s\n", filename);
}

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


int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    if (!Is_Arg_Valid(argc)) {
        printf("Usage: %s [play | create]\n", argv[0]);
        return 1;
    }

    char user_instruction[ARGMAX];
    convertArgToLower(argv[1], user_instruction);

    // ✅ Make window adjustable & run in background
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

    InitWindow(800, 600, "Music Visualizer");
    SetTargetFPS(60);

    if (strcmp(user_instruction, "create") == 0) {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Drop .wav files to create a playlist...", 100, 250, 20, RAYWHITE);
            EndDrawing();

            if (IsFileDropped()) {
                FilePathList dropped = LoadDroppedFiles();

                bool allValid = true;
                for (int i = 0; i < dropped.count; i++) {
                    if (!IsFileExtension(dropped.paths[i], ".wav")) {
                        allValid = false;
                        break;
                    }
                }

                if (!allValid) {
                    UnloadDroppedFiles(dropped);
                    CloseWindow();
                    fprintf(stderr, "Error: Only .wav files are allowed in create mode.\n");
                    return 1;
                }

                if (build_playlist_from_dropped_files(dropped) > 0) {
                    UnloadDroppedFiles(dropped);
                    break;
                }

                UnloadDroppedFiles(dropped);
            }
        }

        char name[256];
        printf("Enter name for your playlist: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';

        char filename[300];
        snprintf(filename, sizeof(filename), "%s.txt", name);
        save_playlist_to_file(filename);
    }

    if (strcmp(user_instruction, "play") == 0 || strcmp(user_instruction, "create") == 0) {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Drop a .wav file or .txt playlist to play...", 100, 250, 20, RAYWHITE);
            EndDrawing();

            if (IsFileDropped()) {
                FilePathList dropped = LoadDroppedFiles();

                bool allValid = true;
                for (int i = 0; i < dropped.count; i++) {
                    if (!IsFileExtension(dropped.paths[i], ".wav") &&
                        !IsFileExtension(dropped.paths[i], ".txt")) {
                        allValid = false;
                        break;
                    }
                }

                if (!allValid) {
                    UnloadDroppedFiles(dropped);
                    CloseWindow();
                    fprintf(stderr, "Error: Only .wav or .txt files are supported.\n");
                    return 1;
                }

                for (int i = 0; i < dropped.count && playlist_size == 0; i++) {
                    const char* path = dropped.paths[i];
                    if (IsFileExtension(path, ".txt")) {
                        load_playlist_from_file(path);
                        break;
                    }
                    else if (IsFileExtension(path, ".wav")) {
                        build_playlist_from_dropped_files(dropped);
                        break;
                    }
                }

                UnloadDroppedFiles(dropped);
                if (playlist_size > 0) break;
            }
        }

        if (playlist_size == 0) {
            CloseWindow();
            return 1;
        }

        InitAudioDevice();
        currentSongIndex = 0;
        plug_init(&plug, playlist[currentSongIndex]);
        shuffleMode = 1;

        while (!WindowShouldClose()) {
            plug_update(&plug);

            float played = GetMusicTimePlayed(plug.music);
            float length = GetMusicTimeLength(plug.music);

            if (length > 0.0f && played >= length - 0.01f) {
                StopMusicStream(plug.music);
                play_next_song();
            }

            if (IsKeyPressed(KEY_N)) {
                StopMusicStream(plug.music);
                play_next_song();
            }

            if (IsKeyPressed(KEY_R)) {
                StopMusicStream(plug.music);
                plug_init(&plug, playlist[currentSongIndex]);
            }

            if (IsKeyPressed(KEY_S)) {
                shuffleMode = !shuffleMode;
                printf("Shuffle mode %s\n", shuffleMode ? "ON" : "OFF");
            }

            if (IsKeyPressed(KEY_ESCAPE)) break;
        }

        UnloadMusicStream(plug.music);
        CloseAudioDevice();
    }

    CloseWindow();
    free_playlist();

    return 0;
}


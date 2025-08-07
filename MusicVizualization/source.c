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

int main(void) {
    srand((unsigned int)time(NULL));

    char playlistFilePath[MAX_PATH_LEN];
    printf("Enter path to playlist text file: ");
    if (fgets(playlistFilePath, sizeof(playlistFilePath), stdin) == NULL) {
        fprintf(stderr, "Failed to read input\n");
        return 1;
    }
    playlistFilePath[strcspn(playlistFilePath, "\r\n")] = 0;

    if (!load_playlist_from_file(playlistFilePath)) {
        fprintf(stderr, "No songs loaded. Exiting.\n");
        return 1;
    }

    if (playlist_size == 0) {
        fprintf(stderr, "Playlist is empty. Exiting.\n");
        return 1;
    }

    InitWindow(800, 600, "Music Player with Visualizer");
    InitAudioDevice();
    SetTargetFPS(60);

    currentSongIndex = 0;
    plug_init(&plug, playlist[currentSongIndex]);

    shuffleMode = 0;

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

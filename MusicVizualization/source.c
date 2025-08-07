#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "plug.h"

#define MAX_SONGS 20
#define MAX_PATH_LEN 512

char playlist[MAX_SONGS][MAX_PATH_LEN] = { 0 };
const char* playlistPtrs[MAX_SONGS] = { 0 };
int songCount = 0;

int currentSongIndex = 0;
Plug plug = { 0 };

void play_next_song(Plug* plug, const char* playlist[], int* currentIndex, int maxSongs) {
    *currentIndex = (*currentIndex + 1) % maxSongs;
    plug_init(plug, playlist[*currentIndex]);
}

void LoadPlaylistFromTextFile(const char* promptMessage) {
    char fileName[256];
    printf("%s: ", promptMessage);
    if (fgets(fileName, sizeof(fileName), stdin) == NULL) {
        fprintf(stderr, "Error reading file name.\n");
        exit(1);
    }

    size_t len = strlen(fileName);
    if (len > 0 && fileName[len - 1] == '\n') {
        fileName[len - 1] = '\0';
    }

    FILE* file = fopen(fileName, "r");
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", fileName);
        exit(1);
    }

    char line[MAX_PATH_LEN];
    while (fgets(line, sizeof(line), file) && songCount < MAX_SONGS) {
        size_t l = strlen(line);
        if (l > 0 && (line[l - 1] == '\n' || line[l - 1] == '\r')) {
            line[l - 1] = '\0';
        }

        TextCopy(playlist[songCount], line);
        playlistPtrs[songCount] = playlist[songCount];
        songCount++;
    }

    fclose(file);

    if (songCount == 0) {
        fprintf(stderr, "No valid song paths found in the file.\n");
        exit(1);
    }

    printf("Loaded %d song(s) from %s\n", songCount, fileName);
}

int main(int argc, char** argv) {
    LoadPlaylistFromTextFile("Enter path to playlist .txt file");

    InitWindow(800, 600, "Music Visualizer");
    SetTargetFPS(60);
    InitAudioDevice();

    plug_init(&plug, playlistPtrs[currentSongIndex]);

    bool songEnded = false;

    while (!WindowShouldClose()) {
        plug_update(&plug);

        float played = GetMusicTimePlayed(plug.music);
        float length = GetMusicTimeLength(plug.music);
        bool done = (length > 0.0f) && (played >= length - 0.01f);

        if (done && !songEnded) {
            StopMusicStream(plug.music);
            play_next_song(&plug, playlistPtrs, &currentSongIndex, songCount);
            songEnded = true;
        }
        else if (!done) {
            songEnded = false;
        }

        if (IsKeyPressed(KEY_R)) {
            currentSongIndex = 0;
            plug_init(&plug, playlistPtrs[currentSongIndex]);
        }

        if (IsKeyPressed(KEY_N)) {
            StopMusicStream(plug.music);
            play_next_song(&plug, playlistPtrs, &currentSongIndex, songCount);
        }
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

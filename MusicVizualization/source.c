#define _CRT_SECURE_NO_WARNINGS 
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define FFT_SIZE 1024            
#define SAMPLE_RATE 44100        
#define MAX_FILEPATH_RECORDED 256
#define MAX_PATH_LENGTH 512

typedef struct {
    char filearray[MAX_FILEPATH_RECORDED][MAX_PATH_LENGTH];
    int front;
    int rear;
    int size;
    int capacity;
} CircularQueue;

typedef struct {
    char filearray[MAX_FILEPATH_RECORDED][MAX_PATH_LENGTH];
    int filepathcounter;
    bool ready_to_play;
} DragDropData;

void initQueue(CircularQueue* queue, int capacity);
bool isFull(CircularQueue* queue);
bool isEmpty(CircularQueue* queue);
void enqueue(CircularQueue* queue, const char* filepath);
const char* dequeue(CircularQueue* queue);
const char* peek(CircularQueue* queue);

void initQueue(CircularQueue* queue, int capacity) {
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->capacity = capacity;
}

bool isFull(CircularQueue* queue) {
    return queue->size == queue->capacity;
}

bool isEmpty(CircularQueue* queue) {
    return queue->size == 0;
}

void enqueue(CircularQueue* queue, const char* filepath) {
    if (isFull(queue)) return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    strncpy(queue->filearray[queue->rear], filepath, MAX_PATH_LENGTH - 1);
    queue->filearray[queue->rear][MAX_PATH_LENGTH - 1] = '\0';
    queue->size++;
}

const char* dequeue(CircularQueue* queue) {
    if (isEmpty(queue)) return NULL;
    const char* filepath = queue->filearray[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return filepath;
}

const char* peek(CircularQueue* queue) {
    if (isEmpty(queue)) return NULL;
    return queue->filearray[queue->front];
}

int main(void) {
    CircularQueue queue;
    initQueue(&queue, MAX_FILEPATH_RECORDED);

    DragDropData dragData = { 0 };
    int currentTrackIndex = 0;
    bool pause = false;
    float timePlayed = 0.0f;

    InitWindow(800, 600, "Music Visualizer");
    InitAudioDevice();
    SetTargetFPS(60);

    Music music = { 0 };
    bool musicLoaded = false;

    while (!WindowShouldClose()) {
        if (IsFileDropped()) {
            FilePathList dropped = LoadDroppedFiles();
            for (int i = 0; i < dropped.count && dragData.filepathcounter < MAX_FILEPATH_RECORDED; i++) {
                if (IsFileExtension(dropped.paths[i], ".wav")) {
                    enqueue(&queue, dropped.paths[i]);
                }
            }
            UnloadDroppedFiles(dropped);
            if (queue.size > 0) {
                dragData.ready_to_play = true;
            }
        }

        if (dragData.ready_to_play && queue.size > 0 && !musicLoaded) {
            const char* path = peek(&queue);
            if (IsFileExtension(path, ".wav")) {
                music = LoadMusicStream(path);
                if (music.ctxData != NULL) {
                    PlayMusicStream(music);
                    musicLoaded = true;
                }
                else {
                    TraceLog(LOG_ERROR, "Failed to load: %s", path);
                }
            }
        }

        if (musicLoaded) {
            UpdateMusicStream(music);
            if (!pause && GetMusicTimePlayed(music) >= GetMusicTimeLength(music)) {
                StopMusicStream(music);
                UnloadMusicStream(music);
                const char* nextPath = dequeue(&queue);
                if (nextPath) {
                    music = LoadMusicStream(nextPath);
                    if (music.ctxData != NULL) {
                        PlayMusicStream(music);
                    }
                }
            }

            float total = GetMusicTimeLength(music);
            if (total > 0) {
                timePlayed = GetMusicTimePlayed(music) / total;
                if (timePlayed > 1.0f) timePlayed = 1.0f;
            }
        }

        if (IsKeyPressed(KEY_R) && musicLoaded) {
            StopMusicStream(music);
            PlayMusicStream(music);
        }

        if ((IsKeyPressed(KEY_K) || IsKeyPressed(KEY_SPACE)) && musicLoaded) {
            pause = !pause;
            if (pause) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }

        if (IsKeyPressed(KEY_N) && musicLoaded) {
            StopMusicStream(music);
            UnloadMusicStream(music);
            const char* nextPath = dequeue(&queue);
            if (nextPath) {
                music = LoadMusicStream(nextPath);
                if (music.ctxData != NULL) {
                    PlayMusicStream(music);
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Drag and drop .wav files", 10, 10, 20, DARKGRAY);
        DrawText("Press [R] to Restart | [K OR SPACE] to Pause/Resume | [N] for Next", 10, 40, 20, DARKGRAY);

        for (int i = 0; i < queue.size; i++) {
            DrawText(queue.filearray[(queue.front + i) % queue.capacity], 10, 70 + (i * 20), 20, RED);
        }

        if (musicLoaded) {
            DrawText("Now Playing:", 200, 140, 20, GRAY);
            const char* filename = GetFileNameWithoutExt(queue.filearray[queue.front]);
            DrawText(filename, 200, 170, 20, MAROON);
            DrawRectangle(200, 220, 400, 12, LIGHTGRAY);
            DrawRectangle(200, 220, (int)(timePlayed * 400.0f), 12, MAROON);
            DrawRectangleLines(200, 220, 400, 12, GRAY);
        }

        EndDrawing();
    }
}

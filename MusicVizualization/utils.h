#pragma once
#include<stdbool.h>
#include <stdio.h>
#include "raylib.h"
#define MAX_FILEPATH_RECORDED 10
#define ARGMAX 10
#define MAX_FILE_SIZE 1024
#define SAMPLES_PER_FRAME 1024

bool Is_Arg_Valid(int argc);				//devki - done
void convertArgToLower(const char* arg, char* output);	//devki - done
char* getFileExtension(char* inputFile); //devki - done
bool validateFilePath(char* inputFile); //tanveer 









//extra stuff i wanted to do.......make code look majestic
//int invalidFilePath();
int invalidFileFormat();
int invalidInstruction();
int usagePrompt();




//void DrawWaveform(Wave wave, float timePlayed, float duration, int screenWidth, int screenHeight);
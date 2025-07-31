#pragma once
#include "raylib.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_FILE_CHAR_LENGTH 512


void Drag_N_Drop(char filearray[MAX_FILEPATH_RECORDED][MAX_PATH_LENGTH], int* filepathcounter);


int Create_File(char* filename, char* inputarray[MAX_FILEPATH_RECORDED]);

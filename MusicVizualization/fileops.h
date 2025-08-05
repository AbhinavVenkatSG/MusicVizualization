#pragma once
#include "raylib.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PATH_LENGTH 512


void Drag_N_Drop(char filearray[MAX_FILEPATH_RECORDED][512], int* filepathcounter);


int Create_File(const char* filename, char filearray[MAX_FILEPATH_RECORDED][512], int filepathcount);
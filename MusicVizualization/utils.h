#pragma once
#include<stdbool.h>

bool IsArgValid(int argc);				//devki - done
char* convertArgToLower(char* argv);	//devki - done
char* getFileExtension(char* inputFile); //devki - done
bool validateFilePath(char* inputFile); //tanveer 









//extra stuff i wanted to do.......make code look majestic
int invalidFilePath();
int invalidFileFormat();
int invalidInstruction();
int usagePrompt();
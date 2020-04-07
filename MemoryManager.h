#pragma once
#define MAX_LENGTH 99

char *AddString(const char *source);
bool RemoveString(char *source);
bool ModifyString(char *oldString, const char *newString);
void QueryStringByContent(const char *subString);
void Defragment();
void ShowInformation();
void ShowEachCharInformation();
void EatLine();
bool IsValidString(char *source);
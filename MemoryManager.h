#pragma once
#include <stdio.h>

#define SIZE 1024*1024
extern char g_szBuff[1024 * 1024];

char *AddString(const char *source);
bool RemoveString(char *source);
bool ModifyString(char *oldString, const char *newString);
//int GetFirstDeprecatedIndex(int startIndex);
//int GetFirstValidIndex(int startIndex);
void Defragment();
void ShowInformation();
void QueryStringByContent(const char *subString);
void ShowEachCharInformation();
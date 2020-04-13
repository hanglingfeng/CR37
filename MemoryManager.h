#pragma once
#define LENGTH 99
#include <stdbool.h>


struct _item {
	unsigned char stringSize;
	bool isUsed;
	bool hasEscapeChar;	
};
typedef struct _item Item;

void EatLine();
char *AddString(const char *source, unsigned char stringSize, bool hasEscapeChar);
void QueryStringByContent(const char *subString);
void QueryStringByContentWithEscapeChar(const char *subString, int subStringSize);
void ShowEachCharInformation();
void ShowInformation();
bool RemoveItem(Item *item);
void Defragment();
bool ModifyItem(Item *oldItem, const char *newString, unsigned char newStringSize, bool hasEscapeChar);
bool SearchItem(Item *item);
#pragma once
#define INPUT_LENGTH 99



struct _item {
	unsigned char nStringSize;
	bool isUsed;
		
};
typedef struct _item Item;

void EatLine();
size_t AddString(const char *pSource, unsigned char nStringSize);
void QueryStringByContent(const char *pSubString);

void ShowEachCharInformation();
void ShowInformation();
bool RemoveItem(Item *pItem);
void Defragment();
bool ModifyItem(Item *pOldItem, const char *pNewString, unsigned char pNewStringSize);
bool SearchItem(Item *pItem);
//在指定数组内存储字符串
//1.增加: 对字符串的增加操作，最长100字节
//2.删除：对字符串的删除操作
//3.修改：修改指定的字符串，如果空间长度不够则存储在其他地方，原字符串视为被删除
//4.查询：按字符串内容查找字符串的基本信息，支持模糊查找
//5.统计：统计每个字母的出现次数和比例。
//6.显示存储信息：按顺序显示已分配(U)、未分配(F)资源,例如10字节空间，显示UFUUFFFUUU
//7.碎片整理：增删改中出现了不连续的“孔隙”，整理后使得这些“孔隙”连续且可用

#define _CRT_SECURE_NO_WARNINGS
#define SIZE 1024*1024
#define BIAS 3

#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>



char g_szBuff[1024 * 1024];
Item *g_itemTable[SIZE / 2] = {NULL};//保存所有字符串的地址，遍历时发现NULL，则停止遍历
int g_currentIndex = 0;//当前可用位置的下标，新增字符串时保存在此处
int g_validIndex = -1;
int g_deprecatedIndex = -1;


//判断字符串size是否符合要求
//参数：
//	int size [in]字符串size
//返回值：
//	bool：size合适返回true，否则返回false
bool IsSizeSuit(int size) {
	return size <= LENGTH+1;
}

//空间是否足够存放
//参数：
//	int need [in]需要的字节数
//返回值：
//	bool：足够返回true，否则返回false
bool IsSpaceEnough(int need) {
	bool enough = SIZE - g_currentIndex >= need;
	return enough;
}


//寻找第一个被标记为已删除字符串的下标,参数代表从哪里开始搜索,如果不存在，返回-1
//参数：
//	int startIndex [in]从哪个下标开始搜索
//返回值：
//	int：返回找到的下标，没找到返回-1
int GetFirstDeprecatedIndex(int startIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (startIndex < g_currentIndex) {
		unsigned char stringSize = g_szBuff[startIndex];
		bool isUsed = g_szBuff[startIndex + 1];
		if (!stringSize) {//长字符串改短，会多出'\0'，算是未使用的空间，可以覆盖
			return startIndex;
		}
		else {
			if (!isUsed) {
				return startIndex;
			}
			else {
				startIndex += BIAS + stringSize;
			}
		}
	}
	return -1;
}

//寻找第一个已使用位置的下标,参数代表从哪里开始搜索,如果空间尚未存放任何有效字符串，返回-1
//参数：
//	int startIndex [in]从哪个下标开始搜索
//返回值：
//	int：返回找到的下标，没找到返回-1 
int GetFirstValidIndex(int startIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (startIndex < g_currentIndex) {
		unsigned char stringSize = g_szBuff[startIndex];
		bool isUsed = g_szBuff[startIndex + 1];
		if (!stringSize) {//长字符串改短，会多出'\0'，不算有效字符，要跳过
			++startIndex;
		}
		else {
			if (isUsed) {
				return startIndex;
			}
			else {
				startIndex += BIAS + stringSize;
			}
		}
	}
	return -1;
}

//清空g_itemTable内容
//参数：
//	无
//返回值：
//	无
void EmptyItemTable() {
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		g_itemTable[i] = NULL;
	}
}

//获取所有item首地址，存在g_itemTable里
//参数：
//	无
//返回值：
//	无
void GetAllItem() {
	EmptyItemTable();
	int validIndex = GetFirstValidIndex(0);
	int i = 0;

	while (validIndex >= 0) {
		Item *item = (Item *)(g_szBuff + validIndex);
		g_itemTable[i] = item;
		++i;
		validIndex = GetFirstValidIndex(validIndex + BIAS + item->stringSize);
	}
}

//碎片整理，使g_szBuff左侧都是已使用空间，右侧都是未使用空间
//参数：
//	无
//返回值：
//	无
void Defragment() {
	int deprecatedIndex = GetFirstDeprecatedIndex(0);
	if (deprecatedIndex < 0) {
		return;
	}
	Item *item = (Item*)(g_szBuff+deprecatedIndex);
	int stringSize = item->stringSize;	
	int validIndex = GetFirstValidIndex(deprecatedIndex + BIAS + stringSize);

	while (deprecatedIndex >= 0 && validIndex > deprecatedIndex) {
		char *destination = g_szBuff + deprecatedIndex;
		Item *source = (Item*)(g_szBuff + validIndex);
		int moveSize = BIAS + source->stringSize;
		memmove(destination, source, moveSize);

		deprecatedIndex += moveSize;
		validIndex += moveSize;
		validIndex = GetFirstValidIndex(validIndex);
	}
	g_currentIndex = deprecatedIndex;
}

//保存字符串，参数1是字符串占用字节
//参数：
//	const char *source [in]要保存字符串的首地址，中间可以有多个\0
//	unsigned char stringSize [in]要保存字符串的size
//	bool hasEscapeChar [in]要保存字符串是否包含转义字符
//返回值：
//	char *	返回管理地址
//caller负责检查字符串的size小于等于LENGTH+1，以及剩余空间是否足够
char *SaveString(const char *source, unsigned char stringSize, bool hasEscapeChar) {
	char *destination = g_szBuff + g_currentIndex;
	Item *item = (Item *)destination;
	item->stringSize = stringSize;
	item->isUsed = true;
	item->hasEscapeChar = hasEscapeChar;
	g_currentIndex += BIAS;

	memmove(g_szBuff + g_currentIndex, source, stringSize);
	g_currentIndex += stringSize;	
	return destination;
}

//添加字符串功能,返回item在g_szBuff中的地址，空间不足返回NULL
//参数：
//	const char *source [in]要添加字符串的首地址，中间可以有多个\0
//	unsigned char stringSize [in]要添加字符串的size
//	bool hasEscapeChar [in]要添加字符串是否包含转义字符
//返回值：
//	char *	返回管理地址
//caller负责检查字符串的size小于等于LENGTH+1，以及剩余空间是否足够
char *AddString(const char *source, unsigned char stringSize, bool hasEscapeChar) {
	char *itemAddress = NULL;
	if (IsSpaceEnough(stringSize + BIAS)) {
		itemAddress = SaveString(source, stringSize, hasEscapeChar);
	}
	else {
		Defragment();
		if (IsSpaceEnough(stringSize + BIAS)) {
			itemAddress = SaveString(source, stringSize, hasEscapeChar);
		}
	}
	return itemAddress;
}

//在itemTable中检查传入的item是否存在
//参数：
//	Item *item [in]要搜索字符串的管理地址
//返回值：
//	bool	存在返回true，否则返回false
bool SearchItem(Item *item) {
	bool found = false;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		if (!g_itemTable[i]) {
			break;
		}
		if (g_itemTable[i] == item) {
			found = true;
			break;
		}
	}
	return found;
}

//移除item，返回true代表成功,false代表失败
//参数：
//	Item *item [in]要移除字符串的管理地址
//返回值：
//	bool	返回true代表成功,false代表失败
bool RemoveItem(Item *item) {
	bool found = SearchItem(item);
	if (found) {
		item->isUsed = false;
	}
	return found;
}

//修改字符串,调用者检查oldItem是否存在SearchItem(oldItem)，字符串的newStringSize <= LENGTH + 1
//参数：
//	Item *oldItem [in]要修改字符串的管理地址
//	const char *newString新字符串的地址
//	unsigned char newStringSize [in]新字符串的size
//	bool hasEscapeChar [in]新字符串是否包含转义字符
//返回值：
//	bool 返回true代表成功,false代表失败
//caller负责检查oldItem是否有效，检查新字符串的size小于等于LENGTH+1，以及剩余空间是否足够
bool ModifyItem(Item *oldItem, const char *newString, unsigned char newStringSize, bool hasEscapeChar) {
	unsigned char oldStringSize = oldItem->stringSize;
	bool oldHasEscapeChar = oldItem->hasEscapeChar;
	char *old = (char *)oldItem;

	if (newStringSize == oldStringSize) {
		memmove(old + BIAS, newString, newStringSize);
		return true;
	}
	else if (newStringSize < oldStringSize) {
		memmove(old + BIAS, newString, newStringSize);
		for (int i = newStringSize; i < oldStringSize; ++i) {
			(old + BIAS)[i] = '\0';//多余的字节用0填充，遍历时既不属于valid，也不属于deprecated									
		}
		oldItem->stringSize = newStringSize;//改变大小
		return true;
	}
	else {
		if (IsSpaceEnough(newStringSize + BIAS)) {
			RemoveItem(oldItem);
			AddString(newString, newStringSize, hasEscapeChar);
			return true;
		}
		else {
			char tempItem[LENGTH + 1 + BIAS] = { '\0' };
			memmove(tempItem, oldItem, oldItem->stringSize + BIAS);
			RemoveItem(oldItem);
			Defragment();
			if (IsSpaceEnough(newStringSize + BIAS)) {
				AddString(newString, newStringSize, hasEscapeChar);
				return true;
			}
			else {
				AddString(tempItem + BIAS, oldStringSize, oldHasEscapeChar);
				return false;
			}
		}
	}
}

//读取并丢弃一行中多余的输出
//参数：
//	无
//返回值：
//	无
void EatLine() {
	while (getchar() != '\n') {

	}
}

//根据传入的字符数打印空格或换行
//参数：
//	int count [in]已经打印的数目
//返回值：
//	bool 返回true代表继续显示,false代表不继续
bool ControlLineByCount(int count) {
	if (count % 160 == 0) {
		printf("\n继续显示吗？按N不继续，其他键继续：");
		
		int ch = getchar();
		EatLine();
		if (ch == 'N' || ch == 'n') {
			return false;
		}
	}
	else if (count % 16 == 0) {
		printf("\n");
	}
	else if (count % 8 == 0) {
		printf("  ");
	}
	else {
		printf(" ");
	}
	return true;
}

//显示存储信息
//参数：
//	无
//返回值：
//	无
void ShowInformation() {
	int validIndex = GetFirstValidIndex(0);
	int lastIndex = 0;
	int count = 0;

	while (validIndex >= 0) {
		if (lastIndex != validIndex) {
			for (int i = lastIndex; i < validIndex; ++i) {
				putchar('F');//空闲区域
				if (!ControlLineByCount(++count)) {
					return;
				}
			}
		}
		Item *item = (Item *)(g_szBuff + validIndex);
		int stringSize = item->stringSize;
		for (int i = 0; i < stringSize+ BIAS; ++i) {
			putchar('U');//占用区域
			if (!ControlLineByCount(++count)) {
				return;
			}
		}
		lastIndex = validIndex + stringSize + BIAS;
		validIndex = GetFirstValidIndex(lastIndex);
	}
	for (int i = lastIndex; i < SIZE; ++i) {
		putchar('F');//空闲区域
		if (!ControlLineByCount(++count)) {
			return;
		}
	}
}

//打印字符串，中间可以有多个'\0'，所以需要参数size，代表字符串占用字节数
//参数：
//	const char *str [in]要打印字符串
//	int size [in]字符串占用字节数
//返回值：
//	无
// \\ \' \" \? 转义字符可以直接打印，不影响排版，所以不特殊对待，当成普通字符
void PrintEscapeChar(const char *str, int size) {
	for (int i = 0; i < size-1; ++i) {
		char ch = str[i];
		switch (ch) {
		case '\a':
			printf("\\a");
			break;
		case '\b':
			printf("\\b");
			break;
		case '\f':
			printf("\\f");
			break;
		case '\n':
			printf("\\n");
			break;
		case '\r':
			printf("\\r");
			break;
		case '\t':
			printf("\\t");
			break;
		case '\v':
			printf("\\v");
			break;
			//case '\\':
			//	printf("\\");//not need
			//	break;
			//case '\'':
			//	printf("\'");//not need
			//	break;
			//case '\"':
			//	printf("\"");//not need
			//	break;
			//case '\?':
			//	printf("?");//not need
			//	break;
		case '\0':
			printf("\\0");
			break;
		default:
			putchar(ch);
			break;
		}
		//putchar(' ');
	}
}

//在content中寻找是否存在subContent
//参数：
//	const char *content [in]要被搜索的字符串
//	int contentSize [in]要被搜索的字符串的size
//	const char *subContent [in]要搜索的字符串
//	const char *subContentSize [in]要被搜索的字符串的size
//返回值：
//	bool 返回true代表找到,false代表没找到
//caller确保content和subContent有效，且contentSize大于等于subContentSize
bool SearchPattern(const char *content, int contentSize, const char *subContent, int subContentSize) {
	int count = 0;
	while (count <= contentSize - subContentSize) {
		bool match = true;
		for (int j = 0; j < subContentSize-1; ++j) {
			if (subContent[j] == content[j + count]) {
				//bool found = true;//调试看效果，无实际作用
			}
			else {
				match = false;
				++count;
				break;
			}
		}
		if (match) {
			return true;
		}
	}
	return false;
}

//查寻字符串,支持模糊查找,支持部分转义序列。找到则打印相关内容
//参数：
//	const char *subString [in]要匹配的字符串,中间可以有多个\0
//	int subStringSize [in]要匹配的字符串的size
//返回值：
//	无
void QueryStringByContentWithEscapeChar(const char *subString, int subStringSize) {
	bool found = false;
	Item *item = NULL;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		item = g_itemTable[i];
		if (item) {
			if (strlen(subString) + 1 == (size_t)subStringSize) {//说明没有\0,直接调用strstr
				if (strstr((char *)item + BIAS, subString)) {
					found = true;
					printf("管理地址：%p, 内容：", item);
					PrintEscapeChar((char *)item + BIAS, item->stringSize);
				}				
			}
			else {//说明中间有\0
				const char *str = (char *)item + BIAS;				
				if (SearchPattern(str, item->stringSize, subString, subStringSize)) {
					found = true;
					printf("管理地址：%p, 内容：", item);
					PrintEscapeChar((char *)item + BIAS, item->stringSize);
				}				
			}	
			printf("\n");
		}
	}
	if (!found) {
		printf("未找到匹配的字符串\n");
	}
}

//查寻字符串,支持模糊查找
//参数：
//	const char *subString [in]要匹配的字符串
//返回值：
//	无
void QueryStringByContent(const char *subString) {
	bool found = false;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		Item *item = g_itemTable[i];
		if (item) {
			if (strstr((char *)item + BIAS, subString)) {
				found = true;
				printf("管理地址：%p, 内容：%s\n", item, (char *)item + BIAS);
			}
		}
	}
	if (!found) {
		printf("未找到匹配的字符串\n");
	}
}


//显示每个字母的出现次数和比例
//参数：
//	无
//返回值：
//	无
void ShowEachCharInformation() {
	int alphabet[26 + 6 + 26] = {0};//ASCII中Z和a之间有6个字符，将来遍历时直接忽略这6个下标
	int allCount = 0;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		if (g_itemTable[i]) {
			Item *item = g_itemTable[i];
			for (int j = 0; j < item->stringSize; ++j) {
				++allCount;
				char ch = ((char*)item+BIAS)[j];
				if (isalpha(ch)) {
					++alphabet[ch - 'A'];
				}
			}
		}
	}
	if (allCount) {
		for (int i = 0; i < 26; ++i) {
			printf("%c出现%5d次，占比%.2f%%\n", 'A' + i, alphabet[i], 100 * (double)alphabet[i] / allCount);
		}
		for (int i = 0; i < 26; ++i) {
			printf("%c出现%5d次，占比%.2f%%\n", 'a' + i, alphabet[i + 26 + 6], 100 * (double)alphabet[i + 26 + 6] / allCount);
		}
	}
	else {
		printf("尚未有数据\n");
	}
}
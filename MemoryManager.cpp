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

#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct _item {
	unsigned char flagAndsize;//最高bit为1代表空间被占用，0代表空间被释放。其他7bit代表字符串占用字节数
	char *str;
};
typedef _item Item;

char g_szBuff[1024 * 1024];
char *stringTable[SIZE / 2] = {};//保存所有字符串的地址，遍历时发现NULL，则停止遍历
int currentIndex = 0;//当前可用位置的下标，新增字符串时保存在此处


//字符串长度是否符合要求
bool IsLengthSuit(int length) {
	return length <= MAX_LENGTH;
}

//空间是否足够存放
bool IsSpaceEnough(int need) {
	bool enough = SIZE - currentIndex >= need;
	return enough;
}

//获取对应字符串的占用字节数，要传入正确的index
int GetStringSizeByIndex(int index) {
	unsigned char flagAndSize = g_szBuff[index];
	int size = flagAndSize & 0x7F;
	return size;
}

//获取对应字符串占是否任在使用，要传入正确的index
bool GetStringFlagByIndex(int index) {
	unsigned char flagAndSize = g_szBuff[index];
	int used = flagAndSize & 0x80;
	return used;
}

//测试字符串是否有效
bool IsValidString(char *source) {
	bool valid = true;
	if (source > g_szBuff && source < g_szBuff + currentIndex - 1) {//范围正确		
		bool used = GetStringFlagByIndex(source - 1 - g_szBuff);
		int size = GetStringSizeByIndex(source - 1 - g_szBuff);		

		if (used && size && source[size - 1] == '\0') {	//状态，大小，结尾都正确
			for (int i = 0; i < size - 1; ++i) {//中间部分不存在0
				if (source[i] == '\0') {
					valid = false;
				}
			}
		}
		else {
			valid = false;
		}
	}
	else {
		valid = false;
	}

	return valid;
}

//测试字符串是否被弃用，即标记为已删除
bool IsDeprecatedString(char *source) {
	bool deprecated = true;
	if (source > g_szBuff &&source < g_szBuff + currentIndex - 1) {	//范围正确
		bool used = GetStringFlagByIndex(source - 1 - g_szBuff);
		int size = GetStringSizeByIndex(source - 1 - g_szBuff);

		if (!used && size && source[size - 1] == '\0') {//状态，大小，结尾都正确
			for (int i = 0; i < size - 1; ++i) {//中间部分不存在0
				if (source[i] == '\0') {
					deprecated = false;
				}
			}
		}
		else {
			deprecated = false;
		}
	}
	else {
		deprecated = false;
	}

	return deprecated;
}

//寻找第一个被标记为已删除字符串的下标,参数代表从哪里开始搜索,如果不存在，返回-1
int GetFirstDeprecatedIndex(int startIndex) {
	for (int i = startIndex; i < currentIndex; ++i) {
		if (IsDeprecatedString(g_szBuff + i)) {
			return i -1;//flagAndsize在字符串前面1字节
		}
	}
	return -1;
}

//寻找第一个已使用位置的下标,参数代表从哪里开始搜索,如果空间尚未存放任何有效字符串，返回-1
int GetFirstValidIndex(int startIndex) {
	for (int i = startIndex; i < currentIndex; ++i) {
		if (IsValidString(g_szBuff + i)) {
			return i - 1;//flagAndsize在字符串前面1字节
		}
	}
	return -1;
}

//碎片整理
void Defragment() {
	int deprecatedIndex = GetFirstDeprecatedIndex(0);
	if (deprecatedIndex < 0) {
		return;
	}
	int size = GetStringSizeByIndex(deprecatedIndex);
	int validIndex = GetFirstValidIndex(deprecatedIndex + 1 + size + 1);

	while (deprecatedIndex >= 0 && validIndex > deprecatedIndex) {
		char *destination = g_szBuff + deprecatedIndex;
		const char *source = g_szBuff + validIndex;
		size = GetStringSizeByIndex(validIndex);

		memmove(destination, source, size + 1);//flagAndsize也要拷贝
		deprecatedIndex += size + 1;
		validIndex += size + 1;
		validIndex = GetFirstValidIndex(validIndex);
	}
	currentIndex = deprecatedIndex;
}

//保存字符串，参数1是字符串占用字节
char *SaveString(int size, const char *source) {
	g_szBuff[currentIndex] = (char)size;
	g_szBuff[currentIndex] |= 0x80;//最高bit设为1，代表空间被使用
	++currentIndex;
	char *destination = g_szBuff + currentIndex;
	strcpy(destination, source);
	currentIndex += size;
	return destination;
}

//添加字符串功能,返回字符串在缓冲区中的地址，失败返回NULL
char *AddString(const char *source) {
	char *address = NULL;
	int length = strlen(source);
	if (IsLengthSuit(length)) {
		if (IsSpaceEnough(length + 2)) {
			address = SaveString(length + 1, source);
		}
		else {
			Defragment();
			if (IsSpaceEnough(length + 2)) {
				address = SaveString(length + 1, source);
			}
			else {
				printf("无法保存，剩余字节：%d, 需要字节：%d\n", SIZE - currentIndex, length + 2);
			}
		}
	}
	else {
		printf("字符串长度必须小于等于%d\n", MAX_LENGTH);
	}

	return address;
}

//移除字符串，返回true代表成功
bool RemoveString(char *source) {
	if (IsValidString(source)) {
		Item *item = (Item *)(source - 1);
		item->flagAndsize &= 0x7F;
		return true;
	}

	printf("欲移除的字符串地址无效\n");
	return false;
}

//修改字符串
bool ModifyString(char *oldString,const char *newString) {
	if (IsValidString(oldString)) {
		int oldLength = strlen(oldString);
		int newLength = strlen(newString);
		if (newLength <= MAX_LENGTH) {
			if (newLength == oldLength) {
				strcpy(oldString, newString);
				return true;
			}
			else if (newLength < oldLength) {
				strcpy(oldString, newString);
				for (int i = newLength + 1; i < oldLength - 1; ++i) {
					oldString[i] = '\0';//多余的字节用0填充，遍历时既不属于valid，也不属于deprecated
					*(oldString - 1) = (unsigned char)(newLength + 1);//改变大小
					*(oldString - 1) |= 0x80;
				}
				return true;
			}
			else {
				if (IsSpaceEnough(newLength + 2)) {
					RemoveString(oldString);
					AddString(newString);
					return true;
				}
				else {
					char temp[MAX_LENGTH + 2] = {};
					strcpy(temp, oldString);
					RemoveString(oldString);
					Defragment();
					if (IsSpaceEnough(newLength + 2)) {
						AddString(newString);
						return true;
					}
					else {
						AddString(temp);
						printf("无法保存，剩余字节：%d, 需要字节：%d\n", SIZE - currentIndex, newLength + 2);
					}					
				}
			}
		}
		else {
			printf("字符串长度必须小于等于%d\n", MAX_LENGTH);
		}		
	}
	else {
		printf("字符串地址无效\n");
	}
	return false;
}

//读取并丢弃一行中多余的输出
void EatLine() {
	while (getchar() != '\n') {

	}
}

//根据传入的字符数打印空格或换行
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

		int size = GetStringSizeByIndex(validIndex);
		for (int i = 0; i < size+1; ++i) {
			putchar('U');//占用区域
			if (!ControlLineByCount(++count)) {
				return;
			}
		}
		lastIndex = validIndex + size + 1;		 
		validIndex = GetFirstValidIndex(lastIndex);
	}
	for (int i = lastIndex; i < SIZE; ++i) {
		putchar('F');//空闲区域
		if (!ControlLineByCount(++count)) {
			return;
		}
	}
}

//清空stringTable内容
void EmptyStringTable(){
	for (int i = 0; i < sizeof(stringTable)/ sizeof(stringTable[0]); ++i) {
		stringTable[i] = NULL;
	}
}

//获取所有字符串首地址，存在stringTable里
void GetAllString() {
	EmptyStringTable();
	int validIndex = GetFirstValidIndex(0);
	int lastIndex = 0;
	int i = 0;

	while (validIndex >= 0) {		
		stringTable[i] = g_szBuff + validIndex + 1;
		++i;
		int size = GetStringSizeByIndex(validIndex);
		lastIndex = validIndex + size + 1;
		validIndex = GetFirstValidIndex(lastIndex);
	}
}

//查寻字符串,支持模糊查找
void QueryStringByContent(const char *subString) {
	bool found = false;
	GetAllString();
	for (int i = 0; i < sizeof(stringTable) / sizeof(stringTable[0]); ++i) {
		if (stringTable[i]) {
			if (strstr(stringTable[i], subString)) {
				found = true;
				printf("地址：%p, %s\n", stringTable[i], stringTable[i]);
			}
		}
	}
	if (!found) {
		printf("未找到匹配的字符串\n");
	}
}

//显示每个字母的出现次数和比例
void ShowEachCharInformation() {
	int alphabet[26 + 6 + 26] = {};//ASCII中Z和a之间有6个字符，将来遍历时直接忽略这6个下标
	int allCount = 0;
	GetAllString();
	for (int i = 0; i < sizeof(stringTable) / sizeof(stringTable[0]); ++i) {
		if (stringTable[i]) {
			const char *pString = stringTable[i];
			while (*pString != '\0') {
				++allCount;
				if (isalpha(*pString)) {
					++alphabet[*pString - 'A'];					
				}
				++pString;
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
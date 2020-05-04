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
#define BIAS 2

#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>



char g_szBuff[1024 * 1024];
Item *g_itemTable[SIZE / 2] = {NULL};//保存所有字符串的地址，遍历时发现NULL，则停止遍历
int g_nCurrentIndex = 0;//当前可用位置的下标，新增字符串时保存在此处
int g_nValidIndex = -1;
int g_nDeprecatedIndex = -1;


//判断字符串size是否符合要求
//参数：
//	int nSize [in]字符串size
//返回值：
//	bool：nSize合适返回true，否则返回false
bool IsSizeSuit(int nSize) {
	return nSize <= LENGTH + 1;
}

//空间是否足够存放
//参数：
//	int nNeed [in]需要的字节数
//返回值：
//	bool：足够返回true，否则返回false
bool IsSpaceEnough(int nNeed) {
	bool bIsEnough = SIZE - g_nCurrentIndex >= nNeed;
	return bIsEnough;
}


//寻找第一个被标记为已删除字符串的下标,参数代表从哪里开始搜索,如果不存在，返回-1
//参数：
//	int nStartIndex [in]从哪个下标开始搜索
//返回值：
//	int：返回找到的下标，没找到返回-1
int GetFirstDeprecatedIndex(int nStartIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (nStartIndex < g_nCurrentIndex) {
		unsigned char chStringSize = g_szBuff[nStartIndex];
		bool bIsUsed = g_szBuff[nStartIndex + 1];
		if (!chStringSize) {//长字符串改短，会多出'\0'，算是未使用的空间，可以覆盖
			return nStartIndex;
		}
		else {
			if (!bIsUsed) {
				return nStartIndex;
			}
			else {
				nStartIndex += BIAS + chStringSize;//加上头部Item的字节数
			}
		}
	}
	return -1;
}

//寻找第一个已使用位置的下标,参数代表从哪里开始搜索,如果空间尚未存放任何有效字符串，返回-1
//参数：
//	int nStartIndex [in]从哪个下标开始搜索
//返回值：
//	int：返回找到的下标，没找到返回-1 
int GetFirstValidIndex(int nStartIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (nStartIndex < g_nCurrentIndex) {
		unsigned char stringSize = g_szBuff[nStartIndex];
		bool isUsed = g_szBuff[nStartIndex + 1];
		if (!stringSize) {//长字符串改短，会多出'\0'，不算有效字符，要跳过
			++nStartIndex;
		}
		else {
			if (isUsed) {
				return nStartIndex;
			}
			else {
				nStartIndex += BIAS + stringSize;//加上头部Item的字节数
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
	int nValidIndex = GetFirstValidIndex(0);
	int i = 0;

	while (nValidIndex >= 0) {
		Item *pItem = (Item *)(g_szBuff + nValidIndex);
		g_itemTable[i] = pItem;
		++i;
		nValidIndex = GetFirstValidIndex(nValidIndex + BIAS + pItem->nStringSize);
	}
}

//碎片整理，使g_szBuff左侧都是已使用空间，右侧都是未使用空间
//参数：
//	无
//返回值：
//	无
void Defragment() {
	int nDeprecatedIndex = GetFirstDeprecatedIndex(0);
	if (nDeprecatedIndex < 0) {//无碎片，直接返回
		return;
	}
	Item *pItem = (Item *)(g_szBuff + nDeprecatedIndex);
	int nStringSize = pItem->nStringSize;
	int nValidIndex = GetFirstValidIndex(nDeprecatedIndex + BIAS + nStringSize);

	while (nDeprecatedIndex >= 0 && nValidIndex > nDeprecatedIndex) {
		char *pDestination = g_szBuff + nDeprecatedIndex;
		Item *pSource = (Item *)(g_szBuff + nValidIndex);
		int nMoveSize = BIAS + pSource->nStringSize;//加上头部Item的字节数
		memmove(pDestination, pSource, nMoveSize);

		nDeprecatedIndex += nMoveSize;//前移头部Item和string的总字节数
		nValidIndex += nMoveSize;
		nValidIndex = GetFirstValidIndex(nValidIndex);//定位下一个有效下标
	}
	g_nCurrentIndex = nDeprecatedIndex;//下一个新字符串的保存在此下标
}

//保存字符串，参数1是字符串占用字节
//参数：
//	const char *pSource [in]要保存字符串的首地址，中间可以有多个\0
//	unsigned char nStringSize [in]要保存字符串的size
//	bool bHasEscapeChar [in]要保存字符串是否包含转义字符
//返回值：
//	char *	返回管理地址
//caller负责检查字符串的size小于等于LENGTH+1，以及剩余空间是否足够
char *SaveString(const char *pSource, unsigned char nStringSize) {
	char *destination = g_szBuff + g_nCurrentIndex;
	Item *item = (Item *)destination;
	item->nStringSize = nStringSize;
	item->isUsed = true;
	g_nCurrentIndex += BIAS;//字符串保存在此下标指示的位置

	memmove(g_szBuff + g_nCurrentIndex, pSource, nStringSize);
	g_nCurrentIndex += nStringSize;//更新下标，下一次新增在此下标
	return destination;
}

//添加字符串功能,返回item在g_szBuff中的地址，空间不足返回NULL
//参数：
//	const char *pSource [in]要添加字符串的首地址，中间可以有多个\0
//	unsigned char nStringSize [in]要添加字符串的size
//	bool bHasEscapeChar [in]要添加字符串是否包含转义字符
//返回值：
//	size_t	返回下标
//caller负责检查字符串的size小于等于LENGTH+1，以及剩余空间是否足够
size_t AddString(const char *pSource, unsigned char chStringSize) {
	char *itemAddress = NULL;
	if (IsSpaceEnough(chStringSize + BIAS)) {
		itemAddress = SaveString(pSource, chStringSize);
	}
	else {
		Defragment();
		if (IsSpaceEnough(chStringSize + BIAS)) {
			itemAddress = SaveString(pSource, chStringSize);
		}
	}
	return (size_t)itemAddress - (size_t)g_szBuff;
}

//在itemTable中检查传入的item是否存在
//参数：
//	Item *pItem [in]要搜索字符串的管理地址
//返回值：
//	bool	存在返回true，否则返回false
bool SearchItem(Item *pItem) {
	bool bFound = false;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		if (!g_itemTable[i]) {
			break;
		}
		if (g_itemTable[i] == pItem) {
			bFound = true;
			break;
		}
	}
	return bFound;
}

//移除item，返回true代表成功,false代表失败
//参数：
//	Item *pItem [in]要移除字符串的管理地址
//返回值：
//	bool	返回true代表成功,false代表失败
bool RemoveItem(Item *pItem) {
	bool bFound = SearchItem(pItem);
	if (bFound) {
		pItem->isUsed = false;
	}
	return bFound;
}

//修改字符串,调用者检查pOldItem是否存在SearchItem(pOldItem)，字符串的chNewStringSize <= LENGTH + 1
//参数：
//	Item *pOldItem [in]要修改字符串的管理地址
//	const char *pNewString新字符串的地址
//	unsigned char chNewStringSize [in]新字符串的size
//	bool bHasEscapeChar [in]新字符串是否包含转义字符
//返回值：
//	bool 返回true代表成功,false代表失败
//caller负责检查pOldItem是否有效，检查新字符串的size小于等于LENGTH+1，以及剩余空间是否足够
bool ModifyItem(Item *pOldItem, const char *pNewString, unsigned char chNewStringSize) {
	unsigned char chOldStringSize = pOldItem->nStringSize;
	char *pOld = (char *)pOldItem;

	if (chNewStringSize == chOldStringSize) {
		memmove(pOld + BIAS, pNewString, chNewStringSize);
		return true;
	}
	else if (chNewStringSize < chOldStringSize) {
		memmove(pOld + BIAS, pNewString, chNewStringSize);
		for (int i = chNewStringSize; i < chOldStringSize; ++i) {
			(pOld + BIAS)[i] = '\0';//多余的字节用0填充，遍历时既不属于valid，也不属于deprecated									
		}
		pOldItem->nStringSize = chNewStringSize;//改变大小
		return true;
	}
	else {
		if (IsSpaceEnough(chNewStringSize + BIAS)) {
			RemoveItem(pOldItem);
			AddString(pNewString, chNewStringSize);
			return true;
		}
		else {
			char tempItem[LENGTH + 1 + BIAS] = { '\0' };
			memmove(tempItem, pOldItem, pOldItem->nStringSize + BIAS);
			RemoveItem(pOldItem);
			Defragment();
			if (IsSpaceEnough(chNewStringSize + BIAS)) {
				AddString(pNewString, chNewStringSize);
				return true;
			}
			else {
				AddString(tempItem + BIAS, chOldStringSize);
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
//	int nCount [in]已经打印的数目
//返回值：
//	bool 返回true代表继续显示,false代表不继续
bool ControlLineByCount(int nCount) {
	if (nCount % 1024 == 0) {
		printf("\n继续显示吗？按N不继续，其他键继续：");

		int ch = getchar();
		EatLine();
		if (ch == 'N' || ch == 'n') {
			return false;
		}
	}
	else if (nCount % 16 == 0) {
		printf("\n");
	}
	else if (nCount % 8 == 0) {
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
	int nValidIndex = GetFirstValidIndex(0);
	int nLastIndex = 0;
	int nCount = 0;

	while (nValidIndex >= 0) {//有效下标大于0，则有数据
		if (nLastIndex != nValidIndex) {
			for (int i = nLastIndex; i < nValidIndex; ++i) {
				putchar('F');//空闲区域
				if (!ControlLineByCount(++nCount)) {//是否继续打印
					return;
				}
			}
		}
		Item *pItem = (Item *)(g_szBuff + nValidIndex);
		int nStringSize = pItem->nStringSize;
		for (int i = 0; i < nStringSize + BIAS; ++i) {
			putchar('U');//占用区域
			if (!ControlLineByCount(++nCount)) {//是否继续打印
				return;
			}
		}
		nLastIndex = nValidIndex + nStringSize + BIAS;//字符串内容和item头部不搜索
		nValidIndex = GetFirstValidIndex(nLastIndex);//继续搜索有效下标
	}
	for (int i = nLastIndex; i < SIZE; ++i) {
		putchar('F');//空闲区域
		if (!ControlLineByCount(++nCount)) {
			return;
		}
	}
}



//在pContent表示的字符串中寻找是否存在pSubContent表示的字符串
//参数：
//	const char *pContent [in]要被搜索的字符串
//	int nContentSize [in]要被搜索的字符串的size
//	const char *pSubContent [in]要搜索的字符串
//	const char *nSubContentSize [in]要被搜索的字符串的size
//返回值：
//	bool 返回true代表找到,false代表没找到
//caller确保pContent和pSubContent有效，且nContentSize大于等于nSubContentSize
bool SearchPattern(const char *pContent, int nContentSize, const char *pSubContent, int nSubContentSize) {
	int nIndex = 0;//从pContent的哪个下标开始比较
	while (nIndex <= nContentSize - nSubContentSize) {//剩余长度不够了，肯定不匹配
		bool bMatch = true;
		for (int j = 0; j < nSubContentSize - 1; ++j) {
			if (pSubContent[j] == pContent[j + nIndex]) {//有nSubContentSize个字符都相同
				//bool found = true;//调试看效果，无实际作用
			}
			else {
				bMatch = false;
				++nIndex;//前进到下一个下标
				break;
			}
		}
		if (bMatch) {
			return true;
		}
	}
	return false;
}

//查寻字符串,支持模糊查找
//参数：
//	const char *pSubString [in]要匹配的字符串
//返回值：
//	无
void QueryStringByContent(const char *pSubString) {
	bool bFound = false;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		Item *pItem = g_itemTable[i];
		if (pItem) {
			if (strstr((char *)pItem + BIAS, pSubString)) {
				bFound = true;
				printf("管理地址：%p, 内容：%s\n", pItem, (char *)pItem + BIAS);
			}
		}
	}
	if (!bFound) {
		printf("未找到匹配的字符串\n");
	}
}


//显示每个字母的出现次数和比例
//参数：
//	无
//返回值：
//	无
void ShowEachCharInformation() {
	int chAlphabet[26 + 6 + 26] = { 0 };//ASCII中Z和a之间有6个字符，将来遍历时直接忽略这6个下标
	int nTotalCount = 0;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		if (g_itemTable[i]) {
			Item *pItem = g_itemTable[i];
			for (int j = 0; j < pItem->nStringSize; ++j) {
				++nTotalCount;
				char ch = ((char *)pItem + BIAS)[j];//获得一个字符串的每个字符
				if (isalpha(ch)) {
					++chAlphabet[ch - 'A'];//大写A在下标0，B在下标1，Z在下标25。小写a在下标32
				}
			}
		}
	}
	if (nTotalCount) {
		for (int i = 0; i < 26; ++i) {
			printf("%c出现%5d次，占比%.2f%%\n", 'A' + i, chAlphabet[i], 100 * (double)chAlphabet[i] / nTotalCount);
		}
		for (int i = 0; i < 26; ++i) {
			printf("%c出现%5d次，占比%.2f%%\n", 'a' + i, chAlphabet[i + 26 + 6], 100 * (double)chAlphabet[i + 26 + 6] / nTotalCount);
		}
	}
	else {
		printf("尚未有数据\n");
	}
}
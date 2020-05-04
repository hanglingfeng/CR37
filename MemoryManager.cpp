//��ָ�������ڴ洢�ַ���
//1.����: ���ַ��������Ӳ������100�ֽ�
//2.ɾ�������ַ�����ɾ������
//3.�޸ģ��޸�ָ�����ַ���������ռ䳤�Ȳ�����洢�������ط���ԭ�ַ�����Ϊ��ɾ��
//4.��ѯ�����ַ������ݲ����ַ����Ļ�����Ϣ��֧��ģ������
//5.ͳ�ƣ�ͳ��ÿ����ĸ�ĳ��ִ����ͱ�����
//6.��ʾ�洢��Ϣ����˳����ʾ�ѷ���(U)��δ����(F)��Դ,����10�ֽڿռ䣬��ʾUFUUFFFUUU
//7.��Ƭ������ɾ���г����˲������ġ���϶���������ʹ����Щ����϶�������ҿ���

#define _CRT_SECURE_NO_WARNINGS
#define SIZE 1024*1024
#define BIAS 2

#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>



char g_szBuff[1024 * 1024];
Item *g_itemTable[SIZE / 2] = {NULL};//���������ַ����ĵ�ַ������ʱ����NULL����ֹͣ����
int g_nCurrentIndex = 0;//��ǰ����λ�õ��±꣬�����ַ���ʱ�����ڴ˴�
int g_nValidIndex = -1;
int g_nDeprecatedIndex = -1;


//�ж��ַ���size�Ƿ����Ҫ��
//������
//	int nSize [in]�ַ���size
//����ֵ��
//	bool��nSize���ʷ���true�����򷵻�false
bool IsSizeSuit(int nSize) {
	return nSize <= LENGTH + 1;
}

//�ռ��Ƿ��㹻���
//������
//	int nNeed [in]��Ҫ���ֽ���
//����ֵ��
//	bool���㹻����true�����򷵻�false
bool IsSpaceEnough(int nNeed) {
	bool bIsEnough = SIZE - g_nCurrentIndex >= nNeed;
	return bIsEnough;
}


//Ѱ�ҵ�һ�������Ϊ��ɾ���ַ������±�,������������￪ʼ����,��������ڣ�����-1
//������
//	int nStartIndex [in]���ĸ��±꿪ʼ����
//����ֵ��
//	int�������ҵ����±꣬û�ҵ�����-1
int GetFirstDeprecatedIndex(int nStartIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (nStartIndex < g_nCurrentIndex) {
		unsigned char chStringSize = g_szBuff[nStartIndex];
		bool bIsUsed = g_szBuff[nStartIndex + 1];
		if (!chStringSize) {//���ַ����Ķ̣�����'\0'������δʹ�õĿռ䣬���Ը���
			return nStartIndex;
		}
		else {
			if (!bIsUsed) {
				return nStartIndex;
			}
			else {
				nStartIndex += BIAS + chStringSize;//����ͷ��Item���ֽ���
			}
		}
	}
	return -1;
}

//Ѱ�ҵ�һ����ʹ��λ�õ��±�,������������￪ʼ����,����ռ���δ����κ���Ч�ַ���������-1
//������
//	int nStartIndex [in]���ĸ��±꿪ʼ����
//����ֵ��
//	int�������ҵ����±꣬û�ҵ�����-1 
int GetFirstValidIndex(int nStartIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (nStartIndex < g_nCurrentIndex) {
		unsigned char stringSize = g_szBuff[nStartIndex];
		bool isUsed = g_szBuff[nStartIndex + 1];
		if (!stringSize) {//���ַ����Ķ̣�����'\0'��������Ч�ַ���Ҫ����
			++nStartIndex;
		}
		else {
			if (isUsed) {
				return nStartIndex;
			}
			else {
				nStartIndex += BIAS + stringSize;//����ͷ��Item���ֽ���
			}
		}
	}
	return -1;
}

//���g_itemTable����
//������
//	��
//����ֵ��
//	��
void EmptyItemTable() {
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		g_itemTable[i] = NULL;
	}
}

//��ȡ����item�׵�ַ������g_itemTable��
//������
//	��
//����ֵ��
//	��
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

//��Ƭ����ʹg_szBuff��඼����ʹ�ÿռ䣬�Ҳ඼��δʹ�ÿռ�
//������
//	��
//����ֵ��
//	��
void Defragment() {
	int nDeprecatedIndex = GetFirstDeprecatedIndex(0);
	if (nDeprecatedIndex < 0) {//����Ƭ��ֱ�ӷ���
		return;
	}
	Item *pItem = (Item *)(g_szBuff + nDeprecatedIndex);
	int nStringSize = pItem->nStringSize;
	int nValidIndex = GetFirstValidIndex(nDeprecatedIndex + BIAS + nStringSize);

	while (nDeprecatedIndex >= 0 && nValidIndex > nDeprecatedIndex) {
		char *pDestination = g_szBuff + nDeprecatedIndex;
		Item *pSource = (Item *)(g_szBuff + nValidIndex);
		int nMoveSize = BIAS + pSource->nStringSize;//����ͷ��Item���ֽ���
		memmove(pDestination, pSource, nMoveSize);

		nDeprecatedIndex += nMoveSize;//ǰ��ͷ��Item��string�����ֽ���
		nValidIndex += nMoveSize;
		nValidIndex = GetFirstValidIndex(nValidIndex);//��λ��һ����Ч�±�
	}
	g_nCurrentIndex = nDeprecatedIndex;//��һ�����ַ����ı����ڴ��±�
}

//�����ַ���������1���ַ���ռ���ֽ�
//������
//	const char *pSource [in]Ҫ�����ַ������׵�ַ���м�����ж��\0
//	unsigned char nStringSize [in]Ҫ�����ַ�����size
//	bool bHasEscapeChar [in]Ҫ�����ַ����Ƿ����ת���ַ�
//����ֵ��
//	char *	���ع����ַ
//caller�������ַ�����sizeС�ڵ���LENGTH+1���Լ�ʣ��ռ��Ƿ��㹻
char *SaveString(const char *pSource, unsigned char nStringSize) {
	char *destination = g_szBuff + g_nCurrentIndex;
	Item *item = (Item *)destination;
	item->nStringSize = nStringSize;
	item->isUsed = true;
	g_nCurrentIndex += BIAS;//�ַ��������ڴ��±�ָʾ��λ��

	memmove(g_szBuff + g_nCurrentIndex, pSource, nStringSize);
	g_nCurrentIndex += nStringSize;//�����±꣬��һ�������ڴ��±�
	return destination;
}

//����ַ�������,����item��g_szBuff�еĵ�ַ���ռ䲻�㷵��NULL
//������
//	const char *pSource [in]Ҫ����ַ������׵�ַ���м�����ж��\0
//	unsigned char nStringSize [in]Ҫ����ַ�����size
//	bool bHasEscapeChar [in]Ҫ����ַ����Ƿ����ת���ַ�
//����ֵ��
//	size_t	�����±�
//caller�������ַ�����sizeС�ڵ���LENGTH+1���Լ�ʣ��ռ��Ƿ��㹻
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

//��itemTable�м�鴫���item�Ƿ����
//������
//	Item *pItem [in]Ҫ�����ַ����Ĺ����ַ
//����ֵ��
//	bool	���ڷ���true�����򷵻�false
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

//�Ƴ�item������true����ɹ�,false����ʧ��
//������
//	Item *pItem [in]Ҫ�Ƴ��ַ����Ĺ����ַ
//����ֵ��
//	bool	����true����ɹ�,false����ʧ��
bool RemoveItem(Item *pItem) {
	bool bFound = SearchItem(pItem);
	if (bFound) {
		pItem->isUsed = false;
	}
	return bFound;
}

//�޸��ַ���,�����߼��pOldItem�Ƿ����SearchItem(pOldItem)���ַ�����chNewStringSize <= LENGTH + 1
//������
//	Item *pOldItem [in]Ҫ�޸��ַ����Ĺ����ַ
//	const char *pNewString���ַ����ĵ�ַ
//	unsigned char chNewStringSize [in]���ַ�����size
//	bool bHasEscapeChar [in]���ַ����Ƿ����ת���ַ�
//����ֵ��
//	bool ����true����ɹ�,false����ʧ��
//caller������pOldItem�Ƿ���Ч��������ַ�����sizeС�ڵ���LENGTH+1���Լ�ʣ��ռ��Ƿ��㹻
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
			(pOld + BIAS)[i] = '\0';//������ֽ���0��䣬����ʱ�Ȳ�����valid��Ҳ������deprecated									
		}
		pOldItem->nStringSize = chNewStringSize;//�ı��С
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

//��ȡ������һ���ж�������
//������
//	��
//����ֵ��
//	��
void EatLine() {
	while (getchar() != '\n') {

	}
}

//���ݴ�����ַ�����ӡ�ո����
//������
//	int nCount [in]�Ѿ���ӡ����Ŀ
//����ֵ��
//	bool ����true���������ʾ,false��������
bool ControlLineByCount(int nCount) {
	if (nCount % 1024 == 0) {
		printf("\n������ʾ�𣿰�N��������������������");

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

//��ʾ�洢��Ϣ
//������
//	��
//����ֵ��
//	��
void ShowInformation() {
	int nValidIndex = GetFirstValidIndex(0);
	int nLastIndex = 0;
	int nCount = 0;

	while (nValidIndex >= 0) {//��Ч�±����0����������
		if (nLastIndex != nValidIndex) {
			for (int i = nLastIndex; i < nValidIndex; ++i) {
				putchar('F');//��������
				if (!ControlLineByCount(++nCount)) {//�Ƿ������ӡ
					return;
				}
			}
		}
		Item *pItem = (Item *)(g_szBuff + nValidIndex);
		int nStringSize = pItem->nStringSize;
		for (int i = 0; i < nStringSize + BIAS; ++i) {
			putchar('U');//ռ������
			if (!ControlLineByCount(++nCount)) {//�Ƿ������ӡ
				return;
			}
		}
		nLastIndex = nValidIndex + nStringSize + BIAS;//�ַ������ݺ�itemͷ��������
		nValidIndex = GetFirstValidIndex(nLastIndex);//����������Ч�±�
	}
	for (int i = nLastIndex; i < SIZE; ++i) {
		putchar('F');//��������
		if (!ControlLineByCount(++nCount)) {
			return;
		}
	}
}



//��pContent��ʾ���ַ�����Ѱ���Ƿ����pSubContent��ʾ���ַ���
//������
//	const char *pContent [in]Ҫ���������ַ���
//	int nContentSize [in]Ҫ���������ַ�����size
//	const char *pSubContent [in]Ҫ�������ַ���
//	const char *nSubContentSize [in]Ҫ���������ַ�����size
//����ֵ��
//	bool ����true�����ҵ�,false����û�ҵ�
//callerȷ��pContent��pSubContent��Ч����nContentSize���ڵ���nSubContentSize
bool SearchPattern(const char *pContent, int nContentSize, const char *pSubContent, int nSubContentSize) {
	int nIndex = 0;//��pContent���ĸ��±꿪ʼ�Ƚ�
	while (nIndex <= nContentSize - nSubContentSize) {//ʣ�೤�Ȳ����ˣ��϶���ƥ��
		bool bMatch = true;
		for (int j = 0; j < nSubContentSize - 1; ++j) {
			if (pSubContent[j] == pContent[j + nIndex]) {//��nSubContentSize���ַ�����ͬ
				//bool found = true;//���Կ�Ч������ʵ������
			}
			else {
				bMatch = false;
				++nIndex;//ǰ������һ���±�
				break;
			}
		}
		if (bMatch) {
			return true;
		}
	}
	return false;
}

//��Ѱ�ַ���,֧��ģ������
//������
//	const char *pSubString [in]Ҫƥ����ַ���
//����ֵ��
//	��
void QueryStringByContent(const char *pSubString) {
	bool bFound = false;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		Item *pItem = g_itemTable[i];
		if (pItem) {
			if (strstr((char *)pItem + BIAS, pSubString)) {
				bFound = true;
				printf("�����ַ��%p, ���ݣ�%s\n", pItem, (char *)pItem + BIAS);
			}
		}
	}
	if (!bFound) {
		printf("δ�ҵ�ƥ����ַ���\n");
	}
}


//��ʾÿ����ĸ�ĳ��ִ����ͱ���
//������
//	��
//����ֵ��
//	��
void ShowEachCharInformation() {
	int chAlphabet[26 + 6 + 26] = { 0 };//ASCII��Z��a֮����6���ַ�����������ʱֱ�Ӻ�����6���±�
	int nTotalCount = 0;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		if (g_itemTable[i]) {
			Item *pItem = g_itemTable[i];
			for (int j = 0; j < pItem->nStringSize; ++j) {
				++nTotalCount;
				char ch = ((char *)pItem + BIAS)[j];//���һ���ַ�����ÿ���ַ�
				if (isalpha(ch)) {
					++chAlphabet[ch - 'A'];//��дA���±�0��B���±�1��Z���±�25��Сдa���±�32
				}
			}
		}
	}
	if (nTotalCount) {
		for (int i = 0; i < 26; ++i) {
			printf("%c����%5d�Σ�ռ��%.2f%%\n", 'A' + i, chAlphabet[i], 100 * (double)chAlphabet[i] / nTotalCount);
		}
		for (int i = 0; i < 26; ++i) {
			printf("%c����%5d�Σ�ռ��%.2f%%\n", 'a' + i, chAlphabet[i + 26 + 6], 100 * (double)chAlphabet[i + 26 + 6] / nTotalCount);
		}
	}
	else {
		printf("��δ������\n");
	}
}
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
#define BIAS 3

#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>



char g_szBuff[1024 * 1024];
Item *g_itemTable[SIZE / 2] = {NULL};//���������ַ����ĵ�ַ������ʱ����NULL����ֹͣ����
int g_currentIndex = 0;//��ǰ����λ�õ��±꣬�����ַ���ʱ�����ڴ˴�
int g_validIndex = -1;
int g_deprecatedIndex = -1;


//�ж��ַ���size�Ƿ����Ҫ��
//������
//	int size [in]�ַ���size
//����ֵ��
//	bool��size���ʷ���true�����򷵻�false
bool IsSizeSuit(int size) {
	return size <= LENGTH+1;
}

//�ռ��Ƿ��㹻���
//������
//	int need [in]��Ҫ���ֽ���
//����ֵ��
//	bool���㹻����true�����򷵻�false
bool IsSpaceEnough(int need) {
	bool enough = SIZE - g_currentIndex >= need;
	return enough;
}


//Ѱ�ҵ�һ�������Ϊ��ɾ���ַ������±�,������������￪ʼ����,��������ڣ�����-1
//������
//	int startIndex [in]���ĸ��±꿪ʼ����
//����ֵ��
//	int�������ҵ����±꣬û�ҵ�����-1
int GetFirstDeprecatedIndex(int startIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (startIndex < g_currentIndex) {
		unsigned char stringSize = g_szBuff[startIndex];
		bool isUsed = g_szBuff[startIndex + 1];
		if (!stringSize) {//���ַ����Ķ̣�����'\0'������δʹ�õĿռ䣬���Ը���
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

//Ѱ�ҵ�һ����ʹ��λ�õ��±�,������������￪ʼ����,����ռ���δ����κ���Ч�ַ���������-1
//������
//	int startIndex [in]���ĸ��±꿪ʼ����
//����ֵ��
//	int�������ҵ����±꣬û�ҵ�����-1 
int GetFirstValidIndex(int startIndex) {
	if (g_szBuff[0] == '\0') {
		return -1;
	}
	while (startIndex < g_currentIndex) {
		unsigned char stringSize = g_szBuff[startIndex];
		bool isUsed = g_szBuff[startIndex + 1];
		if (!stringSize) {//���ַ����Ķ̣�����'\0'��������Ч�ַ���Ҫ����
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
	int validIndex = GetFirstValidIndex(0);
	int i = 0;

	while (validIndex >= 0) {
		Item *item = (Item *)(g_szBuff + validIndex);
		g_itemTable[i] = item;
		++i;
		validIndex = GetFirstValidIndex(validIndex + BIAS + item->stringSize);
	}
}

//��Ƭ����ʹg_szBuff��඼����ʹ�ÿռ䣬�Ҳ඼��δʹ�ÿռ�
//������
//	��
//����ֵ��
//	��
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

//�����ַ���������1���ַ���ռ���ֽ�
//������
//	const char *source [in]Ҫ�����ַ������׵�ַ���м�����ж��\0
//	unsigned char stringSize [in]Ҫ�����ַ�����size
//	bool hasEscapeChar [in]Ҫ�����ַ����Ƿ����ת���ַ�
//����ֵ��
//	char *	���ع����ַ
//caller�������ַ�����sizeС�ڵ���LENGTH+1���Լ�ʣ��ռ��Ƿ��㹻
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

//����ַ�������,����item��g_szBuff�еĵ�ַ���ռ䲻�㷵��NULL
//������
//	const char *source [in]Ҫ����ַ������׵�ַ���м�����ж��\0
//	unsigned char stringSize [in]Ҫ����ַ�����size
//	bool hasEscapeChar [in]Ҫ����ַ����Ƿ����ת���ַ�
//����ֵ��
//	char *	���ع����ַ
//caller�������ַ�����sizeС�ڵ���LENGTH+1���Լ�ʣ��ռ��Ƿ��㹻
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

//��itemTable�м�鴫���item�Ƿ����
//������
//	Item *item [in]Ҫ�����ַ����Ĺ����ַ
//����ֵ��
//	bool	���ڷ���true�����򷵻�false
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

//�Ƴ�item������true����ɹ�,false����ʧ��
//������
//	Item *item [in]Ҫ�Ƴ��ַ����Ĺ����ַ
//����ֵ��
//	bool	����true����ɹ�,false����ʧ��
bool RemoveItem(Item *item) {
	bool found = SearchItem(item);
	if (found) {
		item->isUsed = false;
	}
	return found;
}

//�޸��ַ���,�����߼��oldItem�Ƿ����SearchItem(oldItem)���ַ�����newStringSize <= LENGTH + 1
//������
//	Item *oldItem [in]Ҫ�޸��ַ����Ĺ����ַ
//	const char *newString���ַ����ĵ�ַ
//	unsigned char newStringSize [in]���ַ�����size
//	bool hasEscapeChar [in]���ַ����Ƿ����ת���ַ�
//����ֵ��
//	bool ����true����ɹ�,false����ʧ��
//caller������oldItem�Ƿ���Ч��������ַ�����sizeС�ڵ���LENGTH+1���Լ�ʣ��ռ��Ƿ��㹻
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
			(old + BIAS)[i] = '\0';//������ֽ���0��䣬����ʱ�Ȳ�����valid��Ҳ������deprecated									
		}
		oldItem->stringSize = newStringSize;//�ı��С
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
//	int count [in]�Ѿ���ӡ����Ŀ
//����ֵ��
//	bool ����true���������ʾ,false��������
bool ControlLineByCount(int count) {
	if (count % 160 == 0) {
		printf("\n������ʾ�𣿰�N��������������������");
		
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

//��ʾ�洢��Ϣ
//������
//	��
//����ֵ��
//	��
void ShowInformation() {
	int validIndex = GetFirstValidIndex(0);
	int lastIndex = 0;
	int count = 0;

	while (validIndex >= 0) {
		if (lastIndex != validIndex) {
			for (int i = lastIndex; i < validIndex; ++i) {
				putchar('F');//��������
				if (!ControlLineByCount(++count)) {
					return;
				}
			}
		}
		Item *item = (Item *)(g_szBuff + validIndex);
		int stringSize = item->stringSize;
		for (int i = 0; i < stringSize+ BIAS; ++i) {
			putchar('U');//ռ������
			if (!ControlLineByCount(++count)) {
				return;
			}
		}
		lastIndex = validIndex + stringSize + BIAS;
		validIndex = GetFirstValidIndex(lastIndex);
	}
	for (int i = lastIndex; i < SIZE; ++i) {
		putchar('F');//��������
		if (!ControlLineByCount(++count)) {
			return;
		}
	}
}

//��ӡ�ַ������м�����ж��'\0'��������Ҫ����size�������ַ���ռ���ֽ���
//������
//	const char *str [in]Ҫ��ӡ�ַ���
//	int size [in]�ַ���ռ���ֽ���
//����ֵ��
//	��
// \\ \' \" \? ת���ַ�����ֱ�Ӵ�ӡ����Ӱ���Ű棬���Բ�����Դ���������ͨ�ַ�
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

//��content��Ѱ���Ƿ����subContent
//������
//	const char *content [in]Ҫ���������ַ���
//	int contentSize [in]Ҫ���������ַ�����size
//	const char *subContent [in]Ҫ�������ַ���
//	const char *subContentSize [in]Ҫ���������ַ�����size
//����ֵ��
//	bool ����true�����ҵ�,false����û�ҵ�
//callerȷ��content��subContent��Ч����contentSize���ڵ���subContentSize
bool SearchPattern(const char *content, int contentSize, const char *subContent, int subContentSize) {
	int count = 0;
	while (count <= contentSize - subContentSize) {
		bool match = true;
		for (int j = 0; j < subContentSize-1; ++j) {
			if (subContent[j] == content[j + count]) {
				//bool found = true;//���Կ�Ч������ʵ������
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

//��Ѱ�ַ���,֧��ģ������,֧�ֲ���ת�����С��ҵ����ӡ�������
//������
//	const char *subString [in]Ҫƥ����ַ���,�м�����ж��\0
//	int subStringSize [in]Ҫƥ����ַ�����size
//����ֵ��
//	��
void QueryStringByContentWithEscapeChar(const char *subString, int subStringSize) {
	bool found = false;
	Item *item = NULL;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		item = g_itemTable[i];
		if (item) {
			if (strlen(subString) + 1 == (size_t)subStringSize) {//˵��û��\0,ֱ�ӵ���strstr
				if (strstr((char *)item + BIAS, subString)) {
					found = true;
					printf("�����ַ��%p, ���ݣ�", item);
					PrintEscapeChar((char *)item + BIAS, item->stringSize);
				}				
			}
			else {//˵���м���\0
				const char *str = (char *)item + BIAS;				
				if (SearchPattern(str, item->stringSize, subString, subStringSize)) {
					found = true;
					printf("�����ַ��%p, ���ݣ�", item);
					PrintEscapeChar((char *)item + BIAS, item->stringSize);
				}				
			}	
			printf("\n");
		}
	}
	if (!found) {
		printf("δ�ҵ�ƥ����ַ���\n");
	}
}

//��Ѱ�ַ���,֧��ģ������
//������
//	const char *subString [in]Ҫƥ����ַ���
//����ֵ��
//	��
void QueryStringByContent(const char *subString) {
	bool found = false;
	GetAllItem();
	for (int i = 0; i < sizeof(g_itemTable) / sizeof(g_itemTable[0]); ++i) {
		Item *item = g_itemTable[i];
		if (item) {
			if (strstr((char *)item + BIAS, subString)) {
				found = true;
				printf("�����ַ��%p, ���ݣ�%s\n", item, (char *)item + BIAS);
			}
		}
	}
	if (!found) {
		printf("δ�ҵ�ƥ����ַ���\n");
	}
}


//��ʾÿ����ĸ�ĳ��ִ����ͱ���
//������
//	��
//����ֵ��
//	��
void ShowEachCharInformation() {
	int alphabet[26 + 6 + 26] = {0};//ASCII��Z��a֮����6���ַ�����������ʱֱ�Ӻ�����6���±�
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
			printf("%c����%5d�Σ�ռ��%.2f%%\n", 'A' + i, alphabet[i], 100 * (double)alphabet[i] / allCount);
		}
		for (int i = 0; i < 26; ++i) {
			printf("%c����%5d�Σ�ռ��%.2f%%\n", 'a' + i, alphabet[i + 26 + 6], 100 * (double)alphabet[i + 26 + 6] / allCount);
		}
	}
	else {
		printf("��δ������\n");
	}
}
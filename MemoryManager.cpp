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

#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct _item {
	unsigned char flagAndsize;//���bitΪ1����ռ䱻ռ�ã�0����ռ䱻�ͷš�����7bit�����ַ���ռ���ֽ���
	char *str;
};
typedef _item Item;

char g_szBuff[1024 * 1024];
char *stringTable[SIZE / 2] = {};//���������ַ����ĵ�ַ������ʱ����NULL����ֹͣ����
int currentIndex = 0;//��ǰ����λ�õ��±꣬�����ַ���ʱ�����ڴ˴�


//�ַ��������Ƿ����Ҫ��
bool IsLengthSuit(int length) {
	return length <= MAX_LENGTH;
}

//�ռ��Ƿ��㹻���
bool IsSpaceEnough(int need) {
	bool enough = SIZE - currentIndex >= need;
	return enough;
}

//��ȡ��Ӧ�ַ�����ռ���ֽ�����Ҫ������ȷ��index
int GetStringSizeByIndex(int index) {
	unsigned char flagAndSize = g_szBuff[index];
	int size = flagAndSize & 0x7F;
	return size;
}

//��ȡ��Ӧ�ַ���ռ�Ƿ�����ʹ�ã�Ҫ������ȷ��index
bool GetStringFlagByIndex(int index) {
	unsigned char flagAndSize = g_szBuff[index];
	int used = flagAndSize & 0x80;
	return used;
}

//�����ַ����Ƿ���Ч
bool IsValidString(char *source) {
	bool valid = true;
	if (source > g_szBuff && source < g_szBuff + currentIndex - 1) {//��Χ��ȷ		
		bool used = GetStringFlagByIndex(source - 1 - g_szBuff);
		int size = GetStringSizeByIndex(source - 1 - g_szBuff);		

		if (used && size && source[size - 1] == '\0') {	//״̬����С����β����ȷ
			for (int i = 0; i < size - 1; ++i) {//�м䲿�ֲ�����0
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

//�����ַ����Ƿ����ã������Ϊ��ɾ��
bool IsDeprecatedString(char *source) {
	bool deprecated = true;
	if (source > g_szBuff &&source < g_szBuff + currentIndex - 1) {	//��Χ��ȷ
		bool used = GetStringFlagByIndex(source - 1 - g_szBuff);
		int size = GetStringSizeByIndex(source - 1 - g_szBuff);

		if (!used && size && source[size - 1] == '\0') {//״̬����С����β����ȷ
			for (int i = 0; i < size - 1; ++i) {//�м䲿�ֲ�����0
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

//Ѱ�ҵ�һ�������Ϊ��ɾ���ַ������±�,������������￪ʼ����,��������ڣ�����-1
int GetFirstDeprecatedIndex(int startIndex) {
	for (int i = startIndex; i < currentIndex; ++i) {
		if (IsDeprecatedString(g_szBuff + i)) {
			return i -1;//flagAndsize���ַ���ǰ��1�ֽ�
		}
	}
	return -1;
}

//Ѱ�ҵ�һ����ʹ��λ�õ��±�,������������￪ʼ����,����ռ���δ����κ���Ч�ַ���������-1
int GetFirstValidIndex(int startIndex) {
	for (int i = startIndex; i < currentIndex; ++i) {
		if (IsValidString(g_szBuff + i)) {
			return i - 1;//flagAndsize���ַ���ǰ��1�ֽ�
		}
	}
	return -1;
}

//��Ƭ����
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

		memmove(destination, source, size + 1);//flagAndsizeҲҪ����
		deprecatedIndex += size + 1;
		validIndex += size + 1;
		validIndex = GetFirstValidIndex(validIndex);
	}
	currentIndex = deprecatedIndex;
}

//�����ַ���������1���ַ���ռ���ֽ�
char *SaveString(int size, const char *source) {
	g_szBuff[currentIndex] = (char)size;
	g_szBuff[currentIndex] |= 0x80;//���bit��Ϊ1������ռ䱻ʹ��
	++currentIndex;
	char *destination = g_szBuff + currentIndex;
	strcpy(destination, source);
	currentIndex += size;
	return destination;
}

//����ַ�������,�����ַ����ڻ������еĵ�ַ��ʧ�ܷ���NULL
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
				printf("�޷����棬ʣ���ֽڣ�%d, ��Ҫ�ֽڣ�%d\n", SIZE - currentIndex, length + 2);
			}
		}
	}
	else {
		printf("�ַ������ȱ���С�ڵ���%d\n", MAX_LENGTH);
	}

	return address;
}

//�Ƴ��ַ���������true����ɹ�
bool RemoveString(char *source) {
	if (IsValidString(source)) {
		Item *item = (Item *)(source - 1);
		item->flagAndsize &= 0x7F;
		return true;
	}

	printf("���Ƴ����ַ�����ַ��Ч\n");
	return false;
}

//�޸��ַ���
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
					oldString[i] = '\0';//������ֽ���0��䣬����ʱ�Ȳ�����valid��Ҳ������deprecated
					*(oldString - 1) = (unsigned char)(newLength + 1);//�ı��С
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
						printf("�޷����棬ʣ���ֽڣ�%d, ��Ҫ�ֽڣ�%d\n", SIZE - currentIndex, newLength + 2);
					}					
				}
			}
		}
		else {
			printf("�ַ������ȱ���С�ڵ���%d\n", MAX_LENGTH);
		}		
	}
	else {
		printf("�ַ�����ַ��Ч\n");
	}
	return false;
}

//��ȡ������һ���ж�������
void EatLine() {
	while (getchar() != '\n') {

	}
}

//���ݴ�����ַ�����ӡ�ո����
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

		int size = GetStringSizeByIndex(validIndex);
		for (int i = 0; i < size+1; ++i) {
			putchar('U');//ռ������
			if (!ControlLineByCount(++count)) {
				return;
			}
		}
		lastIndex = validIndex + size + 1;		 
		validIndex = GetFirstValidIndex(lastIndex);
	}
	for (int i = lastIndex; i < SIZE; ++i) {
		putchar('F');//��������
		if (!ControlLineByCount(++count)) {
			return;
		}
	}
}

//���stringTable����
void EmptyStringTable(){
	for (int i = 0; i < sizeof(stringTable)/ sizeof(stringTable[0]); ++i) {
		stringTable[i] = NULL;
	}
}

//��ȡ�����ַ����׵�ַ������stringTable��
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

//��Ѱ�ַ���,֧��ģ������
void QueryStringByContent(const char *subString) {
	bool found = false;
	GetAllString();
	for (int i = 0; i < sizeof(stringTable) / sizeof(stringTable[0]); ++i) {
		if (stringTable[i]) {
			if (strstr(stringTable[i], subString)) {
				found = true;
				printf("��ַ��%p, %s\n", stringTable[i], stringTable[i]);
			}
		}
	}
	if (!found) {
		printf("δ�ҵ�ƥ����ַ���\n");
	}
}

//��ʾÿ����ĸ�ĳ��ִ����ͱ���
void ShowEachCharInformation() {
	int alphabet[26 + 6 + 26] = {};//ASCII��Z��a֮����6���ַ�����������ʱֱ�Ӻ�����6���±�
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
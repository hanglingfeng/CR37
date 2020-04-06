//�ռ�δ��ʹ������0��䣬�ַ�����βҲ��0��ע������

#define _CRT_SECURE_NO_WARNINGS
#define MAX_LENGTH 99
#include "MemoryManager.h"
#include <string.h>
#include <ctype.h>

struct _item {
	unsigned char flagAndsize;//���bitΪ1����ռ�ÿռ䣬0����ռ䱻�ͷš�����7bit�����ַ���ռ���ֽ���
	char *str;
};
typedef _item Item;

char *g_StringTable[SIZE / 2] = {};//���������ַ����ĵ�ַ������ʱ����NULL����ֹͣ����
extern char g_szBuff[SIZE] = {};
char *pCurrent = g_szBuff;//ָ��ǰ����λ��
int currentIndex = 0;//��ǰ����λ�õ��±�


////ֻ���ڲ�ʹ�ã������Ⱪ¶�����Զ���Ϊstatic����ʹ�ռ��δʹ��Ҳ�޷�����g_szBuff����Ҫ�������ط��ж�
//static char *DoSearchUnusedSpace(const char *pSearchStart) {
//	int beg = pSearchStart - g_szBuff;
//	int end = SIZE - beg;//end�ѳ����±�
//
//	for (int i = beg; i != end - 1; ++i) {//����������2��0�����Լ�1����ֹԽ��
//		if (g_szBuff[i] == 0 &&
//			g_szBuff[i + 1] == 0) {
//			return g_szBuff + i + 1;
//		}
//	}
//	return NULL;
//}
//
////����Ƿ�����Ч�ַ��������ȱ���С�ڵ���MAX_LENGTH
//static bool IsGoodString(const  char *str) {
//	if (strlen(str) <= MAX_LENGTH) {
//		return true;
//	}
//	return false;
//}
//
//
////��������˺������ҳ���һ��δʹ�õĿռ䣬�������ַ��δ�ҵ��򷵻�NULL��������ʾ����ʼ������ַ
//char *SearchUnusedSpace222222222222222222222(const char *pSearchStart) {
//	if (pSearchStart < g_szBuff) {
//		printf("����searchStart̫С\n");
//		return NULL;
//	}
//	else if (pSearchStart >= g_szBuff + SIZE) {
//		printf("����searchStart̫��\n");
//		return NULL;
//	}
//	else {
//		return DoSearchUnusedSpace(pSearchStart);
//	}
//}
//
//
////�ҳ���һ��δʹ�õĿռ䣬�������ַ��δ�ҵ��򷵻�NULL
//char *SearchUnusedSpace() {
//	if (g_szBuff[0] == 0 && g_szBuff[1] == 0) {
//		return g_szBuff;
//	}
//	return DoSearchUnusedSpace(g_szBuff);
//}


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

	//printf("û�б����Ϊ��ɾ�����ַ���\n");
	return -1;
}

//Ѱ�ҵ�һ����ʹ��λ�õ��±�,������������￪ʼ����,����ռ���δ����κ���Ч�ַ���������-1
int GetFirstValidIndex(int startIndex) {
	for (int i = startIndex; i < currentIndex; ++i) {
		if (IsValidString(g_szBuff + i)) {
			return i - 1;//flagAndsize���ַ���ǰ��1�ֽ�
		}
	}

	//printf("�ռ���δ����κ���Ч�ַ���\n");
	return -1;
}


//��Ƭ����
void Defragment() {
	int deprecatedIndex = GetFirstDeprecatedIndex(0);
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
		printf("����sourceָ����ַ�������Ч��\n");
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


//���g_StringTable����
void EmptyStringTable(){
	for (int i = 0; i < sizeof(g_StringTable)/ sizeof(g_StringTable[0]); ++i) {
		g_StringTable[i] = NULL;
	}
}


//��ȡ�����ַ����׵�ַ������g_StringTable��
void GetAllString() {
	EmptyStringTable();
	int validIndex = GetFirstValidIndex(0);
	int lastIndex = 0;
	int i = 0;

	while (validIndex >= 0) {		
		g_StringTable[i] = g_szBuff + validIndex + 1;
		++i;
		int size = GetStringSizeByIndex(validIndex);
		lastIndex = validIndex + size + 1;
		validIndex = GetFirstValidIndex(lastIndex);
	}
}

//��Ѱ�ַ���,֧��ģ������
void QueryStringByContent(const char *subString) {
	GetAllString();
	for (int i = 0; i < sizeof(g_StringTable) / sizeof(g_StringTable[0]); ++i) {
		if (g_StringTable[i]) {
			if (strstr(g_StringTable[i], subString)) {
				printf("�����ַ%p��%s\n", g_StringTable[i] - 1, g_StringTable[i]);
			}
		}
	}
}


//��ʾÿ����ĸ�ĳ��ִ����ͱ���
void ShowEachCharInformation() {
	int alphabet[26 + 6 + 26] = {};//ASCII��Z��a֮����6���ַ�����������ʱֱ�Ӻ�����6���±�
	int allCount = 0;
	GetAllString();
	for (int i = 0; i < sizeof(g_StringTable) / sizeof(g_StringTable[0]); ++i) {
		if (g_StringTable[i]) {
			const char *pString = g_StringTable[i];
			while (*pString != '\0') {
				if (isalpha(*pString)) {
					++alphabet[*pString - 'A'];
					++allCount;
				}
				++pString;
			}

		}
	}

	for (int i = 0; i < 26; ++i) {
		printf("%c����%5d�Σ�ռ��%.2f%%\n", 'A' + i, alphabet[i], 100 * (double)alphabet[i] / allCount);
	}
	for (int i = 0; i < 26; ++i) {
		printf("%c����%5d�Σ�ռ��%.2f%%\n", 'a' + i, alphabet[i + 26 + 6], 100 * (double)alphabet[i + 26 + 6] / allCount);
	}
}
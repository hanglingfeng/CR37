//�ռ�δ��ʹ������0��䣬�ַ�����βҲ��0��ע������

#define _CRT_SECURE_NO_WARNINGS
#include "MemoryManager.h"
#include <string.h>
#define MAX_LENGTH 99
struct _item {
	unsigned char flagAndsize;//���bitΪ1����ռ�ÿռ䣬0����ռ䱻�ͷš�����7bit�����ַ���ռ���ֽ���
	char *str;
};
typedef _item Item;

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



//�����ַ����Ƿ���Ч
bool IsValidString(char *source) {
	bool valid = true;
	if (source > g_szBuff && source < g_szBuff + currentIndex - 1) {	//��Χ��ȷ
		Item *item = (Item *)(source - 1);
		bool used = item->flagAndsize & 0x80;
		int size = item->flagAndsize & 0x7F;

		if (used && size && source[size - 1] == '\0') {			//״̬����С����β����ȷ
			for (int i = 0; i < size - 1; ++i) {				//�м䲿�ֲ�����0
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
		Item *item = (Item *)(source - 1);
		bool used = item->flagAndsize & 0x80;
		int size = item->flagAndsize & 0x7F;

		if (!used && size && source[size - 1] == '\0') {			//״̬����С����β����ȷ
			for (int i = 0; i < size - 1; ++i) {				//�м䲿�ֲ�����0
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
	Item *item = (Item *)(g_szBuff + deprecatedIndex);
	int size = item->flagAndsize & 0x7F;
	int validIndex = GetFirstValidIndex(deprecatedIndex + 1 + size + 1);

	while (deprecatedIndex >= 0 && validIndex > deprecatedIndex) {
		char *destination = g_szBuff + deprecatedIndex;
		const char *source = g_szBuff + validIndex;
		item = (Item *)source;
		size = item->flagAndsize & 0x7F;
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
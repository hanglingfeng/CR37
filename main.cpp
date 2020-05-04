//�ṩ���û��������а汾
//��ָ�������ڴ洢�ַ���
//1.����: ���ַ��������Ӳ������100�ֽ�
//2.ɾ�������ַ�����ɾ������
//3.�޸ģ��޸�ָ�����ַ���������ռ䳤�Ȳ�����洢�������ط���ԭ�ַ�����Ϊ��ɾ��
//4.��ѯ�����ַ������ݲ����ַ����Ļ�����Ϣ��֧��ģ������
//5.ͳ�ƣ�ͳ��ÿ����ĸ�ĳ��ִ����ͱ�����
//6.��ʾ�洢��Ϣ����˳����ʾ�ѷ���(U)��δ����(F)��Դ,����10�ֽڿռ䣬��ʾUFUUFFFUUU
//7.��Ƭ������ɾ���г����˲������ġ���϶���������ʹ����Щ����϶�������ҿ���


#define _CRT_SECURE_NO_WARNINGS
#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>

void PrintMenu();
void ReadLine(char *pszBbuff, int nBuffSize);
void UserAddString();
void UserRemoveString();
void UserModifySring();
void UserQuaryString();
int ReadStringWithEscapeChar(char pBuff[]);

int main() {
	int ch = 0;	
	while (true) {		
		PrintMenu();
		printf("��������ţ�");
		ch = getchar();
		EatLine();

		switch (ch) {
		case '1':
			UserAddString();					
			break;
		case '2':			
			UserRemoveString();
			break;
		case '3':			
			UserModifySring();
			break;
		case '4': 
			UserQuaryString();
			break;
		case '5':
			ShowEachCharInformation();
			break;
		case '6':
			ShowInformation();
			break;
		case '7':
			Defragment();
			puts("�������");
			break;
		default:
			printf("�޴�ѡ����������룺");
			break;
		}
	}

	return 0;
}

//��ӡ�˵�
//������
//	��
//����ֵ��
//	��
void PrintMenu() {
	puts("");
	puts("1.�����ַ���    2.ɾ���ַ���    3.�޸��ַ���");
	puts("4.��ѯ�ַ���    5.ͳ����ĸ      6.��ʾ�洢��Ϣ");
	puts("7.��Ƭ����");
}

//��һ�����뵽pBuff����ֻ��ȡ��һ�����з����������򲻱������з�
//������
//	const char *pBuff [in]�����ַ��Ļ�����
//	int nBuffSize [in]�����ַ��Ļ�������С
//����ֵ��
//	��
void ReadLine(char *pBuff, int nBuffSize) {
	fgets(pBuff, nBuffSize, stdin);
	if (pBuff[1] == '\0') {//�û�ֻ������Enter����������
		return;
	}
	else {
		for (int i = 0; i < nBuffSize; ++i) {
			if (pBuff[i] == '\n') {//�������ȡ���Ļ��з�
				pBuff[i] = 0;
				break;
			}
		}
	}
}

//��ȡ�������룬���浽pBuff�Enter�������Ļ��з��������������м���ڶ��\0����������ַ�����LENGTH������0��
//������
//	char *pBuff [in]�����ַ��Ļ�����
//����ֵ��
//	int nBuffSize [in]�����ַ��Ļ�������С
int ReadStringWithEscapeChar(char pBuff[]) {
	int ch = 0;
	int i = 0;
	bool bTooLong = false;
	while ((ch = getchar()) != '\n') {
		if (ch != '\\') {
			pBuff[i] = (char)ch;
		}
		else {
			switch (ch = getchar()) {
			case 'a':
				pBuff[i] = '\a';
				break;
			case 'b':
				pBuff[i] = '\b';
				break;
			case 'f':
				pBuff[i] = '\f';
				break;
			case 'n':
				pBuff[i] = '\n';
				break;
			case 'r':
				pBuff[i] = '\r';
				break;
			case 't':
				pBuff[i] = '\t';
				break;
			case 'v':
				pBuff[i] = '\v';
				break;
			case '0':
				pBuff[i] = '\0';
				break;
			default:
				pBuff[i] = '\\';
				ungetc(ch, stdin);
				break;
			}
		}
		++i;//i�����λ����δ����ֵ
		if (i > LENGTH) {//�û�����̫����Ӧ�÷���
			bTooLong = true;
			break;
		}
	}
	if (bTooLong) {
		return 0;
	}
	else {
		pBuff[i] = '\0';
		return i + 1;
	}

}

//�����û������ַ���
//������
//	��
//����ֵ��
//	��
void UserAddString() {
	puts("1.��ͨ�ַ���    2.��ת���ַ����ַ�����֧��\\a \\b \\f \\n \\r \\t \\v \\0��   \? \"�ȿ���ֱ�����룬����Ϊת���ַ�");
	printf("��������ţ�");
	char aryBuff[LENGTH + 1] = { '\0' };
	int ch = getchar();
	size_t nIndex = 0;
	EatLine();
	
	switch (ch) {
	case '1':
		printf("�������ַ�����");		
		ReadLine(aryBuff, sizeof(aryBuff) / sizeof(aryBuff[0]));
		nIndex = AddString(aryBuff, (unsigned char)strlen(aryBuff) + 1);
		if (nIndex) {
			printf("�ַ��������ڣ�%d\n", nIndex);
		}
		else {
			printf("�ռ䲻�㣬�޷�����");
		}
		break;
	case '2': {
		printf("�������ַ�����");
		int nSize = ReadStringWithEscapeChar(aryBuff);
		if (nSize) {
			nIndex = AddString(aryBuff, (unsigned char)nSize);
			if (nIndex) {
				printf("�ַ��������ڣ�%d\n", nIndex);
			}
			else {
				printf("�ռ䲻�㣬�޷�����");
			}
		}
		else {
			EatLine();
			printf("������ַ���̫����������");
		}
		break;
	}
	default:
		printf("��������޴�ѡ��");
		break;
	}
}

//�����û�ɾ���ַ���
//������
//	��
//����ֵ��
//	��
void UserRemoveString() {
	char *pAddress = NULL;
	printf("������Ҫɾ���Ĺ����ַ��");
	if (scanf("%x", (unsigned *)&pAddress) == 1) {
		if (RemoveItem((Item*)pAddress)) {
			puts("ɾ���ɹ�");
		}
		else {
			printf("���Ƴ����ַ�����ַ��Ч\n");
		}
	}
	else {
		puts("���벻��ȷ");
	}
	EatLine();
}

//�����û��޸��ַ���
//������
//	��
//����ֵ��
//	��
void UserModifySring() {
	

	char *pAddress = NULL;
	char aryBuff[LENGTH + 1] = {'\0'};
	printf("������Ҫ�޸ĵ��ַ�����ַ��");
	if (scanf("%x", (unsigned *)&pAddress) == 1) {
		EatLine();
		if (SearchItem((Item *)pAddress)) {
			puts("1.�޸�Ϊ��ͨ�ַ���    2.�޸�Ϊ��ת���ַ����ַ�����֧��\\a \\b \\f \\n \\r \\t \\v \\0��   \? \"�ȿ���ֱ�����룬����Ϊת���ַ�");
			printf("��������ţ�");
			int ch = getchar();
			EatLine();

			switch (ch) {
			case '1':
				printf("���������ַ�����");
				ReadLine(aryBuff, sizeof(aryBuff) / sizeof(aryBuff[0]));
				if (ModifyItem((Item *)pAddress, aryBuff, (unsigned char)strlen(aryBuff) + 1)) {
					puts("�޸ĳɹ�");
				}
				break;
			case '2': {
				printf("���������ַ�����");
				int nSize = ReadStringWithEscapeChar(aryBuff);
				if (nSize) {
					if (ModifyItem((Item *)pAddress, aryBuff, (unsigned char)nSize)) {
						puts("�޸ĳɹ�");
					}					
				} 
				else {
					EatLine();
					printf("������ַ���̫�������޸�");
				}
				break;
			}
			default:
				printf("��������޴�ѡ��");
				break;
			}
		}
		else {
			puts("��ַ��Ч");
		}		
	}
	else {
		EatLine();
		puts("���벻��ȷ");
	}
}

//�����û���ѯ�ַ���
//������
//	��
//����ֵ��
//	��
void UserQuaryString() {
	puts("1.��ѯ��ͨ�ַ���    2.��ѯ��ת���ַ����ַ�����֧��\\a \\b \\f \\n \\r \\t \\v \\0��   \? \"�ȿ���ֱ�����룬����Ϊת���ַ�");
	printf("��������ţ�");
	char aryBuff[LENGTH + 1] = { '\0' };
	int ch = getchar();
	EatLine();

	switch (ch) {
	case '1':
		printf("�������ѯ���ݣ�֧��ģ��ƥ�䣺");
		ReadLine(aryBuff, sizeof(aryBuff) / sizeof(aryBuff[0]));
		QueryStringByContent(aryBuff);
		break;
	case '2': {
		printf("�������ѯ���ݣ�֧��ģ��ƥ�䣺");
		int nSize = ReadStringWithEscapeChar(aryBuff);
		if (nSize) {
			//delete
		}
		else {
			EatLine();
			printf("������ַ���̫��������ѯ");
		}
		break; 
	}
	default:
		printf("��������޴�ѡ��");
		break;
	}
}
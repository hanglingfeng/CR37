#define _CRT_SECURE_NO_WARNINGS
#include "MemoryManager.h"
#include <stdio.h>


void PrintMenu();
void ReadLine(char *buff, int buffSize);
void UserRemoveString();
void UserModifySring();

int main() {	
	char buff[MAX_LENGTH+1] = {};
	int ch = 0;
	
	while (true) {		
		PrintMenu();
		printf("��������ţ�");
		ch = getchar();
		EatLine();

		switch (ch) {
		case '1':
			printf("�������ַ�����");
			ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
			printf("�ַ��������ڣ�%p\n", AddString(buff));			
			break;
		case '2':			
			UserRemoveString();
			break;
		case '3':			
			UserModifySring();
			break;
		case '4':
			printf("�������ѯ���ݣ�֧��ģ��ƥ�䣺");
			ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
			QueryStringByContent(buff);
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
void PrintMenu() {
	puts("");
	puts("1.�����ַ���    2.ɾ���ַ���    3.�޸��ַ���");
	puts("4.��ѯ�ַ���    5.ͳ����ĸ      6.��ʾ�洢��Ϣ");
	puts("7.��Ƭ����");
}

//��һ�����뵽buff����ֻ��ȡ��һ�����з����������򲻱������з�
void ReadLine(char *buff, int buffSize) {
	fgets(buff, buffSize, stdin);
	if (buff[1] == '\0') {//�û�ֻ������Enter����������
		return;
	}
	else {
		for (int i = 0; i < buffSize; ++i) {
			if (buff[i] == '\n') {//�������ȡ���Ļ��з�
				buff[i] = 0;
				break;
			}
		}
	}
}

//�����û�ɾ���ַ���
void UserRemoveString() {
	char *address = NULL;
	printf("������Ҫɾ�����ַ�����ַ��");
	if (scanf("%x", (unsigned *)&address) == 1) {
		if (RemoveString(address)) {
			puts("ɾ���ɹ�");
		}
	}
	else {
		puts("���벻��ȷ");
	}
	EatLine();
}

//�����û��޸��ַ���
void UserModifySring() {
	char *address = NULL;
	char buff[MAX_LENGTH + 1] = {};
	printf("������Ҫ�޸ĵ��ַ�����ַ��");
	if (scanf("%x", (unsigned *)&address) == 1) {
		EatLine();
		if (IsValidString(address)) {
			printf("���������ַ�����");
			ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
			if (ModifyString(address, buff)) {
				puts("�޸ĳɹ�");
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
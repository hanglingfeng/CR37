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
void ReadLine(char *buff, int buffSize);
void UserAddString();
void UserRemoveString();
void UserModifySring();
void UserQuaryString();
int ReadStringWithEscapeChar(char buff[]);

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

//��һ�����뵽buff����ֻ��ȡ��һ�����з����������򲻱������з�
//������
//	const char *buff [in]�����ַ��Ļ�����
//	int buffSize [in]�����ַ��Ļ�������С
//����ֵ��
//	��
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

//��ȡ�������룬���浽buff�Enter�������Ļ��з��������������м���ڶ��\0����������ַ�����LENGTH������0��
//������
//	char *buff [in]�����ַ��Ļ�����
//����ֵ��
//	int buffSize [in]�����ַ��Ļ�������С
int ReadStringWithEscapeChar(char buff[]) {
	int ch = 0;
	int i = 0;
	bool tooLong = false;
	while ((ch = getchar()) != '\n') {
		if (ch != '\\') {
			buff[i] = (char)ch;
		}
		else {
			switch (ch = getchar()) {
			case 'a':
				buff[i] = '\a';
				break;
			case 'b':
				buff[i] = '\b';
				break;
			case 'f':
				buff[i] = '\f';
				break;
			case 'n':
				buff[i] = '\n';
				break;
			case 'r':
				buff[i] = '\r';
				break;
			case 't':
				buff[i] = '\t';
				break;
			case 'v':
				buff[i] = '\v';
				break;
			case '0':
				buff[i] = '\0';
				break;
			default:
				buff[i] = '\\';
				ungetc(ch, stdin);
				break;
			}
		}
		++i;//i�����λ����δ����ֵ
		if (i > LENGTH) {//�û�����̫����Ӧ�÷���
			tooLong = true;
			break;
		}
	}
	if (tooLong) {
		return 0;
	}
	else {
		buff[i] = '\0';
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
	char buff[LENGTH + 1] = { '\0' };
	int ch = getchar();
	const char *saveAddress = NULL;
	EatLine();
	
	switch (ch) {
	case '1':
		printf("�������ַ�����");		
		ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
		saveAddress = AddString(buff, (unsigned char)strlen(buff) + 1, false);
		if (saveAddress) {
			printf("�ַ��������ڣ�%p\n", saveAddress);
		}
		else {
			printf("�ռ䲻�㣬�޷�����");
		}
		break;
	case '2': {
		printf("�������ַ�����");
		int size = ReadStringWithEscapeChar(buff);
		if (size) {
			saveAddress = AddString(buff, (unsigned char)size, true);
			if (saveAddress) {
				printf("�ַ��������ڣ�%p\n", saveAddress);
			}
			else {
				printf("�ռ䲻�㣬�޷�����");
			}
		}
		else {
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
	char *address = NULL;
	printf("������Ҫɾ���Ĺ����ַ��");
	if (scanf("%x", (unsigned *)&address) == 1) {
		if (RemoveItem((Item*)address)) {
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
	

	char *address = NULL;
	char buff[LENGTH + 1] = {'\0'};
	printf("������Ҫ�޸ĵ��ַ�����ַ��");
	if (scanf("%x", (unsigned *)&address) == 1) {
		EatLine();
		if (SearchItem((Item *)address)) {
			puts("1.�޸�Ϊ��ͨ�ַ���    2.�޸�Ϊ��ת���ַ����ַ�����֧��\\a \\b \\f \\n \\r \\t \\v \\0��   \? \"�ȿ���ֱ�����룬����Ϊת���ַ�");
			printf("��������ţ�");
			int ch = getchar();
			EatLine();

			switch (ch) {
			case '1':
				printf("���������ַ�����");
				ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
				if (ModifyItem((Item *)address, buff, (unsigned char)strlen(buff) + 1, false)) {
					puts("�޸ĳɹ�");
				}
				break;
			case '2': {
				printf("���������ַ�����");
				int size = ReadStringWithEscapeChar(buff);
				if (size) {
					if (ModifyItem((Item *)address, buff, (unsigned char)size, true)) {
						puts("�޸ĳɹ�");
					}					
				} 
				else {
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
	char buff[LENGTH + 1] = { '\0' };
	int ch = getchar();
	EatLine();

	switch (ch) {
	case '1':
		printf("�������ѯ���ݣ�֧��ģ��ƥ�䣺");
		ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
		QueryStringByContent(buff);
		break;
	case '2': {
		printf("�������ѯ���ݣ�֧��ģ��ƥ�䣺");
		int size = ReadStringWithEscapeChar(buff);
		if (size) {
			QueryStringByContentWithEscapeChar(buff, size);
		}
		else {
			printf("������ַ���̫��������ѯ");
		}
		break; 
	}
	default:
		printf("��������޴�ѡ��");
		break;
	}
}
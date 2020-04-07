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
		printf("请输入序号：");
		ch = getchar();
		EatLine();

		switch (ch) {
		case '1':
			printf("请输入字符串：");
			ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
			printf("字符串保存在：%p\n", AddString(buff));			
			break;
		case '2':			
			UserRemoveString();
			break;
		case '3':			
			UserModifySring();
			break;
		case '4':
			printf("请输入查询内容，支持模糊匹配：");
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
			puts("整理完毕");
			break;
		default:
			printf("无此选项，请重新输入：");
			break;
		}
	}

	return 0;
}

//打印菜单
void PrintMenu() {
	puts("");
	puts("1.增加字符串    2.删除字符串    3.修改字符串");
	puts("4.查询字符串    5.统计字母      6.显示存储信息");
	puts("7.碎片整理");
}

//读一行输入到buff里，如果只读取到一个换行符则保留，否则不保留换行符
void ReadLine(char *buff, int buffSize) {
	fgets(buff, buffSize, stdin);
	if (buff[1] == '\0') {//用户只输入了Enter，不做处理
		return;
	}
	else {
		for (int i = 0; i < buffSize; ++i) {
			if (buff[i] == '\n') {//不保存读取到的换行符
				buff[i] = 0;
				break;
			}
		}
	}
}

//处理用户删除字符串
void UserRemoveString() {
	char *address = NULL;
	printf("请输入要删除的字符串地址：");
	if (scanf("%x", (unsigned *)&address) == 1) {
		if (RemoveString(address)) {
			puts("删除成功");
		}
	}
	else {
		puts("输入不正确");
	}
	EatLine();
}

//处理用户修改字符串
void UserModifySring() {
	char *address = NULL;
	char buff[MAX_LENGTH + 1] = {};
	printf("请输入要修改的字符串地址：");
	if (scanf("%x", (unsigned *)&address) == 1) {
		EatLine();
		if (IsValidString(address)) {
			printf("请输入新字符串：");
			ReadLine(buff, sizeof(buff) / sizeof(buff[0]));
			if (ModifyString(address, buff)) {
				puts("修改成功");
			}
		}
		else {
			puts("地址无效");
		}

	}
	else {
		EatLine();
		puts("输入不正确");
	}
}
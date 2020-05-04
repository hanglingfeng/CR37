//提供给用户的命令行版本
//在指定数组内存储字符串
//1.增加: 对字符串的增加操作，最长100字节
//2.删除：对字符串的删除操作
//3.修改：修改指定的字符串，如果空间长度不够则存储在其他地方，原字符串视为被删除
//4.查询：按字符串内容查找字符串的基本信息，支持模糊查找
//5.统计：统计每个字母的出现次数和比例。
//6.显示存储信息：按顺序显示已分配(U)、未分配(F)资源,例如10字节空间，显示UFUUFFFUUU
//7.碎片整理：增删改中出现了不连续的“孔隙”，整理后使得这些“孔隙”连续且可用


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
		printf("请输入序号：");
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
//参数：
//	无
//返回值：
//	无
void PrintMenu() {
	puts("");
	puts("1.增加字符串    2.删除字符串    3.修改字符串");
	puts("4.查询字符串    5.统计字母      6.显示存储信息");
	puts("7.碎片整理");
}

//读一行输入到pBuff里，如果只读取到一个换行符则保留，否则不保留换行符
//参数：
//	const char *pBuff [in]保存字符的缓冲区
//	int nBuffSize [in]保存字符的缓冲区大小
//返回值：
//	无
void ReadLine(char *pBuff, int nBuffSize) {
	fgets(pBuff, nBuffSize, stdin);
	if (pBuff[1] == '\0') {//用户只输入了Enter，不做处理
		return;
	}
	else {
		for (int i = 0; i < nBuffSize; ++i) {
			if (pBuff[i] == '\n') {//不保存读取到的换行符
				pBuff[i] = 0;
				break;
			}
		}
	}
}

//读取键盘输入，保存到pBuff里，Enter键产生的换行符被丢弃，允许中间存在多个\0。如果输入字符大于LENGTH，返回0。
//参数：
//	char *pBuff [in]保存字符的缓冲区
//返回值：
//	int nBuffSize [in]保存字符的缓冲区大小
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
		++i;//i代表的位置尚未被赋值
		if (i > LENGTH) {//用户输入太长，应该废弃
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

//处理用户新增字符串
//参数：
//	无
//返回值：
//	无
void UserAddString() {
	puts("1.普通字符串    2.带转义字符的字符串，支持\\a \\b \\f \\n \\r \\t \\v \\0。   \? \"等可以直接输入，不视为转义字符");
	printf("请输入序号：");
	char aryBuff[LENGTH + 1] = { '\0' };
	int ch = getchar();
	size_t nIndex = 0;
	EatLine();
	
	switch (ch) {
	case '1':
		printf("请输入字符串：");		
		ReadLine(aryBuff, sizeof(aryBuff) / sizeof(aryBuff[0]));
		nIndex = AddString(aryBuff, (unsigned char)strlen(aryBuff) + 1);
		if (nIndex) {
			printf("字符串保存在：%d\n", nIndex);
		}
		else {
			printf("空间不足，无法保存");
		}
		break;
	case '2': {
		printf("请输入字符串：");
		int nSize = ReadStringWithEscapeChar(aryBuff);
		if (nSize) {
			nIndex = AddString(aryBuff, (unsigned char)nSize);
			if (nIndex) {
				printf("字符串保存在：%d\n", nIndex);
			}
			else {
				printf("空间不足，无法保存");
			}
		}
		else {
			EatLine();
			printf("输入的字符串太长，不保存");
		}
		break;
	}
	default:
		printf("输入错误，无此选项");
		break;
	}
}

//处理用户删除字符串
//参数：
//	无
//返回值：
//	无
void UserRemoveString() {
	char *pAddress = NULL;
	printf("请输入要删除的管理地址：");
	if (scanf("%x", (unsigned *)&pAddress) == 1) {
		if (RemoveItem((Item*)pAddress)) {
			puts("删除成功");
		}
		else {
			printf("欲移除的字符串地址无效\n");
		}
	}
	else {
		puts("输入不正确");
	}
	EatLine();
}

//处理用户修改字符串
//参数：
//	无
//返回值：
//	无
void UserModifySring() {
	

	char *pAddress = NULL;
	char aryBuff[LENGTH + 1] = {'\0'};
	printf("请输入要修改的字符串地址：");
	if (scanf("%x", (unsigned *)&pAddress) == 1) {
		EatLine();
		if (SearchItem((Item *)pAddress)) {
			puts("1.修改为普通字符串    2.修改为带转义字符的字符串，支持\\a \\b \\f \\n \\r \\t \\v \\0。   \? \"等可以直接输入，不视为转义字符");
			printf("请输入序号：");
			int ch = getchar();
			EatLine();

			switch (ch) {
			case '1':
				printf("请输入新字符串：");
				ReadLine(aryBuff, sizeof(aryBuff) / sizeof(aryBuff[0]));
				if (ModifyItem((Item *)pAddress, aryBuff, (unsigned char)strlen(aryBuff) + 1)) {
					puts("修改成功");
				}
				break;
			case '2': {
				printf("请输入新字符串：");
				int nSize = ReadStringWithEscapeChar(aryBuff);
				if (nSize) {
					if (ModifyItem((Item *)pAddress, aryBuff, (unsigned char)nSize)) {
						puts("修改成功");
					}					
				} 
				else {
					EatLine();
					printf("输入的字符串太长，不修改");
				}
				break;
			}
			default:
				printf("输入错误，无此选项");
				break;
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

//处理用户查询字符串
//参数：
//	无
//返回值：
//	无
void UserQuaryString() {
	puts("1.查询普通字符串    2.查询带转义字符的字符串，支持\\a \\b \\f \\n \\r \\t \\v \\0。   \? \"等可以直接输入，不视为转义字符");
	printf("请输入序号：");
	char aryBuff[LENGTH + 1] = { '\0' };
	int ch = getchar();
	EatLine();

	switch (ch) {
	case '1':
		printf("请输入查询内容，支持模糊匹配：");
		ReadLine(aryBuff, sizeof(aryBuff) / sizeof(aryBuff[0]));
		QueryStringByContent(aryBuff);
		break;
	case '2': {
		printf("请输入查询内容，支持模糊匹配：");
		int nSize = ReadStringWithEscapeChar(aryBuff);
		if (nSize) {
			//delete
		}
		else {
			EatLine();
			printf("输入的字符串太长，不查询");
		}
		break; 
	}
	default:
		printf("输入错误，无此选项");
		break;
	}
}
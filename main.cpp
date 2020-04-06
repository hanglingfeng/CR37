#include "MemoryManager.h"

#include <iostream>
using namespace std;

int main() {
	//cout << GetFirstValidIndex(0) << endl;
	 
	char *pStrign1 = AddString("Hello");
	
	
	char *pStrign2 = AddString("World");
	char *pStrign3 = AddString("C/C++/C#");


	RemoveString(pStrign1);
	Defragment();
	RemoveString(pStrign1);
	Defragment();
	RemoveString(pStrign1);
	Defragment();
	AddString("goose");
	AddString("World");

	AddString("C/C++/C#");
	ModifyString(pStrign1, "HelloWorld!dddd");






	return 0;
}
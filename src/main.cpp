#include <iostream>
#include <stdio.h>
#include <vector>
#include "DirtyManager.h"

#include "acMach.h"

void DirtyTest()
{
	/*DiryManger<TrieTree>::GetInstance().Init();
	std::string str = "";
	if (DiryManger<TrieTree>::GetInstance().Check("shers"))
	{
		std::cout << "found" << std::endl;
	}
	else
	{
		std::cout << "not found" << std::endl;
	}*/

	DiryManger<AcMachine>::GetInstance().Init();
	std::string str = "";
	if (DiryManger<AcMachine>::GetInstance().Check("shers"))
	{
		std::cout << "found" << std::endl;
	}
	else
	{
		std::cout << "not found" << std::endl;
	}

}
void main()
{
	DirtyTest();
	getchar();
}
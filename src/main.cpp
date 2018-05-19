#include <iostream>
#include <stdio.h>
#include <vector>
#include "DirtyManager.h"
#include <map>
#include "acMach.h"
#include <fstream>
#include "acsmx.h"
#include <string>
#include <type_traits>
#include "DirtyManager2.h"

unsigned char text[MAXLEN];
extern int nline;
using namespace std;


////////使用自己的ac状态机
void DirtyTest()
{
	//utf-8模式
	//1.优点:节省英文存储空间
	//2.缺点:中文可能有多个字符存储，找一个中文字要遍历多个节点，浪费更多的时间
#ifdef _UTF8
	DiryManger::GetInstance().Init();
	string check = "大a海u";
#ifdef _WIN32
	//windows需要把gbk转为uf8-8
	check = Unicode::utf8fromgbk(check);
	for (int i = 0; i < check.length(); ++i)
	{
		printf("%x\n", check[i]);
	}
#else
	std::cout << "before" << std::endl;
	for (int i = 0; i<check.size(); ++i)
	{
		printf("check %d:%x\n", i, check[i]);
	}
#endif	
	if (DiryManger::GetInstance().Check(check))
	{
		char *pout = new char[check.length()];
		DiryManger::GetInstance().Replace(check, '#', pout);
		string out = pout;
		delete[]pout;
#ifdef _WIN32
		//转为gbk
		out = Unicode::gbkfromutf8(out);
#endif
		printf("result:%s\n", out.c_str());
	}
	else
	{
		std::cout << "dirtyword not found" << std::endl;
	}

#else
	//utf-32模式
	DiryManger::GetInstance().Init();
	std::string str = "赛尔号irs";
	std::cout << "start is " << str << std::endl;
	str32 u32_str;
	Unicode::convert(str.c_str(), str.size(), u32_str);
	c32 replace = '#';
	int length = u32_str.length();
	c32 *pout = new c32[length];
	DiryManger::GetInstance().Replace(u32_str, replace, pout);
	std::cout << "after" << std::endl;
	for (int i = 0; i< length; ++i)
	{
		std::cout << pout[i] << std::endl;
	}
	std::string out;
	Unicode::convert(pout, out);
	std::cout << "result is " << out << std::endl;
#endif
}
////////使用snort的ac实现。缺点:使用数组存,浪费空间。只能存英文的模式串,中文怎么实现
int snortTest()
{
	int i, nocase = 0;
	FILE *fd;
	char filename[20];
	ACSM_STRUCT * acsm;
	acsm = acsmNew();
	//strcpy(filename, argv[1]);
	fd = fopen("actext.txt", "r");
	if (fd == NULL)
	{
		fprintf(stderr, "Open file error!\n");
		exit(1);
	}
	nocase = 1;

	ifstream myfile("actext.txt");
	if (!myfile)
	{
		printf("open file failed\n");
	}
	//读取模式串
	std::string str;
	while (getline(myfile, str))
	{
		acsmAddPattern(acsm, (unsigned char *)str.c_str(), strlen((char *)str.c_str()), nocase);
	}
	/* Generate GtoTo Table and Fail Table */
	acsmCompile(acsm);
	/*要搜寻的文本串*/
	std::string text = "ushers";
	text = "我";
	acsmSearch(acsm, (unsigned char *)text.c_str(), text.length(), PrintMatch);

	PrintSummary(acsm->acsmPatterns);

	//acsmFree(acsm);

	printf("\n### AC Match Finished ###\n");
	return (0);
}


void trietreeTest()
{
	//使用trietree树字典树
	DirtyManager2 manager;
	manager.init();
	std::string check = "ushers";
	if (manager.Check(check.c_str()) <= 0)
	{
		std::cout << "not found or error" << std::endl;
	}
	else
	{
		std::cout << "founded" << std::endl;
		c8 *pArray = new c8[6];
		manager.Replace(pArray);
		for (int i = 0; i<6; ++i)
		{
			std::cout << pArray[i] << std::endl;
		}
	}
}

void strTest()
{
	char *p = new char[10];
	p[0] = 'a';
	p[1] = 'b';
	p[2] = '\0';
	std::string out = p;
	std::cout << out << std::endl;
	printf("%s\n", out.c_str());
	std::cout << p << std::endl;
	printf("%s", p);
}


int main()
{
	//strTest();
	DirtyTest();
	//DirtyTest();
	//snortTest();
	getchar();
	return 0;
}

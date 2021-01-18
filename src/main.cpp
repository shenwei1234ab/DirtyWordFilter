#include <iostream>
#include <stdio.h>
#include <vector>
#include "DirtyManager.h"
#include <map>
#include "acMach.h"
#include <fstream>
#include "acsmx.h"
#include <string>
#include <locale>
//#include <type_traits>
#include <codecvt>
#include "DirtyManager2.h"
#ifdef _WIN32
#pragma execution_character_set("utf-8")	
#endif
unsigned char text[MAXLEN];
extern int nline;
using namespace std;


////////使用自己的ac状态机
void DirtyTest()
{
	//utf-32模式
	if (!DiryManger::GetInstance().Init())
	{
		std::cout << "init failed" << std::endl;
	}
	std::string str = "swsww海傻逼wwwss";
	str32 u32_str;
	Unicode::convert(str.c_str(), str.size(), u32_str);
	c32 replace = '#';
	int length = u32_str.length();
	c32 *pout = new c32[length];
	if (DiryManger::GetInstance().Check(u32_str))
	{
		
	}
	if (!DiryManger::GetInstance().Replace(u32_str, replace))
	{
		return;
	}
	
	std::cout << "after--------------------------" << std::endl;
	/*for (int i = 0; i< length; ++i)
	{
		std::cout << pout[i] << std::endl;
	}*/
	std::string out;
	//Unicode::convert(pout, out);
	Unicode::convert(u32_str, out);
	std::cout << out << std::endl;
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

//
//void trietreeTest()
//{
//	//使用trietree树字典树
//	DirtyManager2 manager;
//	manager.init();
//	std::string check = "ushers";
//	if (manager.Check(check.c_str()) <= 0)
//	{
//		std::cout << "not found or error" << std::endl;
//	}
//	else
//	{
//		std::cout << "founded" << std::endl;
//		c8 *pArray = new c8[6];
//		manager.Replace(pArray);
//		for (int i = 0; i<6; ++i)
//		{
//			std::cout << pArray[i] << std::endl;
//		}
//	}
//}

//http://www.baidu.com/link?url=5P7Y0xq9wTxhJM_W_5WPbf3y1pY_CySfncU-9G_OWX0F5Xf_SWu2DO7yfBWEG2n6jSSGA7ZUQ9r6n7YaKwPIKqrBq42V4JRjGJmtTjQqPlXYClcPE4BMA0VpV-U68Kqr
//
//std::u32string To_UTF32(const u8string &s)
//{
//	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
//
//	/*auto p = reinterpret_cast<const char *>(s.data());
//	auto str = conv.from_bytes(p, p + s.size());
//	std::u32string u32_str(str.begin(), str.end());
//	return u32_str;*/
//
//	return conv.from_bytes(s);
//}

void strTest()
{
	std::string str = "海aaaa";
	/*for (int i = 0; i < str.size(); ++i)
	{
		printf("%x: %d\n", str[i],sizeof(str[i]));
	}*/
	str32 u32_str;
	int length = Unicode::convert(str.c_str(), str.size(), u32_str);
	for (int i = 0; i <u32_str.size(); ++i)
	{
		printf("%x\n", u32_str[i]);
	}

	/*int size = u32_str.length();
	c32 *p = new c32[size];
	memcpy(p, u32_str.c_str(), size * sizeof(c32));
	for (int i = 0; i < size; ++i)
	{
		printf("%x\n", p[i]);
	}*/
	getchar();
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

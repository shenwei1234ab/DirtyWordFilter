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
#include <codecvt>
#include <functional>
#include "trieTree.h"
#ifdef _WIN32
#pragma execution_character_set("utf-8")	
#endif
unsigned char text[MAXLEN];
extern int nline;
using namespace std;


////////使用自己的ac状态机
uint64_t GetTickMs()
{
#ifdef _MSC_VER
	return GetTickCount64();

#else
	timeval curTime;
	gettimeofday(&curTime, NULL);
	return  (curTime.tv_sec) * 1000 + (curTime.tv_usec) / 1000;
#endif
}

uint64_t CostAvgTimeTest(int nTimes)
{
	//std::string str = "usherssssssssushers电话交友售手枪电话交友售手枪ssssss";
	//std::cout << "input string is:" << str << std::endl;
	//str32 u32_str;
	//Unicode::convert(str.c_str(), str.size(), u32_str);
	std::u32string context;
	//读取一片文章
	ifstream myfile("actext.txt");
	std::string str;
	if (!myfile.is_open())
	{
		printf("from file  failed \n");
		return false;
	}
	while (!myfile.eof())
	{
		getline(myfile, str);
		if (str == "")
		{
			continue;
		}
		//todo utf8-gbk
		std::u32string u32_str;
		Unicode::convert(str.c_str(), str.size(), u32_str);
		context += u32_str;
	}




	c32 replace = '#';
	int length = context.length();
	c32 *pout = new c32[length];
	std::list<str32> matchingList;


	//测试nTimes次计算平均时间
	uint64_t totalTime = 0;
	for (int times = 0; times <= nTimes; ++times)
	{
		std::u32string testContext = context;
		uint64_t startTime = GetTickCount();
		if (DiryManger::GetInstance().Check(testContext.c_str(), length, matchingList) && matchingList.size() > 0)
		{
			for (auto iter = matchingList.begin(); iter != matchingList.end(); ++iter)
			{
				std::string matchingStr;
				Unicode::convert(iter->c_str(), matchingStr);
				//std::cout << "matchStr:" << matchingStr << std::endl;
			}
		}
		DiryManger::GetInstance().Replace(const_cast<c32 *>(testContext.c_str()), length, replace);
		std::string out;
		Unicode::convert(testContext, out);
		//std::cout << "-------------------------after replace:" << out << std::endl;


		uint64_t endTime = GetTickCount();
		totalTime += endTime - startTime;
	}
	return totalTime / nTimes;
}


void DirtyTest()
{
	//字典树
	DirtyProcessor *pProcess = new TrieTree();
	//if (!DiryManger::GetInstance().Init(pProcess))
	//{
	//	std::cout << "init trietree failed" << std::endl;
	//}
	//uint64_t time1 = CostAvgTimeTest(800);
	//std::cout << "------------------------trietree costTime:" << time1 << std::endl;

	//ac状态机
	pProcess = new AcMachine();
	if (!DiryManger::GetInstance().Init(pProcess))
	{
		std::cout << "init acmachine failed" << std::endl;
	}
	uint64_t time2 = CostAvgTimeTest(1);
	std::cout << "------------------------acmachine costTime:" << time2 << std::endl;
}
////////使用snort的ac实现。缺点:使用数组存,浪费空间。只能存英文的模式串,中文怎么实现
int snortTest()
{
	int i, nocase = 0;
	FILE *fd;
	char filename[20];
	ACSM_STRUCT * acsm;
	acsm = acsmNew();
	ifstream myfile("dirtywords2.txt");
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



int main()
{
	DirtyTest();
	getchar();
	return 0;
}

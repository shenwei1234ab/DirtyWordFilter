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
	string check = "海h浸海呵海";
	std::cout<<"before"<<check<<std::endl;
	if(DiryManger::GetInstance().Check(check))
	{
		char *pout=new char[check.length()];
		DiryManger::GetInstance().Replace(check,'h#',pout);
		//std::cout<<"after pout"<<pout<<std::endl;
	}else
	{
		std::cout<< "dirtyword not found"<<std::endl;
	}
#else
	//test
	DiryManger::GetInstance().Init();
	std::string str = "赛尔号irs";
	std::cout << "start is "<<str << std::endl;
	str32 u32_str;
	Unicode::convert(str.c_str(),str.size(),u32_str);
	c32 replace = '#';
	int length  = u32_str.length();
	c32 *pout=new c32[length];
	DiryManger::GetInstance().Replace(u32_str,replace,pout);
	std::cout<<"after"<<std::endl;
	for(int i=0;i< length;++i)
	{
		std::cout<< pout[i]<<std::endl;
	}
	std::string out;
	Unicode::convert(pout,out);
	std::cout << "result is "<<out << std::endl;
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
	if(manager.Check(check.c_str()) <= 0 )
	{
		std::cout<<"not found or error"<<std::endl;
	}else
	{
		std::cout<<"founded"<<std::endl;
		c8 *pArray = new c8[6];
		manager.Replace(pArray);
		for(int i=0;i<6;++i)
		{
			std::cout<< pArray[i]<<std::endl;
		}
	}
}



void test()
{
	string str = "海";
	char *p=new char[str.size()+1];
	//std::cout<<"str size="<<str.size()<<std::endl;
	for(int i=0;i<str.size();++i)
	{
		p[i] = str[i];
		//std::cout<< i<<":"<<str[i]<<std::endl;
	}
	p[str.size()] = 'h';
	std::cout<<p<<std::endl;
	//str32 u32_str;
	//Unicode::convert(str.c_str(),str.size(),u32_str);

}
int main()
{
	DirtyTest();
	//DirtyTest();
	//snortTest();
	return 0;
}

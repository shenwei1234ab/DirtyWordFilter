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
unsigned char text[MAXLEN];
extern int nline;
using namespace std;
////////使用自己的ac状态机
void DirtyTest()
{
	//test
	DiryManger::GetInstance().Init();
	std::string str = "赛尔号irs";
	std::cout << "start is "<<str << std::endl;
	str32 u32_str;
	Unicode::convert(str.c_str(),str.size(),u32_str);
//	for(auto it = u32_str.begin();it != u32_str.end();++it)
//	{
//		std::cout<< *it<<std::endl;
//	}
//	if (DiryManger::GetInstance().Check(u32_str))
//	{
//		std::cout << "found" << std::endl;
//	}
//	else
//	{
//		std::cout << "not found" << std::endl;
//	}
	c32 replace = '#';
//	std::string replace = "#";
//	std::u32string replace_str;
//	Unicode::convert(replace.c_str(),replace.size(),replace_str);
	//void Repalce(const str32 &src,c32 replace,c32 *pout)
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


int main()
{
	//test();
	DirtyTest();
	//snortTest();
	return 0;
}

#include <iostream>
#include <stdio.h>
#include <vector>
#include "DirtyManager.h"
#include <map>
#include "acMach.h"
#include <fstream>
#include "snort\acsmx.h"
unsigned char text[MAXLEN];
extern int nline;
using namespace std;
////////ʹ���Լ���ac״̬��
void DirtyTest()
{
	//test
	DiryManger<AcMachine>::GetInstance().Init();
	std::string str = "";
	if (DiryManger<AcMachine>::GetInstance().Check("gheghers�����Ŵ򰡴�"))
	{
		std::cout << "found" << std::endl;
	}
	else
	{
		std::cout << "not found" << std::endl;
	}
}
////////ʹ��snort��acʵ�֡�ȱ��:ʹ�������,�˷ѿռ䡣ֻ�ܴ�Ӣ�ĵ�ģʽ��,������ôʵ��
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
	//��ȡģʽ��
	std::string str;
	while (getline(myfile, str))  
	{
		acsmAddPattern(acsm, (unsigned char *)str.c_str(), strlen((char *)str.c_str()) , nocase);
	}
	/* Generate GtoTo Table and Fail Table */
	acsmCompile(acsm);
	/*Ҫ��Ѱ���ı���*/
	std::string text = "ushers";
	text = "��";
	acsmSearch(acsm, (unsigned char *)text.c_str(), text.length(), PrintMatch);

	PrintSummary(acsm->acsmPatterns);

	//acsmFree(acsm);

	printf("\n### AC Match Finished ###\n");
	return (0);
}

void main()
{
	DirtyTest();
	//snortTest();
	getchar();
}
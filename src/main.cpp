#include <iostream>
#include <stdio.h>
#include <vector>
#include "DirtyManager.h"
#include <map>
#include "acMach.h"
#include "snort\acsmx.h"
unsigned char text[MAXLEN];
extern int nline;
using namespace std;
////////使用自己的ac状态机
void DirtyTest()
{
	//test
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
////////使用snort的源码
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
	std::list<std::string> patterList;
	patterList.push_back("hers");
	patterList.push_back("his");
	patterList.push_back("she");
	patterList.push_back("he");
	for (auto it = patterList.begin(); it != patterList.end(); ++it){
		acsmAddPattern(acsm, (unsigned char *)it->c_str(), it->length(), nocase);
	}
	/* Generate GtoTo Table and Fail Table */
	acsmCompile(acsm);

	/*Search Pattern*/
	while (fgets((char *)text, MAXLEN, fd))
	{
		acsmSearch(acsm, text, strlen((char *)text), PrintMatch);
		nline++;
	}

	PrintSummary(acsm->acsmPatterns);

	//acsmFree(acsm);

	printf("\n### AC Match Finished ###\n");
	return (0);
}

void main()
{
	//DirtyTest();
	snortTest();
	getchar();
}
#ifndef _AC_MACH_H
#define _AC_MACH_H
#include <string>
#include <list>
#include "dirtybase.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <fstream>
using namespace std;
//http://blog.csdn.net/joylnwang/article/details/6793192
// http://www.hankcs.com/program/algorithm/implementation-and-analysis-of-aho-corasick-algorithm-in-java.html
//https://tech.meituan.com/ac.html

#define ACSM_FAIL_STATE -1
struct acsm_pattern
{
	string patrn;			//大写
	string casepatrn;		//xiao写
	int      n;
	int      nocase;
	//void   * id;
	//int      nmatch;
};

struct acsm_statetable
{
	/* Next state - based on input character */
	//int      NextState[ALPHABET_SIZE];
	unordered_map<char, int>  NextState;

	/* Failure state - used while building NFA & DFA  */
	int      FailState ;

	/* List of patterns that end here, if any */
	//acsm_pattern *MatchList;
	std::list<acsm_pattern> MatchList;
};


class AcMachine :public DirtyProcessor
{
public:
	virtual void Init()
	{
		//test
		FILE *fd = fopen("actext.txt", "r");
		if (fd == NULL)
		{
			fprintf(stderr, "Open file error!\n");
			exit(1);
		}

		ifstream myfile("actext.txt");
		if (!myfile)
		{
			printf("open file failed\n");
		}
		//读取模式串
		std::string str;
		while (getline(myfile, str))
		{
			if (str == "")
			{
				continue;
			}
			Add(str.c_str(), strlen(str.c_str()));
		}
		//Add("he", 2);
		//Add("she", 3);
		//Add("his", 3);
		//Add("hers", 4);
		Compile();
	}

	virtual void Add(const char *pArray, size_t size)
	{
		acsm_pattern newPattern = acsm_pattern();
		newPattern.patrn = pArray;
		newPattern.casepatrn = pArray;
		newPattern.n = size;
		newPattern.nocase = 1;
		//newPattern.nmatch = 0;
		acsmPatterns.push_back(newPattern);
	}


	//创建goto表和out表
	void _AddPatternStates(acsm_pattern  p)
	{
		int state = 0, next, n;
		int index = 0;
		//找到新节点的位置
		for ( ;index < p.patrn.length(); ++index){
			char str = p.patrn[index];
			auto it = acsmStateTable[state].NextState.find(str);
			if (it == acsmStateTable[state].NextState.end()){
				break;
			}
			state = it->second;
		}
		for (; index < p.patrn.length(); ++index){
			char str = p.patrn[index];
			//插入新的节点
			acsmNumStates++;
			acsmStateTable[state].NextState[str] = acsmNumStates;
			acsmStateTable[acsmNumStates] = acsm_statetable();
			state = acsmNumStates;
		}
		//构建初步的out表
		acsmStateTable[state].MatchList.push_back(p);
	}



	//构建failed表:非确定有限自动机
	void _Build_NFA()
	{
		std::queue<int> stateQueue;
		acsmStateTable[0].FailState = -1;
		//设置度为1的节点的失效状态
		auto hashmap =  acsmStateTable[0].NextState;
		for (auto it = hashmap.begin(); it != hashmap.end(); ++it){
			acsmStateTable[it->second].FailState = 0;
			stateQueue.push(it->second);
		}
		
		while (!stateQueue.empty()){
			int state = stateQueue.front();
			stateQueue.pop();
			//下一个状态
			auto nextStates = acsmStateTable[state].NextState;
			for (auto it = nextStates.begin(); it != nextStates.end(); ++it)
			{
				char c = it->first;
				////获得上一个状态的failed值
				int lastFailed = acsmStateTable[state].FailState;
				//计算下一个状态的failed值
				int nextState = it->second;
				stateQueue.push(nextState);

				auto iter = acsmStateTable[lastFailed].NextState.find(c);
				if (iter != acsmStateTable[lastFailed].NextState.end()){
					acsmStateTable[nextState].FailState = iter->second;
					//更新out表
					if (acsmStateTable[iter->second].MatchList.size() >= 1){
						acsmStateTable[nextState].MatchList.insert(acsmStateTable[nextState].MatchList.end(), acsmStateTable[iter->second].MatchList.begin(), acsmStateTable[iter->second].MatchList.end());
					}
				}
				else{
					//没有找到failed为0
					acsmStateTable[nextState].FailState = 0;
				}
			}
		}
	}




	virtual bool Check(const char *p, size_t size) 
	{
		bool ret = false;
		int state = 0;
		for (; size>=0 && *p; --size, ++p) {
			char c = *p;
			auto it = acsmStateTable[state].NextState.find(c);
			if (it != acsmStateTable[state].NextState.end()){
				state = it->second;
				if (acsmStateTable[state].MatchList.size() != 0){
					ret = true;
					//匹配到
					for (auto i = acsmStateTable[state].MatchList.begin(); i != acsmStateTable[state].MatchList.end(); ++i){
						std::cout << i->patrn.c_str() << std::endl;
					}
				}
			}
			else
			{
				//退回到failed state去找
				int failed_state = acsmStateTable[state].FailState;
				if (failed_state == -1)
				{

				}
				else
				{
					state = failed_state;
					p--;
				}
				//;
			}
		}
		return ret;
	}


	//	/* Generate GtoTo Table and Fail Table */
	void Compile()
	{
		acsmMaxStates = 1;
		for (auto iter = acsmPatterns.begin(); iter != acsmPatterns.end(); ++iter){
			acsmMaxStates += iter->n;
		}
		acsmStateTable[0] = acsm_statetable();
		/*for (int i = 0; i < acsmMaxStates; ++i)
		{
			ACSM_STATETABLE state;
			acsmStateTable.push_back(state);
		}*/
		//step1 创建goto表和初步的out表
		for (auto iter = acsmPatterns.begin(); iter != acsmPatterns.end(); ++iter)
		{
			_AddPatternStates(*iter);
		}
		//step2 创建failed表
		_Build_NFA();
	}

private:
	int acsmMaxStates;
	int acsmNumStates;		//节点总的个数
	list<acsm_pattern> acsmPatterns;
	unordered_map<int, acsm_statetable> acsmStateTable;		//hash key:state  value
};






#endif
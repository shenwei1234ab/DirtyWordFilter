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
#include "Unicode.h"
using namespace std;
//http://blog.csdn.net/joylnwang/article/details/6793192
// http://www.hankcs.com/program/algorithm/implementation-and-analysis-of-aho-corasick-algorithm-in-java.html
//https://tech.meituan.com/ac.html

#define ACSM_FAIL_STATE -1

struct acsm_pattern
{
	str32 patrn;			//��д
	str32 casepatrn;		//xiaoд
	int      n;
	int      nocase;
	//void   * id;
	//int      nmatch;
};

struct acsm_statetable
{
	/* Next state - based on input character */
	//int      NextState[ALPHABET_SIZE];
	unordered_map<c32, int>  NextState;

	/* Failure state - used while building NFA & DFA  */
	int      FailState ;

	/* List of patterns that end here, if any */
	//acsm_pattern *MatchList;
	std::list<acsm_pattern> MatchList;
};



class AcMachine
{
private:
	
public:
	 void Init()
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
		//��ȡģʽ��
		std::string str;
		while (getline(myfile, str))
		{
			if (str == "")
			{
				continue;
			}
			std::u32string u32_str ;
			Unicode::convert(str.c_str(),str.size(),u32_str);
			Add(u32_str.c_str(), u32_str.size());
		}
		//Add("he", 2);
		//Add("she", 3);
		//Add("his", 3);
		//Add("hers", 4);
		Compile();
	}

	 void Add(const c32 *pArray, size_t size)
	{
		acsm_pattern newPattern = acsm_pattern();
		newPattern.patrn = pArray;
		newPattern.casepatrn = pArray;
		newPattern.n = size;
		newPattern.nocase = 1;
		//newPattern.nmatch = 0;
		acsmPatterns.push_back(newPattern);
	}





	 bool Check(const c32 *p, size_t size)
	{
		bool ret = false;
		int state = 0;
		for (; size>=0 && *p; --size, ++p) {
			c32 c = *p;
			auto it = acsmStateTable[state].NextState.find(c);
			if (it != acsmStateTable[state].NextState.end()){
				state = it->second;
				if (acsmStateTable[state].MatchList.size() != 0){
					ret = true;
					//ƥ�䵽
					for (auto i = acsmStateTable[state].MatchList.begin(); i != acsmStateTable[state].MatchList.end(); ++i){
						std::string out;
						Unicode::convert(i->patrn.c_str(),out);
						std::cout << out << std::endl;
					}
				}
			}
			else
			{
				//�˻ص�failed stateȥ��
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
		//step1 ����goto��ͳ�����out��
		for (auto iter = acsmPatterns.begin(); iter != acsmPatterns.end(); ++iter)
		{
			_AddPatternStates(*iter);
		}
		//step2 ����failed��
		_Build_NFA();
	}


	void Replace(c32 *p, size_t size,c32 pRelpace)
	{
		bool ret = false;
		int state = 0;
		for (; size>=0 && *p; --size, ++p) {
			c32 c = *p;
			auto it = acsmStateTable[state].NextState.find(c);
			if (it != acsmStateTable[state].NextState.end()){
				state = it->second;
				m_findingChars.push_back(p);
				if (acsmStateTable[state].MatchList.size() != 0){
					ret = true;
					m_foundedChars.splice(m_foundedChars.end(), m_findingChars);
					m_findingChars.clear();
				}
			}
			else
			{
				int failed_state = acsmStateTable[state].FailState;
				if (failed_state == -1)
				{
					m_findingChars.clear();
				}
				else
				{
					state = failed_state;
					p--;
				}
			}
		}

		for(auto iter = m_foundedChars.begin();iter != m_foundedChars.end();++iter)
		{
			*(*iter) =pRelpace;
		}
	}


private:
	void _AddPatternStates(acsm_pattern  p)
	{
		int state = 0, next, n;
		int index = 0;
		//�ҵ��½ڵ��λ��
		for ( ;index < p.patrn.length(); ++index){
			c32 str = p.patrn[index];
			auto it = acsmStateTable[state].NextState.find(str);
			if (it == acsmStateTable[state].NextState.end()){
				break;
			}
			state = it->second;
		}
		for (; index < p.patrn.length(); ++index){
			c32 str = p.patrn[index];
			//�����µĽڵ�
			acsmNumStates++;
			acsmStateTable[state].NextState[str] = acsmNumStates;
			acsmStateTable[acsmNumStates] = acsm_statetable();
			state = acsmNumStates;
		}
		//����������out��
		acsmStateTable[state].MatchList.push_back(p);
	}



	//����failed��:��ȷ�������Զ���
	void _Build_NFA()
	{
		std::queue<int> stateQueue;
		acsmStateTable[0].FailState = -1;
		//���ö�Ϊ1�Ľڵ��ʧЧ״̬
		auto hashmap =  acsmStateTable[0].NextState;
		for (auto it = hashmap.begin(); it != hashmap.end(); ++it){
			acsmStateTable[it->second].FailState = 0;
			stateQueue.push(it->second);
		}

		while (!stateQueue.empty()){
			int state = stateQueue.front();
			stateQueue.pop();
			//��һ��״̬
			auto nextStates = acsmStateTable[state].NextState;
			for (auto it = nextStates.begin(); it != nextStates.end(); ++it)
			{
				char c = it->first;
				////�����һ��״̬��failedֵ
				int lastFailed = acsmStateTable[state].FailState;
				//������һ��״̬��failedֵ
				int nextState = it->second;
				stateQueue.push(nextState);

				auto iter = acsmStateTable[lastFailed].NextState.find(c);
				if (iter != acsmStateTable[lastFailed].NextState.end()){
					acsmStateTable[nextState].FailState = iter->second;
					//����out��
					if (acsmStateTable[iter->second].MatchList.size() >= 1){
						acsmStateTable[nextState].MatchList.insert(acsmStateTable[nextState].MatchList.end(), acsmStateTable[iter->second].MatchList.begin(), acsmStateTable[iter->second].MatchList.end());
					}
				}
				else{
					//û���ҵ�failedΪ0
					acsmStateTable[nextState].FailState = 0;
				}
			}
		}
	}

	std::list<c32*> m_findingChars;
	std::list<c32*> m_foundedChars;
	int acsmMaxStates;
	int acsmNumStates;		//�ڵ��ܵĸ���
	list<acsm_pattern> acsmPatterns;
	unordered_map<int, acsm_statetable> acsmStateTable;		//hash key:state  value
};






#endif

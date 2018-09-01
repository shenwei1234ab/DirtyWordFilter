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
#include <string.h>
#include <set>
using namespace std;
//http://blog.csdn.net/joylnwang/article/details/6793192
// http://www.hankcs.com/program/algorithm/implementation-and-analysis-of-aho-corasick-algorithm-in-java.html
//https://tech.meituan.com/ac.html

#define ACSM_FAIL_STATE -1
//#define _UTF8
struct acsm_pattern
{
	str32 patrn;			//大写模式串
	str32 casepatrn;		//原始模式串
	int      n;				//模式串个数
	int      nocase;		//是否大写
	//void   * id;
	//int      nmatch;
};

struct acsm_statetable
{
	/* Next state - based on input character */
	//int      NextState[ALPHABET_SIZE];
	unordered_map<c32, int>  NextState;//key:模式串字符 value:可以到达的下一个状态
	/* Failure state - used while building NFA & DFA  */
	int      FailState ;			//failed值

	/* List of patterns that end here, if any */
	//acsm_pattern *MatchList;
	//std::list<acsm_pattern> MatchList;
	//std::list<acsm_pattern> MatchList;
	std::list<acsm_pattern> MatchList;
};



class AcMachine
{
private:
	
public:
	 void Init(const std::string &fileName)
	{
		//ifstream myfile("actext.txt");
		ifstream myfile(fileName);
		if (!myfile)
		{
			std::cout << "open file " << fileName << std::endl; 
		}
		std::string str;
		while (getline(myfile, str))
		{
			if (str == "")
			{
				continue;
			}
			//todo utf8-gbk
			//std::cout << str<<":" << std::endl;
			/*for (int i = 0; i < str.length(); ++i)
			{
				printf("%d:%x\n",i, str[i]);
			}*/
			std::u32string u32_str ;
			Unicode::convert(str.c_str(),str.size(),u32_str);
			Add(u32_str.c_str(), u32_str.size());
		}
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
		for (; size>=0 && *p; --size, ++p) 
		{
			c32 c = *p;
			auto it = acsmStateTable[state].NextState.find(c);
			if (it != acsmStateTable[state].NextState.end())
			{
				state = it->second;
				if (acsmStateTable[state].MatchList.size() != 0)
				{
					ret = true;
					//找到匹配的节点
					for (auto i = acsmStateTable[state].MatchList.begin(); i != acsmStateTable[state].MatchList.end(); ++i)
					{
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
		/*acsmMaxStates = 1;
		for (auto iter = acsmPatterns.begin(); iter != acsmPatterns.end(); ++iter)
		{
			acsmMaxStates += iter->n;
		}*/
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
		acsmPatterns.clear();
		//step2 建failed表和调整out表
		_Build_NFA();
	}


	bool Replace(c32 *p, size_t size,c32 pRelpace)
	{
		//m_foundedChars.clear();
		//m_findingChars.clear();
		m_replaceChars.clear();
		//for (; size>=0 && *p; --size, ++p) 
		//{
		//	c32 c = *p;
		//	auto it = acsmStateTable[state].NextState.find(c);
		//	if (it != acsmStateTable[state].NextState.end())
		//	{
		//		state = it->second;
		//		m_findingChars.push_back(p);
		//		if (acsmStateTable[state].MatchList.size() != 0)
		//		{
		//			//ret = true;
		//			m_foundedChars.splice(m_foundedChars.end(), m_findingChars);
		//			m_findingChars.clear();
		//		}
		//	}
		//	else
		//	{
		//		int failed_state = acsmStateTable[state].FailState;
		//		//
		//		if (failed_state == -1)
		//		{
		//			m_findingChars.clear();
		//		}
		//		//failed_state == 0
		//		else if (failed_state == 0)
		//		{
		//			state = failed_state;

		//		}
		//		else 
		//		{
		//			//
		//			state = failed_state;
		//			p--;
		//		}
		//	}
		//}

		//for(auto iter = m_foundedChars.begin();iter != m_foundedChars.end();++iter)
		//{
		//	*(*iter) =pRelpace;
		//}
		bool ret = false;
		for (int state = 0; size > 0 && *p; --size, ++p)
		{
			c32 c = *p;
			auto it = acsmStateTable[state].NextState.find(c);
			if (it != acsmStateTable[state].NextState.end())
			{
				state = it->second;
				if (acsmStateTable[state].MatchList.size() != 0)
				{
					ret = true;
					//int size = acsmStateTable[state].MatchList;
					//m_foundedChars.splice(m_foundedChars.end(), m_findingChars);
					//begin存放最长的匹配
					int size =  acsmStateTable[state].MatchList.begin()->casepatrn.size();
					//从当前p往前size个字符全是敏感字
					for (c32 *index = p; size > 0; size--, index--)
					{
						m_replaceChars.insert(index);
					}
				}
			}
			else
			{
				int failed_state = acsmStateTable[state].FailState;
				if (failed_state == -1)
				{

				}
				else
				{
					state = failed_state;
					p--;
					size++;
				}
			}
		}

		for (auto iter = m_replaceChars.begin(); iter != m_replaceChars.end(); ++iter)
		{
			*(*iter) = pRelpace;
		}
		return ret;
	}

private:
	void _AddPatternStates(acsm_pattern  p)
	{
		int state = 0, next, n;
		int index = 0;
		//匹配已有的模式
		for ( ;index < p.patrn.length(); ++index)
		{
			c32 str = p.patrn[index];
			auto it = acsmStateTable[state].NextState.find(str);
			if (it == acsmStateTable[state].NextState.end())
			{
				break;
			}
			state = it->second;
		}
		//加入新的模式(is  rs)
		for (; index < p.patrn.length(); ++index)
		{
			c32 str = p.patrn[index];
			//加入新的模式
			acsmNumStates++;
			acsmStateTable[state].NextState[str] = acsmNumStates;
			acsmStateTable[acsmNumStates] = acsm_statetable();
			state = acsmNumStates;
		}
		//构建out表
		acsmStateTable[state].MatchList.push_back(p);
	}



	//建failed表和调整out表
	void _Build_NFA()
	{
		std::queue<int> stateQueue;			
		//State 0 的failed值为-1
		acsmStateTable[0].FailState = -1;
		//度 为1的failed值为0
		auto hashmap =  acsmStateTable[0].NextState;
		for (auto it = hashmap.begin(); it != hashmap.end(); ++it){
			acsmStateTable[it->second].FailState = 0;
			stateQueue.push(it->second);
		}
		//从跟节点开始广度遍历所有State
		while (!stateQueue.empty()){
			int state = stateQueue.front();
			stateQueue.pop();
			//��һ��״̬
			auto nextStates = acsmStateTable[state].NextState;
			for (auto it = nextStates.begin(); it != nextStates.end(); ++it)
			{
				char c = it->first;
				//前一个state的failed值
				int lastFailed = acsmStateTable[state].FailState;
				//下一个state的failed值
				int nextState = it->second;
				stateQueue.push(nextState);

				auto iter = acsmStateTable[lastFailed].NextState.find(c);
				if (iter != acsmStateTable[lastFailed].NextState.end()){
					//下一个节点的failed值=前一个节点的failed值通过c可以到达的state 
					acsmStateTable[nextState].FailState = iter->second;
					//调整out输出因为这个包含了failed状态的输出
					if (acsmStateTable[iter->second].MatchList.size() >= 1){
						acsmStateTable[nextState].MatchList.insert(acsmStateTable[nextState].MatchList.end(), acsmStateTable[iter->second].MatchList.begin(), acsmStateTable[iter->second].MatchList.end());
					}
				}
				else{
					//前一个节点的failed值通过c不可以到达的下一个节点failed值为0
					acsmStateTable[nextState].FailState = 0;
				}
			}
		}
	}

	std::set<c32*> m_replaceChars;
	int acsmMaxStates;		
	int acsmNumStates;		//状态
	list<acsm_pattern> acsmPatterns;		//模式串集合
	unordered_map<int, acsm_statetable> acsmStateTable;		//hash key:state  value:状态表
};






#endif

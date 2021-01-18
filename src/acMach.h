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


#define ACSM_FAIL_STATE -1

typedef int NODE_STATE ;

struct acsm_pattern
{
	str32 patrn;			//大写模式串
	str32 casepatrn;		//原始模式串
	int      n;				//模式串个数
	int      nocase;		//是否大写
};

struct acsm_statetable
{
	unordered_map<c32, NODE_STATE>  NextState;//key:模式串字符  value:可以到达的下一个状态
	NODE_STATE      FailState ;			//failed值(失效的时候要跳往的状态)
	std::list<acsm_pattern> MatchList;		//当前可以匹配输出的模式串
};



class AcMachine
{
private:
	
public:
	//************************************
	// 函数说明:从文件中读取初始化ac状态机
	// 返 回 值: void
	// 参 数: const std::string & fileName
	//************************************
	bool LoadFromFile(const std::string &fileName)
	{
		//ifstream myfile("actext.txt");
		ifstream myfile(fileName);
		std::string str;
		while (getline(myfile, str))
		{
			if (str == "")
			{
				continue;
			}
			//todo utf8-gbk
			for (int i = 0; i < str.length(); ++i)
			{
				printf("load %x ", i, str[i]);
			}
			printf("\n");
			printf("from file %s\n", fileName.c_str());
			std::u32string u32_str ;
			Unicode::convert(str.c_str(),str.size(),u32_str);
			AddPattern(u32_str.c_str(), u32_str.size());
		}
		/*std::list<std::string> strList;
		strList.push_back("he");
		strList.push_back("she");
		strList.push_back("his");
		strList.push_back("hers");
		for (auto iter = strList.begin(); iter != strList.end(); ++iter)
		{
			std::u32string u32_str;
			Unicode::convert(iter->c_str(), iter->size(), u32_str);
			Add(u32_str.c_str(), u32_str.size());
		}*/
		Compile();
		return true;
	}

	 //************************************
	 // 函数说明:添加模式串数组
	 // 返 回 值: void
	 // 参 数: const c32 * pArray
	 // 参 数: size_t size
	 //************************************
	 void AddPattern(const c32 *pArray, size_t size)
	{
		 if (!pArray)
		 {
			 return;
		 }
		acsm_pattern newPattern = acsm_pattern();
		newPattern.patrn = pArray;
		newPattern.casepatrn = pArray;
		newPattern.n = size;
		newPattern.nocase = 1;
		m_acsmPatterns.push_back(newPattern);
	}


	 bool Check(const c32 *p, size_t size)
	{
		bool ret = false;
		int state = 0;
		for (; size>=0 && *p; --size, ++p) 
		{
			c32 c = *p;
			auto it = m_acsmStateTable[state].NextState.find(c);
			if (it != m_acsmStateTable[state].NextState.end())
			{
				state = it->second;
				if (m_acsmStateTable[state].MatchList.size() != 0)
				{
					ret = true;
					//找到匹配的节点
					for (auto i = m_acsmStateTable[state].MatchList.begin(); i != m_acsmStateTable[state].MatchList.end(); ++i)
					{
						std::string out;
						Unicode::convert(i->patrn.c_str(),out);
						//std::cout << out << std::endl;
					}
				}
			}
			else
			{
				//�˻ص�failed stateȥ��
				int failed_state = m_acsmStateTable[state].FailState;
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


	//
	//************************************
	// 函数说明: 建立goto表和out表和failed表
	// 返 回 值: void
	//************************************
	void Compile()
	{
		//step1 创建goto表和初步的out表
		for (auto iter = m_acsmPatterns.begin(); iter != m_acsmPatterns.end(); ++iter)
		{
			_AddPatternStates(*iter);
		}
		m_acsmPatterns.clear();
		//step2 建failed表和调整out表
		_Build_NFA();
	}


	bool Replace(c32 *p, size_t size,c32 pRelpace)
	{
		m_replaceChars.clear();
		//for (; size>=0 && *p; --size, ++p) 
		//{
		//	c32 c = *p;
		//	auto it = m_acsmStateTable[state].NextState.find(c);
		//	if (it != m_acsmStateTable[state].NextState.end())
		//	{
		//		state = it->second;
		//		m_findingChars.push_back(p);
		//		if (m_acsmStateTable[state].MatchList.size() != 0)
		//		{
		//			//ret = true;
		//			m_foundedChars.splice(m_foundedChars.end(), m_findingChars);
		//			m_findingChars.clear();
		//		}
		//	}
		//	else
		//	{
		//		int failed_state = m_acsmStateTable[state].FailState;
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
			auto it = m_acsmStateTable[state].NextState.find(c);
			if (it != m_acsmStateTable[state].NextState.end())
			{
				state = it->second;
				if (m_acsmStateTable[state].MatchList.size() != 0)
				{
					ret = true;
					//int size = m_acsmStateTable[state].MatchList;
					//m_foundedChars.splice(m_foundedChars.end(), m_findingChars);
					//begin存放最长的匹配
					int size =  m_acsmStateTable[state].MatchList.begin()->casepatrn.size();
					//从当前p往前size个字符全是敏感字
					for (c32 *index = p; size > 0; size--, index--)
					{
						m_replaceChars.insert(index);
					}
				}
			}
			else
			{
				int failed_state = m_acsmStateTable[state].FailState;
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

	//************************************
	// 函数说明:添加模式串到状态机中
	// 返 回 值: void
	// 参 数: const acsm_pattern & p 模式串
	//************************************
	void _AddPatternStates(const acsm_pattern  &p)
	{
		//创建goto表
		int state = 0, next, n;
		int index = 0;
		//匹配已有的模式
		for ( ;index < p.patrn.length(); ++index)
		{
			c32 str = p.patrn[index];
			auto it = m_acsmStateTable[state].NextState.find(str);
			if (it == m_acsmStateTable[state].NextState.end())
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
			m_maxStates++;
			m_acsmStateTable[state].NextState[str] = m_maxStates;
			m_acsmStateTable[m_maxStates] = acsm_statetable();
			state = m_maxStates;
		}
		//构建out表
		m_acsmStateTable[state].MatchList.push_back(p);
	}



	
	//************************************
	// 函数说明: 构建状态机 (failed表和调整out表)
	// 返 回 值: void
	//************************************
	void _Build_NFA()
	{
		std::queue<NODE_STATE> stateQueue;			
		//State 0 的failed值为-1
		m_acsmStateTable[0].FailState = -1;
		//度 为1的failed值为0
		auto hashmap =  m_acsmStateTable[0].NextState;
		for (auto it = hashmap.begin(); it != hashmap.end(); ++it)
		{
			m_acsmStateTable[it->second].FailState = 0;
			stateQueue.push(it->second);
		}
		//从跟节点开始广度遍历所有State
		while (!stateQueue.empty())
		{
			NODE_STATE state = stateQueue.front();
			stateQueue.pop();
			auto nextStates = m_acsmStateTable[state].NextState;
			for (auto it = nextStates.begin(); it != nextStates.end(); ++it)
			{
				c32 c = it->first;
				//前一个state的failed值
				int lastFailed = m_acsmStateTable[state].FailState;
				//下一个state的failed值
				int nextState = it->second;
				stateQueue.push(nextState);

				auto iter = m_acsmStateTable[lastFailed].NextState.find(c);
				if (iter != m_acsmStateTable[lastFailed].NextState.end()){
					//下一个节点的failed值=前一个节点的failed值通过c可以到达的state 
					m_acsmStateTable[nextState].FailState = iter->second;
					//调整out输出因为这个包含了failed状态的输出
					if (m_acsmStateTable[iter->second].MatchList.size() >= 1){
						m_acsmStateTable[nextState].MatchList.insert(m_acsmStateTable[nextState].MatchList.end(), m_acsmStateTable[iter->second].MatchList.begin(), m_acsmStateTable[iter->second].MatchList.end());
					}
				}
				else{
					//前一个节点的failed值通过c不可以到达的下一个节点failed值为0
					m_acsmStateTable[nextState].FailState = 0;
				}
			}
		}
	}

	std::set<c32*> m_replaceChars;
	//int acsmMaxStates;		
	int m_maxStates;		//最大状态
	list<acsm_pattern> m_acsmPatterns;		//待检查的模式串集合
	unordered_map<NODE_STATE, acsm_statetable> m_acsmStateTable;		//hash key:state  value: goto表
};






#endif

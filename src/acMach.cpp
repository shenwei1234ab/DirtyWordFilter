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
#include "acMach.h"

AcMachine::AcMachine()
{
	//char filterNode = '#';
	//Unicode::convert(&filterNode, 1, &m_RegularRexNode);
	//建立0号节点
	AcNode firstNode(0);
	firstNode.m_failState = -1;
	m_acsmStateTable.insert({ 0, firstNode });
}

bool AcMachine::Init()
{
	return LoadFromFile("dirtywords.txt", "filterCharacter.txt");
}

bool AcMachine::LoadFromFile(const std::string &pattarnFile, const std::string &filterFile)
{
	//ifstream myfile("actext.txt");
	ifstream myfile(pattarnFile);
	std::string str;
	if (!myfile.is_open())
	{
		printf("from file %s failed \n", pattarnFile.c_str());
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
		AddPattern(u32_str.c_str(), u32_str.size());
	}

	ifstream filterStream(filterFile);
	if (filterStream.is_open())
	{
		while (!filterStream.eof())
		{
			getline(filterStream, str);
			if (str == "")
			{
				continue;
			}
			std::u32string u32_str;
			Unicode::convert(str.c_str(), str.size(), u32_str);
			for (auto iter = u32_str.begin(); iter != u32_str.end(); ++iter)
			{
				m_filterPatterns.insert(*iter);
			}
		}
	}

	Compile();
	return true;
}


void AcMachine::AddPattern(const c32 *pArray, size_t size)
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
	++m_nPatternCounts;
}

bool AcMachine::Check(const c32 *pStr, size_t size, std::list<str32> &outMatchingList)
{
	bool ret = false;
	NODE_STATE state = 0;
	for (; size >= 0 && *pStr; --size, ++pStr)
	{
		c32 c = *pStr;
		if (m_filterPatterns.count(c) != 0)
		{
			continue;
		}
		auto it = m_acsmStateTable[state].m_nextState.find(c);
		if (it != m_acsmStateTable[state].m_nextState.end())
		{
			state = it->second.m_stateId;
			if (m_acsmStateTable[state].m_matchList.size() != 0)
			{
				ret = true;
				//找到匹配的节点
				for (auto iter = m_acsmStateTable[state].m_matchList.begin(); iter != m_acsmStateTable[state].m_matchList.end(); ++iter)
				{
					//输出匹配的字符
					outMatchingList.emplace_back(iter->patrn);
					/*		std::string matchingStr;
							Unicode::convert(iter->patrn.c_str(), matchingStr);
							std::cout << out << std::endl;*/
				}
			}
		}
		else
		{
			//若没找到,就继续跳fail,直到跳到root都没有
			NODE_STATE failed_state = m_acsmStateTable[state].m_failState;
			//跳到了根节点都没有,failed跳回根节点(todo能否)
			if (failed_state == -1)
			{
				continue;
			}
			else
			{

				state = failed_state;
				pStr--;
			}
		}
	}
	return ret;
}


void AcMachine::Compile()
{
	for (auto iter = m_acsmPatterns.begin(); iter != m_acsmPatterns.end(); ++iter)
	{
		acsm_pattern &pattern = *iter;
		//step1:构建 trie树
		_BuildTrieTreeStates(pattern);
		//step2:构建out表
		m_acsmStateTable[m_maxStates].m_matchList.push_back(std::move(pattern));
	}
	m_acsmPatterns.clear();
	//step3 建failed表和调整out表
	_Build_NFA();
}


bool AcMachine::Replace(c32 *pStr, size_t size, const c32  &replaceChar)
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
	for (NODE_STATE state = 0; size > 0 && *pStr; --size, ++pStr)
	{
		c32 c = *pStr;
		if (m_filterPatterns.count(c) != 0)
		{
			continue;
		}
		auto it = m_acsmStateTable[state].m_nextState.find(c);
		if (it != m_acsmStateTable[state].m_nextState.end())
		{
			state = it->second.m_stateId;
			if (m_acsmStateTable[state].m_matchList.size() != 0)
			{
				ret = true;
				//int size = m_acsmStateTable[state].MatchList;
				//m_foundedChars.splice(m_foundedChars.end(), m_findingChars);
				//begin存放最长的匹配
				int size = m_acsmStateTable[state].m_matchList.begin()->casepatrn.size();
				//从当前p往前size个字符全是敏感字,todo中间可能隔着重复的字符
				for (c32 *index = pStr; size > 0; index--)
				{
					//特殊字符
					if (m_filterPatterns.count(*index) != 0)
					{
						continue;
					}
					else
					{
						m_replaceChars.insert(index);
						size--;
					}
				}
			}
		}
		else
		{
			NODE_STATE failed_state = m_acsmStateTable[state].m_failState;
			if (failed_state == -1)
			{

			}
			else
			{
				state = failed_state;
				--pStr;
				++size;
			}
		}
	}

	for (auto iter = m_replaceChars.begin(); iter != m_replaceChars.end(); ++iter)
	{
		*(*iter) = replaceChar;
	}
	return ret;
}


void AcMachine::_BuildTrieTreeStates(const acsm_pattern  &p)
{
	//创建goto表
	int state = 0;
	int index = 0;
	//匹配已有的模式
	for (; index < p.patrn.length(); ++index)
	{
		c32 str = p.patrn[index];
		unordered_map<c32, AcNode>::iterator it = m_acsmStateTable[state].m_nextState.find(str);
		if (it == m_acsmStateTable[state].m_nextState.end())
		{
			break;
		}
		state = it->second.m_stateId;
	}
	if (index >= p.patrn.length())
	{
		return;
	}
	//加入新的模式(is  rs)
	for (; index < p.patrn.length(); ++index)
	{
		c32 str = p.patrn[index];
	
		//加入新的模式
		m_maxStates++;
		//新的节点
		AcNode newNode(m_maxStates);
		AcNode &oldNode = m_acsmStateTable[state];
		//建立节点间的映射关系
		oldNode.m_nextState[str] = newNode;
		m_acsmStateTable[m_maxStates] = newNode;
		state = m_maxStates;
		m_acsmStateTable[ state ] = newNode ;
	}
}


void AcMachine::_Build_NFA()
{
	//当前已知failed值的node节点
	std::queue<NODE_STATE> stateQueue;
	auto &stateMap = m_acsmStateTable[0].m_nextState;
	//度 为1的failed值为0
	for (auto it = stateMap.begin(); it != stateMap.end(); ++it)
	{
		NODE_STATE nextState = it->second.m_stateId;
		m_acsmStateTable[nextState].m_failState = 0;
		stateQueue.push(nextState);
	}
	//从跟节点开始广度遍历所有State
	while (!stateQueue.empty())
	{
		NODE_STATE curState = stateQueue.front();
		stateQueue.pop();

		AcNode &nowNode = m_acsmStateTable[curState];
		NODE_STATE nowFailState = nowNode.m_failState;
		unordered_map<c32, AcNode> &nextStateMap = nowNode.m_nextState;
		//根据nowNode计算后面state的failed值
		for (auto it = nextStateMap.begin(); it != nextStateMap.end(); ++it)
		{
			c32 c = it->first;
			//下一个节点
			NODE_STATE nextState = it->second.m_stateId;
			stateQueue.push(nextState);
			//跳fail,直到跳到根节点或者找到
			while (m_acsmStateTable[nowFailState].m_nextState.find(c) == m_acsmStateTable[nowFailState].m_nextState.end() && -1 != nowFailState)
			{
				nowFailState = m_acsmStateTable[nowFailState].m_failState;
			}
			//找到
			if (nowFailState != -1)
			{
				NODE_STATE newFailStatus =  m_acsmStateTable[nowFailState].m_nextState[c].m_stateId;
				m_acsmStateTable[nextState].m_failState = newFailStatus;
				if (m_acsmStateTable[newFailStatus].m_matchList.size() >= 1)
				{
					m_acsmStateTable[nextState].m_matchList.insert(m_acsmStateTable[nextState].m_matchList.end(), m_acsmStateTable[newFailStatus].m_matchList.begin(), m_acsmStateTable[newFailStatus].m_matchList.end());
				}
			}
			//没有找到指向根节点
			else
			{
				m_acsmStateTable[nextState].m_failState = 0;
			}
			////根据当前state计算下一个state的failed值
			//auto iter = m_acsmStateTable[nowFailState].m_nextState.find(c);
			//if (iter != m_acsmStateTable[nowFailState].m_nextState.end())
			//{
			//	//nextState值=nowNode的failed值通过c可以到达的state 
			//	NODE_STATE failedState = iter->second.m_stateId;
			//	m_acsmStateTable[nextState].m_failState = failedState;
			//	//调整out输出因为这个包含了failed状态的输出
			//	if (m_acsmStateTable[failedState].m_matchList.size() >= 1)
			//	{
			//		m_acsmStateTable[nextState].m_matchList.insert(m_acsmStateTable[nextState].m_matchList.end(), m_acsmStateTable[failedState].m_matchList.begin(), m_acsmStateTable[failedState].m_matchList.end());
			//	}
			//}
			//else
			//{
			//	//nowNode无法通过m_failState到达nextState设置失效值为0,也就是从state0开始,
			//	//bug,直到跳到root还没有,而不是直接指向root,https://www.bilibili.com/video/BV1Nk4y1B7SL
			//	m_acsmStateTable[nextState].m_failState= 0;
			//}
		}
	}
}
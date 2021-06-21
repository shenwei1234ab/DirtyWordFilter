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
#include "acTrie.h"

AcTrie::AcTrie()
{
	//char filterNode = '#';
	//Unicode::convert(&filterNode, 1, &m_RegularRexNode);
	//建立0号节点
	m_root = new State(0,0);
}

bool AcTrie::Init()
{
	return LoadFromFile("dirtywords.txt", "filterCharacter.txt");
}

bool AcTrie::LoadFromFile(const std::string &pattarnFile, const std::string &filterFile)
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


void AcTrie::AddPattern(const c32 *pArray, size_t size)
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

State * AcTrie::getState(State* pCurrentState, const  c32  &character)
{
	State* newCurrentState = pCurrentState->getNextState(character);  // 先按success跳转
	while (newCurrentState == nullptr) // 跳转失败的话，按failure跳转,直到跳到root都没有的话就返回root
	{
		pCurrentState = pCurrentState->getFailureState();
		newCurrentState = pCurrentState->getNextState(character);
	}
	return newCurrentState;
}

bool AcTrie::Check(const c32 *pStr, size_t size, std::list<str32> &outMatchingList)
{
	bool ret = false;
	State *pState = m_root;
	for (; size >= 0 && *pStr; --size, ++pStr)
	{
		c32 c = *pStr;
		if (m_filterPatterns.count(c) != 0)
		{
			continue;
		}
		pState = getState(pState, c);
		if (pState && pState!= m_root)
		{
			std::set<str32>& states = pState->getEmit();
			for (auto iter = states.begin(); iter != states.end(); ++iter)
			{
				outMatchingList.emplace_back(*iter);
			}
			ret = true;
		}
	}
	return ret;
}


void AcTrie::Compile()
{
	for (auto iter = m_acsmPatterns.begin(); iter != m_acsmPatterns.end(); ++iter)
	{
		acsm_pattern &pattern = *iter;
		//step1:构建 trie树
		ConstructTrieStates(pattern.patrn);
	}
	m_acsmPatterns.clear();
	//step3 建failed表和调整out表
	ConstructFailureStates();
}


bool AcTrie::Replace(c32 *pStr, size_t size, const c32  &replaceChar)
{
	return true;
	//m_replaceChars.clear();
	//bool ret = false;
	//for (NODE_STATE state = 0; size > 0 && *pStr; --size, ++pStr)
	//{
	//	c32 c = *pStr;
	//	if (m_filterPatterns.count(c) != 0)
	//	{
	//		continue;
	//	}
	//	auto it = m_acsmStateTable[state].m_nextState.find(c);
	//	if (it != m_acsmStateTable[state].m_nextState.end())
	//	{
	//		state = it->second.m_stateId;
	//		if (m_acsmStateTable[state].m_matchList.size() != 0)
	//		{
	//			ret = true;
	//			//int size = m_acsmStateTable[state].MatchList;
	//			//m_foundedChars.splice(m_foundedChars.end(), m_findingChars);
	//			//begin存放最长的匹配
	//			int size = m_acsmStateTable[state].m_matchList.begin()->casepatrn.size();
	//			//从当前p往前size个字符全是敏感字,todo中间可能隔着重复的字符
	//			for (c32 *index = pStr; size > 0; index--)
	//			{
	//				//特殊字符
	//				if (m_filterPatterns.count(*index) != 0)
	//				{
	//					continue;
	//				}
	//				else
	//				{
	//					m_replaceChars.insert(index);
	//					size--;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		NODE_STATE failed_state = m_acsmStateTable[state].m_failState;
	//		if (failed_state == -1)
	//		{

	//		}
	//		else
	//		{
	//			state = failed_state;
	//			--pStr;
	//			++size;
	//		}
	//	}
	//}

	//for (auto iter = m_replaceChars.begin(); iter != m_replaceChars.end(); ++iter)
	//{
	//	*(*iter) = replaceChar;
	//}
	//return ret;
}


void AcTrie::ConstructTrieStates(const str32  &patrn)
{
	if (patrn.length() <= 0)
	{
		return;
	}
	//创建goto表
	State* currentState = m_root;
	for (int index = 0; index < patrn.length(); ++index)
	{
		c32 str = patrn[index];
		currentState = currentState->addState(str,m_maxStates);
	}
	currentState->addEmit(patrn);

}


void AcTrie::ConstructFailureStates()
{
	//当前已知failed值的node节点
	if (!m_root)
	{
		return;
	}
	std::queue<State*> stateQueue;
	auto &stateMap = m_root->getNextStates();
	//度 为1的failed值为0
	for (auto it = stateMap.begin(); it != stateMap.end(); ++it)
	{
		State* pNextState = it->second;
		pNextState->setFailure(m_root);
		stateQueue.push(pNextState);
	}

	auto &asciMap = m_root->getNextAsciStates();
	for (auto it = asciMap.begin(); it != asciMap.end(); ++it)
	{
		State* pNextState = it->second;
		pNextState->setFailure(m_root);
		stateQueue.push(pNextState);
	}
	//从跟节点开始广度遍历所有State
	while (!stateQueue.empty())
	{
		State* curState = stateQueue.front();
		stateQueue.pop();


		unordered_map<c32, State*> &nextStateMap = curState->getNextStates();
		//根据nowNode计算后面state的failed值
		for (auto it = nextStateMap.begin(); it != nextStateMap.end(); ++it)
		{
			c32 c = it->first;
			//下一个节点
			State* nextState = it->second;
			stateQueue.push(nextState);
			//一直跳fail,直到跳到根节点或者找到
			State* traceFailureState = curState->getFailureState();
			while (traceFailureState->getNextState(c) == nullptr)
			{
				traceFailureState = traceFailureState->getFailureState();
			}
			State *pNewFailureState = traceFailureState->getNextState(c);
			nextState->setFailure(pNewFailureState);
			//补充out输出
			std::set<str32>& matchList = nextState->getEmit();
			matchList.insert(pNewFailureState->getEmit().begin(), pNewFailureState->getEmit().end());
		}


		unordered_map<c8, State*> &nextAsciiStateMap = curState->getNextAsciStates();
		//根据nowNode计算后面state的failed值
		for (auto it = nextAsciiStateMap.begin(); it != nextAsciiStateMap.end(); ++it)
		{
			c8 c = it->first;
			//下一个节点
			State* nextState = it->second;
			stateQueue.push(nextState);
			//一直跳fail,直到跳到根节点或者找到
			State* traceFailureState = curState->getFailureState();
			while (traceFailureState->getNextState(c) == nullptr)
			{
				traceFailureState = traceFailureState->getFailureState();
			}
			State *pNewFailureState = traceFailureState->getNextState(c);
			nextState->setFailure(pNewFailureState);
			//补充out输出
			std::set<str32>& matchList = nextState->getEmit();
			matchList.insert(pNewFailureState->getEmit().begin(), pNewFailureState->getEmit().end());
		}
	}
}
#ifndef _AC_TRIE_H
#define _AC_TRIE_H
#include <list>
#include "dirtybase.h"
#include <vector>
#include <unordered_map>
#include "Unicode.h"
#include <set>
using namespace std;


#define ACSM_FAIL_STATE -1

typedef int NODE_STATE;

static c32 asciiMin = '0';
static c32 asciiMax = 'z';

struct acsm_pattern
{
	str32 patrn;			//大写模式串
	str32 casepatrn;		//原始模式串
	int      n;				//模式串个数
	int      nocase;		//是否大写
};


struct State
{
public:
	State(int nDepth,int nStateId):m_nDepth(nDepth),m_nStateId(nStateId)
	{
		if (nDepth == 0)
		{
			m_pRootState = this;
		}
	}

	State* addState(const  c32  &character,int32_t &outState)
	{
		//m_nStateId++;
		State* pNextState = _nextState(character, true);
		if (pNextState == nullptr)
		{
			outState++;
			pNextState = new State(m_nDepth + 1, outState);
			if (character >= asciiMin && character <= asciiMax)
			{
				c8 asciCharacter = character;
				m_asciiNextState[asciCharacter] = pNextState;
			}
			else
			{
				m_nextState[character] = pNextState;
			}
		}
		return pNextState;
	}

	void setFailure(State *pState)
	{
		m_failState = pState;
	}
	void addEmit(const str32  &pattern)
	{
		m_matchList.insert(pattern);
	}

	std::set<str32>& getEmit() {
		return m_matchList;
	}

	unordered_map<c32, State*>& getNextStates() {return m_nextState;}
	unordered_map<c8, State*>& getNextAsciStates() { return m_asciiNextState; }
	State* getFailureState() {return m_failState;}

	State * getNextState(const  c32  &character)
	{
		return _nextState(character, false);
	}


private:
	int m_nStateId = 0;
	int m_nDepth = 0;
	//当前是否是根节点
	State * m_pRootState = NULL;
	
	//下个可以到达的状态映射表
	unordered_map<c32, State*>  m_nextState;//key:模式串字符  value:可以到达的下一个状态 中文
	unordered_map<c8, State*>  m_asciiNextState;//英文



	State* m_failState;		//失效状态
	std::set<str32> m_matchList;		//当前可以匹配输出的模式串
	State * _nextState(const  c32  &character, bool ignoreRootState)
	{
		State * pNextState = nullptr;
		if (character >= asciiMin && character <= asciiMax)
		{
			c8 asciCharacter = character;
			auto iter = m_asciiNextState.find(asciCharacter);
			if (iter != m_asciiNextState.end())
			{
				pNextState = iter->second;
			}
		}
		else
		{
			auto iter = m_nextState.find(character);
			if (iter != m_nextState.end())
			{
				pNextState = iter->second;
			}
		}
		if (!ignoreRootState && pNextState == NULL && m_pRootState != NULL)
		{
			pNextState = m_pRootState;
		}
		return pNextState;
	}
};


class AcTrie :public DirtyProcessor
{
public:
	AcTrie();
public:
	virtual bool Init()override;

	//************************************
	// 函数说明:从文件中读取模式串初始化ac状态机
	// 返 回 值: void
	// 参 数: const std::string & pattarnFile:模式串的文件  filterFile:标点符号的文件
	//************************************
	bool LoadFromFile(const std::string &pattarnFile, const std::string &filterFile);

	//************************************
	// 函数说明:添加模式串数组
	// 返 回 值: void
	// 参 数: const c32 * pArray
	// 参 数: size_t size
	//************************************
	void AddPattern(const c32 *pArray, size_t size);


	//************************************
	// 函数说明:检查字符串是否存在敏感字
	// 返 回 值: bool
	// 参 数: const c32 * p 要检查的字符串
	// 参 数: size_t size
	// 参 数: std::list<str32> & outMatchingList 返回存在的敏感字
	//************************************
	virtual bool Check(const c32 *pStr, size_t size, std::list<str32> &outMatchingList)override;


	//
	//************************************
	// 函数说明: 建立goto表和out表和failed表
	// 返 回 值: void
	//************************************
	void Compile();


	//************************************
	// 函数说明:脏字替换
	// 返 回 值: bool
	// 参 数: c32 * p 待检查的字符串
	// 参 数: size_t size
	// 参 数: c32 replaceChar 要取代的字符
	//************************************
	virtual bool Replace(c32 *pStr, size_t size, const c32  &replaceChar)override;

	virtual int32_t GetNodeCount()override { return m_maxStates; }
	virtual int32_t GetPatternCount()override { return m_nPatternCounts; }


	//跳转到下一个状态
	State * getState(State* pCurrentState ,const  c32  &character);
private:
	//跟节点
	State *m_root = nullptr;
	int32_t m_nPatternCounts = 0;

	//************************************
	// 函数说明:添加模式串到状态机中,构建字典树
	// 返 回 值: void
	// 参 数: const str32 & p 模式串
	//************************************
	void ConstructTrieStates(const str32  &p);



	//************************************
	// 函数说明: 构建failed表和调整out表
	// 返 回 值: void
	//************************************
	void ConstructFailureStates();

	std::set<c32*> m_replaceChars;
	//int acsmMaxStates;		
	int m_maxStates;		//最大状态
	list<acsm_pattern> m_acsmPatterns;		//待检查的模式串集合
	//过滤的标点符号集合
	std::set<c32> m_filterPatterns;

private:
};






#endif

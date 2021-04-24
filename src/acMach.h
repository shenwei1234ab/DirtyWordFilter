#ifndef _AC_MACH_H
#define _AC_MACH_H
#include <list>
#include "dirtybase.h"
#include <vector>
#include <unordered_map>
#include "Unicode.h"
#include <set>
using namespace std;


#define ACSM_FAIL_STATE -1

typedef int NODE_STATE;

struct acsm_pattern
{
	str32 patrn;			//大写模式串
	str32 casepatrn;		//原始模式串
	int      n;				//模式串个数
	int      nocase;		//是否大写
};


struct AcNode
{
public:
	AcNode(int stateId):m_stateId(stateId) 
	{
			
	}


	AcNode():m_stateId(0)
	{

	}

	NODE_STATE m_stateId;		
	//下个可以到达的状态映射表
	unordered_map<c32, AcNode>  m_nextState;//key:模式串字符  value:可以到达的下一个状态
	NODE_STATE m_failState;		//失效状态
	std::list<acsm_pattern> m_matchList;		//当前可以匹配输出的模式串
};

//struct acsm_statetable
//{
//	unordered_map<c32, NODE_STATE>  NextState;//key:模式串字符  value:可以到达的下一个状态
//	NODE_STATE      FailState;			//failed值(失效的时候要跳往的状态)
//	std::list<acsm_pattern> MatchList;		//当前可以匹配输出的模式串
//	bool m_loop = false;
//};



class AcMachine :public DirtyProcessor
{
public:
	AcMachine();


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
private:

	//************************************
	// 函数说明:添加模式串到状态机中,构建字典树
	// 返 回 值: void
	// 参 数: const acsm_pattern & p 模式串
	//************************************
	void _BuildTrieTreeStates(const acsm_pattern  &p);



	//************************************
	// 函数说明: 构建状态机 (failed表和调整out表)
	// 返 回 值: void
	//************************************
	void _Build_NFA();

	std::set<c32*> m_replaceChars;
	//int acsmMaxStates;		
	int m_maxStates;		//最大状态
	list<acsm_pattern> m_acsmPatterns;		//待检查的模式串集合
	unordered_map<NODE_STATE, AcNode> m_acsmStateTable;		//hash key:stateId  value:AcNode

	//过滤的标点符号集合
	std::set<c32> m_filterPatterns;

	//特殊的字符比如#表示任意多个,todo
	//c32 m_RegularRexNode;

	int32_t m_nPatternCounts;
};






#endif

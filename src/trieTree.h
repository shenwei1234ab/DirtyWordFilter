//tet
#ifndef _TRIETREE_H_
#define _TRIETREE_H

#include <iostream>
#include <string>
#include <map>
#include "dirtybase.h"
#include <unordered_map>
#include "Unicode.h"
#include <set>

class TrieTree :public DirtyProcessor
{
public:
	virtual bool Init()override;
	bool LoadFromFile(const std::string &pattarnFile, const std::string &filterFile);
	void Add(const c32 *pArray, size_t size);
	virtual int32_t GetNodeCount()override {return m_nNodeCounts;}
	virtual int32_t GetPatternCount()override { return m_nPatternCounts; }
	virtual bool Check(const c32 *p, size_t size, std::list<str32> &outMatchingList)override;
	
	virtual bool Replace(c32 *pStr, size_t size, const c32  &replaceChar)override;
private:
	bool _check(c32 *p, size_t size, std::list<str32> &outMatchingList);
	struct Node
	{
		c32 val;
		std::unordered_map<c32, Node> nodes;
		//
		bool m_boolEnd = false;
		//输出的
		std::u32string out;
	};
	Node m_root;

	//节点的个数
	int32_t m_nNodeCounts = 0;
	//文本串的个数
	int32_t m_nPatternCounts = 0;
	std::list<c32*> m_findingNodes;


	//匹配的字符
	std::list<c32*> m_matchingNodes;

	//过滤的标点符号集合
	std::set<c32> m_filterPatterns;
};


#endif 
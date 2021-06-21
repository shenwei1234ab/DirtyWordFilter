#include "trieTree.h"
#include <iostream>     
#include <fstream>      

bool TrieTree::LoadFromFile(const std::string &pattarnFile, const std::string &filterFile)
{
	std::ifstream myfile(pattarnFile);
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
		std::u32string u32_str;
		Unicode::convert(str.c_str(), str.size(), u32_str);
		Add(u32_str.c_str(), u32_str.size());
	}

	std::ifstream filterStream(filterFile);
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
	return true;
}

bool TrieTree::Init()
{
	return LoadFromFile("dirtywords.txt", "filterCharacter.txt");
}




void TrieTree::Add(const c32 *pArray, size_t size)
{
	Node *pNode = &m_root;
	std::u32string out = pArray;
	for (; size&&*pArray; --size, ++pArray)
	{
		c32 c = *pArray;
		if(pNode->nodes.find(c) == pNode->nodes.end())
		{
			Node *pNewNode = new Node();
			pNode->nodes[c] = pNewNode;
		}
		pNode = pNode->nodes[c];
		pNode->val = c;
		++m_nNodeCounts;
	}
	pNode->m_boolEnd = true;
	pNode->out = out;
	++m_nPatternCounts;
}

bool TrieTree::_check(c32 *p, size_t size, std::list<str32> &outMatchingList)
{
	m_findingNodes.clear();
	m_matchingNodes.clear();
	bool if_find = false;
	for (; size && *p; --size, ++p) {
		m_findingNodes.clear();
		auto q = p;
		auto m = size;
		auto node = &m_root;
		do {
			m_findingNodes.push_back(q);
			c32 c = *q;
			if (m_filterPatterns.count(c) != 0)
			{
				continue;
			}
			auto it = node->nodes.find(c);
			if (it == node->nodes.end()) {
				break;
			}
			node = it->second;
			if (node->m_boolEnd)
			{
				//��ƥ���
				m_matchingNodes.insert(m_matchingNodes.end(), m_findingNodes.begin(), m_findingNodes.end());
				outMatchingList.emplace_back(node->out);
				if_find = true;
				if (node->nodes.size() == 0)
				{
					break;
				}
			}
			else
			{
				if (node->nodes.size() == 0)
				{
					break;
				}
			}

		} while (--m && *++q);
	}
	return if_find;
}

bool TrieTree::Check(const c32 *p, size_t size, std::list<str32> &outMatchingList)
{
	return _check(const_cast<c32 *>(p),size, outMatchingList);
}


 bool TrieTree::Replace(c32 *pStr, size_t size, const c32  &replaceChar)
{
	m_findingNodes.clear();
	m_matchingNodes.clear();
	std::list<str32> outMatchingList;
	if (!_check(pStr, size, outMatchingList))
	{
		return false;
	}
	for (auto iter = m_matchingNodes.begin(); iter != m_matchingNodes.end(); ++iter)
	{
		*(*iter) = replaceChar;
	}
	return true;
}

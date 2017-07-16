#ifndef _TRIETREE_H_
#define _TRIETREE_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include "dirtybase.h"

class TrieTree :public DirtyProcessor
{
public:
	virtual void Init()
	{
		//test
		Add("he", 2);
		Add("she", 3);
		Add("his", 3);
		Add("hers", 4);
	}
	virtual void Add(const char *pArray, size_t size)
	{
		Node *pNode = &m_root;
		std::string out = pArray;
		for (; size&&*pArray; --size, ++pArray)
		{
			char c = *pArray;
			if (c >= 'A' && c <= 'Z') {
				c = c - 'A' + 'a';
			}
			pNode = &pNode->nodes[c];
			pNode->val = c;
		}
		pNode->out = out;
	}

	virtual bool Check(const char *p, size_t size)
	{
		bool if_find = false;
		for (; size && *p; --size, ++p) {
			auto q = p;
			auto m = size;
			auto node = &m_root;
			do {
				char c = *q;
				if (c >= 'A' && c <= 'Z') {
					c = c - 'A' + 'a';
				}
				auto it = node->nodes.find(c);
				if (it == node->nodes.end()) {
					break;
				}
				node = &it->second;
				if (node->out != "") {
					//ÓÐÆ¥ÅäµÄ
					std::cout << "find" << node->out.c_str() << std::endl;
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

private:
	struct Node
	{
		char val;
		std::map<char, Node> nodes;
		std::string out = "";
	};

	Node m_root;
};


#endif 
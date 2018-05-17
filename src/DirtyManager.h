#ifndef _DIRTY_MANAGER_
#define _DIRTY_MANAGER_
#include <map>
#include <iostream>
#include <stdio.h>
#include <string>
#include "dirtybase.h"
#include "acMach.h"
//template <class T>
class DiryManger
{
private:
	DiryManger()   
	{
		//m_tree = new T();
	}
	DiryManger(const DiryManger &);
	DiryManger & operator = (const DiryManger &);
	~DiryManger()
	{
		//delete (m_tree);
	}
public:
	static DiryManger & GetInstance()
	{
		static DiryManger instance;
		return instance;
	}

	bool Init()
	{
		m_tree.Init();
		return true;
	}
#ifdef _UTF8
	bool Check(const string &str)
#else
	bool Check(const str32 &str)
#endif
	{
		return m_tree.Check(str.c_str(), str.length());
	}


#ifdef _UTF8
	void Replace(const string &src,char replace,char *pout)
#else
	void Replace(const str32 &src,c32 replace,c32 *pout)
#endif
	{
		//std::cout<<"before"<<std::endl;
//		c32 *idx = pout;
//		for(int i=0;i<src.length();++i)
//		{
//			*idx = src.at(i);
//			std::cout<< *idx << ","<<std::endl;
//			idx ++;
//		}
		m_tree.Replace(src.c_str(), src.length(),replace);
	}


private:
	//DirtyProcessor *m_tree;
	AcMachine m_tree;
};

#endif

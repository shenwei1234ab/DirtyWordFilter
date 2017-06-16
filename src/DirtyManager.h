#ifndef _DIRTY_MANAGER_
#define _DIRTY_MANAGER_
#include <map>
#include <iostream>
#include <stdio.h>
#include <string>
#include "dirtybase.h"

template <class T>
class DiryManger
{
private:
	DiryManger()   
	{
		m_tree = new T();
	}
	DiryManger(const DiryManger &);
	DiryManger & operator = (const DiryManger &);
	~DiryManger()
	{
		delete (m_tree);
	}
public:
	static DiryManger & GetInstance()
	{
		static DiryManger instance;   //局部静态变量  
		return instance;
	}

	bool Init()
	{
		m_tree->Init();
		return true;
	}

	bool Check(const std::string &str)
	{
		return m_tree->Check(str.c_str(), str.length());
	}
private:
	DirtyProcessor *m_tree;
};

#endif 
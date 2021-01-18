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
		
	}
	~DiryManger()
	{
		
	}
public:
	static DiryManger & GetInstance()
	{
		static DiryManger instance;
		return instance;
	}

	bool Init()
	{
		return m_tree.LoadFromFile("actext.txt");
	}

	bool Check(const str32 &str)
	{
		return m_tree.Check(str.c_str(), str.length());
	}



	bool Replace(str32 &src,c32 replace)
	{
		//int size = src.length();
		//memcpy(pout, src.c_str(), size+1);
		return m_tree.Replace(const_cast<c32*>(src.c_str()), src.length(), replace);
		//m_tree.Replace(pout, size, replace);
	}


private:
	//DirtyProcessor *m_tree;
	AcMachine m_tree;
};

#endif

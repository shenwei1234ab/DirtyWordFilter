#ifndef _DIRTY_MANAGER_
#define _DIRTY_MANAGER_
#include <map>
#include <iostream>
#include <stdio.h>
#include <string>
#include "dirtybase.h"
//template <class T>
class DirtyProcessor;
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

	bool Init(DirtyProcessor *pProcess);

	bool Check(const c32 *pSource, size_t size, std::list<str32> &outMatchingList);

	bool Replace(c32 *pStr, size_t size, const c32  &replaceChar);

private:
	DirtyProcessor * m_pProcess = nullptr;
};

#endif

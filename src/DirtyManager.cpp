/*
 * DirtyManager.cpp
 *
 *  Created on: 2015年8月24日
 *      Author: 沈巍
 */

#include "DirtyManager.h"
#include "dirtybase.h"
using namespace std;

bool DiryManger::Init(DirtyProcessor *pProcess)
{
	if (!pProcess)
	{
		return false;
	}
	m_pProcess = pProcess;
	return m_pProcess->Init();
}



bool DiryManger::Check(const c32 *pSource, size_t size, std::list<str32> &outMatchingList)
{
	if (!m_pProcess)
	{
		return false;
	}
	return m_pProcess->Check(pSource, size, outMatchingList);
}



bool DiryManger::Replace(c32 *pStr, size_t size, const c32  &replaceChar)
{
	if (!m_pProcess)
	{
		return false;
	}
	return m_pProcess->Replace(pStr, size, replaceChar);
}



#ifndef _DIRTY_BASE_H_
#define _DIRTY_BASE_H_

#include <iostream>
#include <stdio.h>
#include <list>
#include <map>
#include "Unicode.h"

class DirtyProcessor
{
public:
	virtual bool Check(const c32 *p, size_t size, std::list<str32> &outMatchingList) = 0;
	virtual bool Init ()=  0;
	virtual ~DirtyProcessor(){};
	virtual bool Replace(c32 *pStr, size_t size, const c32  &replaceChar) = 0;
	virtual int32_t GetNodeCount() { return 0; }
	virtual int32_t GetPatternCount() { return 0; }
};


#endif 

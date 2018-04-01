#ifndef _DIRTY_BASE_H_
#define _DIRTY_BASE_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include "Unicode.h"

class DirtyProcessor
{
public:
	virtual void Add(const c32 *pArray, size_t size) {};
	virtual bool Check(const c32 *p, size_t size) { return false;};
	virtual void Init(){};
	virtual ~DirtyProcessor(){};
};


#endif 

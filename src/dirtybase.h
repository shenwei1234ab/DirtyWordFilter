#ifndef _DIRTY_BASE_H_
#define _DIRTY_BASE_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
using namespace std;

class DirtyProcessor
{
public:
	virtual void Add(const char *pArray, size_t size) {};
	virtual bool Check(const char *p, size_t size) { return false; };
	virtual void Init(){};
	virtual ~DirtyProcessor(){};
};


#endif 
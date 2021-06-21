/*
 * DirtyManager.h
 *
 *  Created on: 2015年8月24日
 *      Author: shenwei
 */
#define _CRT_SECURE_NO_WARNINGS
#ifndef SRC_SERVER_MANAGER_DIRTYMANGER2_H_
#define SRC_SERVER_MANAGER_DIRTYMANGER2_H_

#include "Unicode.h"
#include <vector>
class ChineseTree;
class DirtyManager2
{
public:
	DirtyManager2();
	
	virtual ~DirtyManager2()
	{ 
		Destory(); 
	}
	
	int init();
	

	int Check(const c8* content);

	int Replace(c8* content);

	void  Destory();



private:
	ChineseTree *m_tree;
	bool m_ifInitSuccess = false;
	int LoadChar(const char *pStr);
};

#endif /* SRC_SERVER_MANAGER_DIRTYMANGER_H_ */

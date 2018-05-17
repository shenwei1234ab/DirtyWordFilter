/*
 * DirtyManager.cpp
 *
 *  Created on: 2015年8月24日
 *      Author: 沈巍
 */

#include "DirtyManager2.h"
#include <stdio.h>
#include <type_traits>
#include "ChineseTree.h"
#include <fstream>
#include <string.h>
using namespace std;
DirtyManager2::DirtyManager2() :m_tree(new ChineseTree), m_ifInitSuccess(false)
{	

}

/**
*
*
* 
*
0       成功
-1		ChineseTree构造失败
-2		分配内存失败
-3      数据库字符集错误（不是utf-8）
-4		数据库读取错误
*/

int DirtyManager2::init()
{
	FILE *fd = fopen("actext.txt", "r");
	if (fd == NULL)
	{
		fprintf(stderr, "Open file error!\n");
		exit(1);
	}

	ifstream myfile("actext.txt");
	if (!myfile)
	{
		printf("open file failed\n");
	}
	//��ȡģʽ��
	std::string str;
	while (getline(myfile, str))
	{
		if (str == "")
		{
			continue;
		}
		LoadChar(str.c_str());
	}
	m_ifInitSuccess = true ;
	return 0;
}

/**
*
*
* @param  要插入ChineseTree中的utf-8字符
*
0       成功
-1		ChineseTree构造失败
-2		分配内存失败
-3		参数错误(字符不是utf-8)
*/
int DirtyManager2::LoadChar(const c8 *pStr)
{
	if (!m_tree)
	{
		return -1;
	}
	ssize_t keySize = 0;
	size_t size = strlen(pStr) + 1;
	c32  *pKey = new (std::nothrow)c32[size];
	if (!pKey)
	{
		return -2;
	}
	//convert to utf-32 Array
	keySize = Unicode::convert(pStr, size, pKey);
	if (keySize == -1)
	{
		delete pKey;
		return -3;
	}

	//add key to tree
	m_tree->Create(pKey);
	//free space
	delete []pKey;
	return 0;
}







void DirtyManager2::Destory()
{
	//释放myTree
	if (m_tree)
	{
		delete m_tree;
		m_tree = NULL;
	}
}





/**
* utf8DirtyReplace	UTF-8字符集关键字检查接口
*
* @param content	输入字符串UTF-8字符集
* @return 0 		检查成功并且没有关键字
 1			检查成功并且有关键字
-1 			初始化错误
-2 			脏字库无效错误
-3         分配内存失败
-4         输入参数错误(字符集错误或者空指针)

*/
int DirtyManager2::Check(const c8* content)
{

	if (!m_tree )
	{
		return -1;
	}
	if (!m_ifInitSuccess)
	{
		return -1;
	}
	if (!content)
	{
		return -4;
	}
	size_t  size = strlen(content) + 1;
	if (size <= 0)
	{
		return -4;
	}

	c32  *newContent = new(std::nothrow) c32[size];
	if (!newContent)
	{
		return -3;
	}

	ssize_t ret = Unicode::convert(content, size, newContent);
	if (ret == -1)
	{
		delete[]newContent;
		return  -4;
	}
	bool bret =  m_tree->Check(newContent);
	delete[]newContent;
	return bret;

}



/**
* utf8DirtyReplace	UTF-8字符集脏字综合替换接口
*
* @param iDoorLevel	    门限脏话级别
* @param pszUTF8Content	输入字符串UTF-8字符集
* @param iDirtyLevel      返回脏话级别
* @param iBuffLen         pszUTF8Content字符串所在缓冲区的最大长度,零表示采用strlen(pszUTF8Content)计算
* @return 0 			过滤成功, 如果存在脏字将pszUTF8Content中脏字替换为*
-1 			未初始化错误
-2 			脏字库无效错误
-3         分配内存失败
-4         输入参数错误(字符集错误或者空指针)

*/

int DirtyManager2::Replace(c8* content)
{
	if (!m_tree)
	{
		return -1;
	}
	if (!content)
	{
		return -4;
	}
	size_t   size = strlen(content) + 1;
	if (size <= 0)
	{
		return -4;
	}
	c32   *newContent = new(std::nothrow) c32[size];
	if (!newContent)
	{
		return -3;
	}

	ssize_t ret = Unicode::convert(content, size, newContent);
	if (ret == -1)
	{
		delete[]newContent;
		return  -4;
		//..todo
	}
	m_tree->Replace(newContent);
	//.....todo
	Unicode::convert(newContent, ret, content);

	//freespace
	delete[]newContent;
	return 0;
}




/*
**   ACSMX.H
**
**
*/
#ifndef ACSMX_H
#define ACSMX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//snort中的入侵检测
/*
*   Prototypes
*/
#define ALPHABET_SIZE    256     
#define MAXLEN 256

#define ACSM_FAIL_STATE   -1     

//模式串链表
typedef struct _acsm_pattern {

	struct  _acsm_pattern *next;
	unsigned char         *patrn;					//大写模式串
	unsigned char         *casepatrn;				//原始模式串
	int      n;										//模式串个数
	int      nocase;								//是否大写
	void   * id;
	int         nmatch;

} ACSM_PATTERN;


typedef struct  {

	/* Next state - based on input character */
	int      NextState[ALPHABET_SIZE];				//这里浪费了内存 key-字符 value-跳转状态

	/* Failure state - used while building NFA & DFA  */
	int      FailState;

	/* List of patterns that end here, if any */
	ACSM_PATTERN *MatchList;

}ACSM_STATETABLE;


/*
* State machine Struct
*/
typedef struct {

	int acsmMaxStates;						//所有状态的累加值
	int acsmNumStates;						//最大状态值(9)

	ACSM_PATTERN    * acsmPatterns;				//原始的数据链表 
	ACSM_STATETABLE * acsmStateTable;		//key-状态   value-ACSM_STATETABLE信息,out,failed

}ACSM_STRUCT;

/*
*   Prototypes
*/
ACSM_STRUCT * acsmNew();
int acsmAddPattern(ACSM_STRUCT * p, unsigned char * pat, int n, int nocase);
int acsmCompile(ACSM_STRUCT * acsm);
int acsmSearch(ACSM_STRUCT * acsm, unsigned char *Tx, int n, void(*PrintMatch) (ACSM_PATTERN * pattern, ACSM_PATTERN * mlist, int nline, int index));
void acsmFree(ACSM_STRUCT * acsm);
void PrintMatch(ACSM_PATTERN * pattern, ACSM_PATTERN * mlist, int nline, int index);
void PrintSummary(ACSM_PATTERN * pattern);

#endif
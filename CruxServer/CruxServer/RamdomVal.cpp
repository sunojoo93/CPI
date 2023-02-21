#include "StdAfx.h"
#include "RamdomVal.h"


CRamdomVal::CRamdomVal(void)
{
	//srand( (unsigned int)(time(NULL)) );
}


CRamdomVal::~CRamdomVal(void)
{
}


int CRamdomVal::m_fnGetRandomVal(int startNum, int endNum)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	srand(time.wMilliseconds); //2018.11.28 by ryu 
	return ( rand() % (endNum - startNum + 1) + startNum );
}
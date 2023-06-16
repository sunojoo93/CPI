#ifndef __TIME_CHECK_H__
#define __TIME_CHECK_H__
#include <Windows.h>

#pragma once
//
class CTimeCheck
{

public:
	CTimeCheck();
	virtual ~CTimeCheck(){;}

public:

	void  StartTime();
	float EndTime();
	void Delay(float ms);
	double GetFrameRate();

	void  sleep( float waitmsec );

private:
	LARGE_INTEGER	m_st;
	LARGE_INTEGER	m_ed;
	LARGE_INTEGER	m_freq;
	double			m_fgap;

};
#endif
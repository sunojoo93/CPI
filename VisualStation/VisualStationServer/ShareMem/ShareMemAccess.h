/*
*	File Name			:	ShareMemAccess.h
*	Function			:	ShareMemAccess.cpp 의 헤더 파일
*	Create				:	2006.03.06
*	Author				:	고정진
*	Version				:	1.0
*/
#ifndef		SHAREMEMACCESS_H
#define		SHAREMEMACCESS_H

#include <windows.h>
#include "ShareMem.h"
#include "SmMapList.h"
#include "SmMapDefine.h"

void		g_fnPositionSearch(char*, int*,int*,int*);
INT			g_fnMatchPointSearch(char*, char*);
INT			g_fnSM_MatchPointWrite(int , char *, char *,char *);

extern		SFile_Handle			SFile_Handler[FILE_NUMBER];
extern		SSM_MapDefine			m_sGetMapDefine;
extern		CSM_MapListFile			m_cGetListFile;
extern		CSM_MapDefineFile		m_cGetDefineFile;
extern		int						ERROR_CODE;
HWND		hWnd;
#endif
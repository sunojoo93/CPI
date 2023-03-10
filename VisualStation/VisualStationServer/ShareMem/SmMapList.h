/*
*	File Name			:	SmMapList.h
*	Function			:	Map List 의 Table Class 정의와, 해당 Table 의 임의의 Record Data 를
*								저장하는 구조체 선언.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
#ifndef		SMMAPLIST_H
#define		SMMAPLIST_H

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include "ShareMem.h"


/*
*	Module name			:	SFile_Handle
*	Function			:	Shared Memory 사용시 각 Page File 의 Handle 들이다.*								
*	Create				:	2006.02.26
*	Author				:	고정진
*	Version				:	1.0
*/
struct SFile_Handle
{	
	HANDLE hFile;
	HANDLE hFMap;	
	CHAR *PtrInFile;

	SFile_Handle()
	{
		memset(this, 0x00, sizeof(SFile_Handle));
	}
};


/*
*	Module name			:	CSM_MapList
*	Function			:	Map List File 의 구조화된 데이터 저장소를 정의한다.
*	Create				:	2006.02.16
*	Author				:	고정진
*	Version				:	1.0
*/

struct	SSM_MapList
{
public:
	CHAR	m_chFileName[FILE_NAME];
	INT		m_nFileNumber;
	INT		m_nFileSize;
	BOOL	m_bBackup;

	SSM_MapList()
	{
		memset(this, 0x00, sizeof(SSM_MapList));
	}
protected:	
private:
};  //테이블 정보를 임시로 저장하기 위한 저장소.


/*
*	Module name			:	CSM_MapListFile
*	Function			:	CSM_MapList 를 정의하고, 해당 저장소에 대한 IO 를 처리한다.
*	Create				:	2006.02.16
*	Author				:	고정진
*	Version				:	1.0
 */

class	CSM_MapListFile
{
public:
	CSM_MapListFile();
	~CSM_MapListFile();
	INT				m_fnReadListFile(char*);
	INT				m_fnReadListTable(int);
	INT				m_fnReadListTable(char*);
	void			m_fnReadIndexTable(int);	
	void			m_fnShareMemCreate(int,int,char*);
protected:	
private:
	SSM_MapList		m_sMapList[FILE_NUMBER];
};

#endif




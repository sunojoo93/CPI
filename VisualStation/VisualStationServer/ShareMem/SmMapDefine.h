/*
*	File Name			:	SmMapDefine.h
*	Function			:	Map Define File 의 Table Class 정의와, 해당 Table 의 임의의 Record Data 를
*							저장하는 구조체 선언.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
#ifndef		SMMAPDEFINE_H
#define		SMMAPDEFINE_H

#include <windows.h>
#include"ShareMem.h"  
#include "SmMapList.h"




/*
*	Module name			:	SSM_MapDefine
*	Function			:	Map Define File 의 구조화된 데이터 저장소를 정의한다.
*	Create				:	2006.02.16
*	Author				:	고정진
*	Version				:	1.0
*/

struct	SSM_MapDefine
{
public:
	INT		m_nIndex;
	CHAR	m_chIdenify[ID_LENGTH];
	CHAR	m_chDefault[DEFAULT_SIZE];
	INT		m_nStartPosition;
	INT		m_nLength;
	INT		m_nAccessor;
	char	m_chMatchPoint[DEFAULT_SIZE];

	SSM_MapDefine()
	{
		memset(this, 0x00, sizeof(SSM_MapDefine));
	}
protected:
private:
};



/*
*	Module name	:	CSM_MapDefineFile
*	Function			:	CSM_MapDefineFile 를 정의하고, 해당 저장소에 대한 IO 를 처리한다.
*	Create				:	2006.03.29
*	Author				:	고정진
*	Version				:	2.5
*/

class	CSM_MapDefineFile
{
public:
	CSM_MapDefineFile();
	~CSM_MapDefineFile();
	void			m_fnReadDefineFile(char *,int,int);
	INT				m_fnReadDefineTable( int, char *);
	void			StrngToUpper(char *);
	
protected:
private:
	SSM_MapDefine	m_sMapDefine[FILE_NUMBER][ID_NUMBER];
};


#endif

/*
*	File Name			:	SmMapDefine.cpp
*	Function			:	CSM_MapDefineFile 클래스 함수를 정의해 놓았다.
*							Map Define Table 의 생성 및 , 해당 Table 에 접근 함수들이 있다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/

#include "SmMapDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern		SSM_MapDefine			m_sGetMapDefine;
extern		SFile_Handle			SFile_Handler[FILE_NUMBER];

CSM_MapDefineFile::CSM_MapDefineFile()				//생성자. 특이 사항 없음.
{
	return;
}

CSM_MapDefineFile::~CSM_MapDefineFile()				//소멸자 특이사항 없음
{
	return;
}


/*
*	Module Name		:	m_fnReadListFile
*	Parameter		:	경로를 포함한 File Name, Table Index, Initial Type
*	Return			:	None
*	Function		:	초기 DllMain() 에서 DLL_PROCESS_ATTACH 때에 호출되어,
*							Map Define File 을 읽어, m_cMapDefine 에 적재한다.
*	Create			:	2006.03.29
*	Author			:	고정진
*	Version			:	2.5
*/
void		CSM_MapDefineFile::m_fnReadDefineFile(char* chFileName, int nFileIndex, int nInitType)
{
	char		chBuff[READ_BUFFER];						//파일을 읽을때 사용하는 임시 버퍼
	char		chSeps[]   = " \t\n";						//코큰 구분자
	char		*chToken;									//토큰
	int			nItemNumber=0;								//아이템 넘버 인덱스	

	const		char		chUserInput[]="NONE";

	ifstream infile (chFileName);							//파일을 Open 한다.

	while (infile.getline(chBuff,READ_BUFFER)){				//한 라인을 읽는다.

		if(chBuff[0]!=';'&& strlen(chBuff)>0)				//버퍼의 첫 문자가 ';' 가 아니면, 읽은 문장을 토큰 구분해서 저장한다.
		{
			chToken = strtok( chBuff, chSeps );																							
			m_sMapDefine[nFileIndex][nItemNumber].m_nIndex=atoi(chToken);										
			chToken = strtok( NULL, chSeps );																							
			StrngToUpper(chToken);
			strncpy(m_sMapDefine[nFileIndex][nItemNumber].m_chIdenify,chToken,strlen(chToken));	
			chToken = strtok( NULL, chSeps );																							
			strncpy(m_sMapDefine[nFileIndex][nItemNumber].m_chDefault,chToken,strlen(chToken));
			chToken = strtok( NULL, chSeps );																							
			m_sMapDefine[nFileIndex][nItemNumber].m_nStartPosition=atoi(chToken);							
			chToken = strtok( NULL, chSeps );																						
			m_sMapDefine[nFileIndex][nItemNumber].m_nLength=atoi(chToken);										
			chToken = strtok( NULL, chSeps );																							
			m_sMapDefine[nFileIndex][nItemNumber].m_nAccessor=atoi(chToken);
			chToken = strtok( NULL, chSeps );																							
			strncpy(m_sMapDefine[nFileIndex][nItemNumber].m_chMatchPoint,chToken,strlen(chToken));	
		
			if(nInitType==DEFAULT_INIT)						//기본 값 로드 모드 일경우.
			{
				if(strcmp(m_sMapDefine[nFileIndex][nItemNumber].m_chDefault,chUserInput))	//Default 값이 있는지 확인하고 있을경우, 공유메모리에 로드한다.
				{
					strncpy(SFile_Handler[nFileIndex].PtrInFile+m_sMapDefine[nFileIndex][nItemNumber].m_nStartPosition,
						m_sMapDefine[nFileIndex][nItemNumber].m_chDefault,m_sMapDefine[nFileIndex][nItemNumber].m_nLength);
				}
			}			
		}
		else												//버퍼의 첫 문자가 ';' 이면
			continue;														

		nItemNumber++;										//아이템 넘버 증가
	}

	infile.close();
	return;
}

/*
*	Module Name			:	StrngToUpper
*	Parameter			:	Identify
*	Return				:	NONE
*	Function			:	전달받은 문자열을 해당 대분자로 바굼
*	Create				:	2006.09.18
*	Author				:	고정진
*	Version				:	1.0
*/
void CSM_MapDefineFile::StrngToUpper(char* chRcvID)                                                        //이부분이 불러와서 while 문으로 돌리라는것인지요?
{
	while(*chRcvID != '\0')
	{
		if (*chRcvID >= 'a' && *chRcvID <= 'z')
			*chRcvID-= 32;
		chRcvID++;
	}
}


/*
*	Module Name			:	m_fnReadDefineTable
*	Parameter			:	m_sMapList  상의 File Index, Identify
*	Return				:	m_cMapDefine 의 Index Pointer
*	Function			:	SM_MemoryRead() 함수에 의해 호출 되어진다.
*							SM_MemoryRead() 에서 넘겨받은 인자를 이용하여, m_cMapDefine 를 검색, 해당 Identify 와 일치하는
*							Index 의 포인터를 Return 한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		CSM_MapDefineFile::m_fnReadDefineTable( int nFileIndex,char* chIdentify)
{	
	SSM_MapDefine		sTempDefine;						//Map Define 이 임시 버퍼.

	char		*chTempIdentify=0;
	char		*chRcvIdentify=0;
	int			nItemNumber=0;								//아이템 넘버 인덱스
	int			nIdLength=0;

	

	for(;nItemNumber < ID_NUMBER;nItemNumber++)				//아이템 초대 개수 만큼 반복한다.
	{
		chRcvIdentify=chIdentify;

		if((UCHAR)chIdentify[0]=='[' && (UCHAR)m_sMapDefine[nFileIndex][nItemNumber].m_chIdenify[0]=='[')		//요구자의 ID 와 테이블 아이디가 배열로 시작한다면,
		{
			memcpy(&sTempDefine.m_nIndex,&m_sMapDefine[nFileIndex][nItemNumber].m_nIndex,sizeof(sTempDefine));	//해당 레코드를 임시 버퍼에 복사

			chTempIdentify=*&sTempDefine.m_chIdenify;
			
			//배열의 개수 정의부를 제외한 ID 를 설정한다.
			//요구자의 ID 설정
			nIdLength=(int)strlen(chRcvIdentify);
			for(int i=0;i<nIdLength;i++)
			{
				if((UCHAR)chRcvIdentify[0]!=']')
					chRcvIdentify++;
				else
				{
					chRcvIdentify++;
					break;
				}
			}
			//테이블의 아이디 설정
			nIdLength=(int)strlen(chTempIdentify);
			for(int i=0;i<nIdLength;i++)
			{
				if((UCHAR)chTempIdentify[0]!=']')
					chTempIdentify++;
				else
				{
					chTempIdentify++;
					break;
				}
			}
			/////////////////////////////////////////////			
			
			if(FALSE==strcmp(chRcvIdentify,chTempIdentify))		//두 아이디가 같을 경우 해당 레크드 데이터를 전역 버퍼에 복사하고 작업 성공 코드를 리턴
			{				
				memcpy(&m_sGetMapDefine.m_nIndex,&m_sMapDefine[nFileIndex][nItemNumber].m_nIndex,sizeof(m_sGetMapDefine));	
				return SUCCESS_CODE;	
			}			
		}
		else if(FALSE==strcmp(chIdentify,m_sMapDefine[nFileIndex][nItemNumber].m_chIdenify))		//배열고 시작하지 않는 두 아이디가 같다면,			
		{																															// 해당 레크드 데이터를 전역 버퍼에 복사하고 작업 성공 코드를 리턴
			memcpy(&m_sGetMapDefine.m_nIndex,&m_sMapDefine[nFileIndex][nItemNumber].m_nIndex,sizeof(m_sGetMapDefine));	
			return SUCCESS_CODE;		
		}
		else							//같지 않을 경우.
			continue;
	}
	return ERR_SMP_ID_NOT_DEFINE;		//마지막 까지 비교 하여도 같은 아이디가 없을 겨우, 해당 에러 코드 리턴.
}

/*
*	File Name			:	SmMapList.cpp
*	Function			:	CSM_MapListFile 클래스 함수를 정의해 놓았다.
*							Map List Table 의 생성 및 , 해당 Table 에 접근 함수들이 있다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/

#include "SmMapList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern		SSM_MapList		m_sGetMapList;
extern		SFile_Handle	SFile_Handler[FILE_NUMBER];

CSM_MapListFile::CSM_MapListFile()									//생성자(특이 사항 없음)
{
	return;
}

CSM_MapListFile::~CSM_MapListFile()									//생성자(특이 사항 없음)
{
	return;
}


/*
*	Module Name			:	m_fnReadListFile
*	Parameter			:	경로를 포함한 File Name
*	Return				:	None
*	Function			:	초기 DllMain() 에서 DLL_PROCESS_ATTACH 때에 호출되어,
*							Map List File 을 읽어, m_sMapList 에 적재한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT			CSM_MapListFile::m_fnReadListFile(char* FileName)
{	
	char			chBuff[READ_BUFFER];					//파일을 읽을때 사용하는 임시 버퍼
	char			chSeps[]   = " ,\t\n";					//코큰 구분자
	char			*chToken;								//토큰
	int				nFileNumber=0;							//파일 넘버 인덱스	
	string			sFileName;
	
	//File Path 설정 관련
	char	chDrive[5],chDir[100], chFileName[30], chExten[5];
	char * chPartialPath=0;
	char chFullPath[100];
	_fullpath( chFullPath, chPartialPath, 100 );
	_splitpath( chFullPath,chDrive, chDir, chFileName, chExten );
	//파일 패스 설정 관련. 처리 보류


	ifstream infile (FileName);								//파일을 Open 한다.
	
	if (!infile.is_open())									//파일이 없을때에 대한 처리 - 보류.
		return ERR_SMP_LISTFILE_NOTFIND;

	while (infile.getline(chBuff,READ_BUFFER)){				//한 라인을 읽는다.

		if(chBuff[0]!=';'&& strlen(chBuff)>0)									//버퍼의 첫 문자가 ';' 가 아니면, 읽은 문장을 토큰 구분해서 저장한다.
		{
			chToken = strtok( chBuff, chSeps );
			strncpy(m_sMapList[nFileNumber].m_chFileName,chToken,strlen(chToken));
			chToken = strtok( NULL, chSeps );
			m_sMapList[nFileNumber].m_nFileNumber=atoi(chToken);
			chToken = strtok( NULL, chSeps );
			m_sMapList[nFileNumber].m_nFileSize=atoi(chToken);
			chToken = strtok( NULL, chSeps );			
			if(chToken[0]=='Y')
				m_sMapList[nFileNumber].m_bBackup=TRUE;
			else
				m_sMapList[nFileNumber].m_bBackup=FALSE;

			//파일의 경로를 포함한, 파일 이름을 버퍼에 복사 처리 시작
			sFileName=BACKUP_FILE;																		
			sFileName=sFileName.append(m_sMapList[nFileNumber].m_chFileName);			
			sFileName.copy(chBuff,sFileName.length(),0);
			chBuff[sFileName.length()]='\0';																		
			//파일의 경로를 포함한, 파일 이름을 버퍼에 복사 처리 끝
			m_fnShareMemCreate(nFileNumber,m_sMapList[nFileNumber].m_bBackup,chBuff);//메모리 생성 함수 호출
		}
		else																		//버퍼의 첫 문자가 ';' 일 경우 처리 없음.
			continue;

		nFileNumber++;																//파일 넘버 증가.
	}

	infile.close();

	return	SUCCESS_CODE;
}


/*
*	Module Name			:	m_fnShareMemCreate
*	Parameter			:	File Number, Backup Flag, File path
*	Return				:	None.
*	Function			:	m_fnReadListFile() 함수에 의해 호출 되어진다.
*							m_fnReadListFile() 에서 넘겨받은 인자를 이용하여, 공유 메모리를 생성한다.
*	Create				:	2006.03.06
*	Author				:	고정진
*	Version				:	1.0
*/

void		CSM_MapListFile::m_fnShareMemCreate(int nFileNumber,int bBackFlag, char* FilePath)
{
	HANDLE		hHandle;
	DWORD		dwWritten;

	if(bBackFlag)																											//백업 파일을 생성할 것인가?
	{
		hHandle=CreateFile(FilePath,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,	FILE_ATTRIBUTE_NORMAL,NULL);						//빈 파일을 생성한다.
		WriteFile(hHandle,"",0,&dwWritten,NULL);
		CloseHandle(hHandle);

		SFile_Handler[nFileNumber].hFile = CreateFile(FilePath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,	//해당 파일을 연다.
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);

		SFile_Handler[nFileNumber].hFMap=CreateFileMapping(SFile_Handler[nFileNumber].hFile, NULL, PAGE_READWRITE,			//해당 파일을 파일 매핑한다.
			0, m_sMapList[nFileNumber].m_nFileSize, m_sMapList[nFileNumber].m_chFileName);

		SFile_Handler[nFileNumber].PtrInFile=(char *)MapViewOfFile(SFile_Handler[nFileNumber].hFMap, FILE_MAP_ALL_ACCESS,0,0,//해당 파일의 맵을 생성한다.
			m_sMapList[nFileNumber].m_nFileSize);
	}
	else																													//백업 파일을 생성하지 않을 경우
	{	
		SFile_Handler[nFileNumber].hFMap=CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,0,					//Paging File 을 생성하고, 파일 매핑을 한다.
			m_sMapList[nFileNumber].m_nFileSize, m_sMapList[nFileNumber].m_chFileName);

		SFile_Handler[nFileNumber].PtrInFile=(char *)MapViewOfFile(SFile_Handler[nFileNumber].hFMap, FILE_MAP_ALL_ACCESS,0,0,//해당 파일에 대해 매핑을 실시한다.
			m_sMapList[nFileNumber].m_nFileSize);
	}
}


/*
*	Module Name			:	m_fnReadListTable
*	Parameter			:	File Number
*	Return				:	Result Code.
*	Function			:	SM_MemoryRead() 함수에 의해 호출 되어진다.
*							SM_MemoryRead() 에서 넘겨받은 인자를 이용하여, m_sMapList 을 검색, 해당 File Name 과 일치하는
*							Array Index 를 Return 한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		CSM_MapListFile::m_fnReadListTable(int  nRcvFileNumber)
{
	int			nFileIndex=0;	
	for(;nFileIndex<FILE_NUMBER;nFileIndex++)										//파일의 최대 개수 많큼 반복한다.
	{
		if(nRcvFileNumber==m_sMapList[nFileIndex].m_nFileNumber)					//요구 받은 파일 넘버와 해당 레코드의 파일 넘버가 일치한다면,
		{
			memcpy(m_sGetMapList.m_chFileName,m_sMapList[nFileIndex].m_chFileName,sizeof(m_sGetMapList));	//해당 레코드를 전역 버퍼에 저장하고,
			return	nFileIndex;																																			//해당 파일의 테이블 인덱스를 리턴한다.
		}
		else																		//요구 받은 파일 넘버와 해당 레코드의 파일 넘버가 일치하지 않는다면,
			continue;
	}
	return ERR_SMP_FILE_NUMBER;														//마지막 까지 비교하여도 요구 받은 파일 넘버와 일치하는 테이블 넘버가 없을겨우,
}																					//해당 에러 코드를 리턴한다.

/*
*	Module Name			:	m_fnReadListTable
*	Parameter			:	File Name
*	Return				:	Result Code.
*	Function			:	SM_MemoryRead() 함수에 의해 호출 되어진다.
*							SM_MemoryRead() 에서 넘겨받은 인자를 이용하여, m_sMapList 을 검색, 해당 File Name 과 일치하는
*							Array Index 를 Return 한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		CSM_MapListFile::m_fnReadListTable(char*  chFileName)
{
	string		sRcvString,sTempString;
	CHAR		chTempChar[FILE_NAME];
	int			nFileIndex=0;

	for(;nFileIndex<FILE_NUMBER;nFileIndex++)								//파일의 최대 개수 많큼 반복한다.
	{
		sRcvString=chFileName;
		sTempString=m_sMapList[nFileIndex].m_chFileName;		
		sTempString.copy(chTempChar,sTempString.find('.'),0);
		chTempChar[sTempString.find('.')]='\0';		
		sTempString=chTempChar;
											//요구 받은 파일 넘버와 해당 레코드의 파일 넘버가 일치한다면,
		if(sRcvString==sTempString)	
		{
			memcpy(m_sGetMapList.m_chFileName,m_sMapList[nFileIndex].m_chFileName,sizeof(m_sGetMapList));	//해당 레코드를 전역 버퍼에 저장하고,
			return	nFileIndex;																																			//해당 파일의 테이블 인덱스를 리턴한다.
		}
		else																//요구 받은 파일 넘버와 해당 레코드의 파일 넘버가 일치하지 않는다면,
			continue;
	}
	return ERR_SMP_FILE_NUMBER;												//마지막 까지 비교하여도 요구 받은 파일 넘버와 일치하는 테이블 넘버가 없을겨우,
}														


/*
*	Module Name			:	m_fnReadIndexTable
*	Parameter			:	테이블 인덱스 넘버
*	Return				:	None
*	Function			:	SM_MemoryRead() 함수에 의해 호출 되어진다.
*							SM_MemoryRead() 에서 넘겨받은 인자를 이용하여, m_sMapList 을 검색, 해당 File Name 과 일치하는
*							Array Index 를 Return 한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
void		CSM_MapListFile::m_fnReadIndexTable(int  nRcvIndexNumber)
{	
	memcpy(m_sGetMapList.m_chFileName,m_sMapList[nRcvIndexNumber].m_chFileName,sizeof(m_sGetMapList));	//요구 받은 인덱스 넘버에 해당하는 레코드를 임시 
	return ;																																											//버퍼에 복사한다.
}

/*
*	File Name			:	ShareMemDLL.cpp
*	Function			:	Main 함수와, 초기화 함수, export 함수가 있다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/

//Header File Define
#include <windows.h>
#include <string.h>
#include <atlstr.h>
#include <cstringt.h>
#include "ShareMem.h"
#include "SmMapList.h"
#include "SmMapDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//Function Define
INT			g_fnSM_MemoryRead(int* , char *, char*, int*);
INT			g_fnSM_MemoryRead(char* , char *, char*, int*);
INT			g_fnSM_MemoryRead(int* , int*, int*, char*);

INT			g_fnSM_MemoryWrite(int* , char *, char *, char *);
INT			g_fnSM_MemoryWrite(char* , char *, char *, char *);
INT			g_fnSM_MemoryWrite(int* , int*, int*, char *);

INT			g_fnProcess_Attach_Time(int, int);
void		g_fnProcess_Detach();



//변수 선언

CSM_MapListFile		m_cGetListFile;
CSM_MapDefineFile	m_cGetDefineFile;
SSM_MapList			m_sGetMapList;
SSM_MapDefine		m_sGetMapDefine;
SFile_Handle		SFile_Handler[FILE_NUMBER];	
int					ERROR_CODE;

/*
*	Module Name			:	DllMain
*	Parameter			:	Default
*	Return				:	Default
*	Function			:	Main Function, ATTACH, DETACH 메시지에 대한 이벤트 함수 호출.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpRes)
{
	int nResultCode;
	char szName[MAX_PATH] = {0};

	string	sFileName;

	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:			// Process Loading Time

		GetModuleFileName(GetModuleHandle(NULL),szName, sizeof(szName));	
		sFileName=(string)szName;
		nResultCode=sFileName.find(MONITOR_PROCESS);

		if(nResultCode>=0)
			nResultCode	=	g_fnProcess_Attach_Time(DEFAULT_INIT,ONLY_TABLE);
		else
			nResultCode	=	g_fnProcess_Attach_Time(DEFAULT_INIT,DEFAULT_INIT);		

		break;
	case DLL_PROCESS_DETACH:			//Process Unloading Time

		g_fnProcess_Detach();			
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}



/*
*	Module Name			:	Process_Attach_Time
*	Parameter			:	File Number, Initial Type(0 or 1)
*	Return				:	None
*	Function			:	DLL_PROCESS_ATTACH 에서 호출되며, 환경 파일을 일고, 메모리를 생성한다.
*	Create				:	2006.03.29
*	Author				:	고정진
*	Version				:	2.5
*/
INT		g_fnProcess_Attach_Time(int nFileNumber, int nInitType)
{	
	char		chBuff[READ_BUFFER];										//파일의 경로를 저장하는 임시 버퍼
	string		sFileName;													//SMD, SML 파일이 위치한 경로 저장.
	int			nResultCode;
	int			nTableIndex=0;
	nResultCode = m_cGetListFile.m_fnReadListFile(SML_FILE);				//Map List File Reader 를 호출.

	if(nResultCode>=ERR_SMP_FILE_NUMBER)									//리스트 파일이 존재 하지 않는다면,
		return	nResultCode;												//에러 코드를 리턴한다.	
	if(nFileNumber==DEFAULT_INIT && nInitType >= DEFAULT_INIT)
		for(nTableIndex=0;nTableIndex<FILE_NUMBER;nTableIndex++)			//파일의 총 개수많큼 반복한다.
		{
			sFileName=SRC_FILE_PATH;											//파일의 경로를 복사
			m_cGetListFile.m_fnReadIndexTable(nTableIndex);						//nTableIndex 번째의 데이터를 임시 복사.
			sFileName=sFileName.append(m_sGetMapList.m_chFileName);				//파일의 경로와 파일 네임을 합친다.

			if(strlen(m_sGetMapList.m_chFileName)<=0) 	break;					//더이상 파일이 없을겨우 작업을 중단한다.

			sFileName.copy(chBuff,sFileName.length(),0);								
			chBuff[sFileName.length()]='\0';			

			m_cGetDefineFile.m_fnReadDefineFile(chBuff,nTableIndex,nInitType);	//해당 파일의 Define File 정보를 읽는다.				
		}	
	else if(nFileNumber>DEFAULT_INIT && nInitType >= DEFAULT_INIT)
	{
		sFileName=SRC_FILE_PATH;											//파일의 경로를 복사
		nTableIndex=m_cGetListFile.m_fnReadListTable(nFileNumber);			//nTableIndex 번째의 데이터를 임시 복사.
		sFileName=sFileName.append(m_sGetMapList.m_chFileName);				//파일의 경로와 파일 네임을 합친다.

		//if(strlen(m_sGetMapList.m_chFileName)<=0) 	break;				//더이상 파일이 없을겨우 작업을 중단한다.

		sFileName.copy(chBuff,sFileName.length(),0);								
		chBuff[sFileName.length()]='\0';
		m_cGetDefineFile.m_fnReadDefineFile(chBuff,nTableIndex,nInitType);	//해당 파일의 Define File 정보를 읽는다
	}	
	else
	{
		return	ERR_SMP_NONE_DEFINE;
	}
	return	SUCCESS_CODE;													//Success 코드를 리턴한다.
}


/*
*	Module Name			:	Process_Detach
*	Parameter			:	None
*	Return				:	None
*	Function			:	DLL_PROCESS_DETACH 에서 호출되며, 핸들을 닫는다.
*	Create				:	2006.03.02
*	Author				:	고정진
*	Version				:	1.0
*/
void			g_fnProcess_Detach()										//파일 생성시 사용된 핸들을 닫는다.
{
	for(int i=0;i<FILE_NUMBER;i++)
	{
		if(SFile_Handler[i].hFile)
			CloseHandle(SFile_Handler[i].hFile);
		if(SFile_Handler[i].PtrInFile)
			UnmapViewOfFile(SFile_Handler[i].PtrInFile);
		if(SFile_Handler[i].hFMap)
			CloseHandle(SFile_Handler[i].hFMap);
	}	
}



/*
*	Module Name			:	SharedMemoryRead
*	Parameter			:	File Number, Identify, Return Data 를 저장할 버퍼의 주소,
*	Return				:	해당 메모리 영역의 값, 또는 에러 코드
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryRead() 를 호출한다.
*							SM_MemoryRead() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllexport) int  SharedMemoryRead(int* nFileNumber, char * chIdentify ,char*	chReadReturn, int* nReadLength)
{
	int  chResultCode=0;
	m_cGetDefineFile.StrngToUpper(chIdentify);
	chResultCode=g_fnSM_MemoryRead(nFileNumber,chIdentify,chReadReturn,nReadLength);	//메모리 읽기 함수 호출
	return chResultCode;													//상태 코드를 리턴한다.
}


/*
*	Module Name			:	SharedMemoryRead
*	Parameter			:	File Name, Identify, Return Data 를 저장할 버퍼의 주소,
*	Return				:	해당 메모리 영역의 값, 또는 에러 코드
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryRead() 를 호출한다.
*							SM_MemoryRead() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllexport) int  SharedMemoryRead(char* chFileName, char * chIdentify ,char*	chReadReturn, int* nReadLength)
{
	int  chResultCode=0;
	m_cGetDefineFile.StrngToUpper(chIdentify);
	chResultCode=g_fnSM_MemoryRead(chFileName,chIdentify,chReadReturn,nReadLength);	//메모리 읽기 함수 호출
	return chResultCode;												//상태 코드를 리턴한다.
}


/*
*	Module Name			:	SharedMemoryRead
*	Parameter			:	File Name, Identify, Return Data 를 저장할 버퍼의 주소,
*	Return				:	해당 메모리 영역의 값, 또는 에러 코드
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryRead() 를 호출한다.
*							SM_MemoryRead() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/

extern "C++" __declspec(dllexport) int  SharedMemoryRead(int* nFileNumber, int* nStartPosition, int* nLength, char* chReadReturn)
{
	int  chResultCode=0;
	chResultCode=g_fnSM_MemoryRead(nFileNumber,nStartPosition,nLength,chReadReturn);	//메모리 읽기 함수 호출
	return chResultCode; //상태 코드를 리턴한다.
}



/*
*	Module Name			:	SharedMemoryWrite
*	Parameter			:	FileNumber, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryWrite() 를 호출한다.
*							SM_MemoryWrite() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllexport) int SharedMemoryWrite(int* nFileNumber, char *chIdentify, char *chWriteData, char *chAccessor)
{
	int  chReadData=0;
	m_cGetDefineFile.StrngToUpper(chIdentify);
	chReadData=g_fnSM_MemoryWrite(nFileNumber,chIdentify,chWriteData,chAccessor);		//메모리 쓰기 함수 호출
	return chReadData;																	//처리 상태 코드를 리턴한다.

}


/*
*	Module Name			:	SharedMemoryWrite
*	Parameter			:	FileName, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryWrite() 를 호출한다.
*							SM_MemoryWrite() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllexport) int SharedMemoryWrite(char* chFileName, char *chIdentify, char *chWriteData, char *chAccessor)
{
	int  chReadData=0;
	m_cGetDefineFile.StrngToUpper(chIdentify);
	chReadData=g_fnSM_MemoryWrite(chFileName,chIdentify,chWriteData,chAccessor);		//메모리 쓰기 함수 호출
	return chReadData;																	//처리 상태 코드를 리턴한다.

}


/*
*	Module Name			:	SharedMemoryWrite
*	Parameter			:	FileName, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryWrite() 를 호출한다.
*							SM_MemoryWrite() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllexport) int SharedMemoryWrite(int* nFileNumber, int* nStartPosition, int* nLength, char *chWriteData)
{
	int  chReadData=0;
	chReadData=g_fnSM_MemoryWrite(nFileNumber,nStartPosition,nLength,chWriteData);		//메모리 쓰기 함수 호출
	return chReadData;																	//처리 상태 코드를 리턴한다.

}


/*
*	Module Name			:	SM_ProcessInitial
*	Parameter			:	File Number, Initial Type
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							Runtime 에서 환경 설정을 병경할 수 있다.
*							수행 후 결과 코드를 Return 한다.							
*	Create				:	2006.03.24
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllexport) int SM_ProcessInitial(int* nFileNumber,int* nInitType)
{
	int  nResultCode=0;
	int	nFileNum, nInitNum;
	nFileNum=*nFileNumber;
	nInitNum=*nInitType;
	nResultCode=g_fnProcess_Attach_Time(nFileNum,nInitNum);		//메모리 쓰기 함수 호출
	return nResultCode;																	//처리 상태 코드를 리턴한다.

}



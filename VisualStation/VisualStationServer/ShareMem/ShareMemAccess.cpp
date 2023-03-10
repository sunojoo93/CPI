/*
*	File Name			:	ShareMemAccess.cpp
*	Function			:	공유 메모리에 직접 접근하는 함수들이 있다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/

#include "ShareMemAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module Name			:	SM_MemoryRead
*	Parameter			:	File Number, Identify, Return Data Buffer Address, Read 된 길이를 반환 받을 Pointer
*	Return				:	상태 코드
*	Function			:	SharedMemoryRead() 함수에 의해 호출 되어진다.
*							SharedMemoryRead() 에서 넘겨받은 인자를 이용하여, 실제 Shared Memory 에서 값을 읽어
*							파라미터로 받은 버퍼의 주소에 데이터를 저장하고, 수행 상태 코드를 리턴한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnSM_MemoryRead(int* nFileNumber, char * chIdentify, char* chReadReturn, int* nReadLength)
{	
	int			nTableIndex=0;
	int			nStartPosition=0, nTempStart=0;
	int			nLength=0, nTempLength=0;	
	int			nItemCount=0,nRcvItemCount=0 ;
	int			nCharPosition=0;
	int			nCallStatus=0;
	int			nAccessor=0;
	
	
	try {
		*chReadReturn=NULL;

		nTableIndex=m_cGetListFile.m_fnReadListTable(*nFileNumber);						//해당 파일 넘버에 해당하는 테이블의 Index 를 Return 받는다.(주 : Error Code 가 넘어올수도 있다.)
		ERROR_CODE=nTableIndex;															//넘어온 값을 Error Code 로 설정.

		if(nTableIndex>=ERR_SMP_FILE_NUMBER)											//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
			return ERROR_CODE;

		nCallStatus=m_cGetDefineFile.m_fnReadDefineTable(nTableIndex,chIdentify);		//해당 테이블의 Index, ID 를 이용하여, 해당 레코드의 데이터를 복사 하고 상태 코드를 Return.

		ERROR_CODE=nCallStatus;															//넘어온 값을 Error Code 로 설정.

		if(nCallStatus>=ERR_SMP_FILE_NUMBER)											//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
			return ERROR_CODE;

		g_fnPositionSearch(chIdentify,&nStartPosition,&nLength,&nAccessor);				//ID 를 이용하여 데이터의 Position 구함. Position Parameter 의 주소 전달.
		memcpy(chReadReturn,(SFile_Handler[nTableIndex].PtrInFile+nStartPosition),nLength);	//해당 포지션의 데이터를 Map 파일에서 읽어 Return Buffer 에 저장.
		//strncpy(chReadReturn,(SFile_Handler[nTableIndex].PtrInFile+nStartPosition),nLength);
		chReadReturn[nLength]=NULL;		

		*nReadLength = nLength;															//Read 된 반환 값의 길이를 설정한다.

		return SUCCESS_CODE;															//Success  Code Return.
		
	} catch(...) {
		ofstream LogFile;																// 출력 파일 스트림을 LogFile에 연결
		LogFile.open("C:\\VisualStation\\VS64LOG\\SMPLOG.TXT"); 
		LogFile << " Fuile Number = " << *nFileNumber << "ID = "<< chIdentify << ".\n"; // 파일에 써 넣는다.
		LogFile.close();																// 스트림을 닫음
		return NORMAL_ERROR;
	}															
}


/*
*	Module Name			:	SM_MemoryRead
*	Parameter			:	File Name, Identify, Return Data Buffer Address, Read 된 길이를 반환 받을 Pointer
*	Return				:	상태 코드
*	Function			:	SharedMemoryRead() 함수에 의해 호출 되어진다.
*							SharedMemoryRead() 에서 넘겨받은 인자를 이용하여, 실제 Shared Memory 에서 값을 읽어
*							파라미터로 받은 버퍼의 주소에 데이터를 저장하고, 수행 상태 코드를 리턴한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnSM_MemoryRead(char* chFileName, char * chIdentify ,char* chReadReturn, int* nReadLength)
{	
	int			nTableIndex=0;
	int			nStartPosition=0, nTempStart=0;
	int			nLength=0, nTempLength=0;	
	int			nItemCount=0,nRcvItemCount=0 ;
	int			nCharPosition=0;
	int			nCallStatus=0;
	int			nAccessor=0;
	

	try{
		*chReadReturn=NULL;

		nTableIndex=m_cGetListFile.m_fnReadListTable(chFileName);						//해당 파일 넘버에 해당하는 테이블의 Index 를 Return 받는다.(주 : Error Code 가 넘어올수도 있다.)
		ERROR_CODE=nTableIndex;															//넘어온 값을 Error Code 로 설정.

		if(nTableIndex>=ERR_SMP_FILE_NUMBER)											//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
			return ERROR_CODE;

		nCallStatus=m_cGetDefineFile.m_fnReadDefineTable(nTableIndex,chIdentify);		//해당 테이블의 Index, ID 를 이용하여, 해당 레코드의 데이터를 복사 하고 상태 코드를 Return.

		ERROR_CODE=nCallStatus;															//넘어온 값을 Error Code 로 설정.

		if(nCallStatus>=ERR_SMP_FILE_NUMBER)											//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
			return ERROR_CODE;

		g_fnPositionSearch(chIdentify,&nStartPosition,&nLength,&nAccessor);				//ID 를 이용하여 데이터의 Position 구함. Position Parameter 의 주소 전달.

		//for(int i=0;i<nLength;i++)														//해당 포지션의 데이터를 Map 파일에서 읽어 Return Buffer 에 저장.
		//	chReadReturn[i] = *(SFile_Handler[nTableIndex].PtrInFile+nStartPosition+i);
		memcpy(chReadReturn,(SFile_Handler[nTableIndex].PtrInFile+nStartPosition),nLength);	
		//strncpy(chReadReturn,(SFile_Handler[nTableIndex].PtrInFile+nStartPosition),nLength);
		chReadReturn[nLength]=NULL;		

		*nReadLength = nLength;															//Read 된 반환 값의 길이를 설정한다.

		return SUCCESS_CODE;															//Success  Code Return.
	} catch(...) {
		ofstream LogFile;																// 출력 파일 스트림을 LogFile에 연결
		LogFile.open("C:\\VisualStation\\VS64LOG\\SMPLOG.TXT"); 
		LogFile << " Fuile Number = " << chFileName << "ID = "<< chIdentify << ".\n"; // 파일에 써 넣는다.
		LogFile.close();																// 스트림을 닫음
		return NORMAL_ERROR;
	}	
}


/*
*	Module Name			:	SM_MemoryRead
*	Parameter			:	File Number, Start Position, Length, Return Data Buffer Address.
*	Return				:	상태 코드
*	Function			:	SharedMemoryRead() 함수에 의해 호출 되어진다.
*							SharedMemoryRead() 에서 넘겨받은 인자를 이용하여, 실제 Shared Memory 에서 값을 읽어
*							파라미터로 받은 버퍼의 주소에 데이터를 저장하고, 수행 상태 코드를 리턴한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnSM_MemoryRead(int* nFileNumber, int * nStartPosition ,int* nLength, char* chReadReturn)
{	
	int			nTableIndex=0;	

	*chReadReturn=NULL;
	try{
		nTableIndex=m_cGetListFile.m_fnReadListTable(*nFileNumber);							//해당 파일 넘버에 해당하는 테이블의 Index 를 Return 받는다.(주 : Error Code 가 넘어올수도 있다.)
		ERROR_CODE=nTableIndex;																//넘어온 값을 Error Code 로 설정.

		if(nTableIndex>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
			return ERROR_CODE;
		
		//for(int i=0;i<*nLength;i++)															//해당 포지션의 데이터를 Map 파일에서 읽어 Return Buffer 에 저장.
		//	chReadReturn[i] = *(SFile_Handler[nTableIndex].PtrInFile+*nStartPosition+i);
		
		memcpy(chReadReturn,(SFile_Handler[nTableIndex].PtrInFile+*nStartPosition),*nLength);	
		//strncpy(chReadReturn,(SFile_Handler[nTableIndex].PtrInFile+*nStartPosition),*nLength);
		chReadReturn[*nLength]=NULL;		

		return SUCCESS_CODE;																//Success  Code Return.
	} catch(...) {
		ofstream LogFile;																// 출력 파일 스트림을 LogFile에 연결
		LogFile.open("C:\\VisualStation\\VS64LOG\\SMPLOG.TXT"); 
		LogFile << " Fuile Number = " << *nFileNumber << "Start = "<< *nStartPosition << "Length = "<< *nLength<< ".\n"; // 파일에 써 넣는다.
		LogFile.close();																// 스트림을 닫음
		return NORMAL_ERROR;
	}	
}



/*
*	Module Name			:	SM_MemoryWrite
*	Parameter			:	FileName, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	SharedMemoryWrite() 함수에 의해 호출되어 진다.
*							SharedMemoryWrite() 에서 넘겨받은 인자를 이용하여, 해당 영역에 WriteData, Accessor 를 저장한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnSM_MemoryWrite(int* nFileNumber, char *chIdentify, char *chWriteData, char *chAccessor)
{
	int			nTableIndex=0;	
	int			nTempStart=0,nTempLength=0;	
	int			nItemCount=0,nRcvItemCount=0 ;
	int			nCharPosition=0,nCallStatus=0;		
	int			nStartPosition=0,nLength=0,nAccessor=0;		
	

	nTableIndex=m_cGetListFile.m_fnReadListTable(*nFileNumber);							//해당 파일 넘버에 해당하는 테이블의 Index 를 Return 받는다.(주 : Error Code 가 넘어올수도 있다.)
	ERROR_CODE=nTableIndex;																//넘어온 값을 Error Code 로 설정.

	if(nTableIndex>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
		return ERROR_CODE;

	nCallStatus=m_cGetDefineFile.m_fnReadDefineTable(nTableIndex,chIdentify);			//해당 테이블의 Index, ID 를 이용하여, 해당 레코드의 데이터를 복사 하고 상태 코드를 Return.
	ERROR_CODE=nCallStatus;																//넘어온 값을 Error Code 로 설정.
	if(nCallStatus>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
		return ERROR_CODE;

	g_fnPositionSearch(chIdentify,&nStartPosition,&nLength,&nAccessor);					//ID 를 이용하여 데이터의 Position 구함. Position Parameter 의 주소 전달.

	//if(strlen(chWriteData)>0)															//첫 문자가 NULL 경우도 있으므로, 데이터의 길이는 보지 않는다.
	//{
		memcpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chWriteData,nLength);	
		//strncpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chWriteData,nLength);

		if(strlen(chAccessor)>0)														//접근자 데이터의 길이가 1 이상이라면,
		{			
			nStartPosition=0;
			nStartPosition=m_sGetMapDefine.m_nStartPosition + m_sGetMapDefine.m_nLength;//접근자 데이터의 시작위치는 기록 데이터 시작위치 에 기록 데이터의 길이를 합한 값.
			
			int nAccessorLen=strlen(chAccessor);
			if(nAccessorLen < m_sGetMapDefine.m_nAccessor)
				memcpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chAccessor,nAccessor);	
			//strncpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chAccessor,nAccessor);			
		}
		g_fnMatchPointSearch(chWriteData,chAccessor);
		return SUCCESS_CODE;														//기록 성록 코드를 리턴하고 종료.
		
	//}
	//else																				//기록할 데이터가 없으면,
	//{		
	//	return ERR_SMP_WRITE_DATA;														//해당 에러를 리턴하고, 종료.
	//}	
}


/*
*	Module Name			:	SM_MemoryWrite
*	Parameter			:	File Name, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	SharedMemoryWrite() 함수에 의해 호출되어 진다.
*							SharedMemoryWrite() 에서 넘겨받은 인자를 이용하여, 해당 영역에 WriteData, Accessor 를 저장한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnSM_MemoryWrite(char* chFileName, char *chIdentify, char *chWriteData, char *chAccessor)
{
	int			nTableIndex=0;	
	int			nTempStart=0,nTempLength=0;	
	int			nItemCount=0,nRcvItemCount=0 ;
	int			nCharPosition=0,nCallStatus=0;		
	int			nStartPosition=0,nLength=0,nAccessor=0;	
	


	nTableIndex=m_cGetListFile.m_fnReadListTable(chFileName);							//해당 파일 넘버에 해당하는 테이블의 Index 를 Return 받는다.(주 : Error Code 가 넘어올수도 있다.)
	ERROR_CODE=nTableIndex;																//넘어온 값을 Error Code 로 설정.

	if(nTableIndex>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
		return ERROR_CODE;

	nCallStatus=m_cGetDefineFile.m_fnReadDefineTable(nTableIndex,chIdentify);			//해당 테이블의 Index, ID 를 이용하여, 해당 레코드의 데이터를 복사 하고 상태 코드를 Return.
	ERROR_CODE=nCallStatus;																//넘어온 값을 Error Code 로 설정.
	if(nCallStatus>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
		return ERROR_CODE;

	g_fnPositionSearch(chIdentify,&nStartPosition,&nLength,&nAccessor);					//ID 를 이용하여 데이터의 Position 구함. Position Parameter 의 주소 전달.

	//if(strlen(chWriteData)>0)															//첫 문자가 NULL 인 경우도 있으므로 데이터의 길이는 고려치 않는다.
	//{		
		memcpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chWriteData,nLength);	
		//strncpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chWriteData,nLength);//해당 파일에 데이터를 입력,
		
		if(strlen(chAccessor)>0)														//접근자 데이터의 길이가 1 이상이라면,
		{			
			nStartPosition=0;
			nStartPosition=m_sGetMapDefine.m_nStartPosition + m_sGetMapDefine.m_nLength;//접근자 데이터의 시작위치는 기록 데이터 시작위치 에 기록 데이터의 길이를 합한 값.
			int nAccessorLen=strlen(chAccessor);
			if(nAccessorLen < m_sGetMapDefine.m_nAccessor)
				memcpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chAccessor,nAccessor);
			//strncpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chAccessor,nAccessor);			
		}
		
		ERROR_CODE=g_fnMatchPointSearch(chWriteData,chAccessor);
		return ERROR_CODE;														//기록 성록 코드를 리턴하고 종료.
		
	//}
	//else																				//기록할 데이터가 없으면,
	//{		
	//	return ERR_SMP_WRITE_DATA;														//해당 에러를 리턴하고, 종료.
	//}	
}


/*
*	Module Name			:	SM_MemoryWrite
*	Parameter			:	File Number, Start Position, Length , WriteData,
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	SharedMemoryWrite() 함수에 의해 호출되어 진다.
*							SharedMemoryWrite() 에서 넘겨받은 인자를 이용하여, 해당 영역에 WriteData, Accessor 를 저장한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnSM_MemoryWrite(int* nFileNumber, int * nStartPosition ,int* nLength, char *chWriteData)
{
	int			nTableIndex=0;	

	nTableIndex=m_cGetListFile.m_fnReadListTable(*nFileNumber);							//해당 파일 넘버에 해당하는 테이블의 Index 를 Return 받는다.(주 : Error Code 가 넘어올수도 있다.)
	ERROR_CODE=nTableIndex;																//넘어온 값을 Error Code 로 설정.

	if(nTableIndex>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
		return ERROR_CODE;	

	//if(strlen(chWriteData)>0)															//기록할 데이터를의 길이가 1 이상이라면,
	//{		
		memcpy(SFile_Handler[nTableIndex].PtrInFile+*nStartPosition,chWriteData,*nLength);
		//strncpy(SFile_Handler[nTableIndex].PtrInFile+*nStartPosition,chWriteData,*nLength);//해당 파일에 데이터를 입력,
		
		return SUCCESS_CODE;															//기록 성공 코드를 리턴하고 종료.		
	//}
	//else																				//기록할 데이터가 없으면,
	//{		
	//	return ERR_SMP_WRITE_DATA;														//해당 에러를 리턴하고, 종료.
	//}	
}


/*
*	Module Name			:	g_fnMatchPointSearch
*	Parameter			:	Write Data, Accessor
*	Return				:	None
*	Function			:	Match Point 가 존재하면, 해당 포지션에 데이터를 업데이트 한다.
*	Create				:	2006.03.06
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnMatchPointSearch(char *chWriteData, char *chAccessor)
{
	const		char		chUserInput[]	=	"NONE";	
	const		string		stUserInput		=	"NONE";

	string		sTempString1,sTempString2,sTempString3;
	char		chFileNumber[5];						//토근에서 떼어낸 파일 넘버를 저장.
	char		chSeps[]	= " /";						//코큰 구분자
	char		chSeps2[]   = " ,:";					//코큰 구분자2
	char		*chToken;								//토큰	
	char		chReadRocord[DEFAULT_SIZE];				//Match Point 를 받을 임시 저장소.
	int			nFileNumber;
	char		chIdentify[ID_LENGTH];					//토큰에서 떼어낸 ID 를 저장할 임시 저장소.
	
	strcpy(chReadRocord,m_sGetMapDefine.m_chMatchPoint);				//임시 저장소에 복사한다.	
	chToken = strtok( chReadRocord, chSeps);							//Match Point 가 복수개의 파일인지 확인한다.
	do
	{
		if(strcmp(chToken,chUserInput))
		{
			sTempString1=chToken;			
			sTempString2=sTempString1.substr(0,sTempString1.find(':'));						
			sTempString2.copy(chFileNumber,sTempString2.length(),0);
			chFileNumber[sTempString2.length()]='\0';
			nFileNumber=atoi(chFileNumber);
			
			sTempString2=sTempString1.substr(sTempString1.find(':')+1,sTempString1.length()-sTempString1.find(':'));
			
			if(sTempString2==stUserInput)
			{
				break;
			}
			else
			{
				while(sTempString2.size()>0)
				{
					if(-1==sTempString2.find(','))
					{
						sTempString2.copy(chIdentify,sTempString2.length(),0);
						chIdentify[sTempString2.length()]='\0';
						ERROR_CODE=g_fnSM_MatchPointWrite(nFileNumber,chIdentify,chWriteData,chAccessor);						
						if(ERROR_CODE>=ERR_SMP_FILE_NUMBER)
							return ERROR_CODE;
						break;
					}
					else
					{
						sTempString3.assign(sTempString2);						
						sTempString2.assign(sTempString3,0,sTempString3.find(','));						
						sTempString2.copy(chIdentify,sTempString2.length(),0);
						chIdentify[sTempString2.length()]='\0';
						ERROR_CODE=g_fnSM_MatchPointWrite(nFileNumber,chIdentify,chWriteData,chAccessor);						
						if(ERROR_CODE>=ERR_SMP_FILE_NUMBER)
							return ERROR_CODE;
						sTempString2.assign(sTempString3,sTempString3.find(',')+1,sTempString3.length()-sTempString3.find(','));
						
					}
				}
			}			
		}
	}while(chToken = strtok( NULL, chSeps ));

	return ERROR_CODE;
}




/*
*	Module Name			:	g_fnSM_MatchPointWrite
*	Parameter			:	FileName, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	SharedMemoryWrite() 함수에 의해 호출되어 진다.
*							SharedMemoryWrite() 에서 넘겨받은 인자를 이용하여, 해당 영역에 WriteData, Accessor 를 저장한다.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
INT		g_fnSM_MatchPointWrite(int nFileNumber, char *chIdentify, char *chWriteData, char *chAccessor)
{
	int			nTableIndex=0;	
	int			nTempStart=0,nTempLength=0;	
	int			nItemCount=0,nRcvItemCount=0 ;
	int			nCharPosition=0,nCallStatus=0;		
	int			nStartPosition=0,nLength=0,nAccessor=0;		


	nTableIndex=m_cGetListFile.m_fnReadListTable(nFileNumber);							//해당 파일 넘버에 해당하는 테이블의 Index 를 Return 받는다.(주 : Error Code 가 넘어올수도 있다.)
	ERROR_CODE=nTableIndex;																//넘어온 값을 Error Code 로 설정.

	if(nTableIndex>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
		return ERR_SMP_FILE_NUMBER_MATCH;

	nCallStatus=m_cGetDefineFile.m_fnReadDefineTable(nTableIndex,chIdentify);			//해당 테이블의 Index, ID 를 이용하여, 해당 레코드의 데이터를 복사 하고 상태 코드를 Return.
	ERROR_CODE=nCallStatus;																//넘어온 값을 Error Code 로 설정.
	if(nCallStatus>=ERR_SMP_FILE_NUMBER)												//Return 된 값이 Error Code 영역의 값일 경우 해당 Error Code 를 Return 하고 종료한다.
		return ERR_SMP_ID_NOT_DEFINE_MATCH;

	g_fnPositionSearch(chIdentify,&nStartPosition,&nLength,&nAccessor);					//ID 를 이용하여 데이터의 Position 구함. Position Parameter 의 주소 전달.

	//if(strlen(chWriteData)>0)															//기록할 데이터를의 길이가 1 이상이라면,
	//{
		memcpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chWriteData,nLength);	
		//strncpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chWriteData,nLength);


		int nAccessorLen=strlen(chAccessor);
		if(nAccessorLen >0 && nAccessorLen< m_sGetMapDefine.m_nAccessor)														//접근자 데이터의 길이가 1 이상이라면,
		{			
			nStartPosition=0;
			nStartPosition=m_sGetMapDefine.m_nStartPosition + m_sGetMapDefine.m_nLength;//접근자 데이터의 시작위치는 기록 데이터 시작위치 에 기록 데이터의 길이를 합한 값.

			memcpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chAccessor,nAccessor);	
			//strncpy(SFile_Handler[nTableIndex].PtrInFile+nStartPosition,chAccessor,nAccessor);																	
		}
		return SUCCESS_CODE;	//기록 성록 코드를 리턴하고 종료.
		
	//}
	//else																				//기록할 데이터가 없으면,
	//{		
	//	return ERR_SMP_WRITE_DATA_MATCH;														//해당 에러를 리턴하고, 종료.
	//}	
}



/*
*	Module Name			:	g_fnPositionSearch
*	Parameter			:	ID,	Parameter Address
*	Return				:	None
*	Function			:	g_fnSM_MemoryWrite(), g_fnSM_MemoryRead() 함수에 의해 호출되어 진다.
*							넘겨 받은 ID 를 이용하여 포지션 을 구하고, 넘겨받은 파라미터 주세요 값을 저장한다.
*	Create				:	2006.03.06
*	Author				:	고정진
*	Version				:	1.0
*/
void		g_fnPositionSearch(char *chIdentify, int *nStartPosition, int *nLength, int *nAccessor)
{
	string		sTempString1, sTempString2;
	int			nTableIndex=0;	
	int			nTempStart=0,nTempLength=0;	
	int			nItemCount=0,nRcvItemCount=0 ;
	int			nCharPosition=0,nCallStatus=0;
	char		chTempChar[5];	

	if((UCHAR)chIdentify[0]=='[')													//찾고자 하는 위치의 데이터 ID가 배열의 형태 라면,
	{
		sTempString1=chIdentify;													//[] 안의 문자를 잘라오기 위한 위치 연산
		nCharPosition=sTempString1.find(']')-1;

		if( nCharPosition!=0)														//nCharPosition 의 값이 0 이 아니라면(요구하는 값이 배열 중 특정 요소의 값이라면),
		{
			sTempString2=sTempString1.substr(1,nCharPosition);						//[]안의 문자를 숫자로 변형하기 위한 연산.
			sTempString2.copy(chTempChar,sTempString2.length(),0);
			chTempChar[sTempString2.length()]='\0';
			nRcvItemCount=atoi(chTempChar);											//잘라온 토큰을 숫자로 변환.	

			sTempString1=m_sGetMapDefine.m_chIdenify;								//테이블에서 ID 를 받아온다.
			nCharPosition=sTempString1.find(']')-1;									//테이블에서 받아오 ID 에서 요소의 개수를 잘라온다.
			sTempString2=sTempString1.substr(1,nCharPosition);					
			sTempString2.copy(chTempChar,sTempString2.length(),0);
			chTempChar[sTempString2.length()]='\0';
			nItemCount=atoi(chTempChar);											//요소의 개수를 숫자로 변환.

			nTempStart=m_sGetMapDefine.m_nStartPosition;							//테이블에서 데이터의 시작위치를 받는다.
			nTempLength=m_sGetMapDefine.m_nLength;									//테이블에서 데이터의 길이를 받는다.

			*nLength=nTempLength/nItemCount;										//배열 중, 하나의 요소의 길이를 구한다.(전체의 길이 / 요소의 개수)
			*nStartPosition=nTempStart+((nRcvItemCount-1) * *nLength)	;			//[X] 번째의 시작 위치를 구한다.(배열의 시작위치+((X-1) * 요소의 길이)
			*nAccessor=m_sGetMapDefine.m_nAccessor;									//접근자의 길이.
		}
		else																		//요구하는 값이 배열 전체의 값이라면,
		{
			*nStartPosition=m_sGetMapDefine.m_nStartPosition;						//해당 값을 그대로 내보낸다
			*nLength=m_sGetMapDefine.m_nLength;	
			*nAccessor=m_sGetMapDefine.m_nAccessor;
		}
	}
	else																			//배열의 형태가 아니라면,
	{
		*nStartPosition=m_sGetMapDefine.m_nStartPosition;							//해당 값을 그대로 내보낸다
		*nLength=m_sGetMapDefine.m_nLength;	
		*nAccessor=m_sGetMapDefine.m_nAccessor;
	}
}


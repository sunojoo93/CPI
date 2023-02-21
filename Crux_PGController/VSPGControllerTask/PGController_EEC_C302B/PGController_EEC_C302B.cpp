#include "stdafx.h"
#include "PGController_EEC_C302B.h"
#include "VSPGControllerTask.h"
#include "CMD_List_EEC_C302B.h"
#include "version.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define MAX_BUFF_SIZE					256


CPGController_EEC_C302B::CPGController_EEC_C302B()
	//: CDialog(CPGController_EEC_C302B::IDD)
{
	TCHAR szProgramPath[MAX_PATH];
	GetModuleFileName(NULL, szProgramPath, MAX_PATH);

	char chProgramPath[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, szProgramPath, MAX_PATH, chProgramPath, MAX_PATH, NULL, NULL);

	char title[MAX_PATH] = "title ";
	strcat(title, chProgramPath);
	strcat(title, " ");
	strcat(title, _Version);
	system(title);
}


CPGController_EEC_C302B::~CPGController_EEC_C302B()
{	
}

BOOL CPGController_EEC_C302B::Close()
{
	if(IsConnected())
		ClosePort();

	return TRUE;
}

BOOL CPGController_EEC_C302B::TurnOn()
{
	BOOL bRet;
	//if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_TURN_ON);
	Sleep(150);		// PG 안정화 시간 - 상태체크 구문으로 변경 가능 여부 확인
	return bRet;
}

BOOL CPGController_EEC_C302B::TurnOff()
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_TURN_OFF);
	return bRet;
}

BOOL CPGController_EEC_C302B::NextPattern()
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_PATTERN_NEXT);
	return bRet;
}

BOOL CPGController_EEC_C302B::PrevPattern()
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_PATTERN_PREV);

	return bRet;
}

typedef union
{
	WORD uData[4];
	float fData;
} union_data;

BOOL CPGController_EEC_C302B::SwapVoltage(PARAM_PG* ParamPG)
{
	BOOL bRet;
	BYTE Data[300] = {0, };
	int nDataIndex = 0;
	BYTE ChanelIndex[6] = {0, }; // RGB_CH_INDEX 6개

	TCHAR* szToken = NULL;
	
	Data[nDataIndex++] = ParamPG->pVoltageInfo.nPtnNum;
	// (BYTE)GetPatternIndex(ParamPG->pVoltageInfo.strPatternName);	// Ptn_Index(1)
	//Data[nDataIndex++] = (BYTE)1;	// Ptn_Index(1)
	//Data[nDataIndex++] = (BYTE)ParamPG->nChIndexNum;									// ChNum(1)

	int nIndex = 0;
	szToken = _tcstok(ParamPG->strChIndex,_T(","));
	if (szToken == NULL) return FALSE;//추가
	ChanelIndex[nIndex++] = _ttoi(szToken);
	while(szToken != NULL)
	{
		szToken = _tcstok(NULL,_T(","));
		if(szToken != NULL && _ttoi(szToken) != 0)
		{
			ChanelIndex[nIndex++] = _ttoi(szToken);
		}
	}
	////////// ChanelIndex에 파일의 ChIndex 저장

	Data[nDataIndex++] = nIndex;											// ChNum(1)

	// MTP 검사 파일에서 Channel Index 가 6개 들어올 경우, R,G,B,R,G,B 순서로 고정하여 처리 170927 yss
	for(int i = 0 ; i < nIndex ; i++)
	{
		// '0' : RED, '1' : GREEN, '2' : BLUE
		int nRGBIndex = i % 3;

		switch(nRGBIndex)
		{
		case 0:
			Data[nDataIndex++] = ChanelIndex[i];							
			Data[nDataIndex++] = LOBYTE(ParamPG->pVoltageInfo.wVoltage[0]);	// RED
			Data[nDataIndex++] = HIBYTE(ParamPG->pVoltageInfo.wVoltage[0]);
			break;
		case 1:
			Data[nDataIndex++] = ChanelIndex[i];							
			Data[nDataIndex++] = LOBYTE(ParamPG->pVoltageInfo.wVoltage[1]);	// GREEN
			Data[nDataIndex++] = HIBYTE(ParamPG->pVoltageInfo.wVoltage[1]);
			break;
		case 2:
			Data[nDataIndex++] = ChanelIndex[i];							
			Data[nDataIndex++] = LOBYTE(ParamPG->pVoltageInfo.wVoltage[2]);	// BLUE
			Data[nDataIndex++] = HIBYTE(ParamPG->pVoltageInfo.wVoltage[2]);
			break;
		default:
			break;
		}
	}

	bRet = SendCMD(CMD_VOLTAGE_SWAP,nDataIndex, Data);
	return bRet;
}

BOOL CPGController_EEC_C302B::ChangePattern(int nPatternIndex/*TCHAR strPattern[]*/)	
{
	BOOL bRet;
	//if(!IsConnected())	return FALSE;
	
	int nIndex = nPatternIndex;//GetPatternIndex(strPattern);
	bRet = SendCMD(CMD_PATTERN_INDEX,sizeof(USHORT), (BYTE*)&nIndex);
	return bRet;

}

BOOL CPGController_EEC_C302B::OpenPort(int nPort, DWORD dwBaud, BYTE DataBits, BYTE stopbits, BYTE parity, FlowControl fc)
{
	CString comport = m_Comm.byIndexComPort(nPort);
	BOOL bRet = FALSE;
	CString strLog = _T("");
	const int MAX_RETRY_COUNT = 5;	// 1초 마다 5회 재시도하는 기능 추가
	int nRetryCnt = 0;

	do 
	{	
		bRet = m_Comm.OpenPort( comport, dwBaud,DataBits, stopbits, parity, fc);
		
		if ( bRet )
		{
			m_bSchedulerRun = TRUE; 
			m_pScheduler = AfxBeginThread(thrSchedulerELP, (LPVOID)this);
			m_hScheduler = m_pScheduler->m_hThread;		
			m_Comm.m_bConnected = TRUE;
			strLog.Format(_T("OpenPort - ComPort : %d, BaudRate : %d"),nPort,dwBaud);
			theApp.m_fnWriteLog(strLog);
			TurnOff();		// 시작 시 Turn Off 추가
			break;
		}
		else
		{
			Sleep(1000);
			nRetryCnt++;
			strLog.Format(_T("OpenPort Fail!!! - ComPort : %d, BaudRate : %d (Retry - %d)"),nPort,dwBaud,nRetryCnt);
			theApp.m_fnWriteLog(strLog);
		}
	} while (nRetryCnt > MAX_RETRY_COUNT);

	return bRet;

}

BOOL CPGController_EEC_C302B::ClosePort(void)
{

	CString str = _T("");;

	if (m_hScheduler)
	{
		m_bSchedulerRun = FALSE;

		// thread shut down
		DWORD dwExitCode = 0;

		::GetExitCodeThread(m_hScheduler, &dwExitCode);
		if(dwExitCode != 0)
		{
			if(::WaitForSingleObject(m_hScheduler, 5000) != WAIT_OBJECT_0)
			{
				if(::TerminateThread(m_hScheduler, 0))
				{
					TRACE("HANDLE m_hPGScheduler Terminated. \n");
				}
			}
		}
	}

	m_pScheduler = NULL;

	m_Comm.ClosePort();

	return TRUE;
}

int	CPGController_EEC_C302B::ParsingData(PBYTE pData, const int nLen)
{
	// #ifdef _DEBUG_TEST
	// 	return 0;
	// #endif
	CString str,strtemp;
	str = _T("");
	strtemp = _T("");
	for(int i=0 ; i<16 ; i++)
	{
		strtemp.Format(_T("%02X "),pData[i]);
		str += strtemp;
	}
	str += _T("\0");
	switch ( m_eCurCMD )
	{
	case CMD_TURN_ON:
		if (nLen == 3)
		{
			if (pData[0] == HEAD && pData[1] == ACK_TURN_ON && pData[2] == TAIL)
			{
				m_bRecvAckForCommand =TRUE;
				str.Format(_T("Ack : Turn On OK"));
				//ListOutMsg(str);
			}			
		}	
		break;

	case CMD_TURN_OFF:
		if (nLen == 3)
		{
			if (pData[0] == HEAD && pData[1] == ACK_TURN_OFF && pData[2] == TAIL)
			{
				m_bRecvAckForCommand =TRUE;
				str.Format(_T("Ack : Turn OFF OK"));
				//ListOutMsg(str);
			}			
		}	
		break;
	case CMD_PATTERN_NEXT:
		if (nLen == 8)
		{
			if (pData[0] == HEAD && pData[4] == 0x06 && pData[7] == TAIL)
			{
				m_bRecvAckForCommand =TRUE;
				str.Format(_T("Ack : Pattern Next OK"));
				//ListOutMsg(str);
			}			
		}	
		break;
	case CMD_PATTERN_PREV:
		if (nLen == 8)
		{
			if (pData[0] == HEAD && pData[4] == 0x06 && pData[7] == TAIL)
			{
				m_bRecvAckForCommand =TRUE;
				str.Format(_T("Ack : Pattern Prev OK"));
				//ListOutMsg(str);
			}			
		}	
		break;
	case CMD_PATTERN_INDEX	:
		if (nLen == 8)
		{
			if (pData[0] == HEAD && pData[4] == 0x06 && pData[7] == TAIL)
			{
				m_bRecvAckForCommand =TRUE;
				str.Format(_T("Ack : Pattern Index OK"));
				//ListOutMsg(str);
			}			
		}	
		break;
	case CMD_VOLTAGE_SWAP:
		if (nLen == 8)
		{
			if (pData[0] == HEAD && pData[4] == 0x06 && pData[7] == TAIL)
			{
				m_bRecvAckForCommand =TRUE;
				str.Format(_T("Ack : Swap Voltage OK"));
				//ListOutMsg(str);
			}			
		}	
		break;
	default:
		break;
	}

	return 0;
}

BOOL CPGController_EEC_C302B::SendMsg(PBYTE Msg, int nSize)
{
	// #ifdef _DEBUG_TEST
	// 	return true;
	// #endif

	if(!m_Comm.m_bConnected)	return APP_NG;

	DWORD dwlen ;

	if( nSize <0)
		return FALSE;

	dwlen = nSize;

	int nRet = m_Comm.WriteComm(Msg, dwlen);
	if(nRet > 0) return APP_OK;
	else		 return APP_NG;

}

#if 0
USHORT CPGController_EEC_C302B::m_fnCalcCRC(void* Data)

{
	unsigned short usLoop1 ;
	unsigned short usLoop2 ;
	unsigned short usCrc = 0xFFFF ;
	unsigned char ucCnt = *((unsigned char *)Data)-2;    

	for(usLoop1 = 0 ; usLoop1< ucCnt ; ++usLoop1)
	{
		usCrc ^= *((unsigned char*)Data + usLoop1);
		for(usLoop2 =0; usLoop2< 8 ; ++usLoop2)
		{
			if(0 != (usCrc &0x0001))
			{
				usCrc = (usCrc>>1)^0x8408;
			}
			else
			{
				usCrc = (usCrc >>1);
			}
		}
	}
	return usCrc ;
}
#endif

USHORT CPGController_EEC_C302B::m_fnCalcCRC(void* Data, int nSize)

{
	USHORT crc = 0, SrcCnt = 0;
	for (int SrcCnt = 0; SrcCnt < nSize; SrcCnt++)
	{
		//u16 crc_update(u16 crc, u8 data)
		{     
			USHORT i;     
			BYTE bData = *((BYTE*)Data + SrcCnt);
			crc = crc ^ ((USHORT)bData << 8);     
			for (i=0; i<8; i++)     
			{         
				if(crc & 0x8000) 
					crc = (crc << 1) ^ 0x8005;         
				else 
					crc <<= 1;     
			}    
		} 
	}


	return crc;
}



USHORT CPGController_EEC_C302B::Replace_7D(UINT mode, UINT Length, BYTE* DstBuf, BYTE* SrcBuf)  
{   
	//mode : 0-Send   //       1-Receive   
	UINT DstCnt, SrcCnt;      
	DstCnt = 0;   
	if(mode==0) 
	{  //Send     
		for(SrcCnt=0; SrcCnt<Length; SrcCnt++) 
		{       
			if( SrcBuf[SrcCnt] == 0x7D || SrcBuf[SrcCnt] == 0x7F || SrcBuf[SrcCnt] == 0x7E) 
			{         
				DstBuf[DstCnt] = 0x7D;         
				DstCnt++;         
				DstBuf[DstCnt] = 0x20 ^ SrcBuf[SrcCnt];         
				DstCnt++;       
			}       
			else 
			{         
				DstBuf[DstCnt] = SrcBuf[SrcCnt];         
				DstCnt++;       
			}     
		}   
	}   
	else 
	{  //Receive     
		for(SrcCnt=0; SrcCnt<Length; SrcCnt++) 
		{       
			if( SrcBuf[SrcCnt] == 0x7D ) 
			{         
				SrcCnt++;         
				DstBuf[DstCnt] = 0x20 ^ SrcBuf[SrcCnt];         
				DstCnt++;       
			}       
			else 
			{         
				DstBuf[DstCnt] =  SrcBuf[SrcCnt];
				DstCnt++;       
			}     
		}   
	}   
	return DstCnt; 
} 


BOOL CPGController_EEC_C302B::SendCMD(const int nCmd, USHORT uDataLength, BYTE* pData)
{

	if(nCmd <= CMD_NONE) return APP_NG;
	BYTE cSendData[100] = {0,}, cSendData_Replace7D[200] = {0,};

	int nPt = 0, nChangeCnt = 0;
	m_eOldCMD = m_eCurCMD;
	m_eCurCMD = nCmd;

	BYTE hexval[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

	USHORT crc = 0;
	switch(nCmd)
	{
	case CMD_TURN_ON:
// 		nPt = 0;
// 		cSendData[nPt++] = HEAD; //head
// 		*(USHORT*)&cSendData[nPt] = TURN_ON;
// 		nPt += sizeof(USHORT);
// 		*(USHORT*)&cSendData[nPt] = ID;
// 		nPt += sizeof(USHORT);
// 		*(USHORT*)&cSendData[nPt] = DEFAULT_LENGTH;
// 		nPt += sizeof(USHORT);
// 		*(USHORT*)&cSendData[nPt] = m_fnCalcCRC(cSendData);
// 		nPt += sizeof(USHORT);
// 		cSendData[nPt++] = TAIL; //TAIL
// 		m_bRecvAckForCommand = FALSE;

		nPt = 0;
		//cSendData[nPt++] = HEAD; //head 1byte
		cSendData[nPt++] = LOBYTE(TURN_ON); //CMD 2byte
		cSendData[nPt++] = HIBYTE(TURN_ON);
		cSendData[nPt++] = LOBYTE(ID); //ID 2bygte
		cSendData[nPt++] = HIBYTE(ID);
		cSendData[nPt++] = LOBYTE(DEFAULT_LENGTH); // Data Length 2byte
		cSendData[nPt++] = HIBYTE(DEFAULT_LENGTH);
		crc = m_fnCalcCRC(cSendData, nPt); 
		cSendData[nPt++] = LOBYTE(crc); // CRC 2byte
		cSendData[nPt++] = HIBYTE(crc);

		nChangeCnt = Replace_7D(0, nPt, &cSendData_Replace7D[1], &cSendData[0]);
		cSendData_Replace7D[0] = HEAD;
		cSendData_Replace7D[nChangeCnt+1] = TAIL;
		
		m_bRecvAckForCommand = FALSE;

		break;

	case CMD_TURN_OFF:
		nPt = 0;
		//cSendData[nPt++] = HEAD; //head
		cSendData[nPt++] = LOBYTE(TURN_OFF);
		cSendData[nPt++] = HIBYTE(TURN_OFF);
		cSendData[nPt++] = LOBYTE(ID);
		cSendData[nPt++] = HIBYTE(ID);
		cSendData[nPt++] = LOBYTE(DEFAULT_LENGTH);
		cSendData[nPt++] = HIBYTE(DEFAULT_LENGTH);
		crc = m_fnCalcCRC(cSendData, nPt);
		cSendData[nPt++] = LOBYTE(crc);
		cSendData[nPt++] = HIBYTE(crc);
		//cSendData[nPt++] = TAIL; //TAIL

		nChangeCnt = Replace_7D(0, nPt, &cSendData_Replace7D[1], &cSendData[0]);
		cSendData_Replace7D[0] = HEAD;
		cSendData_Replace7D[nChangeCnt+1] = TAIL;

		m_bRecvAckForCommand = FALSE;

		break;																	
	case CMD_PATTERN_NEXT:														
		nPt = 0;																
		//cSendData[nPt++] = HEAD; //head				  							
		cSendData[nPt++] = LOBYTE(NEXT_PATTERN);	  							
		cSendData[nPt++] = HIBYTE(NEXT_PATTERN);	  							
		cSendData[nPt++] = LOBYTE(ID);				  							
		cSendData[nPt++] = HIBYTE(ID);				  							
		cSendData[nPt++] = LOBYTE(DEFAULT_LENGTH);	  							
		cSendData[nPt++] = HIBYTE(DEFAULT_LENGTH);	  							
		crc = m_fnCalcCRC(cSendData, nPt);		  							
		cSendData[nPt++] = LOBYTE(crc);				  							
		cSendData[nPt++] = HIBYTE(crc);				  							
		//cSendData[nPt++] = TAIL; //TAIL	

		nChangeCnt = Replace_7D(0, nPt, &cSendData_Replace7D[1], &cSendData[0]);
		cSendData_Replace7D[0] = HEAD;
		cSendData_Replace7D[nChangeCnt+1] = TAIL;

		m_bRecvAckForCommand = FALSE;											
		break;																	
	case CMD_PATTERN_PREV:														
		nPt = 0;																
		//cSendData[nPt++] = HEAD; //head
		cSendData[nPt++] = LOBYTE(PREV_PATTERN);
		cSendData[nPt++] = HIBYTE(PREV_PATTERN);
		cSendData[nPt++] = LOBYTE(ID);
		cSendData[nPt++] = HIBYTE(ID);
		cSendData[nPt++] = LOBYTE(DEFAULT_LENGTH);
		cSendData[nPt++] = HIBYTE(DEFAULT_LENGTH);
		crc = m_fnCalcCRC(cSendData, nPt);
		cSendData[nPt++] = LOBYTE(crc);
		cSendData[nPt++] = HIBYTE(crc);
		//cSendData[nPt++] = TAIL; //TAIL

		nChangeCnt = Replace_7D(0, nPt, &cSendData_Replace7D[1], &cSendData[0]);
		cSendData_Replace7D[0] = HEAD;
		cSendData_Replace7D[nChangeCnt+1] = TAIL;

		m_bRecvAckForCommand = FALSE;
		break;
		
	case CMD_PATTERN_INDEX:
		nPt = 0;
		//cSendData[nPt++] = HEAD; //head
		cSendData[nPt++] = LOBYTE(SEL_PATTERN);
		cSendData[nPt++] = HIBYTE(SEL_PATTERN);
		cSendData[nPt++] = LOBYTE(ID);
		cSendData[nPt++] = HIBYTE(ID);
		cSendData[nPt++] = LOBYTE( uDataLength );
		cSendData[nPt++] = HIBYTE( uDataLength );
		for(int i = 0 ; i < uDataLength ; nPt++, i++)
		{
			cSendData[nPt] = pData[i];
		}
		crc = m_fnCalcCRC(cSendData, nPt);
		cSendData[nPt++] = LOBYTE(crc);
		cSendData[nPt++] = HIBYTE(crc);
		//cSendData[nPt++] = TAIL; //TAIL

		nChangeCnt = Replace_7D(0, nPt, &cSendData_Replace7D[1], &cSendData[0]);
		cSendData_Replace7D[0] = HEAD;
		cSendData_Replace7D[nChangeCnt+1] = TAIL;

		m_bRecvAckForCommand = FALSE;
		break;

	case CMD_VOLTAGE_SWAP:
		nPt = 0;
		//cSendData[nPt++] = HEAD; //head
		cSendData[nPt++] = LOBYTE(VOLT_SWEEP);
		cSendData[nPt++] = HIBYTE(VOLT_SWEEP);
		cSendData[nPt++] = LOBYTE(ID);
		cSendData[nPt++] = HIBYTE(ID);
		cSendData[nPt++] = LOBYTE( uDataLength );
		cSendData[nPt++] = HIBYTE( uDataLength );
		for(int i = 0 ; i < uDataLength ; nPt++, i++)
		{
			cSendData[nPt] = pData[i];
		}
		crc = m_fnCalcCRC(cSendData, nPt);
		cSendData[nPt++] = LOBYTE(crc);
		cSendData[nPt++] = HIBYTE(crc);
		//cSendData[nPt++] = TAIL; //TAIL

		nChangeCnt = Replace_7D(0, nPt, &cSendData_Replace7D[1], &cSendData[0]);
		cSendData_Replace7D[0] = HEAD;
		cSendData_Replace7D[nChangeCnt+1] = TAIL;
		/*{
		int k = 0;
		cSendData_Replace7D[k++] = 0x7f;
		cSendData_Replace7D[k++] = 0x71;
		cSendData_Replace7D[k++] = 0x10;
		cSendData_Replace7D[k++] = 0x00;
		cSendData_Replace7D[k++] = 0x00;
		cSendData_Replace7D[k++] = 0x0b;
		cSendData_Replace7D[k++] = 0x00;
		cSendData_Replace7D[k++] = 0x01;
		cSendData_Replace7D[k++] = 0x03;
		cSendData_Replace7D[k++] = 0x40;
		cSendData_Replace7D[k++] = 0x2c;
		cSendData_Replace7D[k++] = 0x01;
		cSendData_Replace7D[k++] = 0x44;
		cSendData_Replace7D[k++] = 0x2c;
		cSendData_Replace7D[k++] = 0x01;
		cSendData_Replace7D[k++] = 0x48;
		cSendData_Replace7D[k++] = 0xdc;
		cSendData_Replace7D[k++] = 0x00;
		cSendData_Replace7D[k++] = 0x7b;
		cSendData_Replace7D[k++] = 0x64;
		cSendData_Replace7D[k++] = 0x7e;
		nChangeCnt = k-2;
		}
		*/

		// 17.08.09 PG 안정화 시간 삭제(VS_ChangePattern 함수에서 안정화 시간 추가)
		//Sleep(60);

		m_bRecvAckForCommand = FALSE;
		break;

	default:
		return FALSE;
	}

	m_bRecvAckForCommand = FALSE;
	CString strLog = _T("SendCmd - Data : ");
	CString strTemp;

	for(int i = 0 ; i < nChangeCnt+2 ; i++)
	{
		strTemp.Format(_T("%02x"),cSendData_Replace7D[i]);
		strLog += strTemp;
	}

	theApp.m_fnWriteLog(strLog);
	BOOL bRtn = SendMsg(cSendData_Replace7D, nChangeCnt+2);

	return bRtn;
}


UINT CPGController_EEC_C302B::thrSchedulerELP(LPVOID pParam)
{
	CPGController_EEC_C302B *pOwner = (CPGController_EEC_C302B *)pParam;
	pOwner->m_bSchedulerRun = TRUE;
	BYTE bData;
	BYTE cBuff[1024];
	int nPt=0;

	while ( pOwner->m_bSchedulerRun )
	{
		if ( pOwner->m_Comm.m_bConnected)
		{
			if( pOwner->m_Comm.m_QueueRead.GetSize() > 0 )
			{
				pOwner->m_Comm.m_QueueRead.GetByte(&bData);
				cBuff[nPt++] = bData;
				if(nPt>=1000) 
					nPt = 0;	
				if ( bData == 0x03)
				{
					pOwner->ParsingData(cBuff, nPt);
					nPt = 0;
				}
			}
			Sleep(20);
		}else{
			break;
		}
	}
	return 0;
}

BYTE CPGController_EEC_C302B::CheckSum(BYTE* p, int size)
{
	BYTE chksum = 0;
	for(int i=0; i<size; i++)
	{
		chksum ^= p[i];
	}

	return chksum;
}

BOOL CPGController_EEC_C302B::GetAcknowledge()
{
	// #ifdef _DEBUG_TEST
	// 	return true;
	// #endif
	if(m_bRecvAckForCommand)	return TRUE;
	else						return FALSE;
}

void CPGController_EEC_C302B::SetPatternInfo(TCHAR* strPattern)
{
	memcpy(m_strPatternInfo, strPattern, 256);
}

int CPGController_EEC_C302B::GetPatternIndex(TCHAR* strPattern)
{
	TCHAR s_section[256] ={0}; //섹션 아래의 모든 값을 저장할 버퍼
	TCHAR s_key[256] = {0x00,}; //한 줄을 읽어 저장할 버퍼(키:값)
	TCHAR s_value[10] = {0};
	int pos = 0; //한 줄의 인덱스 위치

	memcpy(s_section, m_strPatternInfo,256);
	BOOL bSection = false;
	bool bEqul = false;
	int nIndex = 0;

	//i는 모든 섹션의 인덱스 위치이고, pos는 한 줄의 인덱스 위치
	for(int i = 0; i < m_nPatternInfoSize ; i++, pos++)
	{
		if(s_section[i] != '\0') //문자가 있을 경우
		{
			if(s_section[i] != '=' && bEqul == false)
			{
				if(bSection) pos = 0;
				memcpy(s_key + pos, s_section+i, sizeof(s_key+pos)-2); //=이전값인 키를 저장
				bSection = FALSE;
				continue;
			}
			else 
			{
				if(_tcsicmp(s_key, strPattern) == 0 )
				{
					++pos;
					++i;
					memcpy(s_value,s_section+ i, sizeof(s_value));
					return _ttoi( s_value );
				}
				bEqul = true; // 문자가 =인 경우
			}
		}
		else //없을경우 ( 키의 끝 / 섹터의 끝)
		{
			//s_key 초기화
			//s_key[i] = '\,';
			memset(s_key, 0x00, sizeof(s_key));
			bSection = true;
			bEqul = false;
		}
	}
	return 0;
}
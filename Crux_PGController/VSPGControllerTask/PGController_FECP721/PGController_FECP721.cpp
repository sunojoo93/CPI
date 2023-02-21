#include "stdafx.h"
#include "PGController_FECP721.h"
#include "CMD_List_FECP721.h"
#include "VSPGControllerTask.h"
#include "version.h"
//IMPLEMENT_DYNCREATE(CPGController_FECP721, CPGController)

CPGController_FECP721::CPGController_FECP721(int nPortNumber,int nBaudRate)
{
	m_nPortNumber = nPortNumber;
	m_nBaudRate = nBaudRate;

	m_eCurCMD = m_eOldCMD = -1;
	m_hScheduler = NULL;

	m_bRecvAckForCommand = false;
	
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

CPGController_FECP721::CPGController_FECP721()
{
	m_eCurCMD = m_eOldCMD = -1;
	m_hScheduler = NULL;

	m_bRecvAckForCommand = false;

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

CPGController_FECP721::~CPGController_FECP721(void)
{
	if(IsConnected())
		ClosePort();
}


BOOL CPGController_FECP721::Close()
{
	if(IsConnected())
		ClosePort();

	return TRUE;
}

BOOL CPGController_FECP721::TurnOn()
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_TURN_ON);
	return bRet;
}

BOOL CPGController_FECP721::TurnOff()
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_TURN_OFF)?true:false;
	return bRet;
}

BOOL CPGController_FECP721::NextPattern()
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_PATTERN_NEXT);
	return bRet;
}

BOOL CPGController_FECP721::PrevPattern()
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_PATTERN_PREV);

	return bRet;
}

BOOL CPGController_FECP721::ChangeVoltage(int nIndex,const float fRval, const float fGval, const float fBval)
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	bRet = SendCMD(CMD_VOLTAGE_SWAP);

	return bRet;
}

BOOL CPGController_FECP721::ChangePattern(int nPatternIndex/*TCHAR strPattern[]*/)	
{
	BOOL bRet;
	if(!IsConnected())	return FALSE;
	int nIndex = nPatternIndex;//GetPatternIndex(strPattern);
	bRet = SendCMD(CMD_PATTERN_INDEX,nIndex);

	return bRet;
}

int CPGController_FECP721::GetPatternIndex(TCHAR* strPattern)
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

BOOL CPGController_FECP721::OpenPort(int nPort, DWORD dwBaud, BYTE DataBits, BYTE stopbits, BYTE parity, FlowControl fc)
{

	// 	char hexval[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	// 	char *temp = new char;
	CString str,strtemp;
	CString strLog;
	str = _T("");
	CString comport = m_Comm.byIndexComPort(nPort);

	BOOL ret = m_Comm.OpenPort( comport, dwBaud,DataBits, stopbits, parity, fc);

	if ( ret )
	{
		//str.Format(_T("%s Port Open Complate !!!!",comport));
		//ListOutMsg(str);
		m_bSchedulerRun = true;
		m_pScheduler = AfxBeginThread(thrSchedulerELP, (LPVOID)this);
		m_hScheduler = m_pScheduler->m_hThread;
		m_Comm.m_bConnected = true;
		strLog.Format(_T("OpenPort - ComPort : %d, BaudRate : %d"),nPort,dwBaud);
		theApp.m_fnWriteLog(strLog);
	}
	else
	{
		strLog.Format(_T("OpenPort Fail!!! - ComPort : %d, BaudRate : %d"),nPort,dwBaud);
		theApp.m_fnWriteLog(strLog);
	}

	return ret;
}

BOOL CPGController_FECP721::ClosePort(void)
{
	//#ifdef _DEBUG_TEST
	//	return true;
	//#endif
	CString str = _T("");;

	if (m_hScheduler)
	{
		m_bSchedulerRun = FALSE;

		// thread shut down
		DWORD dwExitCode = 0;
		str.Format(_T("%n Port Close Complate !!!!"), m_nPortNumber+1);
		//ListOutMsg(str);
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

int	CPGController_FECP721::ParsingData(PBYTE pData, const int nLen)
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

	//ListOutMsg(str);
	//if( m_Comm.m_bConnected)
	//{
	//str.Format(_T("%s Port Already opened !!!!"),m_nPortNumber);
	//ListOutMsg(str);
	//	return FALSE;
	//}
	str += _T("\0");
	switch ( m_eCurCMD )
	{
	case CMD_TURN_ON:
		if (nLen == 8)
		{
			if (pData[0] == 0x02 && pData[4] == 0x06 && pData[7] == 0x03)
			{
				m_bRecvAckForCommand =TRUE;
				str.Format(_T("Ack : Turn On OK"));
				//ListOutMsg(str);
			}			
		}	
		break;

	case CMD_TURN_OFF:
		if (nLen == 8)
		{
			if (pData[0] == 0x02 && pData[4] == 0x06 && pData[7] == 0x03)
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
			if (pData[0] == 0x02 && pData[4] == 0x06 && pData[7] == 0x03)
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
			if (pData[0] == 0x02 && pData[4] == 0x06 && pData[7] == 0x03)
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
			if (pData[0] == 0x02 && pData[4] == 0x06 && pData[7] == 0x03)
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
			if (pData[0] == 0x02 && pData[4] == 0x06 && pData[7] == 0x03)
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

BOOL CPGController_FECP721::SendMsg(PBYTE Msg, int nSize)
{
	// #ifdef _DEBUG_TEST
	// 	return true;
	// #endif

	if(!m_Comm.m_bConnected)	return FALSE;

	DWORD dwlen ;

	if( nSize <0)
		return FALSE;

	dwlen = nSize;

	m_Comm.WriteComm(Msg, dwlen);
	return TRUE;
}

BOOL CPGController_FECP721::SendCMD(const int nCmd, const int nIndex, const float fRval, const float fGval, const float fBval)
{
	// #ifdef _DEBUG_TEST
	// 	return true;
	// #endif
	if(nCmd <= CMD_NONE) return FALSE;
	BYTE cSendData[32];

	int nPt;

	m_eOldCMD = m_eCurCMD;
	m_eCurCMD = nCmd;

	//	Sleep(50); //Light Delay 를 줌으로 주지 않는다.

	switch(nCmd)
	{
	case CMD_TURN_ON:
		nPt = 0;
		cSendData[nPt++] = STX_ASCII;
		cSendData[nPt++] = STATUS; // Status
		cSendData[nPt++] = TYPE;// Type
		cSendData[nPt++] = DEFAULT_LENGTH;// Length
		cSendData[nPt++] = TURN_ON;
		cSendData[nPt++] = DEFAULT_DATA;
		cSendData[nPt++] = CheckSum(&cSendData[1], 5);
		cSendData[nPt++] = ETX_ASCII;

		m_bRecvAckForCommand = FALSE;

		break;

	case CMD_TURN_OFF:
		nPt = 0;
		cSendData[nPt++] = STX_ASCII;
		cSendData[nPt++] = STATUS;
		cSendData[nPt++] = TYPE;
		cSendData[nPt++] = DEFAULT_LENGTH;
		cSendData[nPt++] = TURN_OFF;
		cSendData[nPt++] = DEFAULT_DATA;
		cSendData[nPt++] = CheckSum(&cSendData[1], 5);
		cSendData[nPt++] = ETX_ASCII;

		m_bRecvAckForCommand = FALSE;

		break;
	case CMD_PATTERN_NEXT:
		nPt = 0;
		cSendData[nPt++] = STX_ASCII;
		cSendData[nPt++] = STATUS;// Status
		cSendData[nPt++] = TYPE;// Type
		cSendData[nPt++] = DEFAULT_LENGTH;// Length
		cSendData[nPt++] = NEXT_PATTERN;
		cSendData[nPt++] = DEFAULT_DATA;
		cSendData[nPt++] = CheckSum(&cSendData[1], 5);
		cSendData[nPt++] = ETX_ASCII;

		m_bRecvAckForCommand = FALSE;
		break;
	case CMD_PATTERN_PREV:
		nPt = 0;
		cSendData[nPt++] = STX_ASCII;
		cSendData[nPt++] = STATUS;// Status
		cSendData[nPt++] = TYPE;// Type
		cSendData[nPt++] = DEFAULT_LENGTH;// Length
		cSendData[nPt++] = PREV_PATTERN;
		cSendData[nPt++] = DEFAULT_DATA;
		cSendData[nPt++] = CheckSum(&cSendData[1], 5);
		cSendData[nPt++] = ETX_ASCII;

		m_bRecvAckForCommand = FALSE;
		break;
	case CMD_PATTERN_INDEX:
		nPt = 0;
		cSendData[nPt++] = STX_ASCII;
		cSendData[nPt++] = STATUS;// Status
		cSendData[nPt++] = TYPE;// Type
		cSendData[nPt++] = DEFAULT_LENGTH;// Length
		cSendData[nPt++] = SEL_PATTERN;
		cSendData[nPt++] = nIndex;
		cSendData[nPt++] = CheckSum(&cSendData[1], 5);
		cSendData[nPt++] = ETX_ASCII;

		m_bRecvAckForCommand = FALSE;
		break;

	case CMD_VOLTAGE_SWAP:
		nPt = 0;
		cSendData[nPt++] = STX_ASCII;
		cSendData[nPt++] = 0x00;// Status
		cSendData[nPt++] = 0x80;// Type
		cSendData[nPt++] = 0x08;// Length
		cSendData[nPt++] = 0xA5;
		cSendData[nPt++] = nIndex;

		cSendData[nPt++] = HIBYTE(WORD(fRval*1000));
		cSendData[nPt++] = LOBYTE(WORD(fRval*1000));
		cSendData[nPt++] = HIBYTE(WORD(fGval*1000));
		cSendData[nPt++] = LOBYTE(WORD(fGval*1000));
		cSendData[nPt++] = HIBYTE(WORD(fBval*1000));
		cSendData[nPt++] = LOBYTE(WORD(fBval*1000));

		cSendData[nPt++] = CheckSum(&cSendData[1], 11);
		cSendData[nPt++] = ETX_ASCII;

		m_bRecvAckForCommand = FALSE;
		break;
	default:
		return FALSE;
	}

	m_bRecvAckForCommand = FALSE;
	CString outmsg = _T("");
	CString strLog = _T("SendCmd - Data : ");
	CString strTemp;

	for(int i = 0 ; i < nPt ; i++)
	{
		strTemp.Format(_T("%02x"),cSendData[i]);
		strLog += strTemp;
	}

	theApp.m_fnWriteLog(strLog);
	BOOL bRtn = SendMsg(cSendData, nPt);

	return bRtn;
}


UINT CPGController_FECP721::thrSchedulerELP(LPVOID pParam)
{
	CPGController_FECP721 *pOwner = (CPGController_FECP721 *)pParam;
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

BYTE CPGController_FECP721::CheckSum(BYTE* p, int size)
{
	BYTE chksum = 0;
	for(int i=0; i<size; i++)
	{
		chksum ^= p[i];
	}

	return chksum;
}

BOOL CPGController_FECP721::GetAcknowledge()
{
	// #ifdef _DEBUG_TEST
	// 	return true;
	// #endif
	if(m_bRecvAckForCommand)	return TRUE;
	else						return FALSE;
}
void CPGController_FECP721::SetPatternInfo(TCHAR* strPattern)
{
	memcpy(m_strPatternInfo, strPattern, 256);
}
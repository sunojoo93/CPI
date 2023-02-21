/** 
@addtogroup	RS232_LIB
@{
*/ 

/**
@file		RS232Lib.h
@brief		RS-232C Class library (CRS232Lib)에 대한 정의

@remark		
@author		김용태
@date		2007.05.08
*/

#pragma once
#pragma warning( disable : 4275 )
#pragma warning( disable : 4251 )

//#include <string.h>
#include "MessageQueue.h"
#include "MemPooler.h"
#include "Thread.h"
#include "SafeDeque.h"
#include <string>

using namespace std;

#ifdef RS232LIB_EXPORTS
	#define RS232LIB_API __declspec(dllexport)
#else
	#define RS232LIB_API __declspec(dllimport)
#endif

const int	MAX_COM_MSG_LEN = 500;			///< 1개의 Packet 이 가질수 있는 최대 크기

/** 
@brief		1개의 Packet 을 가지는 메시지 구조체

@remark
-			본 구조체는 Port 에서 Message 수신시 Receive Mesage Queue 의 리스트로 보관 된다.
-			Application 에서는 Receive Mesage Queue 에 접근해서 본 구조체 메시지를 꺼내서 하나씩 처리한다.
@author		김용태
@date		2007.05.09
*/ 
struct PER_PORT_RCV_PACKET
{
	char	szRcvMsg[MAX_COM_MSG_LEN];

	PER_PORT_RCV_PACKET()					///< 생성자로서 구조체를 초기화 한다.
	{
		memset(this, 0x00, sizeof(MAX_COM_MSG_LEN));
	}
};

/** 
@brief		구분자 정보 리스트를 구성하는 구조체

@remark		CRS232Lib Class 초기화시 인자로 받아서 Class 내부에 가지고 있는다.
@author		김용태
@date		2007.05.09
*/ 
struct PER_MSG_SEPARATE_INFO
{
	bool	bSepPosIsEnd;					///< 패킷 구분자의 위치가 패킷의 끝을 나타내는지 여부, true:끝, false:시작
	string	stSeparate;						///< 패킷 구분을 위한 문자열, 구분자.
	bool	bIsPacket;						///< 구분자도 Packet 으로 보는지의 여부

	PER_MSG_SEPARATE_INFO()					///< 생성자
	{
		stSeparate = "";
		bIsPacket = false;
		bSepPosIsEnd = true;
	}

	PER_MSG_SEPARATE_INFO(
		deque<string>& deqToken				///< 구조체 초기 값이 들어 있는 Deque
		)									///< 생성자
	{
		string			strWord;

		if(deqToken.size() != 3) 
		{
			m_fnClear();
			return;
		}

		deque<string>::iterator iterToken;

		iterToken = deqToken.begin();

		strWord			= *iterToken++;
		strWord			= strWord.substr(strWord.find_first_not_of(' '));
		if(strWord == "E")
			bSepPosIsEnd = true;
		else
			bSepPosIsEnd = false;

		strWord		= *iterToken++;
		strWord		= strWord.substr(strWord.find_first_not_of(' '));
		stSeparate	= strWord.substr(strWord.find_first_of('"') + 1, strWord.find_last_of('"') - 1); 
		transform (stSeparate.begin(), stSeparate.end(), stSeparate.begin(), toupper);					// 대문자 전환
		if(stSeparate == "\\R\\N")
			stSeparate = "\r\n";

		else if(stSeparate == "\\N")
			stSeparate = "\n";

		else if(stSeparate == "\\R")
			stSeparate = "\r";
		else if(stSeparate == "\\N\\R")
			stSeparate = "\n\r";


		strWord		= *iterToken;
		strWord		= strWord.substr(strWord.find_first_not_of(' '));
		if(strWord == "0")
			bIsPacket = false;
		else
			bIsPacket = true;
	}

	void m_fnClear()
	{
		stSeparate = "";
		bIsPacket = false;
		bSepPosIsEnd = true;
	}
};

/** 
@brief		RS-232C Class library

@remark		
-			Com Port 에 연결된 장치와 PC간의 RS-232C 통신 규약을 만족하는 Interface 를 지원한다.
-			본 Class 의 기능을 이용하기 위해 Class 상속을 받아서 사용해야 한다.
-			DLL 내의 Run time log 를 기록하기 위해서는 m_fnWriteLog 함수를 재정의해서 사용해야 한다.
@author		김용태
@date		2007.05.08
*/
class RS232LIB_API CRS232Lib : private CThread, private cslock
{
public:
	CRS232Lib( 
		int	port, 
		int	baudrate, 
		int	parity, 
		int	databit, 
		int	stopbit, 
		int	FlowChk,
		CSafeDeque<PER_MSG_SEPARATE_INFO>&	m_DeqMsgSep
		);									///< 생성자
	~CRS232Lib(void);						///< 소멸자

	bool		m_fnOpenPort();				///< Port를 Open 한다.
	void		m_fnClosePort();			///< Port를 Close 한다.
	bool		m_fnSendPortMsg(
		const char*	outbuf,
		const int	len
		);									///< Open된 Port로 메시지를 전송한다.

	PER_PORT_RCV_PACKET*
		m_fnPeekPortMessage();				///< 메시지 큐에 있는 Port Receive Message 가져온다.

	bool		m_fnFreePortMsg(
		PER_PORT_RCV_PACKET* pRcvPacket
		);									///< Peek Port Message 로 가져온 Recevie Packet 정보 구조체를 library 로 반환한다.

protected:
	/** 
	@brief		Run Time Log 기록 기능을 지원한다. UniCode 버젼

	@return		없음
	@remark		
	-			Log 기록 기능을 사용하기 위해 상속 받는 Class 내에서 본 함수를 재정의 해주어야 한다.
	-			Ansi Code로 Compile 할 경우 사용되는 Log 기록 함수
	@author		김용태
	@date		2007.05.09
	*/
	virtual	void m_fnWriteLog( 
		int nLogLevel,						///< Log Level
		const WCHAR* pstzLogData,			///< Log 내용
		...									///< 가변인자
		) = 0;

	/** 
	@brief		Run Time Log 기록 기능을 지원한다. Ansi Code 버젼

	@return		없음
	@remark		
	-			Log 기록 기능을 사용하기 위해 상속 받는 Class 내에서 본 함수를 재정의 해주어야 한다.
	-			Ansi Code로 Compile 할 경우 사용되는 Log 기록 함수
	@author		김용태
	@date		2007.05.09
	*/
	virtual	void m_fnWriteLog( 
		int nLogLevel,						///< Log Level
		const CHAR* pstzLogData,			///< Log 내용
		...									///< 가변인자
		) = 0;
private:
	void		m_fnRunThread();			///< Receive Thread를 수행시킨다.
	void		m_fnReceive();				///< Thread의 실질적인 기능을 한다.
	void		m_fnReceivePortData(
		COMSTAT&			OutComStat,
		char*				cOutBuf,
		int&				nOutBufOffset
		);									///< Port에 RCV 된 Data 를 전부 가져온다.
	void		m_fnSeekSepStartPos(
		char*&	pOutSepStartPos,
		PER_MSG_SEPARATE_INFO& outMsgSepInfo, 
		const char* const inBuffer,
		const int nOffset
		);									///< Receive 된 Buffer 내에서 Packet 구분자들중 제일 처음에 나오는 위치와 구분자 정보를 찾는다.
	int			m_fnPutMsgQuFromRcvData(
		char* const	inbuf,
		const int			nReceiveSize
		);									///< Receive 된 정보를 명령어 구분자로 나누어 Message Queue 에 넣는다.

	BOOL		m_fnResetSerial();			///< Port 연결을 초기화 한다.
	bool		m_fnSendSeparater(
		bool bSTXorETX);					///< Packet 구분자를 전송한다. 메시지 Send 후 마지막에 자동으로 호출되어진다.

private:
	int			m_nPort;					///< 연결 Port 번호
	int			m_nBaudrate;				///< baudrate
	int			m_nParity;					///< parity
	int			m_nDatabit;					///< databit
	int			m_nStopbit;					///< stopbit
	int			m_nFlowChk;					///< Flow Check 유무
	CSafeDeque<PER_MSG_SEPARATE_INFO>&	
		m_DeqMsgSep;						///< Send/Recevie 되는 메시지를 Packet 단위로 분리하는 기준이 되는 문자(열), 항상 ANSI 형 Char 이어야 한다.

	bool		m_bIsOpenned;				///< Port Open 되었는지 여부, Receive Thread 종료 판단에 사용
	HANDLE		m_hComDev;					///< Com Port의 Handle

	OVERLAPPED	m_OLR,m_OLW;				///< 중첩 IO 를 위한 구조체
	HANDLE		m_hRCVThread;				///< 생성된 Receive Thread 의 Handle 을 가진다.

	CMemPooler<PER_PORT_RCV_PACKET>* 
		m_pMemoryPool;						///< Application 으로 메시지를 전달시, 메시지 큐에 입력되는 구조체를 가지는 메모리 풀

	CMessageQueue<PER_PORT_RCV_PACKET*>* 
		m_pRcvMessageQ;						///< Application 으로 전달하는 Receive 한 패킷을 전달하는 메시지 큐
};

/** 
@}
*/ 
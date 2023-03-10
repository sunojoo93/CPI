#pragma once

/*
*	Module name	:	CLogWriter
*	Function	:	로그 기록 기능을 제공한다.
*	Create		:	2006.02.21
*	Author		:	김용태
*	Version		:	1.1
*	-Version 1.1 (김용태), 2006.11.10
*		- Logwriter 동작 Flag 추가
*/
class CLogWriter
{
public:
	CLogWriter(char* pszLogFilePath, char* pszLogFileName, UINT uPeriodDay, UINT uWriteTerm, bool bEnable = true);
	~CLogWriter(void);

	/*멤버 함수*/
public:
	int		m_fnWriteLog(const char* pszLog);			//Log를 기록한다.Log를 기록한다.

private:
	int		m_fnConnectFileHandle();					//현재 시간을 기준으로 Log 파일을 생성하고 파일 핸들을 연결한다.
	int		m_fnMakePath();								//파일의 경로를 만든다.
	int		m_fnDeletePeriodOverFile();					//보관 기간이 지난 파일들을 삭제한다.

	/*멤버 변수*/
private:
	char	m_szLogFilePath[MAX_LOG_FILE_PATH];			//Log 파일 경로를 가진다.
	char	m_szLogFileName[MAX_LOG_FILE_NAME];			//Log 기본 파일 이름을 가진다. 이름 뒤에 시간이 붙는다.
	UINT	m_uPeriodDay;								//Log 파일 보관 일
	UINT	m_uWriteTerm;								//Log 파일 기록 시간, 몇 시간 단위로 파일을 나눌 것인가를 결정한다. (최대 24시간이다.)
	char	m_szLastWriteFileName[MAX_LOG_FILE_NAME];	//마지막으로 기록 성공한 File name (시간대별 기록 및 Log Period 처리시 사용한다.)
	FILE*	m_hLastFileHandle;							//마지막으로 기록 성공한 File 의 Handle 을 가진다.
	CRITICAL_SECTION	m_csLogWriter;					//Log 기록시 필요한 Critical Section
	ULONG	m_ulFileNumber;								//마지막으로 기록 성공한 File 의 이름 중 시간대만 숫자형으로 가지고 있는다. 빠른 파일 이름 비교위함
	bool	m_bEnable;
};


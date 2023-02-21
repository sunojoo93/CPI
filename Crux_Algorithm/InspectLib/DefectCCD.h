
/************************************************************************/
// CCD Defect 관련 헤더
// 수정일 : 17.07.10 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "MatBuf.h"				// 메모리 관리
#include "InspectLibLog.h"

// CCD 불량 구조체
struct tCCD_DEFECT
{
	int x;		// x 좌표
	int y;		// y 좌표
	int gv;		// 해당 GV값

	// 구조체 초기화
	tCCD_DEFECT()
	{
		x	= 0;
		y	= 0;
		gv	= 0;
	}
};

class CDefectCCD
{
public:
	CDefectCCD(void);
	virtual ~CDefectCCD(void);

	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	//////////////////////////////////////////////////////////////////////////
	InspectLibLog		cInspectLibLog;
	InspectLibLog*		m_cInspectLibLog;
	clock_t				m_tInitTime;
	clock_t				m_tBeforeTime;
	wchar_t*			m_strAlgLog;

	void		SetLog(InspectLibLog* cLog, clock_t tTimeI, clock_t tTimeB, wchar_t* strLog)
	{
		m_tInitTime = tTimeI;
		m_tBeforeTime = tTimeB;
		m_cInspectLibLog = cLog;
		m_strAlgLog = strLog;
	};

	void		writeInspectLog(int nAlgType, char* strFunc, wchar_t* strTxt)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, m_strAlgLog);
	};

	void		writeInspectLog_Memory(int nAlgType, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, nMemory_Use_Value, m_strAlgLog);
	};

	CString		GETDRV()
	{
		return m_cInspectLibLog->GETDRV();
	}
	//////////////////////////////////////////////////////////////////////////
	// CCD Defect Load
	long	DefectCCDLoad(CString strFileName, CString strFileName2);

	// CCD Defect Save
	long	DefectCCDSave(cv::Mat& matSrcBuffer, CString strFileName, CString strFileName2);

	// CCD 불량 삭제 갯수 가져오기
	int		GetDefectCCDDeleteCount();

	// CCD 불량 보정 갯수 가져오기
	int		GetDefectCCDOffsetCount();

	// CCD 불량 삭제
	long	DeleteDefectCCD(cv::Mat& matSrcBuffer, int nSize=0, int nPS=1);

	// CCD 불량 자동 삭제
	long	DeleteAutoDefectCCD(cv::Mat& matSrcBuffer, float fGV, float fBkGV, int nPS=1, CMatBuf* cMem=NULL);

	// CCD 불량 보정
	long	OffsetDefectCCD(cv::Mat& matSrcBuffer, int nSize=0, int nPS=1);

protected:
	// CCD Defect Delete ( 삭제용 )
	vector <tCCD_DEFECT>	ptIndexsDelete;

	// CCD Defect Offset ( 보정용 )
	vector <tCCD_DEFECT>	ptIndexsOffset;

	// Load 됐는지 확인 용
	bool	bLoad;
};
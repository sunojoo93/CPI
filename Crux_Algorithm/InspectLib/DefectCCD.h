
/************************************************************************/
// CCD Defect ���� ���
// ������ : 17.07.10 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "MatBuf.h"				// �޸� ����
#include "InspectLibLog.h"

// CCD �ҷ� ����ü
struct tCCD_DEFECT
{
	int x;		// x ��ǥ
	int y;		// y ��ǥ
	int gv;		// �ش� GV��

	// ����ü �ʱ�ȭ
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

	// �޸� ����
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

	// CCD �ҷ� ���� ���� ��������
	int		GetDefectCCDDeleteCount();

	// CCD �ҷ� ���� ���� ��������
	int		GetDefectCCDOffsetCount();

	// CCD �ҷ� ����
	long	DeleteDefectCCD(cv::Mat& matSrcBuffer, int nSize=0, int nPS=1);

	// CCD �ҷ� �ڵ� ����
	long	DeleteAutoDefectCCD(cv::Mat& matSrcBuffer, float fGV, float fBkGV, int nPS=1, CMatBuf* cMem=NULL);

	// CCD �ҷ� ����
	long	OffsetDefectCCD(cv::Mat& matSrcBuffer, int nSize=0, int nPS=1);

protected:
	// CCD Defect Delete ( ������ )
	vector <tCCD_DEFECT>	ptIndexsDelete;

	// CCD Defect Offset ( ������ )
	vector <tCCD_DEFECT>	ptIndexsOffset;

	// Load �ƴ��� Ȯ�� ��
	bool	bLoad;
};
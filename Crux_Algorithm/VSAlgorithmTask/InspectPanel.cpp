// InspCam.cpp : implementation file
//

#include "stdafx.h"
#include "InspectPanel.h"
#include "AviInspection.h"
#include "SviInspection.h"
#include "AppInspection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspPanel

CInspPanel::CInspPanel()
{
	for(int nThrdCnt=0; nThrdCnt<MAX_THREAD_COUNT; nThrdCnt++)
		m_pInspThrd[nThrdCnt] = NULL;
}

CInspPanel::~CInspPanel()
{
}

// BEGIN_MESSAGE_MAP(CInspPanel, CObject)
// 	//{{AFX_MSG_MAP(CInspPanel)
// 	//}}AFX_MSG_MAP
// END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspPanel message handlers
bool CInspPanel::InitVision()
{
	m_nCurThrdID = 0;
	theApp.m_nDefectCnt = 0;

	for(int nThrdCnt=0; nThrdCnt<MAX_THREAD_COUNT; nThrdCnt++)
	{
		if(!m_pInspThrd[nThrdCnt])
		{
			if (theApp.m_Config.GetEqpType() == EQP_AVI || theApp.m_Config.GetEqpType() == EQP_LCP)
			{
				m_pInspThrd[nThrdCnt] = (CInspThrd*)
					AfxBeginThread(RUNTIME_CLASS(AviInspection), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			}
			else if (theApp.m_Config.GetEqpType() == EQP_SVI)
			{
				m_pInspThrd[nThrdCnt] = (CInspThrd*)
					AfxBeginThread(RUNTIME_CLASS(SviInspection), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);				
			}
			else if (theApp.m_Config.GetEqpType() == EQP_APP)
			{
				m_pInspThrd[nThrdCnt] = (CInspThrd*)
					AfxBeginThread(RUNTIME_CLASS(AppInspection), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			}
			m_pInspThrd[nThrdCnt]->Initialize(nThrdCnt);
			m_pInspThrd[nThrdCnt]->ResumeThread();
		}
	}
	return true;
}

bool CInspPanel::ExitVision()
{
	for(int nThrdCnt=0; nThrdCnt<MAX_THREAD_COUNT; nThrdCnt++)
	{
		if(m_pInspThrd[nThrdCnt] != NULL)
		{
			//theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, FALSE, _T("send WM_INSPECT_END_THREAD to Image %d's Camera %d"), i, k);
			m_pInspThrd[nThrdCnt]->ExitInstance();
			delete m_pInspThrd[nThrdCnt];
		}
	}

	return true;
}

int CInspPanel::StartInspection(WPARAM wParam, LPARAM lParam)
{
	int nThreadCount = 0;
	bool bFindFreeThread = false;


	while(!bFindFreeThread)
	{
		for (nThreadCount = 0; nThreadCount < MAX_THREAD_COUNT; nThreadCount++)
		{
			if(!m_pInspThrd[nThreadCount]->IsThrdBusy())
			{
				bFindFreeThread = true;
				break;
			}
		}
	}
	int nRet = m_pInspThrd[nThreadCount]->PostThreadMessage(WM_START_INSPECTION, wParam, lParam);

	DWORD errValue = GetLastError();
	CString errTemp;
	errTemp.Format(L"%lu", (LONG)errValue);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection -- Start Log. nRet : %d, errValue : %s, Thread Num : %d"), nRet, errTemp, nThreadCount);

	if (!nRet)
	{
		DWORD errValue = GetLastError();
		CString errTemp;
		errTemp.Format(L"%lu", (LONG)errValue);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection -- Start Fail. nRet : %d, errValue : %s, Thread Num : %d"), nRet, errTemp, nThreadCount);
		nThreadCount = -1;
	}
	else
	{
		/*int nSleepTime = 0;
		do
		{
			Sleep(10);
			nSleepTime += 10;
		} while (m_pInspThrd[nThreadCount]->IsThrdBusy() == false);*/

		int nSleepTime = 0;

		while (m_pInspThrd[nThreadCount]->IsThrdBusy() == false)
		{
			Sleep(1);
			nSleepTime += 1;

			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Test Log. \nStart Inspect SleepTime %d"), nSleepTime);
		}

		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection -- Start OK. Sleep Time : %d, Thread Num : %d"), nSleepTime, nThreadCount);
	}

	return nThreadCount;
}

int CInspPanel::StartSaveImage(WPARAM wParam, LPARAM lParam)
{
	int nThreadCount = 0;
	bool bFindFreeThread = false;

	while (!bFindFreeThread)
	{
		for (nThreadCount = 0; nThreadCount < MAX_THREAD_COUNT; nThreadCount++)
		{
			if (!m_pInspThrd[nThreadCount]->IsThrdBusy())
			{
				bFindFreeThread = true;
				break;
			}
		}
	}



	int nRet = m_pInspThrd[nThreadCount]->PostThreadMessage(WM_START_SAVE_IMAGE, wParam, lParam);

	DWORD errValue = GetLastError();
	CString errTemp;
	errTemp.Format(L"%lu", (LONG)errValue);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection(Save Image) -- Start Log. nRet : %d, errValue : %s, Thread Num : %d"), nRet, errTemp, nThreadCount);

	if (!nRet)
	{
		DWORD errValue = GetLastError();
		CString errTemp;
		errTemp.Format(L"%lu", (LONG)errValue);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection(Save Image) -- Start Fail. nRet : %d, errValue : %s, Thread Num : %d"), nRet, errTemp, nThreadCount);
		nThreadCount = -1;
	}
	else
	{
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Test Log. \nStart Sleep")); //////////////////////////////////////////////////////////////////////////////////////////////////////
		int nSleepTime = 0;
		while (m_pInspThrd[nThreadCount]->IsThrdBusy() == false);
		{
			Sleep(1);  // 10
			//Sleep(10);  // 10
			nSleepTime += 1;
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Test Log. \nStart SleepTime %d"), nSleepTime); //////////////////////////////////////////////////////////////////////////////////////////////////////
		} 
// 		int nSleepTime = 0;
// 
// 		while (m_pInspThrd[nThreadCount]->IsThrdBusy() == false)
// 		{
// 			Sleep(10);  // 10
// 			//Sleep(10);  // 10
// 			nSleepTime += 1;
// 			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Test Log. \nStart SaveImage SleepTime %d"), nSleepTime); //////////////////////////////////////////////////////////////////////////////////////////////////////
// 		} 

		
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection(Save Image) -- Start OK. Sleep Time : %d, Thread Num : %d"), nSleepTime, nThreadCount);
	}

	return nThreadCount;
}
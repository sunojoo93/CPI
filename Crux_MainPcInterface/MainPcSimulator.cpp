// MainPcSimulator.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Crux_MainPcInterface.h"
#include "MainPcSimulator.h"
#include "afxdialogex.h"
#include "TaskInterfacer.h"
#include "DefineStructure.h"

// CMainPcSimulator 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMainPcSimulator, CDialogEx)

CMainPcSimulator::CMainPcSimulator(VOID* pTaskInterface, CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainPcSimulator::IDD, pParent), m_pTaskInterface(pTaskInterface)
{
	int nRet = APP_OK;

	m_nStartCellNo = 0, m_nEndCellNo = 0, m_nRepeat = 0, m_nDelay = 0;
	
	nRet = m_fnLoadCellIdList();


}

CMainPcSimulator::~CMainPcSimulator()
{
}

void CMainPcSimulator::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CELL_ID, m_ctrlListAutoRunCell);
}


BEGIN_MESSAGE_MAP(CMainPcSimulator, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_RELOAD_CELL_LIST, &CMainPcSimulator::OnBnClickedBtnReloadCellList)
	ON_BN_CLICKED(IDC_BTN_SIMUL_START, &CMainPcSimulator::OnBnClickedBtnSimulStart)
	ON_BN_CLICKED(IDC_BTN_SIMUL_STOP, &CMainPcSimulator::OnBnClickedBtnSimulStop)
END_MESSAGE_MAP()


// CMainPcSimulator 메시지 처리기입니다.



int	CMainPcSimulator::m_fnLoadCellIdList()
{
	int nRet = APP_OK;
	CString strKeyName, strTemp;;
	TCHAR	szTempStr[500] = {0,};

	m_strCellID.RemoveAll();

	for (int i = 0; i < 100; i++)
	{
		strKeyName.Format(_T("LoadPanelId%d"),i+1);
		GetPrivateProfileString(_T("SimulationInfo"), strKeyName,_T("NONE_CELL_ID"), szTempStr, sizeof(szTempStr), VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);
		strTemp = szTempStr;
		if ( strTemp == _T("NONE_CELL_ID"))
			break;
		else
			m_strCellID.Add(szTempStr);
	}

	m_nStartCellNo	= GetPrivateProfileInt(_T("SimulationInfo"), _T("StartCellNo"), 1, VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);
	m_nEndCellNo	= GetPrivateProfileInt(_T("SimulationInfo"), _T("EndCellNo"),	1, VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);
	m_nRepeat		= GetPrivateProfileInt(_T("SimulationInfo"), _T("Repeat"),		1, VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);
	m_nDelay		= GetPrivateProfileInt(_T("SimulationInfo"), _T("Delay"),		1, VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);

	return nRet;
}

int	CMainPcSimulator::m_fnShowCellIdList()
{
	int nRet = APP_OK;

	m_ctrlListAutoRunCell.ResetContent();

	CString strTemp;
	for (int i = m_strCellID.GetCount() - 1; i >= 0; i--)
	{
		strTemp.Format(_T("LOAD CELL ID %02d    =    %30s"), i+1, (LPCTSTR)m_strCellID.GetAt(i));
		m_ctrlListAutoRunCell.InsertString(0, strTemp);
	}

	this->SetDlgItemInt(IDC_EDIT_START_CELL_NO, m_nStartCellNo);
	this->SetDlgItemInt(IDC_EDIT_END_CELL_NO,	m_nEndCellNo);
	this->SetDlgItemInt(IDC_EDIT_REPEAT_COUNT,	m_nRepeat);
	this->SetDlgItemInt(IDC_EDIT_DELAY,			m_nDelay);

	return nRet;
}

void CMainPcSimulator::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	int nRet =  m_fnShowCellIdList();
}

void CMainPcSimulator::OnBnClickedBtnReloadCellList()
{
	int nRet = APP_OK;

	nRet = m_fnLoadCellIdList();

	nRet = m_fnShowCellIdList();

}


void CMainPcSimulator::OnBnClickedBtnSimulStart()
{
	int nRet = APP_OK;
	ST_SIMULATION_INFO stSimulationInfo;

	m_nStartCellNo	= GetDlgItemInt(IDC_EDIT_START_CELL_NO);
	m_nEndCellNo	= GetDlgItemInt(IDC_EDIT_END_CELL_NO);
	m_nRepeat		= GetDlgItemInt(IDC_EDIT_REPEAT_COUNT);
	m_nDelay		= GetDlgItemInt(IDC_EDIT_DELAY);

	int nStartLine = 0, nStartPos = 0, nEndPos = 0, nTotalLine = 0;
	CString strCellId, strStartLine, strTotalLine, strDirection;

	// MergeTool 촬영위치, 검사 Step 추가 
	CString strGrabPos, strInspStep;
	int nGrabPos = 0, nInspStep = 0;


	byte bParam[500] = { 0, };

	CStringA stRcvPacket = "MOT.GRAB.START.8.F.";

	byte* pParamTemp = bParam;

	stRcvPacket.Replace(MAIN_PC_PACKET_GRAB_START, "");

	nStartPos = nEndPos + 0;
	nEndPos = stRcvPacket.Find(".", nStartPos);
	strStartLine = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

	//nStartPos		= nEndPos + 1;
	//nEndPos			= stRcvPacket.Find(".", nStartPos);
	//strTotalLine    = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);
	CString m_strCellID = _T("Test22");

	nStartPos = nEndPos + 1;
	nEndPos = stRcvPacket.Find(".", nStartPos);
	strDirection = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);


	nStartLine = _ttoi(strStartLine);
	nTotalLine = 1/*_ttoi(strTotalLine)*/;

	*(int*)pParamTemp = nStartLine;
	pParamTemp += sizeof(int);

	_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)m_strCellID, m_strCellID.GetLength());
	pParamTemp += SIZE_CELL_ID;

	_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strDirection, strDirection.GetLength());
	pParamTemp += SIZE_CELL_ID;


	*(int*)pParamTemp = nTotalLine;
	pParamTemp += sizeof(int);


	

	/*if (m_nStartCellNo <= 0 || m_nStartCellNo > m_strCellID.GetCount())
	{
		AfxMessageBox(_T("Check Start Cell No!"));
		return;
	}

	if (m_nEndCellNo <= 0 || m_nEndCellNo > m_strCellID.GetCount())
	{
		AfxMessageBox(_T("Check End Cell No!"));
		return;
	}

	if (m_nStartCellNo > m_nEndCellNo)
	{
		AfxMessageBox(_T("Check Start and End Cell No!"));
		return;
	}

	if (m_nRepeat < 0 )
	{
		AfxMessageBox(_T("Check Repeat Cnt No!"));
		return;
	}

	if (m_nDelay < 0 )
	{
		AfxMessageBox(_T("Check Delay!"));
		return;
	}

	if (!m_pTaskInterface)
	{
		AfxMessageBox(_T("IPC Connect Error!"));
		return;
	}*/

	/*CString strTemp;
	strTemp.Format(_T("%d"), m_nStartCellNo);
	WritePrivateProfileString(_T("SimulationInfo"), _T("StartCellNo"),	strTemp,	VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);
	strTemp.Format(_T("%d"), m_nEndCellNo);
	WritePrivateProfileString(_T("SimulationInfo"), _T("EndCellNo"),	strTemp,	VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);
	strTemp.Format(_T("%d"), m_nRepeat);
	WritePrivateProfileString(_T("SimulationInfo"), _T("Repeat"),		strTemp,	VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);
	strTemp.Format(_T("%d"), m_nDelay);
	WritePrivateProfileString(_T("SimulationInfo"), _T("Delay"),		strTemp,	VS_MAIN_PC_SIMULATOR_TASK_INI_FILE);

	stSimulationInfo.nRepeatCnt			= m_nRepeat;
	stSimulationInfo.nDelay				= m_nDelay;
	stSimulationInfo.nTotalCellCount	= m_nEndCellNo - m_nStartCellNo + 1;

	for (int i = m_nStartCellNo - 1; i <= m_nEndCellNo - 1; i++)
	{

		_tcscpy(&stSimulationInfo.szCellId[i][0], m_strCellID.GetAt(i));
	}*/

	//nRet = ((TaskInterfacer*)m_pTaskInterface)->CmdEditSend(61, 1, 0, sizeof(ST_SIMULATION_INFO), VS_MAIN_PC_SIMULATOR_TASK, 
	//	(byte*)&stSimulationInfo, CMD_TYPE_NORES);

	nRet = ((TaskInterfacer*)m_pTaskInterface)->CmdEditSend(60, 1, 0, sizeof(bParam), VS_MAIN_PC_TASK,
		bParam/*(byte*)&stSimulationInfo*/, CMD_TYPE_RES);

}


void CMainPcSimulator::OnBnClickedBtnSimulStop()
{
	int nRet = ((TaskInterfacer*)m_pTaskInterface)->CmdEditSend(99, 99, 0, 0, VS_MAIN_PC_SIMULATOR_TASK, 
		NULL, CMD_TYPE_NORES);
}

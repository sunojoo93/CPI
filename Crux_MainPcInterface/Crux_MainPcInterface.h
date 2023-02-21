
// Crux_MainPcInterface.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include <direct.h>


// CCrux_MainPcInterfaceApp:
// �� Ŭ������ ������ ���ؼ��� Crux_MainPcInterface.cpp�� �����Ͻʽÿ�.
//

class CCrux_MainPcInterfaceApp : public CWinApp
{
public:
	CCrux_MainPcInterfaceApp();

// �������Դϴ�.
public:


	CString GETDRV()
	{

		// 		TCHAR currentDir[MAX_PATH];
		// 		GetCurrentDirectory(MAX_PATH, currentDir);

		TCHAR buff[MAX_PATH];
		memset(buff, 0, MAX_PATH);
		::GetModuleFileName(NULL, buff, sizeof(buff));
		CString strFolder = buff;
		CString strRet = strFolder.Left(1);
		strRet.MakeUpper();		//180919 YSS	// 181001 yskim Lower -> Upper�� ����
		return strRet;

		// 		int nDrive = 0;	nDrive = _getdrive();
		// 		CString str;
		// 		str.Format(_T("%c"), char(nDrive) + 96);
		// 		return strRet;
	};

	virtual BOOL InitInstance();

	int GetPCNum()								{	return m_nPCNum;	};
	int GetInspType()							{	return m_nInspType;	};
	BOOL GetSimulationProgramMode()				{	return m_bSimulationProgramMode; }
	BOOL GetSimulationMode()					{	return m_bSimulationMode; }

	// YHS 18.03.12 - Merge Tool get �Լ� �߰�
	BOOL GetMergeToolUse() {
		m_nInspStep = GetPrivateProfileInt(_T("MergeTool"), _T("USE_MERGE_TOOL"), 1, GETDRV() + _T(":\\Crux\\DATA\\INI\\Initialize.ini")) ? true : false; return m_nInspStep;
	};
	// yskim 19.12.03 - �׷� ���� �������� ����
	int GetGrabLimitCount() {
		return m_nGrabLimitCount;
	};
	// yskim 19.12.03 - �׷� ���� �������� ��� �ð� (ms)
	int GetGrabLimitWaitTime() {
		return m_nGrabLimitWaitTime;
	};

private:
	int m_nPCNum;
	int m_nInspType;						//�˻�� Type, AVI:0, SVi:1, APP:2
	BOOL m_bSimulationProgramMode;			//Program �� Simultor�� ���� �Ǿ�� �ϴ��� Flag , ARGV �� ���� ��带 �����Ѵ�.
	BOOL m_bSimulationMode;					//Crux �� Simulation Mode �� �����Ǿ�� �ϴ��� Flag,

// �����Դϴ�.

	// YHS 18.03.12 - Merge Tool ���� �߰�
	BOOL m_nInspStep;

	int m_nGrabLimitCount;		// yskim 19.12.03 - �׷� ���� �������� ����
	int m_nGrabLimitWaitTime;	// yskim 19.12.03 - �׷� ���� �������� ��� �ð� (ms)

	DECLARE_MESSAGE_MAP()
};

extern CCrux_MainPcInterfaceApp theApp;
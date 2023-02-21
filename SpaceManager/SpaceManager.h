
// SpaceManager.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "LogWriter.h"		// �α� �߰�

#define SM_LOG_PATH		_T("D:\\CRUX\\DATA\\LOG\\")

// CSpaceManagerApp:
// �� Ŭ������ ������ ���ؼ��� SpaceManager.cpp�� �����Ͻʽÿ�.
//

class CSpaceManagerApp : public CWinApp
{
public:
	CSpaceManagerApp();

	CLogWriter*				m_pLogWriter;								// �αװ����� ��ü
	void					m_fnWriteLog(const TCHAR* pszLog, ...);		// �α� ��� �Լ�

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CSpaceManagerApp theApp;
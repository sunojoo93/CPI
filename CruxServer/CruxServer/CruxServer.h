
// CruxServer.h : CruxServer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CCruxServerApp:
// �� Ŭ������ ������ ���ؼ��� CruxServer.cpp�� �����Ͻʽÿ�.
//

class CCruxServerApp : public CWinApp
{
public:
	CCruxServerApp();

protected:
	// GDI+ ��
	ULONG_PTR				gdiplusToken;
	GdiplusStartupInput	gdiplusstartupinput;

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCruxServerApp theApp;

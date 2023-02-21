////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// multi-language utilty class
/// 
/// @author   parkkh
/// @date     Monday, February 02, 2009  9:40:02 AM
/// 
/// Copyright(C) 2009 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
history

- 2011/6/30 : ������ ���̾˷α� �ڽ� ���� �ؽ�Ʈ�� ���� [DIALOG] �� �����ڴ� ����� �����ؼ�
              ������ ���̾˷α׹ڽ� ������ ������ �� �ֵ��� ( [IDD_ABOUT] �� ����.. ) ��� �߰�

*/

/*

#define _XMULTILANG_DO_NOT_ASSERT_WHEN_DUPE_ID

*/


#pragma once

#include "XIni.h"

#define _XMULTILANG_DO_NOT_ASSERT_WHEN_DUPE_ID


class XMultiLang  
{
public:
	XMultiLang();
	~XMultiLang();

	BOOL						Create(CString sIniFilePath, CString sIniFilePathDefault, CString sResourceHPath);
	BOOL						Create(LPBYTE pIniFile, int nIniFileLen, LPBYTE pResourceHeader, int nResourceHeaderLen);

	void						SetDlgTexts(HWND hWnd, UINT nIDD);
	LPCTSTR						Lang(LPCTSTR szLangKey, LPCTSTR szSection=_T("TEXT") );
	int							GetCodePage() { return m_nCodePage; }
	void						TranslateMenuString(HMENU hMenu);
	XIni&						GetXIni() { return m_ini; }

private :
	static BOOL	CALLBACK		EnumProc(HWND hWnd, LPARAM lParam);
	void						FreeLangFunc();
	BOOL						_Create();
	CString						FindDialogControlText(int nCtrlID);

private :
#ifndef _MFC_VER
	CAtlMap<CString, CString>	m_mapid2str;
	CAtlMap<CString, void*>		m_mapLang;			// ���ҽ� ��Ʈ�� ���̺��� ������ ���� ��
#else
	CMapStringToString 			m_mapid2str;
	CMapStringToPtr				m_mapLang;			// ���ҽ� ��Ʈ�� ���̺��� ������ ���� ��
#endif

	int							m_nCodePage;
	HWND						m_hWndParent;
	XIni						m_ini;				// �⺻ ��� ����
	XIni						m_iniDefault;		// �⺻ �� ������ ����� �߰���� ����
	BOOL						m_bCreated;			// ���������� Create() �Ǿ��� ?

	CString						m_sCurDlgID;		// EnumChildWindows() ȣ���Ҷ� ���� ���̾˷α� �ڽ��� ���ڿ� ID ("IDD_ABOUT" �� ���� ������ ���ڿ�)
};

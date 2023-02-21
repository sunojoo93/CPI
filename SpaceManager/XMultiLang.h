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

- 2011/6/30 : 기존에 다이알로그 박스 관련 텍스트를 전부 [DIALOG] 에 때려박던 방식을 개선해서
              각각의 다이알로그박스 섹션을 지정할 수 있도록 ( [IDD_ABOUT] 과 같이.. ) 기능 추가

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
	CAtlMap<CString, void*>		m_mapLang;			// 리소스 스트링 테이블을 저장할 전역 맵
#else
	CMapStringToString 			m_mapid2str;
	CMapStringToPtr				m_mapLang;			// 리소스 스트링 테이블을 저장할 전역 맵
#endif

	int							m_nCodePage;
	HWND						m_hWndParent;
	XIni						m_ini;				// 기본 언어 파일
	XIni						m_iniDefault;		// 기본 언어에 없을때 사용할 추가언어 파일
	BOOL						m_bCreated;			// 성공적으로 Create() 되었나 ?

	CString						m_sCurDlgID;		// EnumChildWindows() 호출할때 현재 다이알로그 박스의 문자열 ID ("IDD_ABOUT" 과 같은 형태의 문자열)
};

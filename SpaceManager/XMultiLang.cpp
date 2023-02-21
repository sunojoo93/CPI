#include "stdafx.h"
#include "XMultiLang.h"
#include "XToken.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define LEN_BUF 1024


XMultiLang::XMultiLang()
{
	m_hWndParent = NULL;
	m_nCodePage = CP_ACP;
	m_bCreated = FALSE;
}

XMultiLang::~XMultiLang()
{
	FreeLangFunc();
}

BOOL XMultiLang::Create(LPBYTE pIniFile, int nIniFileLen, LPBYTE pResourceHeader, int nResourceHeaderLen)
{
	FreeLangFunc();
	m_mapid2str.RemoveAll();
	m_bCreated = FALSE;

	// �޸𸮷� ini ���� ����
	m_ini.OpenMemory(pIniFile, nIniFileLen);

	// �޸𸮷� ���ν� ��� ���� ����
	// �޸� �����ϱ�
	CHAR* src = (CHAR*)malloc(nResourceHeaderLen + sizeof(CHAR));
	if(src==NULL){ASSERT(0); return FALSE;}

	memcpy(src, pResourceHeader, nResourceHeaderLen);
	src[nResourceHeaderLen] = NULL;

	CHAR szDefine[LEN_BUF], szTextID[LEN_BUF], szNumID[LEN_BUF];
	CString temp;

	int		nSrcPos=0;

	while(nSrcPos < nResourceHeaderLen)
	{
		szDefine[0] = szTextID[0] = szNumID[0] = 0;
		//_stscanf(buf, _T("%s %s %s"), szDefine, szTextID, szNumID);
		nSrcPos += sscanf_s(src + nSrcPos, "%s %s %s", szDefine, LEN_BUF, szTextID, LEN_BUF, szNumID, LEN_BUF);
		if(strncmp(szDefine, "#define", 7)==0)
		{
			//m_mapstr2id[szTextID] = szNumID;
			if(szTextID[0]!='_')
			{
				if(m_mapid2str.Lookup(CString(szNumID), temp)) ASSERT(0);	// �̹� �ִ�??
				m_mapid2str[CString(szNumID)] = szTextID;
			}
		}

		while(nSrcPos < nResourceHeaderLen)
		{
			if( *(src + nSrcPos)=='#') 
				break;
			else
				nSrcPos++;
		}
	}
	free(src);


	return _Create();
}

BOOL XMultiLang::Create(CString sIniFilePath, CString sIniFilePathDefault, CString sResourceHPath)
{
	XIni xin;
	FreeLangFunc();
	m_mapid2str.RemoveAll();
	m_bCreated = FALSE;

	BOOL ret1=FALSE, ret2=FALSE;

	ret1 = m_ini.Open(sIniFilePath);
	if(sIniFilePathDefault.IsEmpty()==FALSE)
		ret2 = m_iniDefault.Open(sIniFilePathDefault);

	// ini ������ �� �� ����.
	if(ret1==FALSE && ret2==FALSE)
		return FALSE;

	if(sResourceHPath.IsEmpty()==FALSE)
	{
		//FILE *fp = NULL;
		//fp = _tfopen(sResourceHPath, _T("r"));
		//errno_t err = _tfopen_s(&fp, sResourceHPath, _T("rb"));
		//if(err!=0) {ASSERT(0);return FALSE;}
		//if(fp==NULL) {ASSERT(0);return FALSE;}


		TCHAR buf[LEN_BUF];
		TCHAR szDefine[LEN_BUF], szTextID[LEN_BUF], szNumID[LEN_BUF];
		CString temp;
		BYTE*		pData;							// ����Ÿ ����
		int			nDataLen;						// ����Ÿ ũ��
		HANDLE		hFile;							// ���� �ڵ�
		//HANDLE		hFile1;							// ���� �ڵ�
		DWORD		read;

		// ���� ����
		hFile = ::CreateFile(sResourceHPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if(hFile==INVALID_HANDLE_VALUE) {ASSERT(0); return FALSE; }

		// �޸� ���.
		nDataLen = ::GetFileSize(hFile, NULL);			// ���� ũ��
		if(nDataLen>1000*1000) {ASSERT(0); CloseHandle(hFile); return FALSE;}	// �ʹ� ũ��!!!
		if(nDataLen==0) {CloseHandle(hFile); return TRUE;}						// 0 ����Ʈ�̴�..
		pData = (BYTE*)calloc(1, nDataLen+sizeof(WCHAR));
		if(pData==NULL){ASSERT(0); CloseHandle(hFile); return FALSE;}

		// ���� �б�
		if(::ReadFile(hFile, pData, nDataLen, &read, NULL)==FALSE || read!=(DWORD)nDataLen)
		{
			ASSERT(0);
			free(pData);
			CloseHandle(hFile);
			return FALSE;
		}
		if(nDataLen<2) {ASSERT(0); return FALSE;}

		LPCTSTR		pNewData = NULL;
		LPCTSTR		szData;
		int			nStrLen = 0;									// ���ڿ� ī��Ʈ

		if(xin.IsUnicodeFileHeader(pData))								// �����ڵ� ���Ͽ��� ���� ����Ÿ �ΰ�?
		{
#ifdef _UNICODE													// unicode -> unicode
			szData = (LPCTSTR)(pData+2);					// bom ����
			nStrLen = (nDataLen-2) / sizeof(WCHAR);
#else															// unicode -> ascii
			pNewData = xin.Unicode2Ascii((LPCWSTR)(pData+2), m_nCodePage, nStrLen);
			if(pNewData==NULL){ASSERT(0); return FALSE;}
			szData = pNewData;
#endif
		}
		else														// BOM �� ����? == ASII
		{
#ifdef _UNICODE													// ascii -> unicode
			pNewData = xin.Ascii2Unicode((LPCSTR)pData, m_nCodePage);
			if(pNewData==NULL){ASSERT(0); return FALSE;}
			szData = pNewData;
#else															// ascii -> ascii
			szData = (LPCTSTR)(pData);
#endif

			nStrLen = nDataLen;

		}
		CloseHandle(hFile);
		xin.m_pDataCurrent = szData;

		xin.m_nStrLen = nStrLen;
		xin.m_nStrPos = 0;	
		for(;;)
		{
			if(xin.ReadALine()==FALSE) break;
			_tcscpy_s(buf,xin.m_sLine);
			//	}
			//	while(!feof(fp))
			//	{
			//		_fgetts(buf, LEN_BUF, fp);
			szDefine[0] = szTextID[0] = szNumID[0] = 0;
			//_stscanf(buf, _T("%s %s %s"), szDefine, szTextID, szNumID);
			_stscanf_s(buf, _T("%s %s %s"), szDefine, LEN_BUF, szTextID, LEN_BUF, szNumID, LEN_BUF);
			if(_tcsncmp(szDefine, _T("#define"), 7)==0)
			{
				//m_mapstr2id[szTextID] = szNumID;
				if(szTextID[0]!='_')
				{
					if(m_mapid2str.Lookup(szNumID, temp)) 
					{
#ifndef _XMULTILANG_DO_NOT_ASSERT_WHEN_DUPE_ID
						ASSERT(0);
#endif
						// �̹� �ִ�?? - ��ǥ�� �ڿ� ���δ�
						temp = temp + _T(",") + szTextID;
						m_mapid2str[szNumID] = temp;
					}
					else
						m_mapid2str[szNumID] = szTextID;
				}
			}
		}

		//fclose(fp);
	}

	return _Create();
}


BOOL XMultiLang::_Create()
{
	m_mapid2str[_T("1")] = _T("IDOK");
	m_mapid2str[_T("2")] = _T("IDCANCEL");

	//m_nCodePage = ::GetIniInt(_T("GENERAL"), _T("CODEPAGE"), CP_ACP, m_sIniFilePath);
	m_nCodePage = m_ini.GetInt(	_T("GENERAL"), _T("CODEPAGE"), CP_ACP);

	m_bCreated = TRUE;

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���̾˷α� �ڽ� ��Ʈ���� �ؽ�Ʈ�� ã�´�. (Ÿ��Ʋ�� ã���ش�.)
/// @param  
/// @return 
/// @date   Thursday, June 30, 2011  11:31:10 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
CString	XMultiLang::FindDialogControlText(int nCtrlID)
{
	CString sNumID;
	CString sTextID;
	CString sControlText;

	sNumID.Format(_T("%d"), nCtrlID);

	if(m_mapid2str.Lookup(sNumID, sTextID)==FALSE) return sControlText;

	// ��ǥ�� ���е� ��� ������ ����Ÿ�� ���ؼ� �� ������.
	XToken token;
	token.Init(sTextID, ',');

	sTextID = token.GetNextToken();
	while(sTextID.IsEmpty()==FALSE)
	{
		// "IDD_ABOUT" �� ���� ���ǿ��� ã�´�.
		// ù��° ��� ���Ͽ��� ã��
		sControlText = m_ini.GetStr(m_sCurDlgID, sTextID, _T(""));

		// ������� �⺻ ��� ���Ͽ��� ã��
		if(sControlText.IsEmpty())
			sControlText = m_iniDefault.GetStr(m_sCurDlgID, sTextID, _T(""));

		// �׷��� ���� ��� --- "DIALOG" ���ǿ��� ã�´�.
		if(sControlText.IsEmpty() && m_sCurDlgID.IsEmpty()==FALSE)
		{
			// ù��° ��� ���Ͽ��� ã��
			sControlText = m_ini.GetStr(_T("DIALOG"), sTextID, _T(""));

			// ������� �⺻ ��� ���Ͽ��� ã��
			if(sControlText.IsEmpty())
				sControlText = m_iniDefault.GetStr(_T("DIALOG"), sTextID, _T(""));
		}

		if(sControlText.IsEmpty()==FALSE)
			break;

		// ���� ��ū
		sTextID = token.GetNextToken();
	}


	// �ٹٲ� ó��
	sControlText.Replace(_T("\\n"), _T("\n"));
	sControlText.Replace(_T("\\r"), _T("\r"));


	return sControlText;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         enum �ݹ�
/// @param  
/// @return 
/// @date   Thursday, June 30, 2011  11:36:50 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK XMultiLang::EnumProc(HWND hWnd, LPARAM lParam)
{
	XMultiLang*  _this = (XMultiLang*)lParam;

	if(_this->m_hWndParent != ::GetParent(hWnd)) return TRUE;

	UINT nCtrlID = ::GetDlgCtrlID(hWnd);
	CString sControlText;

	sControlText = _this->FindDialogControlText(nCtrlID);

	// ��Ʈ�� �ؽ�Ʈ �ٲٱ�
	if(sControlText.IsEmpty()==FALSE)
		SetWindowText(hWnd, sControlText);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          ���̾˷α� �������� �ؽ�Ʈ �����ϱ�..
/// @param   hWnd  
/// @return  
/// @date    2005-03-12 ���� 4:54:35
////////////////////////////////////////////////////////////////////////////////////////////////////
void XMultiLang::SetDlgTexts(HWND hDlg, UINT nIDD)
{
	m_hWndParent = hDlg;


	// ���̾˷α� �ڽ��� ���ڿ� ���̵� ã��
	CString sIDD;
	sIDD.Format(_T("%d"), nIDD);
	m_sCurDlgID.Empty();
	m_mapid2str.Lookup(sIDD, m_sCurDlgID);

	// ���̾˷α� �ڽ� �� ��Ʈ�ѵ�
	EnumChildWindows(hDlg, EnumProc, (LPARAM)this);

	
	// ������ ȣ���ϴ� �ڵ�--> � �뵵���� �𸣰ڴ�..
	//EnumProc(hDlg, (LPARAM)this);
	


	// ���̾˷α� �ڽ� Ÿ��Ʋ
	CString sControlText = FindDialogControlText(nIDD);

	if(sControlText.IsEmpty()==FALSE)
		SetWindowText(hDlg, sControlText);
}


LPCTSTR XMultiLang::Lang(LPCTSTR szLangKey, LPCTSTR szSection)
{
	if(m_bCreated==FALSE) return _T("Can't load language file");		// �ʱ�ȭ ������ ���.


	TCHAR*	szStr;

	if(m_mapLang.Lookup(szLangKey, (void*&)szStr)) return szStr;		// map ���� ã��.

	CString buf;

	if(m_ini.GetStr2(szSection, szLangKey,buf)==FALSE || buf.IsEmpty())
	{
		if(m_iniDefault.GetStr2(szSection, szLangKey, buf)==FALSE)
			ASSERT(0);											// ������Ͽ� ����..
	}

	buf.Replace(_T("\\n"), _T("\n"));
	buf.Replace(_T("\\r"), _T("\r"));

	szStr = _tcsdup(buf);
	m_mapLang[szLangKey] = szStr;							// map �� ����.
	return szStr;
}

void XMultiLang::FreeLangFunc()
{
	POSITION p;
	CString		nKey;
	TCHAR*		szValue;
	p = m_mapLang.GetStartPosition();
	while(p)
	{
		m_mapLang.GetNextAssoc(p, nKey, (void*&)szValue);
		free(szValue);								// free..
	}
	m_mapLang.RemoveAll();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///         �޴� ���ڿ� �ٲٱ�
/// @param  
/// @return 
/// @date   2007�� 11�� 29�� �����  ���� 11:01:58
////////////////////////////////////////////////////////////////////////////////////////////////////
#define LEN_BUF	1024
void XMultiLang::TranslateMenuString(HMENU hMenu)
{
	int count;
	int i;
	int ret;
	CString str;
	MENUITEMINFO  info;
	TCHAR			buf[LEN_BUF];
	int				id;

	count = ::GetMenuItemCount(hMenu);

	info.cbSize = sizeof (MENUITEMINFO);
	info.fMask = MIIM_SUBMENU;

	for(i=0;i<count;i++)
	{
		ret = ::GetMenuString(hMenu, i, buf, LEN_BUF, MF_BYPOSITION);
		id = ::GetMenuItemID(hMenu, i);

		str = m_ini.GetStr(_T("MENU"), buf);
		if(str.IsEmpty())
			str = m_iniDefault.GetStr(_T("MENU"), buf);

		str.Replace(_T("\\t"), _T("\t"));		// \t to tab

		if(str.IsEmpty()==FALSE)
			::ModifyMenu(hMenu, i, MF_BYPOSITION | MF_STRING, id, str);

		// sub menu ó��
		if(	GetMenuItemInfo(hMenu, i, TRUE, &info))
		{
			if(info.hSubMenu)
			{
				HMENU hSub = GetSubMenu(hMenu, i);
				TranslateMenuString(hSub);
			}
		}
	}
}


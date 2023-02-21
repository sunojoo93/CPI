// GxButtonEx.h  : Microsoft Visual C++로 만든 ActiveX 컨트롤 래퍼 클래스의 선언입니다.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CGxButtonEx

class CGxButtonEx : public CWnd
{
protected:
	DECLARE_DYNCREATE(CGxButtonEx)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x92FB3768, 0x1729, 0x4040, { 0x84, 0xE3, 0x41, 0xFC, 0xCE, 0x65, 0x26, 0x35 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 특성입니다.
public:

// 작업입니다.
public:

	void StartBlink()
	{
		InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void StopBlink()
	{
		InvokeHelper(0x48, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetShade(short ShadeGranularity, short ShadeColoring, unsigned long ShadeHighColor, unsigned long ShadeMiddleColor, unsigned long ShadeLowColor)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_UI4 VTS_UI4 VTS_UI4 ;
		InvokeHelper(0x49, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ShadeGranularity, ShadeColoring, ShadeHighColor, ShadeMiddleColor, ShadeLowColor);
	}
	void SetFontName(LPCTSTR sName)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x4a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, sName);
	}
	void SetFontSize(short nSize)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x4b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nSize);
	}
	void SetFontBold(long bValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bValue);
	}
	void SetFontItalic(long bValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bValue);
	}
	unsigned long GetBackColor()
	{
		unsigned long result;
		InvokeHelper(0x4e, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetBackColor(unsigned long nNewValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x4f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	unsigned long GetTextColor()
	{
		unsigned long result;
		InvokeHelper(0x50, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetTextColor(unsigned long nNewValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x51, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	unsigned long GetGradationStartColor()
	{
		unsigned long result;
		InvokeHelper(0x52, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetGradationStartColor(unsigned long nNewValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x53, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	unsigned long GetGradationEndColor()
	{
		unsigned long result;
		InvokeHelper(0x54, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetGradationEndColor(unsigned long nNewValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x55, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	short GetIconIndex()
	{
		short result;
		InvokeHelper(0x56, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetIconIndex(short nNewValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x57, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	short GetImgIndex()
	{
		short result;
		InvokeHelper(0x58, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetImgIndex(short nNewValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x59, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	unsigned long GetBlinkTextColor()
	{
		unsigned long result;
		InvokeHelper(0x5a, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetBlinkTextColor(unsigned long nNewValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x5b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	unsigned long GetBlinkBackColor()
	{
		unsigned long result;
		InvokeHelper(0x5c, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetBlinkBackColor(unsigned long nNewValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x5d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	unsigned long GetBlinkBorderColor()
	{
		unsigned long result;
		InvokeHelper(0x5e, DISPATCH_METHOD, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void SetBlinkBorderColor(unsigned long nNewValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x5f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	short GetBlinkTime()
	{
		short result;
		InvokeHelper(0x60, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetBlinkTime(short nNewValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x61, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	CString GetCaption()
	{
		CString result;
		InvokeHelper(0x62, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void SetCaption(LPCTSTR lpszNewValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x63, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lpszNewValue);
	}
	short GetTag()
	{
		short result;
		InvokeHelper(0x64, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetTag(short nNewValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x65, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	CString GetMyAddress()
	{
		CString result;
		InvokeHelper(0x66, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long GetEnabled()
	{
		long result;
		InvokeHelper(0x67, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void SetEnabled(long bNewValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bNewValue);
	}


};

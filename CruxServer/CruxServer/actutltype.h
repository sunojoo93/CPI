#pragma once

// 컴퓨터에서 Microsoft Visual C++를 사용하여 생성한 IDispatch 래퍼 클래스입니다.

// 참고: 이 파일의 내용을 수정하지 마십시오. Microsoft Visual C++에서
//  이 클래스를 다시 생성할 때 수정한 내용을 덮어씁니다.

/////////////////////////////////////////////////////////////////////////////
// CActutltype 래퍼 클래스입니다.

class CActutltype : public CWnd
{
protected:
	DECLARE_DYNCREATE(CActutltype)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x63885648, 0x1785, 0x41A4, { 0x82, 0xD5, 0xC5, 0x78, 0xD2, 0x9E, 0x4D, 0xA8 } };
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

// IActUtlType

// Functions
//

	long get_ActLogicalStationNumber()
	{
		long result;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActLogicalStationNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long Open()
	{
		long result;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Close()
	{
		long result;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetCpuType(BSTR * szCpuName, long * lplCpuCode)
	{
		long result;
		static BYTE parms[] = VTS_PBSTR VTS_PI4 ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szCpuName, lplCpuCode);
		return result;
	}
	long SetCpuStatus(long lOperation)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lOperation);
		return result;
	}
	long ReadDeviceBlock(LPCTSTR szDevice, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lplData);
		return result;
	}
	long WriteDeviceBlock(LPCTSTR szDevice, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lplData);
		return result;
	}
	long ReadDeviceRandom(LPCTSTR szDeviceList, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lplData);
		return result;
	}
	long WriteDeviceRandom(LPCTSTR szDeviceList, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lplData);
		return result;
	}
	long ReadBuffer(long lStartIO, long lAddress, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_PI2 ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lStartIO, lAddress, lSize, lpsData);
		return result;
	}
	long WriteBuffer(long lStartIO, long lAddress, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_PI2 ;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lStartIO, lAddress, lSize, lpsData);
		return result;
	}
	long GetClockData(short * lpsYear, short * lpsMonth, short * lpsDay, short * lpsDayOfWeek, short * lpsHour, short * lpsMinute, short * lpsSecond)
	{
		long result;
		static BYTE parms[] = VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lpsYear, lpsMonth, lpsDay, lpsDayOfWeek, lpsHour, lpsMinute, lpsSecond);
		return result;
	}
	long SetClockData(short sYear, short sMonth, short sDay, short sDayOfWeek, short sHour, short sMinute, short sSecond)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0xd, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sYear, sMonth, sDay, sDayOfWeek, sHour, sMinute, sSecond);
		return result;
	}
	long SetDevice(LPCTSTR szDevice, long lData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0xe, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lData);
		return result;
	}
	long GetDevice(LPCTSTR szDevice, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_PI4 ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lplData);
		return result;
	}
	long CheckDeviceString(LPCTSTR szDevice, long lCheckType, long lSize, long * lplDeviceType, BSTR * lpszDeviceName, long * lplDeviceNumber, long * lplDeviceRadix)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_PI4 VTS_PBSTR VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lCheckType, lSize, lplDeviceType, lpszDeviceName, lplDeviceNumber, lplDeviceRadix);
		return result;
	}
	long EntryDeviceStatus(LPCTSTR szDeviceList, long lSize, long lMonitorCycle, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_PI4 ;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lMonitorCycle, lplData);
		return result;
	}
	long FreeDeviceStatus()
	{
		long result;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_ActPassword()
	{
		CString result;
		InvokeHelper(0x14, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ActPassword(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x14, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long ReadDeviceBlock2(LPCTSTR szDevice, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lpsData);
		return result;
	}
	long WriteDeviceBlock2(LPCTSTR szDevice, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lpsData);
		return result;
	}
	long ReadDeviceRandom2(LPCTSTR szDeviceList, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x17, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lpsData);
		return result;
	}
	long WriteDeviceRandom2(LPCTSTR szDeviceList, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x18, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lpsData);
		return result;
	}
	long GetDevice2(LPCTSTR szDevice, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_PI2 ;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lpsData);
		return result;
	}
	long SetDevice2(LPCTSTR szDevice, short sData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I2 ;
		InvokeHelper(0x1a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, sData);
		return result;
	}
	long Connect()
	{
		long result;
		InvokeHelper(0x1b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Disconnect()
	{
		long result;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}

// Properties
//



};

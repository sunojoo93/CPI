#pragma once

#include ".\Grid\GridCtrl.h"
#include <vector>	

using namespace std;

// CViewIOView 폼 뷰입니다.

class CViewIOView : public CFormView
{
	DECLARE_DYNCREATE(CViewIOView)

protected:
	virtual ~CViewIOView();

public:
	CViewIOView();           // 동적 만들기에 사용되는 protected 생성자입니다.

	enum { IDD = IDD_VIEW_IOVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CGridCtrl m_GridPCSideIO;
	CGridCtrl m_GridPLCSideIO;
	CGridCtrl m_GridCIMSideIO;
	int m_nAddrStartPC;
	int m_nAddrEndPC;
	int m_nAddrStartPLC;
	int m_nAddrEndPLC;
	int m_nItemCntPC;
	int m_nItemCntPLC;
	int m_nAddrStartCim;
	int m_nAddrEndCim;
	int m_nItemCntCim;

	void m_fnInitGridPCSideIO();
	void m_fnInitGridPLCSideIO();

	void m_fnLoadMapConfig();
	void m_fnUpdateGrid(CGridCtrl* pGridCtrl, int nIndex, CString strData);
	void m_fnUpdateMapData();
	void m_fnSetData(CGridCtrl* pGridCtrl, int nStartAddr, CString strAddr, int nValue);

	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void m_fnInitGridCIMSideIO(void);
	CStringArray tet;
//	wchar_t*  PLC_ADDR_READ()
//	{
//		wchar_t * tTemp[] = 
//		{
//#define	PLDADRR(a,b,c) L#a,L#b,L#c,
//#include "PLC_ADDR.inc"
//#undef PLDADRR
//
//		};
//
//		
//		return (wchar_t*)tTemp;
//	};

};



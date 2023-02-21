#pragma once
#include "Grid\\GridCtrl.h"
#include "Grid\\CellRange.h"


// CDlgDefectMap 대화 상자입니다.

struct STRU_DEFECT_MAP_INFO
{
	CString strCurPanelID;
	CString strCurInspDate;
	CString strBefPanelID;
	CString strBefInspDate;

	RECT* rcDefect;

	void init(int nDataBuffer)
	{
		strCurPanelID		= _T("");
		strCurInspDate		= _T("");

		rcDefect = new RECT[nDataBuffer+1];
		memset(rcDefect,0X00,sizeof(rcDefect));
	}
	void SaveCompData()
	{
		strBefPanelID	=	strCurPanelID;
		strBefInspDate	=	strCurInspDate;
	}
	void Delete()
	{
		delete [] rcDefect;
	}
};

class CDlgDefectMap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectMap)

public:
	CDlgDefectMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDefectMap();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	STRU_DEFECT_MAP_INFO	m_stDefMapData;
	void m_fnDrawFrame(CDC* pDC);
	void m_fnDrawPanel(CDC* pDC);
	int m_fnDrawDefect(CDC* pDC, BOOL bSave);
	void m_fnDrawAlignMark(CDC* pDC, int nPosX, int nPosY);
	void m_fnDrawCrossLine(CDC* pDC, int nIndex);

	void m_fnMakeDefectMap(CDC* pDC, BOOL bSave);
 	int m_fnLoadDefectMap(CDC* pDC);	

	BITMAP m_bm;
	CBitmap m_bmpPaint;
	BITMAPINFOHEADER m_bi;
	CDC m_dcMem;		

	int m_nDefectIndex;
	BOOL m_bMouseMapClk;
	
	int m_nPanelSizeX;
	int m_nPanelSizeY;

	float m_fRatioCoordX;
	float m_fRatioCoordY;
	float m_fRatioPanelXY;
	float m_fAlignPositionX;
	float m_fAlignPositionY;
	RECT m_rcPanel;

	//CGridCtrl m_Grid;
	CView* m_pViewAuto;

	void m_fnSetPanelSize(int nSizeX, int nSizeY);
	void m_fnCalcRatioCoordXY(int nSizeX, int nSizeY, int nEqpSizeType);	//2016.02.24 Mr.Chu

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void m_fnDrawPoint(CDC* pDC,int nDefectCnt);
	CPoint m_point;
	BOOL m_bOpenDefectMap;
	int TriggerDefectMap(BOOL bSave);	//KYH01	//2018.12.19 change by mby
	CDC m_dcMem2;				//KYH01
	CRITICAL_SECTION m_csMapPaint;	//2018.12.16 add by mby for Debug window
	CRITICAL_SECTION m_csMapDrawDef;	//2018.12.16 add by mby for Debug window
private:
	BOOL m_bMapAxisObject;		//Defect Map의 Axis 구분 변수
};

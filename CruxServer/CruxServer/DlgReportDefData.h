#pragma once
#include <vector>

// CDlgDefectMap 대화 상자입니다.

#define D_TEXT_DIS_LINE 4
enum 
{
	ENUM_ENUM_PANEL_ID	=0	,
	ENUM_INSPECT_TIME		,
	ENUM_TOTAL_GRADE		,
	ENUM_TOTAL_DEFECT		,
	ENUM_CF_GRADE			,
	ENUM_CF_DEFECT			,
	ENUM_TFT_GRADE			,
	ENUM_TFT_DEFECT			,
	ENUM_POL_GRADE			,
	ENUM_POL_DEFECT			,
	ENUM_DUST				,
	ENUM_WHITE_TURBITY		,
	ENUM_ITO				,
	ENUM_EDGE_BROKEN		,
	ENUM_PAD_SCRATCH		,
	ENUM_DIMPLE_POINT		,
	ENUM_ETCHING_LINE		,
	ENUM_DIMPLE_LINE		,
	ENUM_PAD_BURNT			,
	ENUM_BURR				,
	ENUM_POL_BUBBLE			,
	ENUM_POL_SCRATCH		,
	ENUM_POL_ALIEN_SUB		,
	ENUM_POL_MINOR			,
	ENUM_BLACK_SCRATCH		,
	ENUM_WHITE_SCRATCH		,
	ENUM_CF_MEASURE			,
	ENUM_TFT_MEASURE		,
	ENUM_MURA				,
	ENUM_CF_BURR			,
	ENUM_TFT_BURR			,
	ENUM_CF_BROKEN			,
	ENUM_TFT_BROKEN			,
	ENUM_CF_CORNER_BROKEN	,
	ENUM_TFT_CORNER_BROKEN	,
	ENUM_CF_CELL_BROKEN		,
	ENUM_TFT_CELL_BROKEN	,
	ENUM_CF_PIMPLE_POINT	,
	ENUM_TFT_PIMPLE_POINT	,
	ENUM_CF_PIMPLE_LINE		,
	ENUM_TFT_PIMPLE_LINE	,
	ENUM_CF_PIMPLE_CIRCLE	,
	ENUM_CF_WAVE_PATTERN	,
	ENUM_ETC_POINT			,
	ENUM_ETC_LINE			,
	ENUM_QMS_FILE_PATH		,
	ENUM_DEFECT_CODE_NUM

};

class CDlgReportDefData : public CDialogEx 
{
	DECLARE_DYNAMIC(CDlgReportDefData)

public:
	CDlgReportDefData(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgReportDefData();
	HANDLE			m_H_DefData;
	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_REPORT_DEF_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	DECLARE_MESSAGE_MAP()

private:
	CEdit **mp_edit;
	CEdit **mp_DefLbl;
	CEdit **mp_DefPer;
	int m_row, m_column;
	CFont m_font;
	CFont m_Bigfont;
public:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	DECLARE_EVENTSINK_MAP()

public: 
	CString m_cPanelID		;
	CString m_cGrade		;
	CString m_cCFResult		;
	CString m_cTftResult	;
	CString m_cPenetResult	;
//	CDlgMessage					   *m_pDlgMessage;
	//2015.09.11 Mr.Chu
	CString m_cDefCode;
public:
	LRESULT m_fnUpdateStatsData(WPARAM wParam, LPARAM lParam);
	LRESULT m_fnInitState(WPARAM wParam, LPARAM lParam);
	int		m_nfUpdateEdit(vector<CStringArray*>* listList);
	int		m_fnUpdateLabel(CStringArray* cArryLabel);
	void	m_fnCreateFont();
	int     m_fnSortOrder(CStringArray &cArryLabel, CStringArray &cArryData);
};

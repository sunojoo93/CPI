#pragma once
#include ".\Grid\GridCtrl.h"
#include "afxwin.h"
#include "resource.h"
#include "DlgPopUpImage.h"
// CDlgMessage 대화 상자입니다.


enum
{
	MSG_COORD_START_X = 0,
	MSG_COORD_START_Y,
	MSG_COORD_END_X,
	MSG_COORD_END_Y,
	MSG_COORD_TYPE,
	MSG_COORD_IMG_PATH,
	MSG_MAX_COUNT

};

enum
{
	MSG_DEFTYPE_PANELID = 0,
	MSG_DEFTYPE_TYPE,
	MSG_DEFTYPE_COUNT,
	MSG_DEFTYPE_TERM,
	MSG_DEFTYPE_COORD_START_X ,
	MSG_DEFTYPE_COORD_START_Y,
	MSG_DEFTYPE_COORD_END_X,
	MSG_DEFTYPE_COORD_END_Y,
	MSG_DEFTYPE_IMG_PATH,
	MSG_DEFTYPE_STOP_COUNT,
	MSG_DEF_TYPE_MAX_COUNT

};

class CDlgMessage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMessage)

public:
	CDlgMessage(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMessage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	
	CGridCtrl m_GridOverlapDefect;
	//CString *m_strImagPath;
	void m_fnInitGridOverlapDefect(int nOverlapCnt);
	void m_fnInitGridOverlapDefTypeCol(int nOverlapCnt);
	void m_fnInitGridOverlapContactDefect();
	void m_fnInitGridOverlapMeasureDefect();
	void m_fnInitGridOverlapLuminanceDefect();	//2016.03.02
	void m_fnUpdateOverlapDefect(CString strOverlap, int nOverlapCnt);
	void m_fnUpdateOverlapDefType(CString strOverlap, int nOverlapCnt);
	void m_fnUpdateOverlapContactDefect(CString strOverlap);
	void m_fnUpdateOverlapMeasureDefect(CString strOverlap);
	void m_fnUpdateOverlapLuminanceDefect(CString strOverlap);	//2016.03.02
	void m_fnChangeCaption(CString strCaption);
	void m_fnUpdatePanelListAlarm(CString strMsg);
	void m_fnInitGridColData(CStringArray &strAry);
	afx_msg LRESULT m_fnAlarmSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnMeasureAlarmSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnContactAlarmSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnListlAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnOverlapDefTypeAlarmSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnLuminanceAlarmSet(WPARAM wParam, LPARAM lParam);	//2016.03.02
	afx_msg void OnGridDBClickOverlapList(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	void m_fnPopUpDefectImage();
	CDlgPopUpImage* m_pDlgPopUpImage;
	int m_ngridDetectFileRow;
	int m_ngridDetectFileCol;
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);  

};
//2016.01.29 Mr.Chu       Msg Display Class
class CDlgDisplayMessge : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDisplayMessge)

public:
	CDlgDisplayMessge(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDlgDisplayMessge(BOOL bDlgAutoRecipe);
	virtual ~CDlgDisplayMessge();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DISPLAY_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CFont m_font;
	CBrush m_edtBgColor;
	BOOL  m_bAutoRecipeUse;
	afx_msg void OnBnClickedOk();
	void m_fnSetText(CString strMsgA, CString strMsgB);
	void m_fnAutoRecipeDesign();
//	CStatic m_d;
//	CString m_strStaticText;
	CString m_strText;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonRetry();
	afx_msg void OnBnClickedButtonClose();
	CButton m_btnRetry;
	CButton m_btnClose;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
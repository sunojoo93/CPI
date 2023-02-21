#pragma once

#include <vector>
#include ".\Grid\GridCtrl.h"
#include "afxwin.h"
#include "Structure.h"

using namespace std;
// CDlgManualJudge 대화 상자입니다.

class CDlgManualJudge : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgManualJudge)

public:
	CDlgManualJudge(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgManualJudge();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MANUAL_JUDGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	std::vector<STRU_DEFECT_INFO *> m_vectorManualJudgeDefectInfo;
	LPPICTURE mp_image_object;
	long m_width_size, m_height_size;
	CString m_strPanelID;
	CString m_strManualJudgeDefectFile;
	CString m_strQmsDefectFile;

public:
	int m_nGridManualJudgeDetectFileRow;
	int m_nGridManualJudgeDetectFileCol;
	CGridCtrl* m_GridManualJudgeDefectSummary;

	UINT GetManualJudgeDefectCount();
	STRU_DEFECT_INFO *GetManualJudgeDefectInfo(int nIndex);
	STRU_DEFECT_INFO *pManualJudgeDetectInfo;

	BOOL LoadImage(CString strImgPath);
	void ShowImage();
	void OnDestroy();
	void m_fnInitGridManualJudgeDefectSummary();
	void m_fnClearManualJudgeDefectList();
	void m_fnUpdateSelManualJudgeDefectList(CString strPath);
	void UpdateGridManualJudgeDefectSummaryRow(CString strPath);
	void UpdateGridManualJudgeDefectSummaryText();

	afx_msg void OnGridDBClickManualJudgeDetectList(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg LRESULT m_fnManualJudgeSet(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnNg();
	afx_msg void OnBnClickedImagePath();
	afx_msg void OnBnClickedBtnNgb();
};

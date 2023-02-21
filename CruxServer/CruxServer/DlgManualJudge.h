#pragma once

#include <vector>
#include ".\Grid\GridCtrl.h"
#include "afxwin.h"
#include "Structure.h"

using namespace std;
// CDlgManualJudge ��ȭ �����Դϴ�.

class CDlgManualJudge : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgManualJudge)

public:
	CDlgManualJudge(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgManualJudge();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_MANUAL_JUDGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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

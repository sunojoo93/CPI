


// 판정 항목 등록 관련 UI.
// 기능				: Set Up UI 내 판정항목들의 우선순위, 확인유무 등 을 설정하는 UI
// 항목 추가 방법	: CPP 파일 내	
//					  m_fnIniListCheck() 함수		- 우선순위 등록
//					  m_fnJudgeListDataTerminal		- 항목 Load 및 데이터 Frame 저장
//					  내 추가할 항목에 대한 것을 작성한다.

#pragma once
#include "AdvListCtrl.h"		//2016.12.12
#include "DataManager.h"
#include "Structure.h"

// CDlgSetJudgeItem 대화 상자입니다.

class CDlgSetJudgeItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetJudgeItem)

public:
	CDlgSetJudgeItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSetJudgeItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SET_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CDataManager* m_pDataManager;

	afx_msg void OnPaint();
	void m_fnInitDialog(void);

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnClickListSet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();

private:	// 변수는 가급적 private으로 선언
	int m_nEqpModel;
	int m_nEqpSizeType;
	CFont m_font; 
	CAdvListCtrl m_ctrlListSet;			//2016.12.12
	int m_nListCurIndex;				//2016.12.12
	CComboBox* m_pListCombo;			//2016.12.12

public:
	virtual BOOL OnInitDialog();

	void m_fnInitJudgeItemList();							//2016.12.12	Setting List Box 초기화

	void m_fnIniListCheck();
	void m_fnJudgeListDataTerminal(int nCompItemIndex, int nFunc);					//2016.12.12	List Box의 데이터 교류 기능
	BOOL m_fnDataTerminal(BOOL bCheck, int nIndex, CString strDefName, int strDefCode,
							CString strCompData, int nFunc, CString *strArrSetItem,
							int &nJudgeItem, int &nBypassInfo, int &nPriority
							);

	afx_msg void OnBnClickedButtonAllCheck();
	afx_msg void OnBnClickedButtonNotCheck();
};


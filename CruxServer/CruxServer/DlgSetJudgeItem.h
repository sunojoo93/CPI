


// ���� �׸� ��� ���� UI.
// ���				: Set Up UI �� �����׸���� �켱����, Ȯ������ �� �� �����ϴ� UI
// �׸� �߰� ���	: CPP ���� ��	
//					  m_fnIniListCheck() �Լ�		- �켱���� ���
//					  m_fnJudgeListDataTerminal		- �׸� Load �� ������ Frame ����
//					  �� �߰��� �׸� ���� ���� �ۼ��Ѵ�.

#pragma once
#include "AdvListCtrl.h"		//2016.12.12
#include "DataManager.h"
#include "Structure.h"

// CDlgSetJudgeItem ��ȭ �����Դϴ�.

class CDlgSetJudgeItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetJudgeItem)

public:
	CDlgSetJudgeItem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSetJudgeItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_SET_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CDataManager* m_pDataManager;

	afx_msg void OnPaint();
	void m_fnInitDialog(void);

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnClickListSet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();

private:	// ������ ������ private���� ����
	int m_nEqpModel;
	int m_nEqpSizeType;
	CFont m_font; 
	CAdvListCtrl m_ctrlListSet;			//2016.12.12
	int m_nListCurIndex;				//2016.12.12
	CComboBox* m_pListCombo;			//2016.12.12

public:
	virtual BOOL OnInitDialog();

	void m_fnInitJudgeItemList();							//2016.12.12	Setting List Box �ʱ�ȭ

	void m_fnIniListCheck();
	void m_fnJudgeListDataTerminal(int nCompItemIndex, int nFunc);					//2016.12.12	List Box�� ������ ���� ���
	BOOL m_fnDataTerminal(BOOL bCheck, int nIndex, CString strDefName, int strDefCode,
							CString strCompData, int nFunc, CString *strArrSetItem,
							int &nJudgeItem, int &nBypassInfo, int &nPriority
							);

	afx_msg void OnBnClickedButtonAllCheck();
	afx_msg void OnBnClickedButtonNotCheck();
};


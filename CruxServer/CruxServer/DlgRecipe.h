
#pragma once
#include "AdvListCtrl.h"
#include "Login.h"
// DlgRecipe 대화 상자입니다.

class CDlgRecipe : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRecipe)

public:
	CDlgRecipe(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgRecipe();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_RECIPE };
	BOOL				m_fnChangePLCRecipe();
private:
	//CListCtrl			m_ListRecipeTotal;
	CAdvListCtrl		m_ListRecipeTotal;
	CListCtrl			m_ListRecipe;
	//CAdvListCtrl		m_ListRecipe;
	int					m_nListRecipeCurSel;
	int					m_CurRows;
	CComboBox			m_PlcRecipeListComboCtrl;
	CComboBox			m_PC1RecipeListComboCtrl;
	CComboBox			m_PC2RecipeListComboCtrl;
	CComboBox			m_PC3RecipeListComboCtrl;
	CLogin*				m_pDlgLogin;

public:
	virtual BOOL		OnInitDialog();

	STRU_RECIPE_LIST_INFO *m_stRecipeListInfo;

	void				m_fnRecipe_Init();
	void				m_fnCheckContents();	//2016.02.16 Mr.Chu		Client Recipe 미등록되어있는 것에 관한 불합리 검사
	void				m_fnRefreshRecipeTotal();
	void				m_fnSetRecipeComboBox(int nFlag);
	void				m_fnRecipeSelView(int nCurSel);
	void				m_fnSelectRecipeVision(int nVision, CString strRecipe);
	BOOL				m_fnSelectRecipePLC(int nPLCModeleNo, int nCIMRecipeNo);
	void				m_fnSetDefectMap();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedListRecipeTotal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnCopy();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnSelRecipe();
	afx_msg void OnCbnSelchangeComboPlc();
	afx_msg void OnCbnSelchangeComboPc1();
	afx_msg void OnCbnSelchangeComboPc2();
	afx_msg void OnCbnSelchangeComboPc3();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnCbnDropdownComboPlc();
	CComboBox m_PC4RecipeListComboCtrl;
	CComboBox m_PC5RecipeListComboCtrl;
	CComboBox m_PC6RecipeListComboCtrl;
	CComboBox m_PC7RecipeListComboCtrl;
	CComboBox m_PC8RecipeListComboCtrl;
	CComboBox m_PC9RecipeListComboCtrl;
	afx_msg void OnCbnSelchangeComboPc4();
	afx_msg void OnCbnSelchangeComboPc5();
	afx_msg void OnCbnSelchangeComboPc6();
	afx_msg void OnCbnSelchangeComboPc7();
	afx_msg void OnCbnSelchangeComboPc8();
	afx_msg void OnCbnSelchangeComboPc9();
	afx_msg void OnBnClickedDefectMapOriginType(UINT ID);	//2016.03.14 Mr.Chu
	afx_msg void OnBnClickedDefectMapMajorAxisType(UINT ID);	//2016.03.14 Mr.Chu
	afx_msg void OnBnClickedDefectMapReportCoordinateQMSType(UINT ID);	//2016.12.30 Mr.Chu
	void m_fnInitializeClientUi(int nEqpNo,int nEqpSizeType);
	int m_nDefectMapOriginType;	//2016.03.14 Mr.Chu
	CString m_strDefMapY;		//2016.03.14 Mr.Chu		Edit Box
	CString m_strDefMapX;		//2016.03.14 Mr.Chu		Edit Box
	CString m_strRcpAlignPcNo;	//2021.07.07 add by mby
	afx_msg void OnBnClickedButtonRecipeMapSize();
protected:
	afx_msg LRESULT OnAutoRecipe(WPARAM wParam, LPARAM lParam);
private:
	BOOL m_fnLogin();
public:
	afx_msg void OnBnClickedBtnPlcModelRead();
	afx_msg void OnBnClickedBtnPlcModelNameChange();
	afx_msg void OnBnClickedBtnPlcModelCopy();
	afx_msg void OnBnClickedBtnPlcModelDelete();
	afx_msg void OnBnClickedButtonAlignpcNoSet(); //2021.07.07 add by mby
};

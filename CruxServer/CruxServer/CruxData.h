//
// Crux GUI의 각종 Data를 보관/관리하는 Class로, Singleton으로 사용된다.
//
//

#pragma once

#include "CruxDefine.h"


class CCruxData
{
private:
	CCruxData(void);								// 중복 선언 방지
	~CCruxData(void);
    static CCruxData* m_Instance;					// Singleton 객체
public:
    static CCruxData* m_fnGetInstance();				// Singleton
	static void m_fnReleaseInstance();					// Singleton 삭제
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

private:

	// 화면에 표시되는 View 및 Dialog 관련
	CstViewInfo			m_listViewData;				// 화면 FormView 정보를 담아두고 있는 List
	CstDialogInfo		m_listDialogData;			// 화면 Dialog 정보를 담아두고 있는 List
	UINT				m_nCurrentViewID;			// 현재 화면에 보여지고 있는 View의 ID

	CString	m_sRootPath;							// 실행시 기본이 되는 경로
	CString	m_sConfigFile;							// 설정값이 들어가 있는 파일
	CString	m_sRecipeFile;							// 모델별 레서피가 저장된 파일

public :
	void	m_fnSetRootPath(CString sPath);				//  기본경로 설정
	CString	m_fnGetRootPath(void);						// 기본 경로 조회

	void	m_fnSetRecipeFile(CString sFile);			// 레서피 파일을 지정한다.
	CString	m_fnGetRecipeFile(void);					// 레서피 파일을 조회한다.
	CString	m_fnGetConfigFile(void);					// 설정파일을 조회한다.

public:

	// Form View 관련 함수 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	CString m_fnGetViewCaption(UINT nID);				// 지정 View의 Caption을 조회한다.
	CString m_fnGetCurrentViewCaption(void);			// 현재 보여지고있는 View의 Caption
	CView* m_fnGetViewPtr(UINT nID);					// 지정 View의 포인터를 조회한다.

	void m_fnAddView(UINT nID, CView* pView, CString sCaption, BOOL bNewFlag = TRUE);			// View를 추가한다.
	void m_fnRemoveView(void);							// 모든 View를 삭제한다. (종료시)
	void m_fnSetChangeView(UINT nID);					// 지정 View로 화면을 변경한다.
	void m_fnSetChangeView(CView* pView);				// 지정 View로 화면을 변경한다.
	void m_fnInitialView(void);							// View에서 Initial을 수행 할 수 있도록 조치한다.
	
	// Dialog 관련 함수 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
	void m_fnAddDialog(UINT nIndex, UINT nID, CDialog* pDialog, CString sCaption, BOOL bModal);			// Dialog를 추가한다.
	CDialog* m_fnGetDialogPtr(UINT nIndex);				// 지정 Index의 Dialog 포인터를 조회한다.
	void m_fnRemoveDialog(void);						// 모든 Dialog를 삭제한다. (종료시)
	int m_fnShowDialog(UINT nIndex);					// 지정 Dialog를 화면에 보여준다.
	int m_fnGetDialogIndex(UINT nIndex);				// 지정 Index에 해당되는 Dialog의 배열 접근용 Index를 반환한다.
};


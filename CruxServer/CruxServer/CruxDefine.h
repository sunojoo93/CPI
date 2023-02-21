////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Project 전반에서 사용되어이지는 각종 정의 내용 
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#ifndef _Crux_Define_H_
#define _Crux_Define_H_

#include "Lib\GxButtonEx.h"
#include "Lib\GxStaticEx.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
// 사용자 정의 Messge 정의
#define UM_MESSAGE_BASE				(WM_USER + 1000)			// 이 프로그램에서 사용할 사용자정의 메세지 Base
#define UM_MAINMENU_CHANGE			(UM_MESSAGE_BASE + 1)	// 화면 전환 요청
#define UM_SUBMENU_CHANGE			(UM_MESSAGE_BASE + 2)	// 하위 메뉴 전환 요청
#define UM_GUI_UPDATE_TIMER		(UM_MESSAGE_BASE + 3)	// 화면 갱신용 타이머의 구동 및 정지 지정
#define UM_INITIAL_DATAUPDATE		(UM_MESSAGE_BASE + 4)	// View 화면을 초기화하고 사용을 활성화 한다.
#define UM_UPDATE_PROGRESS			(UM_MESSAGE_BASE + 5)	// 진행되는 Progress를 변경시킨다.
#define UM_UPDATE_RESULT			(UM_MESSAGE_BASE + 6)	// 결과를 전송, Initial 결과
#define UM_UPDATE_EVENT				(UM_MESSAGE_BASE + 7)	// 각종 Event를 처리한다.
#define UM_MODEL_CHANGE				(UM_MESSAGE_BASE + 8)	// Model 변경시 Model명 처리
#define UM_INIT_CAM					(UM_MESSAGE_BASE + 9)	// Camera 초기화 완료 처리 jskim
#define UM_GRAB_TIME_VALUE			(UM_MESSAGE_BASE + 10)	// GrabTime 전송 jskim
#define UM_CLASS_TIME_VALUE			(UM_MESSAGE_BASE + 11)	// ClassTime 전송 jskim
#define UM_PROGRESSBAR_VALUE		(UM_MESSAGE_BASE + 12)	// Progress 전송 jskim

#define UM_SELECT_ENGINEER_MODE		(UM_MESSAGE_BASE + 13)	// Eng'r/Skip 선택
#define WM_AUTO_RECIPE				(UM_MESSAGE_BASE + 14)//Auto Recipe


////////////////////////////////////////////////////////////////////////////////////////////////////
enum {		// 화면 구성

	// Main FormView
	eFORM_Main			= 0,			// Main 화면, 사용하지 않는다.
	eFORM_Auto,							// Auto 화면
	eFORM_Manual,						// Manual 화면
	eFORM_Report,						// 통계 (Report) 화면
	eFORM_Setup,						// Setup 화면
	eFORM_IOView,						// I/O View 화면
	eMAX_FORMCount,	// Form의 수량---------------------------
}; //of 화면 


enum {		
		// Timer index
		eTIMER_Startup		= 0,
		eTIMER_Shutdown		= 1,
		eTIMER_Monitoring	= 2,

		// Timer Interval
		eINTERVAL_Startup		= 200,		// 초기시작 처리 Timer의 interval은 200msec
		eINTERVAL_Shutdown		= 200,		// 최종 종료 처리 Timer의 interval은 200msec
		eINTERVAL_Mointor		= 500,		// 상태 모니터링 Timer의 interval은 500msec
};//of enum


enum {			// 각종 상태

	// 초기화 진행 상태
	eSTATE_InitReady	= 0,				// 초기화 대기중
	eSTATE_InitStart,						// 초기화 시작
	eSTATE_InitOK,							// 초기화 성공
	eSTATE_InitFail,						// 초기화 실패
};

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
// 각종 Color 정의 
#define GXCOLOR_SILVER			RGB(224, 224, 224)							// 선택되지 않은 버튼 색, 은색
#define GXCOLOR_LIME				RGB(191, 255, 0)								// 선택된 버튼 색, Lime
#define GXCOLOR_BLACK			RGB(0, 0, 0)									// 글자색, 검은색
#define GXCOLOR_BLUE				RGB(0, 0, 255)									// 글자색 파란색
#define GXCOLOR_RED				RGB(255, 0, 0)									// 글자색 파란색


#define GXCOLOR_INIT_READY		RGB(224, 224, 224)			// 초기화 진행 전
#define GXCOLOR_INIT_RUN		RGB(192, 192, 255)			// 초기화 진행 중
#define GXCOLOR_INIT_OK			RGB(192, 255, 192)			// 초기화 진행 성공
#define GXCOLOR_INIT_FAIL		RGB(255, 192, 192)			// 초기화 진행 실패


////////////////////////////////////////////////////////////////////////////////////////////////////
// 각종 구조체 정의 

// Main 화면 View 정보를 담아두는 구조체
typedef struct
{
	UINT		nID;					// View의 ID
	CString	sCaption;			// View의 제목/이름
	CView*	pView;				// View의 포인터
	BOOL		bCreate;				// 직접 Create해줄것인가 ?

} stViewInfo;
typedef CArray<stViewInfo, stViewInfo&> CstViewInfo;


// Dialog 화면 정보를 담아두는 구조체
typedef struct
{
	UINT		nIndex;				// Index
	UINT		nID;					// Dialog의 Resouce ID
	BOOL		bModal;				// Modal Dialog인가 ?

	CDialog*	pDialog;				// Dialog의 포인터
	CString	sCaption;			// Dialog의 제목/이름

} stDialogInfo;
typedef CArray<stDialogInfo, stDialogInfo&> CstDialogInfo;


// GxOCX Button과 Static 저장 CArray 형식
typedef CArray<CGxButtonEx*, CGxButtonEx*> CButtonExArray;
typedef CArray<CGxStaticEx*, CGxStaticEx*> CStaticExArray;

// Resource ID 저장 CArray
typedef CArray<UINT, UINT> CResourceIDArray;

//
// 초기화 상태값
// static CString STATES[4] = { "...Waiting", "...Success", "...Fai_T(", "...Running" };




////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEBUG_BUF_LEN 512

//#ifdef _DEBUG
//void _cdecl Debug(LPCTSTR str, ...)
//{
//   TCHAR   szBuffer[DEBUG_BUF_LEN+1];
//   va_list args;
//   va_start(args, str);
//   _vsntprintf_s(szBuffer, DEBUG_BUF_LEN, str, args);
//   va_end(args);
//   // ....
//}
//#else
//#define Debug
//#endif


#endif	//of #ifndef _Crux_Define_H_

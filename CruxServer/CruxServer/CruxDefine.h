////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Project ���ݿ��� ���Ǿ������� ���� ���� ���� 
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#ifndef _Crux_Define_H_
#define _Crux_Define_H_

#include "Lib\GxButtonEx.h"
#include "Lib\GxStaticEx.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
// ����� ���� Messge ����
#define UM_MESSAGE_BASE				(WM_USER + 1000)			// �� ���α׷����� ����� ��������� �޼��� Base
#define UM_MAINMENU_CHANGE			(UM_MESSAGE_BASE + 1)	// ȭ�� ��ȯ ��û
#define UM_SUBMENU_CHANGE			(UM_MESSAGE_BASE + 2)	// ���� �޴� ��ȯ ��û
#define UM_GUI_UPDATE_TIMER		(UM_MESSAGE_BASE + 3)	// ȭ�� ���ſ� Ÿ�̸��� ���� �� ���� ����
#define UM_INITIAL_DATAUPDATE		(UM_MESSAGE_BASE + 4)	// View ȭ���� �ʱ�ȭ�ϰ� ����� Ȱ��ȭ �Ѵ�.
#define UM_UPDATE_PROGRESS			(UM_MESSAGE_BASE + 5)	// ����Ǵ� Progress�� �����Ų��.
#define UM_UPDATE_RESULT			(UM_MESSAGE_BASE + 6)	// ����� ����, Initial ���
#define UM_UPDATE_EVENT				(UM_MESSAGE_BASE + 7)	// ���� Event�� ó���Ѵ�.
#define UM_MODEL_CHANGE				(UM_MESSAGE_BASE + 8)	// Model ����� Model�� ó��
#define UM_INIT_CAM					(UM_MESSAGE_BASE + 9)	// Camera �ʱ�ȭ �Ϸ� ó�� jskim
#define UM_GRAB_TIME_VALUE			(UM_MESSAGE_BASE + 10)	// GrabTime ���� jskim
#define UM_CLASS_TIME_VALUE			(UM_MESSAGE_BASE + 11)	// ClassTime ���� jskim
#define UM_PROGRESSBAR_VALUE		(UM_MESSAGE_BASE + 12)	// Progress ���� jskim

#define UM_SELECT_ENGINEER_MODE		(UM_MESSAGE_BASE + 13)	// Eng'r/Skip ����
#define WM_AUTO_RECIPE				(UM_MESSAGE_BASE + 14)//Auto Recipe


////////////////////////////////////////////////////////////////////////////////////////////////////
enum {		// ȭ�� ����

	// Main FormView
	eFORM_Main			= 0,			// Main ȭ��, ������� �ʴ´�.
	eFORM_Auto,							// Auto ȭ��
	eFORM_Manual,						// Manual ȭ��
	eFORM_Report,						// ��� (Report) ȭ��
	eFORM_Setup,						// Setup ȭ��
	eFORM_IOView,						// I/O View ȭ��
	eMAX_FORMCount,	// Form�� ����---------------------------
}; //of ȭ�� 


enum {		
		// Timer index
		eTIMER_Startup		= 0,
		eTIMER_Shutdown		= 1,
		eTIMER_Monitoring	= 2,

		// Timer Interval
		eINTERVAL_Startup		= 200,		// �ʱ���� ó�� Timer�� interval�� 200msec
		eINTERVAL_Shutdown		= 200,		// ���� ���� ó�� Timer�� interval�� 200msec
		eINTERVAL_Mointor		= 500,		// ���� ����͸� Timer�� interval�� 500msec
};//of enum


enum {			// ���� ����

	// �ʱ�ȭ ���� ����
	eSTATE_InitReady	= 0,				// �ʱ�ȭ �����
	eSTATE_InitStart,						// �ʱ�ȭ ����
	eSTATE_InitOK,							// �ʱ�ȭ ����
	eSTATE_InitFail,						// �ʱ�ȭ ����
};

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
// ���� Color ���� 
#define GXCOLOR_SILVER			RGB(224, 224, 224)							// ���õ��� ���� ��ư ��, ����
#define GXCOLOR_LIME				RGB(191, 255, 0)								// ���õ� ��ư ��, Lime
#define GXCOLOR_BLACK			RGB(0, 0, 0)									// ���ڻ�, ������
#define GXCOLOR_BLUE				RGB(0, 0, 255)									// ���ڻ� �Ķ���
#define GXCOLOR_RED				RGB(255, 0, 0)									// ���ڻ� �Ķ���


#define GXCOLOR_INIT_READY		RGB(224, 224, 224)			// �ʱ�ȭ ���� ��
#define GXCOLOR_INIT_RUN		RGB(192, 192, 255)			// �ʱ�ȭ ���� ��
#define GXCOLOR_INIT_OK			RGB(192, 255, 192)			// �ʱ�ȭ ���� ����
#define GXCOLOR_INIT_FAIL		RGB(255, 192, 192)			// �ʱ�ȭ ���� ����


////////////////////////////////////////////////////////////////////////////////////////////////////
// ���� ����ü ���� 

// Main ȭ�� View ������ ��Ƶδ� ����ü
typedef struct
{
	UINT		nID;					// View�� ID
	CString	sCaption;			// View�� ����/�̸�
	CView*	pView;				// View�� ������
	BOOL		bCreate;				// ���� Create���ٰ��ΰ� ?

} stViewInfo;
typedef CArray<stViewInfo, stViewInfo&> CstViewInfo;


// Dialog ȭ�� ������ ��Ƶδ� ����ü
typedef struct
{
	UINT		nIndex;				// Index
	UINT		nID;					// Dialog�� Resouce ID
	BOOL		bModal;				// Modal Dialog�ΰ� ?

	CDialog*	pDialog;				// Dialog�� ������
	CString	sCaption;			// Dialog�� ����/�̸�

} stDialogInfo;
typedef CArray<stDialogInfo, stDialogInfo&> CstDialogInfo;


// GxOCX Button�� Static ���� CArray ����
typedef CArray<CGxButtonEx*, CGxButtonEx*> CButtonExArray;
typedef CArray<CGxStaticEx*, CGxStaticEx*> CStaticExArray;

// Resource ID ���� CArray
typedef CArray<UINT, UINT> CResourceIDArray;

//
// �ʱ�ȭ ���°�
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

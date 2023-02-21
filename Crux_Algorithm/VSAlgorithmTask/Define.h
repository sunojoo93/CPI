#ifndef DEFINE_H
#define DEFINE_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define DLLAPI __declspec(dllimport)

// 현재 솔루션 실행 드라이브 판단 추가
#define VS_ALGORITHM_TASK_INI_FILE			theApp.m_Config.GETDRV() + _T(":\\CRUX\\Data\\INI\\Crux_Algorithm.ini")
#define ALGORITHM_LOG_PATH					theApp.m_Config.GETDRV() + _T(":\\Crux\\DATA\\LOG\\Algorithm\\")
#define DEVICE_FILE_PATH					theApp.m_Config.GETDRV() + _T(":\\Crux\\DATA\\INI\\Device.cfg")
#define INIT_FILE_PATH						theApp.m_Config.GETDRV() + _T(":\\Crux\\DATA\\INI\\Initialize.ini")
#define MODEL_FILE_PATH						theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\MODEL")
#define CCD_DEFECT_FILE_PATH				theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\CCD\\CCD.Index")
#define CCD_DEFECT_FILE_PATH2				theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\CCD\\DEAD_CCD.Index")
#define COLOR_CORRECTION_FILE_PATH			theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\CCD\\ColorCorrection.ini")
#define ALIGN_IMAGE_PATH					theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\IMAGE\\Align.bmp")
#define DEFECT_INFO_PATH					theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\Defect Info")
#define REPEAT_DEFECT_PIXEL_INFO_PATH		theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\Defect Info\\CountingCCDDefect.txt")
#define REPEAT_DEFECT_WORK_INFO_PATH		theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\Defect Info\\CountingWorkDefect.txt")
#define REPEAT_DEFECT_MACHINE_INFO_PATH		theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\Defect Info\\CountingMachineDefect.txt")	// 2018.09.21 MDJ APP Repeat Defect
#define REPEAT_DEFECT_MACHINE_INFO_PATH2	theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\Defect Info\\CountingMachineDefect")		// 2018.10.09 MDJ Test용 파일 생성

//////// 2018.01.18 sgkim 수정 GetCurrentDrive() -> strDrive - ThreadParameter로 현재 드라이브 받아서 경로 설정
#define ORIGIN_PATH							theApp.m_Config.GetNetworkDrivePath() + strDrive + theApp.m_Config.GetOriginPath()
#define RESULT_PATH							theApp.m_Config.GetNetworkDrivePath() + strDrive + theApp.m_Config.GetResultPath()
#define INSP_PATH							theApp.m_Config.GetNetworkDrivePath() + strDrive + theApp.m_Config.GetInspPath()
#define ALG_RESULT_PATH						theApp.m_Config.GetNetworkDrivePath() + strDrive + _T("ARESULT\\")
#define INSP_INFO_FILE_PATH					theApp.m_Config.GetNetworkDrivePath() + strDrive + _T("Defect Info\\Insp Data")
//////////

// MergeTool 
// 현재 솔루션 실행 드라이브 판단 추가
#define MERGETOOL_PATH						theApp.m_Config.GETDRV() + _T(":\\MergeTool")


#include "..\..\CommonHeaders\Structure.h"

//////////////////////////////////////////////////////////////////////////
// OpenCV 3.1
//////////////////////////////////////////////////////////////////////////
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\opencv.hpp>
#include <opencv2\core\cuda.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\cudafilters.hpp>
#include <opencv2\cudaimgproc.hpp>
#include <opencv2\cudaarithm.hpp>
#include <opencv2\cudabgsegm.hpp>
#include <opencv2\cudacodec.hpp>
#include <opencv2\cudafeatures2d.hpp>
#include <opencv2\cudaobjdetect.hpp>
#include <opencv2\cudawarping.hpp>


using namespace cv;
using namespace cv::ml;
using namespace cv::cuda;



// 전역으로 선언할 변수들은 여기에 Define 한다
// 알고리즘 파라메터가 바뀔경우 대응하기 위해 레서피 메모리 공간을 미리 큰 사이즈로 확보해 놓는다. 이하는 레서피 구조체 크기를 위한 상수
const int MAX_MEM_SIZE_CAM_COUNT				= 4;
const int MAX_MEM_SIZE_ROI_COUNT				= 1;
const int MAX_MEM_SIZE_E_INSPECT_AREA			= 30;		// 비검사 영역, 라운드셀 영역
const int MAX_MEM_SIZE_E_ALGORITHM_NUMBER		= 20;
const int MAX_MEM_SIZE_E_DEFECT_NAME_COUNT		= 140;		// KSW 18.05.31 ( 불량 개수 증가 )
const int MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT		= 200;		// Mura Normal Parameter 증가
const int MAX_MEM_SIZE_GRAB_STEP_COUNT			= 20;
const int MAX_MEM_SIZE_E_DEFECT_JUDGMENT_COUNT	= 60;
const int MAX_MEM_SIZE_E_MAX_INSP_TYPE			= 10;
const int MAX_MEM_SIZE_AD_PARA_TOTAL_COUNT		= 15;
const int MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT	= 200;		// pwj 20.09.10 ( 파라미터 개수 증가 )
const int MAX_MEM_SIZE_ROUND_COUNT				= 30000;	// KSW 17.07.20 [Round]

// 2015.08.06 Defect Filter ROI 추가
enum ENUM_ROI_DRAW_MODE
{
	eBasicROI	=	0,
	eFilterROI,
	eRndROI,
};

//2017.06.06 NDH : GUI와 순서가 같아야 한다.
enum ENUM_PAD
{
	E_PAD_LEFT		= 0	,
	E_PAD_RIGHT			,
	E_PAD_TOP 			,
	E_PAD_BOTTOM				
};


enum ENUM_PANEL_DIRECTION
{
	E_PANEL_LEFT	= 0	,
	E_PANEL_TOP			,
	E_PANEL_RIGHT		,
	E_PANEL_BOTTOM		,
	E_PANEL_DIRECTION_MAX
};

// [CInspectAlign] Corner
enum ENUM_CORNER
{
	E_CORNER_LEFT_TOP		= 0	,
	E_CORNER_RIGHT_TOP			,
	E_CORNER_RIGHT_BOTTOM		,
	E_CORNER_LEFT_BOTTOM		,
	E_CORNER_END
};

// 로그 종류
enum ENUM_KIND_OF_LOG
{
	eLOGCAM0 = 0,
	eLOGCAM1,
	eLOGCAM2,
	eLOGCAM3,
	eRESERVE0,		// Camera 개수 증가 가능
	eRESERVE1,
	eRESERVE2,
	eRESERVE3,
	eLOGPROC,
	eLOGTACT,
	eLOGCOMM
};

enum ENUM_INSPECT_MODE
{
	eAutoRun = 0,
	eManualGrabInspect,
	eManualInspect
};

enum ENUM_ABNORMAL_PAD_EDGE
{
	E_ABNORMAL_PAD_EDGE_TOTAL = 0,
	E_ABNORMAL_PAD_EDGE_TOP,
	E_ABNORMAL_PAD_EDGE_BOTTOM,
	E_ABNORMAL_PAD_EDGE_MIDDLE,
	E_ABNORMAL_PAD_EDGE

};

#define MAX_THREAD_COUNT	100									// 동시에 검사할 수 있는 스레드 개수

// for Tray Icon - User Message
#define	WM_TRAYICON_MSG					WM_USER + 1
#define	WM_PRINT_UI_LOG_MSG_UNICODE		WM_USER + 2
#define	WM_PRINT_UI_LOG_MSG_MULTI_BYTE	WM_USER + 3

#define MAX_GRID_LOG					1000

#define	WM_START_INSPECTION				WM_USER + 108
#define	WM_START_SAVE_IMAGE				WM_USER + 109

// enum ---------------------------------------------------------
// LOG LEVEL
enum ENUM_LOG_LEVEL
{
	eLOGLEVEL_DETAIL	= 1,
	eLOGLEVEL_BASIC,
	eLOGLEVEL_SIMPLE
};

#define ALG_LOG_LEVEL eLOGLEVEL_DETAIL

/// Alarm ID 정의
enum ENUM_ALARM_ID
{
	eALARMID_LIGHT_ABNORMAL		= 1000,
	eALARMID_DIABLE_CHECK_LIGHT,
	eALARMID_DUST_ABNORMAL,
	eALARMID_DIABLE_CHECK_DUST,

	eALARMID_ALIGN_ANGLE_ERROR	= 1100,
	eALARMID_ALIGN_ANGLE_WARNING,
	eALARMID_ALIGN_FAIL,

	eALARMID_CCD_DEFECT_ERROR	= 3000,
	eALARMID_WORK_DEFECT_ERROR,
	eALARMID_MACHINE_DEFECT_ERROR,				// 2018.09.25 MDJ APP Repeat Defect
	eALARMID_CCD_DEFECT_WARNING,
	eALARMID_WORK_DEFECT_WARNING,
	eALARMID_MACHINE_DEFCT_WARNING,				// 2018.09.25 MDJ APP Repeat Defect

	eALARMID_LCP_ALIGN_FAIL = 4000,
	eALARMID_LCP_STAGE_DUST,
	eALARMID_LCP_ALIGN_HOLE_FIND_FAIL,
	eALARMID_LCP_ALIGN_IMAGE_BUFFER_ERROR,
	eALARMID_LCP_ALIGN_BACK_MASK_FAIL,
	eALARMID_LCP_ALIGN_ACTIVE_MASK_FAIL,
	eALARMID_LCP_AUTOCAL_EMPTY_BUFFER,
	eALARMID_LCP_AUTOCAL_MASK_FAIL,
	eALARMID_LCP_AUTOCAL_INSPECT_FAIL,
	/// 50000 ~ 59999 AVI
	//eALARMID_AVI_OFFSET			= 50000,

	/// 60000 ~ 69999 AVI
	//eALARMID_SVI_OFFSET			= 60000,

	/// 70000 ~ 79999 AVI
	//eALARMID_APP_OFFSET			= 70000,
};

/// Alarm Type 정의
enum ENUM_ALARM_TYPE
{
	eALARMTYPE_LIGHT			= 1,		// 경알람
	eALARMTYPE_HEAVY						// 중알람
};

// 반환값 사용 후, 동적 할당 된 메모리 삭제 할 것
__inline char* CSTR2PCH(CString strInput)
{
	char * tmpch;
	int sLen = WideCharToMultiByte(CP_ACP, 0, strInput, -1, NULL, 0, NULL, NULL);       
	tmpch = new char[sLen + 1];
	WideCharToMultiByte(CP_ACP, 0, strInput, -1, tmpch, sLen, NULL, NULL);
	return tmpch;
}

__inline bool COPY_CSTR2TCH(TCHAR* strDst, CString strSrc, size_t sizeDst)
{
	memset(strDst, 0, sizeDst);
	if (strSrc.GetLength() * sizeof(TCHAR) >= sizeDst)
		strSrc = strSrc.Left((int)(sizeDst / sizeof(TCHAR)) - 1);
	try{
		memcpy(strDst, (LPCTSTR)strSrc, strSrc.GetLength() * sizeof(TCHAR));
	}
	catch(...){
		return false;
	}
	return true;
}

// 2017.06.06  NDH : ROI over 체크 함수 추가 
inline void GetCheckROIOver(cv::Rect rtInput, cv::Rect rtCheck, cv::Rect &rtOutput)
{
	rtOutput = rtInput;

	if ( rtInput.x		<  rtCheck.x	)		rtOutput.x	=	rtCheck.x;
	if ( rtInput.y		<  rtCheck.y	)		rtOutput.y	=	rtCheck.y;

	int nCheck = ( rtCheck.x + rtCheck.width ) - ( rtOutput.x + rtInput.width ) ;
	if ( nCheck <  0	)		rtOutput.width += nCheck;

		nCheck = ( rtCheck.y + rtCheck.height) - ( rtOutput.y + rtInput.height );
	if ( nCheck <  0	)		rtOutput.height += nCheck;
	
	if( rtInput.width <= 0)
		rtInput.width = 1;
	if(rtInput.height <= 0)
		rtInput.height = 1;
};

// 2018.03.03 LJM
/*////////////////////////////////////////////////////////////////
함수 기능: 정방 행렬 Mask Size가 짝수 인지 3보다 작은지에 대한 Check
반환 값 : 3이상인 홀수 MaskSize 반환, 
*/////////////////////////////////////////////////////////////////
inline void GetCheckMaskSize(int& nInput)
{
	if(nInput < 3)
		nInput = 3;

	if(nInput % 2 == 0)
	{
		nInput += 1;
	}
};

#endif
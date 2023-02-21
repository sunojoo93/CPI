// Digitizer.cpp: implementation of the CMatroxCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ppl.h>

#include "MatroxCamera.h"
#include "VSCameraTask.h"

using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// 16.03.17 pixel shift tact
LONGLONG m_i64Freq;
LONGLONG m_i64Start;
LONGLONG m_i64End;

LONGLONG m_i64Start2;
LONGLONG m_i64End2;

int g_nHookCount = 0;
int g_nHookGrabStart = 0;
int g_nHookGrabEnd = 0;

HANDLE g_hExpEnd[MAX_BUFFER_NUMBER] = {NULL,};		// Exposure End 시점

// HuaRay 그랩용 이벤트핸들 변수
int g_nHookGrabCount = 0;
HANDLE g_hExpEndHuaRay[MAX_BUFFER_NUMBER] = {NULL,};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatroxCamera::CMatroxCamera()
{
	m_milSystem			= M_NULL;

	for(int i=0;i<MAX_IMAGE_RATIO;i++)
	{
		m_milImageLarge[i] = M_NULL;
	}

	for (int i=0; i<MAX_BUFFER_NUMBER; i++)
		m_OdImage[i]  = M_NULL;

	m_MilWBCoefficients = M_NULL;
	m_LiveImage = M_NULL;
	m_milLiveGrabBuffer = M_NULL;
	m_milCropLoadClrImg = M_NULL; // 17.01.05 GUI 수동 호출 이미지 
// 	m_milCropLoadImg	= M_NULL;
// 
// 	// 2017.01.09 add by ikm - S  
// 	for(int i = 0 ; i < NUM_LOAD_IMAGE_MAX; i++)
// 	{
// 		m_milLoadImgBuff[i] = M_NULL;
// 		m_milLoadClrImgBuff[i] = M_NULL;		
// 	}
// 	// 2017.01.09 add by ikm - E

	// 16.05.11 Exposure End 시점 Event
	for (int i = 0; i < MAX_BUFFER_NUMBER; i++)
	{
		g_hExpEnd[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		g_hExpEndHuaRay[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	// 16.03.16 카메라 TASK 분리 준비 작업 by CWH
	m_milDigitizer			= M_NULL;

	// 16.03.16 카메라 TASK 분리 준비 작업 by CWH
	m_lDigSizeX		= 0;
	m_lDigSizeY		= 0;
	m_lDigBit		= 0;

	// 16.03.17 pixel shift 기능 추가
	m_bIsMultiShot = FALSE;
	//Image current Index
	m_nCurFrameNumber = 0;
	m_nHookCount = 0;
	m_PixelPitch  = 5500;	// 29M Camera 사용 (임시 고정 - 계산해서 넣는 구문 고려) - 현재 사용 안함

	m_pThrClrMulBuf = NULL;
	m_hPreparePixelShift = CreateEvent(NULL, TRUE, FALSE, NULL);
	//Get Clock frequency
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_i64Freq);

	m_hLiveWnd = NULL;
	m_bTriggerLive = FALSE;
	m_bFreeRunLive = FALSE;

	m_milDisplay = M_NULL;
	m_milDispImage = M_NULL;

	m_milCropImage = M_NULL;
	m_nSequenceMode = eNone;
}

CMatroxCamera::~CMatroxCamera()
{
	
}

BOOL CMatroxCamera::m_fnInitializeDigitizer(const CString& strDCF, int nChannel)
{
	BOOL bRet = FALSE;

	MdigAlloc(m_milSystem, nChannel, strDCF, M_DEFAULT, &m_milDigitizer);

	if (m_milDigitizer != M_NULL)
	{
		m_lDigSizeX		= (long)MdigInquire(m_milDigitizer, M_SIZE_X, NULL);
		m_lDigSizeY		= (long)MdigInquire(m_milDigitizer, M_SIZE_Y, NULL);
		m_lDigBit		= MdigInquire(m_milDigitizer, M_SIZE_BIT, NULL);
		// 16.05.20 Band 추가 for Color Camera
		// 19.04.22 Color or Mono 셋팅
		if (m_bUseColorBayer)
			m_lDigBand = 3;
		else
			m_lDigBand		= MdigInquire(m_milDigitizer, M_SIZE_BAND, NULL);
		bRet = TRUE;
	}
	else
	{
		m_lDigSizeX		= 6576;
		m_lDigSizeY		= 4384;
		m_lDigBit		= 8;
		m_lDigBand		= 1;
	}

	// 16.06.30 Live 화면 출력 추가 by CWH
	// Allocate a display
	if( m_milDisplay == NULL )
		MdispAlloc(m_milSystem, M_DEFAULT, M_DISPLAY_SETUP, M_WINDOWED, &m_milDisplay);					

	// 16.03.16 by CWH
	MdigControl(m_milDigitizer, M_GRAB_MODE,			M_ASYNCHRONOUS);
	MdigControl(m_milDigitizer, M_GRAB_EXPOSURE_SOURCE, M_SOFTWARE);
	MdigHookFunction(m_milDigitizer, M_GRAB_START,			(MIL_DIG_HOOK_FUNCTION_PTR)HookGrabStart,		this);
	MdigHookFunction(m_milDigitizer, M_GRAB_END,			(MIL_DIG_HOOK_FUNCTION_PTR)HookGrabEnd,			this);
	MdigHookFunction(m_milDigitizer, M_GRAB_FRAME_START,	(MIL_DIG_HOOK_FUNCTION_PTR)HookFrameStart,	this);
	MdigHookFunction(m_milDigitizer, M_GRAB_FRAME_END,		(MIL_DIG_HOOK_FUNCTION_PTR)HookFrameEnd,	this);
// 	MdigHookFunction(m_milDigitizer, M_TIMER_START+M_TIMER1,(MIL_DIG_HOOK_FUNCTION_PTR)HookTimerStart,	this);
// 	MdigHookFunction(m_milDigitizer, M_TIMER_END+M_TIMER1,	(MIL_DIG_HOOK_FUNCTION_PTR)HookTimerEnd,	this);

	return bRet;
}

BOOL CMatroxCamera::m_fnInitializeImageBuffer()
{
	BOOL bRet = FALSE;

	// m_nSizeBit가 8,16bit외에 다른 bit는 들어 온다면?
	if (m_lDigBit == 12 || m_lDigBit == 8)
	{
		bRet = m_fnPrepareGrabBuffer();
	}
	else
	{
		:: AfxMessageBox(_T(" image bit 8 or 16 are supported.Bit check images!!!!!!!!"));
		bRet = FALSE;
	}

	return bRet;
}

void CMatroxCamera::m_fnFreeImageBuffer()
{
	int i;

	// 16.03.23 Grab 할 때마다 Buffer 재할당하는 불합리 수정
	//Mil Buf 및 WindowBuf 다시 생성...
	for(i=0;i<MAX_IMAGE_RATIO;i++)
		BUFFREE(m_milImageLarge[i]);
	
	for(i=0;i<MAX_BUFFER_NUMBER;i++)
		BUFFREE(m_OdImage[i]);

	if (m_lDigBit == 12)
		delete m_totalImageBuf;
	else if (m_lDigBit == 8)
		delete m_totalImageBuf8Bit;

	BUFFREE(m_MilWBCoefficients);

	// 16.06.30 Live 화면 추가
	BUFFREE(m_LiveImage);
	
	BUFFREE(m_milLiveGrabBuffer);

	BUFFREE(m_milDispImage);

	BUFFREE(m_milCropImage);
 	BUFFREE(m_milCropLoadClrImg);
// 	BUFFREE(m_milCropLoadImg);
// 
// 	for(int i = 0 ; i < NUM_LOAD_IMAGE_MAX; i++)
// 	{
// 		BUFFREE(m_milLoadImgBuff[i]);
// 		BUFFREE(m_milLoadClrImgBuff[i]);		
// 	}
}

void CMatroxCamera::CloseApp()
{   
	m_fnFreeImageBuffer();
	
	if(m_milDisplay != M_NULL)
	{
		StopLiveGrab();
		MdispFree(m_milDisplay);
		m_milDisplay = M_NULL;
	}

	if(m_milDigitizer != M_NULL)
	{
		// 16.03.16 카메라 TASK 분리 준비 작업 by CWH
		MdigFree(m_milDigitizer);
		m_milDigitizer = M_NULL;
	}

	if(m_milSystem != M_NULL)
	{
		MsysFree(m_milSystem);
		m_milSystem = M_NULL;
	}

	if(m_milApplication != M_NULL)
	{
		MappFree(m_milApplication);
		m_milApplication = M_NULL;
	}
}

void CMatroxCamera::CloseGrabber()
{   
	m_fnFreeImageBuffer();

	// 16.06.30 Live 화면 출력 추가 by CWH
	if(m_milDisplay != M_NULL)
	{
		StopLiveGrab();
		MdispFree(m_milDisplay);
		m_milDisplay = M_NULL;
	}

	if(m_milDigitizer != M_NULL)
	{
		MdigFree(m_milDigitizer);
		m_milDigitizer = M_NULL;
	}

	if(m_milSystem != M_NULL)
	{
		MsysFree(m_milSystem);
		m_milSystem = M_NULL;
	}
}

void CMatroxCamera::CloseDigitizer()
{   
	m_fnFreeImageBuffer();

	// 16.06.30 Live 화면 출력 추가 by CWH
	if(m_milDisplay != M_NULL)
	{
		StopLiveGrab();
		MdispFree(m_milDisplay);
		m_milDisplay = M_NULL;
	}

	if(m_milDigitizer != M_NULL)
	{
		MdigFree(m_milDigitizer);
		m_milDigitizer = M_NULL;
	}
}

void CMatroxCamera::OpenFeatureDialog()
{
	MdigControl(m_milDigitizer, M_GC_FEATURE_BROWSER, M_OPEN + M_ASYNCHRONOUS);
}

// 16.03.16 Camera TASK 분리 준비 작업 - CMatroxCamera 는 하나의 Camera 만 대응하도록 기능 변경
// 다수 Camera 연결 시 Digitizer 배열 생성
// 16.03.16 함수화 by CWH
BOOL CMatroxCamera::m_fnPrepareGrabBuffer()
{
	int nBufIndex;
	int nDigBitOffset = 8 * (int)(m_lDigBit==8?1:2);
	MIL_ID milRet = M_NULL;

	// 16.03.23 Grab 할 때마다 Buffer 재할당하는 불합리 수정
	for (nBufIndex = 0; nBufIndex < MAX_IMAGE_RATIO; nBufIndex++)
	{
		if (m_lDigBand == 1)	// Mono
			milRet = MbufAlloc2d(m_milSystem, m_lDigSizeX*(nBufIndex+1), m_lDigSizeY*(nBufIndex+1), nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC+M_DISP, &m_milImageLarge[nBufIndex]);	
		else if (m_lDigBand > 1)	// Color
			milRet = MbufAllocColor(m_milSystem, m_lDigBand, m_lDigSizeX*(nBufIndex+1), m_lDigSizeY*(nBufIndex+1), nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC+M_DISP, &m_milImageLarge[nBufIndex]);	

		if (milRet == M_NULL)
			return FALSE;

		MbufClear(m_milImageLarge[nBufIndex], 0);
	}

	for(nBufIndex=0; nBufIndex<MAX_BUFFER_NUMBER; nBufIndex++)
	{
		if (m_lDigBand == 1)	// Mono
		{
			milRet = MbufAlloc2d(m_milSystem, m_lDigSizeX, m_lDigSizeY, nDigBitOffset + M_UNSIGNED, M_IMAGE + M_GRAB + M_DISP + M_PROC, &m_OdImage[nBufIndex]);
			
		}
		else if (m_lDigBand > 1)	// Color
			milRet = MbufAllocColor(m_milSystem, m_lDigBand, m_lDigSizeX, m_lDigSizeY, nDigBitOffset+M_UNSIGNED, M_IMAGE+M_GRAB+M_DISP+M_PROC, &m_OdImage[nBufIndex]);	// 1 임시적용

		if (milRet == M_NULL)
			return FALSE;

// 2017.02.20 MIL-Lite 에서 Board Memory 직접 접근 시 문제 발생으로 인해 Byte Buffer 생성하여 복사 후 사용하도록 수정
// 		if (m_lDigBit == 12) 
// 			MbufInquire(m_OdImage[nBufIndex], M_HOST_ADDRESS, &m_parFrameBuf[nBufIndex]);
// 		else if (m_lDigBit == 8)
// 			MbufInquire(m_OdImage[nBufIndex], M_HOST_ADDRESS, &m_parFrameBuf8Bit[nBufIndex]);
		MbufClear(m_OdImage[nBufIndex], 0);
	}	

	// 16.03.11 최종 Buffer 추가 for P/S by CWH
	//Final image buffer allocation.

	try	
	{
		//m_BufLen = (UINT32)m_lDigSizeX * (UINT32)m_lDigSizeY * (MAX_IMAGE_RATIO - 1) * (MAX_IMAGE_RATIO - 1) * nDigBitOffset; // Byte
		m_BufLen = (UINT32)m_lDigSizeX * (UINT32)m_lDigSizeY * (MAX_IMAGE_RATIO - 1) * (MAX_IMAGE_RATIO - 1);	// yskim bit 계산이 아닌 바이트 계산임.
		if (m_lDigBit == 12)
		{
			m_totalImageBuf = new unsigned short[m_BufLen];	
			memset(m_totalImageBuf,0,m_BufLen);
		}
		else if (m_lDigBit == 8)
		{
			m_totalImageBuf8Bit = new BYTE[m_BufLen];	
			memset(m_totalImageBuf8Bit,0,m_BufLen);
		}
	}
	catch(int )
	{		
		return FALSE;
	}

	// 16.06.30 Live 화면 출력 추가 by CWH
	if (MbufAlloc2d(m_milSystem, m_lDigSizeX, m_lDigSizeY, nDigBitOffset+M_UNSIGNED, M_IMAGE+M_GRAB+M_DISP, &m_LiveImage) == M_NULL)
		return FALSE;
	if (MbufAlloc2d(m_milSystem, m_lDigSizeX, m_lDigSizeY, nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC, &m_milLiveGrabBuffer) == M_NULL)
		return FALSE;
	if (MbufAlloc2d(m_milSystem, m_lDigSizeX, m_lDigSizeY, nDigBitOffset+M_UNSIGNED, M_IMAGE+M_GRAB+M_DISP, &m_milDispImage) == M_NULL)
		return FALSE;

	//2019.04.11
	if (m_lDigBand > 1)
	{
		MbufAllocColor(m_milSystem, 3L, m_lDigSizeX, m_lDigSizeY, nDigBitOffset + M_UNSIGNED, M_IMAGE + M_GRAB + M_DISP, &m_ColorImage);
		MbufAllocColor(m_milSystem, 3L, m_lDigSizeX, m_lDigSizeY, nDigBitOffset + M_UNSIGNED, M_IMAGE + M_GRAB + M_DISP, &m_milCropLoadClrImg);
		// 16.06.01 WhiteBalance 보정치
		MbufAlloc1d(m_milSystem, 3L, 32 + M_FLOAT, M_ARRAY, &m_MilWBCoefficients);
	}

	// 16.07.19 Live 화면 UI 추가 by CWH
	if (m_lDigBit == 12) 
	{
		MdispControl(m_milDisplay, M_VIEW_MODE, M_BIT_SHIFT);
		MdispControl(m_milDisplay, M_VIEW_BIT_SHIFT, 4);
	}
	else if (m_lDigBit == 8)
	{
		MdispControl(m_milDisplay, M_VIEW_MODE, M_DEFAULT );
	}

	// Minimap, UI 표시 Image Buffer
	if (MbufAlloc2d(m_milSystem, m_lDigSizeX, m_lDigSizeY, nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC, &m_milCropImage) == M_NULL)
		return FALSE;
// 	if (MbufAlloc2d(m_milSystem, m_lDigSizeX, m_lDigSizeY, nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC, &m_milCropLoadImg) == M_NULL)
// 		return FALSE;
// 	if (MbufAllocColor(m_milSystem, 3L, m_lDigSizeX, m_lDigSizeY , nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC+M_BGR24+M_PACKED, &m_milCropLoadClrImg) == M_NULL)
//		return FALSE;
// 
// 	// 2017.01.09 add by ikm GUI Image Load 용 - S  
// 	for(int i = 0 ; i < NUM_LOAD_IMAGE_MAX; i++)
// 	{		
// 		if(MbufAlloc2d(m_milSystem, m_lDigSizeX, m_lDigSizeY, nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC, &m_milLoadImgBuff[i])== M_NULL)
// 			return FALSE;
// 		if(MbufAllocColor(m_milSystem, 3L, m_lDigSizeX , m_lDigSizeY  , nDigBitOffset+M_UNSIGNED, M_IMAGE+M_PROC+M_BGR24+M_PACKED, &m_milLoadClrImgBuff[i]) == M_NULL)
// 			return FALSE;
// 	}
// 	// 2017.01.09 add by ikm GUI Image Load 용 - E

	// for test0627
	// for test0627
	//m_milTEST = MbufImport(theApp.GETDRV() + _T(":\\Doc\\Image\\Test\\Active1.bmp"), M_DEFAULT, M_RESTORE, m_milSystem, M_NULL);

	return TRUE;
}

////////////////////////////////////////////////////////////
// 16.03.16 Camera TASK 분리 준비 작업
// pixel shift 기능 추가
void CMatroxCamera::CameraExpose()
{
	//MIL_ID milRet;

	m_nCurFrameNumber	= 0;
	m_nHookCount = 0;
	g_nHookCount = 0;

	g_nHookGrabStart = 0;
	g_nHookGrabEnd = 0;

	m_GrabTime.Start();
	QueryPerformanceCounter((LARGE_INTEGER *)&m_i64Start);

	//OutputDebugString("OnSoftwareTrigger \n");

	// Grab 시에는 미리 생성해 놓은 Buffer 초기화만 실행
	m_fnClearImageBuffer();

	theApp.m_fnWriteTactLog(_T(">> Prepare Grab : %3.3f"), m_GrabTime.Stop(false));

	//human : MultiShot Enable ????
	if (m_nSequenceMode!=0)
	{
		if (m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
		{
			g_nHookGrabCount = 0;
			MdigProcess(m_milDigitizer, m_OdImage, m_nMaxBuf, M_START, M_ASYNCHRONOUS, MIL_BUF_HOOK_FUNCTION_PTR(ProcessingFunction), this);
			for (int i = 0; i < m_nMaxBuf; i++)
			{
				Sleep(10);
				MdigControl(m_milDigitizer, M_TIMER1 + M_TIMER_TRIGGER_SOFTWARE, M_ACTIVATE);
				WaitForSingleObject(g_hExpEndHuaRay[i], INFINITE);
			}
			MdigProcess(m_milDigitizer, m_OdImage, m_nMaxBuf, M_STOP, M_DEFAULT, MIL_BUF_HOOK_FUNCTION_PTR(ProcessingFunction), this);
		}
		else
		{
			MdigControlFeature(m_milDigitizer, M_FEATURE_EXECUTE, MIL_TEXT("StageReset"), M_TYPE_COMMAND, M_NULL);	// yskim

			MdigProcess(m_milDigitizer, m_OdImage, m_nMaxBuf, M_SEQUENCE, M_ASYNCHRONOUS + M_TRIGGER_FOR_FIRST_GRAB, M_NULL, M_NULL);

			// 트리거 신호 발생 분기
			if (m_eLinkType == eCoaXPress)
				MdigControlFeature(m_milDigitizer, M_DEFAULT, MIL_TEXT("TriggerSoftware"), M_TYPE_COMMAND, M_NULL);
			else
				MdigControl(m_milDigitizer, M_GRAB_EXPOSURE + M_TIMER1, M_ACTIVATE);

			WaitForSingleObject(g_hExpEnd[m_nMaxBuf - 1], INFINITE);		// 16.05.11 Exposure Time End 시점
			for (int i = 0; i<(int)m_nMaxBuf; i++)
				ResetEvent(g_hExpEnd[i]);
			//m_fnGetPixelShiftData();	// Grab End 시점 이 후 Buffer Merge
		}

		// 이미지 획득 (이미지 획득이라고 보다는 영상 들어올 pointer 획득)
		m_milGrabBuffer = m_milImageLarge[m_nSequenceMode];

		// 현재 Pattern Grab 시 Camera 상태 Back up
		m_bIsMultiShot = TRUE;
		m_nCurHRatio = m_nHRatio;
		m_nCurVRatio = m_nVRatio;
		m_nCurMaxBuf = m_nMaxBuf;
		m_nCurSeqMode = m_nSequenceMode;
	}
	else
	{
		if (!m_bTriggerLive && !m_bFreeRunLive)
		{
			if (m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
			{
				g_nHookGrabCount = 0;
				MdigProcess(m_milDigitizer, m_OdImage, m_nMaxBuf, M_START, M_ASYNCHRONOUS, MIL_BUF_HOOK_FUNCTION_PTR(ProcessingFunction), this);
				Sleep(10);
				MdigControl(m_milDigitizer, M_TIMER1 + M_TIMER_TRIGGER_SOFTWARE, M_ACTIVATE);
				WaitForSingleObject(g_hExpEndHuaRay[0], INFINITE);		// 16.05.11 Exposure Time End 시점
				MdigProcess(m_milDigitizer, m_OdImage, m_nMaxBuf, M_STOP, M_DEFAULT, MIL_BUF_HOOK_FUNCTION_PTR(ProcessingFunction), this);
			}
			else
			{
				ResetEvent(g_hExpEnd[0]);
				MdigGrab(m_milDigitizer, m_OdImage[0]);
				// 트리거 신호 발생 분기
				if (m_eLinkType == eCoaXPress)
					MdigControlFeature(m_milDigitizer, M_DEFAULT, MIL_TEXT("TriggerSoftware"), M_TYPE_COMMAND, M_NULL);
				else
					MdigControl(m_milDigitizer, M_GRAB_EXPOSURE + M_TIMER1, M_ACTIVATE);
				WaitForSingleObject(g_hExpEnd[0], INFINITE);		// 16.05.11 Exposure Time End 시점
			}

			// 이미지 획득
			m_milGrabBuffer = m_OdImage[0];
			m_bIsMultiShot = FALSE;		// m_bIsMultiShot Flag Grab End 응답대기 구문 판단으로 용도 변경
		}
		else
		{
			m_milGrabBuffer = GetLiveGrabImage();
			theApp.m_pLogWriter->m_fnWriteLog(_T("Escape Camera Expose - Live Mode !!"));
		}
	}
	
	return;
}

BOOL CMatroxCamera::IsDigitigerConnected()
{
	BOOL bRet = FALSE;
	if (m_milDigitizer != M_NULL)
		bRet = TRUE;
	return bRet;
}

BOOL CMatroxCamera::IsSystemConnected()
{
	BOOL bRet = FALSE;
	if (m_milSystem != M_NULL)
		bRet = TRUE;
	return bRet;
}

// PS 영상 병합하여 m_milImageLarge[m_nCurSeqMode]에 담는 함수
void CMatroxCamera::m_fnGetPixelShiftData()
{
	for( int i=0; i<(int)m_nCurMaxBuf; i++ )
	{
		while( i >= g_nHookCount )
		{
			Sleep(1);
		}

		if( i+1 == (int)m_nCurMaxBuf )
		{
			//Tact-Time
			QueryPerformanceCounter((LARGE_INTEGER *)&m_i64End);

			//MdigProcess( m_milDigitizer, MilImage, 4, M_STOP, M_DEFAULT, (MIL_DIG_HOOK_FUNCTION_PTR)HookSequence, M_NULL );
			MdigProcess( m_milDigitizer, m_OdImage, m_nCurMaxBuf, M_STOP, M_DEFAULT, M_NULL, M_NULL );

			double f64CostTime;
			f64CostTime = (double)(m_i64End - m_i64Start) / m_i64Freq;

			theApp.m_fnWriteTactLog(_T(">>>> P/S Start : %f ms"), f64CostTime*1000);

			LONGLONG l_Start,l_End;
			QueryPerformanceCounter((LARGE_INTEGER *)&l_Start);
			//각 버퍼를 전체 버퍼에 재조합...
			// 병렬 처리에 따른 Vertical / Horizontal 조건 변경
			int nMergeOffset = 1;

			if (m_nCurHRatio != m_nCurVRatio)
			{				
				if (m_nCurHRatio > m_nCurVRatio)
				{
					m_nCurMaxBuf /= m_nCurHRatio;
				}
				else
				{
					nMergeOffset *= (m_nCurVRatio+1);
				}
			}
			parallel_for(0, (int)m_nCurMaxBuf, nMergeOffset, [&](int FOrder)	// 16.05.16 for Pixel Merge Tact
			//for(int FOrder=0;FOrder<(int)m_nCurMaxBuf;FOrder+=nMergeOffset)
			{			
				if (m_lDigBit == 12)
				{
					// 2016.03.31 P/S 기능 확장 - H, V 선택 기능
					m_fnPixelPosSetMono(m_OdImage[FOrder] , (unsigned short int *)m_totalImageBuf , (UINT32)m_lDigSizeX , (UINT32)m_lDigSizeY , m_nCurHRatio , m_nCurVRatio , FOrder % m_nCurHRatio , FOrder / m_nCurVRatio);
					//MbufGet2d(MilImage[FOrder],0,0,m_lDigSizeX,m_lDigSizeY,m_parFrameBuf[FOrder]);				
				}
				else if (m_lDigBit == 8)
				{
					// 2016.03.31 P/S 기능 확장 - H, V 선택 기능
					//MbufGet2d(MilImage[FOrder],0,0,m_lDigSizeX,m_lDigSizeY,m_parFrameBuf8Bit[FOrder]);		
					m_fnPixelPosSetMono8Bit(m_OdImage[FOrder] , (BYTE*)m_totalImageBuf8Bit , (UINT32)m_lDigSizeX , (UINT32)m_lDigSizeY , m_nCurHRatio , m_nCurVRatio , FOrder % m_nCurHRatio , FOrder / m_nCurVRatio);			
				}
			}
			);
			// 16.05.18 병렬처리로 변경하여 모든 처리 끝난 이 후 저장 호출
			//if(FOrder == (m_nCurMaxBuf-1))
			//{
			//조합시간 출력...
			QueryPerformanceCounter((LARGE_INTEGER *)&l_End);
			f64CostTime = (double)(l_End - l_Start) / m_i64Freq;
			theApp.m_fnWriteTactLog(_T(">>>> Merge-Time : %f ms \n"), f64CostTime*1000);

			// 2016.03.31 P/S 기능 확장 - H, V 선택 기능
			if (m_lDigBit == 12)
			{
				//조합 된 이미지 출력
				MbufPut2d(m_milImageLarge[m_nCurSeqMode], 0, 0, m_lDigSizeX*m_nCurHRatio, m_lDigSizeY*m_nCurVRatio , m_totalImageBuf );		
			}
			else if (m_lDigBit == 8)
			{
				//조합 된 이미지 출력.
				MbufPut2d(m_milImageLarge[m_nCurSeqMode], 0, 0, m_lDigSizeX*m_nCurHRatio, m_lDigSizeY*m_nCurVRatio , m_totalImageBuf8Bit );		
			}
			//}
		}	
	}
}

// 12비트용 PS 영상 병합
void CMatroxCamera::m_fnPixelPosSetMono(MIL_ID milSrc, unsigned short int *Odata,UINT32 HLen,UINT32 VLen,BYTE HRatio,BYTE VRatio,UINT32 HOffset,UINT32 VOffset)
{
	unsigned short* pByteInput = new unsigned short[m_lDigSizeX * m_lDigSizeY];
	MbufGet2d(milSrc, 0, 0, m_lDigSizeX, m_lDigSizeY, pByteInput);

	//human
	bool bIsLast = FALSE;
	if(HOffset==HRatio-1 && VOffset==VRatio-1)
	{
		bIsLast = TRUE;
	}

	if( VOffset >= VRatio || HOffset >= HRatio)
	{
		return;
	}

	HOffset = HRatio - HOffset - 1;

	UINT32 HBufLen   =  HLen * HRatio ;
	UINT32 LinePos   = HBufLen * VRatio;
	UINT32 LineIndex = HBufLen * VOffset;
	UINT32 VPos		 = 0;
	int cnt			 = 0;

	for(UINT32 j=0;j<VLen;j++)
	{
		int p = 0;
		VPos = j * LinePos + LineIndex;
		for(UINT32 i=0 ; i<HLen ; i++)
		{
			Odata[p + HOffset + VPos ] = pByteInput[cnt++];
			p += HRatio;
		}
	}
	SAFE_DELETE_ARR(pByteInput);
}

// 8비트용 PS 영상 병합
void CMatroxCamera::m_fnPixelPosSetMono8Bit(MIL_ID milSrc, BYTE *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset)
{
	BYTE* pByteInput = new BYTE[m_lDigSizeX * m_lDigSizeY];
	MbufGet2d(milSrc, 0, 0, m_lDigSizeX, m_lDigSizeY, pByteInput);

	//human
	bool bIsLast = FALSE;
	if(HOffset==HRatio-1 && VOffset==VRatio-1)
	{
		bIsLast = TRUE;
	}

	if( VOffset >= VRatio || HOffset >= HRatio)
	{
		return;
	}

	HOffset = HRatio - HOffset - 1;

	UINT32 HBufLen =  HLen * HRatio ;
	UINT32 LinePos   = HBufLen * VRatio;
	UINT32 LineIndex = HBufLen * VOffset;
	UINT32 VPos = 0;

	int cnt = 0;

	for(UINT32 j=0;j<VLen;j++)
	{
		int p = 0;
		VPos = j * LinePos + LineIndex;
		for(UINT32 i=0 ; i<HLen ; i++)
		{
			Odata[p + HOffset + VPos ] = pByteInput[cnt++];
			p += HRatio;
		}
	}
	SAFE_DELETE_ARR(pByteInput);
}

// 16.03.23 Grab 할 때마다 Buffer 재할당하는 불합리 수정
void CMatroxCamera::m_fnClearImageBuffer()
{
	// for Normal Grab Tact - 메모리 초기화 구문 속도 개선 Todo
	MbufClear(m_OdImage[0], 0);

	theApp.m_fnWriteTactLog(_T(">> Clear Buffer1 : %3.3f"), m_GrabTime.Stop(false));
	
	if (m_nSequenceMode!=0)	// && m_bIsMultiShot)	// m_bIsMultiShot Flag Grab End 응답대기 구문 판단으로 용도 변경
	{
#ifndef _DEBUG
		parallel_for(1, (int)m_nMaxBuf, [&](int i)
#else
		// Debug 시 병렬 미사용.
		for(int i=1;i<MAX_BUFFER_NUMBER;i++)
#endif
		{
			if( m_OdImage[i] != M_NULL )
			{
				MbufClear(m_OdImage[i], 0);
			}
		}
		
#ifndef _DEBUG
		);
#endif
		theApp.m_fnWriteTactLog(_T(">> Clear Buffer2 : %3.3f"), m_GrabTime.Stop(false));

		m_pThrClrMulBuf = AfxBeginThread(ThreadClearMultishotBuffers, (LPVOID)this);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Camera Wrapper functions.

BOOL CMatroxCamera::InitializeCamera(CString strInitFilePath)
{
	BOOL bRet = FALSE;
	TCHAR strDcfFilePath[256];
	TCHAR strTemp[256];
	int nGrabberNo, nDigCh;									// Digitizer 정보
	int nPortNum, nBaudrate;								// Serial Port 정보
	int nTrigMode, nAnalogGain;								// Camera 정보
	int	nInterval;											// Camera Monitoring Interval
	eBoardModel	eGrabberModel;								// 0 : Solios, 1 : Radient CXP

	TCHAR szSimulationFlag[6] = {0,};        //2017.01.02 sgkim H-Project
	/*GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		PostQuitMessage(0);*/

	// 2020.11.13 MDJ 151M Camera 대응 코드 수정
	// 2020.12.21 yskim 해당 분기문 삭제
	m_nInspPCType = GetPrivateProfileInt(_T("Common"), _T("TYPE"), 0, INIT_FILE_PATH);

	SetInitFilePath(strInitFilePath);

	GetPrivateProfileString(_T("Settings"), _T("Init File Path") ,theApp.GETDRV() + _T(":\\CRUX\\DATA\\DCF\\Camera.dcf"), strDcfFilePath, 100, strInitFilePath);

	// Digitizer 정보
	nGrabberNo		= GetPrivateProfileInt(_T("Settings"), _T("Frame Grabber No"), 4, strInitFilePath);
	eGrabberModel	= (eBoardModel)GetPrivateProfileInt(_T("Settings"), _T("Frame Grabber Model"), 0, strInitFilePath);
	m_eLinkType		= (eLinkType)GetPrivateProfileInt(_T("Settings"), _T("Link Type"), 0, strInitFilePath);
	nDigCh			= GetPrivateProfileInt(_T("Settings"), _T("Digitizer Channel"), 4, strInitFilePath);
	m_eCamModel		= (eCamModel)GetPrivateProfileInt(_T("Settings"), _T("Camera Model"), 4, strInitFilePath);

	// Serial Port 정보
	nPortNum		= GetPrivateProfileInt(_T("Settings"), _T("Serial Port"), 4, strInitFilePath);
	nBaudrate		= GetPrivateProfileInt(_T("Settings"), _T("Serial Baudrate"), 4, strInitFilePath);

	// Camera 정보
	nTrigMode		= GetPrivateProfileInt(_T("Settings"), _T("Trigger Mode"), 4, strInitFilePath);
	nAnalogGain		= GetPrivateProfileInt(_T("Settings"), _T("Analog Gain"), 4, strInitFilePath);
	// Camera Monitoring Interval
	nInterval		= GetPrivateProfileInt(_T("Settings"), _T("Monitoring Interval"), 0, strInitFilePath);
	SetMonitoringInterval(nInterval);

	// 설비별 Camera Gain 값 set	181014 yss
	TCHAR szDefaultGain[5] = { 0, };
	GetPrivateProfileString(_T("Settings"), _T("Default Gain Offset"), _T("1.0"), szDefaultGain, sizeof(szDefaultGain), strInitFilePath);
	SetDefaultGainOffset(_ttof(szDefaultGain));
	
	GetPrivateProfileString			(_T("Settings"), _T("Use Horizon Pixel Shift") ,_T("T"), strTemp, 2, strInitFilePath);
	if (_tcscmp(strTemp, _T("T")) == 0)	m_bUseHPS = TRUE;
	else								m_bUseHPS = FALSE;
	GetPrivateProfileString			(_T("Settings"), _T("Use Vertical Pixel Shift") ,_T("T"), strTemp, 2, strInitFilePath);
	if (_tcscmp(strTemp, _T("T")) == 0)	m_bUseVPS = TRUE;
	else								m_bUseVPS = FALSE;
	// 2019.04.22 yskim
	GetPrivateProfileString(_T("Settings"), _T("Use Color Bayer"), _T("F"), strTemp, 2, strInitFilePath);
	if (_tcscmp(strTemp, _T("T")) == 0)	m_bUseColorBayer = TRUE;
	else								m_bUseColorBayer = FALSE;

	if (!InitializeGrabber(nGrabberNo, eGrabberModel, nDigCh, strDcfFilePath))
		return FALSE;		

	MappControl(M_ERROR,M_PRINT_DISABLE);

	if (m_eLinkType == eCameraLink)
		bRet = OpenCameraComPort(nPortNum, nBaudrate, m_eCamModel);
	else	// 현재 CXP Camera 의 경우 별도로 연결 상태 확인 구문 없음. 연결 실패 시 MIL Error 발생. CAM UI 에서는 Digitizer만 할당되면 연결된 것으로 표시.
		bRet = TRUE;
	
	// 2016.02.06 Output Databit 8Bit 고정
	// 2016.02.06 Exposure Source - Pulse Width 로 고정
	// 2017.11.21 Trigger Source - CC1 으로 고정
	switch (m_eCamModel)
	{
	case eVN_Series		:
	case eVP_Series		:
	case eVH_Series		:
	case eVA_Series		:
	case eVA47M5_Series :
		// 기본 카메라 설정		
		//SetDataBit(8);		
		SetExposureSource(ePulseWidth);		// 1 로 설정하면 Pulse Width 맞는지 확인 - Camera Manual 이상	
		SetTriggerSource(eCC1);
		break;

	case eVNP_MX_Series:
	case eHuaRayPS:
	case eHuaRayNonPS:
		SetExposureSource(eTimed);
		SetTriggerSource(eSoftware);
		break;

	default:
		AfxMessageBox(_T("Please Set Exposure Source & Trigger Source Manually"));
		break;
	}
	return bRet;
}

BOOL CMatroxCamera::SetExposureTime(double dExpOnTime)
{
	BOOL bRet = FALSE;

	MappControl(M_ERROR,M_PRINT_DISABLE);

	if (m_eLinkType == eCoaXPress)
	{
		MIL_DOUBLE dExposureTime = dExpOnTime * 1000;
		MdigControlFeature(m_milDigitizer, M_DEFAULT, MIL_TEXT("ExposureTime"), M_TYPE_MIL_DOUBLE, &dExposureTime);
	}
	else
	{
		MdigControl(m_milDigitizer, M_GRAB_EXPOSURE_TIME,		  dExpOnTime * 1000000);	// ms -> ns	
	}
	Sleep(10);
	// Delay Time 0으로 Fix - for tact
	//MdigControl(m_milDigitizer, M_GRAB_EXPOSURE_TIME_DELAY, dExpOffTime);
	MdigControl(m_milDigitizer, M_GRAB_EXPOSURE_TIME_DELAY, 0);	
	Sleep(10);

	MappControl(M_ERROR,M_PRINT_ENABLE);

	bRet = TRUE;	// SW Trigger 설정 완료

	// 16.03.16 ExposureTime 적용 관련 Check - Sample P/G 에서는 Live 에서만 Serial 통신으로 Camera Exp Time 설정
	// 16.03.21 S/W Trigger Test
	// 16.07.15 Live 화면 Control 시 Camera 설정도 수정하도록 변경
	//if (GetTriggerMode() == eFreeRun)
	if (m_eLinkType != eCoaXPress)
		bRet = m_SerialCamera.SetExpTime(m_eCamModel, dExpOnTime);

	return bRet;
}

// Stitch	180809 YSS
BOOL CMatroxCamera::SetStitchMode(int nMode)
{
	BOOL bRet = FALSE;

	// 기존 SetExposureTime() 함수 내용
// 	MappControl(M_ERROR, M_PRINT_DISABLE);
// 
// 	if (m_eLinkType == eCoaXPress)
// 	{
// 		MIL_DOUBLE dExposureTime = dExpOnTime * 1000;
// 		MdigControlFeature(m_milDigitizer, M_DEFAULT, MIL_TEXT("ExposureTime"), M_TYPE_MIL_DOUBLE, &dExposureTime);
// 	}
// 	else
// 	{
// 		MdigControl(m_milDigitizer, M_GRAB_EXPOSURE_TIME, dExpOnTime * 1000000);	// ms -> ns	
// 	}
// 	Sleep(10);
// 	// Delay Time 0으로 Fix - for tact
// 	//MdigControl(m_milDigitizer, M_GRAB_EXPOSURE_TIME_DELAY, dExpOffTime);
// 	MdigControl(m_milDigitizer, M_GRAB_EXPOSURE_TIME_DELAY, 0);
// 	Sleep(10);
// 
// 	MappControl(M_ERROR, M_PRINT_ENABLE);
// 
// 	bRet = TRUE;	// SW Trigger 설정 완료
// 
// 					// 16.03.16 ExposureTime 적용 관련 Check - Sample P/G 에서는 Live 에서만 Serial 통신으로 Camera Exp Time 설정
// 					// 16.03.21 S/W Trigger Test
// 					// 16.07.15 Live 화면 Control 시 Camera 설정도 수정하도록 변경
// 					//if (GetTriggerMode() == eFreeRun)
// 	if (m_eLinkType != eCoaXPress)
// 		bRet = m_SerialCamera.SetExpTime(m_eCamModel, dExpOnTime, nCamMode);
//	bRet = m_SerialCamera.SetStitch(m_eCamModel, nCamMode);

	return bRet;
}

double CMatroxCamera::GetExposureTime()
{
	double dRetExpTime = 0.0;
	if (m_eLinkType == eCoaXPress)
		MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("ExposureTime"), M_TYPE_MIL_DOUBLE, &dRetExpTime);
	else
		dRetExpTime = m_SerialCamera.GetCameraExposureTime(m_eCamModel);

	return dRetExpTime / 1000.;		// 단위 ms
}

BOOL CMatroxCamera::SetSequenceMode(int nMode)
{
	// TODO: Add your control notification handler code here
	BOOL bRet = FALSE;

	m_nSequenceMode = nMode;

	if (m_eCamModel != eVN_Series && m_eCamModel != eVNP_MX_Series && m_eCamModel != eHuaRayPS && m_eCamModel != eHuaRayNonPS)
		return TRUE;

	// 2016.03.31 P/S 기능 확장 - H, V 선택 기능
	int nRatio = nMode + 1;
	m_bUseHPS	?	m_nHRatio = nRatio	:	m_nHRatio = 1;	// Horizontal P/S 기능 사용 여부
	m_bUseVPS	?	m_nVRatio = nRatio	:	m_nVRatio = 1;	// Vertical P/S 기능 사용 여부

	m_nMaxBuf = nRatio * nRatio;

	// Mode 변경 없을 경우 Camera 설정 Skip
	//if (pSystem->m_Config.m_nSeqMode == nMode)
	//	return TRUE;

	//pSystem->m_Config.m_nSeqMode = nMode;
	

	MIL_BOOL MultishotEnable = M_FALSE;
	MIL_BOOL CalibrationAuto = M_FALSE;
	MIL_INT64 StageStatus = 0;
	
	// -------------------------------------------------------------------------------------------------------------------------------------
	// 2020.11.12 MDJ Camera 151M 대응 코드 수정
	MIL_INT64 MultshotEnableStringSize = 0;

	if (m_eLinkType == eCoaXPress)
	{
		// 151M 카메라 PS 모드에 따라 동작
		if (m_eCamModel == eHuaRayPS)
		{
			if (nMode == eNone)
			{
				MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("PixelShiftEnable"), M_TYPE_STRING, MIL_TEXT("Off"));
			}
			else
			{
				MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("PixelShiftEnable"), M_TYPE_STRING, MIL_TEXT("On"));

				switch (nMode)
				{
					case e4Shot_1_2 :
						MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("PixelShiftDistance"), M_TYPE_ENUMERATION, MIL_TEXT("FourShot_1_2_Pixel"));
						break;
					case e4Shot_1 : 
						MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("PixelShiftDistance"), M_TYPE_ENUMERATION, MIL_TEXT("FourShot_1_Pixel"));
						break;
					case e9Shot_1_3:
						MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("PixelShiftDistance"), M_TYPE_ENUMERATION, MIL_TEXT("NineShot_1_3_Pixel"));
						break;
					case eUserTable :
						MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("PixelShiftDistance"), M_TYPE_ENUMERATION, MIL_TEXT("UserTable"));
						break;
				}

				MdigControlFeature(m_milDigitizer, M_FEATURE_EXECUTE, MIL_TEXT("PixelShiftPositionReset"), M_TYPE_COMMAND, M_NULL);
			}
		}
		else if (m_eCamModel == eHuaRayNonPS)
		{
			
		}
		else
		{
			// 기존 카메라(29M, 47M) PS 모드에 따라 동작
			if (nMode == eNone)
			{
				MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("MultishotEnable"), M_TYPE_BOOLEAN, &MultishotEnable);

				if (MultishotEnable == M_TRUE)
				{
					MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("StageMode"), M_TYPE_ENUMERATION, MIL_TEXT("Off"));
					MultishotEnable = M_FALSE;
					CalibrationAuto = M_FALSE;
					MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("MultishotEnable"), M_TYPE_BOOLEAN, &MultishotEnable);
					MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("CalibrationAuto"), M_TYPE_BOOLEAN, &CalibrationAuto);
				}
			}
			else
			{
				MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("MultishotEnable"), M_TYPE_BOOLEAN, &MultishotEnable);
				if (MultishotEnable == M_FALSE)
				{
					MultishotEnable = M_TRUE;
					CalibrationAuto = M_TRUE;
					MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("MultishotEnable"), M_TYPE_BOOLEAN, &MultishotEnable);
					MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("CalibrationAuto"), M_TYPE_BOOLEAN, &CalibrationAuto);

					MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("StageMode"), M_TYPE_ENUMERATION, MIL_TEXT("Mono4Frame"));
					MdigControlFeature(m_milDigitizer, M_FEATURE_EXECUTE, MIL_TEXT("StageReset"), M_TYPE_COMMAND, M_NULL);
				}
			}
		}
	}
	else
	{
		if (nMode == eNone)
		{
			m_SerialCamera.SetMultishotEnable(m_eCamModel, FALSE);
		}
		else
		{
			m_SerialCamera.SetMultishotEnable(m_eCamModel, TRUE);
		}
	}
	
	Sleep(10);

	// 16.03.23 Grab 할 때마다 Buffer 재할당하는 불합리 수정
	//FreeImageBuffer();		
	if (m_eLinkType == eCoaXPress)
	{
		//if (nMode == 0)		
		//	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("StageMode"), M_TYPE_ENUMERATION, MIL_TEXT("Off"));
		//else				
		//	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("StageMode"), M_TYPE_ENUMERATION, MIL_TEXT("Mono4Frame"));
		bRet = TRUE;
	}
	else
	{
		bRet = m_SerialCamera.SetCameraSequence(m_eCamModel, nMode);		// 차후 enum 이나 member var 로 변경하여 Camera 선택
	}

	//InitCamera();		// Multishot 설정 여부 확인 구문 필요하면 추가할 것

	// 16.03.23 Grab 할 때마다 Buffer 재할당하는 불합리 수정
	//InitializeImageBuffer();	

	return bRet;
}

int CMatroxCamera::GetSequenceMode()
{
	int nRet = 0;
	if (m_eLinkType == eCoaXPress)
	{
		// ---------------------------------------------------------------------------------------------------------------------
		// 2020.11.13 MDJ 151M Camera 대응 코드 수정
		// 151M 카메라 PS 모드에 따라 동작
		MIL_TEXT_CHAR szRet[256] = { 0, };
		if (m_eCamModel == eHuaRayPS)
		{
			MIL_INT64 PixelShiftEnableStringSize = 0;
			MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE + M_STRING_SIZE, MIL_TEXT("PixelShiftEnable"), M_TYPE_MIL_INT, &PixelShiftEnableStringSize);
			MIL_TEXT_PTR PixelShiftEnablePtr = new MIL_TEXT_CHAR[PixelShiftEnableStringSize];
			MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("PixelShiftEnable"), M_TYPE_STRING, PixelShiftEnablePtr);
			//delete[] PixelShiftEnablePtr;
				
			if (_tcsicmp(PixelShiftEnablePtr, _T("Off")) == 0)
			{
				nRet = 0;
			}
			else
			{
				MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("PixelShiftDistance"), M_TYPE_ENUMERATION, szRet);
				if (_tcsicmp(szRet, _T("FourShot_1_2_Pixel")) == 0)
					nRet = (int)e4Shot_1_2;
				else if (_tcsicmp(szRet, _T("FourShot_1_Pixel")) == 0)
					nRet = (int)e4Shot_1;
				else if (_tcsicmp(szRet, _T("NineShot_1_3_Pixel")) == 0)
					nRet = (int)e9Shot_1_3;
				else if (_tcsicmp(szRet, _T("UserTable")) == 0)
					nRet = (int)eUserTable;
			}
		}
		else if (m_eCamModel == eHuaRayNonPS)
		{
			nRet = 0;
		}
		else
		{
			MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("StageMode"), M_TYPE_ENUMERATION, szRet);
			if (_tcsicmp(szRet, _T("Off")) == 0)
				nRet = 0;
			else
				nRet = 1;
		}
		/*MIL_TEXT_CHAR szRet[256] = {0, };
		MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("StageMode"), M_TYPE_ENUMERATION, szRet);
		if (_tcsicmp(szRet, _T("Off")) == 0)
			nRet = 0;
		else
			nRet = 1;*/
		// ---------------------------------------------------------------------------------------------------------------------
	}
	else
	{
		nRet = m_SerialCamera.GetCameraSequence(m_eCamModel);
	}
	return nRet;
}

int CMatroxCamera::GetTriggerMode()
{
	int nRet = 0;
	if (m_eLinkType == eCoaXPress)
	{
		MIL_TEXT_CHAR szRet[256] = { 0, };
		MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("TriggerMode"), M_TYPE_ENUMERATION, szRet);
		if (_tcsicmp(szRet, _T("Off")) == 0)
			nRet = 0;
		else
			nRet = 1;
	}
	else
	{
		nRet = m_SerialCamera.GetCameraTriggerMode(m_eCamModel);
	}
	return nRet;
}

BOOL CMatroxCamera::SetTriggerMode(int nMode)
{
	BOOL bRet = FALSE;
	if (m_bFreeRunLive || m_bTriggerLive)
	{
		AfxMessageBox(_T("Please Live Stop !!!"));
		return FALSE;
	}

	if (m_eLinkType == eCoaXPress)
	{
		if (nMode == eFreeRun)
			MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("TriggerMode"), M_TYPE_ENUMERATION, MIL_TEXT("Off"));
		else
		{
			MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("TriggerMode"), M_TYPE_ENUMERATION, MIL_TEXT("On"));
			// HyaRay 카메라이면 추가 설정
			if (m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
			{
				MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("TriggerSelector"), M_TYPE_ENUMERATION, MIL_TEXT("FrameStart"));
				MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("TriggerSource"), M_TYPE_ENUMERATION, MIL_TEXT("CXPin"));
			}
		}
			
		bRet = TRUE;
	}
	else
	{
		bRet = m_SerialCamera.SetCameraTriggerMode(m_eCamModel, nMode);
	}
	return bRet;
}

double CMatroxCamera::GetAnalogGain()
{
	double dRet = 0.0;
	
	if (m_eLinkType == eCoaXPress)
	{
		MIL_DOUBLE			dGetGain = 0.0;
		// ---------------------------------------------------------------------------------------------------------------------
		// 2020.11.14 MDJ 151M Camera 대응 코드 수정
		if (m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
			MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("AnalogGain"), M_TYPE_MIL_DOUBLE, &dRet);
		else
			MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("Gain"), M_TYPE_MIL_DOUBLE, &dRet);
		//MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("Gain"), M_TYPE_MIL_DOUBLE, &dRet);
		// ---------------------------------------------------------------------------------------------------------------------
	}
	else
	{
		dRet = m_SerialCamera.GetCameraAnalogGain(m_eCamModel);
	}
	return dRet;
}

BOOL CMatroxCamera::SetAnalogGain(double dGainValue)
{
	BOOL bRet = FALSE;

	// Matrox Camera 설비별 Gain 값 기존 값에 곱함. 광학팀 요청사항		181014 YSS
	double dDefaultOffset = GetDefaultGainOffset();
	dGainValue = dGainValue * dDefaultOffset;

	if (m_eLinkType == eCoaXPress)
	{
		// Gain 허용 범위 1.0 ~ 4.0
		if (dGainValue < 1.0)	dGainValue = 1.0;
		if (dGainValue > 4.0)	dGainValue = 4.0;
		// ---------------------------------------------------------------------------------------------------------------------
		// 2020.11.14 MDJ 151M Camera 대응 코드 수정
		if (m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
			MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("AnalogGain"), M_TYPE_MIL_DOUBLE, &dGainValue);
		else
			MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("Gain"), M_TYPE_MIL_DOUBLE, &dGainValue);
		//MdigControlFeature(m_milDigitizer, M_DEFAULT, MIL_TEXT("Gain"), M_TYPE_MIL_DOUBLE, &dGainValue);
		// ---------------------------------------------------------------------------------------------------------------------
		bRet = TRUE;
	}
	else
	{
		bRet = m_SerialCamera.SetCameraAnalogGain(m_eCamModel, dGainValue);
	}
	return bRet;
}

// 2017.02.06 Data Bit 설정 (8, 10, 12 bit 중 하나)
int CMatroxCamera::GetDataBit()
{
	int nRet = 0;

	if (m_eLinkType == eCoaXPress)
	{
		MIL_TEXT_CHAR szRet[256] = {0, };
		MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("PixelFormat"), M_TYPE_ENUMERATION, szRet);
		if (_tcsicmp(szRet, _T("Off")) == 0)
			nRet = 0;
		else
			nRet = 1;
	}
	else
	{
		nRet = m_SerialCamera.GetCameraDataBit(m_eCamModel);
	}

	return nRet;
}

BOOL CMatroxCamera::SetDataBit(int nBit)
{
	BOOL bRet = FALSE;

	if (m_eLinkType == eCoaXPress)
	{
		bRet = TRUE;
	}
	else
	{
		bRet = m_SerialCamera.SetCameraDataBit(m_eCamModel, nBit);
	}

	return bRet;
}

// 2017.02.06 Exposure Source 설정
int CMatroxCamera::GetExposureSource()
{
	int nRet = 0;

	if (m_eLinkType == eCoaXPress)
		nRet = 0;
	else
		nRet = m_SerialCamera.GetCameraExpSource(m_eCamModel);

	return nRet;
}

BOOL CMatroxCamera::SetExposureSource(int nMode)
{
	BOOL bRet = FALSE;

	if (m_eLinkType == eCoaXPress)
	{
		switch(nMode)
		{
		case eTimed:
			MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("ExposureMode"), M_TYPE_ENUMERATION, MIL_TEXT("Timed"));
			bRet = TRUE;
			break;
		default:
			AfxMessageBox(_T("Not Defined Exposure Source !!!"));
			bRet = FALSE;
			break;
		}
	}
	else
	{
		bRet = m_SerialCamera.SetCameraExpSource(m_eCamModel, nMode);
	}

	return bRet;
}

bool CMatroxCamera::GetFanStatus()
{
	bool bRet = false;

	if (m_eLinkType == eCoaXPress)
		bRet = true;
	else
		bRet = m_SerialCamera.GetCameraFanStatus(m_eCamModel);

	return bRet;
}

int CMatroxCamera::GetFanRPM()
{
	int nRet = 0;

	if (m_eLinkType == eCoaXPress)
		nRet = 0;
	else
		nRet = m_SerialCamera.GetCameraFanRPM(m_eCamModel);

	return nRet;
}

double CMatroxCamera::GetTemperature()
{
	double dRet = 0.0;

	if (m_eLinkType == eCoaXPress)
		dRet = 0.0;
	else
		dRet = m_SerialCamera.GetCameraTemperature(m_eCamModel);

	return dRet;
}

double CMatroxCamera::GetCCDTemperature()
{
	double dRet = 0.0;

	if (m_eLinkType == eCoaXPress)
		dRet = 0.0;
	else
		dRet = m_SerialCamera.GetCameraCCDTemperature(m_eCamModel);

	return dRet;
}

// 2017.11.21 Trigger Source 설정 추가
int CMatroxCamera::GetTriggerSource()
{
	int nRet = 0;

	if (m_eLinkType == eCoaXPress)
		nRet = 1;	// 구현 필요
	else
		nRet = m_SerialCamera.GetCameraTrigSource(m_eCamModel);

	return nRet;
}

BOOL CMatroxCamera::SetTriggerSource(int nMode)
{
	BOOL bRet = FALSE;

	if (m_eLinkType == eCoaXPress)
	{
		// ---------------------------------------------------------------------------------------------------------------------
		// 2020.11.14 MDJ 151M Camera 대응 코드 수정
		if (m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
		{
			MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("TriggerSelector"), M_TYPE_ENUMERATION, MIL_TEXT("FrameStart"));
		}
		// ---------------------------------------------------------------------------------------------------------------------

		switch(nMode)
		{
		case eSoftware:
			MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE_AS_STRING, MIL_TEXT("TriggerSource"), M_TYPE_ENUMERATION, MIL_TEXT("Software"));
			bRet = TRUE;
			break;
		default:
			bRet = FALSE;
			break;
		}		
	}
	else
	{
		bRet = m_SerialCamera.SetCameraTrigSource(m_eCamModel, nMode);
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Frame grabber hook functions.
// 현재 Hook 사용 안함 
// - Sub Pixel 대응을 위해 Nano Stage 이동 Offset 을 조정 (Manual Multishot Mode)하거나, 
//   Overlap Mode 사용하여 P/S Tact 개선 및 Data ReadOut Time 중 PG Pattern Change 할 경우 사용
long MFTYPE HookGrabStart(long HookType, MIL_ID EventId, void *UserDataPtr)
{
	CMatroxCamera *pData = (CMatroxCamera*)UserDataPtr;	

	if(pData->m_bTriggerLive || pData->m_bFreeRunLive)
		return 0;

	theApp.m_fnWriteTactLog(_T(">> Grab Start : %3.3f"), pData->m_GrabTime.Stop(false));

	SetEvent(g_hExpEnd[g_nHookGrabStart]);	// 16.05.11 Add Exposure End Timming 20.12.18 yskim FrameEnd

	//if(pData->m_bIsMultiShot)	// m_bIsMultiShot Flag Grab End 응답대기 구문 판단으로 용도 변경
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_i64End2);
		double	f64CostTime = 0.0;
		f64CostTime = (double)(m_i64End2 - m_i64Start) / m_i64Freq;
		CString str;
		str.Format(_T("Hook Start (%d) : %03f ms! \n"), g_nHookGrabStart,f64CostTime);
		OutputDebugString(str);			

		g_nHookGrabStart++;
	}


	if(!pData->m_bIsMultiShot)
	{
		pData->m_nCurFrameNumber++;
	}	

	return 0;
}

long MFTYPE HookGrabEnd(long HookType, MIL_ID EventId, void *UserDataPtr)
{
	CMatroxCamera *pData = (CMatroxCamera*)UserDataPtr;

	if(pData->m_bTriggerLive)
	{
		MbufCopy(pData->m_LiveImage, pData->m_milLiveGrabBuffer);
		pData->m_fnSnapAndTrigger();	

		return 0 ;
	}

	g_nHookCount++;

	theApp.m_fnWriteTactLog(_T(">> Grab End : %3.3f"), pData->m_GrabTime.Stop(false));

	//if(pData->m_bIsMultiShot)	// m_bIsMultiShot Flag Grab End 응답대기 구문 판단으로 용도 변경
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_i64End2);
		double	f64CostTime = 0.0;
		f64CostTime = (double)(m_i64End2 - m_i64Start) / m_i64Freq;
		CString str;
		str.Format(_T("Hook End (%d) : %03f ms! \n"), g_nHookGrabEnd,f64CostTime);
		OutputDebugString(str);			

		g_nHookGrabEnd++;
	}

	if(!pData->m_bIsMultiShot)
	{
		pData->m_nHookCount++;

		CString tempstr;
		tempstr.Format(_T("%d"),pData->m_nCurFrameNumber);
		OutputDebugString(tempstr);
	}

	return 0;
}

long MFTYPE HookFrameStart(long HookType, MIL_ID EventId, void *UserDataPtr)
{
	CMatroxCamera *pData = (CMatroxCamera*)UserDataPtr;

	if(pData->m_bTriggerLive || pData->m_bFreeRunLive)
		return 0;	

	theApp.m_fnWriteTactLog(_T(">> Frame Start : %3.3f"), pData->m_GrabTime.Stop(false));

	return 0;
}

long MFTYPE HookFrameEnd(long HookType, MIL_ID EventId, void *UserDataPtr)
{
	CMatroxCamera *pData = (CMatroxCamera*)UserDataPtr;

	if(pData->m_bTriggerLive)
		return 0;

	if(pData->m_bFreeRunLive)
	{	
		MbufCopy(pData->m_LiveImage, pData->m_milLiveGrabBuffer);
		return 0 ;
	}

	theApp.m_fnWriteTactLog(_T(">> Frame End : %3.3f"), pData->m_GrabTime.Stop(false));

	return 0;
}

MIL_INT MFTYPE ProcessingFunction(MIL_INT HookType, MIL_ID HookId, void MPTYPE *HookDataPtr)
{
	CMatroxCamera *pData = (CMatroxCamera*)HookDataPtr;

	theApp.m_fnWriteTactLog(_T(">> ProcessingFunction Start : %3.3f, Count : %d"), pData->m_GrabTime.Stop(false), g_nHookGrabCount);

	SetEvent(g_hExpEndHuaRay[g_nHookGrabCount]);

	g_nHookGrabCount++;

	return 0;
}

long MFTYPE HookTimerStart(long HookType, MIL_ID EventId, void *UserDataPtr)
{
	CMatroxCamera *pData = (CMatroxCamera*)UserDataPtr;

	if(pData->m_bTriggerLive || pData->m_bFreeRunLive)
		return 0;

	theApp.m_fnWriteTactLog(_T(">> Timer Start : %3.3f"), pData->m_GrabTime.Stop(false));

	return 0;
}

long MFTYPE HookTimerEnd(long HookType, MIL_ID EventId, void *UserDataPtr)
{
	CMatroxCamera *pData = (CMatroxCamera*)UserDataPtr;

	if(pData->m_bTriggerLive || pData->m_bFreeRunLive)
		return 0;

	theApp.m_fnWriteTactLog(_T(">> Timer End : %3.3f"), pData->m_GrabTime.Stop(false));

	return 0;
}


void CMatroxCamera::WaitGrabEnd()
{	
	if (m_bFreeRunLive || m_bTriggerLive)
	{
		//MdigGrabWait(m_milDigitizer, M_FRAME_END);
		return;
	}

	if (m_bIsMultiShot)
	{
		WaitForSingleObject(m_hPreparePixelShift, 100);
		ResetEvent(m_hPreparePixelShift);
		m_fnGetPixelShiftData();
	}
	else
	{
		MdigGrabWait(m_milDigitizer, M_GRAB_END);
	}
	m_GrabTime.End();	
}

void CMatroxCamera::SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{
	if (m_lDigBand == 1)
	{
		MbufGet2d(m_milGrabBuffer, 0, 0, m_lDigSizeX * (nSeqMode + 1), m_lDigSizeY * (nSeqMode + 1), theApp.m_pSharedMemory->GetImgAddress(nGrabNum));
	}
	else if (m_lDigBand == 3)
	{
		// YSKIM
		// 배이어 적용 후
		MbufBayer(m_milGrabBuffer, m_ColorImage, m_MilWBCoefficients, M_BAYER_GR);		// YUV to GRBG
																						// 공용메모리에 저장
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* Image = new byte[nBufferSize];
		MbufGetColor(m_ColorImage, M_PACKED + M_BGR24, M_ALL_BANDS, Image);
		memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), Image, nBufferSize);
		SAFE_DELETE_ARR(Image);
	}
}

void CMatroxCamera::SaveFileCurBuffer(TCHAR* strSavePath)
{
	if (m_lDigBand == 1)
	{
		m_fnMbufExport(strSavePath, m_milGrabBuffer);
	}
	else if (m_lDigBand == 3)
	{
		// YSKIM
		MbufBayer(m_milGrabBuffer, m_ColorImage, m_MilWBCoefficients, M_BAYER_GR);		// YUV to GRBG
		m_fnMbufExport(strSavePath, m_ColorImage);
	}
}

UINT CMatroxCamera::ThreadClearMultishotBuffers(LPVOID lParam)
{
	CMatroxCamera* pDigitizer = (CMatroxCamera*)lParam;

	if( pDigitizer->m_milImageLarge[pDigitizer->m_nSequenceMode] != NULL )
	{
		MbufClear(pDigitizer->m_milImageLarge[pDigitizer->m_nSequenceMode], 0);
	}

	theApp.m_fnWriteTactLog(_T(">> Clear Buffer3 : %3.3f"), pDigitizer->m_GrabTime.Stop(false));

	if (pDigitizer->m_lDigBit == 12)
		memset(pDigitizer->m_totalImageBuf,0,pDigitizer->m_BufLen);
	else if (pDigitizer->m_lDigBit == 8)
		memset(pDigitizer->m_totalImageBuf8Bit,0,pDigitizer->m_BufLen);

	theApp.m_fnWriteTactLog(_T(">> Clear Buffer4 : %3.3f"), pDigitizer->m_GrabTime.Stop(false));

	SetEvent(pDigitizer->m_hPreparePixelShift);	// 16.05.11 Add Exposure End Timming

	return 0;
}

MIL_ID CMatroxCamera::ConvertYUVtoGRBG(MIL_ID SrcImg)
{
	MbufBayer(SrcImg,	m_ColorImage, m_MilWBCoefficients,	M_BAYER_GR) ;		// YUV to GRBG

	return m_ColorImage;
}

void CMatroxCamera::GetBufferBGR24(void* DestImg, MIL_ID SrcImg)
{
	MbufGetColor(SrcImg, M_PACKED+M_BGR24, M_ALL_BANDS, DestImg);
}

void CMatroxCamera::GetWBCoefficient(float* pCoefficients)
{
	// YSKIM
	// 1. 그랩
	CameraExpose();
	WaitGrabEnd();
	ResetEvent(g_hExpEnd[0]);
	// 2. 그랩 완료 시 WB Cal 수행
	MbufBayer(m_milGrabBuffer, m_ColorImage, m_MilWBCoefficients, M_BAYER_GR + M_WHITE_BALANCE_CALCULATE);
	// 3. WB 보정값 반환
	MbufGet(m_MilWBCoefficients, (void *)pCoefficients);
}

void CMatroxCamera::SetWBCoefficient(float* pCoefficients)
{
	// YSKIM
	// 1. WB 보정값 넣음
	MbufPut(m_MilWBCoefficients, pCoefficients);
	// 2. WB 잘 넣어졌는지 확인
	float WhiteBalance[3] = { 0, };
	MbufGet(m_MilWBCoefficients, (void *)WhiteBalance);
}

void CMatroxCamera::DisplayLive(HWND hDispWnd)
{
	BOOL bRet = FALSE;

	bRet = StartLiveGrab();

	if (bRet)
	{
		if (hDispWnd == NULL)
		{
			MdispSelect(m_milDisplay, M_NULL);
			MdispSelect(m_milDisplay, m_milLiveGrabBuffer);
			//MdispSelect(m_milDisplay, m_LiveImage);
		}
		else
		{
			MdispSelectWindow(m_milDisplay, m_milLiveGrabBuffer, hDispWnd);
			//MdispSelectWindow(m_milDisplay, m_LiveImage, hDispWnd);
		}
	}
	else
	{
		AfxMessageBox(_T("Live Error !!!"));
	}
}

// 16.07.04 Live 화면 추가
BOOL CMatroxCamera::StartLiveGrab()
{
	BOOL bRet = FALSE;

	theApp.m_pLogWriter->m_fnWriteLog(_T("Start Live Grab"));

	SetSequenceMode(eNone);
	SetTriggerMode(eFreeRun);
	Sleep(10);

	m_GrabTime.Start();
	
	// Serial 통신 오류로 인해 Camera Mode 변경되지 않았을 경우 연속 Grab 호출 -> 사용 안함
	if (GetTriggerMode() != eFreeRun)
	{
		AfxMessageBox(_T("Trigger Mode Error !!"));
		bRet = FALSE;
// 		m_bTriggerLive = TRUE;
// 		m_fnSnapAndTrigger();
	}
	else
	{
		m_bFreeRunLive = TRUE;
		MdigGrabContinuous(m_milDigitizer, m_LiveImage);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CMatroxCamera::StopLiveGrab()
{
	BOOL bRet = FALSE;

	theApp.m_pLogWriter->m_fnWriteLog(_T("Stop Live Grab"));
	
	WaitGrabEnd();
	
	if (m_bTriggerLive)
	{
		m_bTriggerLive = FALSE;		
	}
	
	if (m_bFreeRunLive)
	{
		m_bFreeRunLive = FALSE;
		MdigHalt(m_milDigitizer);
	}

	if (m_eCamModel == eVA47M5_Series || m_eCamModel == eVNP_MX_Series || m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
		bRet = SetTriggerMode(eStandard);
	else
		bRet = SetTriggerMode(eOverlap);

	return bRet;
}

// Live Grab
void CMatroxCamera::m_fnSnapAndTrigger()
{
	MdigGrab(m_milDigitizer, m_LiveImage);
	// 47M Camera 에서 Tigger 신호 강제 발생 시 Timer 해제 안됨. 임시 조치
	//MdigControl(m_milDigitizer, M_GRAB_EXPOSURE+M_TIMER1, M_ACTIVATE);
}

// 현재 Live 화면의 픽셀 정보
int CMatroxCamera::GetLivePixelInfo(CPoint ptBuf)
{
	int nVal;
	if (m_lDigBit == 12)
	{
		WORD val;
		MbufGet2d(m_LiveImage, ptBuf.x, ptBuf.y, 1, 1, &val);
		nVal = (int)val;
	}
	else
	{
		BYTE val;
		MbufGet2d(m_LiveImage, ptBuf.x, ptBuf.y, 1, 1, &val);
		nVal = (int)val;
	}
	return nVal;
}

// 16.07.19 관심 영역 Mean Value 구하기
// 17.01.13 Mil-Lite 대비 내용 삭제
double CMatroxCamera::GetRegionMeanValue(MIL_ID milImage, const CRect rcROI)
{
// 	double		dMeanVal;
// 	MIL_ID		milResult = M_NULL;
// 	MIL_DOUBLE	mildValue;
// 
// 	milResult = MimAllocResult(m_milSystem, 1, M_STAT_LIST, M_NULL);
// 
// 	MIL_ID milCropBuffer = MbufChild2d(milImage,
// 										rcROI.left,
// 										rcROI.top,
// 										rcROI.Width(),
// 										rcROI.Height(),
// 										M_NULL);
// 
// 	// Gray 값 구하기
// 	MimStat(milCropBuffer, milResult, M_MEAN, M_NULL, M_NULL, M_NULL);
// 	MimGetResult(milResult, M_MEAN + M_TYPE_MIL_DOUBLE, &mildValue);
// 
// 	// Gray값 치환
// 	dMeanVal = (double)mildValue;
// 
// 	MbufFree(milCropBuffer);
// 	milCropBuffer = M_NULL;
// 	MimFree(milResult);
// 
// 	return dMeanVal;
	return 0;
}

MIL_ID CMatroxCamera::GetLiveGrabImage()
{
	//MbufCopy(m_LiveImage, m_milLiveGrabBuffer);

	//2019.04.11
	if (m_lDigBand == 1)
	{
		return m_milLiveGrabBuffer;
	}
	else if (m_lDigBand == 3)
	{
		MbufBayer(m_milLiveGrabBuffer, m_ColorImage, m_MilWBCoefficients, M_BAYER_GR);		// YUV to GRBG
		return m_ColorImage;
	}

	return m_milLiveGrabBuffer;
}
// 2017.01.09 add by ikm GUI Image Load 용 - S
// MIL_ID CMatroxCamera::GetManualImage(int nImgCnt, BOOL bIsGrayImg)
// {
// 	//MbufCopy(m_LiveImage, m_milLiveGrabBuffer);
// 	if(bIsGrayImg)
// 		return m_milLoadImgBuff[nImgCnt];
// 	else
// 		return m_milLoadClrImgBuff[nImgCnt];
// }
// 2017.01.09 add by ikm GUI Image Load 용 - S

void CMatroxCamera::SetDisplayImage(MIL_ID milImage, HWND hDispWnd)
{
	MbufCopy(milImage, m_milDispImage);	
	
	if (hDispWnd == NULL)
	{
		MdispSelect(m_milDisplay, M_NULL);
		MdispSelect(m_milDisplay, m_milDispImage);
	}
	else
	{
		MdispSelectWindow(m_milDisplay, m_milDispImage, hDispWnd);
	}
}

// Grab 대기 후 MIL_ID Return (Camera Task 내부적으로 사용)
MIL_ID CMatroxCamera::GetMilGrabBuffer()
{
	CameraExpose();
	WaitGrabEnd();
	ResetEvent(g_hExpEnd[0]);

	if (m_lDigBand == 1)
	{
		return m_milGrabBuffer;
	}
	else if (m_lDigBand == 3)
	{
		// YSKIM
		MbufBayer(m_milGrabBuffer, m_ColorImage, m_MilWBCoefficients, M_BAYER_GR);		// YUV to GRBG
		return m_ColorImage;
	}

	return m_milGrabBuffer;
}

void CMatroxCamera::SaveGrabImage(CString strFilePath)
{
	MIL_ID milGrab;
	if (m_bTriggerLive || m_bFreeRunLive)
	{
	 	milGrab = GetLiveGrabImage();
	}
	else
	{
		//2019.04.11
	 	milGrab = GetMilGrabBuffer();
	}

	// White Balance 확인용
	//float WhiteBalance[3] = { 0, };
	//MbufGet(m_MilWBCoefficients, (void *)WhiteBalance);

	m_fnMbufExport(strFilePath, milGrab);
}

// 2017.05.11 add by ikm GUI -> Image SAVE 용 - S
void CMatroxCamera::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
	MIL_ID milGrab;
	char* filename = CSTR2PCH(strFilePath);
	Mat matSaveimage;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	int nDigBitOffset = (int)(m_lDigBit==8?0:2);

	if(bIsLiveImage) // LIVE 영상 호출인지 확인
	{

		if (m_bTriggerLive || m_bFreeRunLive)
		{
			milGrab = GetLiveGrabImage();
		}
		else
		{
			milGrab = GetMilGrabBuffer();
		}

		
		m_fnMbufExport(strFilePath, milGrab);
	}	
	else // GUI 이미지 호출
	{
// 		Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
// 		cv::imwrite(filename, m_srcImage[nImgCnt](rect));

		matSaveimage = m_srcImage[nImgCnt];
		int nChCnt = matSaveimage.channels();  int imgDepth = matSaveimage.depth();
		if(nChCnt == 1 && imgDepth == CV_16U) matSaveimage.convertTo(matSaveimage, CV_8UC1, 1./16.);		
		cv::imwrite(filename, matSaveimage(rect));

		SAFE_DELETE_ARR(filename);
	}
	
	
}

BOOL CMatroxCamera::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
{
	if( matSrcBuffer.empty() )
		return FALSE;

	cv::Mat matTempBuffer = cv::Mat::zeros(matSrcBuffer.size(), CV_8UC1);
	cv::Mat matRotation = cv::getRotationMatrix2D( Point(matSrcBuffer.cols/2, matSrcBuffer.rows/2), dAngle, 1.0 );

	cv::warpAffine( matSrcBuffer, matTempBuffer, matRotation, matTempBuffer.size() );

	if( !matDstBuffer.empty() )
		matDstBuffer.release();

	matDstBuffer = matTempBuffer.clone();

	matTempBuffer.release();
	matRotation.release();

	return TRUE;
}


// 2017.05.11 add by ikm GUI -> Image SAVE 용 - E

// 원본 이미지
void CMatroxCamera::GetGrabImage(byte* byteImgArr)
{
	MIL_ID milGrab;
	milGrab = GetMilGrabBuffer();

	MbufGet2d(milGrab, 0, 0, m_lDigSizeX, m_lDigSizeY, &byteImgArr);
}

// Crop Image
void CMatroxCamera::GetGrabImage(int nImgCnt, BOOL bIsLiveImage/*, BOOL bIsGrayImg*/, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	MIL_ID milGrab, milCrop;
	Mat matResize, matLive;

	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	//Sleep(10);
	if(bIsLiveImage) // LIVE 영상 호출인지 확인
	{
		if (m_bTriggerLive || m_bFreeRunLive)
		{
			milGrab = GetLiveGrabImage();
		}
		else
		{
			milGrab = GetMilGrabBuffer();
		}

		if(m_lDigBand == 1 && m_lDigBit == 12 )
		{
			MbufTransfer(milGrab, m_milCropImage,
				lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, M_ALL_BANDS,
				0, 0, lWindowSizeX, lWindowSizeY, M_ALL_BANDS,
				M_COPY + M_SCALE, M_DEFAULT, M_NULL, M_NULL);

			byte* pByteInput = new byte[lWindowSizeX * lWindowSizeY * CV_16U];
			MbufGet2d(m_milCropImage, 0, 0, lWindowSizeX, lWindowSizeY, pByteInput);			
			matResize = cv::Mat(lWindowSizeX, lWindowSizeY, CV_16UC1, pByteInput);			
			matResize.convertTo(matResize, CV_8UC1, 1./16.);
			memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
			SAFE_DELETE_ARR(pByteInput);

		}
		else if (m_lDigBand == 3 && m_lDigBit == 8)
		{
			// YSKIM
			int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
			byte* Image = new byte[nBufferSize];
			MbufGetColor(milGrab, M_PACKED + M_BGR24, M_ALL_BANDS, Image);
			matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);
			cv::resize(matLive(rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY), 0, 0, CV_INTER_AREA);
			memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
			SAFE_DELETE_ARR(Image);
		}
		else
		{
			MbufTransfer(milGrab, m_milCropImage,
				lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, M_ALL_BANDS,
				0, 0, lWindowSizeX, lWindowSizeY, M_ALL_BANDS,
				M_COPY + M_SCALE, M_DEFAULT, M_NULL, M_NULL);

			MbufGet2d(m_milCropImage, 0, 0, lWindowSizeX, lWindowSizeY, byteImgArr);
		}
	}	
	// 2017.01.09 add by ikm GUI Image Load 용 - S
	else // GUI 이미지 호출
	{		
		//Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);	
		cv::resize( m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);	

		int nChCnt = matResize.channels();  int imgDepth = matResize.depth();
		if(nChCnt == 1 && imgDepth == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);
		memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
	}
// 	int nChCnt = matResize.channels();  int imgDepth = matResize.depth();
// 	if(nChCnt == 1 && imgDepth == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		

}


// 입력값 : 원하는 평균 밝기 & 밝기를 맞추고자 하는 영역
// 출력값 : 노출시간 & 게인
void CMatroxCamera::GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
{	
	// 노출 시간 초기값
	int nExp = 5;
	int nExpStep = 200;
	const int nMaxExp = 601;

	// 카메라 게인 초기값
	int nGain = 30;
	int nGainStep = 30;
	const int nMaxGain = 300;

	if (m_eCamModel == eVA47M5_Series || m_eCamModel == eVNP_MX_Series || m_eCamModel == eHuaRayPS || m_eCamModel == eHuaRayNonPS)
		SetTriggerMode(eStandard);
	else
		SetTriggerMode(eOverlap);

	SetSequenceMode(eNone);

	SetExposureTime(nMaxExp);

	// Gain 조절 Loop
	while (nGain <= nMaxGain)
	{
		SetAnalogGain(nGain);
		
		MIL_ID milBuf = GetMilGrabBuffer();

		// 평균 GV값 계산
		double dMean = GetMeanValue(milBuf, rect);

		// 현재 평균GV가 원하는 평균 GV보다 낮은 경우, Gain 올려서 다시 Loop ( Exp는 최대 상태 )
		// GV가 딱 맞는 경우도 있을수 있음 ( 2GV 정도 더 남겨 둠 )
		if (nWantMean + 2 > dMean)
			nGain += nGainStep;
		// 만족 하는 경우 Loop 나가기
		else
			break;
	}

	// 최대치보다 게인값이 높은 경우, 만족하는 셋팅 없음 ( Exp는 최대 상태 )
	if (nGain > nMaxGain)	return;

	SetAnalogGain(nGain);

	// Exp 조절 Loop ( 0보다 큰 경우만 )
	while (nExpStep > 0)
	{
		SetExposureTime(nExp + nExpStep);

		// 라이브 영상
		MIL_ID milBuf = GetMilGrabBuffer();

		// 평균 GV값 계산
		double dMean = GetMeanValue(milBuf, rect);

		// 원하는 평균 밝기값과 현재 평균 밝기값 비교
		if (nWantMean > dMean)
			nExp += nExpStep;
		// Step 반으로 해서 다시 시도
		else 
			nExpStep /= 2;
	}

	// 결과
	nOutExp = nExp;
	nOutGain = nGain;
}


BOOL CMatroxCamera::OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
{
	BOOL bRet = FALSE;
	m_eCamModel = eModel;
	bRet = m_SerialCamera.ConnectCameraSerial(nComPort, nBaudrate, eModel);

	return bRet;
}

BOOL CMatroxCamera::InitializeGrabber(int nGrabberNo, eBoardModel eGrabberModel, int nDigCh, CString strDcfFilePath)
{
	// MIL APP - SYSTEM
	MappAlloc(M_DEFAULT,&m_milApplication);

	// 17.11.08 Board Model 에 따라 분기하는 구문 추가
	switch (eGrabberModel)
	{
	case eSolios:
		MsysAlloc(M_SYSTEM_SOLIOS, nGrabberNo, M_DEFAULT, &m_milSystem);
		break;
	case eRadientCXP:
		MsysAlloc(M_SYSTEM_RADIENTCXP, M_DEFAULT, M_DEFAULT, &m_milSystem);
		break;
	default:
		AfxMessageBox(_T("Not Supported Board Model !!!"));
		break;
	}

	if (m_milSystem == M_NULL)
		MsysAlloc(M_SYSTEM_HOST, M_DEFAULT,  M_DEFAULT, &m_milSystem);		// for Test Mode

	if (!m_milSystem)
		CloseGrabber();


	if (!m_fnInitializeDigitizer(strDcfFilePath, nDigCh))
		return FALSE;

	if (!m_fnInitializeImageBuffer())
		return FALSE;

	// 2016.03.31 P/S 기능 확장 - H, V 선택 기능
	int nRatio = m_nSequenceMode + 1;
	m_bUseHPS	?	m_nHRatio = nRatio	:	m_nHRatio = 1;	// Horizontal P/S 기능 사용 여부
	m_bUseVPS	?	m_nVRatio = nRatio	:	m_nVRatio = 1;	// Vertical P/S 기능 사용 여부

	m_nMaxBuf = nRatio * nRatio;
	if (m_nSequenceMode == 0)
		m_bIsMultiShot = FALSE;
	else
		m_bIsMultiShot = TRUE;	

	// Color Camera 현재 사용 안함
	if (m_lDigBand > 1)
	{
		// 프로그램 시작 시 저장된 Exp.On Time / White Balance 보정치 설정
		// 		float CoefficientWB[3] = {0.0, 0.0, 0.0};
		// 
		// 		CoefficientWB[0] = m_fWhiteBalance_R;
		// 		CoefficientWB[1] = m_fWhiteBalance_G;
		// 		CoefficientWB[2] = m_fWhiteBalance_B;
		// 
		// 		SetWBCoefficient(&CoefficientWB[0]);	// White Balance 값
	}

	return TRUE;
}

void CMatroxCamera::DisplayGrabImage(HWND hDispWnd)
{
	MIL_ID milGrab;
	milGrab = GetMilGrabBuffer();

	SetDisplayImage(milGrab);
}

double CMatroxCamera::GetMeanValue(MIL_ID milBuf, CRect rect)
{
	if( milBuf == M_NULL )	return 0;

	BYTE *ptr = (BYTE *)MbufInquire(milBuf, M_HOST_ADDRESS, M_NULL);
	int nW = (int)MbufInquire(milBuf, M_SIZE_X, M_NULL);
	int nH = (int)MbufInquire(milBuf, M_SIZE_Y, M_NULL);
	int nS = (int)MbufInquire(milBuf, M_PITCH_BYTE, M_NULL);

	if( rect.left < 0 )			rect.left	= 0;
	if( rect.top < 0 )			rect.top	= 0;
	if( rect.right >= nW )		rect.right	= nW - 1;
	if( rect.bottom >= nH )		rect.bottom	= nH - 1;

	if( rect.Width() <= 0 )		return 0;
	if( rect.Height() <= 0 )	return 0;

	long long nSum = 0;
	for(int y=rect.top ; y<=rect.bottom ; y++)
	{
		BYTE *pTemp = &ptr[y*nS+rect.left];
		for(int x=rect.left ; x<=rect.right ; x++ )
		{
			nSum += *pTemp++;
		}
	}

	return nSum / (double)( rect.Width()*rect.Height() );
}

void CMatroxCamera::TestFunc( int nGV )
{
	// 노출 시간
	int nExp = 5;
	int nExpStep = 200;
	const int nMaxExp = 601;

	// 카메라 게인
	int nGain = 30;
	int nGainStep = 30;
	const int nMaxGain = 300;

	int nWantMean = nGV;

	SetExposureTime(nMaxExp);

	// Gain 조절 Loop
	while (nGain <= nMaxGain)
	{
		SetAnalogGain(nGain);

		MIL_ID milBuf = GetMilGrabBuffer();

		// 평균 GV값 계산
		double dMean = GetMeanValue(milBuf, CRect(400, 800, 5900, 3000));

		// 현재 평균GV가 원하는 평균 GV보다 낮은 경우, Gain 올려서 다시 Loop ( Exp는 최대 상태 )
		// GV가 딱 맞는 경우도 있을수 있음 ( 2GV 정도 더 남겨 둠 )
		if (nWantMean + 2 > dMean)
			nGain += nGainStep;
		// 만족 하는 경우 Loop 나가기
		else
			break;
	}

	// 최대치보다 게인값이 높은 경우, 만족하는 셋팅 없음 ( Exp는 최대 상태 )
	if (nGain > nMaxGain)	return;

	SetAnalogGain(nGain);

	// Exp 조절 Loop ( 0보다 큰 경우만 )
	while (nExpStep > 0)
	{
		SetExposureTime(nExp + nExpStep);

		// 라이브 영상
		MIL_ID milBuf = GetMilGrabBuffer();

		// 평균 GV값 계산
		double dMean = GetMeanValue(milBuf, CRect(400, 800, 5900, 3000));

		// 원하는 평균 밝기값과 현재 평균 밝기값 비교
		if (nWantMean > dMean)
			nExp += nExpStep;
		// Step 반으로 해서 다시 시도
		else 
			nExpStep /= 2;
	}

	CString str;
	str.Format(_T("Exp : %d, Gain : %d"), nExp, nGain);
	AfxMessageBox(str);

	//////////////////////////////////////////////////////////////////////////
/*
	MIL_ID milTest = MbufImport(theApp.GETDRV() + _T(":\\Saveimage.bmp"), M_DEFAULT, M_RESTORE, m_milSystem, M_NULL);

// 	m_fnMbufExport(theApp.GETDRV() + _T(":\\1234\\test\\123456\\test.bmp"), milTest);
// 	MbufExport(theApp.GETDRV() + _T(":\\TEST11111111111.bmp"), M_BMP, milTest);
	//byte* byteTest = new byte[657*438];

	//m_GrabTime.Start();
	//for (int i=0; i<1000; i++)
	//{
	//	MbufTransfer(milTest, m_milCropImage, 
	//		0, 0, 6576, 4384, M_ALL_BANDS,
	//		0, 0, 657, 438, M_ALL_BANDS,
	//		M_COPY+M_SCALE, M_DEFAULT, M_NULL, M_NULL);
	//	MbufGet2d(m_milCropImage, 0, 0, 657, 438, byteTest);
	//}

	m_milLiveGrabBuffer = milTest;
// 	theApp.m_fnWriteTactLog(_T("Transfer : %3.3f"), m_GrabTime.Stop(false));
// 	MbufExport(theApp.GETDRV() + _T(":\\TEST2222222222.bmp"), M_BMP, m_milCropImage);
// 	MbufPut2d(m_milCropImage, 0, 0, 657, 438, byteTest);
// 	MbufExport(theApp.GETDRV() + _T(":\\TEST333333333333.bmp"), M_BMP, m_milCropImage);
*/
}

// 2017.01.09 add by ikm GUI 이미지 로드 시 MIL 버퍼 저장 (color, gray image 구분) - S  
void CMatroxCamera::m_fnSetBufImage(TCHAR* strImgPath, int nImgCnt, int& nChcnt, int &nWidth, int &nHeight, double dAngle = 0.0F)
{
	//MIL_ID milTest = MbufImport(theApp.GETDRV() + _T(":\\Saveimage.bmp"), M_DEFAULT, M_RESTORE, m_milSystem, M_NULL);
	//m_bIsLive = TRUE;	
// 	CString strImgPath = theApp.GETDRV() + _T(":\\ResponseImage.bmp");
// 	CFile file(strImgPath, CFile::modeCreate | CFile::modeWrite);  // image 버퍼를 bmp 파일로 저장
// 	file.Write(imageBub, nBufSize); 
// 	file.Close();
	char strTempPath[1000] = {0,};
	WideCharToMultiByte(CP_ACP, 0, strImgPath, 1000, strTempPath, 1000, NULL, NULL);	
	
	m_srcImage[nImgCnt] = cv::imread(strTempPath, IMREAD_UNCHANGED); 
	if(dAngle != 0.0F)DoRotateImage( m_srcImage[nImgCnt],  m_srcImage[nImgCnt], dAngle);
	nChcnt = m_srcImage[nImgCnt].channels();
	nWidth = m_srcImage[nImgCnt].size().width;
	nHeight = m_srcImage[nImgCnt].size().height;

// 2017.01.09 add by ikm GUI 이미지 로드 시 MIL 버퍼 저장 (color, gray image 구분) - E
}


void CMatroxCamera::m_fnMbufExport(CString strFilePath, MIL_ID milBuffer)
{
	// 파일 경로 생성
	SHCreateDirectoryEx(NULL, strFilePath.Left(strFilePath.GetLength() - (strFilePath.GetLength() - strFilePath.ReverseFind(_T('\\')))), NULL);

	if (m_lDigBit == 8 && m_lDigBand == 1)
	{
		MbufExport(strFilePath, M_BMP, milBuffer);
	}
	else if (m_lDigBit == 8 && m_lDigBand == 3)
	{
		// YSKIM
		// BMP 저장하고싶은데 밀 함수 쓰면 BMP 32bit가 되버려서 cv 함수사용하여 저장
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* Image = new byte[nBufferSize];
		MbufGetColor(milBuffer, M_PACKED + M_BGR24, M_ALL_BANDS, Image);
		Mat mat = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);
		cv::imwrite(CSTR2PCH(strFilePath), mat);
		SAFE_DELETE_ARR(Image);
	}
	else
	{
		MbufExport(strFilePath, M_TIFF, milBuffer);
	}
}

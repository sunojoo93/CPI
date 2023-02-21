// Digitizer.h: interface for the CSimulationCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
#define AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_

//#include <mil.h>
#include "CameraInterface.h"
#include "CPUTimer.h"
#include "define.h"
#include "..\..\CommonHeaders\Structure.h"




#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define BUFFREE(milBuffer)		if(milBuffer)	{ MbufFree	(milBuffer);	milBuffer=M_NULL;	}

// 16.03.16 pixel shift 기능 추가
//#define MAX_IMAGE_RATIO		2
#define MAX_BUFFER_NUMBER	MAX_IMAGE_RATIO * MAX_IMAGE_RATIO
#define MAX_CAM_PULSE_TIME	2000

#define WINDOWSIZE_X	988
#define WINDOWSIZE_Y	644

#define IMAGESIZE_X		6576
#define IMAGESIZE_Y		4384


// for test0627
const int TESTOFFSET = 100;
const int TESTCROPSIZE = 100;


class CSimulationCamera : public CameraInterface
{
public:
	CSimulationCamera();
	virtual ~CSimulationCamera();

	// virtual //////////////////////////////////////////////////////////////////////////////////
	// Init
	BOOL			InitializeCamera(CString strInitFilePath);
	// 순수 가상함수
	void			CameraExpose(ST_LINE_INFO stLine)									{	return					;};
	BOOL			DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle);
	void			GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY);
	void			WaitGrabEnd();
	void			SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode);
	void			SaveFileCurBuffer(TCHAR* strSavePath);

	BOOL			StartLiveGrab();			// 16.06.30 Live 화면 출력 추가 by CWH
	BOOL			StopLiveGrab();
	void			SetSMemBuffer(UINT nGrabNum, bool bSimulationMode);			// 현재 Grab Image Shared Memory 에 적재
	int				SetSharedMemForSimulation(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName);	       //2017.01.09 sgkim 시뮬레이션 모드

	// 순수 가상함수
	BOOL			SetAnalogGain(double nGain)						{	return TRUE				;};
	BOOL			SetDataBit(int nDataBit)						{	return TRUE				;};

	// Get
	int				GetImageWidth()									{ return (int)m_lDigSizeX;	}
	int				GetImageHeight()								{ return (int)m_lDigSizeY;	}
	int				GetImageBitrate()								{ return (int)m_lDigBit;	}
	int				GetImageBandwidth()								{ return (int)m_lDigBand;	}
	int				GetImageSize()									{ return (int)m_lDigSizeX * (int)m_lDigSizeY * (int)m_lDigBand * (int)(m_lDigBit==8?1:2);	}

	double			GetExposureTime()								{	return 0		;};
	double			GetAnalogGain()									{	return 0		;};
	int				GetTriggerMode()								{	return 0		;};
	int				GetSequenceMode()								{	return 0		;};	

	// 순수 가상함수
	void			GetGrabImage(byte* byteImgArr)					{	return			;};
	int				GetDataBit()									{	return 8		;};


	BOOL	SetExposureTime(double nExpTime)					{	return TRUE	;};
	BOOL	SetStitchMode(int nCamMode)							{	return TRUE ;};		// 180809 YSS
	BOOL	SetAnalogGain(int nGain)							{	return TRUE	;};	
	BOOL	SetTriggerMode(int nTrigMode)						{	return TRUE	;};
	BOOL	SetSequenceMode(int nSeqMode)						{	return TRUE	;};	
	void	m_fnSetBufImage(TCHAR* strImgPath, int nImgCnt, int& nChCnt, int &nWidth, int &nHeight, double dAngle);
	void    SaveGrabImage(CString strFilePath);
	void	SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath);
	double  GetMeanValue(cv::Mat &matBuf, CRect rect);

	void	GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain );

	int				GetImageCallBackState(){return 0;}
	void			SetImageCallBackState(int nState);
	UINT			retryConnect(){return 0;}
	// Set


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// 16.07.04 Live 사용 여부
	BOOL			m_bIsLive;


private:
	int		m_lDigSizeX;
	int		m_lDigSizeY;
	int		m_lDigBit;
	int		m_lDigBand;

	TCHAR				m_strSimulationImagePath[200];
	TCHAR				m_strSimulationLiveImagePath[200];
	
	int					m_nSimulationGrabTime;
	cv::Mat m_srcImage[NUM_LOAD_IMAGE_MAX];
	cv::Mat m_liveImage;
	
};

#endif // !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)

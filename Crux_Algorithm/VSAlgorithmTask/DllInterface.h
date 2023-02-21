////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare Algorithm DLL Interface Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DLL_INTERFACE_H
#define DLL_INTERFACE_H

#pragma once

#include "Define.h"

//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "InspectLib.lib")


//////////////////////////////////////////////////////////////////////////
// Align 관련
//////////////////////////////////////////////////////////////////////////

// Top Line 각도 찾기
EXTERNC DLLAPI long	Align_FindTheta(cv::Mat matSrcBuf, double* dPara, double& dThet, cv::Point& ptCentera=cv::Point(), wchar_t* strID=NULL);

// 검사 영역 찾기
EXTERNC DLLAPI long	Align_FindActive(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nCameraNum, int nEQType,double dCamResolution, 
	double dPannelSizeX,double dPannelSizeY ,int nRatio=1, cv::Point& ptCenter=cv::Point(), wchar_t* strID=NULL);

EXTERNC DLLAPI long	Align_FindActive_LCP(cv::Mat matSrcBuf, cv::Mat matDrawBuf, cv::Mat matBackStage,cv::Mat matSrcStage,cv::Mat matMask_Active,cv::Mat matMask_Hole ,int nColorCode ,double* dPara, double& dTheta, cv::Point* ptResCorner,cv::Rect2i& rcCellRoi,int nAlignImageClassify,int nCameraNum, int nImageClassify, int nEQType, double dCamResolution,
	double dPannelSizeX, double dPannelSizeY, int nRatio = 1, cv::Point& ptCenter = cv::Point(), cv::Point& ptAlignCenter = cv::Point(), wchar_t* strID = NULL ,  wchar_t* strAlgPath = NULL);

EXTERNC DLLAPI long	Align_FindActive_LCP_Mask(cv::Mat matSrcBuf, cv::Mat& matDstActive, cv::Mat& matDstLabel, cv::Mat& matDstHole, double* dPara ,int nCameraNum, int nImageClassify, int nEQType, double dCamResolution,
	double dPannelSizeX, double dPannelSizeY,int nColorCode ,int nRatio = 1, cv::Point& ptCenter = cv::Point(), wchar_t* strID = NULL,  wchar_t* strAlgPath = NULL);

EXTERNC DLLAPI long	Align_FindActive_LCP_BACK(cv::Mat matSrcBuf, cv::Mat& matBackStageBuf, double* dPara, cv::Rect2i& rcActiveRoi, int nAlignImageClassify, int nCameraNum, int nImageClassify, int nEQType, double dCamResolution,
	double dPannelSizeX, double dPannelSizeY, int nRatio = 1, cv::Point& ptStandard = cv::Point(), wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);

EXTERNC DLLAPI long	Align_FindActive_LCP_Vaccum(cv::Mat matSrcBuf, cv::Mat& matVaccumHoleBuf, double* dPara, cv::Rect2i& rcActiveRoi, int nAlignImageClassify, int nCameraNum, int nImageClassify, int nEQType, double dCamResolution,
	double dPannelSizeX, double dPannelSizeY, int nRatio = 1, cv::Point& ptStandard = cv::Point(), wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);
// 외곽 곡선 설정 & 파일 저장
EXTERNC DLLAPI long	Align_SetFindContour(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt,
	double* dPara, int nAlgImg, int nCameraNum, int nRatio, int nEqpType, wchar_t* strPath, Point* ptAlignCorner=NULL, CStringA strImageName=NULL, double dAlignTheta=0, bool bIamgeSave=false);

// Camera Hole
EXTERNC DLLAPI long	Align_SetFindContour_(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], INSP_AREA CHoleROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, int nCHoleROICnt,
	double* dPara, int nAlgImg, int nCameraNum, int nRatio, int nEqpType, wchar_t* strPath, Point* ptAlignCorner = NULL, CStringA strImageName = NULL, double dAlignTheta = 0, bool bIamgeSave = false);

// 외곽 부분 처리
EXTERNC DLLAPI long	Align_FindFillOutArea(cv::Mat matSrcBuf, cv::Mat matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], ROUND_SET tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA], cv::Mat* matCHoleROIBuf, cv::Rect* rcCHoleROI, bool* bCHoleAD,
	double* dPara, int nAlgImg, int nCameraNum, int nRatio, int nEqpType, wchar_t* strAlgLog, wchar_t* strID=NULL, cv::Point* ptCorner=NULL, vector<vector<Point2i>> &ptActive = vector<vector<Point2i>>(), double dAlignTH=0,  CString strPath=NULL, bool bImageSave = false);

// 외곽 부분 처리
EXTERNC DLLAPI long	Align_FindFillOutAreaDust(cv::Mat matSrcBuf, cv::Point ptResCorner[E_CORNER_END], double dAngle, cv::Rect* rcCHoleROI, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], ROUND_SET tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA],
	double* dPara, int nAlgImg, int nRatio, wchar_t* strAlgLog, wchar_t* strID = NULL);

// 영상 회전
EXTERNC DLLAPI long	Align_RotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle);

// 좌표 회전
EXTERNC DLLAPI long	Align_DoRotatePoint(cv::Point matSrcPoint, cv::Point& matDstPoint, cv::Point ptCenter, double dAngle);

// AD 검사 ( AVI, SVI ) / dResult : 현재 Cell 일치율
EXTERNC DLLAPI long	Align_FindDefectAD(cv::Mat matSrcBuf, double* dPara, double* dResult, int nRatio, int nCamera, int nEqpType);

// AD GV 검사
EXTERNC DLLAPI long	Align_FindDefectAD_GV(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner, int nEqpType, int nImageNum, wchar_t* strAlgLog);

//PANEL CURL
EXTERNC DLLAPI long	Panel_Curl_Judge(cv::Mat& matSrcBuf, double* dPara, cv::Point* ptCorner, BOOL &bCurl, stMeasureInfo* stCurlMeasure,BOOL bSaveImage, CString strPath);

//LCP Auto Cal 
EXTERNC DLLAPI long	Align_AutoCal_Make_Mask(cv::Mat matBackBuf, cv::Mat matEmptyBuf, cv::Mat matSideBuf, CString ColorCode, CString T_Judge_Code,CString Path_Result ,double* dPara, int nEqpType);

//LCP Auto Cal 
EXTERNC DLLAPI long	Align_AutoCal(cv::Mat matSrcBuf, cv::Mat matActiveMask, cv::Mat matLabelMask,CString PattenName ,CString ColorCode, CString T_Judge_Code, CString Path_Result, int nMinGV, int nMaxGV, double& MeanGV, bool& Judge_OK_NG, double* dPara, int nEqpType);
//////////////////////////////////////////////////////////////////////////
// Pol
//////////////////////////////////////////////////////////////////////////

// POL MARK Image Load
EXTERNC DLLAPI long	PM_ImageLoad(int nImageNum, wchar_t* strPath);


//////////////////////////////////////////////////////////////////////////
// CCD Defect
//////////////////////////////////////////////////////////////////////////

// Load
EXTERNC DLLAPI long	CCD_IndexLoad(CString strPath, CString strPath2);

// Save
EXTERNC DLLAPI long	CCD_IndexSave(cv::Mat& matSrcBuffer, CString strPath, CString strPath2);


//////////////////////////////////////////////////////////////////////////
// SVI 컬러 보정 관련
//////////////////////////////////////////////////////////////////////////

EXTERNC DLLAPI long	ColorCorrection_Load(CString strPath);


//////////////////////////////////////////////////////////////////////////
// Point Defect
//////////////////////////////////////////////////////////////////////////

// Point 불량 검사
EXTERNC DLLAPI long	Point_FindDefect(cv::Mat matSrcBuffer, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, wchar_t* strAlgPath, wchar_t* strBubblePath, wchar_t* strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, wchar_t* strAlgLog , cv::Mat* matCholeBuffer);

EXTERNC DLLAPI long	Point_FindDefect_Transparency(cv::Mat matSrcBuffer, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, wchar_t* strAlgLog, cv::Mat* matCholeBuffer, cv::Mat matSrc_Stage);

EXTERNC DLLAPI long	Point_FindDefect_Black(cv::Mat matSrcBuffer, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, wchar_t* strAlgPath, wchar_t* strBubblePath, wchar_t* strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, wchar_t* strAlgLog, cv::Mat* matCholeBuffer);

// Dust 제거 후, 결과 벡터 넘겨주기
EXTERNC DLLAPI long	Point_GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer ,cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer,
	cv::Point* ptCorner, double* dPara, double* dAlgPara,int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, bool bBubble = false);

//////////////////////////////////////////////////////////////////////////
//LCPLine Defect
//////////////////////////////////////////////////////////////////////////
EXTERNC DLLAPI long	LCPLine_FindDefect(cv::Mat matSrcBuffer, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active, cv::Mat matMask_Hole,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, wchar_t* strAlgPath, wchar_t* strBubblePath, wchar_t* strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, wchar_t* strAlgLog, cv::Mat* matCholeBuffer);

EXTERNC DLLAPI long	LCPLine_GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
	cv::Point* ptCorner, double* dPara, double* dAlgPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, bool bBubble = false);

//////////////////////////////////////////////////////////////////////////
//Moisture Defect
//////////////////////////////////////////////////////////////////////////
EXTERNC DLLAPI long	Moisture_FindDefect(cv::Mat matSrcBuffer, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active, cv::Mat matMask_Hole,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, wchar_t* strAlgPath, wchar_t* strBubblePath, wchar_t* strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, wchar_t* strAlgLog, cv::Mat* matCholeBuffer);

EXTERNC DLLAPI long	Moisture_GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
	cv::Point* ptCorner, double* dPara, double* dAlgPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, bool bBubble = false);

//////////////////////////////////////////////////////////////////////////
//Variance Defect
//////////////////////////////////////////////////////////////////////////
EXTERNC DLLAPI long	Variance_FindDefect(cv::Mat matSrcBuffer, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, wchar_t* strAlgPath, wchar_t* strBubblePath, wchar_t* strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, wchar_t* strAlgLog, cv::Mat* matCholeBuffer);

EXTERNC DLLAPI long	Variance_GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
	cv::Point* ptCorner, double* dPara, double* dAlgPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, bool bBubble = false);

//////////////////////////////////////////////////////////////////////////
//Concealment Defect
//////////////////////////////////////////////////////////////////////////
EXTERNC DLLAPI long	Concealment_FindDefect(cv::Mat matSrcBuffer, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, wchar_t* strAlgPath, wchar_t* strBubblePath, wchar_t* strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, wchar_t* strAlgLog, cv::Mat* matCholeBuffer);

EXTERNC DLLAPI long Concealment_GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
	cv::Point* ptCorner, double* dPara, double* dAlgPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, bool bBubble = false);


//////////////////////////////////////////////////////////////////////////
// Line Defect
//////////////////////////////////////////////////////////////////////////

// Line 불량 검사
EXTERNC DLLAPI long Line_FindDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Mat matBKBuffer, vector<int> NorchIndex, CPoint OrgIndex, cv::Point* ptCorner, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, wchar_t* strContourTxt = NULL);

//////////////////////////////////////////////////////////////////////////
// Mura Defect
//////////////////////////////////////////////////////////////////////////

// Mura 불량 검사
EXTERNC DLLAPI long Mura_FindDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog);
//woojin 19.08.27
EXTERNC DLLAPI long Mura_FindDefect2(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog);

EXTERNC DLLAPI long Mura_FindDefect3(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, cv::Mat& matDrawBuffer,wchar_t* strContourTxt = NULL);

// Mura4에서 사용함
EXTERNC DLLAPI long Mura_FindDefect4(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, cv::Mat& matDrawBuffer, wchar_t* strContourTxt = NULL);

// Mura Chole에서 사용함	2021.01.06 HGM
EXTERNC DLLAPI long Mura_FindDefect_Chole(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, cv::Rect* rcCHoleROI, cv::Mat* matCholeBuffer, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, cv::Mat& matDrawBuffer, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long Mura_FindDefect_Scratch(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog, cv::Mat& matDrawBuffer, wchar_t* strContourTxt = NULL);

// Dust 제거 후, 결과 벡터 넘겨주기
EXTERNC DLLAPI long Mura_GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
	double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt, wchar_t* strAlgLog);

// Dust 제거 후, 결과 벡터 넘겨주기
EXTERNC DLLAPI long Mura_GetDefectList2(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
	double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt, wchar_t* strAlgLog);

//////////////////////////////////////////////////////////////////////////
// Mura Normal Defect
//////////////////////////////////////////////////////////////////////////

// Mura 불량 검사
EXTERNC DLLAPI long MuraNormal_FindDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat matBKBuffer, cv::Mat& matDstBright, cv::Mat& matDstDark,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog);

// Dust 제거 후, 결과 벡터 넘겨주기
EXTERNC DLLAPI long MuraNormal_GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
	double* dPara, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt, wchar_t* strAlgLog);



//////////////////////////////////////////////////////////////////////////
// SVI Defect
//////////////////////////////////////////////////////////////////////////

// SVI Main 검사 알고리즘
EXTERNC DLLAPI long	SVI_FindMuraDefect(cv::Mat matSrcBuf, cv::Mat matBKBuf, cv::Mat& matDrawBuf, cv::Point* ptCorner, double* dPara, int* nCommonPara,
	wchar_t strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strAlgLog);

//////////////////////////////////////////////////////////////////////////
// Edge Defect
//////////////////////////////////////////////////////////////////////////
//2017.06.03 외관 Edge 검사 추가
EXTERNC DLLAPI long APP_FindDefect_CCut(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, CString strCurModelPath, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long APP_FindDefect_Cutting(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long	APP_PanelMeasure(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,   double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, Mat& mtMeasureImage, CString strCurModelPath, vector<CRect> pFilterROI, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long APP_FindDefect_PadScartch(cv::Mat matSrcBuffer, cv::Mat matRefer, cv::Mat matDust, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CString strCurRecipePath, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long APP_FindDefect_ActiveCorner(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, vector<vector<Point2i>> ptRound, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long APP_FindDefect_ActiveBurnt(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stActiveBurntROI* stROI, CString strCurRecipePath, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long APP_FindDefect_ActiveDust(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

EXTERNC DLLAPI long APP_FindDefect_PadDust(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara,
	wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

#endif

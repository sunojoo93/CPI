
// VSAlgorithmTask.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.

#include "TaskInterfacer.h"
#include "Config.h"
#include "CPUTimer.h"
#include "..\..\CommonHeaders\Structure.h"
#include "InspectPanel.h"
#include "Define.h"
#include "LogWriter.h"
#include "VSAlgorithmTaskDlg.h"
#include "DefineInterface.h"
#include "DiskInfo.h"

#include <map>
#include <list>

#define DRIVE_CHECK_INTERVAL 30000

#include "DefineInterface.h"

// ���� �� ���� ���丮 ��� �����ϴ� �Լ��� ����
inline void DeleteAllFilesWithDirectory(CString dirName, bool bRecurse = FALSE)
{
    CFileFind finder;
       
    BOOL bWorking = finder.FindFile((CString)dirName + "/*.*");
       
    while(bWorking)
    {
        bWorking = finder.FindNextFile();
        if(finder.IsDots())
        {
			continue;
        }
		else if(finder.IsDirectory())
		{
			// ���ȣ��
			DeleteAllFilesWithDirectory(finder.GetFilePath(), TRUE);
		}
		else
		{
			if (!finder.IsSystem())
				DeleteFile(finder.GetFilePath());
		}
    }
    finder.Close();
	if (bRecurse)
		RemoveDirectory(dirName);
}

// CVSAlgorithmTaskApp:
// �� Ŭ������ ������ ���ؼ��� VSAlgorithmTask.cpp�� �����Ͻʽÿ�.
//
class CVSAlgorithmTaskApp : public CWinApp
{
public:
	CVSAlgorithmTaskApp();
	virtual ~CVSAlgorithmTaskApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	CInspPanel					InspPanel;
	
	BOOL						m_bExecIPC;				// IPC Trhead ���� Flag
	BOOL						m_bExecDisk;			// Check Disk Trhead ���� Flag
	// �������� ������ �ʿ䰡 �ִ� ���� �� �Լ��� ���⿡ ���� �� theApp. ���
	// Variable

	CRITICAL_SECTION			m_SVICoordFile;				//	SVI ���� ũ��Ƽ�� ����
	
	CConfig						m_Config;
	TaskInterfacer*				m_AlgorithmTask;
	int							m_nDefectCnt;

	STRU_INFO_GRAB_STEP*		m_pGrab_Step;				// Grab_step = �̹��� ����				

	STRU_INFO_PAD**				m_pPad_Area;		

	CRITICAL_SECTION			m_csCntFileSafe;

	CRITICAL_SECTION			m_csJudgeRepeatCount;

 	// Function
	// Visual Station Thread
	static UINT					ThreadConnectVSServer(LPVOID pParam);

	void						StartInitialize();			// �ʱ�ȭ ����, (ó������ ����)
	bool						InitializeVision();
	void						ExitVision() {InspPanel.ExitVision(); DeletePadAreaInfo();}; 

	// �α� ���� Wrapper �Լ�
	// �α� ����, GUI �α� �ۺ� ����, ��ü UI �α� ǥ�� ����, �α�
	void						WriteLog(const ENUM_KIND_OF_LOG eLogID, const ENUM_LOG_LEVEL eLogLevel, const BOOL bSendGUITask, const BOOL bTraceList, TCHAR* pszLog, ...);

	// �˰��� �Ķ���� �ε� - XML
	bool						ReadAlgorithmParameter(TCHAR* strModelPath, TCHAR* strCornerEdgePath=NULL);
	// PAD �˻� �̹��� && ��ǥ ������ �Ķ���� �ε� - XML && BMP
	bool						ReadPadInspParameter(TCHAR* strModelPath);
	// ���� �Ķ���� �ε�
	bool						ReadJudgeParameter(TCHAR* strModelPath);
	// ����� ���� ���� �ε�
	bool						ReadUserDefinedFilter(TCHAR* strModelPath);
	// ��ǥ�ҷ� ���� ���� �Ķ���� �ε�
	bool						ReadDefectClassify(TCHAR* strModelPath);
	//�ϵ� ����̺� üũ, ���� Align���� �ѹ��� ����
	void						CheckDrive();
	// Get Func
		
	BOOL						GetIPCState()													{	return m_bIPCConnect						;};
	int							GetGrabStepCount()												{	return m_nParamCount						;};
	int							GetCameraCount(int nGrabStep)									{	return m_pGrab_Step[nGrabStep].nCamCnt		;};
	int							GetROICnt(int nImageNum, int nCamIndex)							{	return m_pGrab_Step[nImageNum].stInfoCam[nCamIndex].nROICnt				;};
	TCHAR*						GetROIName(int nGrabStep, int nCamIndex, int nROIIndex)			{	return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].ROI[nROIIndex].strROIName	;};

	BOOL						GetUseGrabStep(int nGrabStep)									{	return m_pGrab_Step[nGrabStep].bUse												;};	

	CRect						GetFilterROI(int nGrabStep, int nCamIndex, int nROIIndex, int nRatio)		{	CRect rcRet(m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].NonROI[nROIIndex].rectROI.left * nRatio,	\
																															m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].NonROI[nROIIndex].rectROI.top * nRatio,	\
																															m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].NonROI[nROIIndex].rectROI.right * nRatio,	\
																															m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].NonROI[nROIIndex].rectROI.bottom * nRatio);\
																												return rcRet																						;};
	BOOL						GetUseFilterROI(int nGrabStep, int nCamIndex, int nROIIndex)	{	return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].NonROI[nROIIndex].bUseROI	;};
	int							GetFilterROICnt(int nGrabStep, int nCamIndex)					{	return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].nNonROICnt			;};	
	TCHAR*						GetFilterROIName(int nGrabStep, int nCamIndex, int nROIIndex)	{	return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].NonROI[nROIIndex].strROIName;};

	CRect						GetRndROI(int nGrabStep, int nCamIndex, int nROIIndex, int nRatio)			{	CRect rcRet(m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].RndROI[nROIIndex].rectROI.left * nRatio,	\
																															m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].RndROI[nROIIndex].rectROI.top * nRatio,	\
																															m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].RndROI[nROIIndex].rectROI.right * nRatio,	\
																															m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].RndROI[nROIIndex].rectROI.bottom * nRatio);\
																															return rcRet																						;};
	int							GetRndROICnt(int nGrabStep, int nCamIndex)						{	return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].nRndROICnt			;};

	// KSW 17.10.24 [Round] - ���� ����
	//int						GetRoundROICnt(int nGrabStep, int nCamIndex)					{	return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].nRndROICnt			;};	
	INSP_AREA*					GetRoundROI(int nGrabStep, int nCamIndex)						{	return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].RndROI				;};

	// 19.03.05 [ Camera Hole ] - ���� ����
	int							GetCHoleROICnt(int nGrabStep, int nCamIndex) { return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].nHoleROICnt; };

	INSP_AREA*					GetCHoleROI(int nGrabStep, int nCamIndex) { return m_pGrab_Step[nGrabStep].stInfoCam[nCamIndex].HoleROI; };

	BOOL						GetUseAlgorithm(int nImageNum, int nCameraNum, int nROINum, int nAlgorithmNum)
																								{	return m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].ROI[nROINum].AlgorithmList[nAlgorithmNum].bAlgorithmUse;	};
	BOOL						GetUseDefectFilterParam(int nImageNum, int nCameraNum, int nROINum, int nAlgorithmNum, int nDefectItemNum)
																								{	return m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].ROI[nROINum].AlgorithmList[nAlgorithmNum].stDefectItem[nDefectItemNum].bDefectItemUse;	};
	double*						GetAlgorithmParameter(int nImageNum, int nCameraNum, int nROINum, int nAlgorithmNum)					// Teaching���� ������ ROI�� Defect List üũ Ȯ��.
																								{	return m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].ROI[nROINum].AlgorithmList[nAlgorithmNum].dPara;	};
// 	TCHAR*						GetAlgorithmName(int nImageNum, int nCameraNum, int nROINum, int nAlgorithmNum)
// 																								{	return m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].ROI[nROINum].AlgorithmList[nAlgorithmNum].strAlgName;	};
	STRU_DEFECT_ITEM*			GetDefectFilteringParam(int nImageNum, int nCameraNum, int nROINum, int nAlgorithmNum)
																								{	return m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].ROI[nROINum].AlgorithmList[nAlgorithmNum].stDefectItem;	};
	int							GetInsTypeIndex(int nImageNum, int nCameraNum, int nROINum, CString strAlgName);
	int							GetDefectItemCount()											{	return MAX_MEM_SIZE_E_DEFECT_NAME_COUNT			;};
	int							GetDefectFilterCount()											{	return m_nDefectFilterParamCount	;};
	int							GetImageClassify(int nImageNum)									{	return m_pGrab_Step[nImageNum].eImgClassify		;};		// ENUM_IMAGE_CLASSIFY -> int
	int							GetImageNum(int eImgClassify)					{	for (int nImageNum=0; nImageNum<MAX_GRAB_STEP_COUNT; nImageNum++)	\
																									{																	\
																										if (m_pGrab_Step[nImageNum].eImgClassify == eImgClassify)		\
																											return nImageNum;											\
																									}
																									return -1
																								;};
	//2018.04.13 add by ikm for pad area
	STRU_INFO_PAD**				GetPadAreaInfo()												{   return m_pPad_Area							;};
	TCHAR*						GetGrabStepName(int nGrabStepNo)								{	return m_pGrab_Step[nGrabStepNo].strGrabStepName	;};
	// AD Defect & Align Parameter �и��ؾ� ��. ���� Common Parameter ����ϰ� ����.
	BOOL						GetUseFindDefectAD(int nImageNum, int nCameraNum)				{	return m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].bUseAD;				};
	double*						GetFindDefectADParameter(int nImageNum, int nCameraNum)			{	return m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].dADPara;				};
	double*						GetAlignParameter(int nCameraNum)								{	return m_dAlignParam;	};
	ST_COMMON_PARA*				GetCommonParameter()											{	return &m_stCommonPara;	};
	ST_MODEL_INFO*				GetModelInfo()										         	{   return &m_stModelInfo;    };

	int							GetDefectTypeIndex(const CString strDefTypeName)				{	map<CString, UINT>::const_iterator iter = m_MapDefItemList.find(strDefTypeName);	\
																									if ( iter != m_MapDefItemList.end())	return iter->second;	\
																									else	return -1;	\
																								};
	CString						GetDefectTypeName(const UINT nDefTypeIndex)						{	map<CString, UINT>::iterator iter;	\
																									for (iter = m_MapDefItemList.begin(); iter != m_MapDefItemList.end(); iter++)	\
																										if (iter->second == nDefTypeIndex)	return iter->first;
																									return _T("NOT_FOUND");	\
																								};
	TCHAR*						GetDefectSysName(const UINT nDefTypeIndex)						{	return m_stDefClassify[nDefTypeIndex].strDefectName		;};
	TCHAR*						GetDefectCode(const UINT nDefTypeIndex)							{	return m_stDefClassify[nDefTypeIndex].strDefectCode		;};
	int							GetAlgorithmIndex(const CString strAlgorithmName)				{	map<CString, UINT>::const_iterator iter = m_MapAlgList.find(strAlgorithmName);	\
																									if ( iter != m_MapAlgList.end())	return iter->second;	\
																									else								return -1;	\
																								};
	CString						GetAlgorithmName(const UINT nAlgorithmIndex)					{	map<CString, UINT>::iterator iter;	\
																									for (iter = m_MapAlgList.begin(); iter != m_MapAlgList.end(); iter++)	\
																										if (iter->second == nAlgorithmIndex)	return iter->first;
																									return _T("NOT_FOUND");	\
																								};

	TCHAR*						GetCurModelID()													{	return m_strCurModelID		;};

	// KSW 17.11.03 - [Round] ��� ���� ��� ��������
	TCHAR*						GetRoundPath()													{	return m_strModelPath		;};
	
	TCHAR*						GetCurInspRecipePath()											{	return m_strCurInspRecipePath;	};

	CWriteResultInfo*			GetCurWorkCoordInfo()											{	return &m_WrtResultInfo		;};

	CString						GetCurStepFileName(TCHAR* strFindeFolder, TCHAR* strFindFileName);

	std::vector<stPanelJudgePriority> GetPanelJudgeInfo()										{	return m_vStPanelJudge		;};

	std::vector<stUserDefinedFilter> GetUserDefinedFilter()										{	return m_vStUserDefinedFilter		;};

	int							GetPanelJudgeIndex(CString strJudge)							{	vector<stPanelJudgePriority>::iterator iter;	int nIndex = 0;\
																									for (iter = m_vStPanelJudge.begin(); iter != m_vStPanelJudge.end(); iter++,nIndex++)	\
																										if (iter->strGrade == strJudge)	return nIndex;
																											return -1;	\
																								};
	std::list<RepeatDefectInfo>*	GetRepeatDefectInfo()										{	return m_listRepeatDefInfo			;};		// ���� ���� �ҷ� List �ҷ�����




	stPanelJudgeInfo*			GetReportFilter(int nJudge)												{	return m_vStPanelJudge[nJudge].stFilterInfo	;};

	int							CheckImageRatio(UINT nCurRatio, int nDstWidth, int nDstHeight, int nSrcWidth, int nSrcHeight);
	int							GetDefectRank(int nDefectTypeNum)									{	return m_nDefectRank[nDefectTypeNum]	;};
	int							GetDefectGroup(int nDefectTypeNum)								{	return m_nDefectGroup[nDefectTypeNum]	;};

	// Set Func
	void						SetIPCState(BOOL bState)										{	m_bIPCConnect = bState		;};
	void						SetGrabStepCount(int nParamCount)								{	m_nParamCount = nParamCount	;};

	//2018.04.13 add by ikm for pad area
	void						SetPadAreaInfo(STRU_INFO_PAD** pPad_Area)						{	m_pPad_Area = pPad_Area/*memcpy(m_pPad_Area, &pPad_Area, sizeof(STRU_INFO_PAD) * 5)*/	;};

	// UI - System Interface �Լ� �߰�
	double						CallFocusValue(Mat matSrc, CRect rect);

	void						SetCurModelID(TCHAR* strModelID)								{	_tcscpy_s(m_strCurModelID, strModelID)		;};

	void						SetCurInspRecipePath(TCHAR* strPath)							{	_tcscpy_s(m_strCurInspRecipePath, strPath)	;};	

	// KSW 17.11.03 - [Round] ��� ���� ��� ����
	void						SetRoundPath(TCHAR* strPath)									{	_tcscpy_s(m_strModelPath, strPath)			;};

	void						SetWorkCoordInfo(double dPanelSizeX, double dPanelSizeY, int nCurWorkDirection, int nCurWorkOrgPos, 
													int nCurWorkOffsetX, int nCurWorkOffsetY, 
													int nCurDataDrection, int nCurGDOriginPos, 
													int nCurGDOffsetX, int nCurGDOffsetY, 
													double dCurGatePitch, double dCurDataPitch, 
													double* dCurResolution)						{	m_WrtResultInfo.SetWorkCoordInfo(
																								dPanelSizeX, dPanelSizeY,
																								nCurWorkDirection, nCurWorkOrgPos, nCurWorkOffsetX, nCurWorkOffsetY, 
																								nCurDataDrection, nCurGDOriginPos, 
																								nCurGDOffsetX, nCurGDOffsetY, 
																								dCurGatePitch, dCurDataPitch,
																								dCurResolution)			;};
	void						SetAlignParameter(double* dAlignPara)							{	memcpy(m_dAlignParam, dAlignPara, sizeof(double) * MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT)		;};
	void						SetCommonParameter(ST_COMMON_PARA* pStCommonPara)				{	memcpy(&m_stCommonPara, pStCommonPara, sizeof(ST_COMMON_PARA))	;};
	void						SetCommonParameter(BOOL bSaveFlag)                              {   m_stCommonPara.bIFImageSaveFlag = bSaveFlag; };	// GVO ��û���� Auto�� ��� �߰� �̹��� �ȳ����� ���� - 190425 YWS

	void						SetAlgoritmList(map<CString, UINT> AlgList)						{	m_MapAlgList = AlgList					;};
	void						SetDefectItemList(map<CString, UINT> DefItemList)				{	m_MapDefItemList = DefItemList			;};
	void						SetGrabStepInfo(STRU_INFO_GRAB_STEP* pGrabStepInfo)				{	memcpy(m_pGrab_Step, pGrabStepInfo, sizeof(STRU_INFO_GRAB_STEP) * MAX_GRAB_STEP_COUNT)			;};

	void						SetPanelJudgeInfo(std::vector<stPanelJudgePriority> vPanelJudge){	m_vStPanelJudge = vPanelJudge			;};
	void						SetUserDefinedFilter(std::vector<stUserDefinedFilter> vUserFilter){	m_vStUserDefinedFilter = vUserFilter			;};
	void						SetReportFilter(stReportFilter* pStRF)							{	memcpy(&m_StReportFilter, pStRF, sizeof(stReportFilter))			;};

	void						SetDefectClassify(stDefClassification* stDefectClassify)		{	memcpy(m_stDefClassify, stDefectClassify, sizeof(stDefClassification) * MAX_MEM_SIZE_E_DEFECT_NAME_COUNT)	;};
	stDefClassification*		GetDefectClassify()												{	return m_stDefClassify;};
	void						SetDefectRank(int* DefectRank)									{	memcpy(m_nDefectRank, DefectRank, sizeof(m_nDefectRank))			 ;};
	void						SetDefectGroup(int* DefectGroup)								{	memcpy(m_nDefectGroup, DefectGroup, sizeof(m_nDefectGroup))			 ;};
	bool WriteResultFile(CString strPanelID, CString strFilePath, CString strFileName, CString strColumn, TCHAR* strResult);
	
	void						DeletePadAreaInfo()												{
																									if (m_pPad_Area != NULL)
																									{
																										for (int nPadInfoCnt = 0 ; nPadInfoCnt < E_PAD_AREA_COUNT; nPadInfoCnt++)	delete[] m_pPad_Area[nPadInfoCnt]; 
																										delete[] m_pPad_Area; m_pPad_Area = NULL; m_pPad_Area = (NULL);
																									}
																								};
	// YHS 18.03.12 - Merge Tool get �Լ� �߰�
	BOOL GetMergeToolUse();
	
	// 2018.10.01 MDJ File Read
	void						ReadRepeatDefectInfo();
	DECLARE_MESSAGE_MAP()

private:
	// Variable
	CVSAlgorithmTaskDlg*		m_pDlg;
	CWinThread*					m_pVSThread;
	CWinThread*					m_pDiskCheckThread;
	BOOL						m_bIPCConnect;
	HANDLE						m_hEventIPCThreadAlive;
	HANDLE						m_hEventDiskThreadAlive;

	double						m_dAlignParam[MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT];			// Align �Ķ���� �ִ� 15��

	ST_COMMON_PARA				m_stCommonPara;
	ST_MODEL_INFO				m_stModelInfo;

	int							m_nInspCamID;

	TCHAR						m_strCurModelID[50];

	// KSW 17.11.03 - [Round] ��� ���� ���
	TCHAR						m_strModelPath[200];
	
	TCHAR						m_strCurInspRecipePath[256];


	// Function
	int							m_fnConectVisualStation();
	BOOL						m_fnInitFunc();

	int							m_nParamCount;				// �̹��� ����
	int							m_nDefectItemCount;
	int							m_nDefectFilterParamCount;
	bool						m_bStopInspect;			// �˻� ���� Flag

	// Log Class
	CAlgLogWriter*				m_pLogWriterCam[MAX_CAMERA_COUNT];		// ī�޶� �α�
	CAlgLogWriter*				m_pLogWriterProc;						// ���� �α� (���� ������ �� ���� / ��� ���� ������)
	CAlgLogWriter*				m_pLogWriterTact;						// Tact �α�
	CAlgLogWriter*				m_pLogWriterComm;						// ��� �α�

	map<CString, UINT>			m_MapDefItemList;						// ��ü Defect Item List
	map<CString, UINT>			m_MapAlgList;							// ��ü Algorithm List

	CWriteResultInfo			m_WrtResultInfo;						// ��ũ��ǥ ��� / ��������� ���� Ŭ����

	std::vector<stPanelJudgePriority>	m_vStPanelJudge;				// �г� ���� �켱����
	std::vector<stUserDefinedFilter>	m_vStUserDefinedFilter;				// ����� ���� ����
	stReportFilter						m_StReportFilter;				// ���� ���� ���͸�
	int									m_nDefectRank[E_DEFECT_JUDGEMENT_COUNT];	/// ��ǥ �ҷ� ���� ����, ���ڰ� ���� ���� �� ���� ������ ����.
	int									m_nDefectGroup[E_DEFECT_JUDGEMENT_COUNT];	/// ��ǥ�� �� ��ǥ�ҷ� ���� ���� - ��ǥ �ҷ��� Index (0 : ��ǥ�ҷ� ��� ��� ����, N : ���� ��ȣ���� �ҷ������� �����Ͽ� ��ǥ�ҷ� ����)

	void						m_fnGetJudgeInfo(stPanelJudgeInfo* pStJudgeInfo, CString strVal, int nCount);

	stDefClassification			m_stDefClassify[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];

	CDiskInfo* m_pDiskInfo;
	std::vector<tDriveInfoParam>	m_tDriveInfoParam;
	static UINT					ThreadDiskCheck(LPVOID pParam);

	std::list<RepeatDefectInfo>		m_listRepeatDefInfo[eCOORD_KIND];				// ���� ��ġ ���� �ҷ� Count
	bool							m_fnReadRepeatDefectInfo();						// ���Ͽ��� ���� Data �о���� - ���α׷� �⵿ �� �ѹ���	
	BOOL							m_fnReadRepeatFile(CString strFilePath, std::list<RepeatDefectInfo>* pList);	// Pixel, Work ��ǥ ���� �о����

	// YHS 18.03.12 - Merge Tool ���� �߰�
	BOOL m_nInspStep;
};

extern CVSAlgorithmTaskApp theApp;
#if !defined(AFX_INSPTHRD_H__1C81645D_D41C_4CB4_AD97_DDAAEAB90337__INCLUDED_)
#define AFX_INSPTHRD_H__1C81645D_D41C_4CB4_AD97_DDAAEAB90337__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InspThrd.h : header file
//

#include "InspResultInfo.h"

// CHole Align ����
struct tCHoleAlignInfo
{
	bool            bLineDirection;
	bool			bCHoleAD[E_IMAGE_CLASSIFY_AVI_COUNT][MAX_MEM_SIZE_E_INSPECT_AREA];					// CHole ���� AD Ȯ�ο�
	cv::Mat			matCHoleROIBuf[E_IMAGE_CLASSIFY_AVI_COUNT][MAX_MEM_SIZE_E_INSPECT_AREA];			// White Pattern ���� ��ǥ����(AD �ƴ� �� ����)
	cv::Rect		rcCHoleROI[E_IMAGE_CLASSIFY_AVI_COUNT][MAX_MEM_SIZE_E_INSPECT_AREA];				// CHole ROI (AD �϶� AD ���� / AD �ƴҶ� CHole��ǥ����)
};

// Panel Align ����
struct tAlignInfo
{
	cv::Point		ptCorner[4];				// (����)���� Cell �� �ܰ� ������
	int				nRatio;						// (����) Align �� ���� P/S ��� ���� // KSW 17.08.29
	cv::Rect		rcAlignCellROI;				// (����)Edge ROI
	cv::Rect		rcAlignPadROI;				// (����)Pad ROI
	cv::Rect		rcAlignActiveROI;			// (����)Active ROI
	double			dAlignTheta;				// ȸ�� ����
	cv::Point		ptAlignCenter;				// ȸ�� ��, �߽�
	cv::Point		ptCellCenter;				// ���󿡼� Cell �߽�
	cv::Point		ptStandard;					// Left-Top ������
	bool			bAlignSuccess;				// AD - Align ���� ����
	int				nStageNo;					// �������� ��ȣ
	vector<vector<vector<Point2i>>> ptRoundContour;
	vector<vector<Point2i>> ptActiveContour;
	cv::Rect		rcActive;					// APP Active ROI ���� ��� Align Active ROI�� ���� ������ ���� ��� ���� ��(UI���� �����ϴ� ����). 
	bool			bLineDirection;
	cv::Mat matMask_Active;
	cv::Mat matMask_Label;
	cv::Mat matMask_Hole;
	cv::Mat matMask_BackStage;
	cv::Mat matMask_VacuumHole;

	cv::Mat matSrc_Stage;
	cv::Mat matSrc_DrawMain;
	// CHole Align ����
	tCHoleAlignInfo* tCHoleAlignData;

	tAlignInfo()
	{
		// Init Align Parameter
		for (int i=0; i<4; i++)
		{
			ptCorner[i]	= cv::Point(0, 0);
		}
		rcAlignCellROI	= cv::Rect(0, 0, 0, 0);
		rcAlignPadROI	= cv::Rect(0, 0, 0, 0);
		rcAlignActiveROI= cv::Rect(0, 0, 0, 0);
		dAlignTheta		= 0.0;
		ptAlignCenter	= cv::Point(0, 0);
		ptCellCenter	= cv::Point(0, 0);
		ptStandard		= cv::Point(0, 0);
		bAlignSuccess	= false;
		nRatio			= 0;
		nStageNo		= 0;
		vector<vector<vector<Point2i>>>().swap(ptRoundContour);
		vector<vector<Point2i>>().swap(ptActiveContour);
		rcActive		= cv::Rect(0,0,0,0);

		tCHoleAlignData = NULL;
		bLineDirection = true;
	}

	void SetAdjustAlignInfoRatio(tAlignInfo* pStAlignInfo, double dRatio, bool bSuccess = true)
	{
		ptAlignCenter.x	= (int)(pStAlignInfo->ptAlignCenter.x	* dRatio);
		ptAlignCenter.y = (int)(pStAlignInfo->ptAlignCenter.y	* dRatio);
		ptCellCenter.x	= (int)(pStAlignInfo->ptCellCenter.x	* dRatio);
		ptCellCenter.y	= (int)(pStAlignInfo->ptCellCenter.y	* dRatio);
		ptStandard.x	= (int)(pStAlignInfo->ptStandard.x		* dRatio);
		ptStandard.y	= (int)(pStAlignInfo->ptStandard.y		* dRatio);

		nRatio			= (int)dRatio;

		dAlignTheta		= pStAlignInfo->dAlignTheta;

		for (int nPoint = E_CORNER_LEFT_TOP; nPoint <= E_CORNER_LEFT_BOTTOM; nPoint++)
		{
			ptCorner[nPoint].x = (int)(pStAlignInfo->ptCorner[nPoint].x * dRatio);
			ptCorner[nPoint].y = (int)(pStAlignInfo->ptCorner[nPoint].y * dRatio);
		}

		rcAlignCellROI.x		= (int)(pStAlignInfo->rcAlignCellROI.x		* dRatio);
		rcAlignCellROI.y		= (int)(pStAlignInfo->rcAlignCellROI.y		* dRatio);
		rcAlignCellROI.width	= (int)(pStAlignInfo->rcAlignCellROI.width	* dRatio);
		rcAlignCellROI.height	= (int)(pStAlignInfo->rcAlignCellROI.height	* dRatio);

		rcAlignActiveROI.x		= (int)(pStAlignInfo->rcAlignActiveROI.x		* dRatio);
		rcAlignActiveROI.y		= (int)(pStAlignInfo->rcAlignActiveROI.y		* dRatio);
		rcAlignActiveROI.width	= (int)(pStAlignInfo->rcAlignActiveROI.width	* dRatio);
		rcAlignActiveROI.height	= (int)(pStAlignInfo->rcAlignActiveROI.height	* dRatio);

		rcAlignPadROI.x			= (int)(pStAlignInfo->rcAlignPadROI.x		* dRatio);
		rcAlignPadROI.y			= (int)(pStAlignInfo->rcAlignPadROI.y		* dRatio);
		rcAlignPadROI.width		= (int)(pStAlignInfo->rcAlignPadROI.width	* dRatio);
		rcAlignPadROI.height	= (int)(pStAlignInfo->rcAlignPadROI.height	* dRatio);

		bAlignSuccess			= bSuccess;
		ptActiveContour			= pStAlignInfo->ptActiveContour;
		rcActive				= pStAlignInfo->rcActive;

		pStAlignInfo->matMask_Active.copyTo(matMask_Active);
		pStAlignInfo->matMask_Label.copyTo(matMask_Label);
		pStAlignInfo->matMask_Hole.copyTo(matMask_Hole);
		pStAlignInfo->matMask_BackStage.copyTo(matMask_BackStage);
		pStAlignInfo->matMask_VacuumHole.copyTo(matMask_VacuumHole);
		pStAlignInfo->matSrc_Stage.copyTo(matSrc_Stage);
		pStAlignInfo->matSrc_DrawMain.copyTo(matSrc_DrawMain);

	}

	void SetAdjustAlignInfo(cv::Point* ptAdjCorner, double* dAlignPara, int nMaxWidth, int nMaxHeight)
	{
		ptStandard.x			= ptAdjCorner[E_CORNER_LEFT_TOP].x;
		ptStandard.y			= ptAdjCorner[E_CORNER_LEFT_TOP].y;

		// 2017.06.06 NDH : APP ROI Setting
		cv::Rect rtCheck(0, 0, nMaxWidth , nMaxHeight);		// �̹��� ���� 
		cv::Rect rtEdge, rtActive, rtPAD;

		rtEdge.x		= ptAdjCorner[E_CORNER_LEFT_TOP].x;
		rtEdge.y		= ptAdjCorner[E_CORNER_LEFT_TOP].y;

		rtEdge.width	= ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x - ptAdjCorner[E_CORNER_LEFT_TOP].x;
		rtEdge.height	= ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y - ptAdjCorner[E_CORNER_LEFT_TOP].y;

		GetCheckROIOver(rtEdge, rtCheck, rcAlignCellROI);

		// active Setting
		int nActiveLeft		=	(int)dAlignPara[E_PARA_ALIGN_AREA_ACTIVE_L];
		int nActiveRight	=	(int)dAlignPara[E_PARA_ALIGN_AREA_ACTIVE_R];
		int nActiveTop		=	(int)dAlignPara[E_PARA_ALIGN_AREA_ACTIVE_T];
		int nActiveBottom	=	(int)dAlignPara[E_PARA_ALIGN_AREA_ACTIVE_B];

		rtActive.x			=	(ptAdjCorner[E_CORNER_LEFT_TOP].x		+ ptAdjCorner[E_CORNER_LEFT_BOTTOM].x		)	/ 2		+	nActiveLeft	;
		rtActive.width		=	(((ptAdjCorner[E_CORNER_RIGHT_TOP].x	+ ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x		)	/ 2		-	nActiveRight)	- 	rtActive.x);

		rtActive.y			=	(ptAdjCorner[E_CORNER_LEFT_TOP].y		+ ptAdjCorner[E_CORNER_RIGHT_TOP].y			)	/ 2		+	nActiveTop	;
		rtActive.height		=	(((ptAdjCorner[E_CORNER_LEFT_BOTTOM].y	+ ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y		)	/ 2		-	nActiveBottom)	-	rtActive.y);

		GetCheckROIOver(rtActive, rtCheck, rcAlignActiveROI);

		// Pad Setting
		int nPadWidth		= (int)dAlignPara[E_PARA_ALIGN_AREA_PAD_WIDTH];
		int nPadLocation	= (int)dAlignPara[E_PARA_ALIGN_AREA_PAD_LOCATION];

		switch(nPadLocation)
		{
		case E_PAD_LEFT :
			{
				rtPAD.x			=	(ptAdjCorner[E_CORNER_LEFT_TOP].x		+ ptAdjCorner[E_CORNER_LEFT_BOTTOM].x	)	/ 2		;
				rtPAD.width		=	nPadWidth;
				rtPAD.y			=	ptAdjCorner[E_CORNER_LEFT_TOP].y	;
				rtPAD.height	=	ptAdjCorner[E_CORNER_LEFT_BOTTOM].y	- rtPAD.y	;
			}
			break;

		case E_PAD_TOP :
			{
				rtPAD.x			=	ptAdjCorner[E_CORNER_LEFT_TOP].x	;
				rtPAD.width		=	ptAdjCorner[E_CORNER_RIGHT_TOP].x		- rtPAD.x	;
				rtPAD.y			=	(ptAdjCorner[E_CORNER_LEFT_TOP].y		+ ptAdjCorner[E_CORNER_RIGHT_TOP].y)		/ 2		;
				rtPAD.height	=	nPadWidth;
			}
			break;

		case E_PAD_RIGHT :
			{
				rtPAD.x			=	(ptAdjCorner[E_CORNER_RIGHT_TOP].x		+ ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x	)	/ 2	 - nPadWidth 	;
				rtPAD.width		=	nPadWidth;
				rtPAD.y			=	ptAdjCorner[E_CORNER_RIGHT_TOP].y	;
				rtPAD.height	=	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y	- rtPAD.y	;
			}
			break;

		case E_PAD_BOTTOM :
			{
				rtPAD.x			=	ptAdjCorner[E_CORNER_LEFT_BOTTOM].x	;
				rtPAD.width		=	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x	- rtPAD.x	;
				rtPAD.y			=	(ptAdjCorner[E_CORNER_LEFT_BOTTOM].y	+ ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y	)	/ 2	 - nPadWidth 	;
				rtPAD.height	=	nPadWidth;
			}
			break;
		}

		GetCheckROIOver(rtPAD, rtCheck, rcAlignPadROI);
	}

	void SetRoundContour(vector<vector<Point2i>> ptContour)
	{
		ptRoundContour.push_back(vector<vector<Point2i>>(ptContour));
	}

	void SetRoundContour(tAlignInfo* pStAlignInfo)
	{
		ptRoundContour = pStAlignInfo->ptRoundContour;
	}
};

struct tInspectThreadParam
{
	/// ����
	cv::Mat				MatOrgImage[MAX_GRAB_STEP_COUNT][MAX_CAMERA_COUNT];		// ���� �̹��� �����
	cv::Mat				MatColorImage[MAX_GRAB_STEP_COUNT][MAX_CAMERA_COUNT];			// ���� ��� �̹���
	cv::Mat				MatDraw[MAX_GRAB_STEP_COUNT][MAX_CAMERA_COUNT];			// ���� ��� �̹���
	cv::Mat				MatResult[MAX_GRAB_STEP_COUNT][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE];		// [2:Dark/Bright]	

	bool				bInspectEnd[MAX_GRAB_STEP_COUNT][MAX_CAMERA_COUNT];		// �˻� ���� �÷���
	bool				bStAlignEnd[MAX_CAMERA_COUNT];							// �⺻ ����� ���� �÷���
	bool				bStBackAlignEnd[MAX_CAMERA_COUNT];							// BackStage Get Mask ���� �÷���
	bool				bStVacuumAlignEnd[MAX_CAMERA_COUNT];						// ���� �÷� Vacuum Get Mask ���� �÷���
	bool				bStLabelMaskMakeEnd[MAX_CAMERA_COUNT];						// ���� �÷� �� Mask Make �÷���
	bool				bStStageDustEnd[MAX_CAMERA_COUNT];						// Stage ���� �÷���
	long                nError_Thread[MAX_CAMERA_COUNT];						// Thread �� Error ����
	int                 nError_Image_Num[MAX_CAMERA_COUNT];						// Thread �� Error �̹��� ����
	bool                bAlign_Success[MAX_CAMERA_COUNT];						// Thread �� Align ��� ����

	bool				bAlignEnd[MAX_CAMERA_COUNT];							// ����� ���� �÷���
	bool				bInspectAlgEnd[MAX_GRAB_STEP_COUNT][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER];							// ����� ���� �÷���
	ResultPanelData		ResultPanelData;										// �ǳ� ����Ÿ
	tAlignInfo			stCamAlignInfo[MAX_CAMERA_COUNT];						// ī�޶� ����� ����
	CWriteResultInfo	WrtResultInfo;											// ��ũ��ǥ ��� / ��������� ���� Ŭ����
	bool				bUseInspect;											// �˻� ���� ���� (Grab Only ���)
	ENUM_INSPECT_MODE	eInspMode;
	bool				bHeavyAlarm;											// �߾˶� �߻� ����. �߾˶��� �߻��ϸ� ��� �˻� Skip
	/// AVI ����
	bool				bChkDustEnd;											// Dust ���� ���� üũ ���� �÷���
	bool				bIsNormalDust;											// Dust ���� ���� (true : ���� ����)
	bool				bUseDustRetry;											// Dust Retry ��� ����
	int					nDustRetryCnt;											// Dust Retry Ƚ��
	TCHAR				strSaveDrive[10];										// image, result ���� ����̺�
	CString				strImagePath[MAX_GRAB_STEP_COUNT][MAX_CAMERA_COUNT];	// ����������, 2020.07.22 yskim, ���� ���� �񵿱� ó���� ���� ����

	CString				strImagePath_LCP_Bubble;								//LCP �߰��˻�� 

	tCHoleAlignInfo		tCHoleAlignData;										// CHole Align����

	bool				bParamUse;												// �Ķ���� ��� ����

	tInspectThreadParam()
	{
		clear();	// yskim 20.07.03
	}

	void clear()	// yskim 20.07.03
	{

		////////////////////////////////////////////////////////////////////////// 21.05.27 choi
// 		for (int i = 0; i < MAX_GRAB_STEP_COUNT; i++)
// 		{
// 			for (int j = 0; j < MAX_CAMERA_COUNT; j++)
// 			{
// 				MatOrgImage[i][j].release();
// 				MatDraw[i][j].release();
// 
// 				for (int u = 0; u < MAX_MEM_SIZE_E_ALGORITHM_NUMBER; u++)
// 				{
// 					for (int k = 0; k < MAX_MEM_SIZE_E_MAX_INSP_TYPE; k++)
// 					{
// 						MatResult[i][j][u][k].release();
// 					}
// 				}
// 			}
// 		}

		//////////////////////////////////////////////////////////////////////////
		// �Ķ���� ��� �÷��׸� ����������..
		bParamUse = false;

		// bInspectEnd �ʱ�ȭ
		memset(bInspectEnd, true, sizeof(bInspectEnd));

		// bAlignEnd �ʱ�ȭ
		memset(bAlignEnd, true, sizeof(bAlignEnd));
		memset(bStAlignEnd, true, sizeof(bStAlignEnd));
		memset(bStBackAlignEnd, true, sizeof(bStBackAlignEnd));
		memset(bStVacuumAlignEnd, true, sizeof(bStVacuumAlignEnd));
		memset(bStLabelMaskMakeEnd, true, sizeof(bStLabelMaskMakeEnd));
		memset(bStStageDustEnd, true, sizeof(bStStageDustEnd));
		memset(nError_Thread, true, sizeof(nError_Thread));
		memset(nError_Image_Num, true, sizeof(nError_Image_Num));
		memset(bAlign_Success, true, sizeof(bAlign_Success));
		memset(bInspectAlgEnd, true, sizeof(bInspectAlgEnd));

		bChkDustEnd = true;
		bIsNormalDust = false;
		bUseInspect = true;
		bHeavyAlarm = false;
		eInspMode = eAutoRun;
		bUseDustRetry = false;
		nDustRetryCnt = 0;
		memset(strSaveDrive, 0, sizeof(strSaveDrive));

//		memset(fMain_Stan, 0.0f, sizeof(fMain_Stan));

		// ResultPanelData �ʱ�ȭ
		ResultPanelData.m_ResultHeader = ResultHeaderInfo();
		ResultPanelData.m_ResultPanel = ResultPanelInfo();
		memset(&ResultPanelData.CornerPt, 0, sizeof(struct CPoint));
		memset(ResultPanelData.m_nDefectTrend, 0, sizeof(ResultPanelData.m_nDefectTrend));
		ResultPanelData.m_ListDefectInfo.RemoveAll();

		// tAlignInfo �ʱ�ȭ
		for (int i = 0; i < MAX_CAMERA_COUNT; i++)
			stCamAlignInfo[i] = tAlignInfo();

		// WrtResultInfo �ʱ�ȭ
		WrtResultInfo = CWriteResultInfo();

		// tCHoleAlignData �ʱ�ȭ
		for (int i = 0; i < E_IMAGE_CLASSIFY_AVI_COUNT; i++)
		{
			for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
			{
				tCHoleAlignData.bCHoleAD[i][j] = false;
				tCHoleAlignData.rcCHoleROI[i][j] = cv::Rect(0, 0, 0, 0);
			}
		}
	}
};


/////////////////////////////////////////////////////////////////////////////
// CInspThrd thread
class CInspThrd : public CWinThread
{
	DECLARE_DYNCREATE(CInspThrd)

protected:
	CInspThrd();           // protected constructor used by dynamic creation
	
	bool		m_bBusy;				// �����尡 �˻� ������ �������ΰ� Ȯ��
	tAlignInfo	m_stThrdAlignInfo;
	
// Attributes
public:
	// thread info
	int m_nThrdID;

// Operations
public:
	////////////////
	virtual void Initialize(int nThreadCount);

	////////////////

	bool IsThrdBusy();

	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

public:
	virtual ~CInspThrd();

	// Generated message map functions
	//{{AFX_MSG(CInspThrd)
	//}}AFX_MSG
	virtual afx_msg LRESULT OnStartInspection(WPARAM wp, LPARAM lp);	//2016.10.17
	virtual afx_msg LRESULT OnStartSaveImage(WPARAM wp, LPARAM lp);		//2020.07.22
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPTHRD_H__1C81645D_D41C_4CB4_AD97_DDAAEAB90337__INCLUDED_)

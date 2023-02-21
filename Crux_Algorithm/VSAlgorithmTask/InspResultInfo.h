#pragma once

// InspResultInfo ��ȭ �����Դϴ�.

#if !defined(AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_)
#define AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>		
#include <list>
using namespace std;	

#define DEFAULT_DATE  _T("YYYY/MM/DD")
#define DEFAULT_TIME  _T("HH:MM:SS")

enum ENUM_KIND_OF_REPEAT_COORD
{
	ePIXEL = 0	,
	eWORK		,
	eMACHINE	,					// 2018.09.21 MDJ APP Repeat Defect
	eCOORD_KIND
};

#include "DefineInterface.h"

struct ResultHeaderInfo 
{   
	//	������ 18.12.06
	//	MergeTool Falg �� ������� ������ ����
	int				MERGE_nRatio;
	cv::Rect		MERGE_rcAlignCellROI;
	float MERGE_dPanelSizeX;
	float MERGE_dPanelSizeY;
	int	MERGE_nWorkDirection;
	int	MERGE_nWorkOriginPosition;
	int	MERGE_nWorkOffsetX;
	int	MERGE_nWorkOffsetY;
	int	MERGE_nDataDirection;
	int	MERGE_nGateDataOriginPosition;
	int	MERGE_nGateDataOffsetX;
	int	MERGE_nGateDataOffsetY;
	int	MERGE_dGatePitch;
	int	MERGE_dDataPitch;
	/////////////


	/*/////////////////////////////////////////////////////////////////////////////////////////////
	ITEM										FORTMAT    ��ó			�ʼ�		COMMENT
	/////////////////////////////////////////////////////////////////////////////////////////////*/
	CString	EQP_ID;							/// 999			EQP			Y			Equipment ID
	CString Insp_start_date;				///	YYYY/MM/DD	EQP			Y			�˻� ���� ��¥
	CString Insp_start_time;				/// HH:MM:SS	EQP			Y			�˻� ���� �ð�
	CString Insp_end_date;					/// YYYY/MM/DD	EQP			Y			�˻� ���� ��¥
	CString Insp_end_time;					/// HH:MM:SS	EQP			Y			�˻� ���� �ð�


	ResultHeaderInfo()
	{
		EQP_ID		= _T("");

		SYSTEMTIME time;
		::GetLocalTime(&time);
		Insp_start_date.Format(_T("%04d/%02d/%02d"), time.wYear, time.wMonth, time.wDay);
		Insp_start_time.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);

	}

	void SetInspectStartTime()
	{
		SYSTEMTIME time;
		::GetLocalTime(&time);
		Insp_start_date.Format(_T("%04d/%02d/%02d"), time.wYear, time.wMonth, time.wDay);
		Insp_start_time.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
	}

	void SetInspectEndTime()
	{
		SYSTEMTIME time;
		::GetLocalTime(&time);
		Insp_end_date.Format(_T("%04d/%02d/%02d"), time.wYear, time.wMonth, time.wDay);
		Insp_end_time.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
	}	
};

struct ResultPanelInfo 
{   /*/////////////////////////////////////////////////////////////////////////////////////////////
	ITEM										FORTMAT    ��ó			�ʼ�		COMMENT
	/////////////////////////////////////////////////////////////////////////////////////////////*/
	CString EQP_ID;							//	Add By KYH EQP ID Write
	CString LOT_ID;							//  Add By KYH LOT ID Write
	CString Panel_ID;						///	A-18-A		EQP			Y			CIM ���� ����											
	CString Judge;							///		 A		EQP			Y	
	CString judge_code_1;					///	  AAAA		EQP			Y	
	CString judge_code_2;					///	  AAAA		EQP			Y
	CString	Recipe_ID;						/// A-16-A		EQP			Y
	int nFinalDefectNum;
	cv::Rect rcAlignCellROI;				/// Auto ROI
	ResultPanelInfo()
	{	
		EQP_ID			=  _T("EQP_ID");
		LOT_ID			= _T("LOTID");
		Panel_ID		= _T("PNID");
		Judge			= _T("JUDGE");
		judge_code_1	= _T("CODE1");
		judge_code_2	= _T("CODE2");
		Recipe_ID		= _T("RCPID");
		nFinalDefectNum = 0;
		rcAlignCellROI	= cv::Rect(0, 0, 0, 0);
	}
	void SetAlignCellROI(cv::Rect* rcInput, double dRatio)
	{
		rcAlignCellROI.x		= (int)(rcInput->x		* dRatio);
		rcAlignCellROI.y		= (int)(rcInput->y		* dRatio);
		rcAlignCellROI.width	= (int)(rcInput->width	* dRatio);
		rcAlignCellROI.height	= (int)(rcInput->height	* dRatio);
	}
};


struct ResultDefectInfo 
{   /*///////////////////////////////////////////////////////////////////////////
	ITEM										FORTMAT    ����			����
	///////////////////////////////////////////////////////////////////////////*/
	int			Defect_No;						///	999						0 ~ 999	
	TCHAR		Defect_Code[50];				///	AAAAA					K+4����ex��KSoRh��
	cv::Point	Defect_Rect_Point[4];			/// 99999,99999				���� �ҷ� Rect ������
	int			Repair_Gate;					/// 99999					repair ���� ��� Gate Line �� ���� ��ġ
	int			Repair_Data;					/// 99999					repair ���� ��� Data Line �� ���� ��ġ
	DOUBLE		Repair_Coord_X;					/// ��9999.999	um			repair ���� ��� ���� ���� ��ġ X
	DOUBLE		Repair_Coord_Y;					/// ��9999.999	um			repair ���� ��� ���� ���� ��ġ Y
	int			Pixel_Start_X;					///	99999					Image ��ǥ X
	int			Pixel_Start_Y;					/// 99999					Image ��ǥ Y
	DOUBLE		Pixel_Center_X;					/// 99999.999				Image ��ǥ X
	DOUBLE		Pixel_Center_Y;					/// 99999.999				Image ��ǥ Y
	int			Pixel_End_X;					///	99999					Image ��ǥ X
	int			Pixel_End_Y;					/// 99999					Image ��ǥ Y
	DOUBLE		Pixel_Repair_X;					/// 99999.999				Repair ��ǥ X	// �߽� ��ǥ ����� ���� �����Ƿ� DOUBLE
	DOUBLE		Pixel_Repair_Y;					/// 99999.999				Repair ��ǥ Y
	int			Pixel_Crop_Start_X;				/// 99999					Crop�� �ҷ� ���� ��ǥ X
	int			Pixel_Crop_Start_Y;				/// 99999					Crop�� �ҷ� ���� ��ǥ Y
	int			Pixel_Crop_End_X;				/// 99999					Crop�� �ҷ� ���� ��ǥ X
	int			Pixel_Crop_End_Y;				/// 99999					Crop�� �ҷ� ���� ��ǥ Y
	int			Gate_Start_No;					/// 99999					Gate Line �� ���� ���� ��ġ
	int			Data_Start_No;					/// 99999					Data Line �� ���� ���� ��ġ
	int			Gate_End_No;					/// 99999					Gate Line �� ���� �� ��ġ
	int			Data_End_No;					/// 99999					Data Line �� ���� �� ��ġ
	DOUBLE		Coord_Start_X;					/// ��9999.999	um			���� ���� ��ġ X
	DOUBLE		Coord_Start_Y;					/// ��9999.999	um			���� ���� ��ġ Y
	DOUBLE		Coord_End_X;					/// ��9999.999	um			���� �� ��ġ X
	DOUBLE		Coord_End_Y;					/// ��9999.999	um			���� �� ��ġ Y
	int			Defect_Size;					///	999		um				���԰���
	int			Img_Number;						///	999						Pattern ��ȣ
	TCHAR		Defect_Img_Name[50];			/// 9-12-9.AAA				���� �̹��� ���� �̸�
	DOUBLE		Img_Size_X;						/// 9999.99	um				���� �̹��� ũ�� X
	DOUBLE		Img_Size_Y;						/// 9999.99	um				���� �̹��� ũ�� Y
	int			Defect_Type;					/// 99						���� ����
	int			Pattern_Type;					/// 99						���� ����
	int			Camera_No;						/// 9					
	double		Defect_BKGV;					/// �ҷ��� ��� GV
	double		Defect_MeanGV;					/// �ҷ��� ��� GV
	int			Defect_MinGV;					/// �ҷ��� �ִ� GV
	int			Defect_MaxGV;					/// �ҷ��� �ִ� GV
	int			Defect_Size_Pixel;				/// �ҷ��� ũ�� pixel
	bool		Draw_Defect_Rect;				/// �̴ϸʿ� �ҷ� Rect �׸��� ���� - ���� ���Ϸ� ������ ��ǥ�� �̴ϸ� �׸��� �ҷ� ���� (ex:MURA)
	int			nRatio;							/// Pixel Shift ��� Flag - 0 : Non / 1 : 4-Shot / 2 : 9-Shot
												/// �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	bool		bUseReport;						/// ���� ���� ����

	double		dDimension;						/// KSW17.12.04 - (���� + ����) / 2 -> Spec �߰� ( ���� ��û )

	cv::String	strColor;
	TCHAR		Color_Code[50];					// 2021.12.08~ MDJ LCP ������ ���� �ֱ� ���� ���� �߰�
	////////////////////////////////////////////////////////////////////////// 0607 choi ������ġ �������� ���� Data �߰�
	double		d_L;					
	double		d_Degree;					
	double		d_Lenght;					
	double		d_MVper;					
	//////////////////////////////////////////////////////////////////////////

#if USE_ALG_CONTOURS
	int			nContoursX[MAX_CONTOURS];		/// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
	int			nContoursY[MAX_CONTOURS];		/// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
#endif

	ResultDefectInfo()
	{
		Defect_No		= 0;
		_tcscpy_s(Defect_Code, _T("CODE"));
		for (int i=0; i<4; i++)
			Defect_Rect_Point[i] = cv::Point(0, 0);
		Repair_Gate			= 0;
		Repair_Data			= 0;
		Repair_Coord_X		= 0;
		Repair_Coord_Y		= 0;
		Pixel_Start_X		= 0;
		Pixel_Start_Y		= 0;
		Pixel_Center_X		= 0;
		Pixel_Center_Y		= 0;
		Pixel_End_X			= 0;
		Pixel_End_Y			= 0;
		Pixel_Repair_X		= 0;
		Pixel_Repair_Y		= 0;
		Gate_Start_No		= 0;
		Data_Start_No		= 0;
		Gate_End_No			= 0;
		Data_End_No			= 0;
		Coord_Start_X		= 0;
		Coord_Start_Y		= 0;
		Coord_End_X			= 0;
		Coord_End_Y			= 0;
		Defect_Size			= 0;
		Img_Number			= 0;
		_tcscpy_s(Defect_Img_Name, _T("IMG_NAME"));
		Img_Size_X			= 0;
		Img_Size_Y			= 0;
		Defect_Type			= 0;
		Pattern_Type		= 0;
		Camera_No			= 0;
		Defect_BKGV			= 0.0;					
		Defect_MeanGV		= 0.0;					
		Defect_MinGV		= 0;					
		Defect_MaxGV		= 0;					
		Defect_Size_Pixel	= 0;
		Draw_Defect_Rect	= true;
		nRatio				= 0;
		bUseReport			= true;
		dDimension			= 0.0;
		strColor			= "";
		_tcscpy_s(Color_Code, _T(""));
		////////////////////////////////////////////////////////////////////////// 0607 choi ������ġ ������������ Data �߰�
		d_L					=0.0;
		d_Degree			= 0.0;
		d_Lenght			= 0.0;
		d_MVper				= 0.0;
	///////////////////////////////////////////////////////// ���� �ҽ��� �ٸ� �κ�

#if USE_ALG_CONTOURS
		memset(nContoursX, 0, sizeof(int) * MAX_CONTOURS );
		memset(nContoursY, 0, sizeof(int) * MAX_CONTOURS );
#endif
	}
};

struct STRU_DEFECT_INFO
{
	//2015.03.02 add by IKM - S
	ResultHeaderInfo Hinfo;
	ResultPanelInfo  Pinfo;
	ResultDefectInfo Dinfo;
	//2015.03.02 add by IKM - E
};

struct ResultPanelData
{
	ResultHeaderInfo							m_ResultHeader;
	ResultPanelInfo								m_ResultPanel;
	CArray<ResultDefectInfo, ResultDefectInfo&>	m_ListDefectInfo;
	CPoint										CornerPt;
	// �ҷ��� ����
	int											m_nDefectTrend[E_PANEL_DEFECT_TREND_COUNT];			// �� ���� E_PANEL_DEFECT_TREND_COUNT -> BP+DP �� ������ �ʿ��� �ҷ� ī��Ʈ

	CRITICAL_SECTION	m_cs;

	INT_PTR Add_DefectInfo(ResultDefectInfo& DefectInfo)
	{
		EnterCriticalSection(&m_cs);
		INT_PTR ps = m_ListDefectInfo.Add(DefectInfo);
		LeaveCriticalSection(&m_cs);

		return ps;
	}

	ResultPanelData()
	{
		m_ListDefectInfo.RemoveAll();
		InitializeCriticalSection(&m_cs);		

		CornerPt.x= 0;
		CornerPt.y= 0;

		memset(m_nDefectTrend, 0, sizeof(int) * E_PANEL_DEFECT_TREND_COUNT);
	}
	~ResultPanelData()
	{
		m_ListDefectInfo.RemoveAll();
	}
};

struct Coord
{   
	DOUBLE X ;
	DOUBLE Y ;
	Coord()
	{
		X = 0;
		Y = 0;		
	}
	Coord(DOUBLE dX, DOUBLE dY)
	{
		X = dX;
		Y = dY;		
	}
};


struct WorkCoordCrt
{
	double	dPanelSizeX;
	double	dPanelSizeY;

	// for Work Coordinate
	int		nWorkDirection;					// 0 : X = Width, 1 : Y = Width
	int		nWorkOriginPosition;			// 0 : LT, 1 : RT, 2 : RB, 3 : LB
	int     nWorkOffsetX;
	int     nWorkOffsetY;

	// for Gate/Data Coordinate
	int		nDataDirection;						// 0 : Data = X, 1 : Data = Y
	int     nGateDataOriginPosition;			// 0 : LT, 1 : RT, 2 : RB, 3 : LB
	int     nGateDataOffsetX;					// ���� Pixel ��ǥ Offset
	int     nGateDataOffsetY;
	double  dGatePitch;							// Gate Pitch (um)
	double  dDataPitch;							// Data Pitch (um)	

	// Common
	double	dResolution[MAX_CAMERA_COUNT];		// Camera �� Resolution

	WorkCoordCrt()
	{
		dPanelSizeX = 0.0;
		dPanelSizeY = 0.0;
		nWorkDirection = 0;
		nWorkOriginPosition = 0;
		nWorkOffsetX = 0;
		nWorkOffsetY = 0;		
		nDataDirection = 0;
		nGateDataOriginPosition = 0;
		nGateDataOffsetX = 0;
		nGateDataOffsetY = 0;
		dGatePitch = 0.0;
		dDataPitch = 0.0;
		for (int nCamIndex = 0; nCamIndex < MAX_CAMERA_COUNT; nCamIndex++)
			dResolution[nCamIndex] = 0.0;
	}	
};

struct GD_POINT /// Gate / Data�� �� ����ü
{   
	int Gate ;
	int Data ;
	//enum DataDirection{Xdirection,	Ydirertion} Direction; //  GATE/DATA �� X ������ Y ������ �����Ѵ�.

	GD_POINT()
	{
		Gate = 0;
		Data = 0;		
	}
	GD_POINT(int nGate, int nData)
	{
		Gate = nGate;
		Data = nData;		
	}
};

class CFileProcess 
{	
public:	
	void    m_fnCreateFolder(CString szPath);									
	int		m_fnOnWritefile(CString DstFileName, CString DstFileInfo );

	CFileProcess(); 
	virtual ~CFileProcess();

private:
	CString m_fnLastToken(CString strSplitVal, CString strSplit);
	int     m_fnLastTokenCnt(CString strSplitVal, CString strSplit);
	int		m_fnCutToken(CString strSplitVal, CString strSplit);

protected:	

public:
};

/// ��ũ ��ǥ �� G/D ��ǥ ����� ���� ����/Pitch/Resolution ���� �����ϴ� Class
/// GUI ���� Recipe �� �����ϸ� Recipe ���� ������ �ִٰ� ����� �� ���
class CWriteResultInfo 
{	

private:	
	CFileProcess m_FileLOGPROC;
	WorkCoordCrt m_stWorkCoordCrt;

public:
	CWriteResultInfo(); 
	virtual ~CWriteResultInfo();


	//	������ 18.12.06
	//	MergeTool Falg �� ������� ������ ����
	WorkCoordCrt GetWorkCoordCrt(){	return m_stWorkCoordCrt;	};
	

	// KSW 17.11.24 - Panel Size
	double		GetPanelSizeX(){ return m_stWorkCoordCrt.dPanelSizeX; };
	double		GetPanelSizeY(){ return m_stWorkCoordCrt.dPanelSizeY; };

	// Get
	// 17.07.07 Ratio ���� LT ��� Corner ���� �ѱ⵵�� ���� - ��ǥ�� ��� ��� ����
	Coord		CalcWorkCoord(cv::Rect rcAlignedCell, Coord cpPixelCoord, int nDefectRatio, int nCurRatio);	// �ȼ� �߽���ǥ�� ���� CPoint -> Coord �� ����

	/// ���� ��ǥ�� ���� ��ǥ,GATE/DATA�� ����� �̿��Ͽ� GATE/DATA LINE ���� ����.
	GD_POINT	CalcGateDataCoord(cv::Rect rcAlignedCell, Coord CoordPixel, int nDefectRatio, int nCurRatio);	// �ȼ� �߽���ǥ�� ���� CPoint -> Coord �� ����
	double		CalcDistancePixelToUm(double dPixelDistance, int nCameraNum, int nRatio);
	double		GetCamResolution(int nCameraNum)	{	return m_stWorkCoordCrt.dResolution[nCameraNum]		;};
	void		GetCalcResolution(cv::Rect rcAlignedCell, double& dHResolution, double& dVResolution)	{	dHResolution = m_stWorkCoordCrt.dPanelSizeX / (rcAlignedCell.width * 1.0) * 1000; 	\
																											dVResolution = m_stWorkCoordCrt.dPanelSizeY / (rcAlignedCell.height * 1.0) * 1000;	};
	//CIH 2017.07.14
	// JudgementRepair���� ���� ���� ���� ���Ͽ�  
	// m_stWorkCoordCrt ������ private
	//
	void		GetWorkCoordUsingRepair(int &nWorkOriginPosition, int &nWorkDirection )	{	nWorkOriginPosition = m_stWorkCoordCrt.nWorkOriginPosition;			 
																							nWorkDirection		= m_stWorkCoordCrt.nWorkDirection;				};

	// Set
	void		SetWorkCoordInfo(double dPanelSizeX, double dPanelSizeY, int nCurWorkDirection, int nCurWorkOrgPos, int nCurWorkOriginX, int nCurWorkOriginY, 
							int nCurDataDrection, int nCurGDOriginPos, int nCurGDOrgX, int nCurGDOrgY, double dCurGatePitch, double dCurDataPitch, 
							double* dCurResolution);

	// ��� ���� ����
	int			WriteResultPanelData(CString DstFileName, ResultPanelData& resultPanelData);
	int			WriteResultPanelData_ToMainPC(CString DstFileName, ResultPanelData& resultPanelData);

	int			WritePanelTrend(CString DstFileName, int* pDefectTrend, CString strPanelID, CString strPanelGrade);
	int			WriteFinalDefect(CString DstFileName, int nDefectNum, CString strPanelID);

private:
	int			m_fnWriteHeaderInfo(CString DstFileName, ResultHeaderInfo& HeaderInfo);	
	int			m_fnWritePanelInfo(CString DstFileName, ResultPanelInfo& PanelInfo);
	int			m_fnWriteDefectInfo(CString DstFileName, CArray<ResultDefectInfo, ResultDefectInfo&> &DefectInfo);
	CString		m_fnGetHeaderString(ResultHeaderInfo HeaderInfo);

private:
	CString		m_fnDivisionPoint(CString  strSplitVal, CString strSplit);
	CString		m_fnIntToCstr(int iConvert,CString strDivision=_T(""));
	CString		m_fnDblToCstr(double iConvert,CString strDivision);
	CString		m_fnBoolToCstr(bool iConvert);
	CString		m_fnConvertDateFormat(CString strConvert, CString strDivision);		

protected:	
public:
private:
};

struct RepeatDefectInfo
{
	ENUM_DEFECT_JUDGEMENT	eDefType;
	CPoint					ptCenterPos;
	int						nRepeatCount;
	int						nStageNo;		//2018.09.21 MDJ APP Repeat Defect

	RepeatDefectInfo()
	{
		eDefType = (ENUM_DEFECT_JUDGEMENT)0;
		ptCenterPos = CPoint(0, 0);
		nRepeatCount = 0;
		nStageNo = 0;
	}
	void SetRepeatInfo(ENUM_DEFECT_JUDGEMENT eDefectJudge, DOUBLE dX, DOUBLE dY)
	{
		// ���� �ҷ� ��ǥ�ڵ�
		eDefType = eDefectJudge;
		// CCD �ҷ� - Pixel �߽���ǥ�� ���
		ptCenterPos.x = (LONG)dX;
		ptCenterPos.y = (LONG)dY;
		nRepeatCount = 1;
	}
	// 2018.09.21 MDJ APP Repeat Defect
	void SetRepeatInfo(ENUM_DEFECT_JUDGEMENT eDefectJudge, DOUBLE dX, DOUBLE dY, INT StageNo)
	{
		// ���� �ҷ� ��ǥ�ڵ�
		eDefType = eDefectJudge;
		// CCD �ҷ� - Pixel �߽���ǥ�� ���
		ptCenterPos.x = (LONG)dX;
		ptCenterPos.y = (LONG)dY;
		nRepeatCount = 1;
		nStageNo = StageNo;
	}

	// List Merge �� Pixel ��ǥ �������� ���� ������
	bool operator < (const RepeatDefectInfo& p)
	{
		return (this->ptCenterPos.x + this->ptCenterPos.y < p.ptCenterPos.x + p.ptCenterPos.y);
	}
	bool operator > (const RepeatDefectInfo& p)
	{
		return (this->ptCenterPos.x + this->ptCenterPos.y > p.ptCenterPos.x + p.ptCenterPos.y);
	}	
	// �ӵ� ���� �׽�Ʈ - Functor
	void CheckVal(const std::list<RepeatDefectInfo>* nVal)
	{
		struct DoSetVal1
		{
			DoSetVal1(RepeatDefectInfo* val) : m_val(val) {}
			bool operator()(RepeatDefectInfo This)
			{
				// 			if (NULL == This) // NULL �� ���� ���ذ���.
				// 				return false;
				//			This.CheckVal1(m_val);
				return true;
			}
			const RepeatDefectInfo* m_val;
		};
		
		// ���� �ҷ� ����Ʈ
		//for_each(nVal->begin(), nVal->end(), DoSetVal1(this));
		//for (iterSrc = nVal->begin(); iterSrc != nVal->end(); )
		{
			
		}

// 		// �ϳ��� ��ġ�°� ������ ���� �ҷ� ����
// 		if (!bRepeat)
// 		{
// 			this->erase();
// 		}
// 		else
// 		{
// 			iterDst++;
// 		}
	}
};

struct ListCurDefect
{
	std::list<RepeatDefectInfo> listCurDefInfo[eCOORD_KIND];	// ���� �ҷ� ����Ʈ
	BOOL bUseChkRptDefect[eCOORD_KIND];							// Pixel, Work ��ǥ �ߺ� üũ ��� ����

	ListCurDefect()
	{
		for (int i=0; i<eCOORD_KIND; i++)
		{
			listCurDefInfo[i].clear();
			bUseChkRptDefect[i] = false;
		}
	}
 	void Add_Tail(ENUM_KIND_OF_REPEAT_COORD eKind, ENUM_DEFECT_JUDGEMENT eJudge, ResultDefectInfo* pDefect)
 	{
 		if (bUseChkRptDefect[eKind])
 		{
 			RepeatDefectInfo stRepeatDefectInfo;
 			DOUBLE dX = 0., dY = 0.;
 
 			if (eKind ==ePIXEL)
 			{
 				dX = pDefect->Pixel_Center_X;
 				dY = pDefect->Pixel_Center_Y;
 			}
 			else
 			{
 				dX = pDefect->Repair_Coord_X;
 				dY = pDefect->Repair_Coord_Y;
 			}
 			stRepeatDefectInfo.SetRepeatInfo(eJudge, dX, dY);
 			listCurDefInfo[eKind].push_back(stRepeatDefectInfo);
 		}
 	}
	// 2018.09.21 MDJ APP Repeat Defect
	void Add_Tail(ENUM_KIND_OF_REPEAT_COORD eKind, ENUM_DEFECT_JUDGEMENT eJudge, ResultDefectInfo* pDefect, INT nStageNo)
	{
		if (bUseChkRptDefect[eKind])
		{
			RepeatDefectInfo stRepeatDefectInfo;
			DOUBLE dX = 0., dY = 0.;

			if (eKind ==eMACHINE)
			{
				dX = pDefect->Pixel_Center_X;
				dY = pDefect->Pixel_Center_Y;
			}
			else
			{
				dX = pDefect->Repair_Coord_X;
				dY = pDefect->Repair_Coord_Y;
			}
			stRepeatDefectInfo.SetRepeatInfo(eJudge, dX, dY, nStageNo);
			listCurDefInfo[eKind].push_back(stRepeatDefectInfo);
		}
	}
};


#endif // !defined(AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_)

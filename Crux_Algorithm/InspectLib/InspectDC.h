#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "MatBuf.h"				// 메모리 관리
#include "InspectLibLog.h"


enum ENUM_PARA_UNIF
{
	E_PARA_UNIF_TEXT = 0,
	E_PARA_UNIF_RESIZE,	// IMAGE RESIZE
	E_PARA_UNIF_POW, // IMAGE POW
	E_PARA_UNIF_DIAGONAL_LINE_THICKNESS, // 대각선
	E_PARA_UNIF_HISTO_TEXT, //HISTOGRAM ON/OFF
	E_PARA_UNIF_HISTO_MIN,	//HISTOGRAM MIN VALUE
	E_PARA_UNIF_HISTO_MAX,	//HISTOGRAM MAX VALUE
	E_PARA_UNIF_GAUS_TEXT,	//GAUSSIAN BLUR ON/OFF
	E_PARA_UNIF_GAUS_SIZE_X,
	E_PARA_UNIF_GAUS_SIZE_Y,
	E_PARA_UNIF_GAUS_SIG,
	E_PARA_UNIF_NONINSPAREA_TEXT,
	E_PARA_UNIF_NONINSPAREA_LAFT,
	E_PARA_UNIF_NONINSPAREA_RIGHT,
	E_PARA_UNIF_NONINSPAREA_TOP,
	E_PARA_UNIF_NONINSPAREA_BOTTOM,
	E_PARA_UNIF_DIVISION_AREA_TEXT,
	E_PARA_UNIF_DIVISION_AREA_X,
	E_PARA_UNIF_DIVISION_AREA_Y,
	E_PARA_UNIF_RATIO_TEXT,
	E_PARA_UNIF_DARK_RATIO,
	E_PARA_UNIF_BRIGHT_RATIO,
	E_PARA_UNIF_TH_TEXT,
	E_PARA_UNIF_TH_DARK,
	E_PARA_UNIF_TH_BRIGHT,

	E_PARA_UNIF_TOTAL_COUNT
};
enum ENUM_PARA_DC
{
	E_PARA_DC_TEXT = 0,
	E_PARA_DC_Strong_Delta,
	E_PARA_DC_Weak_Delta,
	E_PARA_DC_Bright_Ratio,
	E_PARA_DC_Dark_Ratio,
	E_PARA_DC_MEAN_SIZE,
	E_PARA_DC_HistoGram_MinValue,
	E_PARA_DC_HistoGram_MaxValue,
	E_PARA_DC_ROI_Size,
	E_PARA_DC_Padding_Left,
	E_PARA_DC_Padding_Right,
	E_PARA_DC_Padding_Top,
	E_PARA_DC_Padding_Bottom,
	E_PARA_DC_VIEW_GRID_TEXT,
	E_PARA_DC_VIEW_GRID_X,
	E_PARA_DC_VIEW_GRID_Y,

	E_PARA_DC_SAVE_AMPIMAGE,

	E_PARA_AD_TOTAL_COUNT
};

enum ENUM_PARA_LeakLight
{
	E_PARA_LeakLight_TEXT = 0,
	E_PARA_LeakLight_Threshold,
	E_PARA_LeakLight_Resize,
	E_PARA_LeakLight_MORP_Dilate,
	E_PARA_LeakLight_Morp_Erode,

// 	E_PARA_LeakLight_Strong_Delta,
// 	E_PARA_LeakLight_Weak_Delta,
// 	E_PARA_LeakLight_Bright_Ratio,
// 	E_PARA_LeakLight_Dark_Ratio,
// 	E_PARA_LeakLight_MEAN_SIZE,
// 	E_PARA_LeakLight_HistoGram_MaxValue,
// 	E_PARA_LeakLight_ResizeRatio,

	E_PARA_LeakLight_TOTAL_COUNT
};


enum ENUM_PARA_ETC_Mura
{
	E_PARA_ETC_MURA_TEXT = 0,
	E_PARA_ETC_MURA_RESIZE,
	E_PARA_ETC_MURA_AMP,
	E_PARA_ETC_MURA_THRESH_LOCAL_SIZE,
	E_PARA_ETC_MURA_THRESH_B,
	E_PARA_ETC_MURA_THRESH_D,
	E_PARA_ETC_MURA_EDGE_DELETE_THICK,
	E_PARA_ETC_MURA_MORP_CLOSE
};

enum ENUM_PARA_CORNER_BURN
{
	E_PARA_CornerBurn_TEXT = 0,
	E_PARA_CornerBurn_Delta,
	E_PARA_CornerBurn_RoiWidth,
	E_PARA_CornerBurn_EdgeContactRatio,
};

enum ENUM_PARA_EdgeDiffGV
{
	E_PARA_EdgeDiffGV_Version = 0,
	E_PARA_EdgeDiffGV_DiffGV,
	E_PARA_EdgeDiffGV_Gap_A,
	E_PARA_EdgeDiffGV_Gap_B,
	E_PARA_EdgeDiffGV_Width_A,
	E_PARA_EdgeDiffGV_Width_B,
	E_PARA_EdgeDiffGV_LengthRatio,
	E_PARA_EdgeDiffGV_SumLengthRoiSize

};

enum ENUM_PARA_EdgeDiffGV2
{
	E_PARA_EdgeDiffGV2_Version = 0,
	E_PARA_EdgeDiffGV2_DiffGV,
	E_PARA_EdgeDiffGV2_H_Cut,
	E_PARA_EdgeDiffGV2_HD_Cut,
	E_PARA_EdgeDiffGV2_Gap_A,
	E_PARA_EdgeDiffGV2_Gap_B,
	E_PARA_EdgeDiffGV2_Gap_C,
	E_PARA_EdgeDiffGV2_Width_A,
	E_PARA_EdgeDiffGV2_Width_B,
	E_PARA_EdgeDiffGV2_Width_C,
	E_PARA_EdgeDiffGV2_LengthRatio,
	E_PARA_EdgeDiffGV2_SumLengthRoiSize

};

enum ENUM_BINARY_IMAGE2
{
	E_BINARY_2NIT_BRIGHT,
	E_BINARY_2NIT_DARK
};

class CInspectDC
{
public:
	CInspectDC(void);
	virtual ~CInspectDC(void);

	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data) { cMem = data; };
	CMatBuf*	GetMem() { return	cMem; };

	// 로그 관리
	InspectLibLog*		m_cInspectLibLog;
	clock_t				m_tInitTime;
	clock_t				m_tBeforeTime;
	wchar_t*			m_strAlgLog;

	void		SetLog(InspectLibLog* cLog, clock_t tTimeI, clock_t tTimeB, wchar_t* strLog)
	{
		m_tInitTime = tTimeI;
		m_tBeforeTime = tTimeB;
		m_cInspectLibLog = cLog;
		m_strAlgLog = strLog;
	};

	void		writeInspectLog(int nAlgType, char* strFunc, wchar_t* strTxt)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, m_strAlgLog);
	};


	// Main 검사 알고리즘
	long FindDCDefect(cv::Mat matSrcBuffer, int & nResult, float & fUnif,  cv::Mat& matBKBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, CString strResultPath, CString strInspPath,
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, double dResolution, CString * arrStrAnalysisData, wchar_t* strContourTxt = NULL);

protected:
	long LogicStart_DC(Mat& matSrcImage, float & fUnif, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, CString strResultPath, CString strInspPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner, CString * arrStrAnalysisData, stDefectInfo* pResultBlob, double dResolution);


	long LogicStart_UNIF(Mat& matSrcImage, float & fUnif, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, CString strResultPath, CString strInspPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner, CString * arrStrAnalysisData, stDefectInfo* pResultBlob, double dResolution);
	//long LogicStart_LeakLight(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution);
	//long LogicStart_ETC_MURA(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution);
	//long LogicStart_CornerBurn(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution);
	//long LogicStart_EdgeDiffGV(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution);


protected:
	long	ImageSave(CString strPath, cv::Mat matSrcBuf);
	bool m_bProcess;

	int _fnOnWritefile(CString DstFileName, CString DstFileInfo)
	{
		CStdioFile fileWrite;
		setlocale(LC_ALL, "korean");
		try
		{
			if (fileWrite.Open(DstFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
			{
				fileWrite.SeekToEnd();
				fileWrite.WriteString(DstFileInfo);
			}
			fileWrite.Close();
			return 0;
		}
		catch (CFileException* e)
		{
			e->Delete();
			fileWrite.Close();
			return 1;
		}
	}


	// DefectCode 처리
public:
	void	initDefectInfo(map<CString, UINT> di)
	{
		DI = di;
	}

protected:
	map<CString, UINT> DI;

};
// AD�� �˰��� ������ ���� �; ����

#include "StdAfx.h"
#include "InspectDC.h"


#include <opencv2/features2d/features2d.hpp>
#include "FeatureExtraction.h"

CInspectDC::CInspectDC(void)
{
	m_bProcess = false;

	cMem = NULL;
	m_cInspectLibLog = NULL;
	m_strAlgLog = NULL;
	m_tInitTime = 0;
	m_tBeforeTime = 0;
}

CInspectDC::~CInspectDC(void)
{

}

long CInspectDC::FindDCDefect(cv::Mat matSrcBuffer, int & nResult, float & fUnif, cv::Mat& matBKBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, CString strResultPath, CString strInspPath,
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, double dResolution, CString * arrStrAnalysisData, wchar_t* strContourTxt)
{

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	// �Ķ���� NULL�� ���.
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth = (long)matSrcBuffer.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcBuffer.rows;	// ���� ���� ũ��

	long	nAlgoListNum = nCommonPara[E_PARA_COMMON_ALG_NUMBER];

	writeInspectLog(E_ALG_TYPE_AVI_DC, __FUNCTION__, _T("Start."));

	cv::Mat matDstImage[2];
	// 	matDstImage[E_BINARY_2NIT_BRIGHT] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type(), false);
	// 	matDstImage[E_BINARY_2NIT_DARK] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type(), false);

		// ū �ܰ�
	CRect rectROI = new CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// �˻� ������ ���� ũ�⸦ ��� ���
	if (rectROI.left < 0 ||
		rectROI.top < 0 ||
		rectROI.right >= nWidth ||
		rectROI.bottom >= nHeight)	return E_ERROR_CODE_ROI_OVER;


	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	m_bProcess = false;
	CString strPath;
	strPath.Format(_T("%s"), strAlgPath);

	writeInspectLog(E_ALG_TYPE_AVI_DC, __FUNCTION__, _T("Init."));

	cv::Mat matSrcBuf8bit;
	if (matSrcBuffer.type() == CV_8U)
	{
		matSrcBuf8bit = matSrcBuffer;
	}
	else
		matSrcBuffer.convertTo(matSrcBuf8bit, CV_8U, 1. / 16.);


	//switch (nAlgoListNum)	// ���� ���� �˰��� �ٸ�.
	//{
	//case E_ALGORITHM_DC:
	//{
	//	nErrorCode = LogicStart_DC(matSrcBuffer, fUnif, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, strResultPath, EngineerDefectJudgment, ptCorner, arrStrAnalysisData, pResultBlob, dResolution);
	//	break;
	//}
	//case E_ALGORITHM_LEAK_LIGHT:
	//{
	//	nErrorCode = LogicStart_LeakLight(matSrcBuffer, nResult, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, strResultPath, EngineerDefectJudgment, pResultBlob, ptCorner, arrStrAnalysisData, dResolution);
	//	break;
	//}
	//case E_ALGORITHM_ETC_MURA:
	//{
	//	nErrorCode = LogicStart_ETC_MURA(matSrcBuffer, nResult, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, strResultPath, EngineerDefectJudgment, pResultBlob, ptCorner, arrStrAnalysisData, dResolution);
	//	break;
	//}
	//case E_ALGORITHM_CORNER_BURN:
	//{
	//	nErrorCode = LogicStart_CornerBurn(matSrcBuffer, nResult, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, strResultPath, EngineerDefectJudgment, pResultBlob, ptCorner, arrStrAnalysisData, dResolution);
	//	break;
	//}
	//case E_ALGORITHM_EDGE_DIFFGV:
	//{
	//	nErrorCode = LogicStart_EdgeDiffGV(matSrcBuffer, nResult, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, strResultPath, EngineerDefectJudgment, pResultBlob, ptCorner, arrStrAnalysisData, dResolution);
	//	break;
	//}

	switch (nImageNum)	// ���� ���� �˰��� �ٸ�.
	{
	case E_IMAGE_CLASSIFY_AVI_L3:
	{
		nErrorCode = LogicStart_UNIF(matSrcBuffer, fUnif, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, strResultPath, strInspPath, EngineerDefectJudgment, ptCorner, arrStrAnalysisData, pResultBlob, dResolution);
		//nErrorCode = LogicStart_DC(matSrcBuffer, fUnif, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, strResultPath, strInspPath, EngineerDefectJudgment, ptCorner, arrStrAnalysisData, pResultBlob, dResolution);
		break;
	}
	default:
		return E_ERROR_CODE_TRUE;		// ���� ��ȣ�� �߸� ���� ���.
	}

	if (nErrorCode != E_ERROR_CODE_TRUE)
	{

		return nErrorCode;
	}

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Memory Release."));

	return nErrorCode;
}


bool _Cmp(const pair<ushort, CPoint> &a, const pair<ushort, CPoint> &b)
{
	return a.first > b.first;
}


long CInspectDC::LogicStart_UNIF(Mat& matSrcImage, float & fResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath, CString strResultPath, CString strInspPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner, CString * arrStrAnalysisData, stDefectInfo* pResultBlob, double dResolution)
{
	// ���� �ڵ�
	long	nErrorCode = E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
	long	nAlgoListNum = nCommonPara[E_PARA_COMMON_ALG_NUMBER];

	//////////////////////////////////////////////////////////////////////////
	int nReSize = dPara[E_PARA_UNIF_RESIZE];
	double dPow = dPara[E_PARA_UNIF_POW];
	int nDiagonalThickness = dPara[E_PARA_UNIF_DIAGONAL_LINE_THICKNESS];


	bool bHistoUse = (dPara[E_PARA_UNIF_HISTO_TEXT] > 0) ? true : false;
	int nHistoMin = dPara[E_PARA_UNIF_HISTO_MIN];
	int nHistoMax = dPara[E_PARA_UNIF_HISTO_MAX];


	bool bGausUse = (dPara[E_PARA_UNIF_GAUS_TEXT] > 0) ? true : false;
	int nGausX = dPara[E_PARA_UNIF_GAUS_SIZE_X];
	int nGausY = dPara[E_PARA_UNIF_GAUS_SIZE_Y];
	double dGausSig = dPara[E_PARA_UNIF_GAUS_SIG];


	bool bNonInspsUse = (dPara[E_PARA_UNIF_NONINSPAREA_TEXT] > 0) ? true : false;
	int nNonLeft = dPara[E_PARA_UNIF_NONINSPAREA_LAFT];
	int nNonRight = dPara[E_PARA_UNIF_NONINSPAREA_RIGHT];
	int nNonTop = dPara[E_PARA_UNIF_NONINSPAREA_TOP];
	int nNonBottom = dPara[E_PARA_UNIF_NONINSPAREA_BOTTOM];


	bool bDivisionUse = (dPara[E_PARA_UNIF_DIVISION_AREA_TEXT] > 0) ? true : false;
	int nDivisionX = dPara[E_PARA_UNIF_DIVISION_AREA_X];
	int nDivisionY = dPara[E_PARA_UNIF_DIVISION_AREA_Y];
	double dDarkRatio = dPara[E_PARA_UNIF_DARK_RATIO];
	double dBrightRatio = dPara[E_PARA_UNIF_BRIGHT_RATIO];

	double nDarkTh = dPara[E_PARA_UNIF_TH_DARK];
	double nBrightTh = dPara[E_PARA_UNIF_TH_BRIGHT];

	int nDarkAreaCnt = 0;
	int nBrightAreaCnt = 0;

	int nTextXOffset = 15;
	int nTextYOffset = 50;


	Mat matSrcBuffer;
	matSrcImage.copyTo(matSrcBuffer);

	//������׷� Min Max Value �ڸ��� -> L3 Pattern Garb �� ���� ���Ⱚ������ CCD�� ���� ����
	if (bHistoUse)
	{
		vector<pair<uchar, CPoint>> vecValues;
		for (int j = 0; j < matSrcBuffer.rows; ++j)
		{
			uchar * ptr = matSrcBuffer.ptr<uchar>(j);
			for (int i = 0; i < matSrcBuffer.cols; ++i)
			{
				if (ptr[i] >= nHistoMin)
					if (ptr[i] <= nHistoMax)
						if (ptr[i] == 0)
							vecValues.push_back(make_pair(ptr[i], CPoint(i, j)));
			}
		}
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Src_MinMax_GVCut.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuffer);
	}

	//16bit ��ȯ 
	Mat matSrc16U = Mat::zeros(matSrcImage.rows, matSrcImage.cols, CV_16UC1);

	matSrcImage.convertTo(matSrc16U, CV_16UC1);

	//Image ����
	cv::pow(matSrc16U, dPow, matSrc16U);

	//���ʿ��� �ܰ� ����
	Rect rSrcRoi;
	rSrcRoi.x = rectROI.left + nNonLeft;
	rSrcRoi.y = rectROI.top + nNonTop;
	rSrcRoi.width = rectROI.Width() - nNonLeft - nNonRight;
	rSrcRoi.height = rectROI.Height() - nNonTop - nNonBottom;

	

	Mat matSrcROIBuffer;

	matSrc16U(rSrcRoi).copyTo(matSrcROIBuffer);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Src_ROI.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matSrcROIBuffer);
	}

	int nDivisionXArea = (int)matSrcROIBuffer.cols / nDivisionX;
	int nDivisionYArea = (int)matSrcROIBuffer.rows / nDivisionY;

	vector<double> dXmeanvalue;
	vector<double> dYmeanvalue;
	vector<double> dBoxmeanvalue;

	//X���� ���
	for (int x = 0; x < nDivisionX; x++)
	{
		Rect rXmean;

		//������ �ȼ��� ������ �迭�� �־���
		if (x == nDivisionX - 1)
		{
			rXmean.x = nDivisionXArea*x;
			rXmean.y = 0;
			rXmean.width = nDivisionXArea + (matSrcROIBuffer.cols - (nDivisionXArea * nDivisionX));
			rXmean.height = matSrcROIBuffer.rows;
		}
		else
		{
			rXmean.x = nDivisionXArea*x;
			rXmean.y = 0;
			rXmean.width = nDivisionXArea;
			rXmean.height = matSrcROIBuffer.rows;
		}

		double dXmean = 0.0;
		cv::Scalar m = cv::mean(matSrcROIBuffer(rXmean));
		dXmean = m[0];
		dXmeanvalue.push_back(dXmean);
	}

	//Y���� ���
	for (int y = 0; y < nDivisionY; y++)
	{
		Rect rYmean;

		//������ �ȼ��� ������ �迭�� �־���
		if (y == nDivisionY - 1)
		{
			rYmean.x = 0;
			rYmean.y = nDivisionYArea*y;
			rYmean.width = matSrcROIBuffer.cols;
			rYmean.height = nDivisionYArea + (matSrcROIBuffer.rows - (nDivisionYArea * nDivisionY));
		}
		else
		{
			rYmean.x = 0;
			rYmean.y = nDivisionYArea*y;
			rYmean.width = matSrcROIBuffer.cols;
			rYmean.height = nDivisionYArea;
		}

		double dYmean = 0.0;
		cv::Scalar m = cv::mean(matSrcROIBuffer(rYmean));
		dYmean = m[0];
		dYmeanvalue.push_back(dYmean);
	}

	//���� �ڽ� ��հ�
	//Y���� ���
	for (int y = 0; y < nDivisionY; y++)
	{
		Rect rBoxmean;
		if (y == nDivisionY - 1)
		{
			rBoxmean.y = nDivisionYArea*y;
			rBoxmean.height = nDivisionYArea + (matSrcROIBuffer.rows - (nDivisionYArea * nDivisionY));
		}
		else
		{
			rBoxmean.y = nDivisionYArea*y;
			rBoxmean.height = nDivisionYArea;
		}

		//X���� ���
		for (int x = 0; x < nDivisionX; x++)
		{
			//������ �ȼ��� ������ �迭�� �־���
			if (x == nDivisionX - 1)
			{
				rBoxmean.x = nDivisionXArea*x;
				rBoxmean.width = nDivisionXArea + (matSrcROIBuffer.cols - (nDivisionXArea * nDivisionX));
			}
			else
			{
				rBoxmean.x = nDivisionXArea*x;
				rBoxmean.width = nDivisionXArea;
			}

			double dBoxmean = 0.0;
			cv::Scalar m = cv::mean(matSrcROIBuffer(rBoxmean));
			dBoxmean = m[0];
			dBoxmeanvalue.push_back(dBoxmean);

		}

	}

	// �߰� Image Ȯ��
	// X���� ���� �߰��̹���
	if (bImageSave)
	{
		Mat MatImageSave;

		matSrcROIBuffer.copyTo(MatImageSave);

		cv::cvtColor(MatImageSave, MatImageSave, COLOR_GRAY2RGB);
		char cstr[30];

		for (int i = 1; i < nDivisionX; i++)
		{
			cv::line(MatImageSave, Point(nDivisionXArea*i, 0), Point(nDivisionXArea*i, MatImageSave.rows), Scalar(0, 0, 255), 3, 8, 0);

		}
		for (int j = 0; j < nDivisionX; j++)
		{

			sprintf_s(cstr, "%.2lf", dXmeanvalue[j]);
			putText(MatImageSave, cstr, Point(nDivisionXArea*j + nTextXOffset, nTextYOffset), 1, 2, Scalar(0, 0, 255), 3, 8, false);
		}


		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_X_Division.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	// �߰� Image Ȯ��
	// Y���� ���� �߰��̹���
	if (bImageSave)
	{
		Mat MatImageSave;

		matSrcROIBuffer.copyTo(MatImageSave);

		cv::cvtColor(MatImageSave, MatImageSave, COLOR_GRAY2RGB);
		char cstr[30];

		for (int i = 1; i < nDivisionY; i++)
		{
			cv::line(MatImageSave, Point(0, nDivisionYArea*i), Point(MatImageSave.cols, nDivisionYArea*i), Scalar(0, 0, 255), 3, 8, 0);
		}
		for (int j = 0; j < nDivisionY; j++)
		{
			sprintf_s(cstr, "%.2lf", dYmeanvalue[j]);
			putText(MatImageSave, cstr, Point(nTextXOffset, nDivisionYArea*j + nTextYOffset), 1, 2, Scalar(0, 0, 255), 3, 8, false);
		}

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Y_Division.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	// �߰� Image Ȯ��
	// BOX �߰��̹���
	if (bImageSave)
	{
		Mat MatImageSave;

		matSrcROIBuffer.copyTo(MatImageSave);

		cv::cvtColor(MatImageSave, MatImageSave, COLOR_GRAY2RGB);
		char cstr[30];

		for (int i = 1; i < nDivisionY; i++)
		{
			cv::line(MatImageSave, Point(0, nDivisionYArea*i), Point(MatImageSave.cols, nDivisionYArea*i), Scalar(0, 0, 255), 3, 8, 0);
		}
		for (int i = 1; i < nDivisionX; i++)
		{
			cv::line(MatImageSave, Point(nDivisionXArea*i, 0), Point(nDivisionXArea*i, MatImageSave.rows), Scalar(0, 0, 255), 3, 8, 0);
		}

		int dLineChange = 0;

		for (int i = 0; i < nDivisionY; i++)
		{
			for (int j = 0; j < nDivisionX; j++)
			{
				dLineChange = i*nDivisionX;
				sprintf_s(cstr, "%.2lf", dBoxmeanvalue[j + dLineChange]);
				putText(MatImageSave, cstr, Point(nDivisionXArea*j + nTextXOffset, nDivisionYArea*i + nTextYOffset), 1, 2, Scalar(0, 0, 255), 3, 8, false);
			}
		}

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_BOX_Division.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	// �߰� Image Ȯ��
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_X_Area_MeanData.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "wt");

		if (out)
		{
			for (int i = 0; i < nDivisionX; i++)
			{
				fprintf_s(out, "%d,%f\n", i, dXmeanvalue[i]);
			}

			fclose(out);
		}
	}

	// �߰� Image Ȯ��
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Y_Area_MeanData.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "wt");

		if (out)
		{
			for (int i = 0; i < nDivisionY; i++)
			{
				fprintf_s(out, "%d,%f\n", i, dYmeanvalue[i]);
			}

			fclose(out);
		}
	}

	// �߰� Image Ȯ��
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_BOX_Area_MeanData.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "wt");
		int dLineChange = 0;
		if (out)
		{
			for (int i = 0; i < nDivisionY; i++)
			{
				for (int j = 0; j < nDivisionX; j++)
				{
					dLineChange = i*nDivisionX;
					fprintf_s(out, "%f,", dBoxmeanvalue[j + dLineChange]);
				}
				fprintf_s(out, "\n");
			}

			fclose(out);
		}
	}


	Mat matLTTORBMask = Mat::zeros(matSrcROIBuffer.rows, matSrcROIBuffer.cols, CV_8UC1);
	Mat matRTTOLBMask = Mat::zeros(matSrcROIBuffer.rows, matSrcROIBuffer.cols, CV_8UC1);

	matLTTORBMask.setTo(0);
	matRTTOLBMask.setTo(0);

	cv::line(matLTTORBMask, Point(0, 0), Point(matLTTORBMask.cols, matLTTORBMask.rows), Scalar::all(255), nDiagonalThickness, 8, 0);
	cv::line(matRTTOLBMask, Point(matRTTOLBMask.cols, 0), Point(0, matLTTORBMask.rows), Scalar::all(255), nDiagonalThickness, 8, 0);

	double dLTRBMean;
	double dRTLBMean;

	cv::Scalar sLTRBMean = cv::mean(matSrcROIBuffer, matLTTORBMask);
	cv::Scalar sRTLBMean = cv::mean(matSrcROIBuffer, matRTTOLBMask);

	dLTRBMean = sLTRBMean[0];
	dRTLBMean = sRTLBMean[0];

	if (bImageSave)
	{
		Mat MatImageSave;

		matSrcROIBuffer.copyTo(MatImageSave);
		matLTTORBMask.convertTo(matLTTORBMask, CV_16UC1);
		cv::bitwise_or(MatImageSave, matLTTORBMask, MatImageSave);

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_LTTORB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}


	if (bImageSave)
	{
		Mat MatImageSave;

		matSrcROIBuffer.copyTo(MatImageSave);
		matRTTOLBMask.convertTo(matRTTOLBMask, CV_16UC1);
		cv::bitwise_or(MatImageSave, matRTTOLBMask, MatImageSave);

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_RTTOLB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	// �߰� Image Ȯ��
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Diagonal_MeanData.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "wt");

		if (out)
		{
			fprintf_s(out, "LTtoRB,%f\n", dLTRBMean);
			fprintf_s(out, "RTtoLB,%f\n", dRTLBMean);
			fclose(out);
		}
	}

	vector<int> nXmeanDarkResult;
	vector<int> nXmeanBrightResult;
	vector<int> nYmeanDarkResult;
	vector<int> nYmeanBrightResult;
	vector<int> nLTTRBDarkResult;
	vector<int> nLTTRBBrightResult;
	vector<int> nRTTLBDarkResult;
	vector<int> nRTTLBBrightResult;

	vector<int> nDarkResult;
	vector<int> nBrightResult;

	//���� ����� �Է�
	int dXLineChange = 0;
	int dTrue = 1;
	int dFalse = 0;

	for (int y = 0; y < nDivisionY; y++)
	{
		//X����
		for (int x = 0; x < nDivisionX; x++)
		{
			dXLineChange = y*nDivisionX;
			//X���� ��
			if (dBoxmeanvalue[x + dXLineChange] < dXmeanvalue[x] * dDarkRatio)
			{
				nXmeanDarkResult.push_back(dTrue);
			}
			else
			{
				nXmeanDarkResult.push_back(dFalse);
			}

			if (dBoxmeanvalue[x + dXLineChange] > dXmeanvalue[x] * dBrightRatio)
			{
				nXmeanBrightResult.push_back(dTrue);
			}
			else
			{
				nXmeanBrightResult.push_back(dFalse);
			}

			//Y ���⸸
			if (dBoxmeanvalue[x + dXLineChange] < dYmeanvalue[y] * dDarkRatio)
			{
				nYmeanDarkResult.push_back(dTrue);
			}
			else
			{
				nYmeanDarkResult.push_back(dFalse);
			}
			if (dBoxmeanvalue[x + dXLineChange] > dYmeanvalue[y] * dBrightRatio)
			{
				nYmeanBrightResult.push_back(dTrue);
			}
			else
			{
				nYmeanBrightResult.push_back(dFalse);
			}

			//LTTRB ��
			if (dBoxmeanvalue[x + dXLineChange] < dLTRBMean * dDarkRatio)
			{
				nLTTRBDarkResult.push_back(dTrue);
			}
			else
			{
				nLTTRBDarkResult.push_back(dFalse);
			}
			if (dBoxmeanvalue[x + dXLineChange] > dLTRBMean * dBrightRatio)
			{
				nLTTRBBrightResult.push_back(dTrue);
			}
			else
			{
				nLTTRBBrightResult.push_back(dFalse);
			}

			//RTTLB ��
			if (dBoxmeanvalue[x + dXLineChange] < dRTLBMean * dDarkRatio)
			{
				nRTTLBDarkResult.push_back(dTrue);
			}
			else
			{
				nRTTLBDarkResult.push_back(dFalse);
			}
			if (dBoxmeanvalue[x + dXLineChange] > dRTLBMean * dBrightRatio)
			{
				nRTTLBBrightResult.push_back(dTrue);
			}
			else
			{
				nRTTLBBrightResult.push_back(dFalse);
			}
		}
	}
	//���� ����� �Է�
	int dDarkcnt = 0;
	int dBrightcnt = 0;
	for (int y = 0; y < nDivisionY; y++)
	{
		//X����
		for (int x = 0; x < nDivisionX; x++)
		{
			dXLineChange = y*nDivisionX;
			dDarkcnt = nXmeanDarkResult[x + dXLineChange] + nYmeanDarkResult[x + dXLineChange] + nLTTRBDarkResult[x + dXLineChange] + nRTTLBDarkResult[x + dXLineChange];
			nDarkResult.push_back(dDarkcnt);
			dBrightcnt = nXmeanBrightResult[x + dXLineChange] + nYmeanBrightResult[x + dXLineChange] + nLTTRBBrightResult[x + dXLineChange] + nRTTLBBrightResult[x + dXLineChange];
			nBrightResult.push_back(dBrightcnt);
		}
	}


	Mat matDarkDstImage = Mat::zeros(matSrcROIBuffer.rows, matSrcROIBuffer.cols, CV_8UC1);
	Mat matBrightDstImage = Mat::zeros(matSrcROIBuffer.rows, matSrcROIBuffer.cols, CV_8UC1);
	int dDarkValue = 0;
	int dBrightValue = 0;
	int dResultChange = 0;

	for (int y = 0; y < nDivisionY; y++)
	{
		Rect rBoxmean;

		if (y == nDivisionY - 1)
		{
			rBoxmean.y = nDivisionYArea*y;
			rBoxmean.height = nDivisionYArea + (matDarkDstImage.rows - (nDivisionYArea * nDivisionY));
		}
		else
		{
			rBoxmean.y = nDivisionYArea*y;
			rBoxmean.height = nDivisionYArea;
		}

		//X���� ���
		for (int x = 0; x < nDivisionX; x++)
		{
			//������ �ȼ��� ������ �迭�� �־���
			if (x == nDivisionX - 1)
			{
				rBoxmean.x = nDivisionXArea*x;
				rBoxmean.width = nDivisionXArea + (matDarkDstImage.cols - (nDivisionXArea * nDivisionX));
			}
			else
			{
				rBoxmean.x = nDivisionXArea*x;
				rBoxmean.width = nDivisionXArea;
			}


			dResultChange = y*nDivisionX;

			dDarkValue = nDarkResult[x + dResultChange];
			if (dDarkValue > nDarkTh)nDarkAreaCnt++;
			matDarkDstImage(rBoxmean).setTo(dDarkValue);

			dBrightValue = nBrightResult[x + dResultChange];

			if (dBrightValue > nBrightTh)nBrightAreaCnt++;
			matBrightDstImage(rBoxmean).setTo(dBrightValue);

		}

	}

	// �߰� Image Ȯ��
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Dark_Result.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "wt");
		int dLineChange = 0;
		if (out)
		{
			for (int i = 0; i < nDivisionY; i++)
			{
				for (int j = 0; j < nDivisionX; j++)
				{
					dLineChange = i*nDivisionX;
					fprintf_s(out, "%d,", nDarkResult[j + dLineChange]);
				}
				fprintf_s(out, "\n");
			}

			fclose(out);
		}
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Bright_Result.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "wt");
		int dLineChange = 0;
		if (out)
		{
			for (int i = 0; i < nDivisionY; i++)
			{
				for (int j = 0; j < nDivisionX; j++)
				{
					dLineChange = i*nDivisionX;
					fprintf_s(out, "%d,", nBrightResult[j + dLineChange]);
				}
				fprintf_s(out, "\n");
			}

			fclose(out);
		}
	}

	if (bImageSave)
	{
		Mat MatImageSave;

		matBrightDstImage.copyTo(MatImageSave);

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Bright_Result.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	if (bImageSave)
	{
		Mat MatImageSave;

		matDarkDstImage.copyTo(MatImageSave);

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Dark_Result.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	Mat matDarkThDstImage;
	Mat matBrightThDstImage;

	cv::threshold(matDarkDstImage, matDarkThDstImage, nDarkTh, 255.0, THRESH_BINARY);
	cv::threshold(matBrightDstImage, matBrightThDstImage, nBrightTh, 255.0, THRESH_BINARY);

	if (bImageSave)
	{
		Mat MatImageSave;

		matBrightThDstImage.copyTo(MatImageSave);

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Bright_Result_Th_%d.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++, nBrightAreaCnt);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	if (bImageSave)
	{
		Mat MatImageSave;

		matDarkThDstImage.copyTo(MatImageSave);

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Dark_Result_Th_%d.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++, nDarkAreaCnt);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}
	int unif = 0;
	unif = nBrightAreaCnt + nDarkAreaCnt;
	float arrFeature[9];

	arrFeature[0] = unif;
	arrFeature[1] = nDarkAreaCnt;
	arrFeature[2] = nBrightAreaCnt;
	arrFeature[3] =	0.0;
	arrFeature[4] = 0.0;
	arrFeature[5] = 0.0;
	arrFeature[6] = 0.0;
	arrFeature[7] = 0.0;
	arrFeature[8] = 0.0;

	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_DC, m_tInitTime, m_tBeforeTime, m_strAlgLog);
	cFeatureExtraction.DoDefectBlobJudgment_DC(arrFeature, nCommonPara, EngineerDefectJudgment, pResultBlob, nAlgoListNum);

	Mat matResultOR;

	cv::bitwise_or(matBrightThDstImage, matDarkThDstImage, matResultOR);

	if (bImageSave)
	{
		Mat MatImageSave;

		matResultOR.copyTo(MatImageSave);

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_UNIT_%02d_Result_Th_OR_%d.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++, unif);
		cv::imwrite((cv::String)(CStringA)strTemp, MatImageSave);

		MatImageSave.release();
	}

	//��� ���� �����

	Mat matFinalResult;
	
	{

		Mat matOriMask;
		cv::cvtColor(matSrcBuffer, matOriMask, COLOR_GRAY2RGB);
		Rect rOrgROI;

		rOrgROI.x = rectROI.left;
		rOrgROI.y = rectROI.top;
		rOrgROI.width = rectROI.Width();
		rOrgROI.height = rectROI.Height();

		matSrcBuffer.copyTo(matFinalResult);

		cv::cvtColor(matFinalResult, matFinalResult, COLOR_GRAY2RGB);

		cv::rectangle(matFinalResult, rOrgROI, Scalar(30, 255, 255), CV_FILLED, 8, 0);
		
		matOriMask(rSrcRoi).copyTo(matFinalResult(rSrcRoi));

		Mat matColorS;
		Mat Color3[3];
		Mat matSameMask;
		Mat matDrakM; 
		Mat matBrightM;
		matBrightThDstImage.copyTo(matBrightM);
		matDarkThDstImage.copyTo(matDrakM);

		cv::bitwise_and(matBrightThDstImage, matDarkThDstImage, matSameMask);

		cv::subtract(matBrightM, matBrightM, matSameMask);
		cv::subtract(matDrakM, matDrakM, matSameMask);
		matOriMask(rSrcRoi).copyTo(matColorS);

		cv::split(matColorS, Color3);
		//0:B 1:G 2:R

		//�������: û��
		cv::add(Color3[0], matBrightM, Color3[0]);
		cv::subtract(Color3[1], matBrightM, Color3[1]);
		cv::subtract(Color3[2], matBrightM, Color3[2]);

		//��ο� ��� : ���

		cv::add(Color3[1], matDrakM, Color3[1]);
		cv::subtract(Color3[0], matDrakM, Color3[0]);
		cv::subtract(Color3[2], matDrakM, Color3[2]);

		//��ġ�� ��� : ����

		cv::add(Color3[2], matSameMask, Color3[2]);
		cv::subtract(Color3[1], matSameMask, Color3[1]);
		cv::subtract(Color3[0], matSameMask, Color3[0]);

		cv::merge(Color3, 3, matColorS);

		matColorS.copyTo(matFinalResult(rSrcRoi));
		
	}

	if (pResultBlob->nDefectCount > 0)
	{	
		CString str;
		str.Format(_T("%s\\DC_unif_%d.jpg"), strResultPath, unif);
		cv::imwrite((cv::String)(CStringA)str, matFinalResult);
	}
	
	//		// 5. �м��� ������
	//		//////////////////////////
	//		CString strAnalysisDataPath = strInspPath;
	//		CreateDirectory(strAnalysisDataPath, NULL);
	//
	//		SYSTEMTIME time;
	//		::GetLocalTime(&time);
	//
	//		// ���
	//		CString strFileName;
	//		strFileName.Format(L"%s\\%02d%02d%02d_DC.csv", strAnalysisDataPath, time.wYear, time.wMonth, time.wDay);
	//		if (!PathFileExists(strFileName))
	//		{
	//			CString str = _T("Time,Recipe,ID,Pattern,Unif,M-m,M-C,C-m,MaxDelta,M/m,M/C,C/m,MaxRatio,Avg,BrightGV,DarkGV,GridMaxGV,GridMinGV,GridMaxPos,GridMinPos,GridMaxDelta,GridDeltaA,GridDeltaB,CenterGV");
	//			for (int i = 1; i <= ��üĭ; ++i)
	//			{
	//				str.Format(_T("%s,%d"), str, i);
	//			}
	//			str.Format(_T("%s\n"), str);
	//			int nTimeOut = 1000;
	//			while (_fnOnWritefile(strFileName, str))
	//			{
	//				nTimeOut -= 10;
	//				if (nTimeOut <= 0) break;
	//				Sleep(10);
	//			}
	//		}
	//		// ����
	//		str.Format(_T("%02d:%02d:%02d,%s,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%f,%d,%d,%f"), time.wHour, time.wMinute, time.wSecond,
	//			arrStrAnalysisData[1],	// Recipe
	//			arrStrAnalysisData[0],	// Cell ID
	//			arrStrAnalysisData[2],	// Pattern Name
	//			unif,
	//			fDiffMm,
	//			fDiffMC,
	//			fDiffCm,
	//			fMaxDiff,
	//			fMnRatio,
	//			fMCRatio,
	//			fCmRatio,
	//			fMaxRatio,
	//			fMean,
	//			fBrightDelta,
	//			fDarkDelta,
	//			fMax,
	//			fMin,
	//			nMaxPos,
	//			nMinPos,
	//			fMaxDiff,
	//			nAPos,
	//			nBPos,
	//			fCenterGV
	//		);
	//
	//		for (int i = 1; i < vecGridMeanGV.size(); ++i)
	//		{
	//			str.Format(_T("%s,%f"), str, vecGridMeanGV[i] * 0.01f);
	//		}
	//		str.Format(_T("%s\n"), str);
	//
	//		int nTimeOut = 1000;
	//		while (_fnOnWritefile(strFileName, str))
	//		{
	//			nTimeOut -= 10;
	//			if (nTimeOut <= 0) break;
	//			Sleep(10);
	//		}
	//
	//	}
	return E_ERROR_CODE_TRUE;
}
//
//long CInspectDC::LogicStart_DC(Mat& matSrcImage, float & fResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
//	int* nCommonPara, CString strAlgPath, CString strResultPath, CString strInspPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner, CString * arrStrAnalysisData, stDefectInfo* pResultBlob, double dResolution)
//{
//	// ���� �ڵ�
//	long	nErrorCode = E_ERROR_CODE_TRUE;
//
//	//////////////////////////////////////////////////////////////////////////
//	// ���� �Ķ����
//	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
//	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
//	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
//	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
//	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
//	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
//	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
//	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
//	long	nAlgoListNum = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
//
//	//////////////////////////////////////////////////////////////////////////
//
//	Rect rectRoi;
//	rectRoi.x = rectROI.left;
//	rectRoi.y = rectROI.top;
//	rectRoi.width = rectROI.Width();
//	rectRoi.height = rectROI.Height();
//
//	Mat mat16U;
//	matSrcImage.convertTo(mat16U, CV_16UC1);
//	mat16U *= 100;
//
//	// Bkg - Edge ��� ���� ���
//	int nROI_Size = (int)dPara[E_PARA_DC_ROI_Size];
//	if (nROI_Size <= 0) return E_ERROR_CODE_DC_WRONG_PARA;
//	float fResizeRatio = 1.f / nROI_Size;
//	Mat matBkResize, MatBkEdge;
//	cv::resize(matBKBuffer, MatBkEdge, Size(), fResizeRatio, fResizeRatio, 0);
//	blur(MatBkEdge, matBkResize, Size(7, 7));
//	cv::threshold(matBkResize, matBkResize, 1, 255, THRESH_BINARY);
//	Mat matBk = matBkResize.clone();
//
//	// �Ҽ��� ��°�ڸ����� �����
//	// blur�� resize�ؾ� ��Ʒ� �Ȼ���(FFT�� ���� �Ŀ� �����ϴ°Ű�, �̰� ������ �����ϴ°�)
//	// �ȼ������� ������ �������� �ϱ� ���� �ٴ����� resize
//	blur(mat16U, mat16U, Size(3, 3));
//	cv::resize(mat16U, mat16U, Size(), fResizeRatio / 2, fResizeRatio / 2, INTER_AREA);
//	blur(mat16U, mat16U, Size(3, 3));
//	cv::resize(mat16U, mat16U, matBkResize.size(), 0, 0, INTER_AREA);
//
//
//	// ����
//	// 1) ��ü���gv�� 127�� �ǰ� offset, ��ü��Ⱑ ��ο��ʿ� ġ�����ִ� ���·� �����ϸ� ����÷ο� ���� ����
//	// 2) ��մ�� ������� ������ n��� ����(��������, ������ �ƴ�)
//	//float avg = mean(mat16U(Rect(mat16U.cols / 2 - 10, mat16U.rows / 2 - 10, 20, 20)))[0];
////  	float avg = mean(mat16U, ~matBkResize)[0];
////  	avg = 12700.f - avg;
////  	mat16U += avg;
//
////  	float fAmpRatio = 1.f;
////  
////  #pragma omp parallel for schedule(dynamic)
////  	for (int j = 0; j < mat16U.rows; ++j)
////  	{
////  		ushort * ptr = mat16U.ptr<ushort>(j);
////  		uchar * ptrBkg = matBkResize.ptr<uchar>(j);
////  		for (int i = 0; i < mat16U.cols; ++i)
////  		{
////  			if (ptrBkg[i] == 255)
////  			{
////  				ptr[i] = 12700;
////  			}
////  			else
////  			{
////  				int gv = (int)ptr[i] + ((int)ptr[i] - 12700) * fAmpRatio;
////  				if (gv > 25500)
////  					gv = 25500;
////  				else if (gv < 1)
////  					gv = 1; // 0 ������ �ɱ�� 1(0.01f)�� ó����
////  				ptr[i] = gv;
////  			}
////  
////  		}
////  	}
//
//
//
//	if (bImageSave)
//	{
//		CString strTemp;
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_AD_%02d_Src_PreProcess.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, mat16U);
//	}
//
//	// 2. �˻� ROI
//	int nPaddingL = (int)dPara[E_PARA_DC_Padding_Left];
//	int nPaddingR = (int)dPara[E_PARA_DC_Padding_Right];
//	int nPaddingT = (int)dPara[E_PARA_DC_Padding_Top];
//	int nPaddingB = (int)dPara[E_PARA_DC_Padding_Bottom];
//
//	rectRoi.x += nPaddingL;
//	rectRoi.y += nPaddingT;
//	rectRoi.width = rectRoi.width - nPaddingL - nPaddingR;
//	rectRoi.height = rectRoi.height - nPaddingT - nPaddingB;
//	rectRoi.x = rectRoi.x * fResizeRatio + 0.5f;
//	rectRoi.y = rectRoi.y * fResizeRatio + 0.5f;
//	rectRoi.width = rectRoi.width * fResizeRatio + 0.5f;
//	rectRoi.height = rectRoi.height * fResizeRatio + 0.5f;
//
//	Mat matROI = mat16U(rectRoi);
//	Mat matBkROI = matBkResize(rectRoi);
//
//	// Bkg - edge�����ſ��ٰ�, �����¿� �е��� ��ħ
//	Mat matTemp = Mat::zeros(MatBkEdge.size(), CV_8UC1);
//	rectangle(matTemp, rectRoi, 255, CV_FILLED);
//	matBkResize += ~matTemp;
//	cv::bitwise_xor(MatBkEdge, matBkResize, MatBkEdge);
//
//
//	// 3. �����ؼ� ���� 15%, ���� 15%�� ���� ��ձ��ϱ�
//	int nMin = (int)dPara[E_PARA_DC_HistoGram_MinValue];
//	int nMax = (int)dPara[E_PARA_DC_HistoGram_MaxValue];
//	float fBrightRatio = (float)dPara[E_PARA_DC_Bright_Ratio];
//	float fDarkRatio = (float)dPara[E_PARA_DC_Dark_Ratio];
//	vector<pair<ushort, CPoint>> vecValues;
//	for (int j = 0; j < matROI.rows; ++j)
//	{
//		ushort * ptr = matROI.ptr<ushort>(j);
//		uchar * ptr2 = matBkROI.ptr<uchar>(j);
//		for (int i = 0; i < matROI.cols; ++i)
//		{
//			if (ptr[i] >= nMin * 100)
//				if (ptr[i] <= nMax * 100)
//					if (ptr2[i] == 0)
//						vecValues.push_back(make_pair(ptr[i], CPoint(i, j)));
//		}
//	}
//	if (vecValues.size() <= 0)
//		return E_ERROR_CODE_DC_ROI_EMPTY;
//
//	sort(vecValues.begin(), vecValues.end(), _Cmp);
//
//	// 3-1. ��� ��� �� �м��̹��� ����
//	Mat matAnalysis;
//	cv::resize(matSrcImage, matAnalysis, Size(), fResizeRatio, fResizeRatio, INTER_AREA);
//	medianBlur(matAnalysis, matAnalysis, 3);
//	cv::cvtColor(matAnalysis, matAnalysis, CV_GRAY2BGR);
//
//	float fDarkDelta, fBrightDelta, fMean;
//
//	// ROI �� ��ü���
//	double sum = 0.f;
//	int cnt = 0;
//	for (int i = 0; i < vecValues.size() - 1; ++i)
//	{
//		sum += vecValues[i].first;
//		cnt++;
//	}
//	fMean = (cnt > 0) ? sum / cnt / 100.f : 0;
//
//
//	// ������
//	sum = 0.f;
//	cnt = 0;
//	if (0 < vecValues.size()*fBrightRatio)
//	{
//		for (int i = 0; i < (int)(vecValues.size()*fBrightRatio); ++i)
//		{
//			sum += vecValues[i].first;
//			cnt++;
//		}
//		fBrightDelta = (cnt > 0) ? sum / cnt / 100.f : 0;
//	}
//	else
//	{
//		fBrightDelta = 0;
//	}
//
//	// ��ο���
//	sum = 0.f;
//	cnt = 0;
//	if (vecValues.size() - (int)(vecValues.size()*fDarkRatio > vecValues.size() - 1))
//	{
//		for (int i = vecValues.size() - (int)(vecValues.size()*fDarkRatio); i < vecValues.size() - 1; ++i)
//		{
//			sum += vecValues[i].first;
//			cnt++;
//		}
//		fDarkDelta = (cnt > 0) ? sum / cnt / 100.f : 0;
//	}
//	else
//	{
//		fDarkDelta = 0;
//	}
//	Mat matBright, matDark;
//
//	cv::threshold(mat16U, matBright, fBrightDelta * 100, 255, THRESH_TOZERO);
//	cv::threshold(mat16U, matDark, fDarkDelta * 100, 255, THRESH_TOZERO_INV);
//	matBright.convertTo(matBright, CV_8UC1);
//	matDark.convertTo(matDark, CV_8UC1);
//	matBright -= matBkResize;
//	matDark -= matBkResize;
//
//#pragma omp parallel for schedule(dynamic)
//	for (int j = 0; j < matAnalysis.rows; ++j)
//	{
//		Vec3b * ptr = (Vec3b *)matAnalysis.ptr<Vec3b>(j);
//		uchar * ptr2 = (uchar *)matBright.ptr<uchar>(j);
//		uchar * ptr3 = (uchar *)matDark.ptr<uchar>(j);
//		uchar * ptrBkg = (uchar *)MatBkEdge.ptr<uchar>(j);
//		for (int i = 0; i < matAnalysis.cols; ++i)
//		{
//			if (ptrBkg[i] == 255)
//				ptr[i] = Vec3b(0, 150, 0);
//			else if (ptr2[i] == 255)
//				ptr[i] = Vec3b(0, 0, 255);
//			else if (ptr3[i] == 255)
//				ptr[i] = Vec3b(255, 0, 0);
//		}
//	}
//
//	float unif = (fMean > 0) ? (fBrightDelta - fDarkDelta) / fMean : 0.0001f;
//
//	// �ӽñ��
//	// �İ��� �̹����� ������ϱ� ���� ��ֶ����� �̹��� ����
//	float r = (float)dPara[E_PARA_DC_SAVE_AMPIMAGE];
//	if (r >= 1.f && r <= 2.f)
//	{
//		//float avg = mean(mat16U(Rect(mat16U.cols / 2 - 10, mat16U.rows / 2 - 10, 20, 20)))[0];
//		float avg = mean(mat16U, ~matBkResize)[0];
//		avg = 12700.f - avg;
//		Mat matNormalize = mat16U + avg;
//
//		float fAmpRatio = 1.f;
//
//#pragma omp parallel for schedule(dynamic)
//		for (int j = 0; j < matNormalize.rows; ++j)
//		{
//			ushort * ptr = matNormalize.ptr<ushort>(j);
//			uchar * ptrBkg = matBkResize.ptr<uchar>(j);
//			for (int i = 0; i < matNormalize.cols; ++i)
//			{
//				if (ptrBkg[i] == 255)
//				{
//					ptr[i] = 12700;
//				}
//				else
//				{
//					int gv = (int)ptr[i] + ((int)ptr[i] - 12700) * fAmpRatio;
//					if (gv > 25500)
//						gv = 25500;
//					else if (gv < 1)
//						gv = 1; // 0 ������ �ɱ�� 1(0.01f)�� ó����
//					ptr[i] = gv;
//				}
//
//			}
//		}
//		int nMin2 = fDarkDelta;
//		int nMax2 = fBrightDelta;
//		matNormalize = matNormalize * 0.01f;
//		matNormalize.convertTo(matNormalize, CV_8UC1);
//
//		cv::resize(matNormalize, matNormalize, Size(), 0.5f, 0.5f);
//		blur(matNormalize, matNormalize, Size(15, 15));
//		medianBlur(matNormalize, matNormalize, 3);
//		cv::resize(matNormalize, matNormalize, mat16U.size());
//
//
//		// #pragma omp parallel for schedule(dynamic)
//		// 		for (int j = 0; j < matNormalize.rows; ++j)
//		// 		{
//		// 			uchar * ptrGray = matNormalize.ptr<uchar>(j);
//		// 			for (int i = 0; i < matNormalize.cols; ++i)
//		// 			{
//		// 				if (ptrGray[i] < nMin2) ptrGray[i] = nMin2;
//		// 				else if (ptrGray[i] > nMax2) ptrGray[i] = nMax2;
//		// 			}
//		// 		}
//		cv::normalize(matNormalize, matNormalize, 0, 255, NORM_MINMAX);
//
//#pragma omp parallel for schedule(dynamic)
//		for (int j = 0; j < matNormalize.rows; ++j)
//		{
//			uchar * ptrGray = matNormalize.ptr<uchar>(j);
//			uchar * ptrBkg = matBk.ptr<uchar>(j);
//			for (int i = 0; i < matNormalize.cols; ++i)
//			{
//				if (ptrBkg[i] != 0) ptrGray[i] = 127;
//			}
//		}
//
//		rectRoi.x = rectROI.left * fResizeRatio + 0.5f;
//		rectRoi.y = rectROI.top * fResizeRatio + 0.5f;
//		rectRoi.width = rectROI.Width() * fResizeRatio + 0.5f;
//		rectRoi.height = rectROI.Height() * fResizeRatio + 0.5f;
//		matNormalize = matNormalize(rectRoi);
//		cv::rotate(matNormalize, matNormalize, 0);
//
//		cv::resize(matNormalize, matNormalize, Size((int)(500.f*((float)matNormalize.cols / (float)matNormalize.rows) + 0.5f), 500), INTER_AREA);
//	
//		if (bImageSave)
//		{
//			CString strTemp;
//			strTemp.Format(_T("%s\\%02d_%02d_%02d_AD_%02d_Normalize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//			cv::imwrite((cv::String)(CStringA)strTemp, matNormalize);
//		}
///*
//		CString str = _T("D:\\DC");
//		CreateDirectory(str, NULL);
//		str.Format(_T("%s\\%s_%s.jpg"), str, arrStrAnalysisData[0], arrStrAnalysisData[2]);
//		cv::imwrite((cv::String)(CStringA)str, matNormalize);*/
//	}
//
//
//
//	float xĭ = dPara[E_PARA_DC_VIEW_GRID_X];
//	float yĭ = dPara[E_PARA_DC_VIEW_GRID_Y];
//	if (xĭ <= 0 || xĭ > 20) xĭ = 5;
//	if (yĭ <= 0 || yĭ > 20) yĭ = 4;
//	int ��üĭ = xĭ * yĭ;
//
//
//
//	// ���Ȱ˻� �������� �� ���� ���ǥ��
//// 	Rect rectRoi3;
//// 	rectRoi3.x = rectROI.left;
//// 	rectRoi3.y = rectROI.top;
//// 	rectRoi3.width = rectROI.Width();
//// 	rectRoi3.height = rectROI.Height();
//// 	rectRoi3.x += nPaddingL;
//// 	rectRoi3.y += nPaddingT;
//// 	rectRoi3.width = rectRoi3.width - nPaddingL - nPaddingR;
//// 	rectRoi3.height = rectRoi3.height - nPaddingT - nPaddingB;
//	Rect rectRoi3 = rectRoi;
//
//
//	Mat matGrid = Mat::zeros(Size(xĭ, yĭ), CV_32FC1);
//	vector<float> vecGridMeanGV(��üĭ + 1, 0.f);
//	Mat matRoi3 = mat16U(rectRoi3);
//	Mat matRoiBkg3;
//	matRoiBkg3 = ~matBkResize(rectRoi3);
//	// 	resize(matBkResize, matRoiBkg3, matBKBuffer.size(), INTER_LINEAR);
//	// 	matRoiBkg3 = ~matRoiBkg3(rectRoi3);
//	int xũ�� = matRoi3.cols / xĭ;
//	int yũ�� = matRoi3.rows / yĭ;;
//
//
//	Point ptMin, ptMax;
//	float fMax = 0.f;
//	float fMin = 99999.f;
//	for (int j = 0; j < yĭ; ++j)
//	{
//		for (int i = 0; i < xĭ; ++i)
//		{
//			Rect rectMeanRoi(i*xũ��, j*yũ��, xũ��, yũ��);
//			if (rectMeanRoi.x + rectMeanRoi.width >= matRoi3.cols) rectMeanRoi.width = matRoi3.cols - 1 - rectMeanRoi.x;
//			if (rectMeanRoi.y + rectMeanRoi.height >= matRoi3.rows) rectMeanRoi.height = matRoi3.rows - 1 - rectMeanRoi.y;
//			Scalar m = mean(matRoi3(rectMeanRoi), matRoiBkg3(rectMeanRoi));
//			matGrid.at<float>(j, i) = m[0];
//			vecGridMeanGV[((int)(xĭ - (i + 1)) * yĭ + (j + 1))] = m[0];
//
//			if (m[0] > fMax)
//			{
//				fMax = m[0];
//				ptMax = Point(i + 1, j + 1);
//			}
//			if (m[0] < fMin)
//			{
//				fMin = m[0];
//				ptMin = Point(i + 1, j + 1);
//			}
//
//		}
//	}
//	Rect rectRoi2;
//	rectRoi2.x = rectRoi3.x;
//	rectRoi2.y = rectRoi3.y;
//	rectRoi2.width = rectRoi3.width;
//	rectRoi2.height = rectRoi3.height;
//
//
//	int centerW = 20;
//	int centerH = 20;
//	Rect rectCenter = Rect(matRoi3.cols / 2 - centerW / 2, matRoi3.rows / 2 - centerH / 2, centerW, centerH);
//	Scalar m = mean(matRoi3(rectCenter));
//	float fCenterGV = m[0];
//
//
//	int nMinPos = (xĭ - ptMin.x) * yĭ + ptMin.y;
//	int nMaxPos = (xĭ - ptMax.x) * yĭ + ptMax.y;
//
//	for (int i = 0; i <= xĭ; ++i)
//		line(matAnalysis, Point(rectRoi2.x + (rectRoi2.width * (float)i / (float)(xĭ)), rectRoi2.y), Point(rectRoi2.x + (rectRoi2.width * (float)i / (float)(xĭ)), rectRoi2.y + rectRoi2.height), Scalar(0, 0, 0));
//	for (int j = 0; j <= yĭ; ++j)
//		line(matAnalysis, Point(rectRoi2.x, rectRoi2.y + (rectRoi2.height * (float)j / (float)(yĭ))), Point(rectRoi2.x + rectRoi2.width, rectRoi2.y + (rectRoi2.height * (float)j / (float)(yĭ))), Scalar(0, 0, 0));
//
//
//	// �������� �� �����
//	float fDiff;
//	float fMaxDiff = 0;
//	float fMaxRatio;
//	Point ptA;
//	Point ptB;
//	// X����
//	for (int j = 0; j < matGrid.rows; ++j)
//	{
//		float * ptrGrid = matGrid.ptr<float>(j);
//		for (int i = 0; i < matGrid.cols - 1; ++i)
//		{
//			fDiff = abs(ptrGrid[i] - ptrGrid[i + 1]);
//			if (fDiff > fMaxDiff)
//			{
//				fMaxDiff = fDiff;
//				if (ptrGrid[i + 1] != 0)
//					fMaxRatio = ptrGrid[i] / (float)ptrGrid[i + 1];
//				else
//					fMaxRatio = ptrGrid[i] / 1.f;
//				if (fMaxRatio < 1.f) fMaxRatio = 1.f / fMaxRatio;
//
//				ptA = Point(i + 1, j + 1);
//				ptB = Point(i + 2, j + 1);
//			}
//		}
//	}
//	// Y����
//	for (int j = 0; j < matGrid.rows - 1; ++j)
//	{
//		float * ptrGrid = matGrid.ptr<float>(j);
//		float * ptrGrid2 = matGrid.ptr<float>(j + 1);
//		for (int i = 0; i < matGrid.cols; ++i)
//		{
//			fDiff = abs(ptrGrid[i] - ptrGrid2[i]);
//			if (fDiff > fMaxDiff)
//			{
//				fMaxDiff = fDiff;
//				ptA = Point(i + 1, j + 1);
//				ptB = Point(i + 1, j + 2);
//			}
//		}
//	}
//	int nAPos = (xĭ - ptA.x) * yĭ + ptA.y;
//	int nBPos = (xĭ - ptB.x) * yĭ + ptB.y;
//
//
//
//
//	rectCenter.x += rectRoi3.x;
//	rectCenter.y += rectRoi3.y;
//	rectCenter.x *= fResizeRatio;
//	rectCenter.y *= fResizeRatio;
//	rectCenter.width *= fResizeRatio;
//	rectCenter.height *= fResizeRatio;
//	//rectangle(matAnalysis, rectCenter, Scalar(0, 255, 0), 1);
//
//	float xx = rectRoi2.width / (float)xĭ;
//	float yy = rectRoi2.height / (float)yĭ;
//	circle(matAnalysis,
//		Point(rectRoi2.x + (float)(ptMax.x - 1) * xx + xx*0.5, rectRoi2.y + ((float)(ptMax.y - 1) * yy) + yy*0.5),
//		3, Scalar(255, 255, 255), CV_FILLED, LINE_AA);
//	circle(matAnalysis,
//		Point(rectRoi2.x + (float)(ptMin.x - 1) * xx + xx*0.5, rectRoi2.y + ((float)(ptMin.y - 1) * yy) + yy*0.5),
//		3, Scalar(255, 255, 255), CV_FILLED, LINE_AA);
//
//	circle(matAnalysis,
//		Point(rectRoi2.x + (float)(ptA.x - 1) * xx + xx*0.5, rectRoi2.y + ((float)(ptA.y - 1) * yy) + yy*0.5),
//		3, Scalar(150, 150, 150), CV_FILLED, LINE_AA);
//
//	circle(matAnalysis,
//		Point(rectRoi2.x + (float)(ptB.x - 1) * xx + xx*0.5, rectRoi2.y + ((float)(ptB.y - 1) * yy) + yy*0.5),
//		3, Scalar(150, 150, 150), CV_FILLED, LINE_AA);
//	line(matAnalysis, Point(rectRoi2.x + (float)(ptA.x - 1) * xx + xx*0.5, rectRoi2.y + ((float)(ptA.y - 1) * yy) + yy*0.5),
//		Point(rectRoi2.x + (float)(ptB.x - 1) * xx + xx*0.5, rectRoi2.y + ((float)(ptB.y - 1) * yy) + yy*0.5),
//		Scalar(150, 150, 150), 1, LINE_AA);
//
//
//	// Feature
//	fMax *= 0.01f;
//	fMin *= 0.01f;
//	fCenterGV *= 0.01f;
//	fMaxDiff *= 0.01;
//
//
//	//float unif = (fBrightDelta - fDarkDelta) / fMean; ���� ����
//	float fDiffMm = fMax - fMin;
//	float fDiffMC = fMax - fCenterGV;
//	float fDiffCm = fCenterGV - fMin;
//	//float fMaxDiff; ��������
//	float fMnRatio = (fMin > 0.f) ? fMax / fMin : 1.f;
//	float fMCRatio = (fCenterGV > 0.f) ? fMax / fCenterGV : 1.f;
//	float fCmRatio = (fMin > 0.f) ? fCenterGV / fMin : 1.f;
//	//float fMaxRatio; ��������
//
//	float arrFeature[9];
//	arrFeature[0] = unif;
//	arrFeature[1] = fDiffMm;
//	arrFeature[2] = fDiffMC;
//	arrFeature[3] = fDiffCm;
//	arrFeature[4] = fMaxDiff;
//	arrFeature[5] = fMnRatio;
//	arrFeature[6] = fMCRatio;
//	arrFeature[7] = fCmRatio;
//	arrFeature[8] = fMaxRatio;
//
//	//if (pResultBlob != NULL)
//	//{
//	//	pResultBlob->unif[0] = unif;
//	//	pResultBlob->fDiffMm[0] = fDiffMm;
//	//	pResultBlob->fDiffMC[0] = fDiffMC;
//	//	pResultBlob->fDiffCm[0] = fDiffCm;
//	//	pResultBlob->fMaxDiff[0] = fMaxDiff;
//	//	pResultBlob->fMnRatio[0] = fMnRatio;
//	//	pResultBlob->fMCRatio[0] = fMCRatio;
//	//	pResultBlob->fCmRatio[0] = fCmRatio;
//	//	pResultBlob->fMaxRatio[0] = fMaxRatio;
//	//}
//
//	CFeatureExtraction cFeatureExtraction;
//	cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_DC, m_tInitTime, m_tBeforeTime, m_strAlgLog);
//	cFeatureExtraction.DoDefectBlobJudgment_DC(arrFeature, nCommonPara, EngineerDefectJudgment, pResultBlob, nAlgoListNum);
//
//	fResult = unif;
//	
//	if (pResultBlob->nDefectCount>0)
//	{
//
//		// 4.result������ ������� ����
//		CString str;
//		str.Format(_T("%s\\DC_unif_%.6f.png"), strResultPath, unif);
//		cv::imwrite((cv::String)(CStringA)str, matAnalysis);
//
//
//		// 5. �м��� ������
//		//////////////////////////
//		CString strAnalysisDataPath = strInspPath;
//		CreateDirectory(strAnalysisDataPath, NULL);
//
//		SYSTEMTIME time;
//		::GetLocalTime(&time);
//
//		// ���
//		CString strFileName;
//		strFileName.Format(L"%s\\%02d%02d%02d_DC.csv", strAnalysisDataPath, time.wYear, time.wMonth, time.wDay);
//		if (!PathFileExists(strFileName))
//		{
//			CString str = _T("Time,Recipe,ID,Pattern,Unif,M-m,M-C,C-m,MaxDelta,M/m,M/C,C/m,MaxRatio,Avg,BrightGV,DarkGV,GridMaxGV,GridMinGV,GridMaxPos,GridMinPos,GridMaxDelta,GridDeltaA,GridDeltaB,CenterGV");
//			for (int i = 1; i <= ��üĭ; ++i)
//			{
//				str.Format(_T("%s,%d"), str, i);
//			}
//			str.Format(_T("%s\n"), str);
//			int nTimeOut = 1000;
//			while (_fnOnWritefile(strFileName, str))
//			{
//				nTimeOut -= 10;
//				if (nTimeOut <= 0) break;
//				Sleep(10);
//			}
//		}
//		// ����
//		str.Format(_T("%02d:%02d:%02d,%s,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%f,%d,%d,%f"), time.wHour, time.wMinute, time.wSecond,
//			arrStrAnalysisData[1],	// Recipe
//			arrStrAnalysisData[0],	// Cell ID
//			arrStrAnalysisData[2],	// Pattern Name
//			unif,
//			fDiffMm,
//			fDiffMC,
//			fDiffCm,
//			fMaxDiff,
//			fMnRatio,
//			fMCRatio,
//			fCmRatio,
//			fMaxRatio,
//			fMean,
//			fBrightDelta,
//			fDarkDelta,
//			fMax,
//			fMin,
//			nMaxPos,
//			nMinPos,
//			fMaxDiff,
//			nAPos,
//			nBPos,
//			fCenterGV
//		);
//
//		for (int i = 1; i < vecGridMeanGV.size(); ++i)
//		{
//			str.Format(_T("%s,%f"), str, vecGridMeanGV[i] * 0.01f);
//		}
//		str.Format(_T("%s\n"), str);
//
//		int nTimeOut = 1000;
//		while (_fnOnWritefile(strFileName, str))
//		{
//			nTimeOut -= 10;
//			if (nTimeOut <= 0) break;
//			Sleep(10);
//		}
//
//	}
//	// 5. ���� ������ ���� �ҷ�����
//	// 	float fStrongDelta = (float)dPara[E_PARA_DC_Strong_Delta];
//	// 	float fWeakDelta = (float)dPara[E_PARA_DC_Weak_Delta];
//	// 
//	// 	if (fWeakDelta > 0)
//	// 	{
//	// 		if (unif >= fWeakDelta)
//	// 		{
//	// 			
//	// 			nResult = DI[DEFECT_JUDGEMENT_DC_WEAK];
//	// 		}
//	// 	}
//	// 	if (fStrongDelta > 0)
//	// 	{
//	// 		if (unif >= fStrongDelta)
//	// 		{
//	// 			nResult = DI[DEFECT_JUDGEMENT_DC];
//	// 		}
//	// 	}
//
//	return E_ERROR_CODE_TRUE;
//}
//
//long CInspectDC::LogicStart_LeakLight(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution)
//{
//	// ���� �ڵ�
//	long	nErrorCode = E_ERROR_CODE_TRUE;
//
//	//////////////////////////////////////////////////////////////////////////
//	// ���� �Ķ����
//	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
//	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
//	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
//	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
//	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
//	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
//	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
//	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
//
//	//////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//	try
//	{
//		int nText = (int)dPara[E_PARA_LeakLight_TEXT];
//		int nThreshold = (int)dPara[E_PARA_LeakLight_Threshold];
//		int nResize = (int)dPara[E_PARA_LeakLight_Resize];
//		int nDilate = (int)dPara[E_PARA_LeakLight_MORP_Dilate];
//		int nErode = (int)dPara[E_PARA_LeakLight_Morp_Erode];
//
//		// ������ �˰��� ������ �浹����
//		if (nText == 5) return E_ERROR_CODE_LEAK_LIGHT_WRONG_PARAM;
//
//
//		float fResize = 1 / (float)nResize;
//		nDilate *= fResize;
//		nErode *= fResize;
//
//		if (nDilate % 2 == 0) nDilate++;
//		if (nErode % 2 == 0) nErode++;
//
//
//
//		Rect rectRoi;
//		rectRoi.x = rectROI.left;
//		rectRoi.y = rectROI.top;
//		rectRoi.width = rectROI.Width();
//		rectRoi.height = rectROI.Height();
//
//		// ����ȭ
//		Mat matRoi = matSrcImage(rectRoi);
//		Mat matBin;
//		threshold(matRoi, matBin, nThreshold, 255, THRESH_BINARY);
//
//		if (bImageSave)
//		{
//			CString strTemp;
//			strTemp.Format(_T("%s\\%02d_%02d_%02d_AD_%02d_Threshold.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//			cv::imwrite((cv::String)(CStringA)strTemp, matBin);
//		}
//
//		// ��������
//		// ������ ����ϱ� ������ ������ Resize�ϸ� �� ������
//		// ����ȭ�ϰ� ���� Resize�ؾ���
//		Mat matBin2;
//		resize(matBin, matBin2, Size(), fResize, fResize, INTER_AREA);
//		threshold(matBin2, matBin2, 0, 255, THRESH_BINARY);
//
//		if (bImageSave)
//		{
//			CString strTemp;
//			strTemp.Format(_T("%s\\%02d_%02d_%02d_AD_%02d_Resize.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//			cv::imwrite((cv::String)(CStringA)strTemp, matBin2);
//		}
//
//		// Dilate
//		Mat matBlur;
//		blur(matBin2, matBlur, Size(nDilate, nDilate));
//		threshold(matBlur, matBlur, 0, 255, THRESH_BINARY);
//
//		if (bImageSave)
//		{
//			CString strTemp;
//			strTemp.Format(_T("%s\\%02d_%02d_%02d_AD_%02d_Dilate.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//			cv::imwrite((cv::String)(CStringA)strTemp, matBlur);
//		}
//
//		// Erode & upscale
//		// ���� resize�ϸ� �� �������� ������
//		// Blob�Ҷ� Feature�����Ϸ��� ����ũ��� Upscale�ؾ���
//		blur(matBlur, matBlur, Size(nErode, nErode));
//		resize(matBlur, matBlur, matRoi.size());
//		Mat matDst = Mat::zeros(matSrcImage.size(), CV_8UC1);
//		threshold(matBlur, matDst(rectRoi), 254, 255, THRESH_BINARY);
//
//		if (bImageSave)
//		{
//			CString strTemp;
//			strTemp.Format(_T("%s\\%02d_%02d_%02d_AD_%02d_Erode.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//			cv::imwrite((cv::String)(CStringA)strTemp, matDst(rectRoi));
//		}
//
//		Rect rectRoi2;
//		rectRoi2.x = 0;
//		rectRoi2.y = 0;
//		rectRoi2.width = matDst.cols;
//		rectRoi2.height = matDst.rows;
//
//
//
//		// 4.result������ ������� ����
//		Mat matDraw;
//		resize(matDst, matDraw, Size(), 0.25, 0.25, INTER_AREA);
//		cvtColor(matDraw, matDraw, CV_GRAY2BGR);
//
//		Mat matEdge;
//		resize(matBKBuffer, matEdge, matDraw.size(), 0, 0, INTER_AREA);
//		Canny(matEdge, matEdge, 30, 100);
//
//		for (int j = 0; j < matEdge.rows; ++j)
//		{
//			uchar * ptr1 = matEdge.ptr<uchar>(j);
//			Vec3b * ptr2 = matDraw.ptr<Vec3b>(j);
//			for (int i = 0; i < matEdge.cols; ++i)
//			{
//				if (ptr1[i] != 0)
//				{
//					ptr2[i] = Vec3b(0, 255, 0);
//				}
//			}
//		}
//
//		CString str;
//		str.Format(_T("%s\\%s_LeakLight.png"), strResultPath, arrStrAnalysisData[2]);
//		cv::imwrite((cv::String)(CStringA)str, matDst);
//
//
//
//		// Blbo
//		CFeatureExtraction cFeatureExtraction;
//		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_DC, m_tInitTime, m_tBeforeTime, m_strAlgLog);
//		cFeatureExtraction.DoDefectBlobJudgment_ROI(matSrcImage, matDst, rectRoi2, 1, nCommonPara, EngineerDefectJudgment, pResultBlob, dResolution);
//
//
//
//	}
//	catch (...)
//	{
//	}
//
//
//	return E_ERROR_CODE_TRUE;
//}
//
//
//long CInspectDC::LogicStart_ETC_MURA(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution)
//{
//	// ���� �ڵ�
//	long	nErrorCode = E_ERROR_CODE_TRUE;
//
//	//////////////////////////////////////////////////////////////////////////
//	// ���� �Ķ����
//	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
//	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
//	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
//	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
//	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
//	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
//	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
//	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
//
//	//////////////////////////////////////////////////////////////////////////
//
//
//	if (bImageSave)
//	{
//		CString strTemp;
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Origin.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matSrcImage);
//	}
//
//
//
//	Rect rectRoi;
//	rectRoi.x = rectROI.left;
//	rectRoi.y = rectROI.top;
//	rectRoi.width = rectROI.Width();
//	rectRoi.height = rectROI.Height();
//
//	Mat mat16U;
//	matSrcImage.convertTo(mat16U, CV_16UC1);
//	mat16U *= 100;
//
//	// Bkg - Edge ��� ���� ���
//	int nResize = (int)dPara[E_PARA_ETC_MURA_RESIZE];
//	if (nResize <= 0) return E_ERROR_CODE_DC_WRONG_PARA;
//	float fResizeRatio = 1.f / nResize;
//	Mat matBkResize;
//	resize(matBKBuffer, matBkResize, Size(), fResizeRatio, fResizeRatio, 0);
//
//	// �Ҽ��� ��°�ڸ����� �����
//	// blur�� resize�ؾ� ��Ʒ� �Ȼ���(FFT�� ���� �Ŀ� �����ϴ°Ű�, �̰� ������ �����ϴ°�)
//	// �ȼ������� ������ �������� �ϱ� ���� �ٴ����� resize
//	blur(mat16U, mat16U, Size(3, 3));
//	resize(mat16U, mat16U, Size(), fResizeRatio / 2, fResizeRatio / 2, INTER_AREA);
//	blur(mat16U, mat16U, Size(3, 3));
//	resize(mat16U, mat16U, matBkResize.size(), 0, 0, INTER_AREA);
//
//	rectRoi.x *= fResizeRatio;
//	rectRoi.y *= fResizeRatio;
//	rectRoi.width *= fResizeRatio;
//	rectRoi.height *= fResizeRatio;
//
//	if (bImageSave)
//	{
//		CString strTemp;
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Ori_Resize.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, mat16U);
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Bkg_Resize.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matBkResize);
//	}
//
//	// ����
//	// 1) ��ü���gv�� 127�� �ǰ� offset, ��ü��Ⱑ ��ο��ʿ� ġ�����ִ� ���·� �����ϸ� ����÷ο� ���� ����
//	// 2) ��մ�� ������� ������ n��� ����(��������, ������ �ƴ�)
//	//float avg = mean(mat16U(Rect(mat16U.cols / 2 - 10, mat16U.rows / 2 - 10, 20, 20)))[0];
//	float avg = mean(mat16U, ~matBkResize)[0];
//	avg = 12700.f - avg;
//	mat16U += avg;
//
//	float fAmpRatio = (float)dPara[E_PARA_ETC_MURA_AMP];
//
//#pragma omp parallel for schedule(dynamic)
//	for (int j = 0; j < mat16U.rows; ++j)
//	{
//		ushort * ptr = mat16U.ptr<ushort>(j);
//		for (int i = 0; i < mat16U.cols; ++i)
//		{
//				int gv = (int)ptr[i] + ((int)ptr[i] - 12700) * fAmpRatio;
//				if (gv > 25500)
//					gv = 25500;
//				else if (gv < 1)
//					gv = 1; // 0 ������ �ɱ�� 1(0.01f)�� ó����
//				ptr[i] = gv;
//			
//		}
//	}
//
//	if (bImageSave)
//	{
//		CString strTemp;
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Amp.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, mat16U);
//	}
//
//
//	// ��������ȭ
//	// ���� ���ο� ���� 8���� �񱳵� �����ؾ��� �� ����
//	int nLocalSize = (int)dPara[E_PARA_ETC_MURA_THRESH_LOCAL_SIZE];
//	if (nLocalSize % 2 == 0) nLocalSize++;
//	Mat matMean;
//	blur(mat16U, matMean, Size(nLocalSize, nLocalSize));
//	blur(matMean, matMean, Size(nLocalSize, nLocalSize));
//
//	Mat matThB, matThD;
//
//	matThB = mat16U - matMean;
//	matThD = matMean - mat16U;
//
//	if (bImageSave)
//	{
//		CString strTemp;
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Intensity_Bright.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matThB);
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Intensity_Dark.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matThD);
//	}
//
//	int nThB = (int)(dPara[E_PARA_ETC_MURA_THRESH_B] * 100);
//	int nThD = (int)(dPara[E_PARA_ETC_MURA_THRESH_D] * 100);
// 	threshold(matThB, matThB, nThB, 255, THRESH_BINARY);
//	threshold(matThD, matThD, nThD, 255, THRESH_BINARY);
//
//
//	// Edge�� ���� �β� ����ŷ
//	// Edge���� �˻���Ұ���. ����ȭ�Ҷ� Blur����� ���� �� ���� ���ؾ���
//	int nDeleteEdgeSize = (int)dPara[E_PARA_ETC_MURA_EDGE_DELETE_THICK];
//	nDeleteEdgeSize /= nResize;
//	if (nDeleteEdgeSize % 2 == 0) nDeleteEdgeSize++;
//
//	Mat matEdgeMask = matBkResize;
//	blur(matBkResize, matEdgeMask, Size(nDeleteEdgeSize, nDeleteEdgeSize));
//	threshold(matEdgeMask, matEdgeMask, 1, 255, THRESH_BINARY_INV);
//
//	matThB.convertTo(matThB, CV_8UC1);
//	matThD.convertTo(matThD, CV_8UC1);
//
//	bitwise_and(matThB, matEdgeMask, matThB);
//	bitwise_and(matThD, matEdgeMask, matThD);
//
//	if (bImageSave)
//	{
//		CString strTemp;
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Threshold_Bright.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matThB);
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Threshold_Dark.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matThD);
//	}
//
//
//	// �������� Close
//	int nCloseSize = (int)dPara[E_PARA_ETC_MURA_MORP_CLOSE];
//	if (nCloseSize % 2 == 0) nCloseSize++;
//
//	blur(matThB, matThB, Size(nCloseSize, nCloseSize));
//	threshold(matThB, matThB, 1, 255, THRESH_BINARY);
//	blur(matThB, matThB, Size(nCloseSize, nCloseSize));
//	threshold(matThB, matThB, 254, 255, THRESH_BINARY);
//
//
//	blur(matThD, matThD, Size(nCloseSize, nCloseSize));
//	threshold(matThD, matThD, 1, 255, THRESH_BINARY);
//	blur(matThD, matThD, Size(nCloseSize, nCloseSize));
//	threshold(matThD, matThD, 254, 255, THRESH_BINARY);
//
//	if (bImageSave)
//	{
//		CString strTemp;
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Morp_Bright.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matThB);
//		strTemp.Format(_T("%s\\%02d_%02d_%02d_%02d_Morp_Dark.png"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
//		cv::imwrite((cv::String)(CStringA)strTemp, matThD);
//	}
//
//
//	// Blob�Ϸ��� �ٽ� 8��Ʈ�� ��ȯ�ؾ���
//	Mat mat8U;
//	mat16U *= 0.01f;
//	mat16U.convertTo(mat8U, CV_8UC1);
//
//	rectRoi.x = 0;
//	rectRoi.y = 0;
//	CFeatureExtraction cFeatureExtraction;
//	cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_DC, m_tInitTime, m_tBeforeTime, m_strAlgLog);
//	cFeatureExtraction.DoDefectBlobJudgment_ROI(mat8U, matThB, rectRoi, nResize, nCommonPara, EngineerDefectJudgment, pResultBlob, dResolution);
//	cFeatureExtraction.DoDefectBlobJudgment_ROI(mat8U, matThD, rectRoi, nResize, nCommonPara, EngineerDefectJudgment, pResultBlob, dResolution);
//
//	return E_ERROR_CODE_TRUE;
//}
//
//
//long CInspectDC::LogicStart_CornerBurn(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution)
//{
//	int nEdgeRoiSize = (int)dPara[E_PARA_CornerBurn_RoiWidth]; // 500;
//	int Delta = (int)dPara[E_PARA_CornerBurn_Delta]; // 1;
//	float fRatioCut = (float)dPara[E_PARA_CornerBurn_EdgeContactRatio]; //0.5;
//
//	int nResize = 16;
//	int nResize2 = 4;
//
//	Rect rectRoi = Rect(rectROI.left + rectROI.Width() - nEdgeRoiSize + 100, rectROI.top,
//		nEdgeRoiSize, rectROI.Height());
//	if (rectRoi.x < 0) rectRoi.x = 0;
//	if (rectRoi.y < 0) rectRoi.y = 0;
//	if (rectRoi.x + rectRoi.width >= matSrcImage.cols) rectRoi.x = matSrcImage.cols - 1 - rectRoi.width;
//	if (rectRoi.y + rectRoi.height >= matSrcImage.rows) rectRoi.y = matSrcImage.rows - 1 - rectRoi.height;
//
//	Mat matSrc = matSrcImage(rectRoi);
//	Mat matBkg = matBKBuffer(rectRoi);
//
//
//	resize(matSrc, matSrc, Size(), 1 / (float)nResize2, 1 / (float)nResize2, INTER_AREA);
//	blur(matSrc, matSrc, Size(3, 3));
//	resize(matSrc, matSrc, Size(), 1 / (float)nResize2, 1 / (float)nResize2, INTER_AREA);
//	blur(matSrc, matSrc, Size(3, 3));
//
//	resize(matBkg, matBkg, matSrc.size(), 0, 0, INTER_NEAREST);
//
//	Mat matEdge;
//	blur(matBkg, matEdge, Size(5, 5));
//	threshold(matEdge, matEdge, 1, 255, THRESH_BINARY);
//
//
//	Mat matBigBlur;
//	blur(matSrc, matBigBlur, Size(15, 15));
//	Mat matSub = matBigBlur;
//	subtract(matBigBlur, matSrc, matSub);
//	Mat matBin = matSub;
//	threshold(matSub, matBin, Delta, 255, THRESH_BINARY);
//	subtract(matBin, matBkg, matBin);
//
//	Mat matLabel, matStats, matCentroid;
//	long nTotalLabel = cv::connectedComponentsWithStats(matBin, matLabel, matStats, matCentroid, 8, CV_32S) - 1;
//
//	if (nTotalLabel > 0)
//	{
//		int tempMaxArea = 0;
//		int maxIdx = 0;
//		for (int idx = 1; idx <= nTotalLabel; idx++)
//		{
//			double sumDelta = 0;
//			int sumCnt = 0;
//			double	fThicknessSum = 0;
//			int		nThicknessCnt = 0;
//
//			// Blob ���� Rect
//			int left = matStats.at<int>(idx, CC_STAT_LEFT);
//			int width = matStats.at<int>(idx, CC_STAT_WIDTH);
//			int top = matStats.at<int>(idx, CC_STAT_TOP);
//			int height = matStats.at<int>(idx, CC_STAT_HEIGHT);
//
//			long nArea = matStats.at<int>(idx, CC_STAT_AREA);
//
//			const int nOffset = 2;
//			left -= nOffset;
//			top -= nOffset;
//			width += nOffset + nOffset;
//			height += nOffset + nOffset;
//			if (left < 0) left = 0;
//			if (top < 0) top = 0;
//			if (left + width > matLabel.cols - 1) width = matLabel.cols - 1 - left;
//			if (top + height > matLabel.rows - 1) height = matLabel.rows - 1 - top;
//			if (width <= 0 || height <= 0) continue;
//
//			Rect rectRoi = Rect(left, top, width, height);
//			Mat matLabelRoi = matLabel(rectRoi);
//			Mat matBinRoi = matBin(rectRoi);
//			Mat matBkgRoi = matEdge(rectRoi);
//
//			// ��ü�� �����ʸ��� Edge�� �پ��ִ��� Ȯ��
//			int cnt = 0;
//			for (int y = 0; y < matLabelRoi.rows; ++y)
//			{
//				uchar * ptrBkg = (uchar *)matBkgRoi.ptr(y);
//				int * ptrLabel = (int *)matLabelRoi.ptr(y);
//				bool b = true;
//				for (int x = 0; x < matLabelRoi.cols; ++x)
//				{
//					if (ptrLabel[x] == idx)
//					{
//						if (ptrBkg[x] == 255)
//						{
//							if (b == true)
//							{
//								ptrBkg[x] = 150;
//								b = false;
//								cnt++;
//							}
//						}
//					}
//				}
//			}
//			// �������� �̻� �پ����� ������ ����
//			if (fRatioCut > (float)cnt / (float)height)
//			{
//				for (int y = 0; y < matLabelRoi.rows; ++y)
//				{
//					uchar * ptrGray = (uchar *)matBinRoi.ptr(y);
//					int * ptrLabel = (int *)matLabelRoi.ptr(y);
//
//					for (int x = 0; x < matLabelRoi.cols; ++x)
//					{
//						if (ptrLabel[x] == idx)
//						{
//							ptrGray[x] = 0;
//						}
//					}
//				}
//			}
//
//		}
//	}
//
//
//	CFeatureExtraction cFeatureExtraction;
//	cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_DC, m_tInitTime, m_tBeforeTime, m_strAlgLog);
//	cFeatureExtraction.DoDefectBlobJudgment_ROI(matSrc, matBin, rectRoi, nResize, nCommonPara, EngineerDefectJudgment, pResultBlob, dResolution);
//
//
//	return E_ERROR_CODE_TRUE;
//}
//
//
//long CInspectDC::LogicStart_EdgeDiffGV(Mat& matSrcImage, int & nResult, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
//	int* nCommonPara, CString strAlgPath, CString strResultPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Point* ptCorner, CString * arrStrAnalysisData, double dResolution)
//{
//	// version�Ķ���ͷ� ������ ��İ���
//	// ������ �����ǰ� ���忡�� �������� �� �κ� �����ص���
//	int nResize = 4;
//
//	if (dPara[E_PARA_EdgeDiffGV_Version] >= 3)
//	{
//		nResize = dPara[E_PARA_EdgeDiffGV2_SumLengthRoiSize];
//		if (nResize <= 1) nResize = 4;
//	}
//	else
//	{
//		return E_ERROR_CODE_POINT_WARNING_PARA;
//	}
//
//	float fResize = 1 / (float)nResize;
//
//
//
//	float	fDiffGvCut = (float)dPara[E_PARA_EdgeDiffGV2_DiffGV];
//	float	fHCut = (float)dPara[E_PARA_EdgeDiffGV2_H_Cut];
//	float	fHDCut = (float)dPara[E_PARA_EdgeDiffGV2_HD_Cut];
//	int	nGapA = (int)dPara[E_PARA_EdgeDiffGV2_Gap_A] * fResize;
//	int	nGapB = (int)dPara[E_PARA_EdgeDiffGV2_Gap_B] * fResize;
//	int	nGapC = (int)dPara[E_PARA_EdgeDiffGV2_Gap_C] * fResize;
//	int	nWidthA = (int)dPara[E_PARA_EdgeDiffGV2_Width_A] * fResize;
//	int	nWidthB = (int)dPara[E_PARA_EdgeDiffGV2_Width_B] * fResize;
//	int	nWidthC = (int)dPara[E_PARA_EdgeDiffGV2_Width_C] * fResize;
//	float	fLengthRatio = (float)dPara[E_PARA_EdgeDiffGV2_LengthRatio];
//
//
//
//
//
//	// 1. ��ó��
//	// ���� ���������� �� �־, ����
//
//	cv::Point2f ptSrc[4] = { cv::Point2f((float)ptCorner[E_CORNER_LEFT_TOP].x,	(float)ptCorner[E_CORNER_LEFT_TOP].y),
//		cv::Point2f((float)ptCorner[E_CORNER_RIGHT_TOP].x,	(float)ptCorner[E_CORNER_RIGHT_TOP].y),
//		cv::Point2f((float)ptCorner[E_CORNER_RIGHT_BOTTOM].x,(float)ptCorner[E_CORNER_RIGHT_BOTTOM].y),
//		cv::Point2f((float)ptCorner[E_CORNER_LEFT_BOTTOM].x,	(float)ptCorner[E_CORNER_LEFT_BOTTOM].y) };
//
//
//	cv::Point2f ptDst[4] = { cv::Point2f(0,0),
//		cv::Point2f(rectROI.Width(), 0),
//		cv::Point2f(rectROI.Width(), rectROI.Height()),
//		cv::Point2f(0, rectROI.Height()) };
//
//
//	Rect rectRoi;
//	rectRoi.x = rectROI.left;
//	rectRoi.y = rectROI.top;
//	rectRoi.width = rectROI.Width();
//	rectRoi.height = rectROI.Height();
//
//	Mat matSrc, matBkg;
//	cv::Mat mat2Warp = cv::getPerspectiveTransform(ptSrc, ptDst);
//	cv::warpPerspective(matSrcImage, matSrc, mat2Warp, Size(rectROI.Width(), rectROI.Height()), CV_INTER_AREA);
//	cv::warpPerspective(matBKBuffer, matBkg, mat2Warp, Size(rectROI.Width(), rectROI.Height()), CV_INTER_AREA);
//
//
//	// Resize
//	blur(matSrc, matSrc, Size(5, 5));
//	resize(matSrc, matSrc, Size(), fResize, fResize, INTER_AREA);
//
//	matSrc.convertTo(matSrc, CV_32FC1);
//	blur(matSrc, matSrc, Size(5, 5));
//	resize(matBkg, matBkg, matSrc.size());
//	matBkg = ~matBkg;
//
//	// �����¿� ����ROI ����
//	enum LTRB
//	{
//		L,
//		T,
//		R,
//		B
//	};
//
//	int hSt = matSrc.rows*(1 - fLengthRatio) * 0.5f;
//	int hLen = matSrc.rows *fLengthRatio;
//	int wSt = matSrc.cols*(1 - fLengthRatio) * 0.5f;
//	int wLen = matSrc.cols *fLengthRatio;
//
//	Rect rectArrA[4];
//	rectArrA[L] = Rect(nGapA, hSt, nWidthA, hLen);
//	rectArrA[T] = Rect(wSt, nGapA, wLen, nWidthA);
//	rectArrA[R] = Rect(matSrc.cols - 1 - nGapA - nWidthA, hSt, nWidthA, hLen);
//	rectArrA[B] = Rect(wSt, matSrc.rows - nGapA - nWidthA, wLen, nWidthA);
//
//
//	Rect rectArrB[4];
//	rectArrB[L] = Rect(nGapA + nWidthA + nGapB, hSt, nWidthB, hLen);
//	rectArrB[T] = Rect(wSt, nGapA + nWidthA + nGapB, wLen, nWidthB);
//	rectArrB[R] = Rect(matSrc.cols - 1 - nGapA - nWidthA - nGapB - nWidthB, hSt, nWidthB, hLen);
//	rectArrB[B] = Rect(wSt, matSrc.rows - 1 - nGapA - nWidthA - nGapB - nWidthB, wLen, nWidthB);
//
//	Rect rectArrC[4];
//	rectArrC[L] = Rect(nGapC, hSt, nWidthC, hLen);
//	rectArrC[T] = Rect(wSt, nGapC, wLen, nWidthC);
//	rectArrC[R] = Rect(matSrc.cols - 1 - nGapC - nWidthC, hSt, nWidthC, hLen);
//	rectArrC[B] = Rect(wSt, matSrc.rows - nGapC - nWidthC, wLen, nWidthC);
//
//
//	// ���� - DiffGV
//	Scalar m;
//	float fArrMeanA[4];
//	float fArrMeanB[4];
//	for (int i = 0; i < 4; ++i)
//	{
//		m = mean(matSrc(rectArrA[i]), matBkg(rectArrA[i]));
//		fArrMeanA[i] = m[0];
//
//		m = mean(matSrc(rectArrB[i]), matBkg(rectArrB[i]));
//		fArrMeanB[i] = m[0];
//	}
//
//	float fArrDiffGV[4];
//
//	for (int i = 0; i < 4; ++i)
//	{
//		fArrDiffGV[i] = fArrMeanB[i] - fArrMeanA[i];
//
//	}
//
//
//	// ����2 - DiffGV�� �Ѵ� ������ ����
//	Mat matArrA[4];
//	Mat matArrB[4];
//	// Roi Crop
//	for (int i = 0; i < 4; ++i)
//	{
//		matArrA[i] = matSrc(rectArrA[i]);
//		matArrB[i] = matSrc(rectArrB[i]);
//	}
//	// ��������
//	resize(matArrA[L], matArrA[L], Size(1, matArrA[L].rows), 0, 0, INTER_AREA);
//	resize(matArrA[R], matArrA[R], Size(1, matArrA[R].rows), 0, 0, INTER_AREA);
//	resize(matArrA[T], matArrA[T], Size(matArrA[T].cols, 1), 0, 0, INTER_AREA);
//	resize(matArrA[B], matArrA[B], Size(matArrA[B].cols, 1), 0, 0, INTER_AREA);
//
//	resize(matArrB[L], matArrB[L], Size(1, matArrB[L].rows), 0, 0, INTER_AREA);
//	resize(matArrB[R], matArrB[R], Size(1, matArrB[R].rows), 0, 0, INTER_AREA);
//	resize(matArrB[T], matArrB[T], Size(matArrB[T].cols, 1), 0, 0, INTER_AREA);
//	resize(matArrB[B], matArrB[B], Size(matArrB[B].cols, 1), 0, 0, INTER_AREA);
//
//
//	int nArrCnt[4] = { 0 };
//	for (int j = 0; j < matArrA[L].rows; ++j)
//	{
//		if (matArrB[L].at<float>(j, 0) - matArrA[L].at<float>(j, 0) > fDiffGvCut)
//		{
//			nArrCnt[L]++;
//		}
//	}
//	for (int j = 0; j < matArrA[L].rows; ++j)
//	{
//		if (matArrB[R].at<float>(j, 0) - matArrA[R].at<float>(j, 0) > fDiffGvCut)
//			nArrCnt[R]++;
//	}
//	float * ptrA = matArrA[T].ptr<float>(0);
//	float * ptrB = matArrB[T].ptr<float>(0);
//	for (int i = 0; i < matArrA[T].cols; ++i)
//	{
//		if (ptrB[i] - ptrA[i] > fDiffGvCut)
//			nArrCnt[T]++;
//	}
//	ptrA = matArrA[B].ptr<float>(0);
//	ptrB = matArrB[B].ptr<float>(0);
//	for (int i = 0; i < matArrA[B].cols; ++i)
//	{
//		if (ptrB[i] - ptrA[i] > fDiffGvCut)
//			nArrCnt[B]++;
//	}
//
//
//
//	float fArrScore[4];
//	for (int i = 0; i < 4; ++i)
//	{
//		fArrScore[i] = fArrDiffGV[i] * nArrCnt[i];
//	}
//
//
//	// ����3 - ver2���� �߰��䱸��
//	// ROI C���� �� �������� �̾Ƽ� �����ͻ̱�
//
//	Mat matArrC[4];
//
//	float	fArrH_SumLength[4] = { 0 };
//	float	fArrSlope_SumLength[4] = { 0 };
//	float		fArrMeanRange[4] = { 0 };
//	float		fArrMeanH[4] = { 0 };
//	float		fArrMeanSlope[4] = { 0 };
//
//	matArrC[L] = matSrc(rectArrC[L]);
//	cv::rotate(matSrc(rectArrC[T]), matArrC[T], 2);
//	cv::rotate(matSrc(rectArrC[R]), matArrC[R], 1);
//	cv::rotate(matSrc(rectArrC[B]), matArrC[B], 0);
//
//	float	fMaxGV;
//	float	fMinGV;
//	int		nMaxPos;
//	int		nMinPos;
//	float fHeight;
//	int nDist;
//	int nRange;
//	float fSlope;
//	float fSumRange;
//	int nCntRange;
//	float fSumH;
//	float fSumSlope;
//
//	for (int k = 0; k < 4; ++k)
//	{
//		fSumRange = 0;
//		fSumH = 0;
//		fSumSlope = 0;
//		nCntRange = 0;
//		for (int j = 0; j < matArrC[k].rows; ++j)
//		{
//			fMaxGV = 0.f;
//			fMinGV = 255.f;
//
//			float * ptrC = matArrC[k].ptr<float>(j);
//			for (int i = 0; i < matArrC[k].cols; ++i)
//			{
//				float & gv = ptrC[i];
//				if (gv > fMaxGV)
//				{
//					fMaxGV = gv;
//					nMaxPos = i;
//				}
//				if (gv < fMinGV)
//				{
//					fMinGV = gv;
//					nMinPos = i;
//				}
//			}
//
//			nMaxPos = nMaxPos + nGapC;
//			nMinPos = nMinPos + nGapC;
//			nMaxPos *= nResize;
//			nMinPos *= nResize;
//			fHeight = (fMaxGV - fMinGV);
//			if (fHeight > fHCut)
//			{
//				fArrH_SumLength[k]++;
//				fSumH += fHeight;
//			}
//			nDist = abs(nMaxPos - nMinPos);
//
//
//			fSlope = (nDist > 0.f) ? fHeight / (float)nDist : 0.f;
//			if (fSlope > fHDCut)
//			{
//				fArrSlope_SumLength[k]++;
//				fSumSlope += fSlope;
//			}
//
//			fSumRange += (nMinPos < nMaxPos) ? nMinPos : nMaxPos;
//			nCntRange++;
//		}
//
//
//		fArrMeanRange[k] = fSumRange / (float)nCntRange;
//		fArrMeanH[k] = fSumH / (float)fArrH_SumLength[k];
//		fArrMeanSlope[k] = fSumSlope / (float)fArrSlope_SumLength[k];
//
//	}
//
//
//
//
//
//	// 5. �м��� ������
//	//////////////////////////
//	CString strAnalysisDataPath = _T("D:\\AnalysisData");
//	CreateDirectory(strAnalysisDataPath, NULL);
//
//	SYSTEMTIME time;
//	::GetLocalTime(&time);
//
//	// 5-1. ver1 Feature ����
//	// ���
//	CString strFileName;
//	strFileName.Format(L"%s\\%02d%02d%02d_EdgeDiffGV.csv", strAnalysisDataPath, time.wYear, time.wMonth, time.wDay);
//	if (!PathFileExists(strFileName))
//	{
//		CString str = _T("Time,Recipe,ID,Pattern,L_DiffGV, L_SumLen, LScore, T_DiffGV, T_SumLen, T_Score, R_DiffGV, R_SumLen, R_Scroe, B_DiffGV, B_SumLen, B_Score");
//		str.Format(_T("%s\n"), str);
//		int nTimeOut = 1000;
//		while (_fnOnWritefile(strFileName, str))
//		{
//			nTimeOut -= 10;
//			if (nTimeOut <= 0) break;
//			Sleep(10);
//		}
//	}
//	// ����
//	CString str;
//	str.Format(_T("%02d:%02d:%02d,%s,%s,%s,%f,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,%f"), time.wHour, time.wMinute, time.wSecond,
//		arrStrAnalysisData[1],	// Recipe
//		arrStrAnalysisData[0],	// Cell ID
//		arrStrAnalysisData[2],	// Pattern Name
//		fArrDiffGV[L],
//		nArrCnt[L],
//		fArrScore[L],
//		fArrDiffGV[T],
//		nArrCnt[T],
//		fArrScore[T],
//		fArrDiffGV[R],
//		nArrCnt[R],
//		fArrScore[R],
//		fArrDiffGV[B],
//		nArrCnt[B],
//		fArrScore[B]
//	);
//	str.Format(_T("%s\n"), str);
//
//	int nTimeOut = 1000;
//	while (_fnOnWritefile(strFileName, str))
//	{
//		nTimeOut -= 10;
//		if (nTimeOut <= 0) break;
//		Sleep(10);
//	}
//
//
//
//
//	// 5-1. ver2 Feature ����
//	// ���
//	strFileName.Format(L"%s\\%02d%02d%02d_EdgeDiffGV2.csv", strAnalysisDataPath, time.wYear, time.wMonth, time.wDay);
//	if (!PathFileExists(strFileName))
//	{
//		CString str = _T("Time,Recipe,ID,Pattern,L_Mean_H,L_H_SumLength,L_MeanH/D,L_H/D_SumLength,L_Mean_Range,T_Mean_H,T_H_SumLength,T_MeanH/D,T_H/D_SumLength,T_Mean_Range,R_Mean_H,R_H_SumLength,R_MeanH/D,R_H/D_SumLength,R_Mean_Range,B_Mean_H,B_H_SumLength,B_MeanH/D,B_H/D_SumLength,B_Mean_Range");
//		str.Format(_T("%s\n"), str);
//		int nTimeOut = 1000;
//		while (_fnOnWritefile(strFileName, str))
//		{
//			nTimeOut -= 10;
//			if (nTimeOut <= 0) break;
//			Sleep(10);
//		}
//	}
//	//                                     1  2  3  4  5  1  2  3  4  5  1  2  3  4  5  1  2  3  4  5
//	str.Format(_T("%02d:%02d:%02d,%s,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"), time.wHour, time.wMinute, time.wSecond,
//		arrStrAnalysisData[1],	// Recipe
//		arrStrAnalysisData[0],	// Cell ID
//		arrStrAnalysisData[2],	// Pattern Name
//		fArrMeanH[L],
//		fArrH_SumLength[L],
//		fArrMeanSlope[L],
//		fArrSlope_SumLength[L],
//		fArrMeanRange[L],
//		fArrMeanH[T],
//		fArrH_SumLength[T],
//		fArrMeanSlope[T],
//		fArrSlope_SumLength[T],
//		fArrMeanRange[T],
//		fArrMeanH[R],
//		fArrH_SumLength[R],
//		fArrMeanSlope[R],
//		fArrSlope_SumLength[R],
//		fArrMeanRange[R],
//		fArrMeanH[B],
//		fArrH_SumLength[B],
//		fArrMeanSlope[B],
//		fArrSlope_SumLength[B],
//		fArrMeanRange[B]
//	);
//	str.Format(_T("%s\n"), str);
//
//	nTimeOut = 1000;
//	while (_fnOnWritefile(strFileName, str))
//	{
//		nTimeOut -= 10;
//		if (nTimeOut <= 0) break;
//		Sleep(10);
//	}
//
//
//
//
//
//
//	float arrFeatureL[24] = { 0 };
//	float arrFeatureT[24] = { 0 };
//	float arrFeatureR[24] = { 0 };
//	float arrFeatureB[24] = { 0 };
//	arrFeatureL[0] = fArrDiffGV[L];
//	arrFeatureL[1] = (float)nArrCnt[L];
//	arrFeatureL[2] = fArrScore[L];
//	arrFeatureT[3] = fArrDiffGV[T];
//	arrFeatureT[4] = (float)nArrCnt[T];
//	arrFeatureT[5] = fArrScore[T];
//	arrFeatureR[6] = fArrDiffGV[R];
//	arrFeatureR[7] = (float)nArrCnt[R];
//	arrFeatureR[8] = fArrScore[R];
//	arrFeatureB[9] = fArrDiffGV[B];
//	arrFeatureB[10] = (float)nArrCnt[B];
//	arrFeatureB[11] = fArrScore[B];
//
//	const int idx = 12;
//	arrFeatureL[idx + 0] = fArrH_SumLength[L];
//	arrFeatureL[idx + 1] = fArrSlope_SumLength[L];
//	arrFeatureL[idx + 2] = (float)fArrMeanRange[L];
//	arrFeatureT[idx + 3] = fArrH_SumLength[T];
//	arrFeatureT[idx + 4] = fArrSlope_SumLength[T];
//	arrFeatureT[idx + 5] = (float)fArrMeanRange[T];
//	arrFeatureR[idx + 6] = fArrH_SumLength[R];
//	arrFeatureR[idx + 7] = fArrSlope_SumLength[R];
//	arrFeatureR[idx + 8] = (float)fArrMeanRange[R];
//	arrFeatureB[idx + 9] = fArrH_SumLength[B];
//	arrFeatureB[idx + 10] = fArrSlope_SumLength[B];
//	arrFeatureB[idx + 11] = (float)fArrMeanRange[B];
//
//
//	long	nAlgoListNum = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
//	CFeatureExtraction cFeatureExtraction;
//	cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_LINE_MURA, m_tInitTime, m_tBeforeTime, m_strAlgLog);
//	cFeatureExtraction.DoDefectBlobJudgment_CustomFeature(arrFeatureL, nCommonPara, EngineerDefectJudgment, pResultBlob, nAlgoListNum);
//	cFeatureExtraction.DoDefectBlobJudgment_CustomFeature(arrFeatureT, nCommonPara, EngineerDefectJudgment, pResultBlob, nAlgoListNum);
//	cFeatureExtraction.DoDefectBlobJudgment_CustomFeature(arrFeatureR, nCommonPara, EngineerDefectJudgment, pResultBlob, nAlgoListNum);
//	cFeatureExtraction.DoDefectBlobJudgment_CustomFeature(arrFeatureB, nCommonPara, EngineerDefectJudgment, pResultBlob, nAlgoListNum);
//
//
//	return E_ERROR_CODE_TRUE;
//}

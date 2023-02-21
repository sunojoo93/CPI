#include "stdafx.h"
#include "InspectMuraScratch.h"


CInspectMuraScratch::CInspectMuraScratch(void)
{
	cMem = NULL;
	m_cInspectLibLog = NULL;
	m_strAlgLog = NULL;
	m_tInitTime = 0;
	m_tBeforeTime = 0;
}


CInspectMuraScratch::~CInspectMuraScratch(void)
{
}

long CInspectMuraScratch::DoFindMuraDefect_Scratch(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Mat& matDrawBuffer, wchar_t* strContourTxt)
{
	// �Ķ���� NULL�� ���
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("MuraScratch Inspect start."));
	
	// ���� ���۰� NULL�� ���
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth = (long)matSrcBuffer.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcBuffer.rows;	// ���� ���� ũ��	

												// ���� ����
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// �˻� ������ ���� ũ�⸦ ��� ���
	if (rectROI.left < 0 ||
		rectROI.top < 0 ||
		rectROI.right >= nWidth ||
		rectROI.bottom >= nHeight)	return E_ERROR_CODE_ROI_OVER;

	if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
	if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	// ���� �Ҵ� & �ʱ�ȭ
	cv::Mat matDstImage[E_DEFECT_COLOR_COUNT];

	// �˰��� ���� ��ȣ
	int nAlgImgNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	// ���� �Ҵ�
	matDstImage[E_DEFECT_COLOR_DARK] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// ��� ���۰� NULL�� �ƴ� ��� ����
	if (matDstImage[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstImage[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;

	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("Dst Buf Set."));

	// ���� ���� �˰��� �ٸ�
	switch (nAlgImgNum)
	{
	case E_IMAGE_CLASSIFY_AVI_R:
	case E_IMAGE_CLASSIFY_AVI_G:
	case E_IMAGE_CLASSIFY_AVI_B:
	{

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_BLACK:
	case E_IMAGE_CLASSIFY_AVI_PCD:
	{
		// �˻� ����
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
	{
		// Point���� �˻��� ��� ���
	}
	break;

	//case E_IMAGE_CLASSIFY_AVI_GRAY_32:
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	//case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	//case E_IMAGE_CLASSIFY_AVI_GRAY_128:
	//case E_IMAGE_CLASSIFY_AVI_WHITE:
	{
		// ���� ����
		nErrorCode = LogicStart_Scratch(matSrcBuffer, matSrcBufferRGB, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath);
	}
	break;

	// ���� ��ȣ�� �߸� ���� ���.
	default:
		return E_ERROR_CODE_TRUE;
	}

	// �� ������ �ƴ� ���
	if (!matDstImage[E_DEFECT_COLOR_BRIGHT].empty() &&
		!matDstImage[E_DEFECT_COLOR_DARK].empty())
	{
		// ���� ���� �̿� ���� ���� ( ���� ���� ���� )
		if (!matBKBuffer.empty())
		{
			cv::subtract(matDstImage[E_DEFECT_COLOR_BRIGHT], matBKBuffer, matDstImage[E_DEFECT_COLOR_BRIGHT]);		//�޸� �Ҵ�
			cv::subtract(matDstImage[E_DEFECT_COLOR_DARK], matBKBuffer, matDstImage[E_DEFECT_COLOR_DARK]);		//�޸� �Ҵ�

			writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("Copy CV Sub Result."));
		}
	
		CMatBuf cMatBufTemp;
		cMatBufTemp.SetMem(cMem);

		cv::Rect rectBlobROI;
		rectBlobROI.x = rectROI.left;
		rectBlobROI.y = rectROI.top;
		rectBlobROI.width = rectROI.Width();
		rectBlobROI.height = rectROI.Height();
		// ��
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem(&cMatBufTemp);
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA_SCRATCH, m_tInitTime, m_tBeforeTime, m_strAlgLog);


			//////////////////////////////////////////////////////////////////////////
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstImage[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
		nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("M3_B"), EngineerDefectJudgment, pResultBlob);


		if (!USE_ALG_CONTOURS)	// ��� �ܰ��� ����
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// ��� �ܰ��� �׸���
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
		////////////////////////////////////////////////////////////////////////

		// �Ҵ� ����
		matDstImage[E_DEFECT_COLOR_BRIGHT].release();
		matDstImage[E_DEFECT_COLOR_DARK].release();

		writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("Mura3 End."));

// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 		}

	}

	return nErrorCode;
}

long CInspectMuraScratch::LogicStart_Scratch(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath)
{
	// ���� �ڵ�
	long	nErrorCode = E_ERROR_CODE_TRUE;



	// ��� �Ķ����
	int		nGauSize				= (int)dPara[E_PARA_AVI_MURA_SCRATCH_GAUSSIAN_SIZE];
	double	dGauSig					= dPara[E_PARA_AVI_MURA_SCRATCH_GAUSSIAN_SIGMA];
	int		dResize_count			= (int)dPara[E_PARA_AVI_MURA_SCRATCH_RESIZE_LOOP_COUNT];

	int		nAdjust1_Min_GV			= (int)dPara[E_PARA_AVI_MURA_SCRATCH_ADJUST1_VALUE]; //20
	double	dContrast_Value			= (double)dPara[E_PARA_AVI_MURA_SCRATCH_CONTRAST_VALUE]; //0.01
	
	int		nSegX					= (int)dPara[E_PARA_AVI_MURA_SCRATCH_SEG_X];
	int		nSegY					= (int)dPara[E_PARA_AVI_MURA_SCRATCH_SEG_Y];

	float	fBrightRatio			= (float)dPara[E_PARA_AVI_MURA_SCRATCH_ACTIVE_BRIGHT_RATIO];
	float	fBrightRatio_Edge		= (float)dPara[E_PARA_AVI_MURA_SCRATCH_EDGE_BRIGHT_RATIO];

	int     nHough_TH				= (int)dPara[E_PARA_AVI_MURA_SCRATCH_HOUGH_TH];
	int     nHough_minLine_Length	= (int)dPara[E_PARA_AVI_MURA_SCRATCH_HOUGH_MIN_LINE_LENGHT];
	int     nHough_maxLine_Gap		= (int)dPara[E_PARA_AVI_MURA_SCRATCH_HOUGH_MAX_LINE_GAP];


	float	fDegree_Min				= (float)dPara[E_PARA_AVI_MURA_SCRATCH_DEGREE_MIN];
	float	fDegree_Max				= (float)dPara[E_PARA_AVI_MURA_SCRATCH_DEGREE_MAX];

	//int		nDliateMorp				= (int)dPara[E_PARA_AVI_MURA_SCRATCH_MORP_DLIATE];
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

																								 // ���� �Ҵ� & �ʱ�ȭ
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// �˻� ����
	CRect rectTemp(rectROI);

	long	nWidth = (long)matSrcImage.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcImage.rows;	// ���� ���� ũ��

	cv::Mat matSrcROIBuf = cMatBufTemp.GetMat(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()).size(), matSrcImage.type(), false);
	cv::Mat matBrROIBuf = cMatBufTemp.GetMat(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()).size(), matDstImage[E_DEFECT_COLOR_BRIGHT].type(), false);
	cv::Mat matDaROIBuf = cMatBufTemp.GetMat(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()).size(), matDstImage[E_DEFECT_COLOR_DARK].type(), false);

	matSrcROIBuf = matSrcImage(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	matBrROIBuf = matDstImage[E_DEFECT_COLOR_BRIGHT](cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	matDaROIBuf = matDstImage[E_DEFECT_COLOR_DARK](cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	//////////////////////////////////////////////////////////////////////////

	// ��� ����
	cv::Mat matBkROIBuf;
	if (!matBKBuffer.empty())
		matBkROIBuf = matBKBuffer(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROIBuf);
	}

	///////////////////////////////////////////////////////////////////////////*/
	// Blur
	cv::Mat matGauBuf = cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);
	cv::GaussianBlur(matSrcROIBuf, matGauBuf, cv::Size(nGauSize, nGauSize), dGauSig, dGauSig);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_Gau.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matGauBuf);
	}


	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("GaussianBlur."));

	
	//////////////////////////////////////////////////////////////////////////
	//��ο� �κ� ��� ������ �ø���
	//////////////////////////////////////////////////////////////////////////
	cv::Scalar m, s;
	cv::meanStdDev(matGauBuf, m, s);

	for (int i = 0; i < matGauBuf.cols*matGauBuf.rows; i++) {
		if (matGauBuf.data[i] < nAdjust1_Min_GV) {
			matGauBuf.data[i] = m[0];
		}
	}

	//matTmp.copyTo(matResize(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height())));
	//FFT(matGauBuf);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_Adjust1.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matGauBuf);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("Adjust1."));
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//��Ʈ��Ʈ
	//////////////////////////////////////////////////////////////////////////
	int var_brightness1 = 0;
	double var_contrast1 = dContrast_Value;
	double c, d;
	if (var_contrast1 > 0)
	{
		double delta1 = 127.0*var_contrast1;
		c = 255.0 / (255.0 - delta1 * 2);
		d = c*(var_brightness1 - delta1);
	}
	else
	{
		double delta1 = -128.0*var_contrast1;
		c = (256.0 - delta1 * 2) / 255.0;
		d = c*var_brightness1 + delta1;
	}

	cv::Mat mat_dst = cMatBufTemp.GetMat(matGauBuf.size(), matGauBuf.type());

	matGauBuf.convertTo(mat_dst, CV_8U, c, d);


	normalize(mat_dst, mat_dst, 0, 255, NORM_MINMAX);

// 	cv::Scalar m_2, s_2;
// 	cv::meanStdDev(mat_dst, m_2, s_2);
// 
// 	for (int i = 0; i < mat_dst.cols*mat_dst.rows; i++) {
// 		if (mat_dst.data[i] < nAdjust1_Min_GV) {
// 			mat_dst.data[i] = m_2[0];
// 		}
// 	}

	//cv::equalizeHist(mat_dst, mat_dst);


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_CONTRAST.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, mat_dst);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("Contrast."));

	cv::GaussianBlur(mat_dst, mat_dst, cv::Size(nGauSize, nGauSize), dGauSig, dGauSig);

	
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_Gau2.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, mat_dst);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("GaussianBlur2."));

	cv::Mat mat_TH = cMatBufTemp.GetMat(mat_dst.size(), mat_dst.type());


	CRect resize_Rect(0, 0, mat_dst.cols - 1, mat_dst.rows - 1);
	RangeAvgThreshold_Gray(mat_dst, mat_TH, resize_Rect, 1, nSegX, nSegY, 0.96, fBrightRatio, 0.96, fBrightRatio_Edge, 1, &cMatBufTemp); //choi 05.01

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, mat_TH);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("RangeThreshold."));

	for (int i = 0; i < dResize_count; i++) {
		cv::resize(mat_TH, mat_TH, mat_TH.size() / 2, 3, 3, INTER_AREA);
	}


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_resize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, mat_TH);
	}

	vector<Vec4i> linesP;
	cv::HoughLinesP(mat_TH, linesP, 1, (CV_PI / 180), nHough_TH, nHough_minLine_Length, nHough_maxLine_Gap);

	Mat mat_houghP= cMatBufTemp.GetMat(mat_dst.size(), mat_dst.type());
	Mat mat_reult = cMatBufTemp.GetMat(mat_dst.size(), mat_dst.type());
	
	mat_dst.copyTo(mat_houghP);

	for (int i = 0; i < dResize_count; i++) {
		cv::resize(mat_houghP, mat_houghP, mat_houghP.size() / 2, 3, 3, INTER_AREA);
	}
	for (int i = 0; i < dResize_count; i++) {
		cv::resize(mat_reult, mat_reult, mat_reult.size() / 2, 3, 3, INTER_AREA);
	}

	for (size_t i = 0; i < linesP.size(); i++)
	{
		Vec4i l = linesP[i];

		int x = l[0] - l[2];
		int y = l[1] - l[3];
		double radian = atan2(y, x);
		double degree = abs(abs((radian * 180) / CV_PI) - 180.0);// ���� -> ��׸� ��ȯ

		if (degree >= fDegree_Min && degree <= fDegree_Max) {
			line(mat_houghP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar::all(255), 2, 8);
			line(mat_reult, Point(l[0], l[1]), Point(l[2], l[3]), Scalar::all(255), 2, 8);
		}
	}

	//cv::morphologyEx(mat_reult, mat_reult, MORPH_CLOSE, cv::getStructuringElement(MORPH_DILATE, cv::Size(nDliateMorp, nDliateMorp)));

	//writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("Morp."));

	cv::resize(mat_houghP, mat_houghP, matGauBuf.size(), 3, 3, INTER_AREA);
	cv::resize(mat_reult, mat_reult, matGauBuf.size(), 3, 3, INTER_AREA);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_HoughP.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, mat_houghP);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_Result.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, mat_reult);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_SCRATCH, __FUNCTION__, _T("HoughP."));

	

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA_SCRATCH_%02d_Final_Result.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, mat_reult);
	}

	vector<Vec4i>().swap(linesP);


	mat_reult.copyTo(matBrROIBuf);
	matDaROIBuf.setTo(0);
	// ��� ����
	

	return nErrorCode;
}

long CInspectMuraScratch::ImageSave(CString strPath, cv::Mat matSrcBuf)
{
	if (matSrcBuf.type() == CV_8U)
	{
		cv::imwrite((cv::String)(CStringA)strPath, matSrcBuf);
	}
	else
	{
		cv::Mat matTemp = cv::Mat::zeros(matSrcBuf.size(), CV_8U);
		matSrcBuf.convertTo(matTemp, CV_8U, 1. / 16.);

		cv::imwrite((cv::String)(CStringA)strPath, matTemp);

		matTemp.release();
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectMuraScratch::RangeAvgThreshold_Gray(cv::Mat& matSrcImage, cv::Mat& matDstImage, CRect rectROI,
	long nLoop, long nSegX, long nSegY, float fDarkRatio, float fBrightRatio, float fDarkRatio_Edge, float fBrightRatio_Edge, int Defect_Color_mode, CMatBuf* cMemSub)
{
	// ���� ���� 10���� ���� ���.
	if (nSegX <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;
	if (nSegY <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;

	// �̹��� ũ��
	long	nWidth = (long)matSrcImage.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcImage.rows;	// ���� ���� ũ��

	long x, y;
	long nStart_X, nStart_Y, nEnd_X, nEnd_Y;

	long nPixelSum, nPixelCount, nPixelAvg;

	// Active ������ �˻�
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// Temp �޸� �Ҵ�	
	cv::Mat matBlurBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	int nBlur = 5;

	//���귮 ���Ҹ���
	//cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	//Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if (nLoop > 0)
	{
		cv::blur(matSrcImage, matBlurBuf, cv::Size(nBlur, nBlur));

		if (nLoop > 1)
		{
			// Avg
			for (int i = 1; i < nLoop; i++)
			{
				cv::blur(matBlurBuf, matBlurBuf1, cv::Size(nBlur, nBlur));

				matBlurBuf1.copyTo(matBlurBuf);
			}
		}
	}
	else
	{
		matSrcImage.copyTo(matBlurBuf);
	}
	matBlurBuf1.release();

	//////////////////////////////////////////////////////////////////////////

	for (y = 0; y < nRangeY; y++)
	{
		// y ���� ���
		nStart_Y = y * nSegY + rectROI.top;
		if (y == nRangeY - 1)		nEnd_Y = rectROI.bottom;
		else					nEnd_Y = nStart_Y + nSegY;

		for (x = 0; x < nRangeX; x++)
		{
			double dbDarkRatio = fDarkRatio;
			double dbBrightRatio = fBrightRatio;


			// x ���� ���
			nStart_X = x * nSegX + rectROI.left;
			if (x == nRangeX - 1)		nEnd_X = rectROI.right;
			else					nEnd_X = nStart_X + nSegX;

			//Edge �κ�
			if (nStart_X == rectROI.left || y == rectROI.top || nEnd_X == rectROI.right || nEnd_Y == rectROI.bottom)
			{
				dbDarkRatio = fDarkRatio_Edge;
				dbBrightRatio = fBrightRatio_Edge;
			}

			// ���� ����
			cv::Rect rectTemp;
			rectTemp.x = nStart_X;
			rectTemp.y = nStart_Y;
			rectTemp.width = nEnd_X - nStart_X + 1;
			rectTemp.height = nEnd_Y - nStart_Y + 1;

			// ���� ROI
			cv::Mat matTempBuf = matBlurBuf(rectTemp);

			// ������׷�
			cv::Mat matHisto;
			GetHistogram_th(matTempBuf, matHisto);

			double dblAverage;
			double dblStdev;

			if (Defect_Color_mode == 1) {
				GetMeanStdDev_From_Histo(matHisto, 0, 255, dblAverage, dblStdev);
			}
			if (Defect_Color_mode == 0) {
				GetMeanStdDev_From_Histo(matHisto, 0, 256, dblAverage, dblStdev);
			}

			// ��� ���� ����
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if (nMinGV < 0)	nMinGV = 0;
			if (nMaxGV > 255)	nMaxGV = 255;

			// �ʱ�ȭ
			nPixelSum = 0;
			nPixelCount = 0;
			nPixelAvg = 0;

			// ������ ��� ������ŭ�� �����
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m = nMinGV; m <= nMaxGV; m++, pVal++)
			{
				nPixelSum += (m * *pVal);
				nPixelCount += *pVal;
			}

			if (nPixelCount == 0)	continue;

			// ���� �� ���
			nPixelAvg = (long)(nPixelSum / (double)nPixelCount);

			// ��� * Ratio
			long nDarkTemp = (long)(nPixelAvg * dbDarkRatio);
			long nBrightTemp = (long)(nPixelAvg * dbBrightRatio);

			// Gray �ʹ� ��ο� ��� ����.
			// ( ��� GV 2 ~ 3 * fBrightRatio -> ����ȭ : ������ �� �ö�� )
			if (nBrightTemp < 15)	nBrightTemp = 15;

			// ���� ó��
			if (nDarkTemp < 0)		nDarkTemp = 0;
			if (nBrightTemp > 255)	nBrightTemp = 255;

			// �Ķ���� 0�ΰ�� ���� ó��
			if (dbDarkRatio == 0)	nDarkTemp = -1;
			if (dbBrightRatio == 0)	nBrightTemp = 256;

			// E_DEFECT_COLOR_DARK Threshold
			if (Defect_Color_mode == 1) {
				cv::Mat matTempBufT = matDstImage(rectTemp);
				cv::threshold(matTempBuf, matTempBufT, nBrightTemp, 255.0, THRESH_BINARY);
			}

			if (Defect_Color_mode == 0) {
				cv::Mat matTempBufT = matDstImage(rectTemp);
				cv::threshold(matTempBuf, matTempBufT, nDarkTemp, 255.0, THRESH_BINARY_INV);
			}
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


long CInspectMuraScratch::GetHistogram_th(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if (nBit == -1)	return E_ERROR_CODE_LINE_HISTO;

	int nHistSize = (int)pow((double)256, (double)nBit);
	float fHiStronge[] = { 0, (float)nHistSize };
	const float *ranges[] = { fHiStronge };

	cv::calcHist(&matSrcImage, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}


int CInspectMuraScratch::GetBitFromImageDepth(int nDepth)
{
	int nRet = -1;

	switch (nDepth)
	{
	case CV_8U:
	case CV_8S:
		nRet = 1;
		break;

	case CV_16U:
	case CV_16S:
		nRet = 2;
		break;

	case CV_32S:
	case CV_32F:
		nRet = 4;
		break;

	case CV_64F:
		nRet = 8;
		break;

	default:
		nRet = -1;
		break;
	}

	return nRet;
}

long CInspectMuraScratch::GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev)
{
	GetAverage_From_Histo(matHisto, nMin, nMax, dblAverage);

	double dblValSum = 0;
	double dblCountSum = 0;
	double dblTmp;
	float *pVal = (float*)matHisto.data;

	// �л��� ���Ѵ�.
	for (int i = nMin; i < nMax; i++)
	{
		dblTmp = (i - dblAverage);
		dblTmp = dblTmp*dblTmp;
		dblValSum += (*pVal * dblTmp);
		dblCountSum += *pVal;
		pVal++;
	}

	dblStdev = dblValSum / dblCountSum; ;
	dblStdev = sqrt(dblStdev);

	return E_ERROR_CODE_TRUE;
}

long CInspectMuraScratch::GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage)
{
	double dblValSum = 0;
	double dblCountSum = 0;
	float *pVal = (float*)matHisto.data;

	// ����� ���Ѵ�.
	for (int i = nMin; i < nMax; i++)
	{
		dblValSum += (*pVal * i);
		dblCountSum += *pVal;
		pVal++;
	}

	dblAverage = dblValSum / dblCountSum;

	return E_ERROR_CODE_TRUE;
}

long CInspectMuraScratch::FFT(cv::Mat& matSrcImage)
{
	cv::Mat Padded;

	getZeroPaddedImage(matSrcImage, Padded);
	//////////////////////////////////////////////////////////////////////////
	cv::Mat planes[] = { Mat_<float>(Padded), Mat::zeros(Padded.size(),CV_32F)};
	cv::Mat complexI;
	merge(planes, 2, complexI);

	dft(complexI, complexI);
	split(complexI, planes);

	//�Ǽ��� ����� ���
	Mat magI, angI;
	getLogMsg(planes, magI, angI);

	//����ȯ�� ���� �ִ� �ּҰ� ���
	double min_mag, max_mag, min_ang, max_ang;
	minMaxLoc(magI, &min_mag, &max_mag);
	minMaxLoc(angI, &min_ang, &max_ang);

	//����ȭ
	normalize(magI, magI, 0, 1, NORM_MINMAX);
	normalize(angI, angI, 0, 1, NORM_MINMAX);

	rearrange(magI);
	rearrange(angI);

	// 	if (bImageSave)
	// 	{
	// 		CString strTemp;
	// 		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_magI.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
	// 		ImageSave(strTemp, magI);
	//
	// 		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_angI.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
	// 		ImageSave(strTemp, angI);
	// 	}
	//���͸�
	//////////////////////////////////////////////////////////////////////////
	myFilter(magI);

	// 	if (bImageSave)
	// 	{
	// 		CString strTemp;
	// 		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_FFT_Filer.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
	// 		ImageSave(strTemp, magI);
	// 	}
	//////////////////////////////////////////////////////////////////////////
	rearrange(magI);
	rearrange(angI);

	magI = magI * (max_mag - min_mag) + min_ang;
	angI = angI * (max_ang - min_ang) + min_ang;

	cv::exp(magI, magI);
	magI -= 1;

	cv::polarToCart(magI, angI, planes[0], planes[1], true);
	cv::merge(planes, 2, complexI);
	//Inverse FFT
	//////////////////////////////////////////////////////////////////////////
	dft(complexI, complexI, DFT_INVERSE | DFT_SCALE | DFT_REAL_OUTPUT);
	split(complexI, planes);
	//normalize(planes[0], planes[0], 0, 1, NORM_MINMAX);

	Mat result;
	cv::resize(planes[0], result, matSrcImage.size(), 0, 0, INTER_LINEAR);

	cv::Rect roi(0, 0, matSrcImage.cols, matSrcImage.rows);
	planes[0](roi).copyTo(result);

	Mat result_tmp;
	result.convertTo(matSrcImage, CV_8U);

	return E_ERROR_CODE_TRUE;
}

void CInspectMuraScratch::getZeroPaddedImage(Mat& src, Mat& dst) {
	int m = getOptimalDFTSize(src.rows);
	int n = getOptimalDFTSize(src.cols);
	cv::copyMakeBorder(src, dst, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));

	return;
}

void CInspectMuraScratch::getLogMsg(Mat planes[], Mat& magI, Mat& angI) {
	cv::cartToPolar(planes[0], planes[1], magI, angI, true);
	magI += Scalar::all(1);
	cv::log(magI, magI);

	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows&-2));

	return;
}
void CInspectMuraScratch::rearrange(Mat magI) {
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));
	Mat q1(magI, Rect(cx, 0, cx, cy));
	Mat q2(magI, Rect(0, cy, cx, cy));
	Mat q3(magI, Rect(cx, cy, cx, cy));

	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	return;
}

void CInspectMuraScratch::myFilter(Mat& magI) {
	Mat f = Mat::zeros(magI.rows, magI.cols, CV_32F);
	//f.setTo(1);

	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	// 	Mat tmp;
	// 	magI.copyTo(tmp);

// 		cv::RotatedRect rectE(cv::Point(cx, cy), cv::Size(11000, 500), 20.0);
// 	 	cv::RotatedRect rectE_2(cv::Point(cx, cy), cv::Size(11000, 500), -20.0);
// 	 	cv::ellipse(f, rectE, cv::Scalar(1), -1);
// 	 	cv::ellipse(f, rectE_2, cv::Scalar(1), -1);
	circle(f, Point(cx, cy), 1000, Scalar(1), -1);


	magI = magI.mul(f);
}
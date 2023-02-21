
/************************************************************************/
// Mura �ҷ� ���� ���� �ҽ�
// ������ : 18.05.31 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "InspectMura4.h"




CInspectMura4::CInspectMura4(void)
{
	cMem = NULL;
	m_cInspectLibLog = NULL;
	m_strAlgLog = NULL;
	m_tInitTime = 0;
	m_tBeforeTime = 0;

	//////////////////////////////////////////////////////////////////////////
	//sz = 11;
	//////////////////////////////////////////////////////////////////////////
}


CInspectMura4::~CInspectMura4(void)
{
}


// Main �˻� �˰���
long CInspectMura4::DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// �Ķ���� NULL�� ���
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

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
	if (rectROI.left	<	0 ||
		rectROI.top		<	0 ||
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

	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
	case E_IMAGE_CLASSIFY_AVI_WHITE:
	{
		// ���� ����
		nErrorCode = LogicStart_SPOT(matSrcBuffer, matSrcBufferRGB, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath);
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
			cv::subtract(matDstImage[E_DEFECT_COLOR_BRIGHT], matBKBuffer, matBrightBuffer);		//�޸� �Ҵ�
			cv::subtract(matDstImage[E_DEFECT_COLOR_DARK], matBKBuffer, matDarkBuffer);		//�޸� �Ҵ�

			writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Copy CV Sub Result."));
		}
		// ��� �ѱ��
		else
		{
			// 			matBrightBuffer	= matDstImage[E_DEFECT_COLOR_BRIGHT].clone();		//�޸� �Ҵ�
			// 			matDarkBuffer	= matDstImage[E_DEFECT_COLOR_DARK].clone();			//�޸� �Ҵ�

			matDstImage[E_DEFECT_COLOR_DARK].copyTo(matDarkBuffer);
			matDstImage[E_DEFECT_COLOR_BRIGHT].copyTo(matBrightBuffer);

			writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Copy Clone Result."));
		}

		// �Ҵ� ����
		matDstImage[E_DEFECT_COLOR_BRIGHT].release();
		matDstImage[E_DEFECT_COLOR_DARK].release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return nErrorCode;
}


long CInspectMura4::DoFindMuraDefect2(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Mat& matDrawBuffer, wchar_t* strContourTxt)
{
	// �Ķ���� NULL�� ���
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Inspect start."));
	////////////////////////////////////////////////////////////////////////// choi 05.13
//	int JudgeSpot_Flag = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_FLAG];
	//////////////////////////////////////////////////////////////////////////
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

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Dst Buf Set."));

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

	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
		break;
	case E_IMAGE_CLASSIFY_AVI_WHITE:
	{
		// ���� ����
		nErrorCode = LogicStart_SPOT(matSrcBuffer, matSrcBufferRGB, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath);
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
			cv::subtract(matDstImage[E_DEFECT_COLOR_BRIGHT], matBKBuffer, matBrightBuffer);		//�޸� �Ҵ�
			cv::subtract(matDstImage[E_DEFECT_COLOR_DARK], matBKBuffer, matDarkBuffer);		//�޸� �Ҵ�

			writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Copy CV Sub Result."));
		}
		// ��� �ѱ��
		else
		{
			// 			matBrightBuffer	= matDstImage[E_DEFECT_COLOR_BRIGHT].clone();		//�޸� �Ҵ�
			// 			matDarkBuffer	= matDstImage[E_DEFECT_COLOR_DARK].clone();			//�޸� �Ҵ�

			matDstImage[E_DEFECT_COLOR_DARK].copyTo(matDarkBuffer);
			matDstImage[E_DEFECT_COLOR_BRIGHT].copyTo(matBrightBuffer);

			writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Copy Clone Result."));
		}

		///////////////////////////// woojin 19.08.28
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
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA_4, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_bright ���
		// choikwangil 12.03 black hole
		switch (nAlgImgNum)
		{
		case E_IMAGE_CLASSIFY_AVI_WHITE:
		{
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matBrightBuffer(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_DARK, _T("DM_"), EngineerDefectJudgment, pResultBlob);
		}
		break;
		case E_IMAGE_CLASSIFY_AVI_GRAY_64:
		{
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matBrightBuffer(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("DM_"), EngineerDefectJudgment, pResultBlob);

// 			if (JudgeSpot_Flag == 1) {
// 				// White Spot ��з�
// 				JudgeWhiteSpot(matSrcBuffer, matBrightBuffer, rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
// 				writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("JudgeWhiteSpot."));
// 			}
		}
		break;
		}


		// �ܰ� ���� ����ϴ� ���, Judgement()���� ���� ���� ( �ߺ� ���Ž�, �ش� �ҷ� �ܰ��� �ȱ׸� ) //choikwangil 04.07 �߰� draw ���� ����
		// ��� ���ϴ� ���, Alg�ܿ��� ���� ���� ( �ߺ� ���ŵǵ�, �ش� �ҷ� �ܰ��� �׸� )
		if (!USE_ALG_CONTOURS)	// ��� �ܰ��� ����
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// ��� �ܰ��� �׸���
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);



		// �Ҵ� ����
		matDstImage[E_DEFECT_COLOR_BRIGHT].release();
		matDstImage[E_DEFECT_COLOR_DARK].release();

		writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 End."));

// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 		}
 	}

	return nErrorCode;
}

void CInspectMura4::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
{
	rectInspROI.x = rectROI.left - nOffset;
	rectInspROI.y = rectROI.top - nOffset;
	rectInspROI.width = rectROI.Width() + nOffset * 2;
	rectInspROI.height = rectROI.Height() + nOffset * 2;

	if (rectInspROI.x < 0) rectInspROI.x = 0;
	if (rectInspROI.y < 0) rectInspROI.y = 0;
	if (rectInspROI.width > nWidth - rectInspROI.x) rectInspROI.width = nWidth - rectInspROI.x;
	if (rectInspROI.height > nHeight - rectInspROI.y) rectInspROI.height = nHeight - rectInspROI.y;
}




// Dust ���� ��, ��� ���� �Ѱ��ֱ�
long CInspectMura4::GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
	double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	// ���� �ڵ�
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// �Ķ���� NULL�� ���.
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if (matSrcBuffer.empty())							return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuffer[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuffer[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDustBuffer[E_DEFECT_COLOR_DARK].empty())	return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDustBuffer[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;


	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	//////////////////////////////////////////////////////////////////////////

	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
	bool	bDefectNum = (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG] > 0) ? true : false;
	bool	bDrawDust = (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG] > 0) ? true : false;

	//////////////////////////////////////////////////////////////////////////

	// Dust ���� �ҷ� ����
	if (nImageNum == E_IMAGE_CLASSIFY_AVI_DUST)
		return E_ERROR_CODE_TRUE;

	// ���� ���� ( �̹� �˻� ��, ����ó�� �˻� �� )
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// ���� �Ҵ� & �ʱ�ȭ
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);
	cv::Mat matDustTemp = cMatBufTemp.GetMat(matDustBuffer[E_DEFECT_COLOR_BRIGHT].size(), matDustBuffer[E_DEFECT_COLOR_BRIGHT].type(), false);

	// �ӽ� ���� ����
	cv::Mat matResBuf = cMatBufTemp.GetMat(matDstBuffer[E_DEFECT_COLOR_BRIGHT].size(), matDstBuffer[E_DEFECT_COLOR_BRIGHT].type(), false);

	int nModePS = 1;

	// Dust ������ �ִ� ��� ���� ���� ���� ����
	if (!matDustBuffer[E_DEFECT_COLOR_BRIGHT].empty() &&
		!matDustBuffer[E_DEFECT_COLOR_DARK].empty())
	{
		// ���� ���� ������ Ȯ��		
		if (matSrcBuffer.rows == matDustBuffer[E_DEFECT_COLOR_DARK].rows	&&
			matSrcBuffer.cols == matDustBuffer[E_DEFECT_COLOR_DARK].cols)
			nModePS = 1;
		else
			nModePS = 2;



		writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("DeleteArea."));
	}
	// Dust ���� ���� ���� ���
	// ���� ���� ���� �ҷ� ����
	else
	{
		writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Empty Dust Image."));
	}



	// �ҷ� ���� & ��� ������
	{
		cv::Rect rectBlobROI;
		rectBlobROI.x = rectROI.left;
		rectBlobROI.y = rectROI.top;
		rectBlobROI.width = rectROI.Width();
		rectBlobROI.height = rectROI.Height();
		// ��
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem(&cMatBufTemp);
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA_4, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_DARK ���
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_DARK, _T("DM_"), EngineerDefectJudgment, pResultBlob);
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// �޸� ����
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}
		writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("DoDefectBlobJudgment-Dark."));



		// Dark �ҷ� ����
		int nStartIndex = pResultBlob->nDefectCount;

		// �ܰ� ���� ����ϴ� ���, Judgement()���� ���� ���� ( �ߺ� ���Ž�, �ش� �ҷ� �ܰ��� �ȱ׸� )
		// ��� ���ϴ� ���, Alg�ܿ��� ���� ���� ( �ߺ� ���ŵǵ�, �ش� �ҷ� �ܰ��� �׸� )
		if (!USE_ALG_CONTOURS)	// ��� �ܰ��� ����
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// ��� �ܰ��� �׸���
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

		//////////////////////////////////////////////////////////////////////////

		BOOL bUse[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];

		// ���� Spot �˻� �ϴ� ���
		if (EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse || EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse) // 04.16 choi
		{
			// ���� �ϱ� ���� ����ȭ ( ���� : 255 / �鹫�� 200 )
			cv::threshold(matDstBuffer[E_DEFECT_COLOR_BRIGHT], matResBuf, 220, 255.0, CV_THRESH_BINARY);

			// ���� �Ķ���� ����
			for (int p = 0; p < MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; p++)
			{
				bUse[p] = EngineerDefectJudgment[p].bDefectItemUse;

				// ��� �Ķ���� �˻� ����
				EngineerDefectJudgment[p].bDefectItemUse = false;
			}

			// Spot �˻縸 ����
			EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = true;
			EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = true; // 04.16 choi

																								// E_DEFECT_COLOR_BRIGHT ���
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matResBuf(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BM_"), EngineerDefectJudgment, pResultBlob);
			if (nErrorCode != E_ERROR_CODE_TRUE)
			{
				// �޸� ����
				matSrcBuffer.release();
				matDstBuffer[E_DEFECT_COLOR_DARK].release();
				matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

				return nErrorCode;
			}
			writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("DoDefectBlobJudgment-Bright Spot."));

			// White Spot ��з�
			//		JudgeWhiteSpot(matSrcBuffer, matDstBuffer[E_DEFECT_COLOR_BRIGHT], rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
			writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("JudgeWhiteSpot."));

			// �ܰ� ���� ����ϴ� ���, Judgement()���� ���� ���� ( �ߺ� ���Ž�, �ش� �ҷ� �ܰ��� �ȱ׸� )
			// ��� ���ϴ� ���, Alg�ܿ��� ���� ���� ( �ߺ� ���ŵǵ�, �ش� �ҷ� �ܰ��� �׸� )
			if (!USE_ALG_CONTOURS)	// ��� �ܰ��� ����
				cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

			// ��� �ܰ��� �׸���
			cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

			// ����
			for (int p = 0; p < MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; p++)
			{
				EngineerDefectJudgment[p].bDefectItemUse = bUse[p];
			}
		}

		//////////////////////////////////////////////////////////////////////////

		// �鹫�� �ϱ� ���� ����ȭ ( ���� : 255 / �鹫�� 200 )
		cv::threshold(matDstBuffer[E_DEFECT_COLOR_BRIGHT], matResBuf, 190, 255.0, CV_THRESH_BINARY);

		// Spot �˻縸 ����
		bUse[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT] = EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse;
		bUse[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT] = EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse; //04.16 choi

		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = false;
		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = false; //04.16 choi

																							 // E_DEFECT_COLOR_BRIGHT ���
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matResBuf(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BM_"), EngineerDefectJudgment, pResultBlob);
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// �޸� ����
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}
		writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("DoDefectBlobJudgment-Bright."));

		// ����
		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = bUse[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT];
		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = bUse[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT]; // 04.16 choi


																																  // �ܰ� ���� ����ϴ� ���, Judgement()���� ���� ���� ( �ߺ� ���Ž�, �ش� �ҷ� �ܰ��� �ȱ׸� )
																																  // ��� ���ϴ� ���, Alg�ܿ��� ���� ���� ( �ߺ� ���ŵǵ�, �ش� �ҷ� �ܰ��� �׸� )
		if (!USE_ALG_CONTOURS)	// ��� �ܰ��� ����
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// ��� �ܰ��� �׸���
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
	}

	// �޸� ����
	matSrcBuffer.release();
	matDstBuffer[E_DEFECT_COLOR_DARK].release();
	matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}


long CInspectMura4::LogicStart_SPOT(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath)
{
	int nAlgImgNum				= nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int nresize					= (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_RESIZE];
	int nblur_size				= (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_BLUR_SIZE];
	int nblur_sigma				= (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_BLUR_SIGMA];

	float fBrightRatio_RGB		= (float)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_RATIO];
	float fBrightRatio_RGB_Edge = (float)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_EDGE_RATIO];

	int nSegX					= (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_SEG_X];
	int nSegY					= (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_SEG_Y];

	float fDarkRatio_RGB		= (float)dPara[E_PARA_AVI_MURA4_WHITE_JUDGE_SPOT_ACTIVE_DARK_RATIO];
	float fDarkRatio_RGB_Edge	= (float)dPara[E_PARA_AVI_MURA4_WHITE_JUDGE_SPOT_ACTIVE_DARK_EDGE_RATIO];
	int   nDark_Minimum_Size = (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_ACTIVE_DARK_MINIMUM_SIZE];				//choi_2104

	float fContrast				= (float)dPara[E_PARA_AVI_MURA4_WHITE_JUDGE_SPOT_ACTIVE_DARK_CONTRAST];
	int nInspect_Area			= (int)dPara[E_PARA_AVI_MURA4_WHITE_JUDGE_SPOT_DARK_INSPECTAREA];
	int nRignt_edge_offset		= (float)dPara[E_PARA_AVI_MURA4_WHITE_JUDGE_SPOT_ACTIVE_DARK_RIGHT_EDGE_OFFSET];

	int nMexican_size = (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_MEXICAN_FILTER_SIZE];						//choi_2104
	int nMxblur_size = (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_MEXICAN_BLUR_SIZE];							//choi_2104
	int nMxblur_sigma = (int)dPara[E_PARA_AVI_MURA4_GRAY_JUDGE_SPOT_MEXICAN_BLUR_SIGMA];						//choi_2104
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

	//////////////////////////////////////////////////////////////////////////
	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot Start."));
	// ���� �Ҵ� & �ʱ�ȭ
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// �˻� ����
	CRect rectTemp(rectROI);

	long	nWidth = (long)matSrcImage.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcImage.rows;	// ���� ���� ũ��

												////////////////////////////////////////////////////////////////////////// choi 06.24 ����
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
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROIBuf);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot Buf Set."));

	int nResizeWidth = matSrcROIBuf.cols / nresize;
	int nResizeHeight = matSrcROIBuf.rows / nresize;
	cv::Mat matResize = cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type());

	cv::resize(matSrcROIBuf, matResize, cv::Size(nResizeWidth, nResizeHeight), 3, 3, INTER_AREA);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot Resize."));

	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_resize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResize);		//matResult �� �����Ұ�
	}

	int var_brightness1 = 0;
	double var_contrast1 = fContrast; // ������ �ٲ����
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

	cv::Mat temp1 = cMatBufTemp.GetMat(matResize.size(), matResize.type());
	cv::Mat dst1 = cMatBufTemp.GetMat(matResize.size(), matResize.type());
	matResize.copyTo(temp1);

	temp1.convertTo(dst1, CV_8U, c, d);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Contrast."));

	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_MURA4_contrast.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, dst1);		
	}


	//cv::Mat tt = dst1.clone();
	//matResize = tt.clone();

	cv::GaussianBlur(dst1, matResize, cv::Size(nblur_size, nblur_size), nblur_sigma, nblur_sigma);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Blur."));

	nErrorCode = C_Mexican_filter(matResize, nMexican_size,nMxblur_size,nMxblur_sigma);
	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot CM Filter."));

	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_Mexican_filter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResize);
	}


	///////////////////////////
	cv::Mat OutTestmat = cMatBufTemp.GetMat(matResize.size(), matResize.type());
	matResize.copyTo(OutTestmat);

	MinimumFilter(OutTestmat, OutTestmat, nDark_Minimum_Size);
	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot MinimumFilter"));

	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_Minimum_filter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, OutTestmat);
	}

	//float	fDarkRatio_RGB = 2.0;
	//float	fBrightRatio_RGB = 1.1;
	//float	fDarkRatio_RGB_Edge = 2.0;
	//float	fBrightRatio_RGB_Edge = 1.2;

	CRect resize_Rect(0, 0, OutTestmat.cols - 1, OutTestmat.rows - 1);
	RangeAvgThreshold_Gray(OutTestmat, OutTestmat, resize_Rect, 1, nSegX, nSegY, fDarkRatio_RGB, fBrightRatio_RGB, fDarkRatio_RGB_Edge, fBrightRatio_RGB_Edge, 0, &cMatBufTemp); //choi 05.01
	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot RangeTH"));

	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_RANGE_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, OutTestmat);
	}

	cv::morphologyEx(OutTestmat, matResize, MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(3, 3)));
	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot Morph"));
	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_Morp.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResize);		//matResult �� �����Ұ�
	}


	
	for (int i = 0; i<matResize.rows; i++)
	{
		for (int j = 0; j<matResize.cols; j++)
		{
			// ������ �ֿܰ� ����� (���� �ҷ� �����ϴ� ���뿡�� �̰� �ǹ̰� �ִ���? ���ȳ�...)
			if (j > matResize.cols - nRignt_edge_offset) 
			{
				matResize.data[i*matResize.cols + j] = 0;

			}
			// �˻翵�� ����... �ϴ� �ӽ÷� �˻� �� ������ 0���� �����..
			if (j < matResize.cols - (matResize.cols / nInspect_Area))
			{
				matResize.data[i*matResize.cols + j] = 0;

			}
		}
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot Edge Del"));

	cv::resize(matResize, matBrROIBuf, cv::Size(matSrcROIBuf.cols, matSrcROIBuf.rows), 3, 3, INTER_AREA);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot Resize"));

	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_resize_INV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matBrROIBuf);
	}

	if (!matBkROIBuf.empty())
	{
		cv::subtract(matBrROIBuf, matBkROIBuf, matBrROIBuf);
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA4_%02d_BG_SUB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matBrROIBuf);
	}
	
	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Mura4 Logic_Spot BG Sub"));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}


// 8bit & 12bit ���� ����
long CInspectMura4::ImageSave(CString strPath, cv::Mat matSrcBuf)
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

bool CInspectMura4::OrientedBoundingBox(cv::RotatedRect& rect1, cv::RotatedRect& rect2)
{
	// Distance Vector
	cv::Point2d dist, unit, vec[4];
	dist.x = rect1.center.x - rect2.center.x;
	dist.y = rect1.center.y - rect2.center.y;

	// rect1 Height Vector
	vec[0].x = rect1.size.height * cos((rect1.angle - 90.0) / 180.0 * PI) / 2.0;
	vec[0].y = rect1.size.height * sin((rect1.angle - 90.0) / 180.0 * PI) / 2.0;

	// rect2 Height Vector
	vec[1].x = rect2.size.height * cos((rect2.angle - 90.0) / 180.0 * PI) / 2.0;
	vec[1].y = rect2.size.height * sin((rect2.angle - 90.0) / 180.0 * PI) / 2.0;

	// rect1 Width Vector
	vec[2].x = rect1.size.width * cos(rect1.angle / 180.0 * PI) / 2.0;
	vec[2].y = rect1.size.width * sin(rect1.angle / 180.0 * PI) / 2.0;

	// rect2 Width Vector
	vec[3].x = rect2.size.width * cos(rect2.angle / 180.0 * PI) / 2.0;
	vec[3].y = rect2.size.width * sin(rect2.angle / 180.0 * PI) / 2.0;

	// 4�� ���� ��...
	bool bRes = true;
	for (int i = 0; i<4; i++)
	{
		double dSum = 0;

		double dSize = sqrt(vec[i].x * vec[i].x + vec[i].y * vec[i].y);
		unit.x = (double)(vec[i].x / dSize);
		unit.y = (double)(vec[i].y / dSize);

		// 4�� ���� ��...
		for (int j = 0; j<4; j++)
			dSum += abs(unit.x * vec[j].x + unit.y * vec[j].y);

		// �ϳ��� �и��Ҽ� �ִ� ��찡 �����ϴ� ���
		if (abs(unit.x * dist.x + unit.y * dist.y) > dSum)
		{
			bRes = false;
			break;
		}
	}

	return bRes;
}

void CInspectMura4::Filter8(BYTE* InImg, BYTE* OutImg, int nMin, int nMax, int width, int height)
{
	int i, j, k, m;
	long nmax, nmin, nsum;
	double dvalue;

	int coef[8] = { 13,14,20,14,7,-4,-7,-1 };
	//	int coef[5]={48, 22,	20,	-2,	-8};
	//	int coef[3]={12, 1, -3};
	//	int coef[7]={6,	7,	9,	5,	2,	-3,	-2};
	//	int coef[4]={10, 18, -1, -3};
	//int coef[4] = { 42, 14, 7, -7 };
	//	int coef[16]={2, 6, 10, 15, 20, 13, 14, 18, 8, 2, -2, -3, -5,-5, -3, -2};
	//	int coef[26]={ 12,	22,	32,	41,	51,	60,	50,	45,	40,	42,	40,	35,	34,	20,	11,	11,	7,	-8,	-14,	-15,	-11,	-12,	-7,	-10,	-4,	-8};
	nmax = nMax;
	nmin = nMin;
	long* NewImg = new long[(width + 1)*(height + 1)];
	long* NewLoG = new long[(width + 1)*(height + 1)];

	//	memcpy(NewImg,InImg, width*height);
	memset(NewLoG, (long)0, width*height);


	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			NewImg[(i + 1)*(width + 1) + j + 1] = (long)InImg[i*width + j];
		}
		NewImg[(i + 1)*(width + 1)] = 0; // �̰� �߸� �Ȱ� ����. NewImg[i * width ] = 0;  �̰� �´µ�? 
	}
	for (j = 0; j <= width; j++)
		NewImg[j] = 0;


	m = 8;

	width++; // width = width +1;
	height++; // height = height +1;
	for (j = 1; j < width; j++) NewImg[j] += NewImg[j - 1]; // ù��° �����ٿ� ���� �־��ִµ�. �� ���� ��� 0�ε� �� �ִ� �ɱ�? �ٷ� ������ �̹� 0���� �ʱ�ȭ �ϰ��ִµ�? �� �Ȱ��� ������ �ϰ� ����. �ǹ� ���� ����

	for (i = 1; i < height; i++) NewImg[i*width] += NewImg[(i - 1)*width]; // �̰͵� ��ܿ� NewImg[i * width ] = 0; �̷��� �������ָ� �ǹ� ���� �ڵ���

	for (i = 1; i < height; i++)
		for (j = 1; j < width; j++)
			NewImg[i*width + j] += NewImg[(i - 1)*width + j] + NewImg[i*width + (j - 1)] - NewImg[(i - 1)*width + (j - 1)];

	// 	nmax = -10000000; //�ʿ� ���� ����
	// 	nmin = 10000000;
	//	 	nmax=-12000; //�ʿ� ���� ����
	//	 	nmin= 12000;


	for (i = m + 1; i < height - m; i++)
		for (j = m + 1; j < width - m; j++)
		{
			nsum = 0;

			//13,14,20,14,7,-4,-7,-1

			for (k = 0; k < m; k++)
				nsum += (long)(coef[k] * (NewImg[(i + k)*width + (j + k)] - NewImg[(i - (k + 1))*width + (j + k)] - NewImg[(i + k)*width + (j - (k + 1))] + NewImg[(i - (k + 1))*width + (j - (k + 1))]));
			//���簢�� ��翡�� 1,3 ��и� �տ� 2,4 ��и����� ���� �����ٰ� ����� ���Ͽ� 3��и� ������ ��ü ��.

			NewLoG[i*width + j] = nsum;

			if (NewLoG[i*width + j] > nmax) nmax = NewLoG[i*width + j]; //NewLoG[i*width+j] = nmax; 
			if (NewLoG[i*width + j] < nmin) nmin = NewLoG[i*width + j];

		}
	// 
	// 	nmax = 8000; // Min, Max ���� �ö󰡸� ��� ��Ⱑ ������ (Gray scale)
	// 	nmin = -8000;
	// 		nmax= 10000;
	// 		nmin=-10000;
	// min ���� max ���� �ٸ� �� �ִ�.
	for (i = 1; i < height; i++)
	{
		for (j = 1; j < width; j++)
		{
			if (i >= m && i < height - m && j >= m && j < width - m) {
				dvalue = ((double)((NewLoG[i*width + j] - nmin) * 255) / (double)(nmax - nmin));
				if (dvalue < 0.0f)			OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)0;
				else if (dvalue > 255.0f)	OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)255;
				else					OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)dvalue;
			}
			else						OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)127;

		}
	}

	delete[] NewImg;
	delete[] NewLoG;


}

long  CInspectMura4::C_Mexican_filter(cv::Mat& ip, int sz, int nMx_blur_sz, int nMx_blur_sigma) {
	int r = (sz - 1) / 2;
	// 	float* kernx = gauss1D(r);
	// 	float* kern_diff = diff2Gauss1D(r);
	// 	//kernel = new float[3][];
	// 	kernel[0] = kernx;
	// 	kernel[1] = kern_diff;
	float* kernel2 = computeKernel2D(r);
	//	kernel[2] = kernel2;


	// 	if (sep)
	// 		convolveSep(ip, kernx, kern_diff);
	// 	else {
	//Convolver con = new Convolver();
	boolean err = true;
	err = convolveFloat(ip, kernel2, sz, sz ,sz);
	//}
	double sigma2 = (sz - 1) / 6.0;
	sigma2 *= sigma2;
	ip *= sigma2;

	cv::GaussianBlur(ip, ip, cv::Size(nMx_blur_sz, nMx_blur_sz), nMx_blur_sigma, nMx_blur_sigma);

	free(kernel2);

	if (err) {
		return E_ERROR_CODE_TRUE;
	}
	else {
		return E_ERROR_CODE_MURA_WRONG_PARA;
	}
}

// float*  CInspectMura4::gauss1D(int r) {
// 	sz = 2 * r + 1;
// 	//final double sigma2=2*((double)r/3.5 +1/7.0)*((double)r/3.5 +1/7.0);
// 	double sigma2 = ((double)r / 3.0 + 1 / 6)*((double)r / 3.0 + 1 / 6.0);
// 	float* kernel;
// 	kernel = (float *)malloc(sz * sizeof(float));
// 	float sum = 0;
// 	double PIs = 1 / sqrt(2 * PI*sigma2);
// 	for (int u = -r; u <= r; u++) {
// 		double x2 = u*u;
// 		int idx = u + r;
// 		kernel[idx] = (float)(exp(-0.5*x2 / sigma2)*PIs);
// 
// 	}
// 	sum = abs(sum);
// 	if (sum < 1e-5) sum = 1;
// 	if (sum != 1) {
// 		for (int i = 0; i < sz; i++) {
// 			kernel[i] /= sum;
// 			//System.out.print(kernel[i] +" ");
// 		}
// 	}
// 	return kernel;
// }


float*  CInspectMura4::diff2Gauss1D(int r) {
	int sz = 2 * r + 1;
	double sigma2 = ((double)r / 3.0 + 1 / 6)*((double)r / 3.0 + 1 / 6.0);
//	int *num;

	float* kernel;
	kernel = (float *)malloc(sz * sizeof(float));
	//((w^2-r^2)*%e^(-r^2/(2*w^2)))/(2^(3/2)*sqrt(%pi)*w^4*abs(w))
	float sum = 0;
	double PIs = 1 / sqrt(2 * PI*sigma2);
	for (int u = -r; u <= r; u++) {
		double x2 = u*u;
		int idx = u + r;
		kernel[idx] = (float)((x2 - sigma2)*exp(-0.5*x2 / sigma2)*PIs);

	}
	sum = abs(sum);
	if (sum < 1e-5) sum = 1;
	if (sum != 1) {
		for (int i = 0; i < sz; i++) {
			kernel[i] /= sum;
			//System.out.print(kernel[i] +" ");
		}
	}
	return kernel;
}

float*  CInspectMura4::computeKernel2D(int r) {
	int sz = 2 * r + 1;
	double sigma2 = 2 * ((double)r / 3.0 + 1 / 6.0)*((double)r / 3.0 + 1 / 6.0);
	float* kernel;
	kernel = (float *)malloc(sz*sz * sizeof(float));
	double PIs = 4 / sqrt(PI*sigma2) / sigma2 / sigma2;
	float sum = 0;
	for (int u = -r; u <= r; u++) {
		for (int w = -r; w <= r; w++) {
			double x2 = u*u + w*w;
			int idx = u + r + sz*(w + r);
			kernel[idx] = (float)((x2 - sigma2)*exp(-x2 / sigma2)*PIs);
			///System.out.print(kernel[c] +" ");		
			sum += kernel[idx];

		}
	}
	sum = abs(sum);
	if (sum < 1e-5) sum = 1;
	if (sum != 1) {
		for (int i = 0; i < sz*sz; i++) {
			kernel[i] /= sum;
			//System.out.print(kernel[i] +" ");
		}
	}
	return kernel;
}

boolean  CInspectMura4::convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh, int sz) {

	//if (canceled) return false;
	int width = ip.cols;
	int height = ip.rows;
	Rect r(0, 0, width, height);
	int x1 = r.x;
	int y1 = r.y;
	int x2 = x1 + r.width;
	int y2 = y1 + r.height;
	int uc = kw / 2;
	int vc = kh / 2;
	//float pixels = width*height;

	cv::Mat pixels;
	cv::Mat pixels2;
	pixels = ip;
	ip.copyTo(pixels2);

	double scale = getScale(kernel, sz);
	// 	Thread thread = Thread.currentThread();
	// 	boolean isMainThread = thread == mainThread || thread.getName().indexOf("Preview") != -1;
	// 	if (isMainThread) pass++;

	bool e_size = true;

	double sum;
	int offset, i;
	boolean edgePixel;
	int xedge = width - uc;
	int yedge = height - vc;
	//long lastTime = System.currentTimeMillis();

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int y = y1; y < y2; y++) {
		// 		long time = System.currentTimeMillis();
		// 		if (time - lastTime > 100) {
		// 			lastTime = time;
		// 			if (thread.isInterrupted()) return false;
		// 			if (isMainThread) {
		// 				if (IJ.escapePressed()) {
		// 					canceled = true;
		// 					ip.reset();
		// 					ImageProcessor originalIp = imp.getProcessor();
		// 					if (originalIp.getNChannels() > 1)
		// 						originalIp.reset();
		// 					return false;
		// 				}
		// 				showProgress((y - y1) / (double)(y2 - y1));
		// 			}
		// 		}
		for (int x = x1; x < x2; x++) {
			//if (canceled) return false;
			sum = 0.0;
			i = 0;
			edgePixel = y < vc || y >= yedge || x < uc || x >= xedge;
			for (int v = -vc; v <= vc; v++) {
				offset = x + (y + v)*width;
				for (int u = -uc; u <= uc; u++) {
					if (edgePixel) {
						if (i >= sz*sz) {
							e_size = false;
							break;
						}

						int x_r = x + u;
						int y_r = y + v;

						if (x_r < 0) { x_r = 0; }
						if (x_r > width - 1) { x_r = width - 1; }
						if (y_r < 0) { y_r = 0; }
						if (y_r > height - 1) { y_r = height - 1; }

						sum += pixels2.at<uchar>(y_r, x_r)*kernel[i++];
					}
					else {

						sum += pixels2.data[offset + u] * kernel[i++];
					}
				}
			}
			int input = sum*scale;
			if (input < 0) { input = 0; }
			if (input > 255) { input = 255; }
			pixels.data[x + y*width] = input;
		}
		if (!e_size) {
			break;
		}
	}

	if (e_size) {
		return true;
	}
	else {
		return false;
	}

}

double CInspectMura4::getScale(float* kernel, int sz) {
	double scale = 1.0;
	double sum = 0.0;
	for (int i = 0; i < sz*sz; i++)
		sum += kernel[i];
	if (sum != 0.0)
		scale = 1.0 / sum;
	return scale;
}

long CInspectMura4::RangeAvgThreshold_Gray(cv::Mat& matSrcImage, cv::Mat& matDstImage, CRect rectROI,
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
			GetHistogram(matTempBuf, matHisto);

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


// void CInspectMura4::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
// {
// 	rectInspROI.x = rectROI.left - nOffset;
// 	rectInspROI.y = rectROI.top - nOffset;
// 	rectInspROI.width = rectROI.Width() + nOffset * 2;
// 	rectInspROI.height = rectROI.Height() + nOffset * 2;
// 
// 	if (rectInspROI.x < 0) rectInspROI.x = 0;
// 	if (rectInspROI.y < 0) rectInspROI.y = 0;
// 	if (rectInspROI.width > nWidth - rectInspROI.x) rectInspROI.width = nWidth - rectInspROI.x;
// 	if (rectInspROI.height > nHeight - rectInspROI.y) rectInspROI.height = nHeight - rectInspROI.y;
// }

long CInspectMura4::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if (nBit == -1)	return E_ERROR_CODE_LINE_HISTO;

	int nHistSize = (int)pow((double)256, (double)nBit);
	float fHiStronge[] = { 0, (float)nHistSize };
	const float *ranges[] = { fHiStronge };

	cv::calcHist(&matSrcImage, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}


int CInspectMura4::GetBitFromImageDepth(int nDepth)
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

long CInspectMura4::GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev)
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

long CInspectMura4::GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage)
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

void CInspectMura4::MinimumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
{
	Mat prc;
	src.copyTo(prc);

	if (nMaskSize % 2 == 0)
		nMaskSize++;

	int nCols = src.cols;
	int nRows = src.rows;
	int nStep = src.step;

	int nMaskX_ST, nMaskY_ST, nMask_ST;

	nMask_ST = nMaskSize / 2 * -1;
	int nMin;

	uchar* ucSrcdata;
	uchar* ucDstdata;

	for (int nY = 0; nY < nRows; nY++)
	{
		if (nY + nMask_ST < 0 || nY + abs(nMask_ST) > nRows - 1)
			continue;
		ucDstdata = prc.ptr(nY);
		for (int nX = 0; nX < nCols; nX++)
		{
			if (nX + nMask_ST < 0 || nX + nMask_ST > nCols - 1)
				continue;

			nMin = 255;

			nMaskY_ST = nMask_ST;
			for (int nMaskY = 0; nMaskY < nMaskSize; nMaskY++)
			{
				int nY_ = nY + nMaskY_ST;
				ucSrcdata = src.ptr(nY_);

				nMaskX_ST = nMask_ST;
				for (int nMaskX = 0; nMaskX < nMaskSize; nMaskX++)
				{
					int nX_ = nX + nMaskX_ST;
					if ((int)ucSrcdata[nX_] < nMin)
						nMin = (int)ucSrcdata[nX_];
					nMaskX_ST++;
				}
				nMaskY_ST++;
			}
			ucDstdata[nX] = (uchar)nMin;
		}
	}

	prc.copyTo(dst);
}

void CInspectMura4::MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
{
	Mat prc;
	src.copyTo(prc);

	if (nMaskSize % 2 == 0)
		nMaskSize++;

	int nCols = src.cols;
	int nRows = src.rows;
	int nStep = src.step;

	int nMaskX_ST, nMaskY_ST, nMask_ST;

	nMask_ST = nMaskSize / 2 * -1;
	int nMax;

	uchar* ucSrcdata;
	uchar* ucDstdata;

	for (int nY = 0; nY < nRows; nY++)
	{
		if (nY + nMask_ST < 0 || nY + abs(nMask_ST) > nRows - 1)
			continue;
		ucDstdata = prc.ptr(nY);
		for (int nX = 0; nX < nCols; nX++)
		{
			if (nX + nMask_ST < 0 || nX + nMask_ST > nCols - 1)
				continue;

			nMax = 0;

			nMaskY_ST = nMask_ST;
			for (int nMaskY = 0; nMaskY < nMaskSize; nMaskY++)
			{
				int nY_ = nY + nMaskY_ST;
				ucSrcdata = src.ptr(nY_);

				nMaskX_ST = nMask_ST;
				for (int nMaskX = 0; nMaskX < nMaskSize; nMaskX++)
				{
					int nX_ = nX + nMaskX_ST;
					if ((int)ucSrcdata[nX_] > nMax)
						nMax = (int)ucSrcdata[nX_];
					nMaskX_ST++;
				}
				nMaskY_ST++;
			}
			ucDstdata[nX] = (uchar)nMax;
		}
	}

	prc.copyTo(dst);
}

//////////////////////////////////////////////////////////////////////////choi 04.26
bool CInspectMura4::cMeanFilte(cv::Mat matActiveImage, cv::Mat& matDstImage)
{
	try
	{
		cv::Scalar mean, std;
		cv::meanStdDev(matActiveImage, mean, std);
		double sub;
		double com;
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
		for (int i = 0; i < matActiveImage.rows*matActiveImage.cols; i++) {

			sub = mean[0] - matActiveImage.data[i];
			sub /= 2.0;
			com = matActiveImage.data[i] + sub;
			if (com < 0) { matActiveImage.data[i] = 0; }
			else if (com > 254) { matActiveImage.data[i] = 255; }
			else { matActiveImage.data[i] = com; }
		}
		matActiveImage.copyTo(matDstImage);

		//cv::medianBlur(matActiveImage, matActiveImage, 5)
	}
	catch (cv::Exception& e)
	{
		const char* err_msg = e.what();
		cout << "Exception Caught: " << err_msg << endl;

		return false;
	}

	return true;
}

// long CInspectMura4::JudgeWhiteSpot(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
// 	stDefectInfo* pResultBlob, CMatBuf* cMemSub)
// {
// 	// �ҷ��� ���� ��� ������
// 	int nCount = pResultBlob->nDefectCount;
// 	if (nCount <= 0)		return E_ERROR_CODE_TRUE;
// 
// 	// ��� �Ķ����
// 	int		nMorpObj = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_OBJ];
// 	int		nMorpBKG = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_BKG];
// 	int		nThreshold = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_THRESHOLD];
// 
// 	// Active Spec
// 	double	dSpecActiveBrightRatio = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_BRIGHT_RATIO];
// 
// 	//////////////////////////////////////////////////////////////////////////spec1
// 	int		nSpec1_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC1_FLAG];
// 	double	dSpecActiveDarkRatio1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_1];
// 	double	dSpecActiveDarkArea1_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MIN];
// 	double	dSpecActiveDarkArea1_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MAX];
// 	double	dSpecActiveDarkDiff1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_1];
// 
// 	//////////////////////////////////////////////////////////////////////////spec2
// 	int		nSpec2_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC2_FLAG];
// 	double	dSpecActiveDarkRatio2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_2];
// 	double	dSpecActiveDarkArea2_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MIN];
// 	double	dSpecActiveDarkArea2_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MAX];
// 	double	dSpecActiveDarkDiff2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_2];
// 
// 	//////////////////////////////////////////////////////////////////////////spec3
// 	int		nSpec3_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC3_FLAG];
// 	double	dSpecActiveDarkRatio3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_3];
// 	double	dSpecActiveDarkArea3_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MIN];
// 	double	dSpecActiveDarkArea3_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MAX];
// 	double	dSpecActiveDarkDiff3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_3];
// 
// 	//////////////////////////////////////////////////////////////////////////spec4
// 	int		nSpec4_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC4_FLAG];
// 	double	dSpecActiveDarkRatio4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_4];
// 	double	dSpecActiveDarkArea4_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MIN];
// 	double	dSpecActiveDarkArea4_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MAX];
// 	double	dSpecActiveDarkDiff4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_4];
// 
// 	// Edge ���� ����
// 	double	dSpecEdgeAreaL = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_LEFT];
// 	double	dSpecEdgeAreaT = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_TOP];
// 	double	dSpecEdgeAreaR = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_RIGHT];
// 	double	dSpecEdgeAreaB = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_BOTTOM];
// 
// 	// ����ó��
// 	if (dSpecEdgeAreaL < 0)				dSpecEdgeAreaL = 0;
// 	if (dSpecEdgeAreaT < 0)				dSpecEdgeAreaT = 0;
// 	if (dSpecEdgeAreaR < 0)				dSpecEdgeAreaR = 0;
// 	if (dSpecEdgeAreaB < 0)				dSpecEdgeAreaB = 0;
// 
// 	// Edge Spec
// 	double	dSpecEdgeBrightRatio = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_BRIGHT_RATIO];
// 
// 	//////////////////////////////////////////////////////////////////////////SPEC1
// 	int		nSpec1_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC1_FLAG];
// 	double	dSpecEdgeDarkRatio1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_1];
// 	double	dSpecEdgeDarkArea1_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MIN];
// 	double	dSpecEdgeDarkArea1_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MAX];
// 	double	dSpecEdgeDarkDiff1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_1];
// 
// 	//////////////////////////////////////////////////////////////////////////SPEC2
// 	int		nSpec2_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC2_FLAG];
// 	double	dSpecEdgeDarkRatio2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_2];
// 	double	dSpecEdgeDarkArea2_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MIN];
// 	double	dSpecEdgeDarkArea2_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MAX];
// 	double	dSpecEdgeDarkDiff2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_2];
// 
// 	//////////////////////////////////////////////////////////////////////////SPEC3
// 	int		nSpec3_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC3_FLAG];
// 	double	dSpecEdgeDarkRatio3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_3];
// 	double	dSpecEdgeDarkArea3_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MIN];
// 	double	dSpecEdgeDarkArea3_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MAX];
// 	double	dSpecEdgeDarkDiff3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_3];
// 
// 	//////////////////////////////////////////////////////////////////////////SPEC4
// 	int		nSpec4_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC4_FLAG];
// 	double	dSpecEdgeDarkRatio4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_4];
// 	double	dSpecEdgeDarkArea4_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MIN];
// 	double	dSpecEdgeDarkArea4_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MAX];
// 	double	dSpecEdgeDarkDiff4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_4];
// 	//////////////////////////////////////////////////////////////////////////
// 	// ���� �Ķ����
// 	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
// 	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
// 	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
// 	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
// 	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
// 	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
// 	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
// 	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
// 
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	// ���� �Ҵ� & �ʱ�ȭ
// 	CMatBuf cMatBufTemp;
// 	cMatBufTemp.SetMem(cMemSub);
// 
// 	long	nWidth = (long)matSrcBuffer.cols;	// ���� ���� ũ��
// 	long	nHeight = (long)matSrcBuffer.rows;	// ���� ���� ũ��
// 
// 	int nOffset = 200;
// 	cv::Mat matDefectMorp1Buf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
// 	cv::Mat matDefectMorp2Buf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
// 	cv::Mat matDefectBKBuf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
// 	cv::Mat matDefectThBuf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
// 
// 	// �ҷ� ���� ��ŭ
// 	for (int i = 0; i < nCount; i++)
// 	{
// 		// Spot �ҷ��� �ƴ� ��� �Ѿ��
// 		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT && pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) //04.16 choi
// 			continue;
// 
// 		// �ҷ� �ֺ� ����
// 		cv::Rect rectTempROI(pResultBlob->nCenterx[i] - nOffset / 2, pResultBlob->nCentery[i] - nOffset / 2, nOffset, nOffset);
// 		if (rectTempROI.x < 0)						rectTempROI.x = 0;
// 		if (rectTempROI.y < 0)						rectTempROI.y = 0;
// 		if (rectTempROI.x + nOffset >= nWidth)		rectTempROI.x = nWidth - nOffset - 1;
// 		if (rectTempROI.y + nOffset >= nHeight)	rectTempROI.y = nHeight - nOffset - 1;
// 
// 		// �ҷ� ����
// 		cv::Mat matDefectSrcBuf = matSrcBuffer(rectTempROI);
// 		cv::Mat matTempBuf = matDstBuffer(rectTempROI);
// 
// 		// ���� �ϱ� ���� ����ȭ ( ���� : 255 / �鹫�� 200 )
// 		cv::Mat matDefectResBuf = cMatBufTemp.GetMat(matTempBuf.size(), matTempBuf.type(), false);
// 		cv::threshold(matTempBuf, matDefectResBuf, 220, 255.0, CV_THRESH_BINARY);
// 
// 		if (bImageSave)
// 		{
// 			// 			CString strTemp;
// 			// 			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Src_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
// 			// 			ImageSave(strTemp, matDefectSrcBuf);
// 			// 
// 			// 			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Res_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
// 			// 			ImageSave(strTemp, matDefectResBuf);
// 		}
// 
// 		// �ҷ� ���� ũ�� ����
// 		int nValue = nMorpObj * 2 + 1;
// 		if (nMorpObj >= 1)
// 			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
// 		else
// 			matDefectResBuf.copyTo(matDefectMorp1Buf);
// 
// 		// ��� �κ�
// 		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf);
// 
// 		// ��ο� �κ� ã��
// 		cv::threshold(matDefectSrcBuf, matDefectThBuf, 20, 255.0, THRESH_BINARY_INV);
// 
// 		// ��ο� �κ� ����
// 		cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp1Buf);	// ����
// 		cv::subtract(matDefectBKBuf, matDefectThBuf, matDefectBKBuf);	// ���
// 
// 																		// ��� �κ� mean & stdDev ���ϱ�
// 		cv::Scalar meanBK, stdBK;
// 		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf);
// 
// 		// ���� �κ� ã��
// 		cv::threshold(matDefectSrcBuf, matDefectThBuf, nThreshold, 255.0, THRESH_BINARY);
// 
// 		// ���� �κ� ���� ũ�� ����
// 		nValue = nMorpBKG * 2 + 1;
// 		if (nMorpBKG >= 1)
// 			cv::morphologyEx(matDefectThBuf, matDefectMorp2Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_RECT, cv::Size(nValue, nValue)));
// 		else
// 			matDefectThBuf.copyTo(matDefectMorp2Buf);
// 
// 		// ����� �ҷ� �ȿ� ���� �κ� �ִ��� Ȯ��
// 		double valMax = 0;
// 		cv::bitwise_and(matDefectMorp1Buf, matDefectMorp2Buf, matDefectThBuf);
// 		cv::minMaxLoc(matDefectThBuf, NULL, &valMax);
// 
// 		if (bImageSave)
// 		{
// 			// 			CString strTemp;
// 			// 			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Bri_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
// 			// 			ImageSave(strTemp, matDefectThBuf);
// 		}
// 
// 		// ���� ������ �ִ��� Ȯ��
// 		bool bPoint = (valMax > 100) ? true : false;
// 
// 		// Active ������ ����
// 		CRect rectTemp;
// 		rectTemp.left = rectROI.left + dSpecEdgeAreaL;
// 		rectTemp.top = rectROI.top + dSpecEdgeAreaT;
// 		rectTemp.right = rectROI.right - dSpecEdgeAreaR;
// 		rectTemp.bottom = rectROI.bottom - dSpecEdgeAreaB;
// 
// 		// �ҷ� �߽� ��ǥ
// 		CPoint ptSrc(pResultBlob->nCenterx[i], pResultBlob->nCentery[i]);
// 
// 		// Active ���� �ȿ� Spot �ҷ� ����
// 		if (rectTemp.PtInRect(ptSrc))
// 		{
// 			// Mura Active �ҷ�
// 			pResultBlob->bMuraActive[i] = true;
// 
// 			// ���� �������� ��� ���� �� ��� Spec
// 			if (bPoint)
// 			{
// 				// Mura ���� GV ����
// 				pResultBlob->bMuraBright[i] = true;
// 
// 				// ����� �ҷ� �ȿ� ���� �κ� ���ŵ� ����
// 				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);
// 
// 				// ���� �κ� mean & stdDev ���ϱ�
// 				cv::Scalar meanObj, stdObj;
// 				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);
// 
// 				// Mura ��� ���� �Է�
// 				pResultBlob->dMuraObj[i] = meanObj[0];
// 				pResultBlob->dMuraBk[i] = meanBK[0];
// 
// 				if (meanObj[0] - meanBK[0] > dSpecActiveBrightRatio) { pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT; }
// 				else													pResultBlob->bUseResult[i] = false;
// 
// 				if (bImageSave)
// 				{
// 					// 					CString strTemp;
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_Src_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanObj[0]);
// 					// 					ImageSave(strTemp, matDefectMorp2Buf);
// 					// 
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_BK_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanBK[0]);
// 					// 					ImageSave(strTemp, matDefectBKBuf);
// 				}
// 			}
// 			// ���� �������� ��� ���� ���� ��� Spec
// 			else
// 			{
// 				// Mura ���� GV ���� ����
// 				pResultBlob->bMuraBright[i] = false;
// 
// 				// ����� �ҷ� �ȿ� ���� �κ� ���ŵ� ����
// 				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);
// 
// 				// 				//////////////////////////////////////////////////////////////////////////
// 				// 				cv:Mat test;
// 				// 				cv::bitwise_and(matDefectSrcBuf, matDefectMorp2Buf, test);
// 				// 				cv::threshold(test, matDefectMorp2Buf, pResultBlob->dMeanGV[i]*2.0, 255, THRESH_BINARY);
// 				//////////////////////////////////////////////////////////////////////////
// 				// ���� �κ� mean & stdDev ���ϱ�
// 				cv::Scalar meanObj, stdObj;
// 				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);
// 
// 				// Mura ��� ���� �Է�
// 				pResultBlob->dMuraObj[i] = meanObj[0];
// 				pResultBlob->dMuraBk[i] = meanBK[0];
// 
// 				// ���� ����
// 				double	dArea = pResultBlob->nArea[i];
// 				double	dSub = meanObj[0] - meanBK[0];
// 				double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];
// 
// 				// Spec1
// 				if (dArea >= dSpecActiveDarkArea1_MIN		&&
// 					dArea < dSpecActiveDarkArea1_MAX	&&
// 					nSpec1_Act_Flag > 0 &&
// 					dSub > dSpecActiveDarkRatio1		&&
// 					dDiff > dSpecActiveDarkDiff1)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				// Spec2
// 				else if (dArea >= dSpecActiveDarkArea2_MIN	&&
// 					dArea < dSpecActiveDarkArea2_MAX	&&
// 					nSpec2_Act_Flag > 0 &&
// 					dSub > dSpecActiveDarkRatio2		&&
// 					dDiff > dSpecActiveDarkDiff2)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				// Spec3
// 				else if (dArea >= dSpecActiveDarkArea3_MIN	&&
// 					dArea < dSpecActiveDarkArea3_MAX	&&
// 					nSpec3_Act_Flag > 0 &&
// 					dSub > dSpecActiveDarkRatio3		&&
// 					dDiff > dSpecActiveDarkDiff3)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				// Spec4
// 				else if (dArea >= dSpecActiveDarkArea4_MIN	&&
// 					dArea < dSpecActiveDarkArea4_MAX	&&
// 					nSpec4_Act_Flag > 0 &&
// 					dSub > dSpecActiveDarkRatio4		&&
// 					dDiff > dSpecActiveDarkDiff4)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				else
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT;
// 				}
// 
// 				if (bImageSave)
// 				{
// 					// 					CString strTemp;
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_Src_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanObj[0]);
// 					// 					ImageSave(strTemp, matDefectMorp2Buf);
// 					// 
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_BK_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanBK[0]);
// 					// 					ImageSave(strTemp, matDefectBKBuf);
// 				}
// 			}
// 		}
// 		// Edge ���� �ȿ� Spot �ҷ� ����
// 		else
// 		{
// 			// Mura Active �ҷ� �ƴ�
// 			pResultBlob->bMuraActive[i] = false;
// 
// 			// ���� �������� ��� ���� �� ��� Spec
// 			if (bPoint)
// 			{
// 				// Mura ���� GV ����
// 				pResultBlob->bMuraBright[i] = true;
// 
// 				// ����� �ҷ� �ȿ� ���� �κ� ���ŵ� ����
// 				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);
// 
// 				// ���� �κ� mean & stdDev ���ϱ�
// 				cv::Scalar meanObj, stdObj;
// 				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);
// 
// 				// Mura ��� ���� �Է�
// 				pResultBlob->dMuraObj[i] = meanObj[0];
// 				pResultBlob->dMuraBk[i] = meanBK[0];
// 
// 				if (meanObj[0] - meanBK[0] > dSpecEdgeBrightRatio)	pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				else												pResultBlob->bUseResult[i] = false;
// 
// 				if (bImageSave)
// 				{
// 					// 					CString strTemp;
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_Src_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanObj[0]);
// 					// 					ImageSave(strTemp, matDefectMorp2Buf);
// 					// 
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_BK_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanBK[0]);
// 					// 					ImageSave(strTemp, matDefectBKBuf);
// 				}
// 			}
// 			// ���� �������� ��� ���� ���� ��� Spec
// 			else
// 			{
// 				// Mura ���� GV ���� ����
// 				pResultBlob->bMuraBright[i] = false;
// 
// 				// ����� �ҷ� �ȿ� ���� �κ� ���ŵ� ����
// 				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);
// 
// 				// ���� �κ� mean & stdDev ���ϱ�
// 				cv::Scalar meanObj, stdObj;
// 				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);
// 
// 				// Mura ��� ���� �Է�
// 				pResultBlob->dMuraObj[i] = meanObj[0];
// 				pResultBlob->dMuraBk[i] = meanBK[0];
// 
// 				// Mura ����
// 				double	dArea = pResultBlob->nArea[i];
// 				double	dSub = meanObj[0] - meanBK[0];
// 				double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];
// 
// 				// Spec1
// 				if (dArea >= dSpecEdgeDarkArea1_MIN		&&
// 					dArea < dSpecEdgeDarkArea1_MAX		&&
// 					nSpec1_Edge_Flag > 0 &&
// 					dSub > dSpecEdgeDarkRatio1		&&
// 					dDiff > dSpecEdgeDarkDiff1)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				// Spec2
// 				else if (dArea >= dSpecEdgeDarkArea2_MIN		&&
// 					dArea < dSpecEdgeDarkArea2_MAX		&&
// 					nSpec2_Edge_Flag > 0 &&
// 					dSub > dSpecEdgeDarkRatio2			&&
// 					dDiff > dSpecEdgeDarkDiff2)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				// Spec3
// 				else if (dArea >= dSpecEdgeDarkArea3_MIN		&&
// 					dArea < dSpecEdgeDarkArea3_MAX		&&
// 					nSpec3_Edge_Flag > 0 &&
// 					dSub > dSpecEdgeDarkRatio3			&&
// 					dDiff > dSpecEdgeDarkDiff3)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				// Spec4
// 				else if (dArea >= dSpecEdgeDarkArea4_MIN		&&
// 					dArea < dSpecEdgeDarkArea4_MAX		&&
// 					nSpec4_Edge_Flag > 0 &&
// 					dSub > dSpecEdgeDarkRatio4			&&
// 					dDiff > dSpecEdgeDarkDiff4)
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				else
// 				{
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT;
// 				}
// 
// 				if (bImageSave)
// 				{
// 					// 					CString strTemp;
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_Src_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanObj[0]);
// 					// 					ImageSave(strTemp, matDefectMorp2Buf);
// 					// 
// 					// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_BK_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanBK[0]);
// 					// 					ImageSave(strTemp, matDefectBKBuf);
// 				}
// 			}
// 		}
// 	}
// 
// 	return E_ERROR_CODE_TRUE;
// }

/*8bit & 12bit ���� ����
long CInspectMura4::ImageSave(CString strPath, cv::Mat matSrcBuf)
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
}*/
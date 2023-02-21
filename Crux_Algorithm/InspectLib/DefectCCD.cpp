
/************************************************************************/
// CCD Defect ���� �ҽ�
// ������ : 17.07.10 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "DefectCCD.h"

// ������ �̻� : CCD Defect Delete
// ������ ���� : CCD Defect Offset
#define THRESH_GV	20


CDefectCCD::CDefectCCD(void)
{
	// ���� �ʱ�ȭ
	vector <tCCD_DEFECT>().swap(ptIndexsDelete);
	vector <tCCD_DEFECT>().swap(ptIndexsOffset);

	bLoad = false;
}

CDefectCCD::~CDefectCCD(void)
{
	// ���� �ʱ�ȭ
	vector <tCCD_DEFECT>().swap(ptIndexsDelete);
	vector <tCCD_DEFECT>().swap(ptIndexsOffset);

	bLoad = false;
}

// CCD Defect Load
long CDefectCCD::DefectCCDLoad(CString strFileName, CString strFileName2)
{
	// �̹� �ε� �� ���
	if( bLoad )	return E_ERROR_CODE_TRUE;

	// ���� ���� ��/�� Ȯ�� �ʿ�
	CFileFind find;
	BOOL bFindFile = FALSE;

	bFindFile = find.FindFile(strFileName);
	find.Close();

	// ���� ���� ���
	if (!bFindFile)
	{
		return E_ERROR_CODE_CCD_EMPTY_FILE;
	}

	char szFileName[256] = {0, };
	WideCharToMultiByte(CP_ACP, 0, strFileName, -1, szFileName, sizeof(szFileName), NULL, NULL);

	FILE *out = NULL;
	fopen_s(&out, szFileName, "r");

	if( !out )	return E_ERROR_CODE_TRUE;

	tCCD_DEFECT pt, ptOld;
	for (int m=0 ;  ; m++)
	{
		// �о����
		fscanf_s(out, "%d,%d,%d\n", &pt.x, &pt.y, &pt.gv);

		// ������ ���� ��� ������
		if(	pt.x == ptOld.x	&&
			pt.y == ptOld.y	)
			break;
		
		// ������ �̻� : CCD Defect Delete
		// ������ ���� : CCD Defect Offset
		if( pt.gv >= THRESH_GV )
			ptIndexsDelete.push_back( pt );
		else
			ptIndexsOffset.push_back( pt );

		// ������ ����
		ptOld = pt;
	}

	fclose(out);
	out = NULL;

	/////////////////////////////////////////////////////////////////////////////////////////
	// LJH DEAD_CCD ������ �ִ� ��� ���� �ε��Ͽ� �߰�

	bFindFile = FALSE;
	bFindFile = find.FindFile(strFileName2);
	find.Close();

	if ( bFindFile )
	{
		char szFileName[256] = {0, };
		WideCharToMultiByte(CP_ACP, 0, strFileName2, -1, szFileName, sizeof(szFileName), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szFileName, "r");

		if( !out )	return E_ERROR_CODE_TRUE;

		tCCD_DEFECT pt, ptOld;
		for (int m=0 ;  ; m++)
		{
			// �о����
			fscanf_s(out, "%d,%d,%d\n", &pt.x, &pt.y, &pt.gv);

			// ������ ���� ��� ������
			if(	pt.x == ptOld.x	&&
				pt.y == ptOld.y	)
				break;
			
			//Dead CCD�� ��� �������� �ǹ̰� �����Ƿ�..
			ptIndexsDelete.push_back( pt );

			// ������ ����
			ptOld = pt;
		}

		fclose(out);
		out = NULL;
	}
	////////////////////////////////////////////////////////////////////////////////////////

	bLoad = true;
	
	return E_ERROR_CODE_TRUE;
}

// CCD Defect Save
long CDefectCCD::DefectCCDSave(cv::Mat& matSrcBuffer, CString strFileName, CString strFileName2)
{
	// ���� ���� ���� ���
	if( matSrcBuffer.empty() )	return E_ERROR_CODE_CCD_EMPTY_BUFFER;

	// PS ������ ��� ����
	if( matSrcBuffer.cols > 10000 )		return E_ERROR_CODE_CCD_PS_BUFFER;

	// ���� Bright/Dark Ȯ�ο�
	cv::Scalar m = cv::mean(matSrcBuffer);	
	
	// ���� �ʱ�ȭ
	vector <tCCD_DEFECT>().swap(ptIndexsDelete);
	vector <tCCD_DEFECT>().swap(ptIndexsOffset);

	// LJH ���������� ����� 20�� ���� �����Ƿ�..
	if ( (double)(m[0]) < 20. )
	{
		tCCD_DEFECT pt;
		char szFileName[256] = {0, };
		WideCharToMultiByte(CP_ACP, 0, strFileName, -1, szFileName, sizeof(szFileName), NULL, NULL);

		// �����ڵ� ȯ�� ���� ���� "t" -> "wt"
		FILE *out = NULL;
		fopen_s(&out, szFileName, "wt");

		// ���� ����
		if( !out )	return E_ERROR_CODE_TRUE;

		for (int y=0; y<matSrcBuffer.rows ; y++)
		{
			BYTE *ptr = (BYTE *)matSrcBuffer.ptr(y);

			for (int x=0 ; x<matSrcBuffer.cols ; x++, ptr++)
			{
				// 0 GV�� ��� �Ѿ��
				if ( *ptr == 0 )	continue;

				pt.x	= x;
				pt.y	= y;
				pt.gv	= (*ptr);

				// ������ �̻� : CCD Defect Delete
				// ������ ���� : CCD Defect Offset
				if( pt.gv >= THRESH_GV )
					ptIndexsDelete.push_back( pt );
				else
					ptIndexsOffset.push_back( pt );

				// ���� ����
				fprintf_s(out, "%d,%d,%d\n", pt.x, pt.y, pt.gv);
			}
		}
		fclose(out);
		out = NULL;
	}
	else
	{
		tCCD_DEFECT pt;
		char szFileName[256] = {0, };
		WideCharToMultiByte(CP_ACP, 0, strFileName2, -1, szFileName, sizeof(szFileName), NULL, NULL);

		// �����ڵ� ȯ�� ���� ���� "t" -> "wt"
		FILE *out = NULL;
		fopen_s(&out, szFileName, "wt");

		// ���� ����
		if( !out )	return E_ERROR_CODE_TRUE;

		for (int y=1; y<matSrcBuffer.rows-1 ; y++)
		{
			BYTE *ptr = (BYTE *)matSrcBuffer.ptr(y);

			for (int x=1 ; x<matSrcBuffer.cols-1 ; x++, ptr++)
			{				
				// ���� ������ ��պ��� 60% ��ο�� ���
				if ( *ptr / (double)(m[0]) < 0.6 )
				{
					pt.x	= x;
					pt.y	= y;
					pt.gv	= 0;

					// ���� ����
					fprintf_s(out, "%d,%d,%d\n", pt.x, pt.y, pt.gv);
					continue;
				}
			}
		}
		fclose(out);
		out = NULL;

		AfxMessageBox(_T("Dead CCD Save OK"));
	}

	bLoad = true;

	return E_ERROR_CODE_TRUE;
}

// CCD �ҷ� ���� ���� ��������
int CDefectCCD::GetDefectCCDDeleteCount()
{
	// Load �ȵ� ���
	if( !bLoad )	return 0;

	return (int)ptIndexsDelete.size();
}

// CCD �ҷ� ���� ���� ��������
int CDefectCCD::GetDefectCCDOffsetCount()
{
	// Load �ȵ� ���
	if( !bLoad )	return 0;

	return (int)ptIndexsOffset.size();
}

// CCD �ҷ� ����
long CDefectCCD::DeleteDefectCCD(cv::Mat& matSrcBuffer, int nSize, int nPS)
{
	if( !bLoad )					return E_ERROR_CODE_CCD_NOT_LOAD;
	if( matSrcBuffer.empty()	)	return E_ERROR_CODE_CCD_EMPTY_BUFFER;
	if( nSize < 0 )					return E_ERROR_CODE_CCD_WARNING_PARA;

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i=0 ; i<GetDefectCCDDeleteCount() ; i++)
	{
		cv::Rect rect;

		// Size ����
		rect.x = ptIndexsDelete[i].x * nPS - nSize;
		rect.y = ptIndexsDelete[i].y * nPS - nSize;
		rect.width	= nSize + nSize + nPS;
		rect.height	= nSize + nSize + nPS;

		// ���������� ĥ�ϱ�
		cv::rectangle(matSrcBuffer, rect, cv::Scalar(0, 0, 0), -1);
	}

	return E_ERROR_CODE_TRUE;
}

// CCD �ҷ� �ڵ� ����
long CDefectCCD::DeleteAutoDefectCCD(cv::Mat& matSrcBuffer, float fGV, float fBkGV, int nPS, CMatBuf* cMem)
{
	if( matSrcBuffer.empty() )	return E_ERROR_CODE_CCD_EMPTY_BUFFER;

	CMatBuf cMatBufTemp;

	// CCD �ҷ� ����
	long	nCountCCD = 0;

	// PS����� ���
	if( nPS == 2 )
	{
		// Temp �޸� �Ҵ�
		cMatBufTemp.SetMem(cMem);
		cv::Mat matInBuf	= cMatBufTemp.GetMat(matSrcBuffer.size(), CV_32FC1);
		cv::Mat matOutBuf	= cMatBufTemp.GetMat(matSrcBuffer.size(), CV_32FC1);
		cv::Mat matKernel1	= cv::Mat::zeros(4, 4, CV_8UC1);
		cv::Mat matKernel2	= cv::Mat::zeros(4, 4, CV_8UC1);

		// Kernel �����
		for (int y=0 ; y<4 ; y++)
		{
			for (int x=0 ; x<4 ;x++)
			{
				if( x==0	||
					x==3	||
					y==0	||
					y==3	)
				{
					// matKernel1
					matKernel1.at<uchar>(y, x) = (uchar)1;
				}
				else
				{
					// matKernel2
					matKernel2.at<uchar>(y, x) = (uchar)1;
				}
			}
		}

		// �ܺ� GV ���ϱ�
		cv::filter2D(matSrcBuffer, matOutBuf, CV_32FC1, matKernel1);

		// ���� GV ���ϱ�
		cv::filter2D(matSrcBuffer, matInBuf, CV_32FC1, matKernel2);

		// CCD �ҷ����� �ڵ� ����
		for (int y=1 ; y<matSrcBuffer.rows ; y++)
		{
			float *ptrIn	= (float *)matInBuf.ptr(y);
			float *ptrOut	= (float *)matOutBuf.ptr(y);

			ptrIn++;
			ptrOut++;

			for (int x=1 ; x<matSrcBuffer.cols ; x++, ptrIn++, ptrOut++)
			{
				if( *ptrIn >= fGV		&&	// ���� ��� ? �̻�
					*ptrOut <= fBkGV	)	// �ܺ� ��� ? ���� ( Black ������ ����ϹǷ�... )
				{
					// ���� ��Ⱑ ������� �ʴ� ��쵵 ������...
					cv::rectangle(matSrcBuffer, cv::Rect(x-1, y-1, 2, 2), cv::Scalar(0, 0, 0), -1 );

					// CCD �ҷ� ����
					nCountCCD++;
				}
			}
		}

		// �Ҵ� ����
		matInBuf.release();
		matOutBuf.release();
		matKernel1.release();
		matKernel2.release();

		clock_t tBeforeTime = cInspectLibLog.writeInspectLog(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("DeleteAutoDefectCCD End."));

		SetLog(&cInspectLibLog, tBeforeTime, tBeforeTime, NULL);

// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 		}
	}
	// PS��尡 �ƴ� ���
	else if( nPS == 1 )
	{
		// Temp �޸� �Ҵ�
		cMatBufTemp.SetMem(cMem);
		cv::Mat matOutBuf	= cMatBufTemp.GetMat(matSrcBuffer.size(), CV_32FC1);
		cv::Mat matKernel	= cv::Mat::ones(3, 3, CV_8UC1);
		matKernel.at<uchar>(1, 1) = (uchar)0;

		// �ܺ� GV ���ϱ�
		cv::filter2D(matSrcBuffer, matOutBuf, CV_32FC1, matKernel);

		// CCD �ҷ����� �ڵ� ����
		for (int y=0 ; y<matSrcBuffer.rows ; y++)
		{
			uchar *ptrIn	= (uchar *)matSrcBuffer.ptr(y);
			float *ptrOut	= (float *)matOutBuf.ptr(y);

			for (int x=0 ; x<matSrcBuffer.cols ; x++, ptrIn++, ptrOut++)
			{
				if( *ptrIn >= fGV		&&	// ���� ��� ? �̻�
					*ptrOut <= fBkGV	)	// �ܺ� ��� ? ���� ( Black ������ ����ϹǷ�... )
				{
					*ptrIn = (uchar)0;

					// CCD �ҷ� ����
					nCountCCD++;
				}
			}
		}

		// �Ҵ� ����
		matOutBuf.release();
		matKernel.release();
		
		clock_t tBeforeTime = cInspectLibLog.writeInspectLog(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("DeleteAutoDefectCCD End."));

		SetLog(&cInspectLibLog, tBeforeTime, tBeforeTime, NULL);
// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 		}
	}

	//return E_ERROR_CODE_TRUE;
	return nCountCCD;
}

// CCD �ҷ� ����
long CDefectCCD::OffsetDefectCCD(cv::Mat& matSrcBuffer, int nSize, int nPS)
{
	if( !bLoad )				return E_ERROR_CODE_CCD_NOT_LOAD;
	if( matSrcBuffer.empty() )	return E_ERROR_CODE_CCD_EMPTY_BUFFER;
	if( nSize < 0 )				return E_ERROR_CODE_CCD_WARNING_PARA;

	// �̹��� ũ��
	long	nWidth	= (long)matSrcBuffer.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuffer.rows;	// ���� ���� ũ��

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i=0 ; i<GetDefectCCDOffsetCount() ; i++)
	{
		// PS��忡 ���� ����
		int nSX		= ptIndexsOffset[i].x * nPS - nSize;
		int nSY		= ptIndexsOffset[i].y * nPS - nSize;
		int nEX		= nSX + nPS + nSize + nSize;
		int nEY		= nSY + nPS + nSize + nSize;
		int nOffset	= ptIndexsOffset[i].gv;

		// ����ó��
		if(nSX < 0)			nSX = 0;
		if(nSY < 0)			nSY = 0;
		if(nEX >= nWidth)	nEX = nWidth-1;
		if(nEY >= nHeight)	nEY = nHeight-1;

		for (int y=nSY ; y<nEY ; y++)
		{
			for (int x=nSX ; x<nEX ; x++)
			{
				// Black Patern�� ����ϹǷ�...
				int nGV = matSrcBuffer.at<uchar>(y, x) - nOffset;

				// ����ó��
				if( nGV < 0 )	nGV = 0;

				// �� �ٽ� �ֱ�
				matSrcBuffer.at<uchar>(y, x) = (uchar)nGV;
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}
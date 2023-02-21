
/************************************************************************/
// CCD Defect 관련 소스
// 수정일 : 17.07.10 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "DefectCCD.h"

// 설정값 이상 : CCD Defect Delete
// 설정값 이하 : CCD Defect Offset
#define THRESH_GV	20


CDefectCCD::CDefectCCD(void)
{
	// 벡터 초기화
	vector <tCCD_DEFECT>().swap(ptIndexsDelete);
	vector <tCCD_DEFECT>().swap(ptIndexsOffset);

	bLoad = false;
}

CDefectCCD::~CDefectCCD(void)
{
	// 벡터 초기화
	vector <tCCD_DEFECT>().swap(ptIndexsDelete);
	vector <tCCD_DEFECT>().swap(ptIndexsOffset);

	bLoad = false;
}

// CCD Defect Load
long CDefectCCD::DefectCCDLoad(CString strFileName, CString strFileName2)
{
	// 이미 로드 된 경우
	if( bLoad )	return E_ERROR_CODE_TRUE;

	// 파일 존재 유/무 확인 필요
	CFileFind find;
	BOOL bFindFile = FALSE;

	bFindFile = find.FindFile(strFileName);
	find.Close();

	// 파일 없는 경우
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
		// 읽어오기
		fscanf_s(out, "%d,%d,%d\n", &pt.x, &pt.y, &pt.gv);

		// 동일한 값인 경우 나가기
		if(	pt.x == ptOld.x	&&
			pt.y == ptOld.y	)
			break;
		
		// 설정값 이상 : CCD Defect Delete
		// 설정값 이하 : CCD Defect Offset
		if( pt.gv >= THRESH_GV )
			ptIndexsDelete.push_back( pt );
		else
			ptIndexsOffset.push_back( pt );

		// 이전값 복사
		ptOld = pt;
	}

	fclose(out);
	out = NULL;

	/////////////////////////////////////////////////////////////////////////////////////////
	// LJH DEAD_CCD 파일이 있는 경우 같이 로드하여 추가

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
			// 읽어오기
			fscanf_s(out, "%d,%d,%d\n", &pt.x, &pt.y, &pt.gv);

			// 동일한 값인 경우 나가기
			if(	pt.x == ptOld.x	&&
				pt.y == ptOld.y	)
				break;
			
			//Dead CCD의 경우 오프셋이 의미가 없으므로..
			ptIndexsDelete.push_back( pt );

			// 이전값 복사
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
	// 영상 버퍼 없는 경우
	if( matSrcBuffer.empty() )	return E_ERROR_CODE_CCD_EMPTY_BUFFER;

	// PS 영상인 경우 제외
	if( matSrcBuffer.cols > 10000 )		return E_ERROR_CODE_CCD_PS_BUFFER;

	// 영상 Bright/Dark 확인용
	cv::Scalar m = cv::mean(matSrcBuffer);	
	
	// 벡터 초기화
	vector <tCCD_DEFECT>().swap(ptIndexsDelete);
	vector <tCCD_DEFECT>().swap(ptIndexsOffset);

	// LJH 차광영상의 평균이 20을 넘지 않으므로..
	if ( (double)(m[0]) < 20. )
	{
		tCCD_DEFECT pt;
		char szFileName[256] = {0, };
		WideCharToMultiByte(CP_ACP, 0, strFileName, -1, szFileName, sizeof(szFileName), NULL, NULL);

		// 유니코드 환경 에러 수정 "t" -> "wt"
		FILE *out = NULL;
		fopen_s(&out, szFileName, "wt");

		// 파일 오류
		if( !out )	return E_ERROR_CODE_TRUE;

		for (int y=0; y<matSrcBuffer.rows ; y++)
		{
			BYTE *ptr = (BYTE *)matSrcBuffer.ptr(y);

			for (int x=0 ; x<matSrcBuffer.cols ; x++, ptr++)
			{
				// 0 GV인 경우 넘어가기
				if ( *ptr == 0 )	continue;

				pt.x	= x;
				pt.y	= y;
				pt.gv	= (*ptr);

				// 설정값 이상 : CCD Defect Delete
				// 설정값 이하 : CCD Defect Offset
				if( pt.gv >= THRESH_GV )
					ptIndexsDelete.push_back( pt );
				else
					ptIndexsOffset.push_back( pt );

				// 파일 쓰기
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

		// 유니코드 환경 에러 수정 "t" -> "wt"
		FILE *out = NULL;
		fopen_s(&out, szFileName, "wt");

		// 파일 오류
		if( !out )	return E_ERROR_CODE_TRUE;

		for (int y=1; y<matSrcBuffer.rows-1 ; y++)
		{
			BYTE *ptr = (BYTE *)matSrcBuffer.ptr(y);

			for (int x=1 ; x<matSrcBuffer.cols-1 ; x++, ptr++)
			{				
				// 점등 영역의 평균보다 60% 어두우면 등록
				if ( *ptr / (double)(m[0]) < 0.6 )
				{
					pt.x	= x;
					pt.y	= y;
					pt.gv	= 0;

					// 파일 쓰기
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

// CCD 불량 삭제 갯수 가져오기
int CDefectCCD::GetDefectCCDDeleteCount()
{
	// Load 안된 경우
	if( !bLoad )	return 0;

	return (int)ptIndexsDelete.size();
}

// CCD 불량 보정 갯수 가져오기
int CDefectCCD::GetDefectCCDOffsetCount()
{
	// Load 안된 경우
	if( !bLoad )	return 0;

	return (int)ptIndexsOffset.size();
}

// CCD 불량 삭제
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

		// Size 설정
		rect.x = ptIndexsDelete[i].x * nPS - nSize;
		rect.y = ptIndexsDelete[i].y * nPS - nSize;
		rect.width	= nSize + nSize + nPS;
		rect.height	= nSize + nSize + nPS;

		// 검은색으로 칠하기
		cv::rectangle(matSrcBuffer, rect, cv::Scalar(0, 0, 0), -1);
	}

	return E_ERROR_CODE_TRUE;
}

// CCD 불량 자동 삭제
long CDefectCCD::DeleteAutoDefectCCD(cv::Mat& matSrcBuffer, float fGV, float fBkGV, int nPS, CMatBuf* cMem)
{
	if( matSrcBuffer.empty() )	return E_ERROR_CODE_CCD_EMPTY_BUFFER;

	CMatBuf cMatBufTemp;

	// CCD 불량 개수
	long	nCountCCD = 0;

	// PS모드인 경우
	if( nPS == 2 )
	{
		// Temp 메모리 할당
		cMatBufTemp.SetMem(cMem);
		cv::Mat matInBuf	= cMatBufTemp.GetMat(matSrcBuffer.size(), CV_32FC1);
		cv::Mat matOutBuf	= cMatBufTemp.GetMat(matSrcBuffer.size(), CV_32FC1);
		cv::Mat matKernel1	= cv::Mat::zeros(4, 4, CV_8UC1);
		cv::Mat matKernel2	= cv::Mat::zeros(4, 4, CV_8UC1);

		// Kernel 만들기
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

		// 외부 GV 구하기
		cv::filter2D(matSrcBuffer, matOutBuf, CV_32FC1, matKernel1);

		// 내부 GV 구하기
		cv::filter2D(matSrcBuffer, matInBuf, CV_32FC1, matKernel2);

		// CCD 불량으로 자동 판정
		for (int y=1 ; y<matSrcBuffer.rows ; y++)
		{
			float *ptrIn	= (float *)matInBuf.ptr(y);
			float *ptrOut	= (float *)matOutBuf.ptr(y);

			ptrIn++;
			ptrOut++;

			for (int x=1 ; x<matSrcBuffer.cols ; x++, ptrIn++, ptrOut++)
			{
				if( *ptrIn >= fGV		&&	// 내부 밝기 ? 이상
					*ptrOut <= fBkGV	)	// 외부 밝기 ? 이하 ( Black 에서만 사용하므로... )
				{
					// 내부 밝기가 비슷하지 않는 경우도 존재함...
					cv::rectangle(matSrcBuffer, cv::Rect(x-1, y-1, 2, 2), cv::Scalar(0, 0, 0), -1 );

					// CCD 불량 개수
					nCountCCD++;
				}
			}
		}

		// 할당 해제
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
	// PS모드가 아닌 경우
	else if( nPS == 1 )
	{
		// Temp 메모리 할당
		cMatBufTemp.SetMem(cMem);
		cv::Mat matOutBuf	= cMatBufTemp.GetMat(matSrcBuffer.size(), CV_32FC1);
		cv::Mat matKernel	= cv::Mat::ones(3, 3, CV_8UC1);
		matKernel.at<uchar>(1, 1) = (uchar)0;

		// 외부 GV 구하기
		cv::filter2D(matSrcBuffer, matOutBuf, CV_32FC1, matKernel);

		// CCD 불량으로 자동 판정
		for (int y=0 ; y<matSrcBuffer.rows ; y++)
		{
			uchar *ptrIn	= (uchar *)matSrcBuffer.ptr(y);
			float *ptrOut	= (float *)matOutBuf.ptr(y);

			for (int x=0 ; x<matSrcBuffer.cols ; x++, ptrIn++, ptrOut++)
			{
				if( *ptrIn >= fGV		&&	// 내부 밝기 ? 이상
					*ptrOut <= fBkGV	)	// 외부 밝기 ? 이하 ( Black 에서만 사용하므로... )
				{
					*ptrIn = (uchar)0;

					// CCD 불량 개수
					nCountCCD++;
				}
			}
		}

		// 할당 해제
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

// CCD 불량 보정
long CDefectCCD::OffsetDefectCCD(cv::Mat& matSrcBuffer, int nSize, int nPS)
{
	if( !bLoad )				return E_ERROR_CODE_CCD_NOT_LOAD;
	if( matSrcBuffer.empty() )	return E_ERROR_CODE_CCD_EMPTY_BUFFER;
	if( nSize < 0 )				return E_ERROR_CODE_CCD_WARNING_PARA;

	// 이미지 크기
	long	nWidth	= (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuffer.rows;	// 영상 세로 크기

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i=0 ; i<GetDefectCCDOffsetCount() ; i++)
	{
		// PS모드에 따른 범위
		int nSX		= ptIndexsOffset[i].x * nPS - nSize;
		int nSY		= ptIndexsOffset[i].y * nPS - nSize;
		int nEX		= nSX + nPS + nSize + nSize;
		int nEY		= nSY + nPS + nSize + nSize;
		int nOffset	= ptIndexsOffset[i].gv;

		// 예외처리
		if(nSX < 0)			nSX = 0;
		if(nSY < 0)			nSY = 0;
		if(nEX >= nWidth)	nEX = nWidth-1;
		if(nEY >= nHeight)	nEY = nHeight-1;

		for (int y=nSY ; y<nEY ; y++)
		{
			for (int x=nSX ; x<nEX ; x++)
			{
				// Black Patern만 사용하므로...
				int nGV = matSrcBuffer.at<uchar>(y, x) - nOffset;

				// 예외처리
				if( nGV < 0 )	nGV = 0;

				// 값 다시 넣기
				matSrcBuffer.at<uchar>(y, x) = (uchar)nGV;
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

/*****************************************************************************
  File Name		: ImageSave.h
  Version		: ver 1.0
  Create Date	: 2015.03.06
  Author		: KSW
  Description	: 이미지 저장 관련 함수
  Abbreviations	: 
 *****************************************************************************/


#pragma once

//#include "AviDefineInspect.h"
#include "TaskList.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// User
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void ImageSave(cv::Mat& MatSrcBuffer, TCHAR* szName, ...)
{
	if( MatSrcBuffer.empty() )	return;

	// 가변 인자 파일 경로 받아와서 경로 생성 및 이미지 저장 - Todo. 최적화 할 것
	va_list vaList;
	va_start(vaList, szName);
	TCHAR* cBuffer = NULL;

	if (szName != NULL)
	{
		int len = _vscwprintf( szName, vaList ) + 1;

		cBuffer = new TCHAR[sizeof(TCHAR)*len];
		memset(cBuffer,0,sizeof(TCHAR)*len);

		if (cBuffer)
			vswprintf(cBuffer, szName, (va_list)vaList);
	}
	va_end(vaList);

	CString strPath(cBuffer);	
	SHCreateDirectoryEx(NULL, strPath.Left(strPath.GetLength() - (strPath.GetLength() - strPath.ReverseFind(_T('\\')))), NULL);

	SAFE_DELETE(cBuffer);

	char* pTemp = CSTR2PCH(strPath);
	// 영상 저장
	try{
// TIFF 포맷 저장 속도 관련 OpenCV 소스 수정
// 		if (MatSrcBuffer.type() != CV_8U && MatSrcBuffer.type() != CV_8UC1 && MatSrcBuffer.type() != CV_8UC3)
// 		{
// 			// 압축 안함
// 			vector<int> compression_params;
// 			compression_params.push_back(IMWRITE_PNG_COMPRESSION);
// 			compression_params.push_back(0);
// 			cv::imwrite(pTemp, MatSrcBuffer, compression_params);
// 		}
// 		else
		{
			cv::imwrite(pTemp, MatSrcBuffer);
		}
	}
	catch(cv::Exception& ex){
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Exception ImageSave() : %s"), ex.what());
	}

	SAFE_DELETE_ARR(pTemp);	
}

//void ImageAsyncSaveJPG(cv::Mat& MatSrcBuffer, const char* strPath)
//{
//	const int   MAX_COUNT_OF_ASYNC_IMAGE_SAVE = 0;
//	const int	MAX_JPG_IMAGE_SAVE_SIZE_Y	= 20000;
//	static		int				stnExeCount = 0;
//
//	// 최대 동시 실행 갯수를 제한한다. 과도하게 남용하는 것을 방지
//	if (stnExeCount >= MAX_COUNT_OF_ASYNC_IMAGE_SAVE)
//	{
//	 	//ImageSaveJPG(&MatSrcBuffer, strPath);
//		cv::imwrite( (cv::String)strPath, MatSrcBuffer);
//	 	return;
//	}
//	 	
//	cv::Mat*	pMatSrcBuffer	= &MatSrcBuffer;
//	const char	*strPath_		= strPath;
//	static		CTaskList		TaskList;
//	 
//	stnExeCount++;
//	
//	std::string* pStrPathMC = new std::string(strPath_);
//	 
//	task_group* pTasks = TaskList.AllocNewTaskGroup();
//	 
//	pTasks->run([pMatSrcBuffer, pStrPathMC, pTasks, MAX_JPG_IMAGE_SAVE_SIZE_Y]
//	{
//		//ImageSaveJPG(pMatSrcBuffer, pStrPathMC->c_str());
//		cv::imwrite( (cv::String)(*pStrPathMC), *pMatSrcBuffer);
//
//	 	delete pStrPathMC;
//	 	TaskList.EndTaskGroup(pTasks);
//	});	 
//	 
//	stnExeCount--;
//
//}
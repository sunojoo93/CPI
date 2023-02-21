
/*****************************************************************************
  File Name		: ImageSave.h
  Version		: ver 1.0
  Create Date	: 2015.03.06
  Author		: KSW
  Description	: �̹��� ���� ���� �Լ�
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

	// ���� ���� ���� ��� �޾ƿͼ� ��� ���� �� �̹��� ���� - Todo. ����ȭ �� ��
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
	// ���� ����
	try{
// TIFF ���� ���� �ӵ� ���� OpenCV �ҽ� ����
// 		if (MatSrcBuffer.type() != CV_8U && MatSrcBuffer.type() != CV_8UC1 && MatSrcBuffer.type() != CV_8UC3)
// 		{
// 			// ���� ����
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
//	// �ִ� ���� ���� ������ �����Ѵ�. �����ϰ� �����ϴ� ���� ����
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
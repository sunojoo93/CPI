#pragma once

#include "Define.h"

class CBlob_Sub
{
public:
	CBlob_Sub(void);
	~CBlob_Sub(void);

	//����ū Blob�� ������ �� ū Blob�� Rect�� ��������.
	BOOL FindBiggestBlob(cv::Mat& src, cv::Mat& dst);
	
	//����ū Blob�� RotateRect�� ��������
	BOOL GetBiggestBlobMinAreaRect(cv::Mat& src, cv::RotatedRect& rtRect);

};


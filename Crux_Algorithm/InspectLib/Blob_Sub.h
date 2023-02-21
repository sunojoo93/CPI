#pragma once

#include "Define.h"

class CBlob_Sub
{
public:
	CBlob_Sub(void);
	~CBlob_Sub(void);

	//가장큰 Blob만 추출한 후 큰 Blob의 Rect를 추출해줌.
	BOOL FindBiggestBlob(cv::Mat& src, cv::Mat& dst);
	
	//가장큰 Blob의 RotateRect를 추출해줌
	BOOL GetBiggestBlobMinAreaRect(cv::Mat& src, cv::RotatedRect& rtRect);

};


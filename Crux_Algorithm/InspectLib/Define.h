
/************************************************************************/
// Link, define, enum, struct
// 수정일 : 17.03.08 - KSW
/************************************************************************/

#pragma once

#include "..\VSAlgorithmTask\AviDefineInspect.h"
//#include "..\VSAlgorithmTask\DefineInterface.h"

// 병렬 처리
#include <ppl.h>
using namespace Concurrency;

#include <vector>
#include <concurrent_queue.h>
using namespace std;


//////////////////////////////////////////////////////////////////////////
// OpenCV 3.1
//////////////////////////////////////////////////////////////////////////
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\opencv.hpp>
#include <opencv2\core\cuda.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\cudafilters.hpp>
#include <opencv2\cudaimgproc.hpp>
#include <opencv2\cudaarithm.hpp>
#include <opencv2\cudabgsegm.hpp>
#include <opencv2\cudacodec.hpp>
#include <opencv2\cudafeatures2d.hpp>
#include <opencv2\cudaobjdetect.hpp>
#include <opencv2\cudawarping.hpp>


using namespace cv;
using namespace cv::ml;
using namespace cv::cuda;

//////////////////////////////////////////////////////////////////////////
// DEFINE
//////////////////////////////////////////////////////////////////////////

#define PI acos(-1.)

//////////////////////////////////////////////////////////////////////////
// struct
//////////////////////////////////////////////////////////////////////////
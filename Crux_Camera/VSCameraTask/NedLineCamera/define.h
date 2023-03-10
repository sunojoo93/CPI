#ifndef DEFINE_H
#define DEFINE_H

#define CAM_TACT_LOG_PATH		theApp.GETDRV() + _T(":\\Crux\\DATA\\LOG\\Camera\\Tact\\")
#define CAM_LINESCAN_LOG_PATH	theApp.GETDRV() + _T(":\\Crux\\DATA\\LOG\\Camera\\LineScan\\")
#define CAM_MONITORING_LOG_PATH	theApp.GETDRV() + _T(":\\Crux\\DATA\\LOG\\Camera\\Monitoring\\")
#define DEVICE_FILE_PATH		theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\Device.cfg")
#define INIT_FILE_PATH			theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\Initialize.ini")

// 전역으로 선언할 변수들은 여기에 Define 한다
#define NUM_FRAME_GRABBER	4
#define NUM_INSP_CAMERA		4
#define NUM_LOAD_IMAGE_MAX 10 // GUI 이미지 로드 MAX 카운트
// Camera Model
enum eCamModel	{
	eNUCLi7370AT6 = 0,
	eVN_Series,
	eVP_Series,
	eVH_Series,
	eVA_Series,
	eVA47M5_Series,			// VA47M5 Model 은 별도의 설정값을 가짐 (ex: Trigger Mode)
	eModel_Count
};

enum eLinkType {
	eCameraLink = 0,
	eCoaXPress,
	eLinkType_Count
};

static CString strCamModel[] = {
	_T("VN Series"),
	_T("VP Series"),
	_T("VH Series"),
	_T("VA Series"),
	_T("VA47M5 Series"),
};

// Camera Color
enum eCamColor	{
	eMono = 0,
	eColor,
	eColor_Count
};

static CString strCamColor[] = {
	_T("Mono"),
	_T("Color"),
};

// Camera Baudrate
enum eBaudrate	{
	e1200 = 0,
	e2400,
	e4800,
	e9600,
	e14400,
	e19200,
	e38400,
	e56000,
	e57600,
	e115200,
	eBaudrate_Count
};

static CString strBaudrate[] = {
	_T("1200"),
	_T("2400"),
	_T("4800"),
	_T("9600"),
	_T("14400"),
	_T("19200"),
	_T("38400"),
	_T("56000"),
	_T("57600"),
	_T("115200"),
};

// Camera Trigger Mode
enum eTrigMode	{
	eFreeRun = 0,
	eStandard,
	eFast,
	eDouble,	
	eOverlap,
	eTrigMode_Count
};

static CString strTrigMode[] = {
	_T("Freerun"),
	_T("Standard"),	
	_T("Fast"),
	_T("Double"),
	_T("Overlap"),
};

// Camera Pixel Shift Mode
enum ePSMode	{
	eNone = 0,
	e4Shot,
	e9Shot,
	ePSMode_Count
};

static CString strPSMode[] = {
	_T("None"),
	_T("4-Shot"),
	_T("9-Shot"),
};

// Camera Output Data Bit
enum eDataBit	{
	e8Bit = 8,
	e10Bit = 10,
	e12Bit = 12
};

__inline char* CSTR2PCH(CString strInput)
{
	char * tmpch;
	int sLen = WideCharToMultiByte(CP_ACP, 0, strInput, -1, NULL, 0, NULL, NULL);       
	tmpch = new char[sLen + 1];
	WideCharToMultiByte(CP_ACP, 0, strInput, -1, tmpch, sLen, NULL, NULL);
	return tmpch;
}
__inline CString PCH2CSTR(char* strInput)
{
	int len;
	CString str;
	BSTR buf;
	len = MultiByteToWideChar(CP_ACP, 0, strInput, (int)strlen(strInput), NULL, NULL);
	buf = SysAllocStringLen(NULL, len);
	MultiByteToWideChar(CP_ACP, 0, strInput, (int)strlen(strInput), buf, len);
	str.Format(_T("%s"), buf);
	return str;
}
#endif
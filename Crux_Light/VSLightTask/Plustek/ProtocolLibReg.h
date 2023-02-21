#ifndef PLK_DEBUG_DEFINE

#define PLK_MAX_NUMBEROF_HANDLE		10
#define PLK_BUFF_SIZE				2048
#define PLK_ONOFF_NUMBEROF_CHANNEL	32

#define WM_COMM_READ (WM_USER+100)

//Module type
enum{
	PLK_MODULE_2CH,
	PLK_MODULE_4CH,
	PLK_MODULE_8CH,
	PLK_MODULE_16CH,
	PLK_MODULE_32CH
};

//return value
enum{
	PLK_SUCCESS,
	PLK_INVALID_PARAMETER,
	PLK_INVALID_HANDLE,
	PLK_INVALID_PORT,
	PLK_INVALID_TYPE,
	PLK_TIME_OUT,
	PLK_WRITE_ERR,
	PLK_OPEN_ERR,
	PLK_NOT_SUPPORTED,
	PLK_NONE_DATA
};

typedef struct{
	int moduleType;
	int port;
} PLK_MODULE_INFO;

#endif

#ifndef PLK_DEBUG_EXPORT

#pragma comment(lib, ".\\PlusTek\\ProtocolLibReg.lib") 

typedef struct{
	int		nPort;
	int		nRemotePwmStatus[16];
	int		nManPwmStatus[16];
	int		nPioPwmStatus[16];
	BYTE	nPioOnOffStatus[2];
	int		nBaudStatus;
	int		nMode;
	float	fFpgaVersion;
	float	fFirmwareVersion;
} PROTOCOL_16CH_INFO;

typedef struct{
	int		nPort;
	int		nRemotePwmStatus[8];
	int		nManPwmStatus[8];
	int		nPioPwmStatus[8];
	int		nDacValue[3];
	UCHAR	nRemoteOnOffStatus[4];
} PROTOCOL_8CH_INFO;

typedef struct{
	int		nPort;
	int		nRemotePwmStatus[2];
} PROTOCOL_2CH_INFO;

typedef struct{
	char	strcmd[1024];
	int		nPort;
	int		nRemotePwmStatus[4];
	int		nOnOffStatus[4];
} PROTOCOL_4CH_INFO;

extern "C" __declspec(dllimport) void plkGetLibraryVersion(USHORT *version);
extern "C" __declspec(dllimport) int plkInit(void);
extern "C" __declspec(dllimport) int plkOpenPort(int port,int type,void (*callback)(int nPort,char *pBuff));
extern "C" __declspec(dllimport) int plkOpenPortEx(int port,int type,HWND hWnd);
extern "C" __declspec(dllimport) int plkSetBrightness(int port,int channel,int brightness);
extern "C" __declspec(dllimport) int plkGetBrightness(int channel);
extern "C" __declspec(dllimport) int plkClosePort(int port);
extern "C" __declspec(dllimport) int plkCloseAll(void);
extern "C" __declspec(dllimport) int plkRequestCurrentStatus(int port);
extern "C" __declspec(dllimport) int plkSaveCurrentStatus(int port);
extern "C" __declspec(dllimport) int plkSetOnOff(int port,int channel,int onoff);
extern "C" __declspec(dllimport) int plkSetOnOffEx(int port,ULONG onoff);
extern "C" __declspec(dllimport) int plkGetError(int port);
extern "C" __declspec(dllimport) int plkGetBandWidth(int port);
extern "C" __declspec(dllimport) int plkGetMemIndex(int port);
extern "C" __declspec(dllimport) int plkGetC(int port, int channel);

#endif
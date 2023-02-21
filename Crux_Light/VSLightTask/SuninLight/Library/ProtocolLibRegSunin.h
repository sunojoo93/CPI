#ifndef DEBUG_DEFINE

#define MAX_NUMBEROF_HANDLE		10
#define BUFF_SIZE				2048
#define ONOFF_NUMBEROF_CHANNEL	49

#define WM_COMM_READ (WM_USER+100)

//Module type
enum{
	MODULE_12CH = 12,
	MODULE_17CH = 17
};

//return value
enum{
	SUCCESS,
	INVALID_PARAMETER,
	INVALID_HANDLE,
	INVALID_PORT,
	INVALID_TYPE,
	TIME_OUT,
	WRITE_ERR,
	OPEN_ERR,
	NOT_SUPPORTED,
	NONE_DATA
};

typedef struct{
	int moduleType;
	int port;
	int nBaudRate;
} MODULE_INFO;

#endif

#ifndef DEBUG_EXPORT

#pragma comment(lib, "SuninLight\\Library\\ProtocolLibRegSunin.lib") 

typedef struct{
	char	strcmd[1024];
	int		nPort;
	int		nRemotePwmStatus[4];
	int		nOnOffStatus[4];
} PROTOCOL_INFO;

namespace SuninDll	{
extern "C" __declspec(dllimport) void GetLibraryVersion(USHORT *version);
extern "C" __declspec(dllimport) int Init(void);
extern "C" __declspec(dllimport) int OpenPort(int port, UINT controlCnt, int nBaud, UINT CtrlChannelCnt[], void (*callback)(int nPort,char *pBuff));
extern "C" __declspec(dllimport) int OpenPortEx(int port, UINT controlCnt, int nBaud, UINT CtrlChannelCnt[], HWND hWnd);
extern "C" __declspec(dllimport) int SetBrightness(int port, int ctrlID, UINT LightVal[], int channelCnt);	// All Channels light on 180514 YSS
extern "C" __declspec(dllimport) int GetBrightness(int channel);
extern "C" __declspec(dllimport) int ClosePort(int port);
extern "C" __declspec(dllimport) int CloseAll(void);
extern "C" __declspec(dllimport) int SaveCurrentStatus(int port);
extern "C" __declspec(dllimport) int SetOnOff(int port, int ctrlID,int onoff, int channelCnt);			// All Channels setonoff 180514 YSS
extern "C" __declspec(dllimport) int SetOnOffEx(int port, int ctrlID, ULONG onoff, int channelCnt);		// All Channels setonoff 180514 YSS
extern "C" __declspec(dllimport) int GetError(int port);
// extern "C" __declspec(dllimport) int GetC(int port, int channel);
}

#endif
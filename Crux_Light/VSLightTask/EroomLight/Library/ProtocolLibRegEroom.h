#ifndef DEBUG_DEFINE

#define MAX_NUMBEROF_HANDLE		10
#define BUFF_SIZE				2048
#define ONOFF_NUMBEROF_CHANNEL	32

#define WM_COMM_READ (WM_USER+100)

//Module type
enum{
	MODULE_6CH,
	MODULE_24CH,
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
} MODULE_INFO;

#endif

#ifndef DEBUG_EXPORT

#pragma comment(lib, "EroomLight\\Library\\ProtocolLibRegEroom.lib") 

typedef struct{
	char	strcmd[1024];
	int		nPort;
	int		nRemotePwmStatus[4];
	int		nOnOffStatus[4];
} PROTOCOL_INFO;

namespace EroomDll	{
extern "C" __declspec(dllimport) void GetLibraryVersion(USHORT *version);
extern "C" __declspec(dllimport) int Init(void);
extern "C" __declspec(dllimport) int OpenPort(int port,int type,void (*callback)(int nPort,char *pBuff));
extern "C" __declspec(dllimport) int OpenPortEx(int port,int type,HWND hWnd);
extern "C" __declspec(dllimport) int SetBrightness(int port,int channel,int brightness);
extern "C" __declspec(dllimport) int GetBrightness(int channel);
extern "C" __declspec(dllimport) int ClosePort(int port);
extern "C" __declspec(dllimport) int CloseAll(void);
extern "C" __declspec(dllimport) int SaveCurrentStatus(int port);
extern "C" __declspec(dllimport) int SetOnOff(int port,int channel,int onoff);
extern "C" __declspec(dllimport) int SetOnOffEx(int port,ULONG onoff);
extern "C" __declspec(dllimport) int GetError(int port);
// extern "C" __declspec(dllimport) int GetC(int port, int channel);
}

#endif
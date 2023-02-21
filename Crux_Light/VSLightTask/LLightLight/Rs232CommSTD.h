
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RS232COMMSTD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RS232COMMSTD_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
// #ifdef RS232COMMSTD_EXPORTS
// #define RS232COMMSTD_API __declspec(dllexport)
// #else
// #define RS232COMMSTD_API __declspec(dllimport)
// #endif

#include "CommThread.h"

#define RS232_BUFF_SIZE				4096
#define RS232_BAUDRATE_2400			0x00
#define RS232_BAUDRATE_4800			0x01
#define RS232_BAUDRATE_9600			0x02
#define RS232_BAUDRATE_19200		0x03
#define RS232_BAUDRATE_38400		0x04
#define RS232_BAUDRATE_115200		0x05

#define ASCII_CARRIGE_RETURN 		0x0D
#define ASCII_NEWLINE 				0x0A
#define ASCII_ESC					0x1B
#define ASCII_SPACE					0x20
#define ASCII_BACKSPACE				0x08

// This class is exported from the Rs232CommSTD.dll
class /*RS232COMMSTD_API */CRs232CommSTD {
public:
	CRs232CommSTD(void);
	~CRs232CommSTD(void);
	BOOL Create(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(void));
	BOOL Create(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(int nPort,char *pBuff));
	BOOL Create(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,HWND hWnd);
	DWORD WriteData(unsigned char *pBuff,DWORD nToWrite);
	BOOL IsOpen();
	void GetByte(unsigned char *pBuff);
	int GetSize();
	CCommThread	*m_ptheCommThread;
};

/*
extern RS232COMMSTD_API int nRs232CommSTD;
RS232COMMSTD_API int fnRs232CommSTD(void);
*/
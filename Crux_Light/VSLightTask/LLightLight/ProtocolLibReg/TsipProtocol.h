
// TsipProtocol.h: interface for the CTsipProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TSIPPROTOCOL_H__2D045569_8CB5_45F3_939F_8F022BFAC6AC__INCLUDED_)
#define AFX_TSIPPROTOCOL_H__2D045569_8CB5_45F3_939F_8F022BFAC6AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TSIP_MAX	1024

#define TSIP_DLE	0x10
#define TSIP_ETX	0x03

//ID Define
#define ID_REMOTE_PWM_REQUEST	0x00
#define ID_REMOTE_PWM_SETINFO	0x01
#define ID_REMOTE_PWM_SETONOFF	0x02
	
#define ID_MAN_PWM_REQUEST		0x20
#define ID_MAN_PWM_INFO			0x21
	
#define ID_PIO_PWM_REQUEST		0x30
#define ID_PIO_PWM_INFO			0x31
#define ID_PIO_ONOFF_REQUEST	0x32
#define ID_PIO_ONOFF_INFO		0x33
	
#define ID_BAUDRATE_REQUEST		0x80
#define ID_BAUDRATE_SETINFO		0x81
#define ID_MODE_REQUEST			0x82
#define ID_MODE_INFO			0x83
#define ID_FIRMWARE_VER_REQUEST	0x84
#define ID_FIRMWARE_VER_INFO	0x85
#define ID_FPGA_VER_REQUEST		0x86
#define ID_FPGA_VER_INFO		0x87

	
#define ID_CURRENT_SAVE			0xF0
#define ID_CURRENT_REQUEST		0xF1
#define ID_CURRENT_INFO			0xF2
//End

typedef	struct TsipSendPacket
{
	int		dataSize;
	BYTE	SendData[TSIP_MAX];
} TsipSendPacket;

class TsipProtocol  
{
public:
	TsipProtocol();
	virtual ~TsipProtocol();
	TsipSendPacket 	EncodeData(int Command,int nSize=0,char *src=NULL);
	BOOL DecodeData(int size, char *src);
	int		m_nRxPacket;
	BYTE*	m_pRxBuff;
	BYTE*	m_pRxBuffFix;
	BYTE*   m_pSizeTable;
	BOOL	m_bDleFlag;
	int		m_nRxCnt;
};

#endif // !defined(AFX_TSIPPROTOCOL_H__2D045569_8CB5_45F3_939F_8F022BFAC6AC__INCLUDED_)
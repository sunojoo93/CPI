// TsipProtocol.cpp: implementation of the TsipProtocol class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TsipProtocol.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TsipProtocol::TsipProtocol()
{
	m_bDleFlag		= FALSE;
	m_nRxCnt		= 0;
	m_pRxBuff		= new BYTE[TSIP_MAX];
	m_pRxBuffFix	= new BYTE[TSIP_MAX];
	m_pSizeTable	= new BYTE[TSIP_MAX];
}

TsipProtocol::~TsipProtocol()
{
	if(m_pRxBuff)	{delete m_pRxBuff;}
	if(m_pRxBuffFix){delete m_pRxBuffFix;}
	if(m_pSizeTable){delete m_pSizeTable;}
}

BOOL TsipProtocol::DecodeData(int size, char *src)
{
	int  nOffset=0;
	BOOL bstatus=FALSE;
	m_nRxPacket =0;
	for(int i=0;i<size;i++)
	{
		if(src[i]==TSIP_DLE)	//Start/Data
		{
			if(!m_bDleFlag)			//Fisrt DLE
				m_bDleFlag=TRUE;
			else
			{
				m_bDleFlag=FALSE;
				m_pRxBuff[m_nRxCnt] = src[i];
				m_nRxCnt++;
			}
		}
		else if(src[i]==TSIP_ETX && m_bDleFlag)	//End Frame
		{
			m_bDleFlag = FALSE;
			memcpy(&m_pRxBuffFix[nOffset],m_pRxBuff,m_pSizeTable[m_nRxPacket]=m_nRxCnt);
			nOffset += m_pSizeTable[m_nRxPacket];
			m_nRxCnt = 0;
			bstatus = TRUE;
			m_nRxPacket++;
		}
		else if(m_nRxCnt == TSIP_MAX)
		{
			m_bDleFlag = FALSE;	
			m_nRxCnt = 0;
		}
		else		//ID / Data
		{
			m_bDleFlag = FALSE;
			m_pRxBuff[m_nRxCnt] = src[i];
			m_nRxCnt++;
		}
	}
	return bstatus;
}

TsipSendPacket TsipProtocol::EncodeData(int Command,int nSize,char* src)
{
	TsipSendPacket Packet;
	Packet.SendData[0] = TSIP_DLE;
	Packet.SendData[1] = Command;
	Packet.dataSize = 2;
	for(int i=0;i<nSize;i++)
	{
		if(src[i]==TSIP_DLE) Packet.SendData[Packet.dataSize++] = TSIP_DLE;
		Packet.SendData[Packet.dataSize++] = src[i];
	}
	
	Packet.SendData[Packet.dataSize++] = TSIP_DLE;
	Packet.SendData[Packet.dataSize++] = TSIP_ETX;

	return Packet;
}

/// CPU TYPE
// QnACPU
#define		CPU_Q2ACPU								0x0011					// Q2A
#define		CPU_Q2AS1CPU							0x0012					// Q2AS1
#define		CPU_Q3ACPU								0x0013					// Q3A
#define		CPU_Q4ACPU								0x0014					// Q4A
// QCPU Q
#define		CPU_Q02CPU								0x0022					// Q02(H) Q
#define		CPU_Q06CPU								0x0023					// Q06H   Q
#define		CPU_Q12CPU								0x0024					// Q12H   Q
#define		CPU_Q25CPU								0x0025					// Q25H   Q
#define		CPU_Q00JCPU								0x0030					// Q00J	  Q
#define		CPU_Q00CPU								0x0031					// Q00	  Q
#define		CPU_Q01CPU								0x0032					// Q01	  Q
#define		CPU_Q12PHCPU							0x0041					// Q12PHCPU Q
#define		CPU_Q25PHCPU							0x0042					// Q25PHCPU Q
#define		CPU_Q12PRHCPU							0x0043					// Q12PRHCPU Q
#define		CPU_Q25PRHCPU							0x0044					// Q25PRHCPU Q
#define		CPU_Q25SSCPU							0x0055					// Q25SS
#define		CPU_Q03UDCPU							0x0070					// Q03UDCPU
#define		CPU_Q04UDHCPU							0x0071					// Q04UDHCPU
#define		CPU_Q06UDHCPU							0x0072					// Q06UDHCPU
#define		CPU_Q02UCPU								0x0083					// Q02UCPU
#define		CPU_Q03UDECPU							0x0090					// Q03UDECPU
#define		CPU_Q04UDEHCPU							0x0091					// Q04UDEHCPU
#define		CPU_Q06UDEHCPU							0x0092					// Q06UDEHCPU
#define		CPU_Q13UDHCPU							0x0073					// Q13UDHCPU
#define		CPU_Q13UDEHCPU							0x0093					// Q13UDEHCPU
#define		CPU_Q26UDHCPU							0x0074					// Q26UDHCPU	
#define		CPU_Q26UDEHCPU							0x0094					// Q26UDEHCPU
#define		CPU_Q02PHCPU							0x0045					// Q02PH  Q
#define		CPU_Q06PHCPU							0x0046					// Q06PH  Q
#define		CPU_Q00UJCPU							0x0080					// Q00UJCPU
#define		CPU_Q00UCPU								0x0081					// Q00UCPU
#define		CPU_Q01UCPU								0x0082					// Q01UCPU
#define		CPU_Q10UDHCPU							0x0075					// Q10UDHCPU
#define		CPU_Q20UDHCPU							0x0076					// Q20UDHCPU
#define		CPU_Q10UDEHCPU							0x0095					// Q10UDEHCPU
#define		CPU_Q20UDEHCPU							0x0096					// Q20UDEHCPU
#define		CPU_Q50UDEHCPU							0x0098					// Q50UDEHCPU
#define		CPU_Q100UDEHCPU							0x009A					// Q100UDEHCPU

// ACPU
#define		CPU_A0J2HCPU							0x0102					// A0J2H
#define		CPU_A1FXCPU								0x0103					// A1FX
#define		CPU_A1SCPU								0x0104					// A1S,A1SJ
#define		CPU_A1SHCPU								0x0105					// A1SH,A1SJH
#define		CPU_A1NCPU								0x0106					// A1(N)
#define		CPU_A2CCPU								0x0107					// A2C,A2CJ
#define		CPU_A2NCPU								0x0108					// A2(N),A2S
#define		CPU_A2SHCPU								0x0109					// A2SH
#define		CPU_A3NCPU								0x010A					// A3(N)
#define		CPU_A2ACPU								0x010C					// A2A
#define		CPU_A3ACPU								0x010D					// A3A
#define		CPU_A2UCPU								0x010E					// A2U,A2US
#define		CPU_A2USHS1CPU							0x010F					// A2USHS1
#define		CPU_A3UCPU								0x0110					// A3U
#define		CPU_A4UCPU								0x0111					// A4U
// QCPU A
#define		CPU_Q02CPU_A							0x0141					// Q02(H)
#define		CPU_Q06CPU_A							0x0142					// Q06H
// LCPU
#define		CPU_L02CPU								0x00A1					// L02CPU
#define		CPU_L26CPUBT							0x00A2					// L26CPU-BT
// C Controller
#define		CPU_Q12DC_V								0x0058					// Q12DCCPU-V
// Q MOTION 	
#define		CPU_Q172CPU								0x621					// Q172CPU
#define		CPU_Q173CPU								0x622					// Q173CPU
#define		CPU_Q172HCPU							0x621					// Q172HCPU
#define		CPU_Q173HCPU							0x622					// Q173HCPU
#define		CPU_Q172DCPU							0x625					// Q172DCPU
#define		CPU_Q173DCPU							0x626					// Q173DCPU
#define		CPU_Q172DSCPU							0x62A					// Q172DSCPU
#define		CPU_Q173DSCPU							0x62B					// Q173DSCPU
// QSCPU
#define		CPU_QS001CPU							0x0060					// QS001
// FXCPU
#define		CPU_FX0CPU								0x0201					// FX0/FX0S
#define		CPU_FX0NCPU								0x0202					// FX0N
#define		CPU_FX1CPU								0x0203					// FX1
#define		CPU_FX2CPU								0x0204					// FX2/FX2C
#define		CPU_FX2NCPU								0x0205					// FX2N/FX2NC
#define		CPU_FX1SCPU								0x0206					// FX1S
#define		CPU_FX1NCPU								0x0207					// FX1N
#define		CPU_FX3UCCPU							0x0208					// FX3U/FX3UC 
#define		CPU_FX3GCPU								0x0209					// FX3G
// BOARD
#define		CPU_BOARD								0x0401					// NETWORK BOARD
// MOTION 	
#define		CPU_A171SHCPU							0x0601					// A171SH
#define		CPU_A172SHCPU							0x0602					// A172SH
#define		CPU_A273UHCPU							0x0603					// A273UH
#define		CPU_A173UHCPU							0x0604					// A173UH
// GOT
#define		CPU_A900GOT								0x0701					// A900GOT

/// PORT
#define		PORT_1								    0x01    				// CommunicationPort1
#define		PORT_2								    0x02    				// CommunicationPort2
#define		PORT_3								    0x03    				// CommunicationPort3
#define		PORT_4								    0x04    				// CommunicationPort4
#define		PORT_5								    0x05    				// CommunicationPort5
#define		PORT_6								    0x06    				// CommunicationPort6
#define		PORT_7								    0x07    				// CommunicationPort7
#define		PORT_8								    0x08    				// CommunicationPort8
#define		PORT_9								    0x09    				// CommunicationPort9
#define		PORT_10								    0x0A					// CommunicationPort10


/// BAUDRATE
#define		BAUDRATE_300							CBR_300     	       	// 300bps
#define		BAUDRATE_600							CBR_600  	          	// 600bps
#define		BAUDRATE_1200							CBR_1200            	// 1200bps
#define		BAUDRATE_2400							CBR_2400            	// 2400bps
#define		BAUDRATE_4800							CBR_4800            	// 4800bps
#define 	BAUDRATE_9600							CBR_9600            	// 9600bps
#define 	BAUDRATE_19200							CBR_19200           	// 19200bps
#define 	BAUDRATE_38400							CBR_38400           	// 38400bps
#define 	BAUDRATE_57600							CBR_57600           	// 57600bps
#define 	BAUDRATE_115200							CBR_115200          	// 115200bps


/// DATA BIT
#define 	DATABIT_7								7						// DATA BIT 7
#define 	DATABIT_8								8						// DATA BIT 8


/// PARITY
#define 	NO_PARRITY								NOPARITY            	// NO PARITY
#define 	ODD_PARITY								ODDPARITY           	// ODD PARITY
#define 	EVEN_PARITY								EVENPARITY          	// EVEN PARITY


/// STOP BITS
#define 	STOPBIT_ONE								ONESTOPBIT          	// 1 STOP BIT
#define 	STOPBIT_TWO								TWOSTOPBITS         	// 2 STOP BIT


/// SERIAL CONTROL
#define		TRC_DTR									0x01					// DTR
#define		TRC_RTS									0x02					// RTS
#define		TRC_DTR_AND_RTS							0x07					// DTR and RTS 
#define		TRC_DTR_OR_RTS							0x08					// DTR or RTS


/// SUM CHECK
#define SUM_CHECK									1						// Sum Check
#define NO_SUM_CHECK								0						// No Sum Check


///PACKET TYPE
#define		PACKET_ASCII							0x02					//PACKET TYPE ASCII
#define		PACKET_BINARY							0x03					//PACKET TYPE BINARY

///CONNECT WAY
#define		TEL_AUTO_CONNECT						0x00					// AUTO LINE CONNECT
#define		TEL_AUTO_CALLBACK						0x01					// AUTO LINE CONNECT(CALLBACK FIXATION)
#define		TEL_AUTO_CALLBACK_NUMBER				0x02					// AUTO LINE CONNECT(CALLBACK NUMBER SPECIFICATION)
#define		TEL_CALLBACK							0x03					// CALLBACK CONNECT(FIXATION)
#define		TEL_CALLBACK_NUMBER						0x04					// CALLBACK CONNECT(NUMBER SPECIFICATION)
#define		TEL_CALLBACK_REQUEST					0x05					// CALLBACK REQUEST(FIXATION)
#define		TEL_CALLBACK_REQUEST_NUMBER				0x06					// CALLBACK REQUEST(NUMBER SPECIFICATION)
#define		TEL_CALLBACK_WAIT						0x07					// CALLBACK RECEPTION WAITING

///LINE TYPE
#define		LINETYPE_PULSE							0x00					// PULSE
#define		LINETYPE_TONE							0x01					// TONE
#define		LINETYPE_ISDN							0x02					// ISDN

///GOT TRANSPARENT PC IF
#define         GOT_PCIF_USB			1		// USB
#define         GOT_PCIF_SERIAL			2		// SERIAL
#define         GOT_PCIF_ETHERNET		3		// ETHERNET

///GOT TRANSPARENT PLC IF
#define GOT_PLCIF_SERIAL_QCPUQ		1		// SERIAL-QCPU Q
#define GOT_PLCIF_SERIAL_QCPUA		2		// SERIAL-QCPU A
#define GOT_PLCIF_SERIAL_QNACPU		3		// SERIAL-QnACPU
#define GOT_PLCIF_SERIAL_ACPU		4		// SERIAL-ACPU
#define GOT_PLCIF_SERIAL_FXCPU		5		// SERIAL-FXCPU
#define GOT_PLCIF_SERIAL_LCPU		6		// SERIAL-LCPU
#define GOT_PLCIF_SERIAL_QJ71C24	30		// SERIAL-QJ71C24
#define GOT_PLCIF_SERIAL_LJ71C24	31		// SERIAL-LJ71C24
#define GOT_PLCIF_ETHERNET_QJ71E71	50		// ETHERNET-QJ71E71
#define GOT_PLCIF_ETHERNET_CCIEFADP	60		// ETHERNET-CC IE Field adapter
#define GOT_PLCIF_ETHERNET_QCPU		70		// ETHERNET-QCPU
#define GOT_PLCIF_ETHERNET_LCPU		71		// ETHERNET-LCPU
#define GOT_PLCIF_BUS				90		// BUS

///ACTPROGTYPE UNITTYPE
#define		UNIT_QNCPU				0x13	// QnCPU(Qnﾓｰﾄﾞ)RS232C直結 
#define		UNIT_FXCPU				0x0F	// FXCPU直結 
#define		UNIT_LNCPU				0x50	// LnCPU RS232Cアダプタ接続
#define		UNIT_QNMOTION			0x1C	// QnﾓｰｼｮﾝRS232C直結
#define		UNIT_QJ71C24			0x19	// Qn用C24ﾕﾆｯﾄ
#define		UNIT_FX485BD			0x24	// FXCPU計算機リンク(RS485接続)
#define		UNIT_LJ71C24			0x54	// L用C24ﾕﾆｯﾄ直結
#define		UNIT_QJ71E71			0x1A	// Qn用E71ﾕﾆｯﾄ
#define		UNIT_FXENET				0x26	// FX3U用Ethernetユニット
#define		UNIT_FXENET_ADP			0x27	// FX1N-ENET-ADP
#define		UNIT_QNETHER			0x2C	// QnCPU(Qnﾓｰﾄﾞ)Ether直結 IP指定通信
#define		UNIT_QNETHER_DIRECT		0x2D	// QnCPU(Qnﾓｰﾄﾞ)Ether直結 直結通信
#define		UNIT_LNETHER			0x52	// LnCPU Ethernetﾎﾟｰﾄ接続
#define		UNIT_LNETHER_DIRECT		0x53	// LnCPU Ethernetﾎﾟｰﾄ直結接続
#define		UNIT_NZ2GF_ETB			0x59	// NZ2GF-ETB(Ethernetアダプタ)接続
#define		UNIT_NZ2GF_ETB_DIRECT	0x5A	// NZ2GF-ETB(Ethernetアダプタ)直結接続
#define		UNIT_QNUSB				0x16	// QnCPU(Qnﾓｰﾄﾞ)USB直結 
#define		UNIT_LNUSB				0x51	// LnCPU USBﾎﾟｰﾄ直結
#define		UNIT_QNMOTIONUSB		0x1D	// QnﾓｰｼｮﾝUSB直結
#define		UNIT_G4QNCPU			0x1B	// AJ65BT-G4-S3ユニット直結(Qnアクセス)
#define		UNIT_CCLINKBOARD		0x0C	// CC-Linkﾎﾞｰﾄﾞ 
#define		UNIT_MNETHBOARD			0x1E    // MNET/Hﾎﾞｰﾄﾞ接続
#define		UNIT_MNETGBOARD			0x2B	// CC-Link IE ｺﾝﾄﾛｰﾗﾈｯﾄﾜｰｸ ﾎﾞｰﾄﾞ接続
#define		UNIT_CCIEFBOARD			0x2F	// CC-Link IEフィールドボード
#define		UNIT_SIMULATOR			0x0B	// シミュレータ（GX Simulator）接続
#define		UNIT_SIMULATOR2			0x30	// GX Simulator2
#define		UNIT_QBF				0x1F	// Qn用パソコンCPU
#define		UNIT_QSS				0x20	// Qn用SoftLogic
#define		UNIT_A900GOT			0x21	// GOT
#define		UNIT_GOT_QJ71E71		0x40	// GOT1000 Ethernetトランスペアレント QJ71E71
#define		UNIT_GOT_QNETHER		0x41	// GOT1000 Ethernetトランスペアレント Ethernet内蔵Qn
#define		UNIT_GOT_LNETHER		0x55	// GOT1000ｼﾘｰｽﾞ経由LnCPU Ethernetﾎﾟｰﾄ接続
#define		UNIT_GOT_NZ2GF_ETB		0x5B	// GOT1000ｼﾘｰｽﾞ経由NZ2GF-ETB(Ethernetアダプタ)接続
#define		UNIT_GOTETHER_QNCPU		0x56	// GOT1000ｼﾘｰｽﾞEthernetﾎﾟｰﾄ経由QnCPU-RS422ﾎﾟｰﾄ接続
#define		UNIT_GOTETHER_QBUS		0x58	// GOT1000ｼﾘｰｽﾞEthernetﾎﾟｰﾄ経由Qﾊﾞｽ接続
#define		UNIT_GOTETHER_LNCPU		0x57	// GOT1000ｼﾘｰｽﾞEthernetﾎﾟｰﾄ経由LnCPU RS232Cアダプタ接続

///ACTPROGTYPE PROTOCOLTYPE
#define		PROTOCOL_SERIAL			0x04	// Serialﾎﾟｰﾄ経由 
#define		PROTOCOL_USB			0x0D	// USBﾎﾟｰﾄ経由
#define		PROTOCOL_TCPIP			0x05	// TCP/IP経由 
#define		PROTOCOL_UDPIP			0x08	// UDP/IP経由 
#define		PROTOCOL_MNETH			0x0F	// Mnethﾎﾞｰﾄﾞ経由 
#define		PROTOCOL_MNETG			0x14	// Mnetgボード経由
#define		PROTOCOL_CCIEF			0x15	// CC-Link IEフィールドボード経由
#define		PROTOCOL_CCLINK			0x07	// CC-Link経由
#define		PROTOCOL_SERIALMODEM	0x0E	// ｼﾘｱﾙﾎﾟｰﾄ+ﾓﾃﾞﾑ経由 
#define		PROTOCOL_TEL			0x0A	// TEL経由 */
#define		PROTOCOL_QBF			0x10	// Qn用パソコンCPU経由
#define		PROTOCOL_QSS			0x11	// Qn用SoftLogic経由
#define		PROTOCOL_USBGOT			0x13	// USBﾎﾟｰﾄ+GOT経由
#define		PROTOCOL_SHAREDMEMORY	0x06	//	/* 共有ﾒﾓﾘ経由 */


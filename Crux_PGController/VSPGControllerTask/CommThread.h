#pragma once


enum FlowControl
{
	NoFlowControl,
	CtsRtsFlowControl,
	CtsDtrFlowControl,
	DsrRtsFlowControl,
	DsrDtrFlowControl,
	XonXoffFlowControl
};

//#define WM_COMM_READ (WM_USER +1)
#define COMM_BUFF_SIZE			6500

#define ESC_ASCII			0x1B
#define STX_ASCII			0x02
#define ETX_ASCII			0x03
#define CR_ASCII			0x0D
#define LF_ASCII			0x0A

//	��� Ŭ����	CCommThread 

// ��Ʈ���� �б� :
//   ��Ʈ�� �� �Ŀ� ��Ʈ�� �ڷᰡ �����ϸ� WM_COMM_READ �޽����� ���� 
//   �����쿡 ���޵ȴ�. ON_MESSAGE ��ũ�θ� �̿�, �Լ��� �����ϰ�
//   m_ReadData String�� ����� �����͸� �̿� �б�
// ��Ʈ�� ���� : WriteComm(buff, 30)�� ���� ���ۿ� �� ũ�⸦ �ǳ׸� �ȴ�.
class CQueue
{
public:
	BYTE buff[COMM_BUFF_SIZE];//ť ���� ������� 4192�� ���� 
	int m_iHead, m_iTail;//ť ���ۿ� �����͸� �ְ� �� �ö� ����� ���� 
	CQueue();
	void Clear();//���۸� �ʱ�ȭ ��Ŵ
	int GetSize();//���� ���ۿ� ����ִ� �������� size�� ����
	BOOL PutByte(BYTE b);//ť���ۿ� 1����Ʈ�� ����
	BOOL GetByte(BYTE *pb);//ť���ۿ��� 1����Ʈ�� �� ��
	BOOL GetWord(short *pb);
};

class	CCommThread
{
public:
	CCommThread();
	~CCommThread();
	//--------- ȯ�� ���� -----------------------------------------//
	HANDLE		m_hComm;				// ��� ��Ʈ ���� �ڵ�
	CString		m_sPortName;			// ��Ʈ �̸� (COM1 ..)
	BOOL		m_bConnected;			// ��Ʈ�� ���ȴ��� ������ ��Ÿ��.
	OVERLAPPED	m_osRead, m_osWrite;	// ��Ʈ ���� Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch�Լ� Thread �ڵ�.
	WORD		m_wPortID;				// WM_COMM_READ�� �Բ� ������ �μ�.
	CQueue      m_QueueRead;			//ť����
	//--------- �ܺ� ��� �Լ� ------------------------------------//
	BOOL	OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity, FlowControl fc);//��Ʈ ���� 
	void	ClosePort();				//��Ʈ �ݱ�
	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);//��Ʈ�� ������ ����

	//--------- ���� ��� �Լ� ------------------------------------//
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);//��Ʈ���� ������ �о����

	CString byIndexComPort(int xPort);// ��Ʈ�̸� �ޱ� 
	DWORD byIndexBaud(int xBaud);// ���۷� �ޱ�
	BYTE byIndexData(int xData);//������ ��Ʈ �ޱ�
	BYTE byIndexStop(int xStop);// �����Ʈ �ޱ� 
	BYTE byIndexParity(int xParity);// �丮Ƽ �ޱ�
};

// Thread�� ����� �Լ� 
DWORD	ThreadWatchCommunication(CCommThread* pComm);


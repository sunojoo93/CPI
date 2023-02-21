// Queue.h: interface for the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__67838F53_76C4_484A_8469_14FCEC21A72A__INCLUDED_)
#define AFX_QUEUE_H__67838F53_76C4_484A_8469_14FCEC21A72A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFF_SIZE 40960

class CQueue  
{
public:
	BYTE m_Buff[BUFF_SIZE];
	int m_iHead,m_iTail;
	CQueue();
	void Clear();
	int GetSize();
	BOOL PutByte(BYTE b);
	BOOL GetByte(BYTE *pb);
	virtual ~CQueue();
};

#endif // !defined(AFX_QUEUE_H__67838F53_76C4_484A_8469_14FCEC21A72A__INCLUDED_)
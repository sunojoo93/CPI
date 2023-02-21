// Queue.cpp: implementation of the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Queue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueue::CQueue()
{
	Clear();
}

void CQueue::Clear()
{
	m_iHead = m_iTail = 0;
	memset(m_Buff,0,BUFF_SIZE);
}
int CQueue::GetSize()
{
	return (m_iHead - m_iTail+BUFF_SIZE) % BUFF_SIZE;
}

BOOL CQueue::PutByte(BYTE b)
{
	if(GetSize() == (BUFF_SIZE-1)) return FALSE;
	m_Buff[m_iHead++] = b;
	m_iHead %= BUFF_SIZE;
	return TRUE;
}

BOOL CQueue::GetByte(BYTE *pb)
{
	if(!GetSize()) return FALSE;
	*pb = m_Buff[m_iTail++];
	m_iTail %= BUFF_SIZE;
	return TRUE;
}

CQueue::~CQueue()
{

}

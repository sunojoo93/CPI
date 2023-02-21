/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		Thread.h
@brief		Thread �� Class ȭ �Ͽ� ����� �� �ְ� �ϴ� ���� Class �� �����Ѵ�.

@remark		
@author		�����
@date		2006.
*/

#pragma once

#include <process.h>

/** 
@brief		�������� ���� ����� �����Ѵ�.

@remark		
-			������ ������ ���� CThread �� ��ӹ޾Ƽ� ����ؾ� �Ѵ�. 
-			��� ���� Class �� m_fnRunThread �� �������ؼ� ����Ѵ�.
@author		�����
@date		2006.02.21
*/ 
class CThread
{
public:

	/** 
	@brief		������

	@return		����
	@remark		Thread ������ �ʿ��� ���ڸ� �޾Ƽ� �ʱ�ȭ �Ѵ�.
	@author		�����
	@date		2006.
	*/
	CThread(
		int nStackSize = 0,						///< Thread Stack Size
		int nPriority = THREAD_PRIORITY_NORMAL	///< Thread �� �켱����
		)	
	{
		//Stack size �� Priority �� �Է� �޾� ��� ������ �����Ѵ�.
		m_nStackSize	= nStackSize;
		m_nPriority		= nPriority;

		//Thread ID �� Thread handle �� �ʱ�ȭ �Ѵ�.
		m_dwThreadID	= 0;
		m_hThread		= NULL;
	}

	/** 
	@brief		�Ҹ���

	@return		����
	@remark		
	@author		�����
	@date		2006.
	*/
	~CThread(void)
	{
		//Thread�� �Ҹ��Ѵ�.
		::CloseHandle(m_hThread);

		//Thread handle�� �ʱ�ȭ �Ѵ�.
		m_hThread = NULL;						
	}

	/** 
	@brief		Thread ����� ����ϴ� m_fnRunThread�� ���� ��Ų��.

	@return		������ Thread �� Handle
	@remark		
	@author		�����
	@date		2006.
	*/
	HANDLE m_fnStartThread()
	{
		//���� ��� ������ �����Ͽ� Thread�� �����Ѵ�. 
		m_hThread = (HANDLE) _beginthreadex( 0, m_nStackSize, CThread::__THREAD_ENTRY_POINT, this, 0, (unsigned *)&m_dwThreadID);

		//Thread handle �� ��ȯ�Ѵ�.
		return m_hThread;						
	}

	/** 
	@brief		���� Thread ���� ����� �����Ѵ�.

	@return		Thread ���� �� Ture ��ȯ
	@remark		
	@author		�����
	@date		2006.
	*/
	bool m_fnWaitForTermination()
	{
		//Thread handle �� �����Ͽ� Thread�� �����Ѵ�.
		if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThread, INFINITE))
		{
			::TerminateThread(m_hThread, 0);		
		}

		return true;
	}

protected:

	/** 
	@brief		Virtual Function���� ��� �޴� Class ���� ������ �Ͽ� ����Ѵ�.

	@return		����
	@remark		��� �޴� Class ������ Thread �� ����Ǵ� ����� ������ �Ͽ� ����Ͽ��� �Ѵ�.
	@author		�����
	@date		2006.
	*/
	virtual void				m_fnRunThread() = 0;

private:

	/** 
	@brief		���� Thread�μ� �������� ����� �ϴ� m_fnRunThread�� ȣ���Ѵ�.

	@return		����
	@remark		
	@author		�����
	@date		2006.
	*/
	static unsigned __stdcall __THREAD_ENTRY_POINT(
		void* pVoid									///< ������ CThread ��ü�� Pointer
		)
	{
		//�Ķ���ͷ� �Ѱ� �޴� ���� CThread ��ü�� Pointer �̴�.
		CThread* pThis = (CThread*) pVoid;	

		//���� ����� ����ϴ� �Լ��� ȣ���Ѵ�.
		pThis->m_fnRunThread();					
		return 0;
	}

	/*��� ����*/
private:
	DWORD	m_dwThreadID;		///< Thread ID �μ� Thread ������ ID�� ������ �ִ´�.
	HANDLE	m_hThread;			///< Thread Handle �μ� Thread ������ Handle �� ������ �ִ´�.
	int		m_nStackSize;		///< Thread ������ �ʿ��� Thread Stack size �� ������ �ִ´�.
	int		m_nPriority;		///< Thread ������ �ʿ��� Thread Priority �� ������ �ִ´�.
};

/** 
@}
*/ 
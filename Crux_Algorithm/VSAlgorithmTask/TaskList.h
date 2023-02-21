
#pragma once

#include <ppl.h>

using namespace Concurrency;
// �񵿱� �۾��� �۾�����Ʈ ������ ���� Ŭ����.
// Task �� ���� ���� �� �Ҹ��� ó���Ѵ�.
class CTaskList
{
private:
	//CList<task_group*, task_group*>	m_ListRunningTask;
	CList<task_group*, task_group*>	m_ListFinishedTask;
	CRITICAL_SECTION	m_cs;

	ULONG	m_lCntRunningTask;		// CtaskList ��ü�� �޸𸮿��� �Ҹ� �ɶ� ���� ���� ���� Task �� ������ ����ϱ����� Count

public:
	CTaskList()
	{
		m_lCntRunningTask = 0;
		InitializeCriticalSection(&m_cs);	
	}
	~CTaskList()
	{
		task_group* pTaskGroup = NULL;

		while(m_lCntRunningTask)
			Sleep(0);

		EnterCriticalSection(&m_cs);
		DeleteAllFinishedTask();
		LeaveCriticalSection(&m_cs);
		DeleteCriticalSection(&m_cs);
	}

	// �۾� ���� ����Ʈ�� ����� Task �� �ִٸ� �޸� �Ҹ��� �ϰ� ���ο� Task Group�� ����� ��ȯ�Ѵ�.
	task_group* AllocNewTaskGroup()
	{
		EnterCriticalSection(&m_cs);
		DeleteAllFinishedTask();
		task_group* pTask = new task_group;
		m_lCntRunningTask++;
		LeaveCriticalSection(&m_cs);
		return pTask;
	}

	// Task �� �۾��� ���� �Ǹ� �۾� ���� ����Ʈ�� ����Ѵ�.
	int EndTaskGroup(task_group* pEndTask)
	{
		EnterCriticalSection(&m_cs);
		AddTailFinishedTask(pEndTask);
		m_lCntRunningTask--;
		LeaveCriticalSection(&m_cs);
		return 0;
	}

private:
	int AddTailFinishedTask(task_group* pEndTask)
	{
		POSITION ps = m_ListFinishedTask.AddTail(pEndTask);

		return 0;
	}

	int DeleteAllFinishedTask()
	{
		task_group* pTaskGroup = NULL;

		for(POSITION pos = m_ListFinishedTask.GetHeadPosition(); pos != NULL;)
		{
			pTaskGroup = m_ListFinishedTask.GetNext(pos);

			if (pTaskGroup != NULL)
			{
				pTaskGroup->wait();
				delete pTaskGroup;
				pTaskGroup = NULL;
			}
		}
		m_ListFinishedTask.RemoveAll();
		return 0;
	}
};

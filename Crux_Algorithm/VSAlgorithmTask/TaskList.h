
#pragma once

#include <ppl.h>

using namespace Concurrency;
// 비동기 작업의 작업리스트 관리를 위한 클래스.
// Task 의 동적 생성 및 소멸을 처리한다.
class CTaskList
{
private:
	//CList<task_group*, task_group*>	m_ListRunningTask;
	CList<task_group*, task_group*>	m_ListFinishedTask;
	CRITICAL_SECTION	m_cs;

	ULONG	m_lCntRunningTask;		// CtaskList 객체가 메모리에서 소멸 될때 현재 실행 중인 Task 가 있으면 대기하기위한 Count

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

	// 작업 종료 리스트에 종료된 Task 가 있다면 메모리 소멸을 하고 새로운 Task Group을 만들어 반환한다.
	task_group* AllocNewTaskGroup()
	{
		EnterCriticalSection(&m_cs);
		DeleteAllFinishedTask();
		task_group* pTask = new task_group;
		m_lCntRunningTask++;
		LeaveCriticalSection(&m_cs);
		return pTask;
	}

	// Task 의 작업이 종료 되면 작업 종료 리스트에 등록한다.
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

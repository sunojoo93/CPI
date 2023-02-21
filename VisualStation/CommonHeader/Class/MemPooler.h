/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		MemPooler.h
@brief		메모리 풀의 기능을 제공한다.

@remark		
@author		김용태
@date		2006.06.21
*/

#pragma once

//#pragma warning( disable : 4311 )	//컴파일 경고 메시지를 무시하게 한다. "~에서 ~로 포인터가 잘립니다." 추후 64 bit 환경으로 전환시 검토필요
//#pragma warning( disable : 4312 )	//컴파일 경고 메시지를 무시하게 한다. "~에서 더큰 ~로의 변환입니다." 추후 64 bit 환경으로 전환시 검토필요

#include <assert.h>

/** 
@brief		메모리 풀러

@remark		
-			동적생성이 잦은 메모리를 미리 생성해 놓고 사용할 수 있다.
-			본 Class를 사용함으로 잦은 메모리 new / delete 로 인한 성능 저하를 방지 할 수 있다.
@author		김용태
@date		2006.06.21
*/ 
template<class Type>
class CMemPooler
{
public:
	/** 
	@brief		생성자

	@return		없음
	@remark		Thread 생성시 필요한 인자를 받아서 초기화 한다.
	@author		김용태
	@date		2006.06.21
	*/
	CMemPooler(
		int nNumOfBlock		///< 메모리 생성 개수
		) : m_nNumofBlock(nNumOfBlock), m_pFreeList(NULL), m_pMemBlock(NULL), m_nAllocCount(0)
	{
		//파라메터 유효성 검사
		assert(nNumOfBlock>0);
		//변수 설정
		m_nListBlockSize=sizeof(BlockNode)+sizeof(Type);
		//메모리 생성
		Create();
	}

	/** 
	@brief		소멸자

	@return		없음
	@remark		
	@author		김용태
	@date		2006.06.21
	*/
	~CMemPooler()
	{
		//생성된 메모리 공간을 반환 한다.
		Destroy();
	}

	/** 
	@brief		현재 확보 되어 있는 메모리 공간 중에 하나를 가져온다.

	@return		Template Pointer
	@remark		
	@author		김용태
	@date		2006.06.21
	*/
	Type* Alloc()
	{
	    BlockNode* pNode=NULL;
		Type* pRet=NULL;		

		while(1)
		{
			EnterCriticalSection(&m_cs);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

			//생성한 memory Count 가 최대 개수를 넘지 않는 다면
			if(m_nNumofBlock > m_nAllocCount)
			{
				//사용중이지 않은 메모리 공간의 Pointer를 가져온다.
				pNode=m_pFreeList;

				//현재 할당될 예정인 메모리 공간의 다음 메모리 공간을 가리키는 주소를 사용중이지 않은 메모리 주소로 설정한다.
				m_pFreeList=m_pFreeList->pNext;
				m_nAllocCount++;
				pRet=reinterpret_cast<Type*>(pNode);
				break;
			}
			else
			{
				//이벤트를 Reset 한다.
				ResetEvent(m_hMemFreeEvent);

				LeaveCriticalSection(&m_cs);		//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

				//Thread 를 대기 모드로 진입 시킨다.
				::WaitForSingleObject(m_hMemFreeEvent,INFINITE);
			}

		}

		memset(pRet, 0, sizeof(Type));
		
		LeaveCriticalSection(&m_cs);				//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

		return pRet;
	}

	/** 
	@brief		사용된 메모리를 메모리풀에 반환한다.

	@return		TRUE or FALSE
	@remark		
	@author		김용태
	@date		2006.06.21
	*/
	bool Free(
		Type* freeBlock			///< 반환될 메모리 주소
		)
	{
		BlockNode* pNode=NULL;
        bool bRet=false;

		EnterCriticalSection(&m_cs);				//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
		
		//만일 반환될 주소가 NULL 이 아니면
		if(freeBlock != NULL)
		{
			//신규 노드로 삽입될 노드 인덱스를 가져온다.
			pNode=( reinterpret_cast<BlockNode*>(freeBlock) );
			if(m_nAllocCount>0)
			{
				//반환될 메모리 공간의 주소를 인덱스 처음으로 넣고 할당된 메모리 공간의 개수를 1개 줄인다.
				pNode->pNext=m_pFreeList;
				m_pFreeList=pNode;
				m_nAllocCount--;
				bRet=true;

				//이벤트를 발생 시켜서 대기 중인 Thread를 깨운다.
				SetEvent(m_hMemFreeEvent);
			}
		}

		LeaveCriticalSection(&m_cs);				//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△
		
		return bRet;
	}

	/** 
	@brief		할당된 메모리 개수를 가져온다.

	@return		할당된 메모리 개수
	@remark		
	@author		김용태
	@date		2006.06.21
	*/
	int GetCount()
	{
		return m_nAllocCount;
	}
	
	
protected:
	/** 
	@brief		초기 메모리 공간을 확보하고 Template Size 로 분리하여 할당 Index 를 생성한다.

	@return		없음
	@remark		
	@author		김용태
	@date		2006.06.21
	*/
	void Create()
	{
		//전체 메모리 할당이 필요한 크기만큼을 미리 할당 받는다.
        const int AllocationSize=(m_nListBlockSize) * m_nNumofBlock; // 메모리 할당할 크기 
        m_pMemBlock=VirtualAlloc(NULL,AllocationSize,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		assert(m_pMemBlock);

		//전체 할당된 메모리의 시작 번지를 가져온다.
		BlockNode* pNode = reinterpret_cast<BlockNode*>(m_pMemBlock);

		//전체 메모리 공간 중 끝에서 1 Tamplate Size 만큼 뺀 메모리 번지를 가져온다. 제일 마지막을 구성한다.
		pNode = reinterpret_cast<BlockNode*>((reinterpret_cast<long long>(pNode)) + (m_nNumofBlock-1) * (m_nListBlockSize) );
		//마지막 메모리 공간부터 시작 메모리 주소까지 Tamplate 메모리 시작 주소들을 계산한다.
		for(int i=m_nNumofBlock-1; i>=0; i--)
		{
			pNode->pNext=m_pFreeList; // 처음에는 NULL , 즉 Tail 은 NULL 로 한다.
			m_pFreeList=pNode;
			pNode = reinterpret_cast<BlockNode*>((reinterpret_cast<long long>(pNode)) - m_nListBlockSize);
		}

		//임계영역을 초기화 한다. Multi CPU 를 고려한 Spin Count를 둔다.
		InitializeCriticalSection/*AndSpinCount*/(&m_cs/*,4000*/);

		//대기 쓰레드를 깨우는 이벤트를 생성한다. Memory 가 Full 일경우 대기하는 이벤트이다.
		m_hMemFreeEvent = CreateEvent( NULL, TRUE,  FALSE,  NULL ); 

		//이벤트를 Reset 한다.
		ResetEvent(m_hMemFreeEvent);
	}

	/** 
	@brief		할당된 전체 메모리 공간을 OS 로 반환한다.

	@return		없음
	@remark		
	@author		김용태
	@date		2006.06.21
	*/
	void Destroy()
	{        

		 //할당 받은 메모리 공간을 반환한다.
		if(m_pMemBlock)
		{
			VirtualFree(m_pMemBlock,0,MEM_RELEASE);
		}
		
		//이벤트를 종료 한다.
		if(m_hMemFreeEvent)
		{
			CloseHandle(m_hMemFreeEvent);
		}
			

		//임계영역 종료
		DeleteCriticalSection(&m_cs);
	}

	/** 
	@brief		생성한 메모리를 링크드 리스트처럼 관리를 위한 노드 타입을 정의한다.

	@return		없음
	@remark		
	@author		김용태
	@date		2006.06.21
	*/
	struct BlockNode
	{
		BlockNode* pNext;	///< 다음 node를 가지는 주소
		/** 
		@brief		생성자

		@return		없음
		@remark		다음 node 를 가지는 주소를 NULL로 초기화 한다.
		@author		김용태
		@date		2006.06.21
		*/
		BlockNode()
		{
			pNext=NULL;
		}
	};

	/*맴버변수*/
protected:
	BlockNode* m_pFreeList;						///< 남아 있는 메모리 블럭 리스트
	void* m_pMemBlock;							///< 초기 할당 받은 전체 메모리 공간의 시작 주소
	int m_nNumofBlock;							///< 메모리 할당할 블럭 수
	int m_nListBlockSize;						///< 한 블럭 사이즈 
	int m_nAllocCount;							///< 할당된 메모리 블럭 갯수
	CRITICAL_SECTION m_cs;						///< Thread-Safe를 위한 임계영역
	HANDLE	m_hMemFreeEvent;					///< Memory 할당이 Full 이어서 대기하여야 할 경우 기다리는 이벤트
};

/** 
@}
*/ 
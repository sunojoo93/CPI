#include "stdafx.h"
#include "StatusManager.h"
#include "ErrorProcess.h"
#include "..\..\VisualStation\CommonHeader\ErrorCode\ClientDllErrorCode.h"
//#include "IniProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

StatusManager::StatusManager()
{
}

StatusManager::~StatusManager()
{

}

// Melsec Memory Area を 処理するThreadを開始
int StatusManager::Start()
{
	HANDLE handle;
	StatusManagerQueueParam* param;

	// 상시 감시 버퍼를 사용 하지 않고 로그 기입 기능만 사용할 경우를 위해 COMMENT
	//
	//////// PLC 상시 감시 버퍼를 사용하지 않는 경우 이하 쓰레드 들은 필요 없다.
	//////if (m_nUsePlcBuffer == 0)
	//////{
	//////	return APP_OK;
	//////}

	param = new StatusManagerQueueParam;

	param->data = IN_UPDATE_THREAD;
	m_StatusMessageQueue.m_fnPushMessage(param);

	param = new StatusManagerQueueParam;

	param->data = OUT_UPDATE_THREAD;
	m_StatusMessageQueue.m_fnPushMessage(param);

	param = new StatusManagerQueueParam;

	param->data = USER_DEFINE_UPDATE_THREAD;
	m_StatusMessageQueue.m_fnPushMessage(param);

	handle = m_fnStartThread();
	if ( handle == NULL || handle == (HANDLE)-1 )
		return APP_NG;

	handle = m_fnStartThread();
	if ( handle == NULL || handle == (HANDLE)-1 )
		return APP_NG;

	handle = m_fnStartThread();
	if ( handle == NULL || handle == (HANDLE)-1 )
		return APP_NG;

	return APP_OK;
}

void StatusManager::m_fnRunThread()
{
	StatusManagerQueueParam* param;
	param = m_StatusMessageQueue.m_fnPopMessage();

	switch ( param->data )
	{
	case IN_UPDATE_THREAD:
		delete param;
		InStatusManagerThread();
		break;

	case OUT_UPDATE_THREAD:
		delete param;
		OutStatusManagerThread();
		break;

	case USER_DEFINE_UPDATE_THREAD:
		delete param;
		UserDefineStatusManagerThread();
		break;
	}
}


void StatusManager::InStatusManagerThread()
{
	int nRet = APP_OK;

	return;

	Sleep(3000);

	while (GetThreadRunFlag())
	{

		EXCEPTION_TRY

		
		// 수시로 다른 타스크의 상태 값을 체크하여 자신이 필요한 일을 한다.

		EXCEPTION_CATCH

			
		if ( nRet != APP_OK )
		{

			m_fnPrintLog(_T("ERROR OutBitUpdateThread. Error Code = %d \n"), nRet);

		}
	}
}

void StatusManager::OutStatusManagerThread()
{
	int nRet = APP_OK;

	return;

	Sleep(3000);

	while (GetThreadRunFlag())
	{


		EXCEPTION_TRY


		// 수시로 자신의 상태 값을 체크하여 다른 타스크에 알려주는 일을 한다.




		EXCEPTION_CATCH

		if ( nRet != APP_OK )
		{
			_tprintf(_T("ERROR OutBitUpdateThread. Error Code = %d \n"), nRet);
		}
	}
}


void StatusManager::UserDefineStatusManagerThread()
{

	int		nRet = APP_OK;
	bool	bIsFirstRun	= true;

	return;

	Sleep(3000);

	while (GetThreadRunFlag())
	{
		EXCEPTION_TRY
		
		// 상시로 다른 타스크와 인터페이스를 할 경우 사용하는 쓰레드
		

		EXCEPTION_CATCH

		if ( nRet != APP_OK )
		{
			m_fnPrintLog(_T("ERROR UserDefineStatusManagerThread. Error Code = %d \n"), nRet);

		}

		Sleep(300);

	}
}


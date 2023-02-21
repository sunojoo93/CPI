#ifndef DEFINE_H
#define DEFINE_H

#define VS_SEQUENCE_TASK_INI_FILE			theApp.m_Config.GETDRV() + _T(":\\CRUX\\Data\\INI\\CRUX_Sequence.ini")
#define SEQUENCE_LOG_PATH					theApp.m_Config.GETDRV() + _T(":\\Crux\\DATA\\LOG\\Sequence\\")
#define DEVICE_FILE_PATH					theApp.m_Config.GETDRV() + _T(":\\Crux\\DATA\\INI\\Device.cfg")
#define INIT_FILE_PATH						theApp.m_Config.GETDRV() + _T(":\\Crux\\DATA\\INI\\Initialize.ini")

#include "..\..\CommonHeaders\Structure.h"


// for Tray Icon - User Message
#define	WM_TRAYICON_MSG					WM_USER + 1
#define	WM_PRINT_UI_LOG_MSG_UNICODE		WM_USER + 2
#define	WM_PRINT_UI_LOG_MSG_MULTI_BYTE	WM_USER + 3
#define	WM_WINDOWPOSCHANGING 	WM_USER + 4
#define MAX_GRID_LOG					1000



// 전역으로 선언할 변수들은 여기에 Define 한다

#endif
#ifndef DEFINE_H
#define DEFINE_H

#define LIGHT_LOG_PATH		theApp.GETDRV() + _T(":\\Crux\\DATA\\LOG\\Light\\")
#define DEVICE_FILE_PATH	theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\Device.cfg")
#define INIT_FILE_PATH		theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\Initialize.ini")

#include "..\..\CommonHeaders\Structure.h"

// 전역으로 선언할 변수들은 여기에 Define 한다

#define  MAX_ELP_CNT  1
#define MAX_CMD_LIST 100

// #define HEAD	0x7F
// #define TAIL	0x7E
// #define ID		0x0000

// #define TURN_ON	 0x1063
// #define TURN_OFF 0x1064
// #define HEADER_ID 0x0000
// #define DEFAULT_LENGTH 0x0000

// #define SEL_PATTERN 0x1067
// #define NEXT_PATTERN 0x1065
// #define PREV_PATTERN 0x1066
// #define VOLT_SWEEP	0x1071
// 
// #define ACK_TURN_ON	 0x15
// #define ACK_TURN_OFF 0x06
// #define ACK_CMD_ERROR 0xE0
// #define ACK_CKS_ERROR 0xE1
// #define ACK_SEQ_ERROR 0xE2
// 
// #define SEL_SWEEP_R		0x11
// #define SEL_SWEEP_G		0x12
// #define SEL_SWEEP_B		0x10

#define PATH_PG_INIT_FILE					theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\PGController.ini")

#define RGB_CH								_T("RGB_CH")
#define RGB_CH_INDEX						_T("INDEX")
#define PATTERN_INDEX						_T("PATTERN_INDEX")
#define JUDGE								_T("JUDGE")
#define RED_VOLTAGE							_T("RED_VOLTAGE")
#define GREEN_VOLTAGE						_T("GREEN_VOLTAGE")
#define BLUE_VOLTAGE						_T("BLUE_VOLTAGE")
#define COLOR_X								_T("COLOR_X")
#define COLOR_Y								_T("COLOR_Y")
#define LUMINANCE							_T("LUMINANCE")

#endif
/*
*	File Name			:	ShareMemDLL.h
*	Function			:	상수와, 경로, Error Code 에 관련한 것 들을 정의.
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
#ifndef		SHAREMEMDLL_H
#define		SHAREMEMDLL_H

//Header File Define Start==========================================================
#include<string>
#include<fstream>
//Header File Define End==========================================================

using namespace std;

#pragma warning( disable : 4996 )
//Define Parameter Start==========================================================
#define		FILE_NUMBER						100			//생성할 공유 메모리의 총 파일 수
#define		FILE_NAME						20			//파일 이름의 길이
#define		ID_LENGTH						30			//ID 의 길이
#define		ID_NUMBER						1024		//한 파일당 ID 의 최대 개수
#define		READ_BUFFER						512			//파일을 읽을때 사용하는 버퍼다. 한 라인에 대한 버퍼이다.
#define		FILE_BUFFER						5120		//메모리를 읽을 때 사용하는 버퍼의 최대 크기.
#define		DEFAULT_SIZE					100			//Default Value 의 최대 사이즈.
#define		DEFAULT_INIT					0			//기본 상태로 초기화(파일 넘버일 경우, 전체 파일을 의미, InitType 일 경우 Default Value Load.
#define		ONLY_TABLE						1			//테이블 정보만 다시 로딩.

//Define Parameter End===========================================================


//Error Code Start================================================================
#define		SUCCESS_CODE					00001
#define		NORMAL_ERROR					10001
#define		ERR_SMP_FILE_NUMBER				30000
#define		ERR_SMP_ID_NOT_DEFINE			30001
#define		ERR_SMP_WRITE_DATA				30002
#define		ERR_SMP_ACCESSOR_DATA			30003

#define		ERR_SMP_FILE_NUMBER_MATCH		30010
#define		ERR_SMP_ID_NOT_DEFINE_MATCH		30011
#define		ERR_SMP_WRITE_DATA_MATCH		30012
#define		ERR_SMP_ACCESSOR_DATA_MATCH		30013

#define		ERR_SMP_LISTFILE_NOTFIND		30100
#define		ERR_SMP_DEFINEFILE_NOTFIND		30101
#define		ERR_SMP_NONE_DEFINE				33333


//Error Code  End===============================================================


//Define Path Start===============================================================
#define		SRC_FILE_PATH					"C:\\VisualStation\\Data\\SMD\\"					//SML, SMD 파일이 위치한 파일 Path
#define		BACKUP_FILE						"C:\\VisualStation\\Data\\Backup\\"				//Backup File Path
#define		SML_FILE						"C:\\VisualStation\\Data\\SMD\\SMDFileList.SML"	//SMD File Name
#define		MONITOR_PROCESS					"SMPMonitor"
//Define Path End===============================================================



//Parameter Define Start==========================================================

//Parameter Define End===========================================================
#endif
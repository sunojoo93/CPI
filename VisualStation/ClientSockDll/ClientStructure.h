/**
@file		ClientStructure.h
@brief		Client DLL 내부에서 사용하는 구조체를 정의한다.	
@remark
@author		김용태
@date		2006.
*/

#pragma once

/** 
@brief		Packet 생성을 위한 구조체이다. Memory Pool 생성을 위해 이용된다.
@remark		
@author		김용태
@date		2006.03.15
*/ 
//struct PER_PACKET_BUFFER
//{
//	char cBuff[MAX_BUFFER];		///< MAX_BUFFER byte 크기의 문자 배열
//
//	PER_PACKET_BUFFER()			///< 구조체 생성자, 초기 생성과 동시에 메모리를 초기화 한다.
//	{
//		memset(this, 0x00, sizeof(PER_PACKET_BUFFER));
//	}
//};
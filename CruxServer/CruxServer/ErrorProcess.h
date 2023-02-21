#pragma once


// Inaf DLL Exception Handler
#ifndef _PLC_TASK_EXCEPTION_PROCESS_H 
#define _PLC_TASK_EXCEPTION_PROCESS_H

#include <windows.h>

/************************************************************************/
/*					System Exception Error                              */
/************************************************************************/
#define PLC_TASK_EXCEPTION_ACCESS_VIOLATION				1000	
#define PLC_TASK_EXCEPTION_BREAKPOINT					1001
#define PLC_TASK_EXCEPTION_DATATYPE_MISALIGNMENT		1002
#define PLC_TASK_EXCEPTION_SINGLE_STEP					1003
#define PLC_TASK_EXCEPTION_ARRAY_BOUNDS_EXCEEDED		1004
#define PLC_TASK_EXCEPTION_FLT_DENORMAL_OPERAND			1005
#define PLC_TASK_EXCEPTION_FLT_DIVIDE_BY_ZERO			1006
#define PLC_TASK_EXCEPTION_FLT_INEXACT_RESULT			1007
#define PLC_TASK_EXCEPTION_FLT_INVALID_OPERATION		1008
#define PLC_TASK_EXCEPTION_FLT_OVERFLOW					1009
#define PLC_TASK_EXCEPTION_FLT_STACK_CHECK				1010
#define PLC_TASK_EXCEPTION_FLT_UNDERFLOW				1011
#define PLC_TASK_EXCEPTION_INT_DIVIDE_BY_ZERO			1012
#define PLC_TASK_EXCEPTION_INT_OVERFLOW					1013
#define PLC_TASK_EXCEPTION_PRIV_INSTRUCTION				1014
#define PLC_TASK_EXCEPTION_NONCONTINUABLE_EXCEPTION		1015

#define PLC_TASK_EXCEPTION_UNKNOWN_EXCEPTION			1100

/************************************************************************/
/*							Error Code                                  */
/************************************************************************/

#define PLC_TASK_PARAMETER_INVALID_ERROR				400		// Parameter Invalid Error 

#define PLC_TASK_APPLICATION_ERROR						1200

#define PLC_TASK_INAF_COM_OPEN_ERROR					1300
#define PLC_TASK_INAF_COM_CLOSE_ERROR					1301
#define PLC_TASK_INAF_RESPONSE_SEND_ERROR				1305

#define PLC_TASK_MELSEC_DIVICE_OPEN_ERROR				1400
#define PLC_TASK_MELSEC_DIVICE_CLOSE_ERROR				PLC_TASK_MELSEC_DIVICE_OPEN_ERROR + 10
#define PLC_TASK_MELSEC_DIVICE_READ_ERROR				PLC_TASK_MELSEC_DIVICE_OPEN_ERROR + 20
#define PLC_TASK_MELSEC_DIVICE_READ_RETURN_ERROR		PLC_TASK_MELSEC_DIVICE_OPEN_ERROR + 30
#define PLC_TASK_MELSEC_DEVICE_IS_NOT_OPENED			PLC_TASK_MELSEC_DIVICE_OPEN_ERROR + 40
#define PLC_TASK_MELSEC_DIVICE_WRITE_ERROR				PLC_TASK_MELSEC_DIVICE_OPEN_ERROR + 50
#define PLC_TASK_MELSEC_DIVICE_WRITE_RETURN_ERROR		PLC_TASK_MELSEC_DIVICE_OPEN_ERROR + 60

#define PLC_TASK_SEQUENCE_IS_NOTHING					1500
#define PLC_TASK_SEQUENCE_IS_BUSY						1501
#define PLC_TASK_SEQUENCE_IS_NOT_BUSY					1502
#define PLC_TASK_SEQUENCE_IS_NOT_EXECUTABLE				1505		// Indexer Initial All　中


#define PLC_TASK_SEQUENCE_PARAMETER_IS_WRONG			1511
#define PLC_TASK_SEQUENCE_FAIL							1512


#define PLC_TASK_PLC_RESPONSE_TIMEOUT					2100


/************************************************************************/
/*							Error 処理 MACRO							*/
/************************************************************************/
// C Style
#define START_EXCEPTION		int  uExceptCode; _try 
#define	END_EXCEPTION		__except(uExceptCode = PLC_TASK_Exception_Process(GetExceptionCode())){nRet = uExceptCode;}
// C++ Style 
#define EXCEPTION_TRY	try{
#define EXCEPTION_CATCH	}catch(int nErrCode){nRet = nErrCode;}	catch(...){nRet = PLC_TASK_EXCEPTION_UNKNOWN_EXCEPTION;}


// Exception Error処理関数。
// 実現理由はSystem Error CodeはLONG形だが、ASALM基本関数のReturn形はUSHORTだから、Error Codeの変更のために。
__inline int	PLC_TASK_Exception_Process(long ErrorCode)
{
	int nErrorCode;

	// 必要なら下のCase 内にError処理する。
	switch ( ErrorCode )
	{
	case EXCEPTION_ACCESS_VIOLATION:
		nErrorCode = PLC_TASK_EXCEPTION_ACCESS_VIOLATION;
		break;
	case EXCEPTION_BREAKPOINT:
		nErrorCode = PLC_TASK_EXCEPTION_BREAKPOINT;
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		nErrorCode = PLC_TASK_EXCEPTION_DATATYPE_MISALIGNMENT;
		break;
	case EXCEPTION_SINGLE_STEP:
		nErrorCode = PLC_TASK_EXCEPTION_SINGLE_STEP;
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		nErrorCode = PLC_TASK_EXCEPTION_ARRAY_BOUNDS_EXCEEDED;
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		nErrorCode = PLC_TASK_EXCEPTION_FLT_DENORMAL_OPERAND;
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		nErrorCode = PLC_TASK_EXCEPTION_FLT_DIVIDE_BY_ZERO;
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		nErrorCode = PLC_TASK_EXCEPTION_FLT_INEXACT_RESULT;
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		nErrorCode = PLC_TASK_EXCEPTION_FLT_INVALID_OPERATION;
		break;
	case EXCEPTION_FLT_OVERFLOW:
		nErrorCode = PLC_TASK_EXCEPTION_FLT_OVERFLOW;
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		nErrorCode = PLC_TASK_EXCEPTION_FLT_STACK_CHECK;
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		nErrorCode = PLC_TASK_EXCEPTION_FLT_UNDERFLOW;
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		nErrorCode = PLC_TASK_EXCEPTION_INT_DIVIDE_BY_ZERO;
		break;
	case EXCEPTION_INT_OVERFLOW:
		nErrorCode = PLC_TASK_EXCEPTION_INT_OVERFLOW;
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		nErrorCode = PLC_TASK_EXCEPTION_PRIV_INSTRUCTION;
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		nErrorCode = PLC_TASK_EXCEPTION_NONCONTINUABLE_EXCEPTION;
		break;
	default:
		nErrorCode = PLC_TASK_EXCEPTION_UNKNOWN_EXCEPTION;
		break;
	}

	return nErrorCode;
}


#endif	//_PLC_TASK_EXCEPTION_PROCESS_H
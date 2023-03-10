#pragma once
/*
*	Module Name	:	SharedMemoryRead
*	Parameter			:	File Number, Identify, Return Data 를 저장할 버퍼의 주소,
*	Return				:	해당 메모리 영역의 값, 또는 에러 코드
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryRead() 를 호출한다.
*							SM_MemoryRead() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllimport) int		SharedMemoryRead(int* nFileNumber, char * chIdentify ,char*	chReadReturn, int* nReadLength);

/*
*	Module Name	:	SharedMemoryRead
*	Parameter			:	File Name, Identify, Return Data 를 저장할 버퍼의 주소,
*	Return				:	해당 메모리 영역의 값, 또는 에러 코드
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryRead() 를 호출한다.
*							SM_MemoryRead() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllimport) int		SharedMemoryRead(char* chFileName, char * chIdentify ,char*	chReadReturn,  int* nReadLength);

/*
*	Module Name	:	SharedMemoryRead
*	Parameter			:	File Name, Identify, Return Data 를 저장할 버퍼의 주소,
*	Return				:	해당 메모리 영역의 값, 또는 에러 코드
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryRead() 를 호출한다.
*							SM_MemoryRead() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllimport) int		SharedMemoryRead(int* nFileNumber, int* nStartPosition, int* nLength, char* chReadReturn);

/*
*	Module Name	:	SharedMemoryWrite
*	Parameter			:	FileNumber, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryWrite() 를 호출한다.
*							SM_MemoryWrite() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllimport) int		SharedMemoryWrite(int* nFileNumber, char *chIdentify, char *chWriteData, char *chAccessor);

/*
*	Module Name	:	SharedMemoryWrite
*	Parameter			:	FileName, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryWrite() 를 호출한다.
*							SM_MemoryWrite() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllimport) int		SharedMemoryWrite(char* chFileName, char *chIdentify, char *chWriteData, char *chAccessor);

/*
*	Module Name	:	SharedMemoryWrite
*	Parameter			:	FileName, Identify, WriteData, Accessor
*	Return				:	기능 수행 결과 코드, Success or Error Code.
*	Function			:	타 Process 에서 접근 가능하도록 공개된 Process 이다.
*							InternalServer 에서 호출되어지며, 넘겨받은 파라미터를 이용하여 SM_MemoryWrite() 를 호출한다.
*							SM_MemoryWrite() 에서 넘겨받은 데이터를 다시 InternalServer 로 Return 한다.							
*	Create				:	2006.02.22
*	Author				:	고정진
*	Version				:	1.0
*/
extern "C++" __declspec(dllimport) int		SharedMemoryWrite(int* nFileNumber, int* nStartPosition, int* nLength, char *chWriteData);

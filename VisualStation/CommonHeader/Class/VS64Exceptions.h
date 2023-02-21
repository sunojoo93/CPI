/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		VS64Exceptions.h
@brief		에러 처리에 관련된 Class 를 정의

@remark		
@author		김용태
@date		2007.5.15
*/

#pragma once

/** 
@brief		내부 에러처리시 사용되는 Class

@remark		
-			Complie Option 중 C++ 예외처리 옵션을 <SEH 예외 있음> 으로 설정하고 Thread(Main 과 그밖의 Thread)의 구동부에 
"_set_se_translator(G_fnTransSEH);" 를 추가하면 SEH 와 CEH 가 본 Class 에서 모두 처리 된다.
@author		김용태
@date		2007.5.15
*/ 

class CVs64Exception
{
public:
	/** 
	@brief		생성자

	@return		없음
	@remark		Class 가 생성 되면서 에러 메시지를 초기화 한다.
	@author		김용태
	@date		2007.05.09
	*/
	CVs64Exception()
	{
		memset(m_szErrorMsg, 0, sizeof(m_szErrorMsg));
		lstrcpy(m_szErrorMsg, _T("Error가 발생 되었습니다."));
		m_nErrorCode = 0;
	}

	/** 
	@brief		생성자

	@return		없음
	@remark		Class 가 생성 되면서 에러 메시지를 초기화 한다.
	@author		김용태
	@date		2007.05.09
	*/
	CVs64Exception(const TCHAR* szErrorMsg)
	{
		if(szErrorMsg)
		{
			memset(m_szErrorMsg, 0, sizeof(m_szErrorMsg));
			lstrcpy(m_szErrorMsg, szErrorMsg);
		}
		else
		{
			memset(m_szErrorMsg, 0, sizeof(m_szErrorMsg));
			lstrcpy(m_szErrorMsg, _T("Error가 발생 되었습니다."));
		}
		m_nErrorCode = 0;
	}

	/** 
	@brief		생성자

	@return		없음
	@remark		Class 가 생성 되면서 에러 메시지와 에러 코드를 초기화 한다.
	@author		김용태
	@date		2007.05.09
	*/
	CVs64Exception(const unsigned int nErrorCode)
	{
		memset(m_szErrorMsg, 0, sizeof(m_szErrorMsg));
		lstrcpy(m_szErrorMsg, _T("Error가 발생 되었습니다."));
		m_nErrorCode = nErrorCode;
	}

	/** 
	@brief		생성자

	@return		없음
	@remark		Class 가 생성 되면서 에러 메시지와 에러 코드를 초기화 한다.
	@author		김용태
	@date		2007.05.09
	*/
	CVs64Exception(const TCHAR* szErrorMsg, const unsigned int nErrorCode)
	{
		if(szErrorMsg)
		{
			memset(m_szErrorMsg, 0, sizeof(m_szErrorMsg));
			lstrcpy(m_szErrorMsg, szErrorMsg);
		}
		else
		{
			memset(m_szErrorMsg, 0, sizeof(m_szErrorMsg));
			lstrcpy(m_szErrorMsg, _T("Error가 발생 되었습니다."));
		}
		m_nErrorCode = nErrorCode;
	}

	/** 
	@brief		발생된 에러메시지를 가져온다.

	@return		에러 메시지
	@remark		
	@author		김용태
	@date		2007.05.09
	*/
	const TCHAR* m_fnGetErrorMsg() const
	{
		return &m_szErrorMsg[0];
	}

	/** 
	@brief		발생된 에러 코드를 가져온다.

	@return		Error Code
	@remark		
	@author		김용태
	@date		2007.05.09
	*/
	const unsigned int m_fnGetErrorCode() const
	{
		return m_nErrorCode;
	}

	/**
	@brief		SEH 가 발생되면 CVs64Exception Class 를 초기화 하고, Error 발생된 함수로 Throw 하게 한다.

	@return		없음
	@remark		Complie Option 중 C++ 예외처리 옵션을 <SEH 예외 있음> 으로 설정하고, Thread(Main 과 그밖의 Thread)의 구동부에 "_set_se_translator(G_fnTransSEH);" 를 실행 하여야 한다.
	@author		김용태
	@date		2007.5.15
	*/
	static void  ST_fnTransSEH(unsigned int nCode, struct _EXCEPTION_POINTERS* pExp)
	{
		throw CVs64Exception(_T("Structured Exception Handling 이 발생되었습니다."), nCode);
	}

private:
	TCHAR			m_szErrorMsg[255];			///< Error 메시지
	unsigned int	m_nErrorCode;				///< Error Code
};


/** 
@}
*/ 
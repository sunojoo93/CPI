////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// xtoken - tokenizer
/// 
/// @author   parkkh
/// @date     Tuesday, February 08, 2011  2:11:13 PM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

#ifndef ASSERT
#include <atlbase.h>
#	define ASSERT(x) ATLASSERT(x)
#endif


template<typename CHAR_TYPE>
class XTokenT
{
public :
	XTokenT()
	{
		_str = NULL;
		_cur = NULL;
		_next = NULL;
		_sep = 0;
	}
	~XTokenT()
	{
		if(_str) free(_str);
	}

	void	Init(const CHAR_TYPE* str, CHAR_TYPE sep, int lenChar=-1)
	{
		if(_str) free(_str);
		//_str = _strdup(str);
		_str = my_strdup(str, lenChar);
		_sep = sep;
		if(_str==NULL){ASSERT(0); return; }
		_next = FindNext(_str);
		_cur  = NULL;
	}

	const CHAR_TYPE*	GetNextToken()
	{
		if(_cur==NULL)			// ó�� ȣ���� ���
		{
			_cur = _str;
			return _cur;
		}

		if(_next==NULL) return NULL;

		_cur = _next;
		_next = FindNext(_next);

		return _cur;
	}

private :
	CHAR_TYPE*	FindNext(CHAR_TYPE* p)		// ���� ��ū ���� ��ġ�� ã�´�.
	{
		while(*p)
		{
			if(*p==0) return NULL;			// ���� �����ߴ�.

			if(*p==_sep)					// seperator �� ã�Ҵ�.
			{
				while(*p)
				{
					*p = 0;					// seperator �� 0 ���� �����.
					p++;
					if(*p==0) return NULL;	// ���� �����ߴ�.

					if(*p!=_sep)
						return p;			// ���ο� ��ū�� ���� ��ġ
				}
			}
			p++;
		}
		return NULL;
	}

	CHAR_TYPE* my_strdup(const CHAR_TYPE* str, int lenChar)
	{
		if(str==NULL) return NULL;

		CHAR_TYPE* dest;
		size_t	   len = 0;

		if(lenChar<0)				// ���̰� �������� ���� ��� NULL terminated string ���� �����ϰ� ���̸� ���Ѵ�.
		{
			const CHAR_TYPE	*p;
			for (p = str; *p; ++p);
			len = p - str;
		}
		else len = lenChar;


		dest = (CHAR_TYPE*)malloc(sizeof(CHAR_TYPE) * (len + 1));
		if(dest==NULL){ASSERT(0); return NULL;}

		CHAR_TYPE* q = dest;
		while(len)					// ���̸�ŭ�� �����ϰ�
		{
			*q = *str;
			q++;
			str++;
			len--;
		}
		*q = 0;						// ���������� NULL

		//for (; (*q = *str) != 0; ++q, ++str);			--> NULL ���� �����ϱ� ������ NULL �� ������ �ʴ� ���ڿ������� ���� �߻�

		return dest;
	}


private :
	CHAR_TYPE*	_str;
	CHAR_TYPE*	_cur;
	CHAR_TYPE*	_next;
	CHAR_TYPE	_sep;
};

typedef XTokenT<char> XTokenA;
typedef XTokenT<wchar_t> XTokenW;


#ifdef _UNICODE
#	define XToken XTokenW
#else
#	define XToken XTokenA
#endif

/*

void	Test(TCHAR* str, TCHAR sep)
{
	_tprintf(_T("\nTest() - [%s] [%c]\n"), str, sep);

	XToken			tok;
	const TCHAR*	p;

	tok.Init(str, sep);

	p = tok.GetNextToken();
	while(p)
	{
		_tprintf(_T("====== %s \n"), p);
		p = tok.GetNextToken();
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	Test(_T("Hello:world:!!"), ':');

	Test(_T("Hello:world:!!"), 0);

	Test(NULL, ':');							// assert �׽�Ʈ

	Test(_T("Hello::::::world:::::_:!!"), ':');

	return 0;
}

*/


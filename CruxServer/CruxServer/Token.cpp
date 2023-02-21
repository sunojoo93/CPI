#include "stdafx.h"
#include "Token.h"

////////////////////////////////////////////////////////////////////////////////////
// Tokenizer 클래스

CToken::CToken ( void )
{
	tokenStr = "";
	original = "";
	trimChar = "\n\t\r ";
	moreTokenExist = false;
}

CToken::CToken ( LPCTSTR str )
{
	tokenStr = "";
	original = str;
	trimChar = "\n\t ";
	original.TrimLeft ( trimChar );
	original.TrimRight ( trimChar );

	if ( original != "" ) 
		moreTokenExist = true;
	else 
		moreTokenExist = false;
}

void CToken::TrimChar ( LPCTSTR pTrimChar )
{
	trimChar = pTrimChar;
}

int CToken::GetTokenCount(LPCTSTR delimiter)
{
	int count = 0;
	CToken t(original);
	while (t.IsTokenExist()) {
		t.GetNextToken(_T(" "));
		count ++;
	}

	return count;
}

LPCTSTR CToken::GetNextToken(LPCTSTR delimiter, int offset)
{
	int pos;

	// 더 이상의 토큰이 없을 경우 
	if ( !moreTokenExist )
		return _T("");

	original.TrimLeft ( trimChar );
	original.TrimRight ( trimChar );

	do { 
		pos = original.FindOneOf ( delimiter );

		if ( pos == -1 ) { 
			tokenStr = original;
			tokenStr.TrimLeft ( trimChar );
			tokenStr.TrimRight ( trimChar );
			
			moreTokenExist = false;

			return tokenStr;
		} else {
			tokenStr = original.Left ( pos );
			tokenStr.TrimLeft ( trimChar );
			tokenStr.TrimRight ( trimChar );

			original = original.Mid ( pos + 1 );
			original.TrimLeft ( trimChar );
			original.TrimRight ( trimChar );
		}
		offset--;
	} while ( offset > 0 );

	return tokenStr;
}

LPCTSTR CToken::PeekNextToken ( char * delimiter )
{
	int pos;

	// 더 이상의 토큰이 없을 경우 
	if ( !moreTokenExist )
		return _T("");

	original.TrimLeft ( trimChar );
	original.TrimRight ( trimChar );

	pos = original.FindOneOf ( (LPTSTR)delimiter );

	if ( pos == -1 ) { 
		tokenStr = original;
		tokenStr.TrimLeft ( trimChar );
		tokenStr.TrimRight ( trimChar );
		
		moreTokenExist = false;

		return tokenStr;
	} else {
		tokenStr = original.Left ( pos );
		tokenStr.TrimLeft ( trimChar );
		tokenStr.TrimRight ( trimChar );
	}

	return tokenStr;
}

CString CToken::operator= ( LPCTSTR str )
{
	original = str;
	
	original.TrimLeft ( trimChar );
	original.TrimRight ( trimChar );

	if ( original != "" ) 
		moreTokenExist = true;
	else 
		moreTokenExist = false;

	return original;
}

CToken::operator LPCTSTR() const
{
	return original; 
}

CString CToken::GetString ( void )
{
	return original;
}

void CToken::RemoveRemark( LPCTSTR startRemark )
{
	if ( moreTokenExist ) {
		int pos;
		if ( ( pos = original.Find ( startRemark ) ) != -1 ) {
			original = original.Left ( pos );
			if ( original == "" ) 
				moreTokenExist = false;
		}
	}
}

BOOL CToken::IsTokenExist ( void )
{
	return moreTokenExist;
}

CString CToken::GetStringToken ( char * delimiter ) 
{
	// 더 이상의 토큰이 없을 경우 
	if ( tokenStr == original )
		return _T("");

	original.TrimLeft ( trimChar );
	original.TrimRight ( trimChar );

	if ( original[0] == '\"' ) {
		original = original.Mid(1);
		int pos = original.FindOneOf ( _T("\"") );
		if ( pos == -1 ) { 
			tokenStr = original.Mid(1);
		} else {
			tokenStr = original.Mid ( 0 , pos );
			original = original.Mid ( pos+1 );
		}
		tokenStr.TrimLeft ( trimChar );
		tokenStr.TrimRight ( trimChar );
		return tokenStr;
	} else {
		return GetNextToken ( (LPTSTR)delimiter );
	}
}


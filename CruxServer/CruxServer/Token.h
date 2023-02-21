#ifndef _TOKEN_H_
#define _TOKEN_H_

// Deprecate
class CToken
{
	CString original;
	CString tokenStr;
	CString trimChar;
	bool	moreTokenExist;
public:
	CToken ( void );
	CToken ( LPCTSTR );
	
	int GetTokenCount(LPCTSTR delimiter);
	LPCTSTR GetNextToken(LPCTSTR, int offset = 1);
	LPCTSTR PeekNextToken(char* delimiter);
	CString GetStringToken(char* delimiter);

	CString operator= ( LPCTSTR str );
	operator LPCTSTR() const;
	BOOL IsTokenExist ( void );

	CString GetString ( void );
	void TrimChar ( LPCTSTR trimChar );
	void RemoveRemark( LPCTSTR startRemark );
};

// New Name
//#define CTokenizer CToken

#endif  // _TOKEN_H_

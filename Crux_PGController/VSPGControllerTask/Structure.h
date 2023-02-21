#if !defined(AFX_STRUCTURE_H_INCLUDED_)
#define AFX_STRUCTURE_H_INCLUDED_

#pragma pack(1)

struct STRU_PG_INFO
{
	BOOL bUse;
	BOOL bWithSystemPG;
	int	 nPort;
	int  nBaudRate;
	int  nPG_Kind;
};
// struct STRU_SERIAL_INFO
// {
// 	BOOL bUse;
// 	int	 nPort;
// 	int  nBaudRate;
// 
// 	STRU_SERIAL_INFO()
// 	{
// 		bUse = false;
// 		nPort = 0;
// 		nBaudRate = 0;
// 	};
// };
// 
// struct STRU_LIGHT_INFO
// {
// 	BOOL bUse;
// 	int nCount;
// 	STRU_SERIAL_INFO *stLight;
// 
// 	STRU_LIGHT_INFO()
// 	{
// 		bUse = false;
// 		nCount = 0;
// 	}
// };
#endif

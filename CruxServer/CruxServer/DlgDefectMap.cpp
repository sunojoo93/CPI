
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgDefectMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgDefectMap.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "math.h"
#include "Grid\\GridCtrl.h"
#include "ViewAuto.h"
#include "CruxData.h"


// CDlgDefectMap 대화 상자입니다.
enum		//2016.04.14 Mr.Chu		장축,단축 셋팅 enum
{
	Axis_StartX = 0,
	Axis_EndX,
	Axis_StartY,
	Axis_EndY
};


IMPLEMENT_DYNAMIC(CDlgDefectMap, CDialogEx)

CDlgDefectMap::CDlgDefectMap(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDefectMap::IDD, pParent)
	, m_point(0)
{
	m_bMouseMapClk = FALSE;
	m_nDefectIndex = -1;
	m_bOpenDefectMap = false;
	m_bMapAxisObject = FALSE;

	m_stDefMapData.init(TRUE);
	::InitializeCriticalSection(&m_csMapPaint);	//2018.12.16 add by mby for Debug window
	::InitializeCriticalSection(&m_csMapDrawDef);	//2018.12.16 add by mby for Debug window	
}

CDlgDefectMap::~CDlgDefectMap()
{
	m_stDefMapData.Delete();
	::DeleteCriticalSection(&m_csMapPaint);	//2018.12.16 add by mby for Debug window
	::DeleteCriticalSection(&m_csMapDrawDef);	//2018.12.16 add by mby for Debug window
}

void CDlgDefectMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDefectMap, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgDefectMap 메시지 처리기입니다.


void CDlgDefectMap::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	int nDefectCnt = 0;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if(!(CViewAuto*)pFrame->m_pViewAuto)
		pFrame->m_pViewAuto = (CViewAuto*)pFrame->GetActiveView();

	((CViewAuto*)pFrame->m_pViewAuto)->UpdateData((TRUE));

	if( m_stDefMapData.strBefPanelID == m_stDefMapData.strCurPanelID
		&& m_stDefMapData.strBefInspDate == m_stDefMapData.strCurInspDate 
		&& m_bOpenDefectMap)
			nDefectCnt = m_fnLoadDefectMap(&dc);
	else
	{
		//m_fnDrawFrame(&dc);	
		//m_fnDrawPanel(&dc);	

		//nDefectCnt = m_fnDrawDefect(&dc, FALSE);		// Defect 그리기
		nDefectCnt = TriggerDefectMap(FALSE);
	}

	if(nDefectCnt == NULL || nDefectCnt < m_nDefectIndex)	//Defect 갯수가 0이거나 Index정보와 불일치 시 Defect 위치만 출력
		return;
	EnterCriticalSection(&m_csMapPaint);	//2018.12.16 add by mby for Debug window
	if( m_bMouseMapClk == FALSE)
		m_fnDrawCrossLine(&dc, m_nDefectIndex);
	else if(m_bMouseMapClk)
		m_fnDrawPoint(&dc,nDefectCnt);
	LeaveCriticalSection(&m_csMapPaint);	//2018.12.16 add by mby for Debug window
}

void CDlgDefectMap::m_fnSetPanelSize(int nSizeX, int nSizeY)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	int nTemp = 0;
	int nReSize = 0;
	int nReSizeLength = 0;
	double dReSizeRaito = 0.00;
	//2016.02.24 Mr.Chu
	CString strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	int nEqpSizeType = GetPrivateProfileInt(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0, strConfigFile); //;중형 0 소형 = 1 대형 = 2
	
	//((CViewAuto*)pFrame->m_pViewAuto)->UpdateData((TRUE));

	RECT rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);


	if( nSizeX > nSizeY )
	{
		//m_bMapAxisObject = TRUE;		//Defect Map의 Axis 구분 변수

 		nReSizeLength = (double)(rc.right - rc.left)*(double)nSizeY/(double)nSizeX;//(double)nSizeX/(double)nSizeY;
 
 		nReSize = (( rc.bottom - rc.top ) - nReSizeLength)/2;

		rc.top	  = rc.top		 + nReSize;
		rc.bottom = rc.bottom	 - nReSize;
	}
	else
	{
		//m_bMapAxisObject = TRUE;

//  		dReSizeRaito = (double)nSizeX/(double)nSizeY;
//  
//  		int nReSizeLength = (int)(dReSizeRaito * (double)( rc.bottom - rc.top ));
//  
//  		nReSize = (( rc.bottom - rc.top ) - nReSizeLength)/2;
		nReSizeLength = ( rc.bottom - rc.top )*(double)nSizeX/(double)nSizeY;
		nReSize = (( rc.right - rc.left ) - nReSizeLength)/2;

		rc.left	 = rc.left	 + nReSize;
		rc.right = rc.right	 - nReSize;
	}
	m_nPanelSizeX = nSizeX;
	m_nPanelSizeY = nSizeY;
	
	m_fRatioPanelXY = (float)MAP_PANEL_SIZE_Y / (float)MAP_PANEL_SIZE_X;

		m_rcPanel.left		= rc.left	+ MAP_PANEL_OFFSET_X;
		m_rcPanel.top		= rc.top	+ MAP_PANEL_OFFSET_Y;
		m_rcPanel.right		= rc.right  - MAP_PANEL_OFFSET_X;
		m_rcPanel.bottom	= rc.bottom - MAP_PANEL_OFFSET_Y;  //(LONG)((m_rcPanel.right - m_rcPanel.left) * m_fRatioPanelXY);



	m_fnCalcRatioCoordXY(MAP_PANEL_SIZE_X, MAP_PANEL_SIZE_Y,nEqpSizeType);
}

void CDlgDefectMap::m_fnCalcRatioCoordXY(int nPanelSizeX, int nPanelSizeY, int nEqpSizeType) //2016.02.24 Mr.Chu
{
	RECT rc;

 	//2015.03.02 add by IKM - S
 	m_fAlignPositionX =NULL;
 	m_fAlignPositionY =NULL;
 
 	rc = m_rcPanel;
 	rc.left   = m_rcPanel.left   + m_fAlignPositionX;
 	rc.right  = m_rcPanel.right  - m_fAlignPositionX;
 	rc.top    = m_rcPanel.top    + m_fAlignPositionY;
 	rc.bottom = m_rcPanel.bottom - m_fAlignPositionY;
 
 	m_fRatioCoordX = (float)(rc.right - rc.left) / nPanelSizeX;
 	m_fRatioCoordY = (float)(rc.bottom - rc.top) / nPanelSizeY;
 	
 	m_fRatioCoordX *=  (float)(nPanelSizeX) / m_nPanelSizeX; 
 	m_fRatioCoordY *=  (float)(nPanelSizeY) / m_nPanelSizeY;

}

void CDlgDefectMap::m_fnDrawFrame(CDC* pDC)
{
	RECT rcMap;
	GetWindowRect(&rcMap);
	ScreenToClient(&rcMap);

	m_bmpPaint.DeleteObject();
	m_dcMem.DeleteDC();

	m_bmpPaint.CreateCompatibleBitmap(pDC,rcMap.right,rcMap.bottom);

	m_dcMem.CreateCompatibleDC(pDC);  // 메모리 DC 생성

	m_dcMem.SelectObject(GetStockObject(GRAY_BRUSH));
	m_dcMem.SelectObject(GetStockObject(BLACK_PEN));
	m_dcMem.SelectObject(m_bmpPaint);
	m_dcMem.Rectangle(rcMap.left, rcMap.top, rcMap.right, rcMap.bottom);
}

void CDlgDefectMap::m_fnDrawAlignMark(CDC* pDC, int nPosX, int nPosY)
{
	RECT rcMap;
	GetWindowRect(&rcMap);
	ScreenToClient(&rcMap);
	m_dcMem.SelectObject(GetStockObject(WHITE_BRUSH));
	m_dcMem.SelectObject(GetStockObject(NULL_PEN));

	RECT rcMark;

	rcMark.left = nPosX - 9;
	rcMark.top = nPosY - 3;
	rcMark.right = nPosX + 9;
	rcMark.bottom = nPosY + 3;

	m_dcMem.Rectangle(&rcMark);

	rcMark.left = nPosX - 3;
	rcMark.top = nPosY - 9;
	rcMark.right = nPosX + 3;
	rcMark.bottom = nPosY + 9;

	m_dcMem.Rectangle(&rcMark);

	pDC->BitBlt(0, 0, rcMap.right - rcMap.left , rcMap.bottom - rcMap.top, &m_dcMem, 0, 0, SRCCOPY);	
}

void CDlgDefectMap::m_fnDrawPanel(CDC* pDC)
{
	RECT rcMap;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	GetWindowRect(&rcMap);
	ScreenToClient(&rcMap);
	
	CBrush newPen,*pOldPen;	
	//pDC->SelectObject(GetStockObject(WHITE_PEN));
	newPen.CreateSolidBrush(RGB(69, 61, 65));	
	pOldPen  = m_dcMem.SelectObject(&newPen);				
	//2015.03.02 add by IKM - S
	//pDC->Rectangle(&m_rcPanel);

	m_fnSetPanelSize(pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth,pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight);

	m_dcMem.SelectObject(m_bmpPaint);

	m_dcMem.Rectangle(&m_rcPanel);
	m_dcMem.SelectObject(pOldPen);						
	newPen.DeleteObject();							
	
	//m_fnDrawAlignMark(pDC, m_rcPanel.left + 20, m_rcPanel.top + 20);

	int nEqpSizeType = 0;
	nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);
	if (nEqpSizeType == EQP_TYPE_MEDIUM)
	{
		m_fnDrawAlignMark(pDC, m_rcPanel.left + 20, m_rcPanel.bottom - 20);
		m_fnDrawAlignMark(pDC, m_rcPanel.right - 20, m_rcPanel.bottom - 20);
	}
	else if(nEqpSizeType == EQP_TYPE_SMALL)
	{
		m_fnDrawAlignMark(pDC, m_rcPanel.right - 20, m_rcPanel.top + 20);
		m_fnDrawAlignMark(pDC, m_rcPanel.right - 20, m_rcPanel.bottom - 20);
	}
	else		//2016.04.25 Mr.Chu		Align Mark를 Origin 위치로 표시한다
	{
		switch(pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType)
		{
		case RECIPE_DEF_MAP_ORIGIN_LT:
			m_fnDrawAlignMark(pDC, m_rcPanel.left, m_rcPanel.top );
			break;
		case RECIPE_DEF_MAP_ORIGIN_LB:
			m_fnDrawAlignMark(pDC, m_rcPanel.left, m_rcPanel.bottom );
			break;
		case RECIPE_DEF_MAP_ORIGIN_RT:
			m_fnDrawAlignMark(pDC, m_rcPanel.right, m_rcPanel.top );
			break;
		case RECIPE_DEF_MAP_ORIGIN_RB:
			m_fnDrawAlignMark(pDC, m_rcPanel.right, m_rcPanel.bottom );
			break;
		default:
			break;
		}
	}
	//2015.03.02 add by IKM - E
}

int CDlgDefectMap::m_fnLoadDefectMap(CDC* pDC)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int nDefectCnt = 0;

	if (((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_CURRENT)	//2015.07.06
	{
		nDefectCnt = pFrame->m_DataManager.GetDefectCount();
	}
	else if(((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_REVIEW)
	{
		nDefectCnt = pFrame->m_DataManager.GetReviewDefectCount();
		//2016.03.21 Mr.Chu		대형 55Inch 일 경우 Defect Map에서 55인치 크기로 뿌린다.
// 		if( pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && !strncmp(pFrame->m_DataManager.m_cRevPanelInfo, "N4A", strlen("N4A")) )
// 			m_fnSetPanelSize(1219 , 696);
	}

	CBitmap *pbmpOld;
 	pDC->BitBlt(0, 0, m_bm.bmWidth, m_bm.bmHeight, &m_dcMem, 0, 0, SRCCOPY);  // 페인트 DC에 복사
 	m_dcMem.SelectObject(&pbmpOld);
 	//m_dcMem.DeleteDC();  

	return nDefectCnt;
}

int CDlgDefectMap::m_fnDrawDefect(CDC* pDC,BOOL bSave)		//4
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	double dMapCoordStartX = 0,dMapCoordEndX = 0, dMapCoordStartY = 0, dMapCoordEndY = 0;	//2016.04.14 Mr.Chu
	int nDefectCnt = 0;
	STRU_DEFECT_INFO* pDefectInfo;
	int nBaseX=NULL,nBaseY=NULL;

	if (((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_CURRENT)	//2015.07.06
	{
		nDefectCnt = pFrame->m_DataManager.GetDefectCount();
	}
	else if(((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_REVIEW)
	{
		nDefectCnt = pFrame->m_DataManager.GetReviewDefectCount();
		//2016.03.21 Mr.Chu		대형 55Inch 일 경우 Defect Map에서 55인치 크기로 뿌린다.
// 		if( pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && !strncmp(pFrame->m_DataManager.m_cRevPanelInfo, "N4A", strlen("N4A")) )
// 			m_fnSetPanelSize(1219 , 696);
	}

	m_stDefMapData.SaveCompData();

	m_stDefMapData.init(nDefectCnt);

	//if(nDefectCnt == 0)	return NULL;

	CBrush newBrush, *oldBrush;
	CPen   newPen  , *pOldPen;	// 2015.01.12 ljk

	int nType = 0;

	//2015.03.02 add by IKM - S
	switch(pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType)
	{
	case RECIPE_DEF_MAP_ORIGIN_LT:
		nBaseX = m_rcPanel.left		;//+ m_fAlignPositionX;//+ (960 * m_fRatioCoordX);//+ m_fAlignPosition;
		nBaseY = m_rcPanel.top		;//+ m_fAlignPositionY;//+ (540 * m_fRatioCoordY);// + m_fAlignPosition ;
		break;
	case RECIPE_DEF_MAP_ORIGIN_LB:
		nBaseX = m_rcPanel.left		;//+ m_fAlignPositionX;//+ (960 * m_fRatioCoordX);//+ m_fAlignPosition;
		nBaseY = m_rcPanel.bottom	;//- m_fAlignPositionY;//+ (540 * m_fRatioCoordY);// + m_fAlignPosition ;
		break;
	case RECIPE_DEF_MAP_ORIGIN_RT:
		nBaseX = m_rcPanel.right	;//- m_fAlignPositionX;//+ (960 * m_fRatioCoordX);//+ m_fAlignPosition;
		nBaseY = m_rcPanel.top		;//+ m_fAlignPositionY;//+ (540 * m_fRatioCoordY);// + m_fAlignPosition ;
		break;
	case RECIPE_DEF_MAP_ORIGIN_RB:
		nBaseX = m_rcPanel.right	;//- m_fAlignPositionX;//+ (960 * m_fRatioCoordX);//+ m_fAlignPosition;
		nBaseY = m_rcPanel.bottom	;//	- m_fAlignPositionY;//+ (540 * m_fRatioCoordY);// + m_fAlignPosition ;
		break;
	}

	//2015.03.02 add by IKM - E

	for(int i = 0; i < nDefectCnt; i++)
	{
		if (((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_CURRENT)	//2015.07.06
		{
			pDefectInfo = pFrame->m_DataManager.GetDefectInfo(i);
		}
		else if(((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_REVIEW)
		{
			pDefectInfo = pFrame->m_DataManager.GetReviewDefectInfo(i);
		}

		//2017.07.12
		if( pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat == TRUE )	
		{
			double dTempStart = 0, dTempEnd = 0;

			//Origin 위치 LT 위치로 변경
			switch( pFrame->m_pVsSeverIF->m_nRecvOriginType )
			{
			case RECIPE_DEF_MAP_ORIGIN_LT:
				dMapCoordStartX = pDefectInfo->Dinfo.Coord_Start_X;
				dMapCoordEndX	= pDefectInfo->Dinfo.Coord_End_X;

				dMapCoordStartY = pDefectInfo->Dinfo.Coord_Start_Y;
				dMapCoordEndY	= pDefectInfo->Dinfo.Coord_End_Y;
				break;

			case RECIPE_DEF_MAP_ORIGIN_LB:
				dMapCoordStartX = pDefectInfo->Dinfo.Coord_Start_X;
				dMapCoordEndX	= pDefectInfo->Dinfo.Coord_End_X;

				dMapCoordStartY	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight - pDefectInfo->Dinfo.Coord_Start_Y;
				dMapCoordEndY	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight - pDefectInfo->Dinfo.Coord_End_Y;
				break;

			case RECIPE_DEF_MAP_ORIGIN_RT:
				dMapCoordStartX	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth - pDefectInfo->Dinfo.Coord_Start_X;
				dMapCoordEndX	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth - pDefectInfo->Dinfo.Coord_End_X;

				dMapCoordStartY = pDefectInfo->Dinfo.Coord_Start_Y;
				dMapCoordEndY	= pDefectInfo->Dinfo.Coord_End_Y;
				break;

			case RECIPE_DEF_MAP_ORIGIN_RB:
				dMapCoordStartY	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight - pDefectInfo->Dinfo.Coord_Start_Y;
				dMapCoordEndY	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight - pDefectInfo->Dinfo.Coord_End_Y;

				dMapCoordStartX	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth - pDefectInfo->Dinfo.Coord_Start_X;
				dMapCoordEndX	= pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth - pDefectInfo->Dinfo.Coord_End_X;
				break;

			default:
				dMapCoordStartX = pDefectInfo->Dinfo.Coord_Start_X;
				dMapCoordEndX	= pDefectInfo->Dinfo.Coord_End_X;

				dMapCoordStartY = pDefectInfo->Dinfo.Coord_Start_Y;
				dMapCoordEndY	= pDefectInfo->Dinfo.Coord_End_Y;
				break;
			}

			if( pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType == TRUE )
			{
				dTempStart	= dMapCoordStartX;
				dTempEnd	= dMapCoordEndX;

				dMapCoordStartX	= dMapCoordStartY;
				dMapCoordEndX	= dMapCoordEndY;
				dMapCoordStartY	= dTempStart;
				dMapCoordEndY	= dTempEnd;
			}
		}
		else if( pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat == FALSE )
		{
			//2016.04.14 Mr.Chu		장축, 단축에 따른 X,Y 셋팅
			if( pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType == FALSE )
			{
				dMapCoordStartX = pDefectInfo->Dinfo.Coord_Start_X;
				dMapCoordEndX	= pDefectInfo->Dinfo.Coord_End_X;
				dMapCoordStartY = pDefectInfo->Dinfo.Coord_Start_Y;
				dMapCoordEndY	= pDefectInfo->Dinfo.Coord_End_Y;
			}
			else
			{
				dMapCoordStartX = pDefectInfo->Dinfo.Coord_Start_Y;
				dMapCoordEndX	= pDefectInfo->Dinfo.Coord_End_Y;
				dMapCoordStartY = pDefectInfo->Dinfo.Coord_Start_X;
				dMapCoordEndY	= pDefectInfo->Dinfo.Coord_End_X;
			}
		}
		//

		//2016.02.24 Mr.Chu			상하좌우 반전
		switch(pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType)
		{
		case RECIPE_DEF_MAP_ORIGIN_LT:
			//2015.03.02 add by IKM - S	
			m_stDefMapData.rcDefect[i].left		= (LONG)(nBaseX + (dMapCoordStartX * m_fRatioCoordX)) ;
			m_stDefMapData.rcDefect[i].right	= (LONG)(nBaseX + (dMapCoordEndX * m_fRatioCoordX) + 1 );

			m_stDefMapData.rcDefect[i].top		= (LONG)(nBaseY + (dMapCoordStartY * m_fRatioCoordY)  );
			m_stDefMapData.rcDefect[i].bottom	= (LONG)(nBaseY + (dMapCoordEndY * m_fRatioCoordY) + 1) ;
			break;
 		case RECIPE_DEF_MAP_ORIGIN_LB:
 			//2015.03.02 add by IKM - S	
			m_stDefMapData.rcDefect[i].left		= (LONG)(nBaseX + (dMapCoordStartX * m_fRatioCoordX) );
			m_stDefMapData.rcDefect[i].right	= (LONG)(nBaseX + (dMapCoordEndX * m_fRatioCoordX) + 1);

			m_stDefMapData.rcDefect[i].top		= (LONG)(nBaseY - (dMapCoordStartY * m_fRatioCoordY)  );
			m_stDefMapData.rcDefect[i].bottom	= (LONG)(nBaseY - (dMapCoordEndY * m_fRatioCoordY) + 1);
 			break;
 		case RECIPE_DEF_MAP_ORIGIN_RT:
 			//2015.03.02 add by IKM - S	
 			m_stDefMapData.rcDefect[i].left		= (LONG)(nBaseX - (dMapCoordStartX * m_fRatioCoordX) );
 			m_stDefMapData.rcDefect[i].right	= (LONG)(nBaseX - (dMapCoordEndX * m_fRatioCoordX) + 1);
 
 			m_stDefMapData.rcDefect[i].top		= (LONG)(nBaseY + (dMapCoordStartY * m_fRatioCoordY)  );
 			m_stDefMapData.rcDefect[i].bottom	= (LONG)(nBaseY + (dMapCoordEndY * m_fRatioCoordY) + 1);
 			break;
 		case RECIPE_DEF_MAP_ORIGIN_RB:
 			//2015.03.02 add by IKM - S	
			m_stDefMapData.rcDefect[i].left		= (LONG)(nBaseX - (dMapCoordStartX * m_fRatioCoordX) );
			m_stDefMapData.rcDefect[i].right	= (LONG)(nBaseX - (dMapCoordEndX * m_fRatioCoordX) + 1);
 
			m_stDefMapData.rcDefect[i].top		= (LONG)(nBaseY - (dMapCoordStartY * m_fRatioCoordY)  );
			m_stDefMapData.rcDefect[i].bottom	= (LONG)(nBaseY - (dMapCoordEndY * m_fRatioCoordY) + 1);
 			break;
 		}

		if(m_stDefMapData.rcDefect[i].left <= m_rcPanel.left)
			m_stDefMapData.rcDefect[i].left = m_rcPanel.left;
		else if(m_stDefMapData.rcDefect[i].left >= m_rcPanel.right)
			m_stDefMapData.rcDefect[i].left = m_rcPanel.right;

		if(m_stDefMapData.rcDefect[i].right >= m_rcPanel.right)
			m_stDefMapData.rcDefect[i].right = m_rcPanel.right;
		else if(m_stDefMapData.rcDefect[i].right <= m_rcPanel.left)
			m_stDefMapData.rcDefect[i].right = m_rcPanel.left;

		if(m_stDefMapData.rcDefect[i].top <= m_rcPanel.top)
			m_stDefMapData.rcDefect[i].top = m_rcPanel.top;
		else if(m_stDefMapData.rcDefect[i].top >= m_rcPanel.bottom)
			m_stDefMapData.rcDefect[i].top = m_rcPanel.bottom;

		if(m_stDefMapData.rcDefect[i].bottom >= m_rcPanel.bottom)
			m_stDefMapData.rcDefect[i].bottom = m_rcPanel.bottom;
		else if(m_stDefMapData.rcDefect[i].bottom <= m_rcPanel.top)
			m_stDefMapData.rcDefect[i].bottom = m_rcPanel.top;

		RECT rcDrawDefect;
		rcDrawDefect.left	=	m_stDefMapData.rcDefect[i].left;	
		rcDrawDefect.top	=	m_stDefMapData.rcDefect[i].top;	
		rcDrawDefect.right	=	m_stDefMapData.rcDefect[i].right;	
		rcDrawDefect.bottom	=	m_stDefMapData.rcDefect[i].bottom;

		rcDrawDefect.right++;	
		rcDrawDefect.bottom++;
		
		//2015.03.02 add by IKM - E

		if(pDefectInfo->Dinfo.Defect_Type == 0)			newPen.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));	// 2015.01.12 ljk
		else if(pDefectInfo->Dinfo.Defect_Type == 1)	newPen.CreatePen(PS_SOLID, 2, RGB(0,   255, 255));	// 2015.01.12 ljk
		else if(pDefectInfo->Dinfo.Defect_Type == 2)	newPen.CreatePen(PS_SOLID, 2, RGB(255,   0, 255));	// 2015.01.12 ljk
		else if(pDefectInfo->Dinfo.Defect_Type == 3)	newPen.CreatePen(PS_SOLID, 2, RGB(255, 128, 192));	// 2015.01.12 ljk
		else if(pDefectInfo->Dinfo.Defect_Type == 4)	newPen.CreatePen(PS_SOLID, 2, RGB(128, 255, 255));	// 2015.01.12 ljk

		if(pDefectInfo->Hinfo.Insp_EQP_No == 0)			newBrush.CreateSolidBrush(COLOR_MAP_WHITE);
		else if(pDefectInfo->Hinfo.Insp_EQP_No == 1)	newBrush.CreateSolidBrush(COLOR_MAP_RED);
		else if(pDefectInfo->Hinfo.Insp_EQP_No == 2)	newBrush.CreateSolidBrush(COLOR_MAP_GREEN);
		else if(pDefectInfo->Hinfo.Insp_EQP_No == 3)	newBrush.CreateSolidBrush(COLOR_MAP_BLUE);

		pOldPen  = pDC->SelectObject(&newPen);		// 2015.01.12 ljk
		oldBrush = pDC->SelectObject(&newBrush); 

		//pDC->Rectangle(&rcDefect);						// 랜더링 속도가 느림
		m_dcMem.SelectObject(m_bmpPaint);
		m_dcMem.PatBlt(rcDrawDefect.left, rcDrawDefect.top, rcDrawDefect.right - rcDrawDefect.left, rcDrawDefect.bottom - rcDrawDefect.top, WHITENESS);	

		pDC->SelectObject(pOldPen);						// 2015.01.12 ljk
		newPen.DeleteObject();							// 2015.01.12 ljk
		pDC->SelectObject(oldBrush);
		newBrush.DeleteObject();
	}
	//G_AddLog_SEQ(2, _T("m_fnDrawDefect()_Before Enter DefectMap")); 

	m_fnMakeDefectMap(pDC, bSave);	//Loading 할 Bmp Buffer 생성

	pDC->BitBlt(0, 0, m_bm.bmWidth, m_bm.bmHeight, &m_dcMem, 0, 0, SRCCOPY);  // 페인트 DC에 복사

	//
	pDefectInfo = NULL;


	m_bOpenDefectMap = true;

	return nDefectCnt;
}

void CDlgDefectMap::m_fnDrawCrossLine(CDC* pDC, int nIndex)
{
	if(m_nDefectIndex == -1)	return;
	if (nIndex < 1) return;

	CPen   newPen  , *pOldPen;

	newPen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));

	pOldPen  = pDC->SelectObject(&newPen);
	//pDC->SelectObject(pOldPen);	//2018.07.16
	//newPen.DeleteObject();	//2018.07.16
		
	//2015.03.02 add by IKM - S		
	int nDefSizeX=0,nDefSizeY=0;
																																
	nDefSizeY  =  (( m_stDefMapData.rcDefect[nIndex-1].bottom	- m_stDefMapData.rcDefect[nIndex-1].top) / 2);

	nDefSizeX  =  ((m_stDefMapData.rcDefect[nIndex-1].right		- m_stDefMapData.rcDefect[nIndex-1].left) / 2);

	pDC->MoveTo(m_stDefMapData.rcDefect[nIndex-1].left + nDefSizeX, 0);
	pDC->LineTo(m_stDefMapData.rcDefect[nIndex-1].left + nDefSizeX, 1000);
	pDC->MoveTo(0,		m_stDefMapData.rcDefect[nIndex-1].top + nDefSizeY);
	pDC->LineTo(1000,   m_stDefMapData.rcDefect[nIndex-1].top + nDefSizeY);
	//2015.03.02 add by IKM - E
}

//2015.06.17 Chu
void CDlgDefectMap::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_point = point;

	m_bMouseMapClk = TRUE;

	Invalidate(TRUE);

	CDialogEx::OnLButtonDown(nFlags, point);
}

//2015.06.17 Chu
//기능 : Defect Map 마우스 좌표와 근접한 결함 선택
void CDlgDefectMap::m_fnDrawPoint(CDC* pDC,int nDefectCnt)
{
	if(m_bOpenDefectMap == false)	return;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	double dMapCoordStartX = 0,dMapCoordEndX = 0, dMapCoordStartY = 0, dMapCoordEndY = 0;	//2016.04.14 Mr.Chu

	//////Defect Manager
	STRU_DEFECT_INFO* pCurDefectInfo;
	int nCloseData = 0;
	BOOL bCompareData = false;	// 20150630_1330 OK상태인 Stage 변경 시 프로그램 다운현상 보완을 위한 예외처리
	int nDefSizeX = 0, nDefSizeY = 0;
	LONG nCompareX = 0,nCompareY = 0, nCompareVal1 = 0, nCompareVal2 = 100000;

	///////////////////Find Defect Data////////////////////
 	for(int i=0;i<nDefectCnt; i++)
	{
		nDefSizeY  =  (( m_stDefMapData.rcDefect[i].bottom - m_stDefMapData.rcDefect[i].top) / 2) + m_stDefMapData.rcDefect[i].top;
		nDefSizeX  =  ((m_stDefMapData.rcDefect[i].right - m_stDefMapData.rcDefect[i].left) / 2)  + m_stDefMapData.rcDefect[i].left;

		//////////////////Compare Defect Data///////////////
		nCompareVal1 = (LONG)sqrt((double)((m_point.y-nDefSizeY)*(m_point.y-nDefSizeY) + (m_point.x-nDefSizeX)*(m_point.x-nDefSizeX)));

		if(nCompareVal2 > nCompareVal1) 
		{
			nCompareVal2 = nCompareVal1;

			nCloseData = i;
			bCompareData = true;
		}
	}

	if( bCompareData == false )
		return;

	///////////////////////Draw Point Defect Map//////////////////////////
	m_nDefectIndex = nCloseData+1;

	m_fnDrawCrossLine(pDC,nCloseData+1);

	m_bMouseMapClk = FALSE;
	/////////////Clicked Grid Point////////////////
	CCruxData*	m_pCruxData = CCruxData::m_fnGetInstance();	
	m_pViewAuto = m_pCruxData->m_fnGetViewPtr(eFORM_Auto);

	if (((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_CURRENT)	//2015.07.06
	{
		((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->SetRowCount(nDefectCnt + 1);
		((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->SetColumnCount(21);
		((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->SetListMode(TRUE);
		((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->SetSingleRowSelection(TRUE);
	}
	else if (((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_REVIEW)
	{
		((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->SetRowCount(nDefectCnt + 1);
		((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->SetColumnCount(21);
		((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->SetListMode(TRUE);
		((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->SetSingleRowSelection(TRUE);
	}

	CCellID CelID;
	CelID.col = TRUE;
	CelID.row = nCloseData + 1;//pCurDefectInfo->Dinfo.DEFECT_NO;
	
	if (((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_CURRENT)	//2015.07.06
	{
		((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->SelectRows(CelID);
		((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->SetTimer(WM_LBUTTONDOWN, 25, 0);

		int nScrollDistVal = ((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->m_nVScrollMax * ( CelID.row -4 ) / nDefectCnt;
		((CViewAuto*)m_pViewAuto)->m_GridDefectSummary->SetScrollPos32(1,nScrollDistVal,1);
	}
	else if (((CViewAuto*)pFrame->m_pViewAuto)->m_nViewDefectSummary == DEFECT_REVIEW)
	{
		((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->SelectRows(CelID);
		((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->SetTimer(WM_LBUTTONDOWN, 25, 0);

		int nScrollDistVal = ((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->m_nVScrollMax * ( CelID.row  -4) / nDefectCnt;
		((CViewAuto*)m_pViewAuto)->m_GridReviewDefectSummary->SetScrollPos32(1,nScrollDistVal,1);
	}
	pCurDefectInfo = NULL;
}

void CDlgDefectMap::m_fnMakeDefectMap(CDC* pDC, BOOL bSave)
{
	// BitMap Buffer로 저장
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	HANDLE handle;
	LPBITMAPINFOHEADER lpbi;
	HANDLE hDIB;   
	HPALETTE hPal=NULL;
	DWORD dwLen;

	if (hPal == NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	GetObject(HBITMAP(m_bmpPaint), sizeof(BITMAP), &m_bm);

	m_bi.biSize = sizeof(BITMAPINFOHEADER);
	m_bi.biWidth = m_bm.bmWidth;
	m_bi.biHeight = m_bm.bmHeight;
	m_bi.biPlanes = 1;
	m_bi.biBitCount = 24;//32;       
	m_bi.biCompression = BI_RGB;
	m_bi.biSizeImage = m_bm.bmWidth * m_bm.bmHeight * 3;
	m_bi.biXPelsPerMeter = 0;
	m_bi.biYPelsPerMeter = 0;
	m_bi.biClrUsed = 0;
	m_bi.biClrImportant = 0;

	int nColors = (1 << m_bi.biBitCount);

	if( nColors > 256 ) 
		nColors = 0;


	dwLen  = m_bi.biSize + nColors * sizeof(RGBQUAD);

	hPal = SelectPalette(pDC->GetSafeHdc(),hPal,FALSE);

	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = m_bi;

	GetDIBits(pDC->GetSafeHdc(), 
		HBITMAP(m_bmpPaint), 
		0, 
		(DWORD)m_bi.biHeight,
		(LPBYTE)NULL, 
		(LPBITMAPINFO)lpbi, 
		(DWORD)DIB_RGB_COLORS);

	m_bi = *lpbi;

	if (m_bi.biSizeImage == 0)
		m_bi.biSizeImage = ((((m_bi.biWidth * m_bi.biBitCount) + 31) & ~31) / 8) * m_bi.biHeight;

	dwLen += m_bi.biSizeImage;

	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;

	lpbi = (LPBITMAPINFOHEADER)hDIB;
	GetDIBits(pDC->GetSafeHdc(), 
		HBITMAP(m_bmpPaint),
		0,                    
		(DWORD)m_bi.biHeight,      
		(LPBYTE)lpbi        
		+ (m_bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,   
		(DWORD)DIB_RGB_COLORS);

	BITMAPFILEHEADER      hdr;
	hdr.bfType        = ((WORD) ('M' << 8) | 'B');        
	hdr.bfSize        = GlobalSize (hDIB) + sizeof(hdr);   
	hdr.bfReserved1   = 0;                                 
	hdr.bfReserved2   = 0;                                 
	hdr.bfOffBits=(DWORD)(sizeof(hdr)+lpbi->biSize + nColors * sizeof(RGBQUAD));
	char* pBmpBuf; 
	DWORD FileSize; 
	FileSize=sizeof(hdr)+GlobalSize(hDIB);
	pBmpBuf = new char[FileSize];
	memcpy(pBmpBuf,&hdr,sizeof(hdr));
	memcpy(pBmpBuf+sizeof(hdr),lpbi,GlobalSize(hDIB));

	// QMS Save for DefectMap. Add By KYH01	
	
	CFile file;
	CString strDefectMapDir;
	CString strDefectMapFolderDir;
	CString strPanelID;
	CString strDirectory;
	TCHAR chRead[255] = {0};
	TCHAR chPannelIDinfo[255]={0};
	//STRU_IMAGE_INFO *stImgInfo;	
	memset(chRead,0X00,sizeof(chRead));			
	pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("55000"), (LPTSTR)chRead, 255);	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_PANEL_INFO, KEY_PANNELINFO_PANNEL_ID, _T("ABCD"), (LPTSTR)chPannelIDinfo, 255);
	strPanelID = chPannelIDinfo;
	
	if(bSave == TRUE)
	{
		//G_AddLog_SEQ(2, _T("ImageSaveRoot"));

		CString strFilePath = _T("D:\\CruxServer\\Result\\DefectMap");		
		strDefectMapFolderDir.Format(_T("%s\\%s"), strFilePath, strPanelID);
		
		pFrame->m_LogMng.m_fnMakeDirectory(strDefectMapFolderDir);
		
		strDefectMapDir.Format(_T("%s\\%s\\ALLMAP_000.jpg"), strFilePath, strPanelID);		
		file.Open(strDefectMapDir, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
		file.Write(pBmpBuf,FileSize);
		file.Close();
	}	

	delete[] pBmpBuf;
	if(hDIB)    
		GlobalFree(hDIB);
	SelectPalette(pDC->GetSafeHdc(),hPal,FALSE);  
}

int CDlgDefectMap::TriggerDefectMap(BOOL bSave)
{
	CDC *pDC = GetDC();	
	int ndefcnt = 0;
	EnterCriticalSection(&m_csMapDrawDef);	//2018.12.16 add by mby for Debug window
	//m_fnLoadDefectMap(pTempDC);
	m_fnDrawFrame(pDC);	
	m_fnDrawPanel(pDC);	

	if(bSave == TRUE){
		ndefcnt = m_fnDrawDefect(pDC, TRUE);		// Defect 그리기
	}else{
		ndefcnt = m_fnDrawDefect(pDC, FALSE);		// Defect 그리기
	}
	LeaveCriticalSection(&m_csMapDrawDef);	//2018.12.16 add by mby for Debug window
	//G_AddLog_SEQ(2, _T("TriggerDefectMap()_CPaint dc"));
	return ndefcnt;
}
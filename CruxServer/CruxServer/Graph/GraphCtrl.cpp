// GraphCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CruxServer.h"
#include "GraphCtrl.h"


// CGraphCtrl

IMPLEMENT_DYNAMIC(CGraphCtrl, CStatic)

CGraphCtrl::CGraphCtrl()
{
	m_psReportList = NULL;
}

CGraphCtrl::~CGraphCtrl()
{
	if(m_psReportList)
	{
		delete m_psReportList;
		m_psReportList = NULL;
	}
}


BEGIN_MESSAGE_MAP(CGraphCtrl, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CGraphCtrl 메시지 처리기입니다.




void CGraphCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.

	DrawGraph(&dc);
}

int CGraphCtrl::DrawGraph(CDC *pDC)
{
	HDC hDC = pDC->GetSafeHdc();
	HDC hMemDC;

	RECT rt;
	::GetClientRect(GetSafeHwnd(), &rt);

	HBITMAP hMemBitmap, hOldBitmap;

	hMemDC     = ::CreateCompatibleDC(hDC);
	hMemBitmap = ::CreateCompatibleBitmap(hDC, rt.right - rt.left, rt.bottom - rt.top);
	hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hMemBitmap); 

	HBRUSH hNewBr, hOldBr;
	hNewBr = ::CreateSolidBrush(m_colorBkColor);	//224, 231, 240));
	hOldBr = (HBRUSH)::SelectObject(hMemDC, hNewBr);	
	
	::SetStretchBltMode(hMemDC, COLORONCOLOR);
	::FillRect(hMemDC, &rt, hNewBr);	// 바탕색

	HBRUSH hNewBr2, hOldBr2;
	hNewBr2 = ::CreateSolidBrush(m_colorAxisColor);
	hOldBr2 = (HBRUSH)::SelectObject(hMemDC, hNewBr2);	

	RECT rt2;
	rt2.left   = 70-2;	//50-2;
	rt2.top    = 0;
	rt2.right  = 70;	//50;
	rt2.bottom = rt.bottom - rt.top - (40 - 2);
	::FillRect(hMemDC, &rt2, hNewBr2);	// y축 라인

	rt2.left  = 70;	//50;
	rt2.top   = rt.bottom - rt.top - 40;
	rt2.right = rt.right - rt.left;
	rt2.bottom = rt.bottom - rt.top - 40 + 2;
	::FillRect(hMemDC, &rt2, hNewBr2);	// x축 라인

//	::SetTextColor(hMemDC, RGB(0, 0, 255));
	::SetTextColor(hMemDC, m_colorRatioTextColor);
	::SetBkMode(hMemDC, TRANSPARENT);

	int nActHt = (rt.bottom - rt.top) - 40;
	int nDivH   = (nActHt - 0)/((100 - 0)/5);

	int nlength = 0;
	TCHAR tszRatio[MAX_PATH] = {0,};
	TCHAR tszCh[3] = {0,};
	_stprintf_s(tszRatio, MAX_PATH, _T("%s(%%)"), _T("백분율"));
	nlength = _tcslen(tszRatio);

	for(int i = 0; i < nlength; i++)	// y축 타이틀
	{
		if(tszRatio[i] == _T('\0'))
		{
			break;
		}	
		tszCh[0] = tszRatio[i];
		tszCh[1] = _T(' ');
		::ExtTextOut(hMemDC, 5, (rt.bottom - rt.top)/2 - nlength*17/2 +  17*i, ETO_OPAQUE, NULL, tszCh, 2, NULL);
	}

	// y축 눈금
	int nIndex = 0;
	for(int i = 40; i <= nActHt + 40 + 5; i += ( nDivH/2 )  )
	{
		if(nIndex%2)	// 작은 눈금
		{
			rt2.left   = 70-6;	//50-6;
			rt2.top    = rt.bottom - rt.top - i;
			rt2.right  = 70-2;	//50-2;
			rt2.bottom = rt.bottom - rt.top - i + 1;
			::FillRect(hMemDC, &rt2, hNewBr2);
		}
		else	// 큰 눈금
		{
			rt2.left   = 70-10;	//50-10;
			rt2.top    = rt.bottom - rt.top - i;
			rt2.right  = 70-2;	//50-2;
			rt2.bottom = rt.bottom - rt.top - i + 1;
			::FillRect(hMemDC, &rt2, hNewBr2);
			nlength = _stprintf_s(tszRatio, MAX_PATH, _T("%d"), 0+(nIndex/2)*5);
			tszRatio[nlength] = _T('\0');
			::ExtTextOut(hMemDC, 34, (rt.bottom - rt.top) - i - 3, ETO_OPAQUE, NULL, tszRatio, _tcslen(tszRatio), NULL);
			//::ExtTextOut(hMemDC, 17, (rt.bottom - rt.top) - i - 0, ETO_OPAQUE, NULL, tszRatio, _tcslen(tszRatio), NULL);
		}
		nIndex++;
	}

	TCHAR tszRow[MAX_PATH] = {0,};
	nlength = _stprintf_s(tszRow, MAX_PATH, _T("%s"), _T("항목"));
	tszRow[nlength] = _T('\0');
	// x축 타이틀
	::ExtTextOut(hMemDC, (rt.right - rt.left)/2  - nlength*17/2, (rt.bottom - rt.top) - 40 + 17, ETO_OPAQUE, NULL, tszRow, nlength, NULL);
	
	
	int nActWt = (rt.right - rt.left) - 69;
	int nDivW  = (nActWt - 0)/((55 - 0)/5);
		
	// 막대그래프 출력 부분
	// 막대 배경색
	HBRUSH hNewBrListBk, hOldBrListBk;
	hNewBrListBk = ::CreateSolidBrush(DEFAULT_GRAPH_BAR_BK_COLOR);
	hOldBrListBk = (HBRUSH)::SelectObject(hMemDC, hNewBrListBk);
	
	// Total 색상
	HBRUSH hNewBrTotal, hOldBrTotal;
	hNewBrTotal = ::CreateSolidBrush(m_colorBarTotalColor);
	hOldBrTotal = (HBRUSH)::SelectObject(hMemDC, hNewBrTotal);
		
	rt2.left   = 69;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - nDivH*20;//40+6 - 100;	100%일 경우
	}
	
	rt2.right  = 69+( nDivW/2 );
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - nDivH*20;	// 막대그래프 높이는 nDivH*20*비율만큼 감소시키면 된다.
	}
	
	rt2.right  = 69+( nDivW/2 ) - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;	
	::FillRect(hMemDC, &rt2, hNewBrTotal);
	
	// GOOD 색상
	HBRUSH hNewBrGood, hOldBrGood;
	hNewBrGood = ::CreateSolidBrush(m_colorBarGoodColor);
	hOldBrGood = (HBRUSH)::SelectObject(hMemDC, hNewBrGood);

	rt2.left   = 69+( nDivW/2 );
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nGood/m_psReportList->nTotal);
	}
	
	rt2.right  = 69+( nDivW/2 )*2;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 );
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nGood/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*2 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrGood);

	// Defect 색상
	HBRUSH hNewBrDefect, hOldBrDefect;
	hNewBrDefect = ::CreateSolidBrush(m_colorBarDefectColor);
	hOldBrDefect = (HBRUSH)::SelectObject(hMemDC, hNewBrDefect);

	rt2.left   = 69+( nDivW/2 )*2;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nDefect/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*3;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*2;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nDefect/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*3 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrDefect);

	// Inspect Fail 색상
	HBRUSH hNewBrInspectFail, hOldBrInspectFail;
	hNewBrInspectFail = ::CreateSolidBrush(m_colorBarInspectFailColor);
	hOldBrInspectFail = (HBRUSH)::SelectObject(hMemDC, hNewBrInspectFail);

	rt2.left   = 69+( nDivW/2 )*3;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nInspectFail/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*4;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*3;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nInspectFail/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*4 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrInspectFail);

	// 상부이름 색상
	HBRUSH hNewBrTopName, hOldBrTopName;
	hNewBrTopName = ::CreateSolidBrush(m_colorBarTopNameColor);
	hOldBrTopName = (HBRUSH)::SelectObject(hMemDC, hNewBrTopName);

	rt2.left   = 69+( nDivW/2 )*4;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nTopName/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*5;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*4;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nTopName/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*5 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrTopName);

	// 상부찍힘 색상
	HBRUSH hNewBrTopStabbed, hOldBrTopStabbed;
	hNewBrTopStabbed = ::CreateSolidBrush(m_colorBarTopStabbedColor);
	hOldBrTopStabbed = (HBRUSH)::SelectObject(hMemDC, hNewBrTopStabbed);

	rt2.left   = 69+( nDivW/2 )*5;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nTopStabbed/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*6;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*5;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nTopStabbed/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*6 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrTopStabbed);

	// 하부기포 색상
	HBRUSH hNewBrBottomBubble, hOldBrBottomBubble;
	hNewBrBottomBubble = ::CreateSolidBrush(m_colorBarBottomBubbleColor);
	hOldBrBottomBubble = (HBRUSH)::SelectObject(hMemDC, hNewBrBottomBubble);

	rt2.left   = 69+( nDivW/2 )*6;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nBottomBubble/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*7;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*6;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nBottomBubble/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*7 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrBottomBubble);

	// 하부이상 색상
	HBRUSH hNewBrBottomError, hOldBrBottomError;
	hNewBrBottomError = ::CreateSolidBrush(m_colorBarBottomErrorColor);
	hOldBrBottomError = (HBRUSH)::SelectObject(hMemDC, hNewBrBottomError);

	rt2.left   = 69+( nDivW/2 )*7;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nBottomError/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*8;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*7;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nBottomError/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*8 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrBottomError);

	// 색얼룩 색상
	HBRUSH hNewBrColorSpot, hOldBrColorSpot;
	hNewBrColorSpot = ::CreateSolidBrush(m_colorBarColorSpotColor);
	hOldBrColorSpot = (HBRUSH)::SelectObject(hMemDC, hNewBrColorSpot);

	rt2.left   = 69+( nDivW/2 )*8;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nColorSpot/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*9;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*8;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nColorSpot/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*9 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrColorSpot);

	// 원형얼룩 색상
	HBRUSH hNewBrCircleSpot, hOldBrCircleSpot;
	hNewBrCircleSpot = ::CreateSolidBrush(m_colorBarCircleSpotColor);
	hOldBrCircleSpot = (HBRUSH)::SelectObject(hMemDC, hNewBrCircleSpot);

	rt2.left   = 69+( nDivW/2 )*9;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nCircleSpot/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*10;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*9;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nCircleSpot/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*10 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrCircleSpot);

	// MONOMER 뭉침 색상
	HBRUSH hNewBrMonomerUnite, hOldBrMonomerUnite;
	hNewBrMonomerUnite = ::CreateSolidBrush(m_colorBarMonomerUniteColor);
	hOldBrMonomerUnite = (HBRUSH)::SelectObject(hMemDC, hNewBrMonomerUnite);

	rt2.left   = 69+( nDivW/2 )*10;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nMonomerUnite/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*11;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*10;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nMonomerUnite/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*11 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrMonomerUnite);

	// FILM 꺽임 색상
	HBRUSH hNewBrFilmRollLine, hOldBrFilmRollLine;
	hNewBrFilmRollLine = ::CreateSolidBrush(m_colorBarFilmRollLineColor);
	hOldBrFilmRollLine = (HBRUSH)::SelectObject(hMemDC, hNewBrFilmRollLine);

	rt2.left   = 69+( nDivW/2 )*11;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nFilmRollLine/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*12;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*11;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nFilmRollLine/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*12 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrFilmRollLine);

	// Wave 색상
	HBRUSH hNewBrWave, hOldBrWave;
	hNewBrWave = ::CreateSolidBrush(m_colorBarWaveColor);
	hOldBrWave = (HBRUSH)::SelectObject(hMemDC, hNewBrWave);

	rt2.left   = 69+( nDivW/2 )*12;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nWave/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*13;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*12;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nWave/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*13 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrWave);

	// 탈막 색상
	HBRUSH hNewBrWashing, hOldBrWashing;
	hNewBrWashing = ::CreateSolidBrush(m_colorBarWashingColor);
	hOldBrWashing = (HBRUSH)::SelectObject(hMemDC, hNewBrWashing);

	rt2.left   = 69+( nDivW/2 )*13;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nWashing/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*14;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*13;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nWashing/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*14 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrWashing);

	// Pad Burnt 색상
	HBRUSH hNewBrPadBurnt, hOldBrPadBurnt;
	hNewBrPadBurnt = ::CreateSolidBrush(m_colorBarPadBurntColor);
	hOldBrPadBurnt = (HBRUSH)::SelectObject(hMemDC, hNewBrPadBurnt);

	rt2.left   = 69+( nDivW/2 )*14;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nPadBurnt/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*15;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*14;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nPadBurnt/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*15 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrPadBurnt);

	// Pad Scretch 색상
	HBRUSH hNewBrPadScratch, hOldBrPadScratch;
	hNewBrPadScratch = ::CreateSolidBrush(m_colorBarPadScratchColor);
	hOldBrPadScratch = (HBRUSH)::SelectObject(hMemDC, hNewBrPadScratch);

	rt2.left   = 69+( nDivW/2 )*15;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nPadScratch/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*16;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*15;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nPadScratch/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*16 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrPadScratch);

	// Pad 이상 색상
	HBRUSH hNewBrPadError, hOldBrPadError;
	hNewBrPadError = ::CreateSolidBrush(m_colorBarPadErrorColor);
	hOldBrPadError = (HBRUSH)::SelectObject(hMemDC, hNewBrPadError);

	rt2.left   = 69+( nDivW/2 )*16;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nPadError/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*17;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*16;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nPadError/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*17 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrPadError);

	// Pad 찍힘 색상
	HBRUSH hNewBrPadStabbed, hOldBrPadStabbed;
	hNewBrPadStabbed = ::CreateSolidBrush(m_colorBarPadStabbedColor);
	hOldBrPadStabbed = (HBRUSH)::SelectObject(hMemDC, hNewBrPadStabbed);

	rt2.left   = 69+( nDivW/2 )*17;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nPadStabbed/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*18;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*17;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nPadStabbed/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*18 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrPadStabbed);

	// 컷불량 색상
	HBRUSH hNewBrCutError, hOldBrCutError;
	hNewBrCutError = ::CreateSolidBrush(m_colorBarCutErrorColor);
	hOldBrCutError = (HBRUSH)::SelectObject(hMemDC, hNewBrCutError);

	rt2.left   = 69+( nDivW/2 )*18;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nCutError/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*19;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*18;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nCutError/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*19 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrCutError);

	// PAD 뜯김 색상
	HBRUSH hNewBrPadPulledOut, hOldBrPadPulledOut;
	hNewBrPadPulledOut = ::CreateSolidBrush(m_colorBarPadPulledOutColor);
	hOldBrPadPulledOut = (HBRUSH)::SelectObject(hMemDC, hNewBrPadPulledOut);

	rt2.left   = 69+( nDivW/2 )*19;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nPadPulledOut/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*20;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*19;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nPadPulledOut/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*20 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrPadPulledOut);

	// C-CUT 색상
	HBRUSH hNewBrC_Cut, hOldBrC_Cut;
	hNewBrC_Cut = ::CreateSolidBrush(m_colorBarC_CutColor);
	hOldBrC_Cut = (HBRUSH)::SelectObject(hMemDC, hNewBrC_Cut);

	rt2.left   = 69+( nDivW/2 )*20;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 40;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 40 - (nDivH*20*m_psReportList->nC_Cut/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*21;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrListBk);

	rt2.left   = 70+( nDivW/2 )*20;
	if(m_psReportList->nTotal == 0)
	{
		rt2.top    = (rt.bottom - rt.top) - 39;
	}
	else
	{
		rt2.top    = (rt.bottom - rt.top) - 39 - (nDivH*20*m_psReportList->nC_Cut/m_psReportList->nTotal);
	}
	rt2.right  = 69+( nDivW/2 )*21 - 1;
	rt2.bottom = (rt.bottom - rt.top) - 39;
	::FillRect(hMemDC, &rt2, hNewBrC_Cut);

	CStringArray strAryColumn;
	//strAryColumn.Add(_T("TOTAL"));
	//strAryColumn.Add(_T("GOOD"));
	//strAryColumn.Add(_T("DEFECT"));
	//strAryColumn.Add(_T("INSPECT FAIL"));
	//strAryColumn.Add(_T("상부이름"));
	//strAryColumn.Add(_T("상부찍힘"));
	//strAryColumn.Add(_T("하부기포"));
	//strAryColumn.Add(_T("하부이상"));
	//strAryColumn.Add(_T("색얼룩"));
	//strAryColumn.Add(_T("원형얼룩"));
	//strAryColumn.Add(_T("MONOMER 뭉침"));
	//strAryColumn.Add(_T("FILM 꺽임"));
	//strAryColumn.Add(_T("WAVE"));
	//strAryColumn.Add(_T("탈막"));
	//strAryColumn.Add(_T("PAD BURNT"));
	//strAryColumn.Add(_T("PAD SCRATCH"));
	//strAryColumn.Add(_T("PAD 이상"));
	//strAryColumn.Add(_T("PAD 찍힘"));
	//strAryColumn.Add(_T("컷불량"));
	//strAryColumn.Add(_T("PAD 뜯김"));
	//strAryColumn.Add(_T("C-CUT"));

	strAryColumn.Add(_T("DUST"));
	strAryColumn.Add(_T("WHITE TURBITY"));
	strAryColumn.Add(_T("ITO"));
	strAryColumn.Add(_T("EDGE BROKEN"));
	strAryColumn.Add(_T("PAD SCRATCH"));
	strAryColumn.Add(_T("DIMPLE POINT"));
	strAryColumn.Add(_T("ETCHING LINE"));
	strAryColumn.Add(_T("DIMPLE LINE"));
	strAryColumn.Add(_T("PAD BURNT"));
	strAryColumn.Add(_T("BURR"));
	strAryColumn.Add(_T("POL BUBBLE"));
	strAryColumn.Add(_T("POL SCRATCH"));
	strAryColumn.Add(_T("POL ALIEN SUB"));
	strAryColumn.Add(_T("POL MINOR"));
	strAryColumn.Add(_T("BLACK SCRATCH"));
	strAryColumn.Add(_T("WHITE SCRATCH"));
	strAryColumn.Add(_T("CF MEASURE"));
	strAryColumn.Add(_T("TFT MEASURE"));
	strAryColumn.Add(_T("MURA"));

	
	::SetTextColor(hMemDC, m_colorBarTextColor);
	::SetBkMode(hMemDC, TRANSPARENT);
		
	
	// x축 눈금
	for(int i = 69; i <= nActWt + 69 + 5; i += ( nDivW/2 )  )
	{
		rt2.left   = i;
		rt2.top    = (rt.bottom - rt.top) - 40;
		rt2.right  = i+1;
		rt2.bottom = (rt.bottom - rt.top) - (40+6);
		::FillRect(hMemDC, &rt2, hNewBr2);

		if(strAryColumn.GetCount() == 0)
		{
			break;
		}
		nlength = _stprintf_s(tszRow, MAX_PATH, _T("%s"), strAryColumn.GetAt(0));
		tszRow[nlength] = _T('\0');
		strAryColumn.RemoveAt(0);
		nlength = _tcslen(tszRow);
		for(int j = 0; j < nlength; j++)
		{
			if(tszRow[j] == _T('\0'))
			{
				break;
			}	
			tszCh[0] = tszRow[j];
			tszCh[1] = _T(' ');
			::ExtTextOut(hMemDC, i + 17, (rt.bottom - rt.top) - 40 - nlength*17 +  17*j - 5, ETO_OPAQUE, NULL, tszCh, 2, NULL);
		}
	}	

	::StretchBlt(hDC, 0, 0, rt.right - rt.left, rt.bottom - rt.top, hMemDC, 0, 0, rt.right - rt.left, rt.bottom - rt.top, SRCCOPY);

	::SelectObject(hMemDC, hOldBrC_Cut);
	::DeleteObject(hNewBrC_Cut);

	::SelectObject(hMemDC, hOldBrPadPulledOut);
	::DeleteObject(hNewBrPadPulledOut);

	::SelectObject(hMemDC, hOldBrCutError);
	::DeleteObject(hNewBrCutError);

	::SelectObject(hMemDC, hOldBrPadStabbed);
	::DeleteObject(hNewBrPadStabbed);

	::SelectObject(hMemDC, hOldBrPadError);
	::DeleteObject(hNewBrPadError);

	::SelectObject(hMemDC, hOldBrPadScratch);
	::DeleteObject(hNewBrPadScratch);

	::SelectObject(hMemDC, hOldBrPadBurnt);
	::DeleteObject(hNewBrPadBurnt);

	::SelectObject(hMemDC, hOldBrWashing);
	::DeleteObject(hNewBrWashing);

	::SelectObject(hMemDC, hOldBrWave);
	::DeleteObject(hNewBrWave);

	::SelectObject(hMemDC, hOldBrFilmRollLine);
	::DeleteObject(hNewBrFilmRollLine);

	::SelectObject(hMemDC, hOldBrMonomerUnite);
	::DeleteObject(hNewBrMonomerUnite);
	
	::SelectObject(hMemDC, hOldBrCircleSpot);
	::DeleteObject(hNewBrCircleSpot);

	::SelectObject(hMemDC, hOldBrColorSpot);
	::DeleteObject(hNewBrColorSpot);

	::SelectObject(hMemDC, hOldBrBottomError);
	::DeleteObject(hNewBrBottomError);

	::SelectObject(hMemDC, hOldBrBottomBubble);
	::DeleteObject(hNewBrBottomBubble);

	::SelectObject(hMemDC, hOldBrTopStabbed);
	::DeleteObject(hNewBrTopStabbed);

	::SelectObject(hMemDC, hOldBrTopName);
	::DeleteObject(hNewBrTopName);

	::SelectObject(hMemDC, hOldBrInspectFail);
	::DeleteObject(hNewBrInspectFail);

	::SelectObject(hMemDC, hOldBrDefect);
	::DeleteObject(hNewBrDefect);

	::SelectObject(hMemDC, hOldBrGood);
	::DeleteObject(hNewBrGood);

	::SelectObject(hMemDC, hOldBrTotal);
	::DeleteObject(hNewBrTotal);

	::SelectObject(hMemDC, hOldBrListBk);
	::DeleteObject(hNewBrListBk);
	
	::SelectObject(hMemDC, hOldBr2);
	::DeleteObject(hNewBr2);

	::SelectObject(hMemDC, hOldBr);
	::DeleteObject(hNewBr);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hMemBitmap);

	::DeleteDC(hMemDC);
	

	return 0;
}

void CGraphCtrl::SetValue(stReportList *psReportList)
{
	if(m_psReportList)
	{
		delete m_psReportList;
		m_psReportList = NULL;
	}

	m_psReportList = new stReportList;
	memset(m_psReportList, 0, sizeof(stReportList));
	m_psReportList->nTotal		  = psReportList->nTotal;
	m_psReportList->nGood		  = psReportList->nGood;
	m_psReportList->nDefect		  = psReportList->nDefect;
	m_psReportList->nInspectFail  = psReportList->nInspectFail;
	m_psReportList->nTopName	  = psReportList->nTopName;
	m_psReportList->nTopStabbed	  = psReportList->nTopStabbed;
	m_psReportList->nBottomBubble = psReportList->nBottomBubble;
	m_psReportList->nBottomError  = psReportList->nBottomError;
	m_psReportList->nColorSpot	  = psReportList->nColorSpot;
	m_psReportList->nCircleSpot	  = psReportList->nCircleSpot;
	m_psReportList->nMonomerUnite = psReportList->nMonomerUnite;
	m_psReportList->nFilmRollLine = psReportList->nFilmRollLine;
	m_psReportList->nWave		  = psReportList->nWave;
	m_psReportList->nWashing	  = psReportList->nWashing;
	m_psReportList->nPadBurnt	  = psReportList->nPadBurnt;
	m_psReportList->nPadScratch   = psReportList->nPadScratch;
	m_psReportList->nPadError     = psReportList->nPadError;
	m_psReportList->nPadStabbed   = psReportList->nPadStabbed;
	m_psReportList->nCutError     = psReportList->nCutError;
	m_psReportList->nPadPulledOut = psReportList->nPadPulledOut;
	m_psReportList->nC_Cut        = psReportList->nC_Cut;
}

void CGraphCtrl::SetBkColor(COLORREF colorBkColor)
{
	m_colorBkColor              = colorBkColor;
}									

void CGraphCtrl::SetAxisColor(COLORREF colorAxisColor)
{
	m_colorAxisColor            = colorAxisColor;
}	

void CGraphCtrl::SetRatioTextColor(COLORREF colorRatioTextColor)
{
	m_colorRatioTextColor       = colorRatioTextColor;
}						

void CGraphCtrl::SetBarTextColor(COLORREF colorBarTextColor)
{
	m_colorBarTextColor         = colorBarTextColor;
}							

void CGraphCtrl::SetBarTotalColor(COLORREF colorBarTotalColor)
{
	m_colorBarTotalColor        = colorBarTotalColor;
}							

void CGraphCtrl::SetBarGoodColor(COLORREF colorBarGoodColor)
{
	m_colorBarGoodColor         = colorBarGoodColor;
}								

void CGraphCtrl::SetBarDefectColor(COLORREF colorBarDefectColor)
{
	m_colorBarDefectColor       = colorBarDefectColor;
}							

void CGraphCtrl::SetBarInspectFailColor(COLORREF colorBarInspectFailColor)
{
	m_colorBarInspectFailColor  = colorBarInspectFailColor;
}				

void CGraphCtrl::SetBarTopNameColor(COLORREF colorBarTopNameColor)
{
	m_colorBarTopNameColor      = colorBarTopNameColor;
}							

void CGraphCtrl::SetBarTopStabbedColor(COLORREF colorBarTopStabbedColor)
{
	m_colorBarTopStabbedColor   = colorBarTopStabbedColor;
}				

void CGraphCtrl::SetBarBottomBubbleColor(COLORREF colorBarBottomBubbleColor)
{
	m_colorBarBottomBubbleColor = colorBarBottomBubbleColor;
}			

void CGraphCtrl::SetBarBottomErrorColor(COLORREF colorBarBottomErrorColor)
{
	m_colorBarBottomErrorColor  = colorBarBottomErrorColor;
}					

void CGraphCtrl::SetBarColorSpotColor(COLORREF colorBarColorSpotColor)
{
	m_colorBarColorSpotColor    = colorBarColorSpotColor;
}					

void CGraphCtrl::SetBarCircleSpotColor(COLORREF colorBarCircleSpotColor)
{
	m_colorBarCircleSpotColor   = colorBarCircleSpotColor;
}				

void CGraphCtrl::SetBarMonomerUniteColor(COLORREF colorBarMonomerUniteColor)
{
	m_colorBarMonomerUniteColor = colorBarMonomerUniteColor;
}			

void CGraphCtrl::SetBarFilmRollLineColor(COLORREF colorBarFilmRollLineColor)
{
	m_colorBarFilmRollLineColor = colorBarFilmRollLineColor;
}		

void CGraphCtrl::SetBarWaveColor(COLORREF colorBarWaveColor)
{
	m_colorBarWaveColor         = colorBarWaveColor;
}			

void CGraphCtrl::SetBarWashingColor(COLORREF colorBarWashingColor)
{					
	m_colorBarWashingColor      = colorBarWashingColor;	
}	

void CGraphCtrl::SetBarPadBurntColor(COLORREF colorBarPadBurntColor)
{				
	m_colorBarPadBurntColor     = colorBarPadBurntColor;
}	

void CGraphCtrl::SetBarPadScratchColor(COLORREF colorBarPadScratchColor)
{			
	m_colorBarPadScratchColor   = colorBarPadScratchColor;	
}	

void CGraphCtrl::SetBarPadErrorColor(COLORREF colorBarPadErrorColor)
{				
	m_colorBarPadErrorColor     = colorBarPadErrorColor;	
}	

void CGraphCtrl::SetBarPadStabbedColor(COLORREF colorBarPadStabbedColor)
{				
	m_colorBarPadStabbedColor   = colorBarPadStabbedColor;
}		

void CGraphCtrl::SetBarCutErrorColor(COLORREF colorBarCutErrorColor)
{					
	m_colorBarCutErrorColor     = colorBarCutErrorColor;	
}	

void CGraphCtrl::SetBarPadPulledOutColor(COLORREF colorBarPadPulledOutColor)
{			
	m_colorBarPadPulledOutColor = colorBarPadPulledOutColor;
}

void CGraphCtrl::SetBarC_CutColor(COLORREF colorBarC_CutColor)
{							
	m_colorBarC_CutColor        = colorBarC_CutColor;	
}

COLORREF CGraphCtrl::GetBkColor()
{	
	return m_colorBkColor;
}

COLORREF CGraphCtrl::GetAxisColor()
{			
	return m_colorAxisColor;
}

COLORREF CGraphCtrl::GetRatioTextColor()
{		
	return m_colorRatioTextColor;
}

COLORREF CGraphCtrl::GetBarTextColor()
{		
	return m_colorBarTextColor;
}

COLORREF CGraphCtrl::GetBarTotalColor()
{	
	return m_colorBarTotalColor;	
}

COLORREF CGraphCtrl::GetBarGoodColor()
{		
	return m_colorBarGoodColor;	
}	

COLORREF CGraphCtrl::GetBarDefectColor()
{		
	return m_colorBarDefectColor;
}	

COLORREF CGraphCtrl::GetBarInspectFailColor()
{	
	return m_colorBarInspectFailColor;
}	

COLORREF CGraphCtrl::GetBarTopNameColor()
{	
	return m_colorBarTopNameColor;	
}

COLORREF CGraphCtrl::GetBarTopStabbedColor()
{	
	return m_colorBarTopStabbedColor;	
}	

COLORREF CGraphCtrl::GetBarBottomBubbleColor()
{	
	return m_colorBarBottomBubbleColor;	
}

COLORREF CGraphCtrl::GetBarBottomErrorColor()
{	
	return m_colorBarBottomErrorColor;
}

COLORREF CGraphCtrl::GetBarColorSpotColor()
{	
	return m_colorBarColorSpotColor;
}	

COLORREF CGraphCtrl::GetBarCircleSpotColor()
{	
	return m_colorBarCircleSpotColor;
}	

COLORREF CGraphCtrl::GetBarMonomerUniteColor()
{	
	return m_colorBarMonomerUniteColor;	
}

COLORREF CGraphCtrl::GetBarFilmRollLineColor()
{	
	return m_colorBarFilmRollLineColor;
}

COLORREF CGraphCtrl::GetBarWaveColor()
{		
	return m_colorBarWaveColor;		
}

COLORREF CGraphCtrl::GetBarWashingColor()
{		
	return m_colorBarWashingColor;	
}

COLORREF CGraphCtrl::GetBarPadBurntColor()
{		
	return m_colorBarPadBurntColor;	
}

COLORREF CGraphCtrl::GetBarPadScratchColor()
{	
	return m_colorBarPadScratchColor;
}	

COLORREF CGraphCtrl::GetBarPadErrorColor()
{	
	return m_colorBarPadErrorColor;	
}

COLORREF CGraphCtrl::GetBarPadStabbedColor()
{
	return m_colorBarPadStabbedColor;
}	

COLORREF CGraphCtrl::GetBarCutErrorColor()
{		
	return m_colorBarCutErrorColor;	
}

COLORREF CGraphCtrl::GetBarPadPulledOutColor()
{	
	return m_colorBarPadPulledOutColor;
}

COLORREF CGraphCtrl::GetBarC_CutColor()
{		
	return m_colorBarC_CutColor;
}

void CGraphCtrl::Refresh()
{
	Invalidate(FALSE);
}
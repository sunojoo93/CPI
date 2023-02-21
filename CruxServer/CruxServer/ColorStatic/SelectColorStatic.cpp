// SelectColorStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "SampleReport.h"
#include "SelectColorStatic.h"


// CSelectColorStatic

IMPLEMENT_DYNAMIC(CSelectColorStatic, CStatic)

CSelectColorStatic::CSelectColorStatic()
{

}

CSelectColorStatic::~CSelectColorStatic()
{
}


BEGIN_MESSAGE_MAP(CSelectColorStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CSelectColorStatic 메시지 처리기입니다.
void CSelectColorStatic::SetBkColor(COLORREF colorBk)
{
	m_colorBk = colorBk;
}

void CSelectColorStatic::Refresh()
{
	Invalidate(FALSE);
}


void CSelectColorStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.
	DrawColor(&dc);
}

void CSelectColorStatic::DrawColor(CDC *pDC)
{
	RECT rt;
	::GetClientRect(GetSafeHwnd(), &rt);

	HDC hDC = pDC->GetSafeHdc();
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	
	HBITMAP hNewBitmap, hOldBitmap;
//	hNewBitmap = ::CreateCompatibleBitmap(hMemDC, rt.right - rt.left, rt.bottom - rt.top);
	hNewBitmap = ::CreateBitmap(rt.right - rt.left, rt.bottom - rt.top, 1, 32, NULL);
	hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hNewBitmap);

	HBRUSH hNewBr, hOldBr;
	hNewBr = ::CreateSolidBrush(m_colorBk);
	hOldBr = (HBRUSH)::SelectObject(hMemDC, hNewBr);
	
	::FillRect(hMemDC, &rt, hNewBr);

	::SetStretchBltMode(hMemDC, COLORONCOLOR);
	::StretchBlt(hDC, 0, 0, rt.right - rt.left, rt.bottom - rt.top, hMemDC, 0, 0, rt.right - rt.left, rt.bottom - rt.top, SRCCOPY);

	::SelectObject(hMemDC, hOldBr);
	::DeleteObject(hNewBr);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hNewBitmap);

	::DeleteObject(hMemDC);
}
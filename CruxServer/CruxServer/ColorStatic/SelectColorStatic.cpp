// SelectColorStatic.cpp : ���� �����Դϴ�.
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



// CSelectColorStatic �޽��� ó�����Դϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CStatic::OnPaint()��(��) ȣ������ ���ʽÿ�.
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
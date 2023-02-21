#pragma once


// CSelectColorStatic

class CSelectColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CSelectColorStatic)
private:
	COLORREF m_colorBk;
public:
	CSelectColorStatic();
	virtual ~CSelectColorStatic();

	void SetBkColor(COLORREF colorBk);
	void Refresh();
	void DrawColor(CDC *pDC);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};



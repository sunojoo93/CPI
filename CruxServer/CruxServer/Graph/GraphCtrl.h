#pragma once


// CGraphCtrl

class CGraphCtrl : public CStatic
{
	DECLARE_DYNAMIC(CGraphCtrl)
private:
	stReportList *m_psReportList;

	COLORREF m_colorBkColor;
	COLORREF m_colorAxisColor;
	COLORREF m_colorRatioTextColor;
	COLORREF m_colorBarTextColor;
	COLORREF m_colorBarTotalColor;
	COLORREF m_colorBarGoodColor;				
	COLORREF m_colorBarDefectColor;			
	COLORREF m_colorBarInspectFailColor;		
	COLORREF m_colorBarTopNameColor;			
	COLORREF m_colorBarTopStabbedColor;		
	COLORREF m_colorBarBottomBubbleColor;	
	COLORREF m_colorBarBottomErrorColor;		
	COLORREF m_colorBarColorSpotColor;		
	COLORREF m_colorBarCircleSpotColor;		
	COLORREF m_colorBarMonomerUniteColor;	
	COLORREF m_colorBarFilmRollLineColor;	
	COLORREF m_colorBarWaveColor;				
	COLORREF m_colorBarWashingColor;			
	COLORREF m_colorBarPadBurntColor;		
	COLORREF m_colorBarPadScratchColor;		
	COLORREF m_colorBarPadErrorColor;		
	COLORREF m_colorBarPadStabbedColor;		
	COLORREF m_colorBarCutErrorColor;		
	COLORREF m_colorBarPadPulledOutColor;	
	COLORREF m_colorBarC_CutColor;	

public:
	CGraphCtrl();
	virtual ~CGraphCtrl();

	int DrawGraph(CDC *pDC);
	void SetValue(stReportList *psReportList);

	void SetBkColor(COLORREF colorBkColor);
	void SetAxisColor(COLORREF colorAxisColor);
	void SetRatioTextColor(COLORREF colorRatioTextColor);
	void SetBarTextColor(COLORREF colorBarTextColor);
	void SetBarTotalColor(COLORREF colorBarTotalColor);
	void SetBarGoodColor(COLORREF colorBarGoodColor);			
	void SetBarDefectColor(COLORREF colorBarDefectColor);			
	void SetBarInspectFailColor(COLORREF colorBarInspectFailColor);	
	void SetBarTopNameColor(COLORREF colorBarTopNameColor);		
	void SetBarTopStabbedColor(COLORREF colorBarTopStabbedColor);		
	void SetBarBottomBubbleColor(COLORREF colorBarBottomBubbleColor);	
	void SetBarBottomErrorColor(COLORREF colorBarBottomErrorColor);	
	void SetBarColorSpotColor(COLORREF colorBarColorSpotColor);		
	void SetBarCircleSpotColor(COLORREF colorBarCircleSpotColor);		
	void SetBarMonomerUniteColor(COLORREF colorBarMonomerUniteColor);	
	void SetBarFilmRollLineColor(COLORREF colorBarFilmRollLineColor);	
	void SetBarWaveColor(COLORREF colorBarWaveColor);			
	void SetBarWashingColor(COLORREF colorBarWashingColor);		
	void SetBarPadBurntColor(COLORREF colorBarPadBurntColor);		
	void SetBarPadScratchColor(COLORREF colorBarPadScratchColor);		
	void SetBarPadErrorColor(COLORREF colorBarPadErrorColor);		
	void SetBarPadStabbedColor(COLORREF colorBarPadStabbedColor);		
	void SetBarCutErrorColor(COLORREF colorBarCutErrorColor);		
	void SetBarPadPulledOutColor(COLORREF colorBarPadPulledOutColor);	
	void SetBarC_CutColor(COLORREF colorBarC_CutColor);			
	
	COLORREF GetBkColor();					
	COLORREF GetAxisColor();					
	COLORREF GetRatioTextColor();				
	COLORREF GetBarTextColor();				
	COLORREF GetBarTotalColor();				
	COLORREF GetBarGoodColor();				
	COLORREF GetBarDefectColor();				
	COLORREF GetBarInspectFailColor();		
	COLORREF GetBarTopNameColor();			
	COLORREF GetBarTopStabbedColor();				
	COLORREF GetBarBottomBubbleColor();
	COLORREF GetBarBottomErrorColor();		
	COLORREF GetBarColorSpotColor();			
	COLORREF GetBarCircleSpotColor();			
	COLORREF GetBarMonomerUniteColor();		
	COLORREF GetBarFilmRollLineColor();		
	COLORREF GetBarWaveColor();				
	COLORREF GetBarWashingColor();			
	COLORREF GetBarPadBurntColor();			
	COLORREF GetBarPadScratchColor();			
	COLORREF GetBarPadErrorColor();			
	COLORREF GetBarPadStabbedColor();			
	COLORREF GetBarCutErrorColor();			
	COLORREF GetBarPadPulledOutColor();		
	COLORREF GetBarC_CutColor();				

	void Refresh();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};



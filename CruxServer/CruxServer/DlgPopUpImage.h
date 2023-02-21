#pragma once

#define HIMETRIC_INCH 2540
// CDlgPopUpImage 대화 상자입니다.

class CDlgPopUpImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPopUpImage)

private:
	LPPICTURE mp_image_object;
	long m_width_size, m_height_size;
	afx_msg void OnClose();

public:
	CDlgPopUpImage(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPopUpImage();
	LRESULT m_fnPopupImage(WPARAM wParam, LPARAM lParam);
	BOOL LoadImage(CString strImgPath);
	void ShowImage();
	void OnDestroy();
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_POP_DEF_IMG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

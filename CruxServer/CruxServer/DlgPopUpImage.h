#pragma once

#define HIMETRIC_INCH 2540
// CDlgPopUpImage ��ȭ �����Դϴ�.

class CDlgPopUpImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPopUpImage)

private:
	LPPICTURE mp_image_object;
	long m_width_size, m_height_size;
	afx_msg void OnClose();

public:
	CDlgPopUpImage(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgPopUpImage();
	LRESULT m_fnPopupImage(WPARAM wParam, LPARAM lParam);
	BOOL LoadImage(CString strImgPath);
	void ShowImage();
	void OnDestroy();
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_POP_DEF_IMG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

// DlgPopUpImage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgPopUpImage.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CDlgPopUpImage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgPopUpImage, CDialogEx)

BEGIN_MESSAGE_MAP(CDlgPopUpImage, CDialogEx)
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_UI_CONTROL_POPUP_IMAGE, &CDlgPopUpImage::m_fnPopupImage)
END_MESSAGE_MAP()

CDlgPopUpImage::CDlgPopUpImage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPopUpImage::IDD, pParent)
{
	mp_image_object = NULL;
	m_width_size = m_height_size = 0;
		
	//SetTimer(1000, 100, NULL);
}

CDlgPopUpImage::~CDlgPopUpImage()
{
	//ShowWindow(SW_HIDE);
	OnDestroy();
}

void CDlgPopUpImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
void CDlgPopUpImage::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1000)	// Recipe Edit
	{
		if (mp_image_object != NULL)
		ShowImage();
	}
	
	CDialogEx::OnTimer(nIDEvent);
}
void CDlgPopUpImage::OnDestroy()
{
	if (mp_image_object != NULL)
	{
		mp_image_object->Release();
		mp_image_object = NULL;
	}
	
}
void CDlgPopUpImage::ShowImage()
{
	CClientDC dc(this);

	if (m_width_size && m_height_size )
	{
		int x_size = MulDiv(m_width_size, dc.GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
		int y_size = MulDiv(m_height_size, dc.GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);
		mp_image_object->Render(dc.m_hDC, 1, 1, x_size, y_size, 0, 
			m_height_size, m_width_size, -m_height_size, NULL);
	}
	
	
	//Invalidate(FALSE);
}
BOOL CDlgPopUpImage::LoadImage(CString strImgPath)
{
	
	try
	{
		if (mp_image_object != NULL) mp_image_object->Release();
		BOOL ok_flag = 0;

		HANDLE h_file = CreateFileW(strImgPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (INVALID_HANDLE_VALUE != h_file)
		{
			int file_size = GetFileSize(h_file, NULL);
			if (-1 != file_size)
			{
				HGLOBAL h_global = GlobalAlloc(GMEM_MOVEABLE, file_size);
				if (NULL != h_global)
				{

					void *p_data = GlobalLock(h_global);

					if (NULL != p_data)
					{
						unsigned long read_size = 0 ;

						ReadFile(h_file, p_data, file_size, &read_size, NULL);
						GlobalUnlock(h_global);
						LPSTREAM p_stream = NULL;
						HRESULT h_result = CreateStreamOnHGlobal(h_global, TRUE, &p_stream);
						if (SUCCEEDED(h_result) && p_stream)
						{

							::OleLoadPicture(p_stream, file_size, FALSE, IID_IPicture, (LPVOID*)&mp_image_object);
							if (SUCCEEDED(h_result) && mp_image_object)
							{
								mp_image_object->get_Width(&m_width_size);
								mp_image_object->get_Height(&m_height_size);
							}
							ok_flag = 1;
						}					
					}

				}
				GlobalFree(h_global);
			}
			CloseHandle(h_file);
		}
		return ok_flag;
	}
	catch (CMemoryException* e)
	{
		e->Delete();
	}
	catch (CFileException* e)
	{
		e->Delete();
	}
	catch (CException* e)
	{
		e->Delete();
	}

	return TRUE;
	
}


LRESULT CDlgPopUpImage::m_fnPopupImage(WPARAM wParam, LPARAM lParam)
{
	CString strPath = (LPCTSTR)lParam;

	MoveWindow(1260, 500, 648, 510);
	//Sleep(100);
	SetWindowText(strPath.TrimRight());
	if (LoadImage(strPath.TrimRight()))
	{
		ShowImage();
		ShowWindow(SW_SHOW);
	}
	//else
	//	OnDestroy();

	return APP_OK;
}

// CDlgPopUpImage 메시지 처리기입니다.

void CDlgPopUpImage::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	//CDialogEx::OnWindowPosChanged(lpwndpos);
	if (mp_image_object != NULL)
		ShowImage();
	//ShowWindow(SW_SHOW);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgPopUpImage::OnClose()
{
	
	ShowWindow(SW_HIDE);
	OnDestroy();	
	
}



// 판정 항목 등록 관련 UI.
// 기능				: Set Up UI 내 판정항목들의 우선순위, 확인유무 등 을 설정하는 UI
// 항목 추가 방법	: CPP 파일 내	
//					  m_fnIniListCheck() 함수		- 우선순위 등록
//					  m_fnJudgeListDataTerminal		- 항목 Load 및 데이터 Frame 저장
//					  내 추가할 항목에 대한 것을 작성한다.


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgSetJudgeItem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgSetJudgeItem.h"
#include "afxdialogex.h"
#include "MainFrm.h"

//2016.12.12
enum{
	SET_JUDGE_ITEM_DEF_NAME = 0,
	SET_JUDGE_ITEM_DEF_PRIORITY,
	SET_JUDGE_ITEM_DEF_CODE,
	SET_JUDGE_ITEM__LO_EQP_BYPASS,
	SET_JUDGE_ITEM_MAX
};	//List Box 내 항목

enum{
	SET_JUDGE_DATA_LOAD = 0,
	SET_JUDGE_DATA_SAVE,
	SET_JUDGE_DATA_CODE
};	//List Box 내 항목
//

// CDlgSetJudgeItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSetJudgeItem, CDialogEx)

CDlgSetJudgeItem::CDlgSetJudgeItem(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSetJudgeItem::IDD, pParent)
{

}

CDlgSetJudgeItem::~CDlgSetJudgeItem()
{
}

void CDlgSetJudgeItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SET, m_ctrlListSet);
}


BEGIN_MESSAGE_MAP(CDlgSetJudgeItem, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgSetJudgeItem::OnBnClickedButtonSave)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SET, &CDlgSetJudgeItem::OnClickListSet)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CDlgSetJudgeItem::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CDlgSetJudgeItem::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_ALL_CHECK, &CDlgSetJudgeItem::OnBnClickedButtonAllCheck)
	ON_BN_CLICKED(IDC_BUTTON_NOT_CHECK, &CDlgSetJudgeItem::OnBnClickedButtonNotCheck)
END_MESSAGE_MAP()


// CDlgSetJudgeItem 메시지 처리기입니다.

void CDlgSetJudgeItem::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	// 해당 초기화 함수 중복실행 방지
	// Ini 정보를 받기 위해 OninitDialog 시점이 아닌 Client 화면 초기 갱신 시 한번만 실행 하여야 하여 이곳에서 초기화
	static BOOL bActive = FALSE;

	if( bActive == FALSE )
	{
		m_fnInitDialog();
		bActive = TRUE;
	}
	//
	
}

void CDlgSetJudgeItem::m_fnInitDialog(void)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);	//2016.10.10

	//
	m_nListCurIndex = NULL;	//2016.12.12
	//

	//0:AVL 1:ABI
	m_nEqpModel		= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	m_nEqpSizeType	= pFrame->m_fnEqpSizeType();

	m_pDataManager = &pFrame->m_DataManager;	

	CString strRoot;
	strRoot.Format(_T("%s%s"), ROOT_PATH,JUDGE_ITEM_SET_PATH );
	m_pDataManager->m_stJudgeItemInfo.initDefType();
	m_pDataManager->m_stJudgeItemInfo.setInfoPath(PNL_JUDGE_ITEM, strRoot);
	m_pDataManager->m_stJudgeItemInfo.GetDefInfo();
	//2016.12.12
	m_pDataManager->m_stJudgeItemPriority.initDefType();
	m_pDataManager->m_stJudgeItemPriority.setInfoPath(PNL_JUDGE_PRIORITY, strRoot);
	m_pDataManager->m_stJudgeItemPriority.GetDefInfo();

	//2016.05.28		Defect에 따른 Lower Eqp Bypass 유무 설정 
	if(m_nEqpSizeType == EQP_LARGE_SIZE || m_nEqpSizeType == EQP_MONITOR_SIZE)
	{
		m_pDataManager->m_stJudgeItemBypassInfo.initDefType();
		m_pDataManager->m_stJudgeItemBypassInfo.setInfoPath(PNL_JUDGE_BYPASS_ITEM, strRoot);
		m_pDataManager->m_stJudgeItemBypassInfo.GetDefInfo();
	}

	//2016.12.12 Mr.Chu		ListBox Initialize
	m_fnInitJudgeItemList();

}


void CDlgSetJudgeItem::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_fnJudgeListDataTerminal(SET_JUDGE_ITEM_DEF_NAME,SET_JUDGE_DATA_SAVE);

	CString strRoot;
	strRoot.Format(_T("%s%s"), ROOT_PATH,JUDGE_ITEM_SET_PATH );
	m_pDataManager->m_stJudgeItemInfo.WriteDefInfo();//.init_info(false);
	//2016.12.12
	m_pDataManager->m_stJudgeItemPriority.WriteDefInfo();//.init_info(false);

	//2016.05.28		Defect에 따른 Lower Eqp Bypass 유무 설정
	if(m_nEqpSizeType == EQP_LARGE_SIZE || m_nEqpSizeType == EQP_MONITOR_SIZE)
	{
		m_pDataManager->m_stJudgeItemBypassInfo.WriteDefInfo();

		//UI
		m_ctrlListSet.SetCheck( m_pDataManager->m_stJudgeItemPriority.PanelCodeTest-1,
			m_pDataManager->m_stPanelCodeTest.bTestActive );
		//
	}
	
}


BOOL CDlgSetJudgeItem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//
	m_font.CreateFont(
		15,                     // 글자높이
		8,                     // 글자너비
		0,                      // 출력각도
		0,                      // 기준 선에서의각도
		FW_HEAVY,              // 글자굵기
		FALSE,                  // Italic 적용여부
		FALSE,                  // 밑줄적용여부
		FALSE,                  // 취소선적용여부
		DEFAULT_CHARSET,       // 문자셋종류
		OUT_DEFAULT_PRECIS,    // 출력정밀도
		CLIP_DEFAULT_PRECIS,   // 클리핑정밀도
		DEFAULT_QUALITY,       // 출력문자품질
		DEFAULT_PITCH,         // 글꼴Pitch
		_T("굴림체")           // 글꼴
		);
	//m_font.CreatePointFont(120,_T("굴림"));

	((CButton*)GetDlgItem(IDC_STATIC_PRIORITY	))->SetFont(&m_font);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

//2016.12.12	Mr.Chu
//기능 : 셋팅 관련 리스트박스 초기화 및 Judge 목록 생성
void CDlgSetJudgeItem::m_fnInitJudgeItemList()
{
	int nIndex = 0;
	CString strTemp		= _T("");
	int nCount			= 0;

	m_ctrlListSet.DeleteAllItems( );

	//Title
	DWORD EX_style = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_ONECLICKACTIVATE;

	m_ctrlListSet.SetExtendedStyle(  EX_style );

	//m_ctrlListSet.InsertColumn( 1, _T("Priority"), LVCFMT_LEFT, 50 );
	m_ctrlListSet.InsertColumn( SET_JUDGE_ITEM_DEF_NAME, _T("Defect_Name"), LVCFMT_LEFT, 150 );
	m_ctrlListSet.InsertColumn( SET_JUDGE_ITEM_DEF_PRIORITY, _T("Priority"), LVCFMT_CENTER, 50 );
	m_ctrlListSet.InsertColumn( SET_JUDGE_ITEM_DEF_CODE, _T("Code"), LVCFMT_LEFT, 70 );
	if( m_nEqpSizeType == EQP_LARGE_SIZE || m_nEqpSizeType == EQP_MONITOR_SIZE )
		m_ctrlListSet.InsertColumn( SET_JUDGE_ITEM__LO_EQP_BYPASS, _T("ByPass"), LVCFMT_LEFT, 70 );

	//Set Def Count
	for(int i=0;i<INSP_TYPE_MAX; i++)
		m_ctrlListSet.InsertItem( nIndex++, _T("None") );

	nCount = m_ctrlListSet.GetItemCount();	//List 항목의 갯수를 받아온다.
	
	//우선순위 Index	//Initialize
	for(int i=0;i<nCount;i++)		{	strTemp.Format(_T("%d"),i+1);	m_ctrlListSet.SetItemText(i,SET_JUDGE_ITEM_DEF_PRIORITY, strTemp);	}	

	//m_fnJudgeListDataTerminal(SET_JUDGE_ITEM_DEF_NAME, SET_JUDGE_DATA_CODE);		//Defect Code Set	//Initialize

	//////////////////// Set up Combo Box
	if( m_nEqpSizeType == EQP_LARGE_SIZE  || m_nEqpSizeType == EQP_MONITOR_SIZE )
	{
		CStringList strListItem;

		strListItem.AddTail(_T("FALSE"));
		strListItem.AddTail(_T("TRUE"));
		m_ctrlListSet.SetColumnCombo(3, strListItem);
	}
	//Initialize
	for(int i=0;i<nCount;i++)		{	m_ctrlListSet.SetItemText(i,SET_JUDGE_ITEM__LO_EQP_BYPASS, _T("FALSE"));	}	
	//

	//Load Data
	m_fnIniListCheck();		// Check Missed Def dat
	m_fnJudgeListDataTerminal(SET_JUDGE_ITEM_DEF_PRIORITY, SET_JUDGE_DATA_LOAD);
	//

}

//2016.12.12
//기능 : 현재 지정된 ListBox 의 Cusor Set
void CDlgSetJudgeItem::OnClickListSet(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_nListCurIndex = pNMItemActivate->iItem;

	*pResult = 0;
}

//2016.12.12
//기능 : List Box 의 Cusor를 올려 우선순위를 높힌다.
void CDlgSetJudgeItem::OnBnClickedButtonUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// Cusor가 최상단일 경우 예외처리
	if( m_nListCurIndex == NULL )
		return ;

	//Cur Data 와 Swap Data를 맞바꾼다
	bool	bCheck = false;
	TCHAR	szCur[SET_JUDGE_ITEM_MAX][255]	= {0};
	TCHAR	szSwap[SET_JUDGE_ITEM_MAX][255]	= {0};

	//Get Item
	m_ctrlListSet.GetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_NAME, szCur[SET_JUDGE_ITEM_DEF_NAME], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_CODE, szCur[SET_JUDGE_ITEM_DEF_CODE], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex, SET_JUDGE_ITEM__LO_EQP_BYPASS, szCur[SET_JUDGE_ITEM__LO_EQP_BYPASS], 255);

	m_ctrlListSet.GetItemText(m_nListCurIndex - 1, SET_JUDGE_ITEM_DEF_NAME, szSwap[SET_JUDGE_ITEM_DEF_NAME], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex - 1, SET_JUDGE_ITEM_DEF_CODE, szSwap[SET_JUDGE_ITEM_DEF_CODE], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex - 1, SET_JUDGE_ITEM__LO_EQP_BYPASS, szSwap[SET_JUDGE_ITEM__LO_EQP_BYPASS], 255);

	//Set Item
	m_ctrlListSet.SetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_NAME, szSwap[SET_JUDGE_ITEM_DEF_NAME]);
	m_ctrlListSet.SetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_CODE, szSwap[SET_JUDGE_ITEM_DEF_CODE]);
	m_ctrlListSet.SetItemText(m_nListCurIndex, SET_JUDGE_ITEM__LO_EQP_BYPASS, szSwap[SET_JUDGE_ITEM__LO_EQP_BYPASS]);

	m_ctrlListSet.SetItemText(m_nListCurIndex - 1, SET_JUDGE_ITEM_DEF_NAME, szCur[SET_JUDGE_ITEM_DEF_NAME]);
	m_ctrlListSet.SetItemText(m_nListCurIndex - 1, SET_JUDGE_ITEM_DEF_CODE, szCur[SET_JUDGE_ITEM_DEF_CODE]);
	m_ctrlListSet.SetItemText(m_nListCurIndex - 1, SET_JUDGE_ITEM__LO_EQP_BYPASS, szCur[SET_JUDGE_ITEM__LO_EQP_BYPASS]);

	//Swap Check Box
	bCheck = m_ctrlListSet.GetCheck(m_nListCurIndex);
	if( bCheck != m_ctrlListSet.GetCheck( m_nListCurIndex - 1 ) )
	{
		m_ctrlListSet.SetCheck(m_nListCurIndex, m_ctrlListSet.GetCheck( m_nListCurIndex - 1 ) );
		m_ctrlListSet.SetCheck(m_nListCurIndex - 1, bCheck );
	}

	m_nListCurIndex--;
	if( m_nListCurIndex <= NULL )
		m_nListCurIndex = NULL;
	m_ctrlListSet.SetItemState(m_nListCurIndex,LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);  

}

//2016.12.12
//기능 : List Box 의 Cusor를 올려 우선순위를 낮춘다.
void CDlgSetJudgeItem::OnBnClickedButtonDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCount = m_ctrlListSet.GetItemCount();	//항목의 갯수를 받아온다.

	// Cusor가 최상단일 경우 예외처리
	if( m_nListCurIndex == nCount - 1 )
		return ;

	//Cur Data 와 Swap Data를 맞바꾼다
	bool	bCheck = false;
	TCHAR	szCur[SET_JUDGE_ITEM_MAX][255]	= {0};
	TCHAR	szSwap[SET_JUDGE_ITEM_MAX][255]	= {0};

	//Get Item
	m_ctrlListSet.GetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_NAME, szCur[SET_JUDGE_ITEM_DEF_NAME], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_CODE, szCur[SET_JUDGE_ITEM_DEF_CODE], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex, SET_JUDGE_ITEM__LO_EQP_BYPASS, szCur[SET_JUDGE_ITEM__LO_EQP_BYPASS], 255);

	m_ctrlListSet.GetItemText(m_nListCurIndex + 1, SET_JUDGE_ITEM_DEF_NAME, szSwap[SET_JUDGE_ITEM_DEF_NAME], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex + 1, SET_JUDGE_ITEM_DEF_CODE, szSwap[SET_JUDGE_ITEM_DEF_CODE], 255);
	m_ctrlListSet.GetItemText(m_nListCurIndex + 1, SET_JUDGE_ITEM__LO_EQP_BYPASS, szSwap[SET_JUDGE_ITEM__LO_EQP_BYPASS], 255);

	//Set Item
	m_ctrlListSet.SetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_NAME, szSwap[SET_JUDGE_ITEM_DEF_NAME]);
	m_ctrlListSet.SetItemText(m_nListCurIndex, SET_JUDGE_ITEM_DEF_CODE, szSwap[SET_JUDGE_ITEM_DEF_CODE]);
	m_ctrlListSet.SetItemText(m_nListCurIndex, SET_JUDGE_ITEM__LO_EQP_BYPASS, szSwap[SET_JUDGE_ITEM__LO_EQP_BYPASS]);

	m_ctrlListSet.SetItemText(m_nListCurIndex + 1, SET_JUDGE_ITEM_DEF_NAME, szCur[SET_JUDGE_ITEM_DEF_NAME]);
	m_ctrlListSet.SetItemText(m_nListCurIndex + 1, SET_JUDGE_ITEM_DEF_CODE, szCur[SET_JUDGE_ITEM_DEF_CODE]);
	m_ctrlListSet.SetItemText(m_nListCurIndex + 1, SET_JUDGE_ITEM__LO_EQP_BYPASS, szCur[SET_JUDGE_ITEM__LO_EQP_BYPASS]);

	//Swap Check Box
	bCheck = m_ctrlListSet.GetCheck(m_nListCurIndex);
	if( bCheck != m_ctrlListSet.GetCheck( m_nListCurIndex + 1 ) )
	{
		m_ctrlListSet.SetCheck(m_nListCurIndex, m_ctrlListSet.GetCheck( m_nListCurIndex + 1 ) );
		m_ctrlListSet.SetCheck(m_nListCurIndex + 1, bCheck );
	}
	
	m_nListCurIndex++;
	if( m_nListCurIndex >= nCount  )
		m_nListCurIndex = nCount;
	m_ctrlListSet.SetItemState(m_nListCurIndex,LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);  
}


void CDlgSetJudgeItem::OnBnClickedButtonAllCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCount = m_ctrlListSet.GetItemCount();	// 항목의 갯수를 받아온다.

	for(int i=0;i<nCount;i++)
	{
		m_ctrlListSet.SetCheck( i,TRUE );
	}
}


void CDlgSetJudgeItem::OnBnClickedButtonNotCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCount = m_ctrlListSet.GetItemCount();	// 항목의 갯수를 받아온다.

	for(int i=0;i<nCount;i++)
	{
		m_ctrlListSet.SetCheck( i,FALSE );
	}
}



//2016.12.12
//기능		: ListBox의 Data를 가져오거나 셋팅한다.
//파라메타	: nCompItemIndex - List Box의 목록( Row )을 비교한다.
//				ex) SET_JUDGE_ITEM_DEF_NAME			 - 디펙이름
//				ex) SET_JUDGE_ITEM_DEF_PRIORITY		 - 우선순위
//				ex) SET_JUDGE_ITEM_DEF_CODE			 - 디펙 코드
//				ex) SET_JUDGE_ITEM__LO_EQP_BYPASS	 - 하류설비 Bypass 유무
//
//			  nFunc			 - 함수 기능 설정
//				ex) SET_JUDGE_DATA_SAVE	 - 저장 및 해당 값 셋팅
//				ex) SET_JUDGE_DATA_LOAD	 - 불러오기 및 갱신
//				ex) SET_JUDGE_DATA_CODE	 - Defect Code 분류 및 해당데이터 갱신

//Memo		 : 다음작업 함수 하나에 전체 검색하지 말고 모듈 하나에 함수를 여러개 쓸것. 수정시 비효율적.
void CDlgSetJudgeItem::m_fnJudgeListDataTerminal(int nCompItemIndex, int nFunc)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	bool bSetItem = false;
	int nIndex = 0, nTemp = 0;
	TCHAR szData[255]		= {0};
	TCHAR szTemp[255]		= {0};
	CString strCompData		= _T("");
	CString strCompDefine	= _T("");
	CString strSetItem		= _T("");
	CString strTemp			= _T("");
	CString strArrSetItem[SET_JUDGE_ITEM_MAX];

	if( nFunc == SET_JUDGE_DATA_SAVE )			// 변수 초기화
	{
		m_pDataManager->m_stJudgeItemInfo.initDefType();
		m_pDataManager->m_stJudgeItemPriority.initDefType();
		m_pDataManager->m_stJudgeItemBypassInfo.initDefType();
	}

	int nCount = m_ctrlListSet.GetItemCount();	// 항목의 갯수를 받아온다.
	m_pDataManager->m_stJudgeItemInfo.TotalCnt = nCount;
	m_pDataManager->m_stJudgeItemBypassInfo.TotalCnt = nCount;
	
	while( TRUE )
	{
		// Item Set
		if( bSetItem == true )
		{
			switch( nFunc )
			{
			case SET_JUDGE_DATA_LOAD:
				m_ctrlListSet.SetItemText(nIndex, SET_JUDGE_ITEM_DEF_NAME, strArrSetItem[SET_JUDGE_ITEM_DEF_NAME]);
				m_ctrlListSet.SetItemText(nIndex, SET_JUDGE_ITEM_DEF_PRIORITY, strArrSetItem[SET_JUDGE_ITEM_DEF_PRIORITY]);
				m_ctrlListSet.SetItemText(nIndex, SET_JUDGE_ITEM_DEF_CODE, strArrSetItem[SET_JUDGE_ITEM_DEF_CODE]);
				m_ctrlListSet.SetItemText(nIndex, SET_JUDGE_ITEM__LO_EQP_BYPASS, strArrSetItem[SET_JUDGE_ITEM__LO_EQP_BYPASS]);
				break;

			case SET_JUDGE_DATA_SAVE:
				// None
				break;

			case SET_JUDGE_DATA_CODE:
				m_ctrlListSet.SetItemText(nIndex, SET_JUDGE_ITEM_DEF_CODE, strSetItem);
				break;
			default:
				// None
				break;
			}
			bSetItem = false;
			nIndex++;
		}
		//

		// End Of While
		if( nIndex >= nCount )
			break;
		//

		//Get Index Item
		m_ctrlListSet.GetItemText(nIndex, nCompItemIndex, szData, 255);
		strCompData.Format(_T("%s"),szData);
		strCompData.Trim();
		//

 		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_EDGE_TFT_CORNER, TFTCORNERBROKEN, strCompData, nFunc, strArrSetItem,
 							m_pDataManager->m_stJudgeItemInfo.TftCornerBroken, m_pDataManager->m_stJudgeItemBypassInfo.TftCornerBroken, m_pDataManager->m_stJudgeItemPriority.TftCornerBroken);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_EDGE_CF_CORNER, CFCORNERBROKEN, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CfCornerBroken, m_pDataManager->m_stJudgeItemBypassInfo.CfCornerBroken, m_pDataManager->m_stJudgeItemPriority.CfCornerBroken);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_EDGE_TFT_BROKEN, TFTBROKEN, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TftBroken, m_pDataManager->m_stJudgeItemBypassInfo.TftBroken, m_pDataManager->m_stJudgeItemPriority.TftBroken);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_EDGE_CF_BROKEN, CFBROKEN, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CfBroken, m_pDataManager->m_stJudgeItemBypassInfo.CfBroken, m_pDataManager->m_stJudgeItemPriority.CfBroken);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_EDGE_TFT_BURR, TFTBURR, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TftBurr, m_pDataManager->m_stJudgeItemBypassInfo.TftBurr, m_pDataManager->m_stJudgeItemPriority.TftBurr);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_EDGE_CF_BURR, CFBURR, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CfBurr, m_pDataManager->m_stJudgeItemBypassInfo.CfBurr, m_pDataManager->m_stJudgeItemPriority.CfBurr);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_PAD_SCRATCH, SOCKETSCRACH, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.SocketScrach, m_pDataManager->m_stJudgeItemBypassInfo.SocketScrach, m_pDataManager->m_stJudgeItemPriority.SocketScrach);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_PAD_BURNT, SOCKETBURN, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.SocketBurn, m_pDataManager->m_stJudgeItemBypassInfo.SocketBurn, m_pDataManager->m_stJudgeItemPriority.SocketBurn);

		//
		if( m_nEqpModel == EQP_TYPE_ABI ){
			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_MEASURE, CFPOLMEASUREFAIL, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfMeasureFail, m_pDataManager->m_stJudgeItemBypassInfo.CfMeasureFail, m_pDataManager->m_stJudgeItemPriority.CfMeasureFail);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_MEASURE, TFTPOLMEASUREFAIL, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftMeasureFail, m_pDataManager->m_stJudgeItemBypassInfo.TftMeasureFail, m_pDataManager->m_stJudgeItemPriority.TftMeasureFail);
		}
		else if( m_nEqpModel == EQP_TYPE_AVL ){
			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_MEASURE, CFMEASUREFAIL, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfMeasureFail, m_pDataManager->m_stJudgeItemBypassInfo.CfMeasureFail, m_pDataManager->m_stJudgeItemPriority.CfMeasureFail);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_MEASURE, TFTMEASUREFAIL, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftMeasureFail, m_pDataManager->m_stJudgeItemBypassInfo.TftMeasureFail, m_pDataManager->m_stJudgeItemPriority.TftMeasureFail);
		}
		//

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_DIMPLE_POINT, CFDIMPLEPOINT, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CfDimplePoint, m_pDataManager->m_stJudgeItemBypassInfo.CfDimplePoint, m_pDataManager->m_stJudgeItemPriority.CfDimplePoint);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_DIMPLE_POINT, TFTDIMPLEPOINT, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TftDimplePoint, m_pDataManager->m_stJudgeItemBypassInfo.TftDimplePoint, m_pDataManager->m_stJudgeItemPriority.TftDimplePoint);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_ACTIVE_BROKEN, CFCELLBROKEN, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CfCellBroken, m_pDataManager->m_stJudgeItemBypassInfo.CfCellBroken, m_pDataManager->m_stJudgeItemPriority.CfCellBroken);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_ACTIVE_BROKEN, TFTCELLBROKEN, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TftCellBroken, m_pDataManager->m_stJudgeItemBypassInfo.TftCellBroken, m_pDataManager->m_stJudgeItemPriority.TftCellBroken);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_OTHER_LINE, ETCLINE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.EtcLine, m_pDataManager->m_stJudgeItemBypassInfo.EtcLine, m_pDataManager->m_stJudgeItemPriority.EtcLine);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_OTHER_POINT, ETCPOINT, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.EtcPoint, m_pDataManager->m_stJudgeItemBypassInfo.EtcPoint, m_pDataManager->m_stJudgeItemPriority.EtcPoint);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_ALIGN_FAIL, ALIGNFAIL, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.AlignFail, m_pDataManager->m_stJudgeItemBypassInfo.AlignFail, m_pDataManager->m_stJudgeItemPriority.AlignFail);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CFC_LUMINANCE, CFCLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CFCLuminance, m_pDataManager->m_stJudgeItemBypassInfo.CFCLuminance, m_pDataManager->m_stJudgeItemPriority.CFCLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CBC_LUMINANCE, CBCLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CBCLuminance, m_pDataManager->m_stJudgeItemBypassInfo.CBCLuminance, m_pDataManager->m_stJudgeItemPriority.CBCLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CFS_LUMINANCE, CFSLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CFSLuminance, m_pDataManager->m_stJudgeItemBypassInfo.CFSLuminance, m_pDataManager->m_stJudgeItemPriority.CFSLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CBS_LUMINANCE, CBSLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CBSLuminance, m_pDataManager->m_stJudgeItemBypassInfo.CBSLuminance, m_pDataManager->m_stJudgeItemPriority.CBSLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFC_LUMINANCE, TFCLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TFCLuminance, m_pDataManager->m_stJudgeItemBypassInfo.TFCLuminance, m_pDataManager->m_stJudgeItemPriority.TFCLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TBC_LUMINANCE, TBCLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TBCLuminance, m_pDataManager->m_stJudgeItemBypassInfo.TBCLuminance, m_pDataManager->m_stJudgeItemPriority.TBCLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFS_LUMINANCE, TFSLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TFSLuminance, m_pDataManager->m_stJudgeItemBypassInfo.TFSLuminance, m_pDataManager->m_stJudgeItemPriority.TFSLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TBS_LUMINANCE, TBSLUMINANCE, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TBSLuminance, m_pDataManager->m_stJudgeItemBypassInfo.TBSLuminance, m_pDataManager->m_stJudgeItemPriority.TBSLuminance);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_PROTECTION, CFPROTECTION, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.CfProtection, m_pDataManager->m_stJudgeItemBypassInfo.CfProtection, m_pDataManager->m_stJudgeItemPriority.CfProtection);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_PROTECTION, TFTPROTECTION, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.TftProtection, m_pDataManager->m_stJudgeItemBypassInfo.TftProtection, m_pDataManager->m_stJudgeItemPriority.TftProtection);
#if 0
		//2020.02.28 add by mby
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION1, BUBBLE_STATION1, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation1, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation1, m_pDataManager->m_stJudgeItemPriority.BubbleStation1);
		
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION2, BUBBLE_STATION2, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation2, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation2, m_pDataManager->m_stJudgeItemPriority.BubbleStation2);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION3, BUBBLE_STATION3, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation3, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation3, m_pDataManager->m_stJudgeItemPriority.BubbleStation3);
		
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION4, BUBBLE_STATION4, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation4, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation4, m_pDataManager->m_stJudgeItemPriority.BubbleStation4);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION5, BUBBLE_STATION5, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation5, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation5, m_pDataManager->m_stJudgeItemPriority.BubbleStation5);
		
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION6, BUBBLE_STATION6, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation6, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation6, m_pDataManager->m_stJudgeItemPriority.BubbleStation6);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION7, BUBBLE_STATION7, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation7, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation7, m_pDataManager->m_stJudgeItemPriority.BubbleStation7);
		
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION8, BUBBLE_STATION8, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation8, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation8, m_pDataManager->m_stJudgeItemPriority.BubbleStation8);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION9, BUBBLE_STATION9, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation9, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation9, m_pDataManager->m_stJudgeItemPriority.BubbleStation9);
		
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION10, BUBBLE_STATION10, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation10, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation10, m_pDataManager->m_stJudgeItemPriority.BubbleStation10);

		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION11, BUBBLE_STATION11, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation11, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation11, m_pDataManager->m_stJudgeItemPriority.BubbleStation11);
		
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_BUBBLB_STATION12, BUBBLE_STATION12, strCompData, nFunc, strArrSetItem,
		m_pDataManager->m_stJudgeItemInfo.BubbleStation12, m_pDataManager->m_stJudgeItemBypassInfo.BubbleStation12, m_pDataManager->m_stJudgeItemPriority.BubbleStation12);
#endif
		// Panel Code Test
		bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_PANEL_CODE_TEST, pFrame->m_DataManager.m_stPanelCodeTest.nJudgeCode, strCompData, nFunc, strArrSetItem,
			m_pDataManager->m_stJudgeItemInfo.PanelCodeTest, m_pDataManager->m_stJudgeItemBypassInfo.PanelCodeTest, m_pDataManager->m_stJudgeItemPriority.PanelCodeTest);
		//		Common Item End

		if( m_nEqpModel == EQP_TYPE_ABI )
		{
			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TDI_MURA, MURA, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.mura, m_pDataManager->m_stJudgeItemBypassInfo.mura, m_pDataManager->m_stJudgeItemPriority.mura);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_SUBSTANCE, CFPOLSUBSTANCE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CFpolSubstance, m_pDataManager->m_stJudgeItemBypassInfo.CFpolSubstance, m_pDataManager->m_stJudgeItemPriority.CFpolSubstance);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_SUBSTANCE, TFTPOLSUBSTANCE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TFTpolSubstance, m_pDataManager->m_stJudgeItemBypassInfo.TFTpolSubstance, m_pDataManager->m_stJudgeItemPriority.TFTpolSubstance);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_POL_BUBBLE, CFPOLBUBBLE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfPolBubble, m_pDataManager->m_stJudgeItemBypassInfo.CfPolBubble, m_pDataManager->m_stJudgeItemPriority.CfPolBubble);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_POL_SCRATCH, CFPOLSCRATCH, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfPolScrach, m_pDataManager->m_stJudgeItemBypassInfo.CfPolScrach, m_pDataManager->m_stJudgeItemPriority.CfPolScrach);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_POL_BUBBLE, TFTPOLBUBBLE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftPolBubble, m_pDataManager->m_stJudgeItemBypassInfo.TftPolBubble, m_pDataManager->m_stJudgeItemPriority.TftPolBubble);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_POL_SCRATCH, TFTPOLSCRATCH, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftPolScrach, m_pDataManager->m_stJudgeItemBypassInfo.TftPolScrach, m_pDataManager->m_stJudgeItemPriority.TftPolScrach);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TDI_MINOR, POLMINOR, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.PolMinor, m_pDataManager->m_stJudgeItemBypassInfo.PolMinor, m_pDataManager->m_stJudgeItemPriority.PolMinor);


			//// Contact
			if( nFunc == SET_JUDGE_DATA_SAVE ||  nFunc == SET_JUDGE_DATA_CODE )	
			{
				strCompDefine = _T("");
				strCompDefine = E_DEFECT_NAME_CONTACT;
				strCompDefine.Trim();

				strSetItem = _T("");
				strSetItem.Format(_T("%d , %d"),ITORESISTILEGAL,CONTACTFAIL);
			}
			else if(nFunc == SET_JUDGE_DATA_LOAD )	
			{
				strCompDefine = _T("");
				strCompDefine.Format(_T("%d"),m_pDataManager->m_stJudgeItemPriority.Contact);
			}
			if(strCompData == strCompDefine && bSetItem == FALSE)	{
				if( nFunc == SET_JUDGE_DATA_SAVE )
				{
					m_ctrlListSet.GetItemText(nIndex, SET_JUDGE_ITEM_DEF_PRIORITY, szTemp, 255);
					m_pDataManager->m_stJudgeItemPriority.Contact = _tstoi( szTemp );

					m_pDataManager->m_stJudgeItemInfo.Contact = m_ctrlListSet.GetCheck(nIndex);

					if( m_nEqpSizeType == EQP_LARGE_SIZE || m_nEqpSizeType == EQP_MONITOR_SIZE )
					{
						if( m_pDataManager->m_stJudgeItemInfo.Contact )
						{
							m_ctrlListSet.GetItemText(nIndex, SET_JUDGE_ITEM__LO_EQP_BYPASS, szTemp, 255);
							strTemp.Format(_T("%s"),szTemp);
							strTemp.Trim();
							if( strTemp == _T("TRUE") )
								m_pDataManager->m_stJudgeItemBypassInfo.Contact = TRUE;
							else if( strTemp == _T("FALSE") )
								m_pDataManager->m_stJudgeItemBypassInfo.Contact = FALSE;
						}
					}
				}
				else if(nFunc == SET_JUDGE_DATA_LOAD )	
				{
					if(m_pDataManager->m_stJudgeItemInfo.Contact)
						m_ctrlListSet.SetCheck(nIndex);

					strArrSetItem[SET_JUDGE_ITEM_DEF_NAME]			= E_DEFECT_NAME_CONTACT;
					strArrSetItem[SET_JUDGE_ITEM_DEF_PRIORITY]		= strCompDefine;
					strArrSetItem[SET_JUDGE_ITEM_DEF_CODE].Format(_T("%d"),CONTACTFAIL);
					if( m_nEqpSizeType == EQP_LARGE_SIZE || m_nEqpSizeType == EQP_MONITOR_SIZE )
					{
						if(m_pDataManager->m_stJudgeItemBypassInfo.Contact)
							strArrSetItem[SET_JUDGE_ITEM__LO_EQP_BYPASS] = _T("TRUE");
						else
							strArrSetItem[SET_JUDGE_ITEM__LO_EQP_BYPASS] = _T("FALSE");
					}
				}
				bSetItem = true;	continue;	
			}	
			//

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_POL_MISS, POLMISS, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.PolMiss, m_pDataManager->m_stJudgeItemBypassInfo.PolMiss, m_pDataManager->m_stJudgeItemPriority.PolMiss);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TDI_LUMINANCE, TDILUMINANCE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TDILuminance, m_pDataManager->m_stJudgeItemBypassInfo.TDILuminance, m_pDataManager->m_stJudgeItemPriority.TDILuminance);
		}
		else if( m_nEqpModel == EQP_TYPE_AVL )
		{
			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_SCRATCH, CFSCRATCH, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfScrach, m_pDataManager->m_stJudgeItemBypassInfo.CfScrach, m_pDataManager->m_stJudgeItemPriority.CfScrach);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_SCRATCH, TFTSCRATCH, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftScrach, m_pDataManager->m_stJudgeItemBypassInfo.TftScrach, m_pDataManager->m_stJudgeItemPriority.TftScrach);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_WHITE_TURBIDITY, CFWHITEBLUR, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfWhiteBlur, m_pDataManager->m_stJudgeItemBypassInfo.CfWhiteBlur, m_pDataManager->m_stJudgeItemPriority.CfWhiteBlur);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_ITO, ITOPEEL, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.ItoPeel, m_pDataManager->m_stJudgeItemBypassInfo.ItoPeel, m_pDataManager->m_stJudgeItemPriority.ItoPeel);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_ETCHING_LINE, CFEDGEILEGAL, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfEdgeilegal, m_pDataManager->m_stJudgeItemBypassInfo.CfEdgeilegal, m_pDataManager->m_stJudgeItemPriority.CfEdgeilegal);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_DIMPLE_LINE, CFDIMPLELINE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfDimpleLine, m_pDataManager->m_stJudgeItemBypassInfo.CfDimpleLine, m_pDataManager->m_stJudgeItemPriority.CfDimpleLine);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_WHITE_TURBIDITY, TFTWHITEBLUR, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftWhiteBlur, m_pDataManager->m_stJudgeItemBypassInfo.TftWhiteBlur, m_pDataManager->m_stJudgeItemPriority.TftWhiteBlur);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_ETCHING_LINE, TFTEDGEILEGAL, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftEdgeilegal, m_pDataManager->m_stJudgeItemBypassInfo.TftEdgeilegal, m_pDataManager->m_stJudgeItemPriority.TftEdgeilegal);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_DIMPLE_LINE, TFTDIMPLELINE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftDimpleLine, m_pDataManager->m_stJudgeItemBypassInfo.TftDimpleLine, m_pDataManager->m_stJudgeItemPriority.TftDimpleLine);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_PIMPLE_POINT, CFPIMPLEPOINT, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfPimplePoint, m_pDataManager->m_stJudgeItemBypassInfo.CfPimplePoint, m_pDataManager->m_stJudgeItemPriority.CfPimplePoint);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_PIMPLE_POINT, TFTPIMPLEPOINT, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftPimplePoint, m_pDataManager->m_stJudgeItemBypassInfo.TftPimplePoint, m_pDataManager->m_stJudgeItemPriority.TftPimplePoint);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_CF_PIMPLE_LINE, CFPIMPLELINE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfPimpleLine, m_pDataManager->m_stJudgeItemBypassInfo.CfPimpleLine, m_pDataManager->m_stJudgeItemPriority.CfPimpleLine);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_TFT_PIMPLE_LINE, TFTPIMPLELINE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.TftPimpleLine, m_pDataManager->m_stJudgeItemBypassInfo.TftPimpleLine, m_pDataManager->m_stJudgeItemPriority.TftPimpleLine);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_PIMPLE_CIRCLE, CFPIMPLECIRCLE, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfPimpleCircle, m_pDataManager->m_stJudgeItemBypassInfo.CfPimpleCircle, m_pDataManager->m_stJudgeItemPriority.CfPimpleCircle);

			bSetItem = m_fnDataTerminal(bSetItem, nIndex, E_DEFECT_NAME_DIRT, CFWAVEPATTERN, strCompData, nFunc, strArrSetItem,
				m_pDataManager->m_stJudgeItemInfo.CfWavePattern, m_pDataManager->m_stJudgeItemBypassInfo.CfWavePattern, m_pDataManager->m_stJudgeItemPriority.CfWavePattern);
		}


		if( !bSetItem )
			nIndex++;
	}

}


BOOL CDlgSetJudgeItem::m_fnDataTerminal(BOOL bCheck, int nIndex, CString strDefName, int strDefCode, 
											CString strCompData, int nFunc, CString *strArrSetItem,
											int &nJudgeItem, int &nBypassInfo, int &nPriority)
{
	if( bCheck )			// Checked Already, Function Bypass
		return TRUE;

	TCHAR	szTemp[255]		= {0};
	CString strCompDefine	= _T("");
	CString strTemp			= _T("");
	CString strCntCheck		= _T("");	//2021.03.16 add by mby
	
	if( nFunc == SET_JUDGE_DATA_SAVE ||  nFunc == SET_JUDGE_DATA_CODE )	
	{
		strCompDefine = _T("");
		strCompDefine = strDefName;
		strCompDefine.Trim();
	}
	else if(nFunc == SET_JUDGE_DATA_LOAD )	
	{
		strCompDefine = _T("");
		strCompDefine.Format(_T("%d"),nPriority);
	}
	if(strCompData == strCompDefine)	{
		if( nFunc == SET_JUDGE_DATA_SAVE )
		{
			m_ctrlListSet.GetItemText(nIndex, SET_JUDGE_ITEM_DEF_PRIORITY, szTemp, 255);
			nPriority = _tstoi( szTemp );

			nJudgeItem = m_ctrlListSet.GetCheck(nIndex);

			if( m_nEqpSizeType == EQP_LARGE_SIZE || m_nEqpSizeType == EQP_MONITOR_SIZE )
			{
				if( nJudgeItem )
				{
					m_ctrlListSet.GetItemText(nIndex, SET_JUDGE_ITEM__LO_EQP_BYPASS, szTemp, 255);
					strTemp.Format(_T("%s"),szTemp);
					strTemp.Trim();
					if( strTemp == _T("TRUE") )
						nBypassInfo = TRUE;
					else if( strTemp == _T("FALSE") )
						nBypassInfo = FALSE;
				}
			}
		}
		else if(nFunc == SET_JUDGE_DATA_LOAD )	
		{
			if( nJudgeItem )
				m_ctrlListSet.SetCheck(nIndex);

			strArrSetItem[SET_JUDGE_ITEM_DEF_NAME]			= strDefName;
			strArrSetItem[SET_JUDGE_ITEM_DEF_PRIORITY]		= strCompDefine;
			//2021.03.16 add by mby S-
			strCntCheck.Format(_T("%d"),strDefCode);
			int nCnt = strCntCheck.GetLength();
			if(nCnt < 3) strArrSetItem[SET_JUDGE_ITEM_DEF_CODE].Format(_T("AO%d"),strDefCode);
			else	strArrSetItem[SET_JUDGE_ITEM_DEF_CODE].Format(_T("%d"),strDefCode);
			//2021.03.16 add by mby E-
			if( m_nEqpSizeType == EQP_LARGE_SIZE || m_nEqpSizeType == EQP_MONITOR_SIZE )
			{
				if( nBypassInfo )
					strArrSetItem[SET_JUDGE_ITEM__LO_EQP_BYPASS] = _T("TRUE");
				else
					strArrSetItem[SET_JUDGE_ITEM__LO_EQP_BYPASS] = _T("FALSE");
			}
		}
		return TRUE;
	}	
	return FALSE;
}

//2017.05.23			DlgSetJudgeItem.ini File 작업
//기능		: 추가 된 Code에 관한 Ini update 유무 체크
//Memo		: Data Loading 시 누락된 Code 관련된 Ini Data 내 해당 Code 관련 정보를 추가해 주어 Data 꼬임을 방지한다.
void CDlgSetJudgeItem::m_fnIniListCheck()
{
	int nCnt = 0;

	// Count Check
	if( m_pDataManager->m_stJudgeItemPriority.TftCornerBroken > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TftCornerBroken;
	
	if( m_pDataManager->m_stJudgeItemPriority.CfCornerBroken > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CfCornerBroken;

	if( m_pDataManager->m_stJudgeItemPriority.TftBroken > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TftBroken;

	if( m_pDataManager->m_stJudgeItemPriority.CfBroken > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CfBroken;

	if( m_pDataManager->m_stJudgeItemPriority.TftBurr > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TftBurr;

	if( m_pDataManager->m_stJudgeItemPriority.CfBurr > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CfBurr;

	if( m_pDataManager->m_stJudgeItemPriority.SocketScrach > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.SocketScrach;

	if( m_pDataManager->m_stJudgeItemPriority.SocketBurn > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.SocketBurn;

	if( m_pDataManager->m_stJudgeItemPriority.CfMeasureFail > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CfMeasureFail;

	if( m_pDataManager->m_stJudgeItemPriority.CfMeasureFail > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TftMeasureFail;

	if( m_pDataManager->m_stJudgeItemPriority.CfDimplePoint > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CfDimplePoint;

	if( m_pDataManager->m_stJudgeItemPriority.TftDimplePoint > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TftDimplePoint;

	if( m_pDataManager->m_stJudgeItemPriority.CfCellBroken > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CfCellBroken;

	if( m_pDataManager->m_stJudgeItemPriority.TftCellBroken > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TftCellBroken;

	if( m_pDataManager->m_stJudgeItemPriority.EtcLine > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.EtcLine;

	if( m_pDataManager->m_stJudgeItemPriority.EtcPoint > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.EtcPoint;

	if( m_pDataManager->m_stJudgeItemPriority.AlignFail > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.AlignFail;

	if( m_pDataManager->m_stJudgeItemPriority.CFCLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CFCLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.CBCLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CBCLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.CFSLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CFSLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.CBSLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CBSLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.TFCLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TFCLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.TBCLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TBCLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.TFSLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TFSLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.TBSLuminance > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TBSLuminance;

	if( m_pDataManager->m_stJudgeItemPriority.CfProtection > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.CfProtection;

	if( m_pDataManager->m_stJudgeItemPriority.TftProtection > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.TftProtection;

	if( m_pDataManager->m_stJudgeItemPriority.PanelCodeTest > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.PanelCodeTest;

	//
	if( m_nEqpModel == EQP_TYPE_ABI )
	{
		if( m_pDataManager->m_stJudgeItemPriority.mura > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.mura;

		if( m_pDataManager->m_stJudgeItemPriority.CFpolSubstance > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CFpolSubstance;

		if( m_pDataManager->m_stJudgeItemPriority.TFTpolSubstance > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TFTpolSubstance;

		if( m_pDataManager->m_stJudgeItemPriority.CfPolBubble > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfPolBubble;

		if( m_pDataManager->m_stJudgeItemPriority.CfPolScrach > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfPolScrach;

		if( m_pDataManager->m_stJudgeItemPriority.TftPolBubble > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftPolBubble;

		if( m_pDataManager->m_stJudgeItemPriority.TftPolScrach > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftPolScrach;

		if( m_pDataManager->m_stJudgeItemPriority.PolMinor > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.PolMinor;

		if( m_pDataManager->m_stJudgeItemPriority.Contact > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.Contact;

		if( m_pDataManager->m_stJudgeItemPriority.PolMiss > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.PolMiss;

		if( m_pDataManager->m_stJudgeItemPriority.TDILuminance > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TDILuminance;

		//2020.02.28 add by mby
		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation1 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation1;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation2 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation2;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation3 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation3;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation4 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation4;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation5 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation5;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation6 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation6;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation7 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation7;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation8 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation8;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation9 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation9;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation10 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation10;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation11 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation11;

		if( m_pDataManager->m_stJudgeItemPriority.BubbleStation12 > nCnt )
		nCnt = m_pDataManager->m_stJudgeItemPriority.BubbleStation12;
	}
	else if( m_nEqpModel == EQP_TYPE_AVL )
	{

		if( m_pDataManager->m_stJudgeItemPriority.CfScrach > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfScrach;

		if( m_pDataManager->m_stJudgeItemPriority.TftScrach > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftScrach;

		if( m_pDataManager->m_stJudgeItemPriority.CfWhiteBlur > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfWhiteBlur;

		if( m_pDataManager->m_stJudgeItemPriority.ItoPeel > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.ItoPeel;

		if( m_pDataManager->m_stJudgeItemPriority.CfEdgeilegal > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfEdgeilegal;

		if( m_pDataManager->m_stJudgeItemPriority.CfDimpleLine > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfDimpleLine;

		if( m_pDataManager->m_stJudgeItemPriority.TftWhiteBlur > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftWhiteBlur;

		if( m_pDataManager->m_stJudgeItemPriority.TftEdgeilegal > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftEdgeilegal;

		if( m_pDataManager->m_stJudgeItemPriority.TftDimpleLine > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftDimpleLine;

		if( m_pDataManager->m_stJudgeItemPriority.CfPimplePoint > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfPimplePoint;

		if( m_pDataManager->m_stJudgeItemPriority.TftPimplePoint > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftPimplePoint;

		if( m_pDataManager->m_stJudgeItemPriority.CfPimpleLine > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfPimpleLine;

		if( m_pDataManager->m_stJudgeItemPriority.TftPimpleLine > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.TftPimpleLine;

		if( m_pDataManager->m_stJudgeItemPriority.CfPimpleCircle > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfPimpleCircle;

		if( m_pDataManager->m_stJudgeItemPriority.CfWavePattern > nCnt )
			nCnt = m_pDataManager->m_stJudgeItemPriority.CfWavePattern;
	}

	//Additional missed Def Data
	if( !m_pDataManager->m_stJudgeItemPriority.TftCornerBroken )
		m_pDataManager->m_stJudgeItemPriority.TftCornerBroken = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfCornerBroken )
		m_pDataManager->m_stJudgeItemPriority.CfCornerBroken = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TftBroken )
		m_pDataManager->m_stJudgeItemPriority.TftBroken = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfBroken )
		m_pDataManager->m_stJudgeItemPriority.CfBroken = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TftBurr )
		m_pDataManager->m_stJudgeItemPriority.TftBurr = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfBurr)
		m_pDataManager->m_stJudgeItemPriority.CfBurr = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.SocketScrach )
		m_pDataManager->m_stJudgeItemPriority.SocketScrach = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.SocketBurn )
		nCnt = m_pDataManager->m_stJudgeItemPriority.SocketBurn = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfMeasureFail )
		m_pDataManager->m_stJudgeItemPriority.CfMeasureFail = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfMeasureFail )
		m_pDataManager->m_stJudgeItemPriority.TftMeasureFail = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfDimplePoint )
		m_pDataManager->m_stJudgeItemPriority.CfDimplePoint = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TftDimplePoint )
		m_pDataManager->m_stJudgeItemPriority.TftDimplePoint = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfCellBroken )
		m_pDataManager->m_stJudgeItemPriority.CfCellBroken = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TftCellBroken )
		m_pDataManager->m_stJudgeItemPriority.TftCellBroken = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.EtcLine )
		m_pDataManager->m_stJudgeItemPriority.EtcLine = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.EtcPoint )
		m_pDataManager->m_stJudgeItemPriority.EtcPoint = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.AlignFail )
		m_pDataManager->m_stJudgeItemPriority.AlignFail = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CFCLuminance )
		m_pDataManager->m_stJudgeItemPriority.CFCLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CBCLuminance )
		m_pDataManager->m_stJudgeItemPriority.CBCLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CFSLuminance )
		m_pDataManager->m_stJudgeItemPriority.CFSLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CBSLuminance )
		m_pDataManager->m_stJudgeItemPriority.CBSLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TFCLuminance )
		m_pDataManager->m_stJudgeItemPriority.TFCLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TBCLuminance )
		m_pDataManager->m_stJudgeItemPriority.TBCLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TFSLuminance )
		m_pDataManager->m_stJudgeItemPriority.TFSLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TBSLuminance )
		m_pDataManager->m_stJudgeItemPriority.TBSLuminance = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.CfProtection )
		m_pDataManager->m_stJudgeItemPriority.CfProtection = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.TftProtection )
		m_pDataManager->m_stJudgeItemPriority.TftProtection = ++nCnt;

	//2019.03.15 by ryu 
	if( !m_pDataManager->m_stJudgeItemPriority.Bgradecheck )
		m_pDataManager->m_stJudgeItemPriority.Bgradecheck = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.Cgradecheck )
		m_pDataManager->m_stJudgeItemPriority.Cgradecheck = ++nCnt;

	if( !m_pDataManager->m_stJudgeItemPriority.PanelCodeTest )
		m_pDataManager->m_stJudgeItemPriority.PanelCodeTest = ++nCnt;

	//
	if( m_nEqpModel == EQP_TYPE_ABI )
	{
		if( !m_pDataManager->m_stJudgeItemPriority.mura )
			m_pDataManager->m_stJudgeItemPriority.mura = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CFpolSubstance )
			m_pDataManager->m_stJudgeItemPriority.CFpolSubstance = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TFTpolSubstance )
			m_pDataManager->m_stJudgeItemPriority.TFTpolSubstance = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfPolBubble )
			m_pDataManager->m_stJudgeItemPriority.CfPolBubble = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfPolScrach )
			m_pDataManager->m_stJudgeItemPriority.CfPolScrach = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftPolBubble )
			m_pDataManager->m_stJudgeItemPriority.TftPolBubble = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftPolScrach )
			m_pDataManager->m_stJudgeItemPriority.TftPolScrach = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.PolMinor )
			m_pDataManager->m_stJudgeItemPriority.PolMinor = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.Contact )
			m_pDataManager->m_stJudgeItemPriority.Contact = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.PolMiss )
			m_pDataManager->m_stJudgeItemPriority.PolMiss = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TDILuminance )
			m_pDataManager->m_stJudgeItemPriority.TDILuminance = ++nCnt;

		//2020.02.28 add by mby
		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation1 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation1 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation2 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation2 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation3 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation3 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation4 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation4 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation5 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation5 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation6 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation6 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation7 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation7 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation8 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation8 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation9 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation9 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation10 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation10 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation11 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation11 = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.BubbleStation12 )
			m_pDataManager->m_stJudgeItemPriority.BubbleStation12 = ++nCnt;
	}
	else if( m_nEqpModel == EQP_TYPE_AVL )
	{

		if( !m_pDataManager->m_stJudgeItemPriority.CfScrach )
			m_pDataManager->m_stJudgeItemPriority.CfScrach = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftScrach )
			m_pDataManager->m_stJudgeItemPriority.TftScrach = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfWhiteBlur )
			m_pDataManager->m_stJudgeItemPriority.CfWhiteBlur = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.ItoPeel )
			m_pDataManager->m_stJudgeItemPriority.ItoPeel = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfEdgeilegal )
			m_pDataManager->m_stJudgeItemPriority.CfEdgeilegal = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfDimpleLine )
			m_pDataManager->m_stJudgeItemPriority.CfDimpleLine = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftWhiteBlur )
			m_pDataManager->m_stJudgeItemPriority.TftWhiteBlur = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftEdgeilegal )
			m_pDataManager->m_stJudgeItemPriority.TftEdgeilegal = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftDimpleLine )
			m_pDataManager->m_stJudgeItemPriority.TftDimpleLine = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfPimplePoint )
			m_pDataManager->m_stJudgeItemPriority.CfPimplePoint = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftPimplePoint )
			m_pDataManager->m_stJudgeItemPriority.TftPimplePoint = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfPimpleLine )
			m_pDataManager->m_stJudgeItemPriority.CfPimpleLine = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.TftPimpleLine )
			m_pDataManager->m_stJudgeItemPriority.TftPimpleLine = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfPimpleCircle )
			m_pDataManager->m_stJudgeItemPriority.CfPimpleCircle = ++nCnt;

		if( !m_pDataManager->m_stJudgeItemPriority.CfWavePattern )
			m_pDataManager->m_stJudgeItemPriority.CfWavePattern = ++nCnt;
	}
}
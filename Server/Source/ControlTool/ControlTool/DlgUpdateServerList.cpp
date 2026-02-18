// UpdateServerList.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "DlgUpdateServerList.h"

// CUpdateServerList 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgUpdateServerList, CDialog)

CDlgUpdateServerList::CDlgUpdateServerList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUpdateServerList::IDD, pParent)
{
	m_hThread	= NULL;
	m_ThreadID	= 0;
	m_bRunning	= TRUE;

	m_i32UpdatePercent	= NULL;

	m_i32ServerCount	= 0;

	_tcscpy_s( m_strServerServiceName[0], _T("PBClanServerService") );
	_tcscpy_s( m_strServerServiceName[1], _T("PBControlServerService") ); 
	_tcscpy_s( m_strServerServiceName[2], _T("PBGameServerService") ); 
	_tcscpy_s( m_strServerServiceName[3], _T("PBMessengerServerService") ); 
	_tcscpy_s( m_strServerServiceName[4], _T("PBRelayServerService") );
	_tcscpy_s( m_strServerServiceName[5], _T("PBTransServerService") ); 
	_tcscpy_s( m_strServerServiceName[6], _T("SIAServerService") );
	_tcscpy_s( m_strServerServiceName[7], _T("ZLogServerService") ); 
}

CDlgUpdateServerList::~CDlgUpdateServerList()
{
	if(m_hThread != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}

	if(m_i32UpdatePercent != NULL)
		delete[] m_i32UpdatePercent;
}

void CDlgUpdateServerList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UPDATE_RUN, m_strUpdateRun);
	DDX_Control(pDX, ID_UPDATE_START, m_btStart);
	DDX_Control(pDX, IDOK, m_btFinish);
	DDX_Control(pDX, IDCANCLE, m_btCancel);
	DDX_Control(pDX, IDC_LIST_SERVERLIST2, m_ctrlServerList);
	DDX_Control(pDX, IDC_BUTTON_ALLCHECK, m_btAllCheck);
	DDX_Control(pDX, IDC_BUTTON_UNCHECK, m_btUncheck);
}


BEGIN_MESSAGE_MAP(CDlgUpdateServerList, CDialog)
	ON_BN_CLICKED(IDCANCLE, &CDlgUpdateServerList::OnBnClickedCancle)
	ON_BN_CLICKED(ID_UPDATE_START, &CDlgUpdateServerList::OnBnClickedUpdateStart)
	ON_BN_CLICKED(IDC_BUTTON_ALLCHECK, &CDlgUpdateServerList::OnBnClickedButtonAllcheck)
	ON_BN_CLICKED(IDC_BUTTON_UNCHECK, &CDlgUpdateServerList::OnBnClickedButtonUncheck)
END_MESSAGE_MAP()


// CUpdateServerList 메시지 처리기입니다.

void CDlgUpdateServerList::OnBnClickedCancle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(0);
}

BOOL CDlgUpdateServerList::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CRect rcList;
	GetWindowRect( &rcList );
	ScreenToClient( &rcList );

	LVCOLUMN col;
	col.mask    = LVCF_TEXT | LVCF_WIDTH;

	col.pszText = _T("Server");
	col.cx		= rcList.Width() * 0.3;
	m_ctrlServerList.InsertColumn( 0, &col);

	col.pszText = _T("PROGRESS");
	col.cx		= rcList.Width() * 0.6;
	m_ctrlServerList.InsertColumn( 1, &col);

	ListView_SetExtendedListViewStyle( m_ctrlServerList.m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_INFOTIP );

	LVITEM lvitem;
	lvitem.mask = LVIF_TEXT;
	lvitem.iSubItem = 0;
	m_ctrlServerList.SetItem(&lvitem);

	for(INT32 i = 0; i != 8; i++)
	{
		m_ctrlServerList.InsertItem( i, m_strServerServiceName[i] );
	}

	m_btFinish.EnableWindow(FALSE);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgUpdateServerList::OnBnClickedButtonAllcheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT32 i32Listcount = m_ctrlServerList.GetItemCount();

	for(INT32 i = 0; i != i32Listcount; i++)
	{
		m_ctrlServerList.SetCheck(i, TRUE);
	}
}


void CDlgUpdateServerList::OnBnClickedButtonUncheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT32 i32Listcount = m_ctrlServerList.GetItemCount();

	for(INT32 i = 0; i != i32Listcount; i++)
	{
		m_ctrlServerList.SetCheck(i, FALSE);
	}
}



void CDlgUpdateServerList::OnBnClickedUpdateStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_btCancel.EnableWindow(FALSE);
	m_btStart.EnableWindow(FALSE);
	m_btAllCheck.EnableWindow(FALSE);
	m_btUncheck.EnableWindow(FALSE);
	
	INT32 i32Listcount = m_ctrlServerList.GetItemCount();
	BOOL bCheck = FALSE;

	CRect ItemRect;
	CRect ProgressRect;
	CRect DlgRect;
	CRect ListRct;
	m_ctrlServerList.GetSubItemRect(0, 1, LVIR_BOUNDS, ItemRect);
	GetClientRect(&DlgRect);
	m_ctrlServerList.GetClientRect(&ListRct);

	INT32 UpperVal = DlgRect.bottom - ListRct.bottom;
	UpperVal = UpperVal / 2 + 2;
	INT32 SideVal = DlgRect.right - ListRct.right;
	SideVal = SideVal / 2 - 1;

	INT32 CorrectionVal = ItemRect.bottom -  ItemRect.top; 

	ProgressRect.left	= ItemRect.left + SideVal;
	ProgressRect.right	= ItemRect.right + SideVal;
	ProgressRect.top	= ItemRect.top + UpperVal;
	ProgressRect.bottom	= ItemRect.bottom + UpperVal;

	INT32 i32Val1 = ProgressRect.top;
	INT32 i32Val2 = ProgressRect.bottom;

	for(INT32 i = 0; i != i32Listcount; i++)
	{
		bCheck = m_ctrlServerList.GetCheck(i);

		if(bCheck)
		{
			ProgressRect.top	= i32Val1 + (CorrectionVal * i);
			ProgressRect.bottom	= i32Val2 + (CorrectionVal * i);

			m_pProg[m_i32ServerCount].Create(PBS_SMOOTH | WS_CHILD | WS_VISIBLE, ProgressRect, this, 1);
			m_pProg[m_i32ServerCount].SetRange(0, 100);
			m_pProg[m_i32ServerCount].SetPos(0);

			m_i32ServerCount++;
		}
	}

	//프로그레스바 갯수 만큼 생성.
	m_i32UpdatePercent = new INT32[m_i32ServerCount];

	m_hThread = (HANDLE)CreateThread(0, 0, UpdateThread, (LPVOID)this, 0, &m_ThreadID);
	if ( INVALID_HANDLE_VALUE == m_hThread )
	{
		return;
	}
}

DWORD WINAPI CDlgUpdateServerList::UpdateThread(LPVOID lpParam)
{
	INT32	i32Cur = 0;
	INT32	i32Count = 0;

	INT32   i32temp = 0;
	INT32	i32temp2 = 0;

	INT32	i32CurProgressInc_old = 0;
	INT32	i32TotalProgressInc_old = 0;

	BOOL	bRunning = TRUE;

	CDlgUpdateServerList* pUpdate =  (CDlgUpdateServerList*)lpParam;

	char cDispRunning[5][13] = 
	{
		{"Updating"}, 
		{"Updating."},
		{"Updating.."},
		{"Updating..."},
		{"Updating...."}
	};

	while(bRunning)
	{
		i32temp2++;
		if(i32temp2 == 10)
		{
			i32Cur++;
			i32temp++;

			if(5 == i32temp)
				i32temp = 0;

			i32temp2 = 0;
		}

		for(INT32 i = 0; i != pUpdate->m_i32ServerCount; i++)
		{
			pUpdate->m_pProg[i].SetPos(pUpdate->m_i32UpdatePercent[i]);
			pUpdate->m_i32UpdatePercent[i] = i32Cur;
		}

		Sleep(5);

		if(i32Cur == 100)
		{
			pUpdate->m_btFinish.EnableWindow(TRUE);
			break;
		}
	}

	return 0;
}

// FileTreeDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "GlobalVar.h"
#include "PanelTreeScript.h"
#include "SingleFile.h"
#include "i3PackTool.h"
#include "ScriptCmd.h"

//#include "shfolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CPanelTreeScript, CDialog)
CPanelTreeScript::CPanelTreeScript(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelTreeScript::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPanelTreeScript)
	m_strText = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPanelTreeScript::~CPanelTreeScript()
{
}

void CPanelTreeScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanelTreeScript)
	DDX_Control(pDX, IDD_PANEL_TREE1, m_FileTree);
	DDX_Text(pDX, IDC_SCRIPTEDIT1, m_strText);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT1, m_EdWorkDir);
	DDX_Control(pDX, IDC_EDIT2, m_EdLiveDir);
	DDX_Control(pDX, IDC_EDIT3, m_EdScriptDir);
	DDX_Control(pDX, IDC_RADIO1, m_SelectScript);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressCtrl);
	DDX_Control(pDX, IDC_SCRIPTEDIT3, m_PackLogEditbox);
	DDX_Control(pDX, IDC_LIST1, m_LFileList);
}

BEGIN_MESSAGE_MAP(CPanelTreeScript, CDialog)
	//{{AFX_MSG_MAP(CPanelTreeScript)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GETLIST, OnSetPack)
	ON_NOTIFY(TVN_SELCHANGED, IDD_PANEL_TREE1, OnStateChanged)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDD_PANEL_TREE1, OnStateChanged)
	ON_BN_CLICKED(IDC_GETLIST2, OnGetlist2)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CPanelTreeScript::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPanelTreeScript::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPanelTreeScript::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &CPanelTreeScript::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_RADIO1, &CPanelTreeScript::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CPanelTreeScript::OnBnClickedRadio2)
	//ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CPanelTreeScript::OnDeltaposSpin1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanelTreeScript message handlers

BOOL CPanelTreeScript::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	{
		m_FileTree.AddHidFolder(CSIDL_BITBUCKET);
		m_FileTree.AddHidFolder(CSIDL_CONTROLS);
		m_FileTree.AddHidFolder(CSIDL_WINDOWS);
		m_FileTree.DisplayTree();
		m_SelectScript.SetCheck(1);

		m_EdWorkDir.SetWindowText( g_pOption->m_szWorkPath);
		m_EdLiveDir.SetWindowText( g_pOption->m_szLivePath);
		m_EdScriptDir.SetWindowText( g_pOption->m_szScriptPath);

		i3TDK::RegisterUpdate( m_hWnd, i3PackNode::static_meta(), I3_TDK_UPDATE_ALL);
		i3TDK::RegisterUpdate( m_hWnd, SingleFile::static_meta(), I3_TDK_UPDATE_ALL);

		m_progressCtrl.SetRange(0,100);
	}
  	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*virtual*/ BOOL CPanelTreeScript::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)  // 가상키가 눌렸을 경우...
	{
		switch (pMsg->wParam)  // 어떤 가상키가 눌렸는지 체크...
		{
		case VK_RETURN :  // 엔터키 처리...
			CEdit* pFocusEdit = (CEdit*)GetFocus();  // 현재 포커스를 얻어옴
			CEdit* pEdit1 = (CEdit*)GetDlgItem(IDC_SCRIPTEDIT1);
			if (pFocusEdit == pEdit1)  // 현재 포커스가 첫번째 에디트 박스에 있는 경우
			{
				pEdit1->GetWindowText(m_strText);
				m_FileTree.SetFindFile(m_strText.GetBuffer());
				m_FileTree.SetScriptRefreshFolder();
			}
			break;
		}
		
	}

	return CDialog::PreTranslateMessage(pMsg);
}
void CPanelTreeScript::OnSysCommand(UINT nID, LPARAM lParam)
{
}

void CPanelTreeScript::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
	//	int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
//		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPanelTreeScript::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPanelTreeScript::OnRefresh() 
{
	// TODO: Add your control notification handler code here
	m_FileTree.RefreshTree();
}

void CPanelTreeScript::OnStateChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	//m_strText = m_FileTree.GetInfo(m_FileTree.GetSelectedItem());
	UpdateData( FALSE );
	*pResult = 0;
}


void CPanelTreeScript::OnSetPack() 
{
	// TODO: Add your control notification handler code here
	CString* X = NULL;
	int N=m_FileTree.GetSelectedFiles(X);
	CString S;
	INT32 nMinPos, nMaxPos, nCurPos, num = 0;
	nCurPos, nMaxPos, nMinPos;			// 쓰이지 않는 변수의 경고제거 목적..

	if (N) {
		S="";
		for (int i=0; i<N; i++)
		{
			S.Insert(0, X[i].Mid(X[i].ReverseFind('\\')+1, X[i].GetLength())+"Pack Done \r\n");
//			S+=X[i].Mid(X[i].ReverseFind('\\')+1, X[i].GetLength())+"Pack Done \r\n";
			if(m_SelectScript.GetCheck() == 0)
			{
				theApp.SetBatCmd(X[i].GetBuffer());
			}				
			else
				theApp.SetScriptCmd(X[i].GetBuffer());
		

			num = int(  (((float)i+1)/((float)N+1)) * 100.f  );
			m_progressCtrl.SetPos(num);

			m_PackLogEditbox.SetWindowText(S);
			m_PackLogEditbox.SetFocus();
			m_PackLogEditbox.SetSel(0, -1, TRUE);
			UpdateData(false);
		}
		m_progressCtrl.SetPos(100);
		S.Insert(0, "***      Select Pack Completed      ***\r\n");
		m_PackLogEditbox.SetWindowText(S);
		m_PackLogEditbox.SetFocus();
		m_PackLogEditbox.SetSel(0, -1, TRUE);
		UpdateData(false);

		bool istest = false;
		SHELLEXECUTEINFO si;
		CString	Path;

		Path.Format("/command:commit /path:%s /closeonend:0", g_pOption->m_szLivePath);
		si.cbSize = sizeof(SHELLEXECUTEINFO);
		si.lpVerb = __TEXT("open");						//mode : print, write
		si.lpFile = __TEXT("TortoiseProc.exe");           //실행파일
		si.lpParameters = Path;
		si.lpDirectory =  NULL;	//실핼파일 디렉토리.
		si.nShow = SW_SHOW;								//SW_HIDE
		si.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_DDEWAIT;

		istest = ShellExecuteEx(&si) != FALSE;
	}
	else
	{

	}
	
	delete[] X;
}

void CPanelTreeScript::OnGetlist() 
{
	// TODO: Add your control notification handler code here
	CString* X = NULL;
	int N=m_FileTree.GetSelectedFiles(X);

	m_LFileList.ResetContent();
	if (N) {
		for (int i=0; i<N; i++)
		{
			m_LFileList.AddString(X[i].Right( X[i].GetLength() - X[i].ReverseFind('\\')-1));
		}
	}
	UpdateData(false);
	delete[] X;
}

void CPanelTreeScript::OnGetlist2() 
{
	// TODO: Add your control notification handler code here
	CString* X = NULL;
	int N=m_FileTree.GetSelectedFolders(X);
	CString S;
	if (N) {
		S="";
		for (int i=0; i<N; i++)
		{
			S+=X[i]+"\r\n";
		}
	}
	else S="None";
	UpdateData(false);
	delete[] X;	
}

void CPanelTreeScript::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Workding Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szWorkPath) == TRUE)
	{
		m_EdWorkDir.SetWindowText( dlg.GetSelectedFolderPath());
		m_EdWorkDir.GetWindowText( g_pOption->m_szWorkPath, sizeof(g_pOption->m_szWorkPath) - 1);
		i3ResourceFile::SetWorkingFolder( g_pOption->m_szWorkPath);
		SetCurrentDirectory( g_pOption->m_szWorkPath);

		CString strtemp = g_pOption->m_szWorkPath;
		strtemp = strtemp.Mid(0, strtemp.GetLength()-2) + "Live";
		strcpy(g_pOption->m_szLivePath, (const char*)strtemp.GetBuffer());
		m_EdLiveDir.SetWindowText(strtemp);

		//History 에 저장 경로를 저장합니다.
// 		for(INT32 i = 0; i < MAX_WORKHISTORY; i++)
// 		{
// 			if(g_pOption->m_szWorkHistory[i][0] == 0)
// 			{
// 				strcpy(g_pOption->m_szWorkHistory[i], g_pOption->m_szWorkPath);
// 				break;
// 			}
// 			else if( i == MAX_WORKHISTORY -1)
// 			{
// 				strcpy(g_pOption->m_szWorkHistory[0], g_pOption->m_szWorkHistory[1]);
// 				strcpy(g_pOption->m_szWorkHistory[1], g_pOption->m_szWorkHistory[2]);
// 				strcpy(g_pOption->m_szWorkHistory[2], g_pOption->m_szWorkPath);
// 			}
// 		}

	//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		//pFrame->m_PanelTreeLive.MakeTree();
	}

}

void CPanelTreeScript::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Live Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szLivePath) == TRUE)
	{
		m_EdLiveDir.SetWindowText( dlg.GetSelectedFolderPath());
	}
	m_EdLiveDir.GetWindowText( g_pOption->m_szLivePath, sizeof(g_pOption->m_szLivePath) - 1);

}

void CPanelTreeScript::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Script Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szScriptPath) == TRUE)
	{
		m_EdScriptDir.SetWindowText( dlg.GetSelectedFolderPath());
	}
	m_EdScriptDir.GetWindowText( g_pOption->m_szScriptPath, sizeof(g_pOption->m_szScriptPath) - 1);
	m_FileTree.SetScriptRefreshFolder();
}



UINT64 CPanelTreeScript::GetCommandLine()
{
	UINT64				MacAddress;
	i3Net::GetMACAddress(&MacAddress, 1);
	UINT32 ui32Key1 = static_cast<UINT32>((MacAddress >> 32)&0xFFFFFFFF);
	UINT32 ui32Key2 = static_cast<UINT32>(MacAddress&0xFFFFFFFF);

	UINT8	ui8Encrypt[8];
	ui8Encrypt[0] = (UINT8)((((ui32Key1>>24)	& 0xF0))	| ((ui32Key2>>24)	& 0xF0)>>4);
	ui8Encrypt[1] = (UINT8)((((ui32Key1>>8)		& 0x0F)<<4)	| ((ui32Key2>>16)	& 0x0F));
	ui8Encrypt[2] = (UINT8)((((ui32Key1>>0)		& 0xF0))	| ((ui32Key2>>8)	& 0x0F));
	ui8Encrypt[3] = (UINT8)((((ui32Key1>>16)	& 0x0F)<<4)	| ((ui32Key2>>0)	& 0x0F));
	ui8Encrypt[4] = (UINT8)((((ui32Key1>>8)		& 0xF0))	| ((ui32Key2>>24)	& 0x0F));
	ui8Encrypt[5] = (UINT8)((((ui32Key1>>24)	& 0x0F)<<4)	| ((ui32Key2>>16)	& 0xF0)>>4);
	ui8Encrypt[6] = (UINT8)((((ui32Key1>>0)		& 0x0F)<<4)	| ((ui32Key2>>0)	& 0xF0)>>4);
	ui8Encrypt[7] = (UINT8)((((ui32Key1>>16)	& 0xF0))	| ((ui32Key2>>8)	& 0xF0)>>4);

	BitRotateEncript( ui8Encrypt, sizeof( UINT64 ), 3 );

	memcpy( &MacAddress, ui8Encrypt, sizeof(UINT64) );
	return MacAddress;
}

void CPanelTreeScript::OnBnClickedButton5()
{
	bool istest = false;
	SHELLEXECUTEINFO si;
	CString strtemp;
	strtemp.Format("%I64d", GetCommandLine());	

	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.lpVerb = __TEXT("open");						//mode : print, write
	si.lpFile = __TEXT("PointBlank.exe");           //실행파일
	si.lpParameters = strtemp.GetBuffer();				//실행파일 인자값.
	si.lpDirectory =  g_pOption->m_szLivePath;	//실핼파일 디렉토리.
	si.nShow = SW_SHOW;								//SW_HIDE
	si.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_DDEWAIT;
	
	istest = ShellExecuteEx(&si) != FALSE;
	if(istest == false)
	{
		si.lpFile = __TEXT("PBlackout.exe");
		istest = ShellExecuteEx(&si) != FALSE;
		if(istest == false)
		{
			MessageBox("실행실패");
		}
	}
}

void CPanelTreeScript::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT32 iselitem = m_SelectScript.GetCheck(); 
	m_FileTree.SetScriptCheck(iselitem != FALSE);
	m_FileTree.SetScriptRefreshFolder();
}

void CPanelTreeScript::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT32 iselitem = m_SelectScript.GetCheck(); 
	m_FileTree.SetScriptCheck(iselitem != FALSE);
	m_FileTree.SetScriptRefreshFolder();
}

void CPanelTreeScript::GetWindowsize(RECT* prect, const RECT* pRect1, const RECT* pRect2, INT32 value)
{
	CPoint point1;
	point1.x = pRect2->right - pRect2->left;
	point1.y = pRect2->bottom - pRect2->top;


	//prect->left -= pRect2->left-pRect1->left;
	prect->left = pRect1->right - pRect1->left - value;
	prect->right = prect->left+point1.x;
	prect->top = pRect2->top - pRect1->top;
	prect->bottom = prect->top + point1.y;		
}

void CPanelTreeScript::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize( nType,  cx,  cy);
	RECT rect, dlgrect, itemrect, treerect;
	i3::vector<i3::pair< CWnd*, INT32>> m_pControl1;
	i3::vector<i3::pair< CWnd*, INT32>> m_pControl2;
	i3::vector<i3::pair< CWnd*, INT32>>::iterator it;

	GetWindowRect(&dlgrect);	

	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_BUTTON1), 30));
	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_BUTTON2), 30));
	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_BUTTON3), 30));
	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_BUTTON5), 60));
	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_GETLIST), 70));
	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_SPIN1), 50));
	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_RADIO1), 90));
	m_pControl1.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_RADIO2), 40));

	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_EDIT1), 50));
	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_EDIT2), 65));
	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_EDIT3), 35));
	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_SCRIPTEDIT1), 95));
	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_LIST1), 75));
	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDD_PANEL_TREE1), 10));
	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_PROGRESS1), 10));
	m_pControl2.push_back(i3::pair< CWnd*, INT32>((CWnd*)GetDlgItem(IDC_SCRIPTEDIT3), 10));


	it = m_pControl2.begin();
	while( it != m_pControl2.end())
	{
		if(it->first->GetSafeHwnd())
		{
			it->first->GetWindowRect(&treerect);
			it->first->GetParent()->ScreenToClient(&treerect);

			if(it+1 == m_pControl2.end())
			{
				treerect.bottom = dlgrect.bottom- dlgrect.top - 10;
			}
			treerect.right = dlgrect.right- dlgrect.left -it->second;
			it->first->MoveWindow(&treerect);
			
		}
		it++;
	}


	it = m_pControl1.begin();
	while( it != m_pControl1.end())
	{
		if( it->first->GetSafeHwnd())
		{
			it->first->GetWindowRect(&itemrect);
			GetWindowsize(&rect, &dlgrect, &itemrect, it->second);
			it->first->MoveWindow(&rect);
		}
		it++;
	}
	m_pControl1.clear();
	m_pControl2.clear();
}
//void CPanelTreeScript::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	INT32 ichange = 0;
//	if( pNMUpDown->iDelta < 0)
//	{
//		ichange = +1;
//	}
//	else
//	{
//		ichange = -1;
//	}
//
//	//History에 경로 변경하기
//	for(INT32 i = 0; i < MAX_WORKHISTORY; i++)
//	{
////		if( g_pOption->m_szWorkHistory[i])
////			bool strcheck = false;
//		CString strtemp = g_pOption->m_szWorkPath;
//
//		//strcheck = strcmp(g_pOption->m_szWorkPath, g_pOption->m_szWorkHistory[i]);
//
//		if( (strtemp == g_pOption->m_szWorkHistory[i]))
//		{
//			INT32 idx = 0;
//			bool isselect = true;
//			while( isselect )
//			{
//				if(idx == 0)
//					idx = i+ichange;
//				else
//					idx = idx + ichange;
//
//				if( idx == MAX_WORKHISTORY)
//				{
//					idx = 0;
//				}
//				else if( idx < 0)
//				{
//					idx = MAX_WORKHISTORY-1;
//				}
//
//				if(g_pOption->m_szWorkHistory[idx][0] != 0)
//				{
//					break;
//				}
//			}
//
//			strcpy(g_pOption->m_szWorkPath, g_pOption->m_szWorkHistory[idx]);
//			break;
//		}
//	}
//
//	i3ResourceFile::SetWorkingFolder( g_pOption->m_szWorkPath);
//	SetCurrentDirectory( g_pOption->m_szWorkPath);
//
//	CString strtemp = g_pOption->m_szWorkPath;
//	strtemp = strtemp.Mid(0, strtemp.GetLength()-2) + "Live";
//	strcpy(g_pOption->m_szLivePath, (const char*)strtemp.GetBuffer());
//	m_EdWorkDir.SetWindowText( g_pOption->m_szWorkPath);
//	m_EdLiveDir.SetWindowText( g_pOption->m_szLivePath);
//
//	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	//pFrame->m_PanelTreeLive.MakeTree();
//
//	*pResult = 0;
//}

void CPanelTreeScript::SetPath(const char* chQAPath, const char* chLivePath)
{
	i3::generic_string_copy(g_pOption->m_szWorkPath, chQAPath);
	i3::generic_string_copy(g_pOption->m_szLivePath, chLivePath);

	i3ResourceFile::SetWorkingFolder(g_pOption->m_szWorkPath);
	SetCurrentDirectory(g_pOption->m_szWorkPath);

	m_EdWorkDir.SetWindowText(g_pOption->m_szWorkPath);
	m_EdLiveDir.SetWindowText(g_pOption->m_szLivePath);
}
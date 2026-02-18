// i3MemReportDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3MemReport.h"
#include "i3MemReportDlg.h"
#include ".\i3memreportdlg.h"
#include "i3MemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3MemReportDlg 대화 상자



Ci3MemReportDlg::Ci3MemReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3MemReportDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nCurSortMode = 0;
	m_bAscendOrder = true;
}

void Ci3MemReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	DDX_Control(pDX, IDC_ED_TOTAL, m_TotalSizeCtrl);
}

BEGIN_MESSAGE_MAP(Ci3MemReportDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SAVE_BUTTON, OnBnClickedSaveButton)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickList)
END_MESSAGE_MAP()


// Ci3MemReportDlg 메시지 처리기

BOOL Ci3MemReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	{
		m_ListCtrl.InsertColumn( 0, "File", LVCFMT_LEFT, 270);
		m_ListCtrl.InsertColumn( 1, "Line", LVCFMT_LEFT, 70);
		m_ListCtrl.InsertColumn( 2, "Count", LVCFMT_LEFT, 80);
		m_ListCtrl.InsertColumn( 3, "Size", LVCFMT_LEFT, 80);
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3MemReportDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR Ci3MemReportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3MemReportDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ListCtrl.m_hWnd))
	{
		m_ListCtrl.SetWindowPos( NULL, 3, 3, cx - 130, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}
}

void Ci3MemReportDlg::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];
	char szTempPath[MAX_PATH], * pPath;
	char szName[MAX_PATH];

	MemInfo::DeleteAllInfo();

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);
		i3String::Copy( szTempPath, TempName);

		pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
		*pPath = 0;

		Parse( TempName);
	}

	DragFinish( hDropInfo);

	UpdateInfos();

	CDialog::OnDropFiles(hDropInfo);
}

void Ci3MemReportDlg::Parse( const char * pszPath)
{
	FILE * pFP;
	char szLine[2048], szToken[256];
	MemInfo info;
	i3LineLexer lex;
	char sep;

	pFP = fopen( pszPath, "rt");
	if( pFP == NULL)
	{
		I3FATAL( "Could not open %s file.", pszPath);
		return;
	}

	while(( !ferror(pFP) && !feof( pFP)))
	{
		fgets( szLine, sizeof(szLine) -1, pFP);

		if( strlen( szLine) > 0)
		{
			lex.SetMasterString( szLine);

			// Address
			lex.GetNextToken( szToken, &sep, "\t\r\n");

			// File
			lex.GetNextToken( info.m_szPath, &sep, "\t\r\n");

			lex.GetNextToken( szToken, &sep, "\t\r\n");
			info.m_Line = atoi( szToken);

			lex.GetNextToken( szToken, &sep, "\t\r\n");
			info.m_Size = atoi( szToken);

			MemInfo::AddInfo( &info);
		}
	}

	fclose( pFP);
}


void Ci3MemReportDlg::UpdateInfos(bool bAscend)
{
	INT32 i, idx;
	MemInfo * pInfo;
	char conv[256];
	UINT32 total = 0;

	m_ListCtrl.DeleteAllItems();

	if( bAscend )
	{
		for( i = 0; i < MemInfo::GetCount(); i++)
		{
			pInfo = MemInfo::GetInfo( i);

			total += pInfo->m_Size;

			idx = m_ListCtrl.InsertItem( i, pInfo->m_szPath, 0);

			// Line
			sprintf( conv, "%d", pInfo->m_Line);
			m_ListCtrl.SetItemText( idx, 1, conv);

			// Count
			sprintf( conv, "%d", pInfo->m_Count);
			m_ListCtrl.SetItemText( idx, 2, conv);

			// Size
			i3String::ValueToStr( pInfo->m_Size, conv, sizeof(conv) - 1);
			m_ListCtrl.SetItemText( idx, 3, conv);
		}
	}
	else
	{
		INT32 iii = 0;
		for( i = MemInfo::GetCount() -1; i >= 0 ; i--)
		{
			pInfo = MemInfo::GetInfo( i);

			total += pInfo->m_Size;

			idx = m_ListCtrl.InsertItem( iii, pInfo->m_szPath, 0);

			// Line
			sprintf( conv, "%d", pInfo->m_Line);
			m_ListCtrl.SetItemText( idx, 1, conv);

			// Count
			sprintf( conv, "%d", pInfo->m_Count);
			m_ListCtrl.SetItemText( idx, 2, conv);

			// Size
			i3String::ValueToStr( pInfo->m_Size, conv, sizeof(conv) - 1);
			m_ListCtrl.SetItemText( idx, 3, conv);

			iii++;
		}
	}

	i3String::ValueToStr( total, conv, sizeof(conv) - 1);
	m_TotalSizeCtrl.SetWindowText( conv);
}

void Ci3MemReportDlg::OnDestroy()
{
	CDialog::OnDestroy();

	MemInfo::DeleteAllInfo();
}

void Ci3MemReportDlg::OnBnClickedSaveButton()
{
	// TODO: Add your control notification handler code here
	i3FileStream	DumpFile; 

	DumpFile.Create("c:\\MemDump_OPT.i3m", STREAM_WRITE);

	char OutText[1024];
	MemInfo * pInfo;
	for(INT32 i = 0; i < MemInfo::GetCount(); i++)
	{
		pInfo = MemInfo::GetInfo( i);
		sprintf(OutText, "%s\t%d\t%d\t%d\r", pInfo->m_szPath, pInfo->m_Line, pInfo->m_Count, pInfo->m_Size); 
		DumpFile.WriteLine( OutText );
	}
	DumpFile.Close(); 
}

void Ci3MemReportDlg::OnHdnItemclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	INT32 item = phdr->iItem;
	bool bChanged = false;
	if( m_nCurSortMode != item)
	{
		switch( item)
		{
		case 0:
			{
				MemInfo::SortByName();	
				bChanged = true;
			}break;
		case 2:
			{
				MemInfo::SortByCount();	
				bChanged = true;
			}break;
		case 3: 
			{
				MemInfo::SortBySize();	
				bChanged = true;
			}break;
		}

		if( bChanged)
		{
			UpdateInfos( m_bAscendOrder);

			m_nCurSortMode = item;
		}
	}
	else
	{
		if( item != 1)
		{
			m_bAscendOrder = !m_bAscendOrder;
			UpdateInfos( m_bAscendOrder);
		}
	}

	*pResult = 0;
}

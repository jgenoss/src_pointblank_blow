// i3AnimationKeyDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3AnimationKey.h"
#include "i3AnimationKeyDlg.h"
#include ".\i3animationkeydlg.h"
#include "SetChannelDialog.h"
#include "SetScaleDialog.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/ftoa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnFileOptimize();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//	ON_COMMAND(ID_FILE_OPTIMIZE, OnFileOptimize)
END_MESSAGE_MAP()


// Ci3AnimationKeyDlg 대화 상자



Ci3AnimationKeyDlg::Ci3AnimationKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3AnimationKeyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pAnim = NULL;
	m_szFileName[0] = 0;
	m_pCurSeq = NULL;
}

Ci3AnimationKeyDlg::~Ci3AnimationKeyDlg()
{
	I3_SAFE_RELEASE(m_pAnim);			// 주 릭 원인이었던듯 하다...추가해서 해결시도.(2012.09.10.수빈)
}


void Ci3AnimationKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACKLIST, m_TrackListCtrl);
	DDX_Control(pDX, IDC_KEYLIST, m_KeyListCtrl);
}

BEGIN_MESSAGE_MAP(Ci3AnimationKeyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_NOTIFY(NM_CLICK, IDC_TRACKLIST, OnNMClickTracklist)
	ON_NOTIFY(NM_RCLICK, IDC_TRACKLIST, OnNMRclickTracklist)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_NOTIFY(NM_RCLICK, IDC_KEYLIST, OnNMRclickKeylist)
	ON_COMMAND(ID_FILE_OPTIMIZE, OnFileOptimize)
END_MESSAGE_MAP()


// Ci3AnimationKeyDlg 메시지 처리기

BOOL Ci3AnimationKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	{
		m_TrackListCtrl.InsertColumn( 0, "Bone", LVCFMT_LEFT, 160);
		m_TrackListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_KeyListCtrl.InsertColumn( 0, "Time", LVCFMT_LEFT, 80);

		m_KeyListCtrl.InsertColumn( 1, "Tx", LVCFMT_LEFT, 60);
		m_KeyListCtrl.InsertColumn( 2, "Ty", LVCFMT_LEFT, 60);
		m_KeyListCtrl.InsertColumn( 3, "Tz", LVCFMT_LEFT, 60);

		m_KeyListCtrl.InsertColumn( 4, "Qx", LVCFMT_LEFT, 60);
		m_KeyListCtrl.InsertColumn( 5, "Qy", LVCFMT_LEFT, 60);
		m_KeyListCtrl.InsertColumn( 6, "Qz", LVCFMT_LEFT, 60);
		m_KeyListCtrl.InsertColumn( 7, "Qw", LVCFMT_LEFT, 60);

		m_KeyListCtrl.InsertColumn( 8, "Sx", LVCFMT_LEFT, 60);
		m_KeyListCtrl.InsertColumn( 9, "Sy", LVCFMT_LEFT, 60);
		m_KeyListCtrl.InsertColumn( 10, "Sz", LVCFMT_LEFT, 60);
		m_KeyListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	ReplaceControls( -1, -1);
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3AnimationKeyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3AnimationKeyDlg::OnPaint() 
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
HCURSOR Ci3AnimationKeyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3AnimationKeyDlg::ReplaceControls( INT32 cx, INT32 cy)
{
	if( cx == -1)
	{
		CRect rect;

		GetClientRect( &rect);

		cx = rect.Width();
		cy = rect.Height();
	}

	if( ::IsWindow( m_TrackListCtrl.m_hWnd))
	{
		CRect rt;

		m_TrackListCtrl.GetWindowRect( &rt);
		m_TrackListCtrl.SetWindowPos( NULL, 0, 0, rt.Width(), cy - 10, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

		m_KeyListCtrl.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_KeyListCtrl.SetWindowPos( NULL, 0, 0, cx - rt.left - 6, cy - 10, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}
}

void Ci3AnimationKeyDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ReplaceControls( cx, cy);
}

void Ci3AnimationKeyDlg::SetTitleName( const char * pszName)
{
	char szName[512];

	sprintf( szName, "i3AnimationKey - %s", pszName);
	SetWindowText( szName);

	strcpy( m_szFileName, pszName);
}

bool Ci3AnimationKeyDlg::LoadAnim( const char * pszPath)
{
	i3ResourceFile file;
	INT32 i;
	i3PersistantElement * pElm;
	i3Animation * pAnim = NULL;

	if( file.Load( pszPath) == false)
	{
		DebugBox( NULL, "Could not load %s animation file.", pszPath);
		return false;
	}

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pElm = file.GetObject( i);

		if( i3::kind_of<i3Animation*>(pElm)) //->IsTypeOf( i3Animation::static_meta()))
		{
			pAnim = (i3Animation *) pElm;
		}
	}

	if( pAnim == NULL)
	{
		DebugBox( NULL, "%s file is invalid animation file.", pszPath);
		return false;
	}

	SetTitleName( pszPath);

	SetAnimation( pAnim);

	return true;
}

bool Ci3AnimationKeyDlg::SaveAnim( const char * pszPath)
{
	i3ResourceFile file;

	if( m_pAnim == NULL)
		return false;

	file.AddObject( m_pAnim);

	file.Save( pszPath);

	return true;
}

void Ci3AnimationKeyDlg::SetAnimation( i3Animation * pAnim)
{
	if( pAnim != NULL)
	{
		pAnim->AddRef();
	}

	I3_SAFE_RELEASE( m_pAnim);

	m_pAnim = pAnim;
	m_pCurSeq = NULL;

	UpdateAllTracks();
	UpdateAllKeys();
}

void Ci3AnimationKeyDlg::UpdateAllTracks(void)
{
	INT32 i, idx;
	i3TransformSequence * pSeq;

	m_TrackListCtrl.DeleteAllItems();

	if( m_pAnim == NULL)
		return;

	for( i = 0; i < m_pAnim->GetTrackCount(); i++)
	{
		pSeq = m_pAnim->GetTrack( i);

		idx = m_TrackListCtrl.InsertItem( i, m_pAnim->GetTrackBoneName(i));

		m_TrackListCtrl.SetItemData( idx, (DWORD_PTR) pSeq);
	}
}

void Ci3AnimationKeyDlg::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];
//	char szTempPath[MAX_PATH], * pPath;
//	char szName[MAX_PATH];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

//		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);
		i3::extract_fileext(TempName, Ext);
/*
		i3::safe_string_copy( szTempPath, TempName, MAX_PATH );
		pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
		*pPath = 0;
*/
		if( i3::generic_is_iequal( Ext, "I3A") )
		{
			LoadAnim( TempName);
		}
	}

	DragFinish( hDropInfo);
}

INT32 Ci3AnimationKeyDlg::GetSelectedTrack(void)
{
	POSITION pos;

	pos = m_TrackListCtrl.GetFirstSelectedItemPosition();

	if( pos == NULL)
		return -1;

	return m_TrackListCtrl.GetNextSelectedItem( pos);
}

void Ci3AnimationKeyDlg::OnNMClickTracklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idx = GetSelectedTrack();

	if( idx == -1)
	{
		m_pCurSeq = NULL;
	}
	else
	{
		m_pCurSeq = m_pAnim->GetTrack( idx);
	}

	UpdateAllKeys();

	*pResult = 0;
}

void Ci3AnimationKeyDlg::UpdateKey( INT32 idx)
{
	char conv[256];

	// Translation
	if( m_pCurSeq->HasTranslation())
	{
		VEC3D vec;

		m_pCurSeq->GetTranslation( idx, &vec);

		i3::ftoa( i3Vector::GetX( &vec), conv);
		m_KeyListCtrl.SetItemText( idx, 1, conv);

		i3::ftoa( i3Vector::GetY( &vec), conv);
		m_KeyListCtrl.SetItemText( idx, 2, conv);

		i3::ftoa( i3Vector::GetZ( &vec), conv);
		m_KeyListCtrl.SetItemText( idx, 3, conv);
	}
	else
	{
		m_KeyListCtrl.SetItemText( idx, 1, "-");
		m_KeyListCtrl.SetItemText( idx, 2, "-");
		m_KeyListCtrl.SetItemText( idx, 3, "-");
	}

	// Rotation
	if( m_pCurSeq->HasRotation())
	{
		QUATERNION quat;

		m_pCurSeq->GetRotation( idx, &quat);

		i3::ftoa( i3Quat::GetA( &quat), conv);
		m_KeyListCtrl.SetItemText( idx ,4, conv);

		i3::ftoa( i3Quat::GetB( &quat), conv);
		m_KeyListCtrl.SetItemText( idx ,5, conv);

		i3::ftoa( i3Quat::GetC( &quat), conv);
		m_KeyListCtrl.SetItemText( idx ,6, conv);

		i3::ftoa( i3Quat::GetD( &quat), conv);
		m_KeyListCtrl.SetItemText( idx ,7, conv);
	}
	else
	{
		m_KeyListCtrl.SetItemText( idx, 4, "-");
		m_KeyListCtrl.SetItemText( idx, 5, "-");
		m_KeyListCtrl.SetItemText( idx, 6, "-");
		m_KeyListCtrl.SetItemText( idx, 7, "-");
	}

	// Scale
	if( m_pCurSeq->HasScale())
	{
		VEC3D scale;

		m_pCurSeq->GetScale( idx, &scale);

		i3::ftoa( i3Vector::GetX( &scale), conv);
		m_KeyListCtrl.SetItemText( idx, 8, conv);

		i3::ftoa( i3Vector::GetY( &scale), conv);
		m_KeyListCtrl.SetItemText( idx, 9, conv);

		i3::ftoa( i3Vector::GetZ( &scale), conv);
		m_KeyListCtrl.SetItemText( idx, 10, conv);
	}
	else
	{
		m_KeyListCtrl.SetItemText( idx, 8, "-");
		m_KeyListCtrl.SetItemText( idx, 9, "-");
		m_KeyListCtrl.SetItemText( idx, 10, "-");
	}
}

void Ci3AnimationKeyDlg::UpdateAllKeys(void)
{
	INT32 i, idx;
	REAL32 tm;
	char conv[128];

	m_KeyListCtrl.DeleteAllItems();

	if( m_pCurSeq == NULL)
		return;

	for( i = 0; i < (INT32) m_pCurSeq->GetKeyframeCount(); i++)
	{
		if( m_pCurSeq->GetKeyframeChannels() & i3TransformSequence::TIME)
		{
			tm = m_pCurSeq->GetTime( i);
		}
		else
		{
			//tm = m_pCurSeq->GetKeyframeInterval() * (REAL32)(i + 1); <- 이러면 첫번째 키프레임의 데이터가 잘못 표시되어, 이후 데이터들 모두 잘못표시됨
			tm = m_pCurSeq->GetKeyframeInterval() * (REAL32)(i);
		}

		i3::ftoa( tm, conv);

		idx = m_KeyListCtrl.InsertItem( i, conv, 0);

		UpdateKey( i);

		
	}
}

void Ci3AnimationKeyDlg::OnNMRclickTracklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu, * pPopup;
	CPoint pt;
	UINT32 cmd;

	GetCursorPos( &pt);

	menu.LoadMenu( IDR_POPUP_TRACK);

	pPopup = menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_TRACK_DELETE :	OnDelete();	break;
	}

	*pResult = 0;
}

void Ci3AnimationKeyDlg::OnDelete(void)
{
	INT32 idx = GetSelectedTrack(), dstIdx;

	if( idx == -1)
		return;

	i3Animation * pNew = i3Animation::new_object_ref();
	INT32 i;

	pNew->Create( m_pAnim->GetTrackCount() - 1);

	dstIdx = 0;

	for( i = 0; i < m_pAnim->GetTrackCount(); i++)
	{
		if( i != idx)
		{
			pNew->SetTrack( dstIdx, m_pAnim->GetTrack(i), m_pAnim->GetTrackBoneName(i));

			dstIdx++;
		}
	}

	SetAnimation( pNew);
}

void Ci3AnimationKeyDlg::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "i3a", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Animation File(*.i3a)|*.i3a||", NULL);

	if( Dlg.DoModal() != IDOK)	return;

	LoadAnim( LPCTSTR( Dlg.GetPathName()));
}

 

void Ci3AnimationKeyDlg::OnFileSave()
{
	if( m_szFileName[0] == 0)
	{
		OnFileSaveas();
		return;
	}

	SaveAnim( m_szFileName);
}

void Ci3AnimationKeyDlg::OnFileSaveas()
{
	CFileDialog	Dlg( FALSE, "i3a", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Animation File(*.i3a)|*.i3a||", NULL);

	if( Dlg.DoModal() != IDOK )
	{
		return; 
	}

	if( SaveAnim( LPCTSTR( Dlg.GetPathName())) == FALSE)
	{
		DebugBox( m_hWnd, "Could not write %s file.", LPCTSTR( Dlg.GetPathName()));
		return;
	}

	SetTitleName( LPCTSTR( Dlg.GetPathName()));
}

void Ci3AnimationKeyDlg::OnFileExit()
{
	OnOK();
}

void Ci3AnimationKeyDlg::OnNMRclickKeylist(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu, * pPopup;
	CPoint pt;
	UINT32 cmd;

	GetCursorPos( &pt);

	menu.LoadMenu( IDR_POPUP_SEQ);

	pPopup = menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_SEQ_SETCHANNELS :	
			if( m_pCurSeq != NULL)
			{
				CSetChannelDialog dlg;

				m_pCurSeq = dlg.Execute( m_pAnim, m_pCurSeq);

				UpdateAllKeys();
			}
			break;

		case ID_SEQ_SETSCALEKEY :	
			{
				i3::vector<INT32> SelList;

				GetSelectedKey( SelList);

				if( SelList.size() > 0)
				{
					CSetScaleDialog dlg;
					VEC3D vec;
					INT32 idx;

					idx = SelList[0];

					m_pCurSeq->GetScale( idx, &vec);

					if( dlg.Execute( &vec) == FALSE)
						return;

					i3Vector::Copy( &vec, dlg.GetScale());

					for(size_t i = 0; i < SelList.size(); i++)
					{
						idx = SelList[i];

						m_pCurSeq->SetScale( idx, &vec);
					}

					UpdateAllKeys();
				}
			}
			break;
	}

	*pResult = 0;
}

void Ci3AnimationKeyDlg::GetSelectedKey( i3::vector<INT32>&  List)
{
	POSITION pos;

	pos = m_KeyListCtrl.GetFirstSelectedItemPosition();

	while( pos != NULL)
	{
		INT32 idx = m_KeyListCtrl.GetNextSelectedItem( pos);

		List.push_back(idx);
	}
}

//void CAboutDlg::OnFileOptimize()
//{
//	// TODO: Add your command handler code here
//}

void Ci3AnimationKeyDlg::OnFileOptimize()
{
	if( m_pAnim != NULL)
	{
		i3OptAnimation opt;

		opt.OptimizeAnim( m_pAnim);

		m_pCurSeq = NULL;

		UpdateAllTracks();
		UpdateAllKeys();

	}
}

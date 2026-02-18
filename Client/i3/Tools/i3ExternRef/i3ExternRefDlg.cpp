// i3ExternRefDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3ExternRef.h"
#include "i3ExternRefDlg.h"
#include "DlgOption.h"

#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/ext/make_relative_path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3ExternRefDlg 대화 상자



Ci3ExternRefDlg::Ci3ExternRefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3ExternRefDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pCtx = NULL;
	m_pPhysixCtx = NULL;
}

void Ci3ExternRefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_CHK_TEXTURE, m_ChkTextureCtrl);
}

BEGIN_MESSAGE_MAP(Ci3ExternRefDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
END_MESSAGE_MAP()


// Ci3ExternRefDlg 메시지 처리기

BOOL Ci3ExternRefDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	{
		m_ListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 220);
	}

	{
		if( g_pOption->m_bExternTexture)
			m_ChkTextureCtrl.SetCheck( BST_CHECKED);
		else
			m_ChkTextureCtrl.SetCheck( BST_UNCHECKED);
	}

	{
		i3VideoInfo info;

		m_pCtx = i3RenderContext::new_object();

		m_pCtx->Create( &info, m_hWnd);
	}

	{
		m_pPhysixCtx = i3PhysixContext::new_object();
		m_pPhysixCtx->Create();
	}

	{
		m_pParticleRender = i3ParticleRender::new_object();
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3ExternRefDlg::OnDestroy()
{
	CDialog::OnDestroy();

	I3_SAFE_RELEASE( m_pParticleRender);
	I3_SAFE_RELEASE( m_pCtx);
	I3_SAFE_RELEASE( m_pPhysixCtx);
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3ExternRefDlg::OnPaint() 
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
HCURSOR Ci3ExternRefDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3ExternRefDlg::OnDropFiles(HDROP hDropInfo)
{
	int count, i; 
	char FileName[256];

	count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	m_ListCtrl.DeleteAllItems();

	for( i = 0; i < count; i++)
	{
		DragQueryFile( hDropInfo, i, FileName, sizeof(FileName) - 1);

		_Process( FileName);
	}

	CDialog::OnDropFiles(hDropInfo);
}

void Ci3ExternRefDlg::_ReplaceControls( INT32 cx, INT32 cy)
{
	if( cx == -1)
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	m_ListCtrl.SetWindowPos( NULL, 0, 0, cx - 7, cy - 33, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

void Ci3ExternRefDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	_ReplaceControls( cx, cy);
}

bool Ci3ExternRefDlg::_ReportProc( i3NamedElement * pObj, void * pData)
{
	CListCtrl * pCtrl = (CListCtrl *) pData;
	INT32 idx;

	idx = pCtrl->InsertItem( pCtrl->GetItemCount(), pObj->GetName(), 0);

	return true;
}

void Ci3ExternRefDlg::_Process( char * pszPath)
{
	i3OptExternRef opt;
	char szPath[MAX_PATH];
	i3SceneGraphInfo * pInfo = NULL;

	{
		i3SceneFile file;

		if( file.Load( pszPath) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_NOTICE, "%s 파일을 읽어 들일 수 없습니다.", pszPath);
			return;
		}

		pInfo = file.getSceneGraphInfo();

		if( pInfo == NULL)
		{
			I3PRINTLOG(I3LOG_NOTICE, "%s 파일은 I3S 파일이 아닙니다.", pszPath);
			return;
		}

		pInfo->AddRef();
	}
	
	{
//		char * pszName;
//		char szName[MAX_PATH];

		if( g_pOption->m_szExportResFolder[0] != 0)
		{
			i3::make_relative_path( g_pOption->m_szWorkPath, g_pOption->m_szExportResFolder, szPath );
		}
		else
		{
			i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szPath );
			
			i3::remove_slash_filename(szPath);
		//	pszName = i3String::SplitFileName( szPath, szName, FALSE);
		//	*pszName = 0;
		}

	//	i3String::NormalizePath( szPath);
		opt.setBasePath( szPath);
	}
	
	opt.setUserProc( _ReportProc, &m_ListCtrl);
	opt.setExternTextureEnable( m_ChkTextureCtrl.GetCheck() == BST_CHECKED);

	// Common Scene-Graph
	if( pInfo->getCommonSg() != NULL)
	{
		opt.Trace( pInfo->getCommonSg());
	}
	
	// Instance Scene-Graph
	if( pInfo->getInstanceSg() != NULL)
	{
		opt.Trace( pInfo->getInstanceSg());
	}

	// Overwrite
	{
		i3SceneFile file;

		file.setSceneGraphInfo( pInfo);
		file.SetOverwriteExternalResStatus( true);

		if( file.Save( pszPath) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "%s 파일을 저장할 수 없습니다", pszPath);
		}

		i3ResourceFile::SetProtectionCode( pszPath, "Zepetto Inc.", 12);
	}

	pInfo->Release();
}

void Ci3ExternRefDlg::OnToolsOptions()
{
	CDlgOption dlg;

	dlg.DoModal();
}

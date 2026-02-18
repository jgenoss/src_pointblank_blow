// i3ShaderManagerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3ShaderManager.h"
#include "i3ShaderManagerDlg.h"
#include "DlgISSVersion.h"
#include "DlgRegenOption.h"
#include ".\i3shadermanagerdlg.h"

#include "i3Base/string/ext/extract_directoryname.h"


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
//	afx_msg void OnSize(UINT nType, int cx, int cy);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//	ON_WM_SIZE()
END_MESSAGE_MAP()


// Ci3ShaderManagerDlg 대화 상자



Ci3ShaderManagerDlg::Ci3ShaderManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3ShaderManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pGfx = NULL;
	m_pCache = NULL;
	m_szFileName[0] = 0;
}

void Ci3ShaderManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ShaderListCtrl);
	DDX_Control(pDX, IDC_EDIT_SHADER_COUNT, m_ShaderCountCtrl);
}

BEGIN_MESSAGE_MAP(Ci3ShaderManagerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_FILE_MERGE, OnFileMerge)
	ON_BN_CLICKED(IDC_BTN_ISSVERSION, OnBnClickedBtnIssversion)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)
	ON_COMMAND(ID_FILE_EXPORTXML, OnFileExportxml)
	ON_COMMAND(ID_FILE_IMPORTXML, OnFileImportxml)
END_MESSAGE_MAP()


// Ci3ShaderManagerDlg 메시지 처리기

BOOL Ci3ShaderManagerDlg::OnInitDialog()
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
		CRect rt;

		rt.SetRect( 0, 0, 1, 1);

		m_LogCtrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 102 );
	}

	{
		m_ShaderListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 180);
		m_ShaderListCtrl.InsertColumn( 1, "Vendor", LVCFMT_LEFT, 50);
		m_ShaderListCtrl.InsertColumn( 2, "ISS", LVCFMT_LEFT, 26);
		m_ShaderListCtrl.InsertColumn( 3, "Req VS", LVCFMT_LEFT, 50);
		m_ShaderListCtrl.InsertColumn( 4, "Req PS", LVCFMT_LEFT, 50);
		m_ShaderListCtrl.InsertColumn( 5, "VS", LVCFMT_LEFT, 70);
		m_ShaderListCtrl.InsertColumn( 6, "BVS1", LVCFMT_LEFT, 70);
		m_ShaderListCtrl.InsertColumn( 7, "BVS2", LVCFMT_LEFT, 70);
		m_ShaderListCtrl.InsertColumn( 8, "BVS3", LVCFMT_LEFT, 70);
		m_ShaderListCtrl.InsertColumn( 9, "BVS4", LVCFMT_LEFT, 70);
		m_ShaderListCtrl.InsertColumn( 10, "PS", LVCFMT_LEFT, 70);

		m_ShaderListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		i3VideoInfo info;

		info.m_Width = 100;
		info.m_Height = 100;

		m_pGfx = i3RenderContext::new_object();

		m_pGfx->Create( &info, m_hWnd);
	}

	_PlaceControls( -1, -1);

	_NewCache();
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3ShaderManagerDlg::OnDestroy()
{
	I3_SAFE_RELEASE( m_pCache);
	I3_SAFE_RELEASE( m_pGfx);

	CDialog::OnDestroy();
}

void Ci3ShaderManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3ShaderManagerDlg::OnPaint() 
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
HCURSOR Ci3ShaderManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define LOG_CY			200

void Ci3ShaderManagerDlg::_PlaceControls( int cx, int cy)
{
	if( cx == -1)
	{
		CRect rect;

		GetClientRect( &rect);

		cx = rect.Width();
		cy = rect.Height();
	}

	INT32 y = cy - LOG_CY - 12 - 28;

	m_ShaderListCtrl.SetWindowPos( NULL, 0, 0, cx - 12, y, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	CRect rt;

	m_ShaderListCtrl.GetWindowRect( &rt);

	ScreenToClient( &rt);

	m_LogCtrl.SetWindowPos( NULL, rt.left, rt.bottom + 3, rt.Width(), cy - rt.bottom - 6, SWP_NOACTIVATE | SWP_NOZORDER);
}

void Ci3ShaderManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ShaderListCtrl.m_hWnd))
	{
		_PlaceControls( cx, cy);
	}
}

void Ci3ShaderManagerDlg::_UpdateAllShader(void)
{
	INT32 i, idx;

	m_ShaderListCtrl.DeleteAllItems();

	for( i = 0; i < m_pCache->getShaderCount(); i++)
	{
		i3Shader * pShader = m_pCache->getShader( i);

		idx = m_ShaderListCtrl.InsertItem( 0, pShader->GetName());

		_UpdateShader( idx, pShader);
	}

	// Total Count
	char conv[64];

	sprintf( conv, "%d\n", m_pCache->getShaderCount());
	m_ShaderCountCtrl.SetWindowText( conv);
}

void Ci3ShaderManagerDlg::_UpdateShader( INT32 idx, i3Shader * pShader)
{
	char conv[128];
	char m, n;
	INT32 i;

	m_ShaderListCtrl.SetItemData( idx, (DWORD_PTR) pShader);

	switch( pShader->getTargetGPU())
	{
		case I3G_GPU_VENDOR_NVIDIA :	m_ShaderListCtrl.SetItemText( idx, 1, "NVIDIA");	break;
		case I3G_GPU_VENDOR_ATI :		m_ShaderListCtrl.SetItemText( idx, 1, "ATI");	break;
		default  :						m_ShaderListCtrl.SetItemText( idx, 1, "-");	break;
	}

	// ISS
	sprintf( conv, "%d", pShader->getISSVersion());
	m_ShaderListCtrl.SetItemText( idx, 2, conv);

	// Requested VS
	sprintf( conv, "%d", pShader->getRequestedVSVersion());
	m_ShaderListCtrl.SetItemText( idx, 3, conv);

	// Requested PS
	sprintf( conv, "%d", pShader->getRequestedPSVersion());
	m_ShaderListCtrl.SetItemText( idx, 4, conv);

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		m = (pShader->getShaderVersion( (I3G_SHADER_TYPE) (I3G_SHADER_TYPE_VERTEX + i)) >> 8) & 0xFF;
		n = (pShader->getShaderVersion( (I3G_SHADER_TYPE) (I3G_SHADER_TYPE_VERTEX + i))) & 0xFF;

		sprintf( conv, "%c_%c (%d)", m, n, pShader->getShaderDataSize( (I3G_SHADER_TYPE) (I3G_SHADER_TYPE_VERTEX + i)));
		m_ShaderListCtrl.SetItemText( idx, 5 + i, conv);
	}
}

void Ci3ShaderManagerDlg::_NewCache(void)
{
	I3_SAFE_RELEASE( m_pCache);

	m_pCache = i3ShaderCache::new_object();

	_UpdateAllShader();

	m_szFileName[0] = 0;
}

void Ci3ShaderManagerDlg::_OpenCache( const char * pszPath)
{
	I3_SAFE_RELEASE( m_pCache);

	m_pCache = i3ShaderCache::Load( (char *) pszPath);

	_UpdateAllShader();

	strcpy( m_szFileName, pszPath);
}

INT32 Ci3ShaderManagerDlg::_FindShaderByName( char * pszName)
{
	INT32 i;

	for( i = 0; i < m_pCache->getShaderCount(); i++)
	{
		i3Shader * pShader = m_pCache->getShader( i);

		if( i3::generic_is_iequal( pShader->GetName(), pszName) )
		{
			return i;
		}
	}

	return -1;
}

void Ci3ShaderManagerDlg::_MergeCache( const char * pszPath)
{
	i3ShaderCache * pCache;

	I3ASSERT( m_pCache != NULL);

	pCache = i3ShaderCache::Load( (char *) pszPath);

	INT32 i;
	i3Shader * pShader;

	for( i = 0; i < pCache->getShaderCount(); i++)
	{
		pShader = pCache->getShader( i);

		if( _FindShaderByName( (char *) pShader->GetName()) == -1)
		{
			m_pCache->addShader( pShader);
		}
	}

	_UpdateAllShader();
}

void Ci3ShaderManagerDlg::_SaveCache( const char * pszPath)
{
	if( m_pCache != NULL)
	{
		m_pCache->Save( pszPath);
	}

	strcpy( m_szFileName, pszPath);
}

void Ci3ShaderManagerDlg::OnFileNew()
{
	_NewCache();
}

void Ci3ShaderManagerDlg::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "i3GL", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Shader Cache File(*.i3GL)|*.i3GL||", this);

	if( Dlg.DoModal() != IDOK)	return;

	_OpenCache( (const char *) Dlg.GetPathName());
}

void Ci3ShaderManagerDlg::OnFileMerge()
{
	CFileDialog	Dlg( TRUE, "i3GL", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Shader Cache File(*.i3GL)|*.i3GL||", this);

	if( Dlg.DoModal() != IDOK)	return;

	_MergeCache( (const char *) Dlg.GetPathName());
}


void Ci3ShaderManagerDlg::OnFileSave()
{
	if( m_szFileName[0] == 0)
	{
		OnFileSaveas();
		return;
	}

	_SaveCache( m_szFileName);
}

void Ci3ShaderManagerDlg::OnFileSaveas()
{
	CFileDialog	Dlg( FALSE, "i3GL", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3Engine Shader Cache File(*.i3GL)|*.i3GL||", this);

	if( Dlg.DoModal() != IDOK)	return;

	_SaveCache( (const char *) Dlg.GetPathName());
}


void Ci3ShaderManagerDlg::OnBnClickedBtnIssversion()
{
	CDlgISSVersion dlg;

	dlg.Execute( m_pCache);
}

INT32 Ci3ShaderManagerDlg::_getSelected(void)
{
	POSITION pos = m_ShaderListCtrl.GetFirstSelectedItemPosition();

	if( pos == NULL)
		return -1;

	return m_ShaderListCtrl.GetNextSelectedItem( pos);
}

void Ci3ShaderManagerDlg::_getAllSelected( i3::vector<INT32>& List)
{
	POSITION pos = m_ShaderListCtrl.GetFirstSelectedItemPosition();

	while( pos != NULL)
	{
		INT32 idx = m_ShaderListCtrl.GetNextSelectedItem( pos);

		List.push_back( idx);
	}
}

void Ci3ShaderManagerDlg::_Delete(void)
{
	INT32 idxSel;
	i3Shader * pShader;
	i3::vector<INT32> list;
	INT32 i;

	_getAllSelected( list);

	if ( list.empty() ) return;

	for( i = list.size() ; i > 0; i--)
	{
		idxSel = list[i - 1];

		pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( idxSel);

		m_LogCtrl.AddText( "%s Shader가 제거됨.\n", pShader->GetName());

		m_pCache->RemoveShader( pShader);

		m_ShaderListCtrl.DeleteItem( idxSel);
	}
}

void Ci3ShaderManagerDlg::_Regenerate( INT32 idx)
{
	CDlgRegenOption dlg;
	I3G_SHADER_VERSION vsVersion;
	I3G_SHADER_VERSION psVersion;
//	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	if( dlg.Execute() == false)
		return;

	if( idx == -1)
		idx = _getSelected();

	I3ASSERT( idx != -1);

	i3Shader * pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( idx);

	if( dlg.m_bOverride)
	{
		vsVersion	= dlg.m_vsVersion;
		psVersion	= dlg.m_psVersion;
	}
	else
	{
		vsVersion	= max( pShader->getRequestedVSVersion(), dlg.m_vsVersion);
		psVersion	= max( pShader->getRequestedPSVersion(), dlg.m_psVersion);
	}

	pShader->setRequestedVSVersion( vsVersion);
	pShader->setRequestedPSVersion( psVersion);

	if( dlg.m_Vendor != I3G_GPU_VENDOR_NA)
		pShader->setTargetGPU( dlg.m_Vendor);

	if( pShader->Rebuild() == false)
	{
		m_LogCtrl.AddText( pShader->getContext()->getErrorMsg());
	}
	else
	{
		pShader->Validate( m_pGfx);
		m_LogCtrl.AddText( "%s Shader가 재생성되었습니다.\n", pShader->GetName());

		pShader->setISSVersion( i3ShaderCache::GetRequestedISSVersion());
	}

	_UpdateShader( idx, pShader);
}

void Ci3ShaderManagerDlg::_RegenerateAll(void)
{
	INT32 i;
	i3Shader * pShader;
	bool bResult = true;
	I3G_SHADER_VERSION vsVersion;
	I3G_SHADER_VERSION psVersion;
//	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	CDlgRegenOption dlg;

	if( dlg.Execute() == false)
		return;

	for( i = 0; i < m_ShaderListCtrl.GetItemCount(); i++)
	{
		pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( i);

		vsVersion	= max( pShader->getRequestedVSVersion(), dlg.m_vsVersion);
		psVersion	= max( pShader->getRequestedPSVersion(), dlg.m_psVersion);

		pShader->setRequestedVSVersion( vsVersion);
		pShader->setRequestedPSVersion( psVersion);

		if( dlg.m_Vendor != I3G_GPU_VENDOR_NA)
			pShader->setTargetGPU( dlg.m_Vendor);

		bResult = pShader->Rebuild();
		if( bResult == false)
		{
			m_LogCtrl.AddText( "[ERROR] %s Shader\n", pShader->GetName());
			m_LogCtrl.AddText( pShader->getContext()->getErrorMsg());
		}
		else
		{
			pShader->Validate( m_pGfx);

			m_LogCtrl.AddText( "%s was rebuilt.", pShader->GetName());

			pShader->setISSVersion( i3ShaderCache::GetRequestedISSVersion());
		}

		_UpdateShader( i, pShader);
	}

	// Total Count
	char conv[64];

	sprintf( conv, "%d\n", m_pCache->getShaderCount());
	m_ShaderCountCtrl.SetWindowText( conv);
}

INT32 Ci3ShaderManagerDlg::_FindShaderNameAndTarget( const char * pszName, I3G_GPU_VENDOR target)
{
	INT32 i;
	i3Shader * pShader;

	for( i = 0; i < m_ShaderListCtrl.GetItemCount(); i++)
	{
		pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( i);

		if( i3::generic_is_iequal( pShader->GetName(), pszName) )
		{
			if( pShader->getTargetGPU() == target)
				return i;
		}
	}

	return -1;
}

void Ci3ShaderManagerDlg::_GenerateCloneShader( i3Shader * pSrcShader, I3G_GPU_VENDOR vendor)
{
	i3Shader * pNew;

	pNew = i3ShaderCache::CloneShader( pSrcShader, vendor);

	if( pNew == NULL)
	{
		m_LogCtrl.AddText( "%s Shader의 Clone을 생성할 수 없습니다.", pSrcShader->GetName());
		return;
	}
	else
	{
		m_LogCtrl.AddText( "%s Shader의 Clone 추가.", pSrcShader->GetName());
	}

	pNew->Validate( g_pRenderContext);

	m_pCache->addShader( pNew);

	INT32 idx = m_ShaderListCtrl.InsertItem( 0, pNew->GetName(), 0);

	_UpdateShader( idx, pNew);

	// Total Count
	char conv[64];

	sprintf( conv, "%d\n", m_pCache->getShaderCount());
	m_ShaderCountCtrl.SetWindowText( conv);
}

void Ci3ShaderManagerDlg::_GenerateHWSpecificShaders(void)
{
	INT32 i, j, idx;
	bool bCheckVS, bCheckPS;
	UINT16 version, checkVersion;
	UINT8 minor;
	i3Shader * pShader;

	for( i = 0; i < m_ShaderListCtrl.GetItemCount(); i++)
	{
		pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( i);

		{
			bCheckVS = bCheckPS = false;
			version = checkVersion = 0;

			for( j = 0; j < I3G_SHADER_TYPE_MAX; j++)
			{
				version = pShader->getShaderVersion( (I3G_SHADER_TYPE) j);

				if( (version >> 8) == '2')
				{
					checkVersion = version;

					if( j == I3G_SHADER_TYPE_PIXEL)
						bCheckPS = true;
					else
						bCheckVS = true;
				}
			}
		}

		if( (bCheckVS == false) && (bCheckPS == false))
			continue;

		minor = checkVersion & 0xFF;

		switch( minor)
		{
			case '0' :
				{
					idx = _FindShaderNameAndTarget( pShader->GetName(), I3G_GPU_VENDOR_NVIDIA);
					if( idx == -1)
						_GenerateCloneShader( pShader, I3G_GPU_VENDOR_NVIDIA);

					if( bCheckPS)
					{
						idx = _FindShaderNameAndTarget( pShader->GetName(), I3G_GPU_VENDOR_ATI);
						if( idx == -1)
							_GenerateCloneShader( pShader, I3G_GPU_VENDOR_ATI);
					}
				}
				break;

			case 'a' :
				{
					idx = _FindShaderNameAndTarget( pShader->GetName(), I3G_GPU_VENDOR_STANDARD);
					if( idx == -1)
						_GenerateCloneShader( pShader, I3G_GPU_VENDOR_STANDARD);

					if( bCheckPS)
					{
						idx = _FindShaderNameAndTarget( pShader->GetName(), I3G_GPU_VENDOR_ATI);
						if( idx == -1)
							_GenerateCloneShader( pShader, I3G_GPU_VENDOR_ATI);
					}
				}
				break;

			case 'b' :
				{
					idx = _FindShaderNameAndTarget( pShader->GetName(), I3G_GPU_VENDOR_STANDARD);
					if( idx == -1)
						_GenerateCloneShader( pShader, I3G_GPU_VENDOR_STANDARD);

					idx = _FindShaderNameAndTarget( pShader->GetName(), I3G_GPU_VENDOR_NVIDIA);
					if( idx == -1)
						_GenerateCloneShader( pShader, I3G_GPU_VENDOR_NVIDIA);
				}
				break;
		}
	}

	_UpdateAllShader();
}

void Ci3ShaderManagerDlg::_UnselectAll(void)
{
	INT32 i;

	for( i = 0; i < m_ShaderListCtrl.GetItemCount(); i++)
	{
		m_ShaderListCtrl.SetItemState( i, 0, LVIS_SELECTED);
	}
}

void Ci3ShaderManagerDlg::_SelectAllPPLShaders(void)
{
	INT32 i;
	i3Shader * pShader;
//	bool bResult = true;
//	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	for( i = 0; i < m_ShaderListCtrl.GetItemCount(); i++)
	{
		pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( i);

		if( pShader->getShaderCode()->isPPLShader())
		{
			m_ShaderListCtrl.SetItemState( i, 0xFFFFFFFF, LVIS_SELECTED);
		}
		else
		{
			m_ShaderListCtrl.SetItemState( i, 0, LVIS_SELECTED);
		}
	}
}

void Ci3ShaderManagerDlg::_SelectAllPVLShaders(void)
{
	INT32 i;
	i3Shader * pShader;
//	bool bResult = true;
//	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	for( i = 0; i < m_ShaderListCtrl.GetItemCount(); i++)
	{
		pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( i);

		if( pShader->getShaderCode()->isPPLShader())
		{
			m_ShaderListCtrl.SetItemState( i, 0, LVIS_SELECTED);
		}
		else
		{
			m_ShaderListCtrl.SetItemState( i, 0xFFFFFFFF, LVIS_SELECTED);
		}
	}
}

void Ci3ShaderManagerDlg::_SelectAllRedundantShaders(void)
{
	INT32 i, j;
	i3Shader * pShader1, * pShader2;
//	bool bResult = true;
//	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	_UnselectAll();

	for( i = 0; i < m_ShaderListCtrl.GetItemCount() - 1; i++)
	{
		if( m_ShaderListCtrl.GetItemState( i, LVIS_SELECTED) != 0)
			continue;

		pShader1 = (i3Shader *) m_ShaderListCtrl.GetItemData( i);

		for( j = i + 1; j < m_ShaderListCtrl.GetItemCount(); j++)
		{
			pShader2 = (i3Shader *) m_ShaderListCtrl.GetItemData( j);

			if( i3::generic_is_iequal( pShader1->GetName(), pShader2->GetName()) )
			{
				m_ShaderListCtrl.SetItemState( j, 0xFFFFFFFF, LVIS_SELECTED);
			}
		}
	}
}

void Ci3ShaderManagerDlg::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu, * pPopup;
	CPoint pt, cpt;
	UINT32 cmd;

	GetCursorPos( &pt);

	cpt = pt;
	ScreenToClient( &cpt);

	menu.LoadMenu( IDR_POPUP);

	pPopup = menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_POPUP_DELETE :						_Delete();	break;
		case ID_POPUP_REGENERATE :					_Regenerate();	break;
		case ID_POPUP_REGENERATEALL :				_RegenerateAll();		break;
		case ID_POPUP_GENERATEHWSPECIFICSHADERS :	_GenerateHWSpecificShaders();	break;
		case ID_POPUP_SELECTALLPPLSHADERS :			_SelectAllPPLShaders();	break;
		case ID_POPUP_SELECTALLPVLSHADERS :			_SelectAllPVLShaders();	break;
		case ID_POPUP_SELECTALLREDUNDENTSHADERS :	_SelectAllRedundantShaders();	break;
		case ID_POPUP_SORT :						_Sort(); break;
	}

	*pResult = 0;
}

void Ci3ShaderManagerDlg::_Sort(void)
{
	m_pCache->Sort();
}

void Ci3ShaderManagerDlg::OnFileExportxml()
{
	i3::vector<i3PersistantElement*> list;
	INT32 i, cnt;

	CFileDialog	Dlg( FALSE, "i3GL_XML", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3Engine Shader Cache XML File(*.i3GL_XML)|*.i3GL_XML||", this);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	cnt = m_ShaderListCtrl.GetItemCount();

	for( i = 0; i < cnt; i++)
	{
		i3Shader * pShader = (i3Shader *) m_ShaderListCtrl.GetItemData( i);

		list.push_back( pShader);
	}

	{
		i3XMLFile file;

		file.SetObjects( list);

		file.Save( (LPCTSTR) Dlg.GetPathName());
	}
}

void Ci3ShaderManagerDlg::OnFileImportxml()
{
	char szWorkDir[ MAX_PATH];
	CFileDialog	Dlg( TRUE, "i3GL_XML", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Shader Cache XML File(*.i3GL_XML)|*.i3GL_XML||", this);

	if( Dlg.DoModal() != IDOK)	return;

	{
	//	i3String::SplitPath( (LPCTSTR) Dlg.GetPathName(), szWorkDir);
		i3::extract_directoryname((LPCTSTR) Dlg.GetPathName(), szWorkDir);

		m_pCache->setWorkDir( szWorkDir);
	}

	i3XMLFile file;

	file.Load( (LPCTSTR) Dlg.GetPathName());

	INT32 i;

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		i3PersistantElement * pObj = file.GetObject( i);

		if( i3::same_of<i3Shader*>(pObj)) //->IsExactTypeOf( i3Shader::static_meta()))
		{
			i3Shader * pShader = (i3Shader *) pObj;
			m_pCache->addShader( pShader);
		}
	}

	_UpdateAllShader();
}

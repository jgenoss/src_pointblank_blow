// Panel_Opt.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiOpt.h"
#include "Panel_Opt.h"
#include "GlobalVar.h"
#include "MainFrm.h"
#include "i3Base/string/ext/generic_string_cat.h"
// CPanel_Opt dialog

IMPLEMENT_DYNAMIC(CPanel_Opt, i3TDKDialogBase)

CPanel_Opt::CPanel_Opt(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanel_Opt::IDD, pParent), m_pImageList(NULL)
{
	m_szConfigPath[0] = 0;
	m_pThread = NULL;
}

CPanel_Opt::~CPanel_Opt()
{
	I3_SAFE_DELETE(m_pImageList);
	I3_SAFE_RELEASE( m_pThread);
}

void CPanel_Opt::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OPEN_CONFIG, m_BTN_Open);
	DDX_Control(pDX, IDC_BTN_SAVE_CONFIG, m_BTN_Save);
	DDX_Control(pDX, IDC_BTN_SELECTALL, m_BTN_SelAll);
	DDX_Control(pDX, IDC_BTN_DELETE, m_BTN_Del);
	DDX_Control(pDX, IDC_BTN_RUN, m_BTN_Run);
	DDX_Control(pDX, IDC_BTN_STOP, m_BTN_Stop);
	DDX_Control(pDX, IDC_LIST_OPTS, m_LIST_Opt);
}

void	CPanel_Opt::StartProcessing(void)
{
	m_BTN_Open.EnableWindow( FALSE);
	m_BTN_Save.EnableWindow( FALSE);

	m_BTN_SelAll.EnableWindow( FALSE);
	m_BTN_Del.EnableWindow( FALSE);

	m_BTN_Run.EnableWindow( FALSE);
	m_BTN_Stop.EnableWindow( TRUE);

	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	m_pThread = COptThread::new_object();
	m_pThread->setOwner( pMainWnd->m_hWnd);
	m_pThread->Create( "OptThread");
}

void	CPanel_Opt::EndProcessing(void)
{
	m_BTN_Open.EnableWindow( TRUE);
	m_BTN_Save.EnableWindow( TRUE);

	m_BTN_SelAll.EnableWindow( TRUE);
	m_BTN_Del.EnableWindow( TRUE);

	m_BTN_Run.EnableWindow( TRUE);
	m_BTN_Stop.EnableWindow( FALSE);

	if( m_pThread != NULL)
	{
		m_pThread->Terminate();
		m_pThread->Release();
		m_pThread = NULL;
	}
}

void	CPanel_Opt::AddOpt( i3SceneOptimizer * pOpt)
{
	
	const char * pszName = pOpt->getDescName();
	INT32 idxImage;

	{
		INT32 iClass = pOpt->getClass();
		INT32 idx = FindClassTblByClass( iClass);
		I3ASSERT( idx != -1);

		idxImage = idx;
	}

	INT32 idx = m_LIST_Opt.InsertItem( m_LIST_Opt.GetItemCount(), pszName, idxImage);

	UpdateItem( idx, pOpt);

	m_LIST_Opt.EnsureVisible( idx, FALSE);
}

INT32	CPanel_Opt::FindOpt( i3SceneOptimizer * pOpt)
{
	INT32 i;

	for( i = 0; i < m_LIST_Opt.GetItemCount(); i++)
	{
		if( m_LIST_Opt.GetItemData( i) == (DWORD_PTR) pOpt)
			return i;
	}

	return -1;
}

void	CPanel_Opt::RemoveOpt( i3SceneOptimizer * pOpt)
{
	INT32 idx;

	idx = FindOpt( pOpt);

	if( idx != -1)
	{
		m_LIST_Opt.DeleteItem( idx);
	}
}

void	CPanel_Opt::RemoveAllOpt(void)
{
	m_LIST_Opt.DeleteAllItems();
}

void	CPanel_Opt::UpdateItem( INT32 idx, i3SceneOptimizer * pOpt)
{
	m_LIST_Opt.SetItemData( idx, (DWORD_PTR) pOpt);

	{
		char conv[4096];
		i3XMLElement * pXML = new i3XMLElement( "temp");

		pOpt->SaveToXML( pXML);

		conv[0] = 0;

		ResolveXML( pXML, conv, sizeof(conv));

		m_LIST_Opt.SetItemText( idx, 1, conv);

		delete pXML;
	}
}

void	CPanel_Opt::UpdateAllItem(void)
{
	INT32 i;
	i3SceneOptimizer * pOpt;

	m_LIST_Opt.DeleteAllItems();

	for( i = 0; i < g_pOptConfig->getCount(); i++)
	{
		pOpt = g_pOptConfig->getOpt( i);

		AddOpt( pOpt);
	}
}

void	CPanel_Opt::ResolveXML( i3XMLElement * pXML, char * pszStr, INT32 length)
{
	TiXmlAttribute * pAttr = pXML->FirstAttribute();
	char conv[256];

	while( pAttr != NULL)
	{
		i3::snprintf( conv, sizeof(conv), "%s(%s) ", pAttr->Name(), pAttr->Value());

		i3::generic_string_cat( pszStr, conv);

		pAttr = pAttr->Next();
	}

	i3XMLElement * pChild = (i3XMLElement *) pXML->FirstChildElement();

	while( pChild != NULL)
	{
		ResolveXML( pChild, pszStr, length);

		pChild = (i3XMLElement *) pChild->NextSiblingElement();
	}
}

BEGIN_MESSAGE_MAP(CPanel_Opt, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BTN_OPEN_CONFIG, &CPanel_Opt::OnBnClickedBtnOpenConfig)
	ON_BN_CLICKED(IDC_BTN_SAVE_CONFIG, &CPanel_Opt::OnBnClickedBtnSaveConfig)
	ON_BN_CLICKED(IDC_BTN_SELECTALL, &CPanel_Opt::OnBnClickedBtnSelectall)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CPanel_Opt::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_RUN, &CPanel_Opt::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_STOP, &CPanel_Opt::OnBnClickedBtnStop)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPTS, &CPanel_Opt::OnNMDblclkListOpts)
END_MESSAGE_MAP()


// CPanel_Opt message handlers

static void _SetIcon( CButton * pButton, INT32 id)
{
	HINSTANCE hInst = AfxGetApp()->m_hInstance;
	HICON hIcon;

	hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( id), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR);

	pButton->SetIcon( hIcon);
}

BOOL CPanel_Opt::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_hFontTitle = i3TDK::CreateDefFont( 14, "Tahoma", 700);
		m_hFontConfig = i3TDK::CreateDefFont( 12, "Tahoma", 0);
	}

	{
		INT32 i;
		static INT32 s_IconTable[ i3SceneOptimizer::CLASS_MAX] =
		{
			IDI_BIG_GEOMETRY,
			IDI_BIG_SCENEGRAPH,
			IDI_BIG_TEXTURE,
			IDI_BIG_ANIM,
			IDI_BIG_SOUND,
			IDI_BIG_PHYSICS,
			IDI_BIG_NA
		};

		m_pImageList = new CImageList;
		m_pImageList->Create( 32, 32, ILC_COLORDDB, i3SceneOptimizer::CLASS_MAX, 0); 

		for( i = 0; i < i3SceneOptimizer::CLASS_MAX; i++)
		{
			HICON hIcon;
			HINSTANCE hInst = AfxGetApp()->m_hInstance;

			hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( s_IconTable[i]), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

			m_pImageList->Add( hIcon);
		}
	}

	{
		m_LIST_Opt.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST_Opt.InsertColumn( 0, "Name", LVCFMT_LEFT, 400);
		m_LIST_Opt.InsertColumn( 1, "Config", LVCFMT_LEFT, 32);

		m_LIST_Opt.SetImageList( m_pImageList, LVSIL_SMALL);

		m_LIST_Opt.SetIconSpacing( CSize( 48, 48));
	}

	{
		_SetIcon( &m_BTN_Open,	IDI_FILE_OPEN);
		_SetIcon( &m_BTN_Save,	IDI_FILE_SAVE);
		_SetIcon( &m_BTN_SelAll, IDI_SELECT_ALL);
		_SetIcon( &m_BTN_Del,	IDI_DELETE);
		_SetIcon( &m_BTN_Run,	IDI_RUN);
		_SetIcon( &m_BTN_Stop,	IDI_STOP);
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3SceneOptimizer::static_meta(), I3_TDK_UPDATE_ALL);
	}

	EndProcessing();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Opt::OnOK()
{
}

void CPanel_Opt::OnCancel()
{
}

void CPanel_Opt::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST_Opt.m_hWnd))
	{
		CRect rt;

		m_LIST_Opt.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_LIST_Opt.SetWindowPos( NULL, 0, 0, cx - rt.left - 6, cy - rt.top - 6, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}
}

void	CPanel_Opt::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
	{
		if( pInfo->m_pObject != NULL)
		{
			AddOpt( (i3SceneOptimizer *) pInfo->m_pObject);
		}
		else
		{
			UpdateAllItem();
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		if( pInfo->m_pObject != NULL)
		{
			RemoveOpt( (i3SceneOptimizer *) pInfo->m_pObject);
		}
		else
		{
			UpdateAllItem();
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		UpdateAllItem();
	}
}

LRESULT CPanel_Opt::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanel_Opt::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( nIDCtl != IDC_LIST_OPTS)
	{
		i3TDKDialogBase::OnDrawItem( nIDCtl, lpDrawItemStruct);

		return;
	}

	HDC dc = lpDrawItemStruct->hDC;
	CRect rt = lpDrawItemStruct->rcItem;
	i3SceneOptimizer * pOpt = (i3SceneOptimizer *) lpDrawItemStruct->itemData;

	if( pOpt == NULL)
		return;

	COLORREF textColor;

	HBRUSH hOldBrush = (HBRUSH) ::SelectObject( dc, GetStockObject( NULL_BRUSH));
	HPEN hOldPen = (HPEN) ::SelectObject( dc, GetStockObject( WHITE_PEN));
	HFONT hOldFont = (HFONT) ::SelectObject( dc, m_hFontTitle);

	// Background
	if( lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		::FillRect( dc, &rt, g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_3D_LIT));

		::SelectObject( dc, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW));
		::Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);

		textColor = g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_HILIGHT);
	}
	else if( lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		::FillRect( dc, &rt, g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_3D_LIT_DARK));

		::SelectObject( dc, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW_DARK));
		::Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);

		textColor = g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_DISABLE);
	}
	else
	{
		::FillRect( dc, &rt, g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_3D_FACE));

		::SelectObject( dc, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW_DARK));
		::Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);

		textColor = g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_NORMAL);
	}

	// Icon
	{
		INT32 idxImage;
		CDC * pDC = CDC::FromHandle( dc);

		idxImage = pOpt->getClass();

		m_pImageList->DrawEx( pDC, idxImage, rt.TopLeft(), CSize( 32, 32), CLR_NONE, CLR_NONE, ILD_NORMAL);
	}

	// Name
	{
		CRect rt2;
		const char * pszName = pOpt->getDescName();

		rt2 = rt;
		rt2.left += 32 + 2;
		rt2.right -= 2;
		rt2.top += 2;
		rt2.bottom -= (rt.Height() >> 1);

		::SetBkMode( dc, TRANSPARENT);
		::SetTextColor( dc, textColor);
		::DrawText( dc, pszName, -1, &rt2, DT_VCENTER | DT_END_ELLIPSIS | DT_LEFT | DT_NOPREFIX);
	}

	// Config
	{
		CRect rt2;
		char conv[4096];
			
		m_LIST_Opt.GetItemText( lpDrawItemStruct->itemID, 1, conv, sizeof(conv));

		rt2 = rt;
		rt2.left += 32 + 2 + 10;
		rt2.right -= 2;
		rt2.top = rt.top + ((rt.Height() >> 1) + 1);
		rt2.bottom -= 2;

		::SetTextColor( dc, textColor);
		::SelectObject( dc, m_hFontConfig);
		::DrawText( dc, conv, -1, &rt2, DT_TOP | DT_END_ELLIPSIS | DT_LEFT | DT_NOPREFIX);
	}

	// Restore
	{
		::SelectObject( dc, hOldPen);
		::SelectObject( dc, hOldBrush);
		::SelectObject( dc, hOldFont);
	}
}

void CPanel_Opt::LoadConfig( const char * pszPath)
{
	i3XMLFile file;

	if( file.Load( pszPath) == false)
	{
		AfxMessageBox( "Could not open optimizer configuration file.", MB_OK);
		return;
	}

	COptConfig * pNewOptConfig = (COptConfig *) file.getKeyObject();

	if( !i3::same_of<COptConfig*>(pNewOptConfig)) //->IsExactTypeOf( COptConfig::static_meta()) == FALSE )
	{
		AfxMessageBox( "It is not valid optimizer configuration file.", MB_OK);
		return;
	}

	I3_REF_CHANGE( g_pOptConfig, pNewOptConfig);

	i3TDK::Update( NULL, I3_TDK_UPDATE_SCENE, NULL, i3SceneOptimizer::static_meta());

	i3::string_ncopy_nullpad( m_szConfigPath, pszPath, sizeof(m_szConfigPath));
}

void CPanel_Opt::OnBnClickedBtnOpenConfig()
{
	CFileDialog	Dlg( TRUE, "i3OptConfig", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"i3Engine Optimizer Configuration File(*.i3OptConfig)|*.i3OptConfig||", this);

	if( Dlg.DoModal() != IDOK)return;

	LoadConfig( LPCTSTR( Dlg.GetPathName()));
}

void CPanel_Opt::OnBnClickedBtnSaveConfig()
{
	if( m_szConfigPath[0] == 0)
	{
		CFileDialog	Dlg( FALSE, "i3OptConfig", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"i3Engine Optimizer Configuration File(*.i3OptConfig)|*.i3OptConfig||", this);

		if( Dlg.DoModal() != IDOK )
			return;

		i3::string_ncopy_nullpad( m_szConfigPath, LPCTSTR( Dlg.GetPathName()), sizeof(m_szConfigPath));
	}

	{
		i3XMLFile file;

		file.setKeyObject( g_pOptConfig);

		file.Save( m_szConfigPath);
	}
}

void CPanel_Opt::OnBnClickedBtnSelectall()
{
	INT32 i;

	for( i = 0; i < m_LIST_Opt.GetItemCount(); i++)
	{
		m_LIST_Opt.SetItemState( i, 0xFFFFFFFF, LVIS_SELECTED);
	}
}

void CPanel_Opt::OnBnClickedBtnDelete()
{
	
	i3::vector<i3SceneOptimizer*> list;

	for(INT32 i = 0; i < m_LIST_Opt.GetItemCount(); i++)
	{
		if( m_LIST_Opt.GetItemState( i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			i3SceneOptimizer * pOpt = (i3SceneOptimizer *) m_LIST_Opt.GetItemData( i);

			list.push_back( pOpt);
		}
	}

	for(size_t i = 0; i < list.size(); i++)
	{
		i3SceneOptimizer * pOpt = list[ i];

		g_pOptConfig->removeOpt( pOpt);
	}
}

void CPanel_Opt::OnBnClickedBtnRun()
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	I3ASSERT( m_pThread == NULL);

	pMainWnd->StartProcessing();
}

void CPanel_Opt::OnBnClickedBtnStop()
{
	if( m_pThread != NULL)
	{
		m_pThread->setExit( true);
	}
}

INT32	CPanel_Opt::getSelectedItem(void)
{
	POSITION pos = m_LIST_Opt.GetFirstSelectedItemPosition();

	if( pos != NULL)
	{
		return m_LIST_Opt.GetNextSelectedItem( pos);
	}

	return -1;
}

void CPanel_Opt::OnNMDblclkListOpts(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idxSel;

	idxSel = getSelectedItem();

	if( idxSel != -1)
	{
		i3SceneOptimizer * pOpt = g_pOptConfig->getOpt( idxSel);

		i3TDK::PopupOptimizerSpecDialog( pOpt, this);

		UpdateItem( idxSel, pOpt);

		Invalidate( FALSE);
	}

	*pResult = 0;
}

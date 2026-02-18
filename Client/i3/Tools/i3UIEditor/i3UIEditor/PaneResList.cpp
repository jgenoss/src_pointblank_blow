// PaneTextureList.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "GlobalVariable.h"
#include "PaneResList.h"
#include "SelectTextureDialog.h"
#include "SelectTemplateDialog.h"
#include "SingleSelectControlTypeDialog.h"
#include ".\panereslist.h"

// CPaneTextureList dialog

IMPLEMENT_DYNAMIC(CPaneResList, i3TDKDialogBase)
CPaneResList::CPaneResList(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneResList::IDD, pParent)
{
	m_pTextureList	= i3List::NewObject();
	m_pTemplate		= i3UIControlTemplate::NewObject();

	g_pResList		= this;
	m_pSelectedItemIdxList = NULL;
	m_iCountSelectedItem = 0;
}

CPaneResList::~CPaneResList()
{
	I3_SAFE_RELEASE( m_pTemplate );

	if( m_pTextureList != NULL)
	{
		INT32 nCount = m_pTextureList->GetCount();
		for( INT32 i = 0; i < nCount ; i++)
		{
			i3GuiToolImageElement * pElm = (i3GuiToolImageElement*)m_pTextureList->GetItem(i);
			i3Texture * pTex = pElm->getTexture();

			I3_SAFE_RELEASE( pTex);
			I3_SAFE_RELEASE( pElm );
		}

		m_pTextureList->Clear();
		I3_SAFE_RELEASE( m_pTextureList );
	}
}

void CPaneResList::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESLIST_TEXTURE, m_TextureListView);
	DDX_Control(pDX, IDC_RESLIST_TEMPLATE, m_TemplateListView);
}


BEGIN_MESSAGE_MAP(CPaneResList, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_RESLIST_TEXTURE, OnNMClickTextureList)
	ON_NOTIFY(NM_CLICK, IDC_RESLIST_TEMPLATE, OnNMClickTemplateList)
	ON_NOTIFY(NM_RCLICK, IDC_RESLIST_TEMPLATE, OnNMRclickReslistTemplate)
	ON_NOTIFY(NM_RCLICK, IDC_RESLIST_TEXTURE, OnNMRclickReslistTexture)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_RESLIST_TEMPLATE, OnLvnEndlabeleditReslistTemplate)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_RESLIST_TEXTURE, OnLvnEndlabeleditReslistTexture)
END_MESSAGE_MAP()

void	CPaneResList::_ResetContext()
{
	m_TextureListView.DeleteAllItems();
	m_TemplateListView.DeleteAllItems();
}

i3GuiToolImageElement *	CPaneResList::FindElementByTexture( i3Texture * pTexture)
{
	if( pTexture == NULL)	return NULL;

	INT32 i;
	INT32 nImageCount = m_pTextureList->GetCount();
	for( i = 0; i < nImageCount; ++i)
	{
		i3GuiToolImageElement * pElm = ( i3GuiToolImageElement*)m_pTextureList->GetItem( i);
		i3Texture			  * pTex = pElm->getTexture();

		if( i3String::Compare( pTexture->GetName(), pTex->GetName()) == 0)
		{
			return pElm;
		}
	}

	return NULL;
}

void CPaneResList::OnNMClickTextureList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_TextureListView.GetFirstSelectedItemPosition();
	if( pos != NULL)
	{
		INT32	idx = m_TextureListView.GetNextSelectedItem( pos );

		i3Texture * pData = (i3Texture*)m_TextureListView.GetItemData( idx );
		if( pData )
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, pData, i3Texture::GetClassMeta());
		}
	}

	*pResult = 0;
}

void CPaneResList::OnNMClickTemplateList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_TemplateListView.GetFirstSelectedItemPosition();
	if( pos != NULL)
	{
		INT32	idx = m_TemplateListView.GetNextSelectedItem( pos );

		i3UITemplate * pData = (i3UITemplate*)m_TemplateListView.GetItemData( idx );
		if( pData )
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, pData, i3UITemplate::GetClassMeta());
		}
	}

	*pResult = 0;
}

void CPaneResList::SetTextureList(i3List * pList)
{
	I3_REF_CHANGE( m_pTextureList, pList);

	m_TextureListView.DeleteAllItems();

	if( m_pTextureList != NULL )
	{
		INT32	nCount = m_pTextureList->GetCount();
		for( INT32	i = 0;i < nCount; ++i)
		{
			i3GuiToolImageElement * pElm = ( i3GuiToolImageElement *) m_pTextureList->GetItem(i);
			i3Texture * pTex = pElm->getTexture();
			AddTexture( pTex );
		}
	}
	else
	{
		m_pTextureList = i3List::NewObject();
	}
}

void CPaneResList::SetControlTemplate( i3UIControlTemplate * pTemplate)
{
	if( pTemplate != NULL )
	{
		pTemplate->AddRef();
	}

	I3_SAFE_RELEASE( m_pTemplate );

	m_pTemplate = pTemplate;

	m_TemplateListView.DeleteAllItems();

	if( m_pTemplate != NULL )
	{
		i3List * pTemplateList = m_pTemplate->getTemplateList();
		INT32	nCount = pTemplateList->GetCount();
		for( INT32	i = 0; i< nCount; ++i)
		{
			i3UITemplate * pElement = (i3UITemplate*)pTemplateList->GetItem(i);
			if( pElement != NULL )
			{
				AddTemplate( pElement );
			}
		}
	}
	else
	{
		m_pTemplate = i3UIControlTemplate::NewObject();
	}
}



////////////////////////////////////////////////////////////////////////////////////////////
// CPaneResList message handlers

BOOL CPaneResList::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	SetWindowText("Resource List");

	RECT rt;
	GetClientRect( &rt );

	m_ResTabCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 0);

	//List View Ctrl Init
	m_TextureListView.SetWindowText("Texture List");
//	m_TextureListView.InsertColumn( 0, "Number", LVCFMT_CENTER, 64);
	m_TextureListView.InsertColumn( 0, "Texture Name", LVCFMT_LEFT, 250);
	m_TextureListView.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_TemplateListView.SetWindowText("Template List");
	m_TemplateListView.InsertColumn( 0, "Template Name", LVCFMT_LEFT, 250);
	m_TemplateListView.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ResTabCtrl.AddPane( &m_TextureListView );
	m_ResTabCtrl.AddPane( &m_TemplateListView );

	//Update System Init
	i3TDK::RegisterUpdate( m_hWnd, i3UIImage::GetClassMeta(), I3_TDK_UPDATE_REQUEST);
	i3TDK::RegisterUpdate( m_hWnd, i3GameObjBase::GetClassMeta(), I3_TDK_UPDATE_REQUEST);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPaneResList::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_ResTabCtrl.m_hWnd))
	{
		m_ResTabCtrl.SetWindowPos( NULL, 0, 3, cx , cy -3 , SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPaneResList::_SelectTextureFromDialog(i3UIImage * pImage)
{
	if( pImage == NULL )	return ;

	CSelectTextureDialog Dlg;
	Dlg.SetTextureList( m_pTextureList );

	if( Dlg.DoModal() == IDOK)
	{
		if( Dlg.m_pSelectedTexture != NULL)
		{
			pImage->SetTexture( Dlg.m_pSelectedTexture );

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_RESLIST, pImage);
		}
	}
}

void CPaneResList::_SelectTemplateFromDialog(i3GameObjBase * pObj)
{
	if( pObj == NULL )	return ;
	if( pObj->IsTypeOf( i3UIControl::GetClassMeta()) == FALSE)	return ;

	CSelectTemplateDialog Dlg;
	Dlg.SetTemplate( m_pTemplate );

	if( Dlg.DoModal() == IDOK)
	{
		if( _IsValidTemplateElement(Dlg.m_pSelectedTemplate))
		{
			i3ClassMeta * pMeta = Dlg.m_pSelectedTemplate->GetMeta();

			i3UIControl * pCtrl = (i3UIControl*)pObj;

			i3UITemplate * pElement = (i3UITemplate*)CREATEINSTANCE( pMeta);

			Dlg.m_pSelectedTemplate->CopyTo( pElement, I3_COPY_INSTANCE);

			pCtrl->SetControlShapeTemplate( pElement );
			pCtrl->SetEnable( TRUE );

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST , pElement);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void	CPaneResList::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_REQUEST)
	{
		if( pInfo->m_Event & I3_TDK_USRWND_CTRLSHAPE)
		{
			if( pInfo->m_pMeta->IsExactTypeOf( i3UIImage::GetClassMeta()))
			{
				_SelectTextureFromDialog((i3UIImage*)pInfo->m_pObject);
			}
		}
		else if( pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH)
		{
			if( pInfo->m_pMeta->IsTypeOf(i3GameObjBase::GetClassMeta()))
			{
				_SelectTemplateFromDialog((i3GameObjBase*)pInfo->m_pObject);
			}
		}
	}
}

LRESULT CPaneResList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPaneResList::OnNMRclickReslistTemplate(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	i3UITemplate * pData = NULL;

	POSITION pos		= m_TemplateListView.GetFirstSelectedItemPosition();
	POSITION LastPos	= pos;

	m_iCountSelectedItem = m_TemplateListView.GetSelectedCount();


	if(m_pSelectedItemIdxList != NULL)
	{
		delete m_pSelectedItemIdxList;
		m_pSelectedItemIdxList= NULL;
	}

	if(m_iCountSelectedItem > 0)
	{
		m_pSelectedItemIdxList = new int [m_iCountSelectedItem];
	}

	//if( pos != NULL )
	int i = 0;
	while( pos != NULL )
	{
		INT32	idx = m_TemplateListView.GetNextSelectedItem( pos );

		pData = (i3UITemplate*)m_TemplateListView.GetItemData( idx );

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, pData);
		if(i < m_iCountSelectedItem)
		{
			m_pSelectedItemIdxList[i] = idx;
		}
		else
		{
			I3ASSERT_0;
			break;
		}
		i++;
	}
	
	_TrackPopupMenu( LastPos, i3UITemplate::GetClassMeta());

	*pResult = 0;
}

void CPaneResList::OnNMRclickReslistTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	i3Texture * pTex = NULL;
	
	POSITION pos = m_TextureListView.GetFirstSelectedItemPosition();

	POSITION LastPos = pos;

	m_iCountSelectedItem = m_TextureListView.GetSelectedCount();


	if(m_pSelectedItemIdxList != NULL)
	{
		delete m_pSelectedItemIdxList;
		m_pSelectedItemIdxList = NULL;
	}

	if(m_iCountSelectedItem > 0)
	{
		m_pSelectedItemIdxList = new int [m_iCountSelectedItem];
	}

	//if( pos != NULL )
	int i = 0;
	while( pos != NULL )
	{
		INT32	idx = m_TextureListView.GetNextSelectedItem( pos );

		pTex = (i3Texture*)m_TextureListView.GetItemData( idx );

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, pTex);
		if(i < m_iCountSelectedItem)
		{
			m_pSelectedItemIdxList[i] = idx;
		}
		else
		{
			I3ASSERT_0;
			break;
		}
		i++;
	}

	_TrackPopupMenu( LastPos, i3Texture::GetClassMeta());

	*pResult = 0;
}

void	CPaneResList::_TrackPopupMenu( POSITION pos, i3ClassMeta * pMeta)
{
	POINT pt;
	GetCursorPos(&pt);

	UINT32	cmd;
	CMenu Menu, * pPopup;
	Menu.LoadMenu( IDR_POPUP_RES );

	pPopup = Menu.GetSubMenu(0);

	{
		pPopup->EnableMenuItem( ID_IMPORT_TEMPLATE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_IMPORT_TEXTURE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_IMPORT_ANIMATION, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_IMPORT_SCENEGRAPH, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		
		pPopup->EnableMenuItem( ID_ADD_NEWTEMPLATE, MF_DISABLED | MF_BYCOMMAND  | MF_GRAYED);
		pPopup->EnableMenuItem( ID_IMPORT_TEXTURE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_IMPORT_TEMPLATE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_DELETE_RES, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}

	INT32 idx = -1;
	if( pos == NULL )
	{
		pPopup->EnableMenuItem( ID_ADD_NEWTEMPLATE, MF_ENABLED | MF_BYCOMMAND );
		pPopup->EnableMenuItem( ID_IMPORT_TEXTURE, MF_ENABLED | MF_BYCOMMAND);
		pPopup->EnableMenuItem( ID_IMPORT_TEMPLATE, MF_ENABLED | MF_BYCOMMAND);
	}
	else
	{
		pPopup->EnableMenuItem( ID_DELETE_RES, MF_ENABLED | MF_BYCOMMAND);

		if( pMeta->IsTypeOf( i3Texture::GetClassMeta()))
		{
			idx = m_TextureListView.GetNextSelectedItem( pos);
		}
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *)this, NULL);

	switch( cmd )
	{
	case ID_ADD_NEWTEMPLATE:	AddNewTemplate();	break;
	case ID_IMPORT_TEXTURE:		ImportTexture();	break;
	case ID_IMPORT_TEMPLATE:	ImportTemplate();	break;
	case ID_IMPORT_SCENEGRAPH:	break;
	case ID_IMPORT_ANIMATION:	break;
	case ID_DELETE_RES:		
		{
			int i = 0;
			for(i=0; i<m_iCountSelectedItem; i++)
			{
				_DeleteRes(m_pSelectedItemIdxList[0], m_pSelectedItemIdxList[i], pMeta);					
			}
		}
		break;
	default:
		break;
	}
}

//Texture
BOOL	CPaneResList::_CheckFileName( const char * pszFileName)
{
	INT32	nCount = m_pTextureList->GetCount();
	INT32	i;
	char	szRelFileName[MAX_PATH];

	i3String::MakeRelativePath( g_pProfile->m_szWorkDir, pszFileName, szRelFileName);

	for( i = 0;i < nCount; ++i)
	{
		i3GuiToolImageElement * pElm = ( i3GuiToolImageElement *)m_pTextureList->GetItem( i);
		i3Texture * pTex	= pElm->getTexture();

		if( pTex != NULL )
		{
			if(	i3String::Compare( szRelFileName, pTex->GetName()) == 0 )
			{
				AfxMessageBox( "같은 이름의 리소스가 존재합니다", MB_OK, 0);
				return FALSE;
			}
		}
	}

	return TRUE;
}

void	CPaneResList::_DeleteRes(INT32 FirstIdx, INT32 idx, i3ClassMeta * pMeta)
{
	//texture list와 template의 연관때문에 현재 봉인입니다.
	//이것이 가능하도록 하려면 template list모두를 검색하여 지우려는 텍스쳐를
	//사용하는 템플릿이 있는지 검색하여 지우기 가능한지 확인을 하던지
	//template를 저장하는 경우에도 texture를 저장하도록 하던지를 선택해야 합니다.
	/*
	{
		AfxMessageBox("현재 봉인된 기능입니다.");
		return ;
	}
	*/

	if( pMeta->IsTypeOf( i3Texture::GetClassMeta()))
	{
		i3GuiToolImageElement * pElm = ( i3GuiToolImageElement *) m_pTextureList->GetItem( idx);

		i3Texture * pTex = pElm->getTexture();
		
		INT32		nRefCount = pTex->GetRefCount();

 		if( nRefCount == 3)				//click 되었을때에 이미지가 uvedit에 셋팅되기 때문에
		{								//다른 어떤곳에서도 사용하고 있지 않다면 refcount = 3;
			m_pTextureList->Remove(pElm);
			m_TextureListView.DeleteItem(idx);
			m_TextureListView.UpdateData();
			I3_SAFE_RELEASE( pTex );
			I3_SAFE_RELEASE( pElm);
		}
		else if( nRefCount <= 2)
		{
			MessageBox("어딘가가 이상합니다.");
		}
		else
		{
			MessageBox("선택한 이미지는 다른곳에서 사용중입니다.");
		}
	}
	else if( pMeta->IsTypeOf( i3UITemplate::GetClassMeta()))
	{
		m_pTemplate->RemoveTemplate( FirstIdx );

		m_TemplateListView.DeleteItem( FirstIdx );

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, NULL, i3UITemplate::GetClassMeta());
	}
}

void	CPaneResList::ImportTexture()
{
	CFileDialog	Dlg( TRUE, "i3i", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3i File(*.i3i) |*.i3i|tga파일(*.tga)|*.tga|", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}
	
	Loadi3i( (LPCTSTR)Dlg.GetPathName() );
}

void CPaneResList::Loadi3i(const char * pszFileName)
{
	if( _CheckFileName(pszFileName) )
	{
		i3ImageFile file;

		i3Texture * pTexture = file.Load( pszFileName);
		if( pTexture != NULL)
		{
			char szFileName[MAX_PATH];

			i3String::MakeRelativePath( g_pProfile->m_szWorkDir, pszFileName, szFileName);
			i3String::ToUpper( szFileName);

			pTexture->SetName( szFileName);
			pTexture->SetExternRes();

			i3GuiToolImageElement * pElm = i3GuiToolImageElement::NewObject();
			pElm->SetTexture( pTexture);

			m_pTextureList->Add(pElm);

			AddTexture( pTexture );
		}
	}
	else 
	{
		//같은 이름의 파일이 이어서 처리한다. 메시지를 보여줘야 한다. 
	}
}

void	CPaneResList::AddTexture( i3Texture * pTexture )
{
	if( pTexture == NULL )	return;

	char szName[1024];
	INT32	ItemIndex;

//	sprintf( szName, "%d", m_TextureListView.GetItemCount());
//	ItemIndex = m_TextureListView.InsertItem( m_TextureListView.GetItemCount(),szName, 0);
	
	strcpy( szName, pTexture->GetName());
	if( szName[0] == 0 )	strcpy( szName, "No Named Texture");
	
	ItemIndex = m_TextureListView.InsertItem( m_TextureListView.GetItemCount(),szName, 0);
	//m_TextureListView.SetItemText( ItemIndex, 1, szName);
	m_TextureListView.SetItemData( ItemIndex, (DWORD_PTR)pTexture);
}

//Template
BOOL CPaneResList::_IsValidTemplateElement( i3UITemplate * pElement)
{
	if( pElement == NULL )
	{
		AfxMessageBox("선택된 템플릿이 없습니다");
		return FALSE;
	}

	//Check Valid Texture Image
	INT32	i;
	INT32	nCount = pElement->getShapeCount();
	for( i = 0; i< nCount ; ++i)
	{
		i3UIImage * pImage = (i3UIImage*)pElement->getShape(i);
		
		if( pImage == NULL )
		{
			AfxMessageBox("초기화가 잘못된 템플릿입니다. 지우고 새로 만드삼");
			return FALSE;
		}

		if( pImage->m_pTexture == NULL )
		{
			AfxMessageBox("Texture가 지정되지 않은 Shape가 존재합니다.");
			return FALSE;
		}
	}

	return TRUE;
}

static INT32 s_DefPos[9][2] = 
{
	{ 0, 0 },//Shape 0 
	{ 1, 0 },//Shape 1 
	{ 2, 0 },//Shape 2
	{ 0, 1 },//Shape 3 
	{ 2, 1 },//Shape 4 
	{ 0, 2 },//Shape 5 
	{ 1, 2 },//Shape 6 
	{ 2, 2 },//Shape 7 
	{ 1, 1 }//Shape 8 
};

static void _InitControlPosition( i3UITemplate * pElm)
{
	INT32 nCnt = pElm->getShapeCount();

	for( INT32 i = 0; i < nCnt; ++i)
	{
		i3UIImage * pImage = pElm->getShape( i);
		if( pImage != NULL)
		{
			pImage->m_nX		= (s_DefPos[i][0] * 100.0f);
			pImage->m_nY		= (s_DefPos[i][1] * 100.0f);
			pImage->m_nHeight	= 100.0f;
			pImage->m_nWidth	= 100.0f;
		}
	}
}

void CPaneResList::AddNewTemplate()
{
	CSingleSelectControlTypeDialog  SelDialog;
	SelDialog.setMeta( i3UITemplate::GetClassMeta());

	i3ClassMeta * pMeta = NULL;
	if( SelDialog.DoModal() == IDOK)
	{
		pMeta = i3ClassMeta::FindClassMetaByName( SelDialog.m_szSelectedMeta);
		if( pMeta == NULL )
		{
			char szDebugMsg[MAX_PATH];
			sprintf( szDebugMsg, "%s 이름의 Control Class를 찾을수 없습니다.", SelDialog.m_szSelectedMeta);
			MessageBox( szDebugMsg );
			return;
		}
	}
	else
		return;

	i3UITemplate * pElement = (i3UITemplate*)CREATEINSTANCE( pMeta);
	I3ASSERT( pElement != NULL);

	pElement->SetName("New Template");
	pElement->AddRef();

	if( AddTemplate( pElement ) == false)
	{
		I3_SAFE_RELEASE( pElement);
		return ;
	}

	m_pTemplate->AddTemplate( pElement);

	_InitControlPosition( pElement);
}

bool CPaneResList::AddTemplate( i3UITemplate * pElement)
{
	char szName[1024];
	INT32	ItemIndex;

	if( pElement->hasName() == false)
	{
		strcpy( szName, "New Template");
	}
	else
	{
		strcpy( szName, pElement->GetName());
	}

	LVFINDINFO info;
	i3mem::FillZero( &info, sizeof( info));
	info.flags = LVFI_STRING;
	info.psz = szName;

	//if( m_TemplateListView.FindItem( &info) == -1)
	{
		ItemIndex = m_TemplateListView.InsertItem( m_TemplateListView.GetItemCount(),szName, 0);

		m_TemplateListView.SetItemText( ItemIndex, 0, szName);
		m_TemplateListView.SetItemData( ItemIndex, (DWORD_PTR)pElement);
		return true;
	}

	//return false;
}


void CPaneResList::ExportTemplate()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( FALSE, "guitmp", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES ,  
		"I3GUI TemplateFile(*.GUITMP)| *.guitmp||", AfxGetMainWnd());

	if( Dlg.DoModal() != IDOK)
	{
		return ;
	}

//	m_pTemplate->RemoveRedundantTemplate();

	i3ResourceFile File;
	i3List list;

	INT32 i;
	for( i = 0; i < m_pTextureList->GetCount(); ++i)
	{
		i3GuiToolImageElement * pElm = ( i3GuiToolImageElement*)m_pTextureList->GetItem( i);
		i3Texture * pTex	= pElm->getTexture();
		I3ASSERT( pTex != NULL);

		BOOL bHasSameTex = FALSE;
		for( INT32 j = 0; j < list.GetCount(); ++j)
		{
			i3Texture * pTexture = (i3Texture*)list.GetItem( j);
			if( i3String::Compare( pTex->GetName(), pTexture->GetName()) == 0)
			{
				bHasSameTex = TRUE;
			}
		}

		if( ! bHasSameTex )
		{
			list.Add( pTex);
		}
	}

	list.Add( m_pTemplate);

	File.SetObjects( &list);

	if( File.Save( (LPCTSTR) Dlg.GetPathName()) == STREAM_ERR)
	{
		AfxMessageBox( "템플릿을 저장하는데 실패하였습니다", MB_OK, 0);
		return;
	}
}

void CPaneResList::ImportTemplate()
{
	I3ASSERT( m_pTemplate != NULL);

	CFileDialog	Dlg( TRUE, "guitmp", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3GUI TemplateFile(*.GUITMP)| *.guitmp||", AfxGetMainWnd());

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3ResourceFile File;
	i3List list;

	if( File.Load( (LPCTSTR) Dlg.GetPathName()) == STREAM_ERR)
	{
		AfxMessageBox( "템플릿을 읽어오는데 실패하였습니다.", MB_OK, 0);
		return ;
	}

	INT32 i;
	i3PersistantElement * pObj;
	INT32 nObjCount = File.GetObjectCount();
	
	for( i = 0; i < nObjCount; ++i)
	{
		pObj = File.GetObject( i);

		if( pObj->IsExactTypeOf( i3UIControlTemplate::GetClassMeta()))
		{
			i3UIControlTemplate * pAddTemplate = ( i3UIControlTemplate *)pObj;

			i3List * pDstList			= m_pTemplate->getTemplateList();
			i3List * pSrcTemplateList	= pAddTemplate->getTemplateList();

			
			INT32 nTemplateListCount = pSrcTemplateList->GetCount();
			for( INT32 i = 0; i< nTemplateListCount; ++i)
			{
				i3UITemplate * pElm = ( i3UITemplate*)pSrcTemplateList->GetItem( i);

				pDstList->Add( pElm);

				AddTemplate( pElm);
			}

			{//저장시에 template를 같이 저장했기 때문에 리스트만 넣어준경우 따로 refcount를 증가시켰다가 release해줘야한다.
				pAddTemplate->AddRef();
				pSrcTemplateList->Clear();

				I3_SAFE_RELEASE( pAddTemplate);
			}
		}
		else if( pObj->IsTypeOf( i3Texture::GetClassMeta()))
		{
			i3Texture * pTex = ( i3Texture*)pObj;
			BOOL	bIsInSameTex = FALSE;

			{
				for( INT32 i = 0; i < m_pTextureList->GetCount(); ++i)
				{
					i3GuiToolImageElement * pElm = (i3GuiToolImageElement*) m_pTextureList->GetItem( i);
					i3Texture * pTexture = pElm->getTexture();
					I3ASSERT( pTexture != NULL);

					if( i3String::Compare( pTexture->GetName(), pTex->GetName()) == 0)
					{
						I3TRACE("SameTexture is in List\n");
						bIsInSameTex = TRUE;
					}
				}
			}

			if( bIsInSameTex == FALSE)
			{
				i3GuiToolImageElement * pElm = i3GuiToolImageElement::NewObject();
				pElm->SetTexture( pTex);

				pTex->AddRef();
				m_pTextureList->Add(pElm);

				AddTexture( pTex);
			}
		}
	}
}

void CPaneResList::OnLvnEndlabeleditReslistTemplate(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	if( pDispInfo->item.pszText == NULL)
		return;

	if( pDispInfo->item.pszText == NULL)
	{
		// Cancel
		return;
	}

	// TODO: Add your control notification handler code here
	i3UITemplate * pElement = (i3UITemplate*)m_TemplateListView.GetItemData( pDispInfo->item.iItem);

	if( strlen( pDispInfo->item.pszText) > 0)
	{
		pElement->SetName( pDispInfo->item.pszText);

		m_TemplateListView.SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
	}

	//w
	*pResult = 0;
}

void CPaneResList::OnLvnEndlabeleditReslistTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if( pDispInfo->item.pszText == NULL)
		return;

	// TODO: Add your control notification handler code here
	i3Texture * pTex = (i3Texture *)m_TextureListView.GetItemData( pDispInfo->item.iItem);

	if( strlen( pDispInfo->item.pszText) > 0)
	{
		pTex->SetName( pDispInfo->item.pszText);

		m_TextureListView.SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
	}

	*pResult = 0;
}

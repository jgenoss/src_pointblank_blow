// PaneTextureList.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "GlobalVariable.h"
#include "PaneResList.h"
#include "SelectTextureDialog.h"
#include "SelectTemplateDialog.h"
#include ".\panereslist.h"
#include "i3Base/smart_ptr/make_shared.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/algorithm/to_upper.h"

// CPaneTextureList dialog

IMPLEMENT_DYNAMIC(CPaneResList, i3TDKDialogBase)
CPaneResList::CPaneResList(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneResList::IDD, pParent)
{
	m_pTextureList = i3::make_shared< i3::vector<i3GuiToolImageElement*> >(); 
	m_pTemplate		= i3GuiControlTemplate::new_object();

	g_pResList		= this;
	m_pSelectedItemIdxList = NULL;
	m_iCountSelectedItem = 0;
}

CPaneResList::~CPaneResList()
{
	I3_SAFE_RELEASE( m_pTemplate );

	if( m_pTextureList != NULL)
	{
		const size_t nCount = m_pTextureList->size();
		for( size_t i = 0; i < nCount ; i++)
		{
			i3GuiToolImageElement * pElm = (*m_pTextureList)[i];
			i3Texture * pTex = pElm->getTexture();

			I3_SAFE_RELEASE( pTex);
			I3_SAFE_RELEASE( pElm );
		}

		m_pTextureList->clear();
//		I3_SAFE_RELEASE( m_pTextureList );				// 소멸자에서는 스마트포인터의 경우 아무짓도 안해도 됨..
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

	const size_t nImageCount = m_pTextureList->size();
	for( size_t i = 0; i < nImageCount; ++i)
	{
		i3GuiToolImageElement * pElm = (*m_pTextureList)[i];
		i3Texture			  * pTex = pElm->getTexture();

		if( i3::generic_is_iequal( pTexture->GetName(), pTex->GetName()) )
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
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, pData, i3Texture::static_meta());
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

		i3GuiTemplateElement * pData = (i3GuiTemplateElement*)m_TemplateListView.GetItemData( idx );
		if( pData )
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, pData, i3GuiTemplateElement::static_meta());
		}
	}

	*pResult = 0;
}

void CPaneResList::SetTextureList(const i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >& pList)
{
//	I3_REF_CHANGE( m_pTextureList, pList);
	m_pTextureList = pList;

	m_TextureListView.DeleteAllItems();

	if( m_pTextureList != NULL )
	{
		const size_t	nCount = m_pTextureList->size();
		for( size_t	i = 0;i < nCount; ++i)
		{
			i3GuiToolImageElement * pElm = (*m_pTextureList)[i];
			i3Texture * pTex = pElm->getTexture();
			AddTexture( pTex );
		}
	}
	else
	{
		m_pTextureList = i3::make_shared<i3::vector<i3GuiToolImageElement*> >();
			// List::new_object();
	}
}

void CPaneResList::SetControlTemplate( i3GuiControlTemplate * pTemplate)
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
		i3::vector<i3GuiTemplateElement*>& TemplateList = m_pTemplate->getTemplateList();

		const size_t	nCount = TemplateList.size();
		for( size_t	i = 0; i< nCount; ++i)
		{
			i3GuiTemplateElement * pElement = (i3GuiTemplateElement*)TemplateList[i];
			if( pElement != NULL )
			{
				AddTemplate( pElement );
			}
		}
	}
	else
	{
		m_pTemplate = i3GuiControlTemplate::new_object();
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
	i3TDK::RegisterUpdate( m_hWnd, i3GuiImage::static_meta(), I3_TDK_UPDATE_REQUEST);
	i3TDK::RegisterUpdate( m_hWnd, i3GuiObjBase::static_meta(), I3_TDK_UPDATE_REQUEST);

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

void CPaneResList::_SelectTextureFromDialog(i3GuiImage * pImage)
{
	if( pImage == NULL )	return ;

	CSelectTextureDialog Dlg;
	Dlg.SetTextureList( m_pTextureList );

	if( Dlg.DoModal() == IDOK)
	{
		if( Dlg.m_pSelectedTexture != NULL)
		{
			if( pImage->GetTexture() == NULL)
			{
				i3Texture * pNewTex = Dlg.m_pSelectedTexture;

				pImage->m_u = pImage->m_v = 0;

				pImage->m_w = pNewTex->GetWidth();
				pImage->m_h = pNewTex->GetHeight();
			}

			pImage->SetTexture( Dlg.m_pSelectedTexture );

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_RESLIST, pImage);
		}
	}
}

void CPaneResList::_SelectTemplateFromDialog(i3GuiObjBase * pObj)
{
//	if( pObj == NULL )	return ;
	if( !i3::kind_of<i3GuiControl*>(pObj)) //->IsTypeOf( i3GuiControl::static_meta()) == FALSE)	
		return ;

	CSelectTemplateDialog Dlg;
	Dlg.SetTemplate( m_pTemplate );

	if( Dlg.DoModal() == IDOK)
	{
		if( _IsValidTemplateElement(Dlg.m_pSelectedTemplate))
		{
			i3GuiControl * pCtrl = (i3GuiControl*)pObj;

			i3GuiTemplateElement * pElement = i3GuiTemplateElement::new_object_ref();
			Dlg.m_pSelectedTemplate->CopyTo( pElement, I3_COPY_INSTANCE);

			pCtrl->SetControlShapeTemplate( pElement );
			pCtrl->SetEnable( TRUE );

			//i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE | I3_TDK_USRWND_RESLIST, g_pRoot->getSceneGraphNode());
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
			if( i3::same_of<i3GuiImage*>(pInfo->m_pMeta)) //->IsExactTypeOf( i3GuiImage::static_meta()))
			{
				_SelectTextureFromDialog((i3GuiImage*)pInfo->m_pObject);
			}
		}
		else if( pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH)
		{
			if( i3::kind_of<i3GuiObjBase*>(pInfo->m_pMeta)) //->IsTypeOf(i3GuiObjBase::static_meta()))
			{
				_SelectTemplateFromDialog((i3GuiObjBase*)pInfo->m_pObject);
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
	i3GuiTemplateElement * pData = NULL;

	POSITION pos		= m_TemplateListView.GetFirstSelectedItemPosition();
	POSITION LastPos	= pos;

	m_iCountSelectedItem = m_TemplateListView.GetSelectedCount();


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
		INT32	idx = m_TemplateListView.GetNextSelectedItem( pos );

		pData = (i3GuiTemplateElement*)m_TemplateListView.GetItemData( idx );

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
	
	_TrackPopupMenu( LastPos, i3GuiTemplateElement::static_meta());

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

	_TrackPopupMenu( LastPos, i3Texture::static_meta());

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

		pPopup->EnableMenuItem( ID_SET_FIXEDCOUNTSHAPETYPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_UNSET_FIXEDCOUNTSHAPETYPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
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

		if( i3::kind_of<i3Texture*>(pMeta)) //->IsTypeOf( i3Texture::static_meta()))
		{
			idx = m_TextureListView.GetNextSelectedItem( pos);
		}
		else if(  i3::kind_of<i3GuiTemplateElement*>(pMeta)) //->IsTypeOf( i3GuiTemplateElement::static_meta()))
		{
			idx = m_TemplateListView.GetNextSelectedItem( pos);

			i3::vector<i3GuiTemplateElement*>& List	= m_pTemplate->getTemplateList();
			i3GuiTemplateElement * pTempElm = List[idx];

			if( !pTempElm->isFixedCountShapeType())
			{
				pPopup->EnableMenuItem( ID_SET_FIXEDCOUNTSHAPETYPE, MF_ENABLED | MF_BYCOMMAND);
			}
			else
			{
				pPopup->EnableMenuItem( ID_UNSET_FIXEDCOUNTSHAPETYPE, MF_ENABLED | MF_BYCOMMAND);
			}
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
	case ID_SET_FIXEDCOUNTSHAPETYPE:		_SetFixedCountShapeType( TRUE, idx);	break;
	case ID_UNSET_FIXEDCOUNTSHAPETYPE:		_SetFixedCountShapeType( FALSE, idx);	break;
	default:
		break;
	}
}

void	CPaneResList::_SetFixedCountShapeType( BOOL bTrue, INT32 idx)
{
	i3GuiTemplateElement * pTempElm = m_pTemplate->GetTemplate( idx);

	pTempElm->setFixedCountShapeType( bTrue ? true : false);
}

//Texture
BOOL	CPaneResList::_CheckFileName( const char * pszFileName)
{
	const size_t	nCount = m_pTextureList->size();
	char	szRelFileName[MAX_PATH];

	i3::make_relative_path( g_pProfile->m_szWorkDir, pszFileName, szRelFileName );

	for( size_t i = 0;i < nCount; ++i)
	{
		i3GuiToolImageElement * pElm = (*m_pTextureList)[i];
		i3Texture * pTex	= pElm->getTexture();

		if( pTex != NULL )
		{
			if(	i3::generic_is_iequal( szRelFileName, pTex->GetName())  )
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

	if( i3::kind_of<i3Texture*>(pMeta)) //->IsTypeOf( i3Texture::static_meta()))
	{
		i3GuiToolImageElement * pElm = (*m_pTextureList)[idx];

		i3Texture * pTex = pElm->getTexture();
		
		INT32		nRefCount = pTex->GetRefCount();

 		if( nRefCount == 3)				//click 되었을때에 이미지가 uvedit에 셋팅되기 때문에
		{								//다른 어떤곳에서도 사용하고 있지 않다면 refcount = 3;
		//	m_pTextureList->Remove(pElm);
			i3::vu::remove(*m_pTextureList, pElm);
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
	else if( i3::kind_of<i3GuiTemplateElement*>(pMeta)) //->IsTypeOf( i3GuiTemplateElement::static_meta()))
	{
		m_pTemplate->RemoveTemplate( FirstIdx );

		m_TemplateListView.DeleteItem( FirstIdx );

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_RESLIST, NULL, i3GuiTemplateElement::static_meta());
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

			i3::make_relative_path( g_pProfile->m_szWorkDir, pszFileName, szFileName  );
			i3::to_upper( szFileName);

			pTexture->SetName( szFileName);
			pTexture->SetExternRes();

			i3GuiToolImageElement * pElm = i3GuiToolImageElement::new_object();
			pElm->SetTexture( pTexture);

			m_pTextureList->push_back(pElm);

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
BOOL CPaneResList::_IsValidTemplateElement( i3GuiTemplateElement * pElement)
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
		i3GuiImage * pImage = (i3GuiImage*)pElement->getImage(i);
		
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

static void _InitControlPosition( i3GuiTemplateElement * pElm)
{
	INT32 nCnt = pElm->getShapeCount();

	for( INT32 i = 0; i < nCnt; ++i)
	{
		i3GuiImage * pImage = pElm->getImage( i);
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
	i3GuiTemplateElement * pElement = i3GuiTemplateElement::new_object();

	pElement->SetName("New Template");

	if( AddTemplate( pElement ) == false)
	{
		I3_SAFE_RELEASE( pElement);
		return ;
	}

	m_pTemplate->AddTemplate( pElement, 9);

	_InitControlPosition( pElement);
}

bool CPaneResList::AddTemplate( i3GuiTemplateElement * pElement)
{
	char szName[1024],szConv[1024];
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

	INT32 nT = 0;
	while( m_TemplateListView.FindItem( &info) != -1)
	{
		sprintf( szConv, "%s_%d", szName, nT);

		info.psz = szConv;

		nT++;
	}

	if( nT > 0)
	{
		strcpy( szName, szConv);
	}

	ItemIndex = m_TemplateListView.InsertItem( m_TemplateListView.GetItemCount(),szName, 0);

	m_TemplateListView.SetItemText( ItemIndex, 0, szName);
	m_TemplateListView.SetItemData( ItemIndex, (DWORD_PTR)pElement);

	return true;
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

	m_pTemplate->RemoveRedundantTemplate();

	i3ResourceFile File;
	i3::vector<i3PersistantElement*> list;

	
	for(size_t i = 0; i < m_pTextureList->size(); ++i)
	{
		i3GuiToolImageElement * pElm = (*m_pTextureList)[i];
		i3Texture * pTex	= pElm->getTexture();
		I3ASSERT( pTex != NULL);

		BOOL bHasSameTex = FALSE;
		for( size_t j = 0; j < list.size(); ++j)
		{
			i3Texture * pTexture = (i3Texture*)list[j];
			if( i3::generic_is_iequal( pTex->GetName(), pTexture->GetName()) )
			{
				bHasSameTex = TRUE;
			}
		}

		if( ! bHasSameTex )
		{
			list.push_back( pTex);
		}
	}

	list.push_back( m_pTemplate);

	File.SetObjects( list);

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
//	List list;					// 쓰이지 않는듯 해서 주석..

	if( File.Load( (LPCTSTR) Dlg.GetPathName()) == STREAM_ERR)
	{
		AfxMessageBox( "템플릿을 읽어오는데 실패하였습니다.", MB_OK, 0);
		return ;
	}

	INT32 i;
	i3PersistantElement * pObj;
	INT32 nObjCount = File.GetObjectCount();
	
	INT32 A = 0;
	for( i = 0; i < nObjCount; ++i)
	{
		pObj = File.GetObject( i);

		if( i3::same_of<i3GuiControlTemplate*>(pObj)) //->IsExactTypeOf( i3GuiControlTemplate::static_meta()))
		{
			i3GuiControlTemplate * pAddTemplate = ( i3GuiControlTemplate *)pObj;

			i3::vector<i3GuiTemplateElement*>& DstList			= m_pTemplate->getTemplateList();
			i3::vector<i3GuiTemplateElement*>& SrcTemplateList	= pAddTemplate->getTemplateList();

			
			const size_t nTemplateListCount = SrcTemplateList.size();
			for( size_t j = 0; j< nTemplateListCount; ++j)
			{
				i3GuiTemplateElement * pElm = SrcTemplateList[j];

				DstList.push_back( pElm);

				if( AddTemplate( pElm))
				{
					A++;
				}
			}

			{//저장시에 template를 같이 저장했기 때문에 리스트만 넣어준경우 따로 refcount를 증가시켰다가 release해줘야한다.
				pAddTemplate->AddRef();
				SrcTemplateList.clear();

				I3_SAFE_RELEASE( pAddTemplate);
			}
		}
		else if( i3::kind_of<i3Texture*>(pObj)) //->IsTypeOf( i3Texture::static_meta()))
		{
			i3Texture * pTex = ( i3Texture*)pObj;
			BOOL	bIsInSameTex = FALSE;

			{
				for( size_t j = 0; j < m_pTextureList->size(); ++j)
				{
					i3GuiToolImageElement * pElm = (*m_pTextureList)[j];
					i3Texture * pTexture = pElm->getTexture();
					I3ASSERT( pTexture != NULL);

					if( i3::generic_is_iequal( pTexture->GetName(), pTex->GetName()) )
					{
						I3TRACE("SameTexture is in List\n");
						bIsInSameTex = TRUE;
					}
				}
			}

			if( bIsInSameTex == FALSE)
			{
				i3GuiToolImageElement * pElm = i3GuiToolImageElement::new_object();
				pElm->SetTexture( pTex);

				pTex->AddRef();
				m_pTextureList->push_back(pElm);

				AddTexture( pTex);
			}
		}
	}

	I3TRACE("TEMPLATE COUNT: %d\n", A);
}

void CPaneResList::OnLvnEndlabeleditReslistTemplate(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if( pDispInfo->item.pszText != NULL)
	{
		// TODO: Add your control notification handler code here
		i3GuiTemplateElement * pElement = (i3GuiTemplateElement*)m_TemplateListView.GetItemData( pDispInfo->item.iItem);
		

		pElement->SetName( pDispInfo->item.pszText);

		m_TemplateListView.SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
	}

	//w
	*pResult = 0;
}

void CPaneResList::OnLvnEndlabeleditReslistTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if( pDispInfo->item.pszText != NULL)
	{
		// TODO: Add your control notification handler code here
		i3Texture * pTex = (i3Texture *)m_TextureListView.GetItemData( pDispInfo->item.iItem);

		pTex->SetName( pDispInfo->item.pszText);

		m_TextureListView.SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
	}

	*pResult = 0;
}

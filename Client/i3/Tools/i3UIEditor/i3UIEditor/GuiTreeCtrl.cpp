#include "stdafx.h"
#include "GlobalVariable.h"
#include "MainFrm.h"
#include "GuiTreeCtrl.h"
#include "resource.h"
#include "GuiToolProject.h"
#include "SingleSelectControlTypeDialog.h"
#include "GetStringDlg.h"
#include "DlgAddControl.h"

enum
{
	I3GUI_ICON_WINDOW		= 0,
	I3GUI_ICON_SOUND		= 2,
	I3GUI_ICON_ANIM			= 4,
	I3GUI_ICON_SCENEGRAPH   = 6,
	I3GUI_ICON_TEXTURE		= 8,
	I3GUI_ICON_CONTROL		= 10,	
	I3GUI_ICON_GAMENODE		= 12,
	I3GUI_ICON_TEXT			= 14,
	I3GUI_ICON_RESOURCE		= 16,
};

IMPLEMENT_DYNAMIC( CGuiTreeCtrl, CWnd)

CGuiTreeCtrl::CGuiTreeCtrl(void)
{
	m_pRoot = NULL;
	m_pImageList = NULL;
	m_dwStyle = NULL;
	m_ShowStyle = I3GUI_GUITREE_SHOW_NAME | I3GUI_GUITREE_SHOW_FLAGS | I3GUI_GUITREE_SHOW_ATTRS;

	m_hSelected = NULL;
	m_szSearchString[0] = 0;
}

CGuiTreeCtrl::~CGuiTreeCtrl(void)
{
	if( m_pImageList != NULL )
	{
		delete m_pImageList;
		m_pImageList = NULL;
	}
}

BOOL CGuiTreeCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH	hBack;

	m_dwStyle = dwStyle;

	hCursor = LoadCursor( NULL, IDC_ARROW);
	I3ASSERT( hCursor != NULL);

	hBack = (HBRUSH)GetStockObject( NULL_BRUSH);
	I3ASSERT( hBack != NULL );

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, NULL);
	I3ASSERT( pszClass != NULL );

	dwStyle |= WS_CLIPCHILDREN;

	return CWnd::Create( pszClass, "", dwStyle, rect, pParentWnd, nID, NULL);
}

//Tree에 표시될 이름을 설정하는 부분입니다.
void CGuiTreeCtrl::MakeItemName( char * pszStr, i3GameNode * pNode )
{
	I3ASSERT( pszStr != NULL);
	I3ASSERT( pNode != NULL);
	char szTemp[MAX_PATH];

	i3ClassMeta * pMeta = pNode->GetMeta();

	{
		sprintf( szTemp, "%s", pMeta->GetClassName());

		if( i3String::NCompare( "i3UI", szTemp, 5) == 0)
		{
			if( pNode->hasName() == false)
			{
				sprintf( pszStr, "[%s]",  szTemp);
			}
			else
			{
				sprintf( pszStr, "[%s]: %s",  szTemp + 5, pNode->GetName());
			}
		}
		else
		{
			if( pNode->hasName() == false)
			{
				sprintf( pszStr, "[%s]",  szTemp);
			}
			else
			{
				sprintf( pszStr, "[%s]: %s",  szTemp, pNode->GetName());
			}
		}
	}
}

INT32	CGuiTreeCtrl::GetChildItemCount( i3GameNode * pNode)
{
	INT32	c = 0;

	if( pNode == NULL )	return 0;

	i3GameNode * pChildNode = pNode->getFirstChild();
	while( pChildNode != NULL )
	{
		c++;
		pChildNode = pChildNode->getNext();
	}

	return c;
}

HTREEITEM CGuiTreeCtrl::AddTreeItem( i3GameNode * pObj, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	INT32 ImageIndex	= m_nIndexOfFirstTDKImage;
	TVINSERTSTRUCT info;
	HTREEITEM	hItem;

	char szTreeName[MAX_PATH];
	MakeItemName( szTreeName, pObj);

	info.hInsertAfter	= hInsertAfter;
	info.hParent		= hParent;
	info.item.mask		= TVIF_CHILDREN | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	info.item.hItem		= 0;
	info.item.lParam	= (LPARAM)pObj;
	info.item.pszText	= szTreeName;

	if( pObj->IsTypeOf( i3UIManager::GetClassMeta()))
	{
		ImageIndex += I3GUI_ICON_GAMENODE;
	}
	else if( pObj->IsTypeOf( i3UIFrameWnd::GetClassMeta()))
	{
		ImageIndex += I3GUI_ICON_WINDOW;
	}
	else if( pObj->IsTypeOf( i3UIControl::GetClassMeta()))
	{
		ImageIndex += I3GUI_ICON_CONTROL;
	}
	else
	{
		ImageIndex += I3GUI_ICON_RESOURCE;
	}

	info.item.cChildren = GetChildItemCount(pObj);

	info.item.iImage = ImageIndex + 1;
	info.item.iSelectedImage = ImageIndex;

	hItem = m_TreeCtrl.InsertItem( &info );

	if((hItem != NULL) && (hParent != TVI_ROOT))
	{
		TVITEM item;

		item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_PARAM;
		item.hItem = hParent;

		m_TreeCtrl.GetItem( &item );

		item.cChildren = 1;

		m_TreeCtrl.SetItem( &item);
	}

	return hItem;
}

//
//구조 바뀌어야 합니다. control도 child를 가질수 있습니다.
//

void CGuiTreeCtrl::Rec_SetTree( HTREEITEM hParentItem, i3GameNode * pNode, HTREEITEM hInsertAfter)
{
	if( pNode == NULL )
	{
		return ;
	}

	HTREEITEM	hItem;

	//
	hItem = AddTreeItem( pNode,hParentItem, hInsertAfter);

	//자식노드
	i3GameNode * pChildNode = pNode->getFirstChild();
	if( pChildNode != NULL )
	{
		Rec_SetTree( hItem, pChildNode);
	}

	i3GameNode * pNext = pNode->getNext();
	if( pNext != NULL )
	{
		Rec_SetTree( hParentItem, pNext);
	}
}

void	CGuiTreeCtrl::SetGuiGraph( i3GameNode * pRoot )
{
	m_TreeCtrl.DeleteAllItems();

	m_pRoot = pRoot;

	HTREEITEM hItem;

	hItem = AddTreeItem(pRoot,TVI_ROOT);

	i3GameNode * pFirstChild = pRoot->getFirstChild();
	if( pFirstChild != NULL )
	{
		Rec_SetTree( hItem, pFirstChild );
	}
}

/////////
static BOOL _GetCheckedItemProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3List * pList = (i3List *) pData;

	if( pCtrl->GetCheck( hItem))
	{
		pList->Add( (void *) hItem);
	}

	return TRUE;
}

/////////
static BOOL _GetSelectedItemProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3List * pList = (i3List *) pData;

	if( pCtrl->GetItemState( hItem, 0xFFFFFFFF) & TVIS_SELECTED)
	{
		pList->Add( (void *) hItem);
	}

	return TRUE;
}

void CGuiTreeCtrl::GetCheckedItem( i3List * pList)
{
	if( m_TreeCtrl.GetStyle() & TVS_CHECKBOXES)
	{
		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _GetCheckedItemProc, pList);
	}

	if( pList->GetCount() <= 0)
	{
		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _GetSelectedItemProc, pList);
	}
}

/////////
static BOOL _ExpandProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	if( pData == NULL)
		pCtrl->Expand( hItem, TVE_EXPAND);
	else
		pCtrl->Expand( hItem, TVE_COLLAPSE);

	return TRUE;
}

void CGuiTreeCtrl::Expand( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _ExpandProc, NULL, hItem);
}

void CGuiTreeCtrl::Collapse( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _ExpandProc, (void *) 1, hItem);
}


/////////////////////////////////////////////////////////////////////////////////////////
static bool s_isValidSearch = false;
static bool s_isNextSearch = false;
static bool s_FindExactlySame = false;
static BOOL _FindProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	char * pszFindName = (char*)pData;

	HTREEITEM hSel = pCtrl->GetSelectedItem();
	if( s_isNextSearch  && (hSel == hItem))
	{
		s_isValidSearch = true;
		return TRUE;
	}

	if( !s_isValidSearch )
		return TRUE;

	i3GameNode * pNode = (i3GameNode*)pCtrl->GetItemData( hItem);
	if((pNode == NULL) || (pNode->hasName() == false))
		return TRUE;

	if( s_FindExactlySame )
	{
		if( i3String::Compare( pNode->GetName(), pszFindName) == 0)
		{
			pCtrl->Select( hItem, TVGN_CARET);
			return FALSE;
		}
	}
	else
	{
		if( i3String::Contain( pNode->GetName(), pszFindName) != -1)
		{
			pCtrl->Select( hItem, TVGN_CARET);
			return FALSE;
		}
	}

	return TRUE;
}

void CGuiTreeCtrl::OnFindNode(HTREEITEM hItem)
{
	// TODO: Add your command handler code here
	HTREEITEM hCur = m_hSelected;

	CGetStringDlg dlg;
	if( dlg.DoModal() == IDOK)
	{
		s_isValidSearch = true;
		s_isNextSearch = false;
		s_FindExactlySame = dlg.m_bSearchExactlySame;

		i3String::Copy( m_szSearchString, dlg.m_str);
		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _FindProc, (void*)m_szSearchString, TVI_ROOT);

		if( hCur != m_hSelected)
		{
			if( IsValidItem( m_hSelected))
			{
				i3GameNode * pObj = (i3GameNode*)m_TreeCtrl.GetItemData( m_hSelected);
				I3ASSERT( pObj != NULL);

				i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_GAMEGRAPH, (i3GameObjBase*)pObj);
			}
		}
		else
		{
			MessageBox("찾기 종료","알림");
		}

		SetFocus();
	}
}

void CGuiTreeCtrl::OnFindNext()
{
	HTREEITEM hCur = m_hSelected;

	if( m_szSearchString[0] != '/0')
	{
		s_isValidSearch = false;
		s_isNextSearch = true;

		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _FindProc, (void*)m_szSearchString, TVI_ROOT);

		if( hCur != m_hSelected)
		{
			if( IsValidItem( m_hSelected))
			{
				i3GameNode * pObj = (i3GameNode*)m_TreeCtrl.GetItemData( m_hSelected);
				I3ASSERT( pObj != NULL);

				i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_GAMEGRAPH, (i3GameObjBase*)pObj);
			}

			SetFocus();
		}
		else
		{
			MessageBox("찾기 종료","알림");
		}
	}
}

typedef struct _tagCheckNameCRC
{
	UINT32 crc;
	i3GameNode * pNode;
}GUI_SEARCH;

static INT32	_NameCompareProc( GUI_SEARCH * p1, GUI_SEARCH * p2)
{
	if( p1->crc > p2->crc)
		return 1;
	else if( p1->crc < p2->crc)
		return -1;
	else
		return 0;
}

static INT32	_NameFindProc( GUI_SEARCH * p1, UINT32 crc)
{
	if( p1->crc > crc)
		return 1;
	else if( crc > p1->crc)
		return -1;
	else
		return 0;
}

static BOOL _CheckMnemonicsProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3BinList * pList = (i3BinList *)pData;

	i3GameNode * pNode = (i3GameNode*)pCtrl->GetItemData( hItem);
	if((pNode == NULL) || (pNode->hasName() == false))
	{
		return TRUE;
	}

	GUI_SEARCH * p = (GUI_SEARCH*)i3MemAlloc( sizeof( GUI_SEARCH));

	p->crc = CRC32( 0xFFFFFFFF, (UINT8 *) pNode->GetName(), strlen( pNode->GetName()));
	p->pNode = pNode;

	if( pList->FindItem( (void*)p->crc) == -1)
	{
		pList->SetCompareProc( (COMPAREPROC)_NameCompareProc);

		pList->Add( p);

		pList->SetCompareProc( (COMPAREPROC)_NameFindProc);
	}
	else
	{
		char conv[MAX_PATH];
		sprintf( conv, "중복된 이름이 존재합니다.[%s]", p->pNode->GetName());
		MessageBox( NULL, conv, "알림", MB_OK);

		pCtrl->Select( hItem, TVGN_CARET);
		return FALSE;
	}

	return TRUE;
}

void CGuiTreeCtrl::CheckMnemonics(void)
{
	i3BinList list;
	HTREEITEM hCur = m_hSelected;

	list.SetCompareProc( (COMPAREPROC)_NameFindProc);
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _CheckMnemonicsProc, (void*)&list, TVI_ROOT);

	if( hCur != m_hSelected)
	{
		if( IsValidItem( m_hSelected))
		{
			i3GameNode * pObj = (i3GameNode*)m_TreeCtrl.GetItemData( m_hSelected);
			I3ASSERT( pObj != NULL);

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_GAMEGRAPH, (i3GameObjBase*)pObj);
		}

		SetFocus();
	}

	for( INT32 i = 0;i < list.GetCount(); ++i)
	{
		GUI_SEARCH * ps = (GUI_SEARCH*)list.GetItem( i);

		I3MEM_SAFE_FREE( ps);
	}

	list.Clear();
}

/////////
static BOOL _SelectAllProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	switch( (INT32) pData)
	{
		case 0 :
			if( pCtrl->GetCheck( hItem) == FALSE)
			{
				pCtrl->SetCheck( hItem, TRUE);
			}
			break;

		case 1 :
			if( pCtrl->GetCheck( hItem) == TRUE)
			{
				pCtrl->SetCheck( hItem, FALSE);
			}
			break;

		case 2 :
			pCtrl->SetCheck( hItem, ! pCtrl->GetCheck( hItem));
			break;
	}

	return TRUE;
}

void CGuiTreeCtrl::SelectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, NULL);
}

void CGuiTreeCtrl::UnselectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 1);
}

void CGuiTreeCtrl::ReverseSelection(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 2);
}

void CGuiTreeCtrl::SelectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, NULL, hItem);
}

void CGuiTreeCtrl::UnselectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 1, hItem);
}

BOOL CGuiTreeCtrl::IsValidItem( HTREEITEM hItem)
{
	TVITEM data;

	data.mask	= TVIF_HANDLE;
	data.hItem	= hItem;

	return m_TreeCtrl.GetItem( &data);
}

void CGuiTreeCtrl::SetTemplate( HTREEITEM hItem )
{
	if( IsValidItem( hItem))
	{
		i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hItem);			
		I3ASSERT( pObj != NULL);
		
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REQUEST | I3_TDK_USRWND_GAMEGRAPH, (i3GameObjBase*)pObj);
	}
}

void CGuiTreeCtrl::SetCurrentRootNode( HTREEITEM hItem)
{
	if( IsValidItem( hItem))
	{
		i3GameNode * pObj = (i3GameNode*)m_TreeCtrl.GetItemData( hItem);
		I3ASSERT( pObj != NULL);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_GAMEGRAPH, (i3GameObjBase*)pObj);
	}
}

void CGuiTreeCtrl::Delete( HTREEITEM hItem)
{
	i3List SelList;
	INT32	i;

	GetCheckedItem( &SelList);

	for( i = 0; i < SelList.GetCount(); i++)
	{
		hItem = (HTREEITEM)SelList.GetItem(i);

		if( IsValidItem( hItem))
		{
			
			i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hItem);			
			I3ASSERT( pObj != NULL);
			
			if( pObj->IsTypeOf( i3UIManager::GetClassMeta()))
			{
				AfxMessageBox("제거할수 없는 노드입니다.");
				return ;
			}

			i3GameNode * pParent = pObj->getParent();

			if( pParent->IsTypeOf( i3UIManager::GetClassMeta()))
			{
				//i3UIManager * pManager = (i3UIManager *) pParent;

				//pManager->RemoveF
				//pParent->RemoveControl( pObj);
			}
			else
			{
				i3UIControl * pParentCtrl = (i3UIControl *) pParent;

				pParentCtrl->RemoveControl( (i3UIControl *) pObj);
			}

			m_TreeCtrl.DeleteItem( hItem );			
			m_TreeCtrl.UpdateData( TRUE);

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE | I3_TDK_USRWND_GAMEGRAPH, NULL, i3GameObjBase::GetClassMeta());
		}
	}

	//
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE | I3_TDK_USRWND_GAMEGRAPH, i3UI::getManager());
}

void CGuiTreeCtrl::ExportNode(HTREEITEM hItem)
{
	i3ResourceFile file;
	CString FileName; 
	CFileDialog	Dlg( FALSE, "guiNode", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3UI NodeFile(*.guiNode)|*.guiNode||", this);

	if( Dlg.DoModal() != IDOK )	return; 

	FileName = Dlg.GetFileName(); 

	i3GameNode * pNode = ( i3GameNode*)m_TreeCtrl.GetItemData( hItem);

	if( pNode == NULL )
	{
		MessageBox("저장할 내용이 없습니다.");
		return;
	}

	file.setKeyObject( pNode);

	if( file.Save( (char*)LPCTSTR(FileName) ) == STREAM_ERR)
	{
		AfxMessageBox( "프로젝트를 저장할 수 없습니다.");
		return;
	}
}

void CGuiTreeCtrl::ImportNode(HTREEITEM hItem)
{
	i3ResourceFile file;

	CString FileName; 
	CFileDialog	Dlg( TRUE, "guiNode", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3UI NodeFile(*.guiNode)|*.guiNode||", this);

	if( Dlg.DoModal() != IDOK)return;

	FileName = Dlg.GetFileName();

	i3GameNode * pGameNode = NULL;

	if( file.Load(  (char*)LPCTSTR(FileName)) == STREAM_ERR)
	{
		AfxMessageBox("couldn't read project");
		return;
	}

	pGameNode = (i3GameNode*)file.getKeyObject();

	if( pGameNode == NULL)
	{
		AfxMessageBox("Not a Valid GuiTool Project File");
		return;
	}

	i3GameNode * pParentNode = ( i3GameNode*)m_TreeCtrl.GetItemData( hItem);
	pParentNode->AddChild( pGameNode);


	if( pGameNode->IsTypeOf( i3GameNode::GetClassMeta()))
	{
		Rec_SetTree( hItem, pGameNode);
	}
}


void CGuiTreeCtrl::InsertControl( HTREEITEM hItem )
{
	if( IsValidItem( hItem ))
	{
		CDlgAddControl dlg;
		
		dlg.DoModal();

		/*
		if( SelDialog.DoModal() == IDOK)
		{
			i3GameNode * pParentNode = ( i3GameNode*)m_TreeCtrl.GetItemData( hItem);

			i3UIControl * pNewNode;
			i3ClassMeta * pMeta;
			pMeta = i3ClassMeta::FindClassMetaByName( SelDialog.m_szSelectedMeta);
			if( pMeta == NULL )
			{
				char szDebugMsg[MAX_PATH];
				sprintf( szDebugMsg, "%s 이름의 Control Class를 찾을수 없습니다.", SelDialog.m_szSelectedMeta);
				MessageBox( szDebugMsg );
				return;
			}

			if(pParentNode->IsTypeOf( i3UIManager::GetClassMeta()) == TRUE)
			{
				if( pMeta->IsTypeOf( i3UIFrameWnd::GetClassMeta()) == FALSE)
				{
					MessageBox( "해당위치에는 FrameWindow 만 삽입할수 있습니다.");
					return ;
				}
			}

			//i3GameObjBase * pParentObj = (i3GameObjBase*)pParentNode;

			if( pMeta->IsTypeOf( i3GameObjBase::GetClassMeta()))
			{
				pNewNode = (i3UIControl*)CREATEINSTANCE( pMeta );

				if( pNewNode != NULL )
				{
					pNewNode->setSize( 100, 100);

					Rec_SetTree( hItem, pNewNode);

					if( pParentNode->IsTypeOf( i3UIManager::GetClassMeta()))
					{
						i3UIManager * pManager = (i3UIManager *) pParentNode;

						pManager->AddFrameWnd( (i3UIFrameWnd *) pNewNode);
					}
					else if( pParentNode->IsTypeOf( i3UIControl::GetClassMeta()))
					{
						i3UIControl * pParentCtrl = (i3UIControl *) pParentNode;

						pParentCtrl->AddControl( (i3UIControl *) pNewNode);
					}
					else
					{
						I3ASSERT_0;
					}
				}
			}
		}
		*/
	}
}

void CGuiTreeCtrl::_Rec_CreateClone( i3GameObjBase * pSrcNode, i3GameObjBase * pDstNode, I3_COPY_METHOD method)
{
	i3GameObjBase * pNew = (i3GameObjBase*)CREATEINSTANCE( pSrcNode->GetMeta());

	if( pNew != NULL )
	{
		pSrcNode->CopyTo( pNew, method);

		pDstNode->AddChild( pNew );
	}

	i3GameObjBase * pChild = (i3GameObjBase*)pSrcNode->getFirstChild();
	if( pChild != NULL)
	{
		_Rec_CreateClone( pChild, pNew, I3_COPY_INSTANCE);
	}

	i3GameObjBase * pNext = (i3GameObjBase*)pSrcNode->getNext();
	if( pNext != NULL )
	{
		_Rec_CreateClone( pNext, pDstNode, I3_COPY_INSTANCE);
	}
}

i3GameObjBase * CGuiTreeCtrl::_CreateClone( i3GameObjBase * pSrcNode, I3_COPY_METHOD method)
{
	i3ClassMeta * pMeta = pSrcNode->GetMeta();

	i3GameObjBase * pNew = (i3GameObjBase*)CREATEINSTANCE( pMeta );

	if( pNew != NULL )
	{
		pSrcNode->CopyTo( pNew, method);

		i3GameObjBase * pChild = (i3GameObjBase*)pSrcNode->getFirstChild();
		if( pChild != NULL)
		{
			_Rec_CreateClone( pChild , pNew, I3_COPY_INSTANCE );
		}
	}

	return pNew;
}

//
void CGuiTreeCtrl::DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone)
{
	i3GameNode * pSrcObj, * pTargetObj;

	if( IsValidItem( hSrcItem ) )
	{
		pSrcObj = (i3GameNode *) m_TreeCtrl.GetItemData( hSrcItem);
	}
	else
		return ;

	if( IsValidItem( hTargetItem ))
	{
		pTargetObj = (i3GameNode *) m_TreeCtrl.GetItemData( hTargetItem);
	}
	else
		return ;

	//원본과 타겟이 같은경우 이동또는 카피 불가
	if( pSrcObj == pTargetObj )
	{
		return;
	}

	//타겟이 이미 자신의 부모인경우 불가
	if( !bClone && pTargetObj == pSrcObj->getParent())
	{
		return ;
	}

	//타겟의 위치가 원본의 자식 노드인경우 이동 불가
	if( bClone == FALSE )
	{
		i3GameNode * pParent = pTargetObj->getParent();
		while( pParent != NULL )
		{
			if( pSrcObj == pParent )
			{
				return;
			}

			pParent = pParent->getParent();
		}
	}

	if( pSrcObj->IsTypeOf( i3GameObjBase::GetClassMeta()))
	{
		i3GameObjBase * pSrcNode = (i3GameObjBase *) pSrcObj;

		if( bClone)
		{
			pSrcNode = _CreateClone( pSrcNode, I3_COPY_INSTANCE);
		}

		{//switch parent
			pSrcNode->AddRef();

			//
			i3GameNode * pSrcParent = pSrcObj->getParent();
			if( pSrcParent != NULL )
			{
				pSrcParent->RemoveChild( pSrcNode);
			}

			if( pTargetObj != NULL)
			{
				pTargetObj->AddChild( pSrcNode);
			}

			pSrcNode->Release();
		}

		Rec_SetTree( hTargetItem, pSrcNode);

		if( bClone == FALSE)
		{
			m_TreeCtrl.DeleteItem( hSrcItem);
		}
	}
}

////자신과 타겟의 부모가 같은 경우에만 사용할수 있다.
void CGuiTreeCtrl::ExchangePosition( HTREEITEM hSrcItem, HTREEITEM hTargetItem)
{
	i3GameNode * pSrcObj, * pTargetObj;

	if( IsValidItem( hSrcItem ) )
	{
		pSrcObj = (i3GameNode *) m_TreeCtrl.GetItemData( hSrcItem);
	}
	else
		return ;

	if( IsValidItem( hTargetItem ))
	{
		pTargetObj = (i3GameNode *) m_TreeCtrl.GetItemData( hTargetItem);
	}
	else
		return ;

	if( hSrcItem == hTargetItem)
		return;

	if( pSrcObj->getParent() != pTargetObj->getParent())
		return;

	i3GameNode * pParent = pTargetObj->getParent();
	if( pParent == NULL)
		return;

	pSrcObj->AddRef();
	pTargetObj->AddRef();

	//일단 떼어 내고..
	pParent->RemoveChild( pSrcObj);
	m_TreeCtrl.DeleteItem( hSrcItem);

	HTREEITEM hnew = NULL;
	if( pTargetObj == pParent->getFirstChild())
	{
		pParent->AddChildToHead( pSrcObj);

		hnew = AddTreeItem( pSrcObj, m_TreeCtrl.GetParentItem(hTargetItem), TVI_FIRST);
		if( pSrcObj->getFirstChild() != NULL)
		{
			Rec_SetTree( hnew, pSrcObj->getFirstChild());
		}
	}
	else
	{
		HTREEITEM hPrev = m_TreeCtrl.GetNextItem( hTargetItem, TVGN_PREVIOUS);

		i3GameNode * pPrev = pTargetObj->getParent()->getFirstChild();
		while( pPrev->getNext() != pTargetObj)
		{
			pPrev = pPrev->getNext();
		}

		pPrev->SetNext( pSrcObj);
		pSrcObj->setParent( pTargetObj->getParent());
		pSrcObj->SetNext( pTargetObj);

		hnew = AddTreeItem( pSrcObj, m_TreeCtrl.GetParentItem(hTargetItem), hPrev);
		if( pSrcObj->getFirstChild() != NULL)
		{
			Rec_SetTree( hnew, pSrcObj->getFirstChild());
		}
	}

	if( hnew != NULL )
	{
		m_TreeCtrl.SelectItem( hnew);
	}

	pSrcObj->Release();
	pTargetObj->Release();
}


////////////
BEGIN_MESSAGE_MAP(CGuiTreeCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( NM_CLICK, 100, OnTVNClicked)
	ON_NOTIFY( NM_RCLICK, 100, OnNMRclickGuiTree)
	ON_NOTIFY( TVN_BEGINDRAG, 100, OnTVNBeginDrag)
	ON_NOTIFY( TVN_SELCHANGED, 100, OnTVNSelChanged)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

int CGuiTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// TODO:  Add your specialized creation code here
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	return -1;

	// Tree Control
	{
		CRect rect;

		GetClientRect( &rect);

		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS |
								TVS_INFOTIP | TVS_LINESATROOT,
								rect, this, 100) == FALSE)
		{
			I3ASSERT_0;
		}
	}

	
	{
		m_pImageList			= i3TDK::GetSystemImageList();
		m_nIndexOfFirstTDKImage	= i3TDK::AddTDKImageListTo( m_pImageList);

		if( m_pImageList != NULL)
		{
			m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
		}
	}


	{
		m_hDropCursor = (HCURSOR) ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_DRAG));
		m_hNoDropCursor = (HCURSOR) ::LoadCursor( NULL, IDC_NO);
	}

	return 0;
}

void CGuiTreeCtrl::OnNMRclickGuiTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32	cmd;
	HTREEITEM	hSel = NULL;
	i3GameNode * pSelNode = NULL;

	GetCursorPos(&pt);

	//select
	{
		POINT	cpt;
		UINT	flag;

		cpt = pt;

		m_TreeCtrl.ScreenToClient( &cpt );

		hSel = m_TreeCtrl.HitTest( cpt, &flag );

		if(( hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);
		}
	}
	
	{
		Menu.LoadMenu( IDR_POPUP_GUI );
	}

	pPopup = Menu.GetSubMenu(0);

	if( hSel == NULL )
	{
		pPopup->EnableMenuItem( ID_POPUP_EXPAND, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_POPUP_COLLAPSE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
//		pPopup->EnableMenuItem( ID_POPUP_SHOW, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_POPUP_SELECT_SELECTALLDESCENDANTS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_POPUP_SELECT_UNSELECTALLDESCENDANTS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_POPUP_INSERT_INSERTCONTROL, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_POPUP_INSERT_INSERTWINDOW, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_POPUP_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_POPUP_SETTEMPLATE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);

		pPopup->EnableMenuItem( ID__EXPORTNODE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID__IMPORTNODE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		pSelNode = (i3GameNode*)m_TreeCtrl.GetItemData( hSel );
		if( pSelNode != NULL)
		{
			if( pSelNode->IsTypeOf( i3UIManager::GetClassMeta()))
			{
				pPopup->EnableMenuItem( ID_POPUP_SETTEMPLATE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
				pPopup->EnableMenuItem( ID_POPUP_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
			}
		}
	}

	//OnPopupMenu( pPopup, hSel );

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *)this, NULL);

	switch( cmd )
	{
	case	ID_POPUP_EXPAND:							Expand(hSel);	break;
	case	ID_POPUP_COLLAPSE:							Collapse( hSel);break;
//	case	ID_POPUP_SHOW:								
	case	ID_POPUP_SELECT_SELECTALL:					SelectAll();	break;
	case	ID_POPUP_SELECT_UNSELECTALL:				UnselectAll();	break;
	case	ID_POPUP_SELECT_SELECTALLDESCENDANTS:		SelectDescendant( hSel );		break;
	case	ID_POPUP_SELECT_UNSELECTALLDESCENDANTS:		UnselectDescendant( hSel );	break;
	case	ID_POPUP_SELECT_REVERSESELECTION:			ReverseSelection();	break;
	case	ID_POPUP_INSERT_INSERTCONTROL:				InsertControl( hSel );	break;
	case	ID_POPUP_DELETE:							Delete( hSel );		break;
	case	ID_POPUP_SETTEMPLATE:						SetTemplate( hSel ); break;
		
	case	ID__EXPORTNODE:								ExportNode( hSel); break;
	case	ID__IMPORTNODE:								ImportNode( hSel); break;
	case	ID__SETCURRENTROOT:							SetCurrentRootNode( hSel);	break;

	case ID_POPUP_FIND:									OnFindNode( hSel);
		break;
	case ID_CHECK_MNEMONICS:							CheckMnemonics();
		break;
	default:
		break;
	}
}

void CGuiTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

void CGuiTreeCtrl::OnTVNBeginDrag( NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CPoint pt;

	*pResult = 0;

	//GetDragItem
	m_hSrcItem = pNMTreeView->itemNew.hItem;
	if( m_hSrcItem == NULL )
		return ;

	m_hSelected = m_hSrcItem;
	m_TreeCtrl.Select( m_hSrcItem, TVGN_CARET);
	m_bDragging		= true;
	m_bDropEnable	= true;
	m_hTargetItem	= NULL;

	SetCapture();
	
	{
		m_OldClassCursor = (HCURSOR)::GetClassLong( m_TreeCtrl.m_hWnd, GCL_HCURSOR);
		::SetClassLong( m_TreeCtrl.m_hWnd, GCL_HCURSOR, NULL);
	}
}

void CGuiTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		HTREEITEM hNew;

		hNew = m_TreeCtrl.HitTest( point, NULL );

		if( hNew != NULL )
		{
			i3GameNode * pTargetObj;
			i3GameNode * pSrcObj;

			m_bDropEnable = TRUE;

			{
				if( m_hTargetItem != NULL )
				{
					m_TreeCtrl.SetItemState( m_hTargetItem, 0x00000000, TVIS_DROPHILITED);
				}

				m_hTargetItem = hNew;
				m_TreeCtrl.SetItemState( m_hTargetItem, 0xFFFFFFFF, TVIS_DROPHILITED);
			}

			pTargetObj = (i3GameNode*)m_TreeCtrl.GetItemData( m_hTargetItem);
			pSrcObj = (i3GameNode*)m_TreeCtrl.GetItemData( m_hSrcItem );

			if( pTargetObj->IsTypeOf( i3UIManager::GetClassMeta()))
			{
				m_bDropEnable = FALSE;
			}
		}

		if( m_bDropEnable == TRUE)	SetCursor( m_hDropCursor);
		else						SetCursor( m_hNoDropCursor );
	}

	CWnd::OnMouseMove( nFlags, point);
}

void CGuiTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		if((m_hTargetItem != NULL) && ( m_hSrcItem != NULL ) && ( m_bDropEnable == TRUE ))
		{
			m_TreeCtrl.SetItemState( m_hSrcItem, 0x00000000, TVIS_DROPHILITED);
			m_TreeCtrl.SetItemState( m_hTargetItem, 0x00000000, TVIS_DROPHILITED);

			if( nFlags & MK_SHIFT)
				DragDrop( m_hSrcItem, m_hTargetItem, true );
			else if( nFlags & MK_CONTROL)
				ExchangePosition( m_hSrcItem, m_hTargetItem);
			else
				DragDrop( m_hSrcItem, m_hTargetItem, false );

		}

		m_bDragging = FALSE;
		ReleaseCapture();

		::SetClassLong( m_TreeCtrl.m_hWnd, GCL_HCURSOR, (LONG)m_OldClassCursor);
	}

	CWnd::OnLButtonUp( nFlags, point );
}

void CGuiTreeCtrl::OnDestroy()
{
	if( m_hDropCursor != NULL)
	{
		DeleteObject( m_hDropCursor);
	}

	CWnd::OnDestroy();
}

void CGuiTreeCtrl::OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew)
{
	//Get Selected Item     	
 	if( hNew != NULL)
	{
		i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hNew);

		if( pObj != NULL)
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_GAMEGRAPH, pObj);
		}

		m_hSelected = hNew;
	}
}

void CGuiTreeCtrl::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult )
{
	NMTREEVIEW * pNMTV = (NMTREEVIEW*) pNMHDR;

	OnChangeSelectItem( pNMTV->itemOld.hItem, pNMTV->itemNew.hItem);

	*pResult = 0;
}

void CGuiTreeCtrl::OnTVNClicked(NMHDR * pNMHDR, LRESULT * pResult)
{
	HTREEITEM hCurItem = m_TreeCtrl.GetSelectedItem();
	if( hCurItem != NULL )
	{
		i3GameNode * pObj = (i3GameNode*)m_TreeCtrl.GetItemData( hCurItem );
		if( pObj != NULL )
		{
			if( pObj->IsTypeOf( i3UIControl::GetClassMeta()))
			{
				i3UIControl * pCtrl = (i3UIControl*)pObj;
				i3UIControlData * pData = pCtrl->GetControlData();
				if( pData->m_pShape != NULL )
				{
					i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_GAMEGRAPH, pCtrl);
					i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE | I3_TDK_USRWND_GAMEGRAPH, ((i3GameObjBase*)pObj)->GetNode());
				}
			}
		}
	}

	*pResult = 0;
}

BOOL CGuiTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN)
	{
		::TranslateMessage( pMsg);

		::DispatchMessage( pMsg);

		if( pMsg->wParam == VK_F3)
		{
			if( m_szSearchString[0] == 0)
			{
				OnFindNode();
			}
			else
			{
				OnFindNext();
			}
		}

		return TRUE;
	}

	return CWnd::PreTranslateMessage( pMsg);
}

void CGuiTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( m_hSelected == NULL)
	{
		CWnd::OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	if(GetKeyState( VK_LCONTROL ) & 0x80)
	{
		HTREEITEM hTargetItem = NULL;
		switch( nChar )
		{
			case VK_UP:
				{
					hTargetItem = m_TreeCtrl.GetNextItem( m_hSelected, TVGN_PREVIOUS);
				}
				break;
				
			case VK_DOWN:
				{
					hTargetItem = m_TreeCtrl.GetNextItem( m_hSelected, TVGN_NEXT);
				}
				break;
		}

		if( hTargetItem != NULL)
		{
			ExchangePosition( m_hSelected, hTargetItem);
		}
	}
	else
	{
		CWnd::OnChar(nChar, nRepCnt, nFlags);
	}

	return;
}

void CGuiTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	/*
	// TODO: Add your message handler code here and/or call default
	if( GetKeyState( VK_F3 ) & 0x80)
	{
		if( m_szSearchString[0] == 0)
		{
			OnFindNode();
		}
		else
		{
			OnFindNext();
		}
	}*/

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

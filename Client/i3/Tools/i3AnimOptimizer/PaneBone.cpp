// PaneBone.cpp : implementation file
//

#include "stdafx.h"
#include "i3AnimOptimizer.h"
#include "PaneBone.h"
#include ".\panebone.h"
#include "GlobVar.h"
#include "SelectNodeTypeDialog.h"

// CPaneBone dialog

IMPLEMENT_DYNAMIC(CPaneBone, CDialog)
CPaneBone::CPaneBone(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneBone::IDD, pParent)
{
	m_pRoot = NULL;
	m_pImageList = NULL;
}

CPaneBone::~CPaneBone()
{
	if( m_pImageList != NULL)
	{
		delete m_pImageList;
		m_pImageList = NULL;
	}
}

void CPaneBone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BONE_TREE, m_BoneTreeCtrl);
}

void	MakeItemName( char * pszStr, i3Node * pNode)
{
	i3ClassMeta * pMeta = pNode->GetMeta();

	if( pNode->GetName() == NULL)
	{
		sprintf( pszStr, "%s", pMeta->GetClassName());
	}
	else
	{
		sprintf( pszStr, "%s \"%s\"", pMeta->GetClassName(), pNode->GetName());
	}
}

void CPaneBone::Rec_SetTree( HTREEITEM hParentItem, i3Node * pNode)
{
	INT32 i, ImageIdx;
	HTREEITEM hItem;
	i3Node * pChild;
	char szName[128];
	TVINSERTSTRUCT info;
	
	if( pNode->IsTypeOf( i3Transform::GetClassMeta()))
		ImageIdx = 0;
	else if( pNode->IsTypeOf( i3ColliderSet::GetClassMeta()))
		ImageIdx = 4;
	else if( pNode->IsTypeOf( i3Geometry::GetClassMeta()))
		ImageIdx = 6;
	else
		ImageIdx = 2;

	MakeItemName( szName, pNode);
	
	info.hInsertAfter = TVI_LAST;
	info.hParent = hParentItem;
	info.item.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	info.item.cChildren = pNode->GetChildCount();
	info.item.hItem = 0;
	info.item.iImage = ImageIdx + 1;
	info.item.iSelectedImage = ImageIdx;
	info.item.lParam = (LPARAM) pNode;
	info.item.pszText = szName;
	
	hItem = m_BoneTreeCtrl.InsertItem( &info);

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		Rec_SetTree( hItem, pChild);
	}
}

void CPaneBone::SetSceneGraph( i3Node * pRoot)
{
	m_pRoot = pRoot;

	m_BoneTreeCtrl.DeleteAllItems();

	if( m_pRoot == NULL)
		return;

	Rec_SetTree( TVI_ROOT, m_pRoot);
}

BEGIN_MESSAGE_MAP(CPaneBone, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_BONE_TREE, OnNMRclickBoneTree)
END_MESSAGE_MAP()


// CPaneBone message handlers

BOOL CPaneBone::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon;

		m_pImageList = new CImageList;
		m_pImageList->Create( 16, 16, ILC_COLOR32 | ILC_MASK, 8, 0);

		{
			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_BONE);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);

			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_BONE_UNSEL);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);
		}

		{
			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_NODE);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);

			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_NODE_UNSEL);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);
		}

		{
			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_COLL);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);

			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_COLL_UNSEL);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);
		}

		{
			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_GEO);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);

			hIcon = AfxGetApp()->LoadIcon( IDI_NODE_GEO_UNSEL);
			m_pImageList->Add( hIcon);
			::DestroyIcon( hIcon);
		}

		m_BoneTreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
	}

	RegisterUpdate( m_hWnd, i3Node::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneBone::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_BoneTreeCtrl.m_hWnd))
	{
		CRect rt;

		m_BoneTreeCtrl.GetWindowRect( &rt);
		ScreenToClient( &rt);

		m_BoneTreeCtrl.SetWindowPos( NULL, 0, 0, cx - rt.left - 3, cy - rt.top - 3, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

void CPaneBone::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		SetSceneGraph( (i3Node *) pInfo->m_pObject);
	}
}

LRESULT CPaneBone::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPaneBone::OnNMRclickBoneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = NULL;

	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_BoneTreeCtrl.ScreenToClient( &cpt);

		hSel = m_BoneTreeCtrl.HitTest( cpt, &flag);

		if ((hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_BoneTreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	Menu.LoadMenu( IDR_POPUP_NODE);

	pPopup = Menu.GetSubMenu( 0);

	if( hSel == NULL)
	{
		pPopup->EnableMenuItem( ID_NODE_EXPAND, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_COLLAPSE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_SELECTALLDESCENDANT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_UNSELECTALLDESCENDANTS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_COLLAPSENODE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_NODE_EXPAND :					OnExpand( hSel);	break;
		case ID_NODE_COLLAPSE :					OnCollapse( hSel);	break;
		case ID_NODE_SELECTALL :				OnSelectAll();		break;
		case ID_NODE_UNSELECTALL :				OnUnselectAll();	break;
		case ID_NODE_SELECTALLDESCENDANT :		OnSelectDescendant( hSel);	break;
		case ID_NODE_UNSELECTALLDESCENDANTS :	OnUnselectDescendant( hSel);	break;
		case ID_NODE_SELECTBYTYPE :				OnSelectByType();	break;
		case ID_NODE_REVERSESELECTION :			OnReverseSelection();	break;

		case ID_NODE_DELETE :					OnDelete( hSel);		break;
		case ID_NODE_COLLAPSENODE :				OnCollapseNode( hSel);	break;
		case ID_NODE_COLLAPSETRANSFORM :		OnCollapseTransform( hSel);	break;
		case ID_NODE_REMOVENODENAMES :			OnRemoveNodeName();	break;
		case ID_NODE_REMOVEBOUNDBOX :			OnRemoveBoundBox();	break;

		case ID_NODE_FINDUNUSEDI3TRANSFORM :	OnFindBoneUsage( FALSE);	break;
		case ID_NODE_FINDUSEDI3TRANSFORM :		OnFindBoneUsage( TRUE);	break;

		case ID_NODE_SAVESCENEGRAPH :			g_Project.SaveScene();	break;
		case ID_NODE_SAVEAS :					g_Project.SaveAsScene();	break;

		case ID_TOOLS_BUILDBONEINDEXOFANIMATION :	g_Project.BuildBoneIndex();	break;
	}
	
	*pResult = 1;
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

void CPaneBone::OnExpand( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _ExpandProc, NULL);
}

void CPaneBone::OnCollapse( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _ExpandProc, (void *) 1);
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

void CPaneBone::OnSelectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _SelectAllProc, NULL);
}

void CPaneBone::OnUnselectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _SelectAllProc, (void *) 1);
}

void CPaneBone::OnReverseSelection(void)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _SelectAllProc, (void *) 2);
}

void CPaneBone::OnSelectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _SelectAllProc, NULL, hItem);
}

void CPaneBone::OnUnselectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _SelectAllProc, (void *) 1, hItem);
}

/////////
static BOOL _SelectByMetaProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3List * pList = (i3List *) pData;
	INT32 i;
	i3ClassMeta * pMeta;
	i3Node * pNode;

	for( i = 0; i < pList->GetCount(); i++)
	{
		pMeta = (i3ClassMeta *) pList->Items[i];

		pNode = (i3Node *) pCtrl->GetItemData( hItem);

		if( pNode->IsTypeOf( pMeta))
		{
			pCtrl->SetCheck( hItem, TRUE);
		}
		else
		{
			pCtrl->SetCheck( hItem, FALSE);
		}
	}

	return TRUE;
}

void CPaneBone::OnSelectByType(void)
{
	CSelectNodeTypeDialog dlg;

	dlg.DoModal();

	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _SelectByMetaProc, & dlg.m_MetaList);
}


BOOL CPaneBone::IsValidItem( HTREEITEM hItem)
{
	TVITEM data;

	data.mask = TVIF_HANDLE;
	data.hItem = hItem;

	return m_BoneTreeCtrl.GetItem( &data);
}

void CPaneBone::OnDelete( HTREEITEM hItem)
{
	i3List SelList;
	INT32 i;
	i3Node * pNode;

	GetCheckedItem( &SelList);

	for( i = 0; i < SelList.GetCount(); i++)
	{
		hItem = (HTREEITEM) SelList.Items[i];

		if( IsValidItem( hItem))
		{

			pNode = (i3Node *) m_BoneTreeCtrl.GetItemData( hItem);
			I3ASSERT( pNode != NULL);

			pNode->RemoveFromParent();

			m_BoneTreeCtrl.DeleteItem( hItem);
		}
	}
}

void CPaneBone::OnCollapseNode( HTREEITEM hItem)
{
	i3Node * pNode, * pChild;
	i3Node * pParent;
	HTREEITEM hParent;
	i3List SelList;
	INT32 i;

	GetCheckedItem( &SelList);

	for( i = 0; i < SelList.GetCount(); i++)
	{
		hItem = (HTREEITEM) SelList.Items[i];

		if( IsValidItem( hItem) == FALSE)
			continue;

		pNode = (i3Node *) m_BoneTreeCtrl.GetItemData( hItem);
		I3ASSERT( pNode != NULL);

		pParent = pNode->GetParent();
		if( pParent == NULL)
		{
			AfxMessageBox( "선택 노드는 Root 노드이기 때문에 Collapse할 수 없습니다.");
			return;
		}

		hParent = m_BoneTreeCtrl.GetParentItem( hItem);

		// Node의 재연결
		for(; pNode->GetChildCount() > 0;)
		{
			pChild = pNode->GetChild( 0);

			pChild->AddRef();

			pNode->RemoveChild( pChild);
			pParent->AddChild( pChild);

			Rec_SetTree( hParent, pChild);

			pChild->Release();
		}

		pParent->RemoveChild( pNode);
		m_BoneTreeCtrl.DeleteItem( hItem);
	}

}

void CPaneBone::OnCollapseTransform( HTREEITEM hItem)
{
	i3Node * pNode, * pChild;
	i3Node * pParent;
	HTREEITEM hParent;
	i3List SelList;
	INT32 i;
	i3OptCollapseTransform opt;
	MATRIX tmx;

	GetCheckedItem( &SelList);

	for( i = 0; i < SelList.GetCount(); i++)
	{
		hItem = (HTREEITEM) SelList.Items[i];

		if( IsValidItem( hItem) == FALSE)
			continue;

		pNode = (i3Node *) m_BoneTreeCtrl.GetItemData( hItem);
		I3ASSERT( pNode != NULL);

		if( pNode->IsTypeOf( i3Transform::GetClassMeta()) == FALSE)
			continue;

		if( pNode->IsTypeOf( i3SceneObject::GetClassMeta()))
		{
			i3SceneObject * pSceneObj = (i3SceneObject *) pNode;

			i3Matrix::Copy( &tmx, pSceneObj->GetMatrix());
			//i3Matrix::Mul( &tmx, pSceneObj->GetAxisRotateMatrix(), &tmx);
		}
		else
		{
			i3Transform * pTrans = (i3Transform *) pNode;

			i3Matrix::Copy( &tmx, pTrans->GetMatrix());
		}

		pParent = pNode->GetParent();
		if( pParent == NULL)
		{
			AfxMessageBox( "선택 노드는 Root 노드이기 때문에 Collapse할 수 없습니다.");
			return;
		}

		hParent = m_BoneTreeCtrl.GetParentItem( hItem);

		// Node의 재연결
		for(; pNode->GetChildCount() > 0;)
		{
			pChild = pNode->GetChild( 0);

			pChild->AddRef();

			pNode->RemoveChild( pChild);
			pParent->AddChild( pChild);

			opt.CollapseTransform( &tmx, pChild);

			Rec_SetTree( hParent, pChild);

			pChild->Release();
		}

		pParent->RemoveChild( pNode);
		m_BoneTreeCtrl.DeleteItem( hItem);
	}

}

/////////
static BOOL _RemoveNameProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3Node * pNode;
	char szName[512];

	if( pCtrl->GetCheck( hItem))
	{
		pNode = (i3Node *) pCtrl->GetItemData( hItem);
		I3ASSERT( pNode != NULL);

		pNode->SetName( NULL);

		MakeItemName( szName, pNode);
		pCtrl->SetItemText( hItem, szName);
	}

	return TRUE; 
}

void CPaneBone::OnRemoveNodeName(void)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _RemoveNameProc, NULL);
}

/////////
static BOOL _RemoveBoundBoxProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3Node * pNode;

	pNode = (i3Node *) pCtrl->GetItemData( hItem);
	I3ASSERT( pNode != NULL);

	if( pNode->GetBound() != NULL)
	{
		pNode->SetBound( NULL);
	}

	return TRUE; 
}

void CPaneBone::OnRemoveBoundBox(void)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _RemoveBoundBoxProc, NULL);
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

void CPaneBone::GetCheckedItem( i3List * pList)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _GetCheckedItemProc, pList);
}

/////////
typedef struct _tagBoneData
{
	i3Transform *	m_pBone;
	INT32			m_UsedCount;
} BONEDATA;

static INT32 FindBone( BONEDATA * pData, INT32 Count, char * pszBoneName)
{
	INT32 i;

	for( i = 0; i < Count; i++)
	{
		if( pData[i].m_pBone->GetName() != NULL)
		{
			if( i3String::Compare( pData[i].m_pBone->GetName(), pszBoneName) == 0)
			{
				return i;
			}
		}
	}

	return -1;
}

void CPaneBone::OnFindBoneUsage( BOOL bUsed)
{
	i3Node * pRoot;
	i3List TransList;

	if( g_Project.GetRoot() == NULL)
		return;

	OnUnselectAll();

	pRoot = g_Project.GetRoot();
	
	i3Scene::FindNodeByType( pRoot, i3Transform::GetClassMeta(), &TransList, TRUE);

	if( TransList.GetCount() > 0)
	{
		INT32 i, j, idx;
		BONEDATA * pData;

		pData = new BONEDATA[ TransList.GetCount()];

		for( i = 0; i < TransList.GetCount(); i++)
		{
			pData[i].m_pBone = (i3Transform *) TransList.Items[i];
			pData[i].m_UsedCount = 0;
		}


		// Animation들을 모두 확인하며, 각 Bone의 Reference Count를 계산한다.
		for( i = 0; i < g_Project.GetAnimCount(); i++)
		{
			i3Animation * pAnim = g_Project.GetAnim( i);

			for( j = 0; j < pAnim->GetTrackCount(); j++)
			{
				idx = FindBone( pData, TransList.GetCount(), pAnim->GetTrackBoneName( j));
				if( idx == -1)
				{
					I3TRACE( "Could not find corresponding bone - Bone:%s  Anim:%s\n", pAnim->GetTrackBoneName(j), pAnim->GetName());
				}
				else
				{
					pData[idx].m_UsedCount++;
				}
			}
		}

		// 사용된(사용되지 않은 Bone들에 대한 Check 처리
		for( i = 0; i < TransList.GetCount(); i++)
		{
			bool bSelect = false;
			HTREEITEM hItem;

			if( pData[i].m_pBone != g_Project.GetRoot())
			{
				if( bUsed)
				{
					// 사용된 Bone을 선택
					if( pData[i].m_UsedCount > 0)
						bSelect = true;
				}
				else
				{
					// 사용되지 않은 Bone을 선택
					if( pData[i].m_UsedCount <= 0)
						bSelect = true;
				}
			}

			if( bSelect)
			{
				hItem = i3TDK::FindTreeItemByData( &m_BoneTreeCtrl, pData[i].m_pBone);
				I3ASSERT( hItem != NULL);

				m_BoneTreeCtrl.SetCheck( hItem, TRUE);
			}
		}

		delete pData;
	}
}


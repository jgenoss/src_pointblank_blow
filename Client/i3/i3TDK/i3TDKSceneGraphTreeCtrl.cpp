// i3TDKSceneGraphTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSceneGraphTreeCtrl.h"
#include "i3TDKUtilGDI.h"
#include "i3TDKUtilCtrl.h"
#include "i3TDKUtilCommon.h"
#include "resource.h"
#include "i3TDKSelectNodeTypeDialog.h"
#include "i3TDKSingleSelectNodeTypeDialog.h"
#include "i3TDKSelectAttrTypeDialog.h"
#include "i3TDKMeta.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKOptSpecs.h"
#include "i3TDKUpdateSystem.h"
#include "i3TDKDlgOptProgress.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/algorithm/sort.h"

// i3TDKSceneGraphTreeCtrl

enum
{
	I3ICON_ANIM = 0,
	I3ICON_ATTR = 2,
	I3ICON_BONE = 4,
	I3ICON_CAMERA = 6,
	I3ICON_GEOMETRY = 8,
	I3ICON_NODE = 10,
	I3ICON_RESOURCE = 12,
	I3ICON_SOUND = 14,
	I3ICON_TEXTURE = 16,
};

IMPLEMENT_DYNAMIC(i3TDKSceneGraphTreeCtrl, CWnd)

i3TDKSceneGraphTreeCtrl::~i3TDKSceneGraphTreeCtrl()
{
	if( m_pImageList != nullptr)
	{
		delete m_pImageList;
		m_pImageList = nullptr;
	}

	if( m_hDropCursor != nullptr)
	{
		::DeleteObject( m_hDropCursor);
		::DeleteObject( m_hNoDropCursor);

		m_hDropCursor = nullptr;
		m_hNoDropCursor = nullptr;
	}

	I3_SAFE_RELEASE( m_pSgInfo);
}

BOOL i3TDKSceneGraphTreeCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hBack;

	m_dwStyle = dwStyle;

	hCursor = LoadCursor( nullptr, IDC_ARROW);
	I3ASSERT( hCursor != nullptr);

	hBack = (HBRUSH) GetStockObject( NULL_BRUSH);
	I3ASSERT( hBack != nullptr);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, nullptr);
	I3ASSERT( pszClass != nullptr);

	dwStyle |= WS_CLIPCHILDREN;

	return CWnd::Create( pszClass, _T(""), dwStyle, rect, pParentWnd, nID, nullptr);
}

static void	MakeItemName( TCHAR* pszStr, INT32 outStrSize, i3Node * pNode)
{
	i3ClassMeta * pMeta = pNode->meta();

	i3::snprintf( pszStr, outStrSize, _T("%s"), pMeta->class_name() );
}

INT32 i3TDKSceneGraphTreeCtrl::getChildItemCount( i3ElementBase * pObj)
{
	INT32 c = 0;

	if( i3::kind_of<i3Node*>(pObj))
	{
		i3Node * pNode = (i3Node *) pObj;

		c += pNode->GetChildCount();

		if( m_ShowStyle & I3TDK_SGTREE_SHOW_ATTRS)
		{
			if( i3::kind_of<i3Geometry* >(pObj))
			{
				c += ((i3Geometry *) pNode)->GetGeometryAttrCount();
			}
			else if( i3::kind_of<i3LightSet* >(pObj))
			{
				c += ((i3LightSet *) pNode)->GetLightAttrCount();
			}
			else if( i3::kind_of<i3AttrSet* >(pObj))
			{
				c += ((i3AttrSet *) pNode)->GetAttrCount();
			}
			else if( i3::kind_of<i3ColliderSet* >(pObj))
			{
				c += ((i3ColliderSet *) pNode)->GetColliderCount();
			}
			else if( i3::kind_of< i3Skin* >(pObj))
			{
				c += ((i3Skin *) pNode)->getShapeCount();
			}
			else if( i3::same_of<i3Body* >(pObj))
			{
				c += ((i3Body *) pNode)->getLODCount();
			}
			else if( i3::same_of<i3BinaryPartitionNode* >(pObj))
			{
				i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pObj;

				if( pBSP->getFront() != nullptr)		c++;
				if( pBSP->getBack() != nullptr)		c++;
			}
			else if( i3::kind_of<i3OcTreeNode* >(pObj))
			{
				i3OcTreeNode * pOctree = (i3OcTreeNode *) pObj;
				INT32 i;

				for( i = 0; i < 8; i++)
				{
					if( pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i) != nullptr)
						c++;
				}
			}
		}
	}
	else if( i3::kind_of<i3Collider* >(pObj))
	{
		c = 1;
	}
	else if( i3::kind_of<i3Shape* >(pObj))
	{
		c = ((i3Shape *) pObj)->getGeometryAttrCount();
	}
	else if( i3::kind_of< i3LOD* >(pObj))
	{
		i3LOD * pLOD = (i3LOD *) pObj;

		if( pLOD->getShapeNode() != nullptr)
			c = 1;
		else
			c = 0;
	}

	return c;
}

void i3TDKSceneGraphTreeCtrl::_SetTreeItem( TVITEM * pInfo, i3ElementBase * pObj, const TCHAR* szMetaName, HTREEITEM hItem)
{
//	i3ClassMeta * pMeta = pObj->meta();
	INT32 ImageIdx;

	pInfo->mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	pInfo->hItem = hItem;
	pInfo->lParam = (LPARAM) pObj;
	pInfo->pszText = (TCHAR*)szMetaName;		//	const_cast<char*>(pMeta->class_name().c_str());

	if( i3::kind_of<i3Node* >(pObj))
	{
		i3Node * pNode = (i3Node *) pObj;

		if( i3::kind_of<i3Camera* >(pNode))
			ImageIdx = I3ICON_CAMERA;
		else if( i3::kind_of<i3Transform* >(pNode))
			ImageIdx = I3ICON_BONE;
		else if( i3::kind_of<i3Geometry* >(pNode))
			ImageIdx = I3ICON_GEOMETRY;
		else if( i3::kind_of<i3OcTreeNode* >(pNode))
			ImageIdx = I3ICON_GEOMETRY;
		else
			ImageIdx = I3ICON_NODE;
		
		pInfo->cChildren = getChildItemCount( pNode);
	}
	else if( i3::kind_of<i3RenderAttr* >(pObj))
	{
		if( i3::kind_of<i3TextureBindAttr* >(pObj))
			ImageIdx = I3ICON_TEXTURE;
		else if( i3::kind_of<i3TextureMatrixAttr* >(pObj))
			ImageIdx = I3ICON_TEXTURE;
		else
			ImageIdx = I3ICON_ATTR;

		pInfo->cChildren = 0;
	}
	else if( i3::kind_of<i3Shape* >(pObj))
	{
		i3Shape * pShape = (i3Shape *)  pObj;

		ImageIdx = I3ICON_ATTR;

		pInfo->cChildren = pShape->getGeometryAttrCount();
	}
	else if( i3::same_of<i3LOD* >(pObj))
	{
		i3LOD * pLOD = (i3LOD *) pObj;

		ImageIdx = I3ICON_ATTR;

		if( pLOD->getShapeNode() != nullptr)
			pInfo->cChildren = 2;
		else
			pInfo->cChildren = 0;

	}
	else
	{
		ImageIdx = I3ICON_RESOURCE;
		pInfo->cChildren = 0;
	}

	pInfo->iImage = ImageIdx + 1;
	pInfo->iSelectedImage = ImageIdx;
}

HTREEITEM	i3TDKSceneGraphTreeCtrl::AddTreeItem( i3ElementBase * pObj, HTREEITEM hParent)
{
	TVINSERTSTRUCT info;
	HTREEITEM hItem;

	info.hInsertAfter = TVI_LAST;
	info.hParent = hParent;
	
#ifdef _UNICODE
	i3::stack_wstring strMeta;	i3::mb_to_utf16(pObj->meta()->class_name(), strMeta);
#else
	const i3::fixed_string& strMeta = pObj->meta()->class_name();
#endif

	_SetTreeItem( &info.item, pObj, strMeta.c_str());

	hItem = m_TreeCtrl.InsertItem( &info);

	if((hItem != nullptr) && (hParent != nullptr) && (hParent != m_hCommonRootItem) && (hParent != m_hInstanceRootItem))
	{
		TVITEM item;

		item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_PARAM;
		item.hItem = hParent;

		m_TreeCtrl.GetItem( &item);

		if( item.lParam != 0)
		{
			item.cChildren = getChildItemCount( (i3ElementBase *) item.lParam);
		}
		else
		{
			item.cChildren = 1;
		}

		m_TreeCtrl.SetItem( &item);
	}

	return hItem;
}

void i3TDKSceneGraphTreeCtrl::Rec_SetTree( HTREEITEM hParentItem, i3Node * pNode)
{
	INT32 i;
	HTREEITEM hItem;
	i3Node * pChild;

	if( pNode->IsFlag( I3_NODEFLAG_VOLATILE))
		return;
	
	hItem = AddTreeItem( pNode, hParentItem);

	if( m_ShowStyle & I3TDK_SGTREE_SHOW_ATTRS)
	{
		if( i3::kind_of<i3AttrSet* >(pNode))
		{
			_OnAttrSet( (i3AttrSet *) pNode, hItem);
		}
		else if( i3::kind_of<i3LightSet* >(pNode))
		{
			_OnLightSet( (i3LightSet *) pNode, hItem);
		}
		else if( i3::kind_of<i3Geometry* >(pNode))
		{
			_OnGeometry( (i3Geometry *) pNode, hItem);
		}
		else if( i3::kind_of<i3ColliderSet* >(pNode))
		{
			_OnColliderSet( (i3ColliderSet *) pNode, hItem);
		}
		else if( i3::kind_of<i3Skin* >(pNode))
		{
			_OnSkin( (i3Skin *) pNode, hItem);
		}
		else if( i3::same_of<i3Body* >(pNode))
		{
			_OnBody( (i3Body *) pNode, hItem);
		}
		else if( i3::kind_of<i3OcTreeNode* >(pNode))
		{
			_OnOcTree( (i3OcTreeNode *) pNode, hItem);
		}
		else if( i3::same_of<i3BinaryPartitionNode* >(pNode))
		{
			_OnBinaryPartitionNode( (i3BinaryPartitionNode *) pNode, hItem);
		}
		else if( i3::same_of<i3TimeSequence*>(pNode))
		{
			_OnTimeSequence( static_cast<i3TimeSequence*>(pNode), hItem);
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		Rec_SetTree( hItem, pChild);
	}
}

void i3TDKSceneGraphTreeCtrl::SetSceneGraph( i3Node * pRoot)
{
	if( m_pSgInfo == nullptr)
	{
		m_pSgInfo = i3SceneGraphInfo::new_object();
	}

	if( pRoot == nullptr )
		return;

	m_pSgInfo->setInstanceSg( pRoot);

	Rec_SetTree( m_hInstanceRootItem, pRoot);
}

void i3TDKSceneGraphTreeCtrl::setSgInfo( i3SceneGraphInfo * pSgInfo)
{
	I3_REF_CHANGE( m_pSgInfo, pSgInfo);

	m_TreeCtrl.DeleteAllItems();

	if( m_pSgInfo != nullptr)
	{
		// Common Sg Root
		m_hCommonRootItem	= m_TreeCtrl.InsertItem( _T("Common Sg."), I3ICON_NODE, I3ICON_NODE);

		if( pSgInfo->getCommonSg() != nullptr)
		{
			Rec_SetTree( m_hCommonRootItem, pSgInfo->getCommonSg());
		}

		// Instance Sg Root
		m_hInstanceRootItem = m_TreeCtrl.InsertItem( _T("Instance Sg."), I3ICON_NODE, I3ICON_NODE);
		if( pSgInfo->getInstanceSg() != nullptr)
		{
			Rec_SetTree( m_hInstanceRootItem, pSgInfo->getInstanceSg());
		}
	}

	OnSetSceneGraph();
}

void i3TDKSceneGraphTreeCtrl::SetShowStyle( UINT32 style)
{
	UINT32 xor = style ^ m_ShowStyle;

	m_ShowStyle = style;

	if( xor & I3TDK_SGTREE_SHOW_ATTRS)
	{
		// Tree Item을 새로 Build해야 한다.
		setSgInfo( m_pSgInfo);
	}
	else
	{
		// 단순한 Refresh만으로 충분.
		m_TreeCtrl.Invalidate();
	}
}

void i3TDKSceneGraphTreeCtrl::AddShowStyle( UINT32 style)
{
	SetShowStyle( m_ShowStyle | style);
}

void i3TDKSceneGraphTreeCtrl::RemoveShowStyle( UINT32 style)
{
	SetShowStyle( m_ShowStyle & ~style);
}

void	i3TDKSceneGraphTreeCtrl::ToggleShowName(void)
{
	if( m_ShowStyle & I3TDK_SGTREE_SHOW_NAME)
		RemoveShowStyle( I3TDK_SGTREE_SHOW_NAME);
	else
		AddShowStyle(I3TDK_SGTREE_SHOW_NAME);
}

void	i3TDKSceneGraphTreeCtrl::ToggleShowFlags(void)
{
	if( m_ShowStyle & I3TDK_SGTREE_SHOW_FLAGS)
		RemoveShowStyle( I3TDK_SGTREE_SHOW_FLAGS);
	else
		AddShowStyle(I3TDK_SGTREE_SHOW_FLAGS);
}

void	i3TDKSceneGraphTreeCtrl::ToggleShowAttrs(void)
{
	if( m_ShowStyle & I3TDK_SGTREE_SHOW_ATTRS)
		RemoveShowStyle( I3TDK_SGTREE_SHOW_ATTRS);
	else
		AddShowStyle(I3TDK_SGTREE_SHOW_ATTRS);
}


BEGIN_MESSAGE_MAP(i3TDKSceneGraphTreeCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( NM_RCLICK, ID_TDK_SGTREECTRL, OnNMRclickBoneTree)
	ON_NOTIFY(NM_CUSTOMDRAW, ID_TDK_SGTREECTRL, OnNMCustomdrawBoneTree)
	ON_NOTIFY( TVN_BEGINDRAG, ID_TDK_SGTREECTRL, OnTVNBeginDrag)
	ON_NOTIFY( TVN_SELCHANGED, ID_TDK_SGTREECTRL, OnTVNSelChanged)
	ON_NOTIFY( TVN_KEYDOWN,		ID_TDK_SGTREECTRL, OnTVNKeyDown)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
//	ON_WM_KEYDOWN()
//	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// i3TDKSceneGraphTreeCtrl message handlers

void i3TDKSceneGraphTreeCtrl::AddIcon( INT32 id)
{
	HICON hIcon;

	hIcon = ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));
	m_pImageList->Add( hIcon);
	::DestroyIcon( hIcon);
}

int i3TDKSceneGraphTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Tree Control
	{
		CRect rect;

		GetClientRect( &rect);

		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES |
								TVS_INFOTIP | TVS_LINESATROOT,
								rect, this, ID_TDK_SGTREECTRL) == FALSE)
		{
			I3ASSERT_0;
		}
	}

	{
		m_pImageList = new CImageList;
		m_pImageList->Create( 16, 16, ILC_COLOR32 | ILC_MASK, 32, 0);

		AddIcon( IDI_ANIM);
		AddIcon( IDI_ANIM_UNSEL);
		AddIcon( IDI_ATTR);
		AddIcon( IDI_ATTR_UNSEL);
		AddIcon( IDI_BONE);
		AddIcon( IDI_BONE_UNSEL);
		AddIcon( IDI_CAMERA);
		AddIcon( IDI_CAMERA_UNSEL);
		AddIcon( IDI_GEOMETRY);
		AddIcon( IDI_GEOMETRY_UNSEL);
		AddIcon( IDI_NODE);
		AddIcon( IDI_NODE_UNSEL);
		AddIcon( IDI_RESOURCE);
		AddIcon( IDI_RESOURCE_UNSEL);
		AddIcon( IDI_SOUND);
		AddIcon( IDI_SOUND_UNSEL);
		AddIcon( IDI_TEXTURE);
		AddIcon( IDI_TEXTURE_UNSEL);

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
		//m_TreeCtrl.SetItemHeight( 16);
	}

	{
		m_hDropCursor = (HCURSOR) ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_TDK_DRAG));
		m_hNoDropCursor = (HCURSOR) ::LoadCursor( nullptr, IDC_NO);
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3Node::static_meta(), I3_TDK_UPDATE_ALL);
	}

	return 0;
}

void i3TDKSceneGraphTreeCtrl::OnDestroy()
{
	CWnd::OnDestroy();
}


void i3TDKSceneGraphTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

//static INT32 _MetaCompareProc( i3ClassMeta * p1, i3ClassMeta * p2)
//{
//	return strcmp( p1->GetClassName(), p2->GetClassName());
//}

namespace 
{
	bool		_MetaCompareProc(const i3ClassMeta * p1, const i3ClassMeta * p2)
	{
		return	p1->class_name() < p2->class_name();	//	(strcmp( p1->GetClassName(), p2->GetClassName()) < 0);
	}
}



void i3TDKSceneGraphTreeCtrl::OnNMRclickBoneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = nullptr;
	i3::vector<i3ClassMeta*> MetaList;
	CMenu * pOptMenu;

	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if ((hSel != nullptr) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	{
		HINSTANCE hOld = AfxGetResourceHandle();

		AfxSetResourceHandle( g_hInstTDK);

		Menu.LoadMenu( IDR_POPUP_NODE);

		AfxSetResourceHandle( hOld);
	}

	pPopup = Menu.GetSubMenu( 0);

	if( hSel == nullptr)
	{
		pPopup->EnableMenuItem( ID_NODE_EXPAND, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_COLLAPSE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_SELECTALLDESCENDANT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_UNSELECTALLDESCENDANTS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_COLLAPSENODE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_COLLAPSETRANSFORM, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_REMOVENODENAMES, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_INSERTNODE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_BOUNDBOX_SETBOUNDBOX, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_REMOVEBOUNDBOX, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_ATTRIBUTE_INSERTATTRIBUTES, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_ATTRIBUTE_REMOVEATTRIBUTEFROMDESCENDENTS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_IMPORTSCENE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_NODE_EXPORTSCENE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
	}

	if( m_ShowStyle & I3TDK_SGTREE_SHOW_NAME)
	{
		pPopup->CheckMenuItem( ID_SHOW_NAME, MF_BYCOMMAND | MF_CHECKED);
	}

	if( m_ShowStyle & I3TDK_SGTREE_SHOW_FLAGS)
	{
		pPopup->CheckMenuItem( ID_SHOW_FLAGS, MF_BYCOMMAND | MF_CHECKED);
	}

	if( m_ShowStyle & I3TDK_SGTREE_SHOW_ATTRS)
	{
		pPopup->CheckMenuItem( ID_SHOW_RENDERATTRIBUTES, MF_BYCOMMAND | MF_CHECKED);
	}

	// Optimize Classes
	{
		i3ClassMeta * pMeta = i3SceneOptimizer::static_meta();
	
		i3ClassMeta * pOptMeta;
		i3SceneOptimizer * pOpt;
		TCHAR conv[256];

		pOptMenu = new CMenu;
		pOptMenu->CreatePopupMenu();

	//	pMeta->i3ClassMeta::GetAllDerivedClasses( MetaList, true);
		pMeta->get_all_derived_metas(MetaList, true);

		i3::sort( MetaList.begin(), MetaList.end(), &_MetaCompareProc);
//		MetaList.Sort( (COMPAREPROC) _MetaCompareProc);

		for(size_t i = 0; i < MetaList.size(); i++)
		{
			pOptMeta = MetaList[i];

			pOpt = (i3SceneOptimizer *) pOptMeta->create_instance();	// CREATEINSTANCE( pOptMeta);
			if( pOpt == nullptr)
				continue;

			I3_MUST_ADDREF(pOpt);

			if( pOpt->isInternalOnly() == false)
			{
				if( pOpt->getHelpText() != nullptr)
					i3::snprintf( conv, _countof(conv), _T("%s (%s)"), pOptMeta->class_name(), pOpt->getHelpText());
				else
					i3::snprintf( conv, _countof(conv), _T("%s"), pOptMeta->class_name() );

				pOptMenu->AppendMenu( MF_STRING, (INT_PTR) pOptMeta, conv);
			}

			I3_MUST_RELEASE(pOpt);
		}

		pPopup->InsertMenu( MF_BYCOMMAND, MF_POPUP, (INT_PTR) pOptMenu->m_hMenu, _T("Optimizer") );
	}


	OnPopupMenu( pPopup, hSel);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, nullptr);

	delete pOptMenu;

	switch( cmd)
	{
		case ID_NODE_EXPAND :					Expand( hSel);	break;
		case ID_NODE_COLLAPSE :					Collapse( hSel);	break;
		case ID_NODE_SELECTALL :				SelectAll();		break;
		case ID_NODE_UNSELECTALL :				UnselectAll();	break;
		case ID_NODE_SELECTALLDESCENDANT :		SelectDescendant( hSel);	break;
		case ID_NODE_UNSELECTALLDESCENDANTS :	UnselectDescendant( hSel);	break;
		case ID_NODE_SELECTBYTYPE :				SelectByType();	break;
		case ID_NODE_REVERSESELECTION :			ReverseSelection();	break;

		case ID_NODE_DELETE :					Delete( hSel);		break;
		case ID_NODE_COLLAPSENODE :				CollapseNode( hSel);	break;
		case ID_NODE_COLLAPSETRANSFORM :		CollapseTransform( hSel);	break;
		case ID_NODE_REMOVENODENAMES :			RemoveNodeName();	break;
		case ID_NODE_INSERTNODE :				InsertNode( hSel);	break;

		case ID_SHOW_NAME :						ToggleShowName();	break;
		case ID_SHOW_FLAGS :					ToggleShowFlags();	break;
		case ID_SHOW_RENDERATTRIBUTES :			ToggleShowAttrs();	break;

		case ID_BOUNDBOX_SETBOUNDBOX :			SetBoundBox();		break;
		case ID_NODE_REMOVEBOUNDBOX :			RemoveBoundBox();	break;
		case ID_BOUNDBOX_VISIBLEBOUNDBOX :		VisibleBoundBox( hSel);	break;

		case ID_ATTRIBUTE_INSERTATTRIBUTES :	InsertAttr( hSel);	break;
		case ID_ATTRIBUTE_REMOVEATTRIBUTEFROMDESCENDENTS :	break;

		case ID_NODE_IMPORTSCENE :				ImportNode( hSel);	break;
		case ID_NODE_EXPORTSCENE :				ExportNode( hSel);	break;

		default :
			// i3SceneOptimizer Class를 선택한 것일 수 있기 때문에...
//			if( MetaList.IndexOf( (void *) cmd) != -1)

			if (i3::vu::index_of(MetaList, reinterpret_cast<i3ClassMeta*>(cmd) ) < MetaList.size() )
			{
				// Optimizer를 선택했다.
				i3ClassMeta * pMeta = (i3ClassMeta *) cmd;

				OnOptimize( pMeta, hSel);
			}
			break;
	}

	OnSelectMenu( cmd);
	
	*pResult = 1;
}

/////////
static BOOL _ExpandProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	if( pData == nullptr)
		pCtrl->Expand( hItem, TVE_EXPAND);
	else
		pCtrl->Expand( hItem, TVE_COLLAPSE);

	return TRUE;
}

void i3TDKSceneGraphTreeCtrl::Expand( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _ExpandProc, nullptr);
}

void i3TDKSceneGraphTreeCtrl::Collapse( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _ExpandProc, (void *) 1);
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

void i3TDKSceneGraphTreeCtrl::SelectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, nullptr);
}

void i3TDKSceneGraphTreeCtrl::UnselectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 1);
}

void i3TDKSceneGraphTreeCtrl::ReverseSelection(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 2);
}

void i3TDKSceneGraphTreeCtrl::SelectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, nullptr, hItem);
}

void i3TDKSceneGraphTreeCtrl::UnselectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 1, hItem);
}

/////////
static BOOL _SelectByMetaProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
//	List * pList = (List *) pData;
	i3::vector<i3ClassMeta*>& List = *reinterpret_cast<i3::vector<i3ClassMeta*>*>(pData);

	
	i3ClassMeta * pMeta;
	i3Node * pNode;

	for(size_t i = 0; i < List.size(); i++)
	{
		pMeta = List[i];

		pNode = (i3Node *) pCtrl->GetItemData( hItem);

		if( pNode->kind_of( pMeta))
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

void i3TDKSceneGraphTreeCtrl::SelectByType(void)
{
	i3TDKSelectNodeTypeDialog dlg;
	HINSTANCE hOld = AfxGetResourceHandle();

	AfxSetResourceHandle( g_hInstTDK);
	INT32 rv = dlg.DoModal();
	AfxSetResourceHandle( hOld);

	if( rv == IDCANCEL)
		return;

	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectByMetaProc, & dlg.m_MetaList);
}


BOOL i3TDKSceneGraphTreeCtrl::IsValidItem( HTREEITEM hItem)
{
	TVITEM data;

	data.mask = TVIF_HANDLE;
	data.hItem = hItem;

	return m_TreeCtrl.GetItem( &data);
}

void i3TDKSceneGraphTreeCtrl::Delete( HTREEITEM hItem)
{
	i3::vector<HTREEITEM> SelList;
	
	i3Node * pNode;
	i3ElementBase * pObj;

	GetCheckedItem( SelList);

	for( size_t i = 0; i < SelList.size(); i++)
	{
		hItem = SelList[i];

		if( IsValidItem( hItem))
		{

			pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hItem);
			I3ASSERT( pObj != nullptr);

			if( i3::kind_of< i3Node* >(pObj))
			{
				pNode = (i3Node *) pObj;

				pNode->RemoveFromParent();
			}
			else if( i3::kind_of<i3RenderAttr* >(pObj))
			{
				i3RenderAttr * pAttr = (i3RenderAttr *) pObj;
				i3AttrSet * pParent;
				HTREEITEM hParent = m_TreeCtrl.GetParentItem( hItem);
				I3ASSERT( (hParent != TVI_ROOT) && (hParent != nullptr));

				pParent = (i3AttrSet *) m_TreeCtrl.GetItemData( hParent);
				I3ASSERT( i3::kind_of<i3AttrSet*>(pParent));

				pParent->RemoveAttr( pAttr);
			}
			else if( i3::kind_of< i3Collider* >(pObj))
			{
			}

			m_TreeCtrl.DeleteItem( hItem);
		}
	}
}

void i3TDKSceneGraphTreeCtrl::CollapseNode( HTREEITEM hItem)
{
	i3Node * pNode, * pChild;
	i3Node * pParent;
	HTREEITEM hParent;
	i3::vector<HTREEITEM> SelList;


	GetCheckedItem( SelList);

	for(size_t i = 0; i < SelList.size(); i++)
	{
		hItem = SelList[i];

		if( IsValidItem( hItem) == FALSE)
			continue;

		pNode = (i3Node *) m_TreeCtrl.GetItemData( hItem);
		if( pNode == nullptr)
			continue;

		pParent = pNode->GetParent();
		if( pParent == nullptr)
		{
			AfxMessageBox( _T("선택 노드는 Root 노드이기 때문에 Collapse할 수 없습니다."));
			return;
		}

		hParent = m_TreeCtrl.GetParentItem( hItem);

		// Node의 재연결
		for(; pNode->GetChildCount() > 0;)
		{
			pChild = pNode->GetChild( 0);

			I3_MUST_ADDREF(pChild);

			pNode->RemoveChild( pChild);
			pParent->AddChild( pChild);

			Rec_SetTree( hParent, pChild);

			I3_MUST_RELEASE(pChild);
		}

		pParent->RemoveChild( pNode);
		m_TreeCtrl.DeleteItem( hItem);
	}

}

void i3TDKSceneGraphTreeCtrl::CollapseTransform( HTREEITEM hItem)
{
	i3Node * pNode, * pChild;
	i3Node * pParent;
	HTREEITEM hParent;
	i3::vector<HTREEITEM> SelList;
	i3OptCollapseTransform opt;
	MATRIX tmx;
	bool bRoot = false;

	GetCheckedItem( SelList);

	for( size_t i = 0; i < SelList.size(); i++)
	{
		hItem = SelList[i];

		if( IsValidItem( hItem) == FALSE)
			continue;

		pNode = (i3Node *) m_TreeCtrl.GetItemData( hItem);
		if( pNode == nullptr)
			continue;

		if( !i3::kind_of<i3Transform*>(pNode))
			continue;

		{
			i3Transform * pTrans = (i3Transform *) pNode;

			i3Matrix::Copy( &tmx, pTrans->GetMatrix());
		}

		hParent = m_TreeCtrl.GetParentItem( hItem);
		pParent = (i3Node *) m_TreeCtrl.GetItemData( hParent);

		I3_MUST_ADDREF(pNode);

		if( pNode == m_pSgInfo->getCommonSg())
		{
			if( pNode->GetChildCount() == 1)
			{
				m_pSgInfo->setCommonSg( pNode->GetChild(0));
			}
			else
			{
				pParent = i3Node::new_object_ref();
				pParent->SetName( "collapsed");
				m_pSgInfo->setCommonSg( pParent);

				bRoot = true;
				hParent = hItem;
				m_TreeCtrl.SetItemData( hItem, (DWORD_PTR) pParent);
			}
		}
		else if( pNode == m_pSgInfo->getInstanceSg())
		{
			if( pNode->GetChildCount() == 1)
			{
				m_pSgInfo->setInstanceSg( pNode->GetChild( 0));

				if( pNode->GetParent() != nullptr)
				{
					pNode->GetParent()->AddChild( pNode->GetChild(0));
				}
			}
			else
			{
				pParent = i3Node::new_object_ref();
				pParent->SetName( "collapsed");
				m_pSgInfo->setInstanceSg( pParent);

				if( pNode->GetParent() != nullptr)
				{
					pNode->GetParent()->AddChild( pParent);
				}

				bRoot = true;
				hParent = hItem;
				m_TreeCtrl.SetItemData( hItem, (DWORD_PTR) pParent);
			}
		}

		// Node의 재연결
		for(; pNode->GetChildCount() > 0;)
		{
			pChild = pNode->GetChild( 0);

			I3_MUST_ADDREF(pChild);

			pNode->RemoveChild( pChild);

			if( pParent != nullptr)
				pParent->AddChild( pChild);

			opt.CollapseTransform( &tmx, pChild);

			if( bRoot == false)
			{
				Rec_SetTree( hParent, pChild);
			}

			I3_MUST_RELEASE(pChild);
		}

		pNode->RemoveFromParent();
		I3_MUST_RELEASE(pNode);

		if( bRoot == false)
		{
			m_TreeCtrl.DeleteItem( hItem);
		}
	}

}

void i3TDKSceneGraphTreeCtrl::RemoveNodeName(void)
{
	i3::vector<HTREEITEM> List;
	i3Node * pNode;
	HTREEITEM hItem;
	TCHAR szName[256];

	GetCheckedItem( List);

	for(size_t i = 0; i < List.size(); i++)
	{
		hItem = List[i];

		pNode = (i3Node *) m_TreeCtrl.GetItemData( hItem);
		I3ASSERT( pNode != nullptr);

		pNode->SetName( "");

		MakeItemName( szName, _countof(szName), pNode);
		m_TreeCtrl.SetItemText( hItem, szName);
	}
}

void i3TDKSceneGraphTreeCtrl::RemoveBoundBox(void)
{
	i3::vector<HTREEITEM> List;

	i3Node * pNode;
	HTREEITEM hItem;

	GetCheckedItem( List);

	for( size_t i = 0; i < List.size(); i++)
	{
		hItem = List[i];

		pNode = (i3Node *) m_TreeCtrl.GetItemData( hItem);
		I3ASSERT( pNode != nullptr);

		if( pNode->GetBound() != nullptr)
		{
			pNode->SetBound( nullptr);
		}
	}
}

void i3TDKSceneGraphTreeCtrl::SetBoundBox(void)
{
	i3::vector<HTREEITEM> List;
	i3Node * pNode;
	HTREEITEM hItem;

	GetCheckedItem( List);

	for(size_t i = 0; i < List.size(); i++)
	{
		hItem = List[i];

		pNode = (i3Node *) m_TreeCtrl.GetItemData( hItem);
		I3ASSERT( pNode != nullptr);

		{
			i3BoundBox * pBox = i3BoundBox::new_object_ref();

			pBox->Reset();
			pNode->GetWrappingBound( pBox);

			pNode->SetBound( pBox);
		}
	}
}

void		i3TDKSceneGraphTreeCtrl::VisibleBoundBox( HTREEITEM hItem)
{
	i3Node * pNode;

	pNode = (i3Node *) m_TreeCtrl.GetItemData( hItem);

	i3SceneUtil::BuildVisibleBound( pNode);
}

void i3TDKSceneGraphTreeCtrl::InsertNode( HTREEITEM hParentItem)
{
	i3TDKSingleSelectNodeTypeDialog dlg;

	HINSTANCE hOld = AfxGetResourceHandle();
	AfxSetResourceHandle( g_hInstTDK);

	if( dlg.DoModal() == IDOK)
	{
		i3Node * pParentNode = (i3Node *) m_TreeCtrl.GetItemData( hParentItem);
		i3Node * pNewNode;
		i3ClassMeta * pMeta;

#ifdef _UNICODE
		i3::stack_string strMeta;	i3::utf16_to_mb(dlg.m_szSelectedMeta, strMeta);
		const char* szMeta = strMeta.c_str();
#else
		const char* szMeta = dlg.m_szSelectedMeta;
#endif
		pMeta = i3ClassMetaSystem::i()->find_class_meta(szMeta);
			//i3ClassMeta::FindClassMetaByName( dlg.m_szSelectedMeta);
		if( pMeta == nullptr)
		{
			DebugBox( m_hWnd, "%s 이름의 Node Class를 찾을 수 없습니다.", dlg.m_szSelectedMeta);
			return;
		}

		pNewNode = (i3Node *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
		if( pNewNode != nullptr)
		{
			if( pParentNode == nullptr)
			{
				// CommonSg 이거나 InstanceSg Root인 경우.
				if( hParentItem == m_hCommonRootItem)
				{
					m_pSgInfo->setCommonSg( pNewNode);
				}
				else
				{
					m_pSgInfo->setInstanceSg( pNewNode);
				}
			}
			else
			{
				pParentNode->AddChild( pNewNode);
			}

			Rec_SetTree( hParentItem, pNewNode);
		}
	}

	AfxSetResourceHandle( hOld);
}

void i3TDKSceneGraphTreeCtrl::InsertAttr( HTREEITEM hParentItem)
{
	i3TDKSelectAttrTypeDialog dlg;

	HINSTANCE hOld = AfxGetResourceHandle();
	AfxSetResourceHandle( g_hInstTDK);

	if( dlg.DoModal() == IDOK)
	{
		i3AttrSet * pParentNode = (i3AttrSet *) m_TreeCtrl.GetItemData( hParentItem);
		i3RenderAttr * pAttr;
		i3ClassMeta * pMeta;
		
		for( size_t i = 0; i < dlg.m_MetaList.size(); i++)
		{
			pMeta = dlg.m_MetaList[i];

			if( pMeta == nullptr)
			{
				DebugBox( m_hWnd, "해당 이름의 Attribute Class를 찾을 수 없습니다.");
				return;
			}

			pAttr = (i3RenderAttr *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
			if( pAttr != nullptr)
			{
				pParentNode->AddAttr( pAttr);

				AddTreeItem( pAttr, hParentItem);
			}
		}
	}

	AfxSetResourceHandle( hOld);
}

/////////
static BOOL _GetCheckedItemProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3::vector<HTREEITEM>& List = *reinterpret_cast<i3::vector<HTREEITEM>*>(pData);

	if( pCtrl->GetCheck( hItem))
	{
		List.push_back(hItem);
	}

	return TRUE;
}

/////////
static BOOL _GetSelectedItemProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3::vector<HTREEITEM>& List = *reinterpret_cast<i3::vector<HTREEITEM>*>(pData);

	if( pCtrl->GetItemState( hItem, 0xFFFFFFFF) & TVIS_SELECTED)
	{
		List.push_back(hItem);
	}

	return TRUE;
}

void i3TDKSceneGraphTreeCtrl::GetCheckedItem( i3::vector<HTREEITEM>& List)
{
	if( m_TreeCtrl.GetStyle() & TVS_CHECKBOXES)
	{
		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _GetCheckedItemProc, &List);
	}

	if( List.size() <= 0)
	{
		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _GetSelectedItemProc, &List);
	}
}

void i3TDKSceneGraphTreeCtrl::OnPopupMenu( CMenu * pMenu, HTREEITEM)
{
}

void i3TDKSceneGraphTreeCtrl::OnSelectMenu( UINT)
{
}

void i3TDKSceneGraphTreeCtrl::_DoOptimize( i3SceneOptimizer * pOpt, HTREEITEM hItem, i3Node * pRoot)
{
	bool bProcess = false;

	if( g_pViewerCtrl != nullptr)
		g_pViewerCtrl->SuspendRender();

	pOpt->Optimize( m_pSgInfo);

	if( bProcess == false)
	{
		// 임의의 Node
		m_TreeCtrl.DeleteAllItems();

		setSgInfo( pOpt->getSg());
	}

	if( g_pViewerCtrl != nullptr)
		g_pViewerCtrl->ResumeRender();
}

struct OPTPROGINFO
{
	i3SceneOptimizer *	m_pOpt = nullptr;
	i3TDKSceneGraphTreeCtrl *	m_pWnd = nullptr;
} ;

static DWORD WINAPI _PopupOptProgressThread( LPVOID pData)
{
	OPTPROGINFO * pInfo = (OPTPROGINFO *) pData;

	i3TDKDlgOptProgress dlg;

	dlg.Execute( pInfo->m_pOpt);

	delete pInfo;

	return TRUE;
}

void i3TDKSceneGraphTreeCtrl::PopupOptProgress( i3SceneOptimizer * pOpt)
{
	//원활한 처리를 위해 Background Thread로 돌린다.
	DWORD idThread;
	OPTPROGINFO * pInfo;

	pInfo = new OPTPROGINFO;

	pInfo->m_pOpt = pOpt;
	pInfo->m_pWnd = this;

	::CreateThread( nullptr, 16384, _PopupOptProgressThread, pInfo, 0, &idThread);

	SleepEx( 500, FALSE);
}

void i3TDKSceneGraphTreeCtrl::OnOptimize( i3ClassMeta * pMeta, HTREEITEM hItem)
{
	i3SceneOptimizer * pOpt;
	i3ElementBase * pObj = nullptr;
	i3Node * pNode = nullptr;

	pOpt = (i3SceneOptimizer *)	pMeta->create_instance(); // CREATEINSTANCE( pMeta);
	I3ASSERT( pOpt != nullptr);

	I3_MUST_ADDREF(pOpt);

	if( i3TDK::PopupOptimizerSpecDialog( pOpt, this) == FALSE)
	{
		I3_MUST_RELEASE(pOpt);
		return;
	}

	if( pOpt->isSupportProgress())
	{
		PopupOptProgress( pOpt);
	}

	if( hItem != nullptr)
	{
		pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hItem);

		if( pObj != nullptr)
		{
			if( !i3::kind_of<i3Node*>(pObj))
			{
				AfxMessageBox( _T("Node를 선택해야 합니다.") );
				I3_MUST_RELEASE(pOpt);
				return;
			}
		}

		pNode = (i3Node *) pObj;
	}

	_DoOptimize( pOpt, hItem, pNode);

	I3_MUST_RELEASE(pOpt);
}

void i3TDKSceneGraphTreeCtrl::OnNMCustomdrawBoneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	HDC dc = pNMCD->nmcd.hdc;
	RECT * pRect = & pNMCD->nmcd.rc;
	INT32 len;
	SIZE sz;

	switch( pNMCD->nmcd.dwDrawStage)
	{
		//case CDDS_POSTERASE :
		//case CDDS_POSTPAINT :
		//case CDDS_PREERASE :
		case CDDS_PREPAINT :		*pResult = CDRF_NOTIFYITEMDRAW;	return;

		//case CDDS_ITEM :
		//case CDDS_ITEMPOSTERASE :
		//case CDDS_ITEMPOSTPAINT :
		//case CDDS_ITEMPREERASE :
		case CDDS_ITEMPREPAINT :
			{
				HTREEITEM hItem = (HTREEITEM) pNMCD->nmcd.dwItemSpec;
				i3ElementBase * pElm = (i3ElementBase *) pNMCD->nmcd.lItemlParam;

				if( pElm != nullptr )
				{
					if( i3::kind_of<i3Node*>(pElm))
					{
						CRect rc;
						i3Node * pNode = (i3Node *) pElm;
						TCHAR conv[256];
					

						m_TreeCtrl.GetItemRect( hItem, &rc, TRUE);

						rc.left = rc.right;
						rc.right = pRect->right;

						if( m_ShowStyle & I3TDK_SGTREE_SHOW_FLAGS)
						{
							// Flags
							SetTextColor( dc, RGB( 192, 92, 92));

							i3TDK::GetNodeFlagString( pNode, conv, _countof(conv));

							len = i3::generic_string_size( conv);
							DrawText( dc, conv, len, &rc, 0);

							GetTextExtentPoint32( dc, conv, len, &sz);

							sz.cx += 8;
							rc.left += sz.cx;
							rc.right += sz.cx;
						}

						if( m_ShowStyle & I3TDK_SGTREE_SHOW_NAME)
						{
							SetTextColor( dc, RGB( 0, 0, 255));
#ifdef _UNICODE
							i3::stack_wstring strName;
#else
							i3::rc_string strName;
#endif

							if( pNode->hasName())
							{
#ifdef _UNICODE
								i3::mb_to_utf16( pNode->GetNameString(), strName);
#else
								strName = pNode->GetNameString();
#endif
							}
							else
							{
								strName = _T("\"\"");
							}

							DrawText( dc, strName.c_str(), strName.size(), &rc, 0);
						}

						SetTextColor( dc, pNMCD->clrText);
					}
				}

				*pResult = CDRF_DODEFAULT;
			}
			return;
		//case CDDS_SUBITEM :
	}
	
	*pResult = 0;
}

void i3TDKSceneGraphTreeCtrl::OnTVNBeginDrag( NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CPoint Pt;

	*pResult = 0;

	//Get Drag item
	m_hSrcItem = pNMTreeView->itemNew.hItem;
	if( m_hSrcItem == nullptr)
		return;

	m_TreeCtrl.Select( m_hSrcItem, TVGN_CARET);
	m_bDragging		= true;
	m_bDropEnable	= true;
	m_hTargetItem		= nullptr;

	SetCapture();

	{
		// Set class cursor to nullptr, so that change mouse cursor image.
		m_OldClassCursor = (HCURSOR) ::GetClassLong( m_TreeCtrl.m_hWnd, GCL_HCURSOR);
		::SetClassLong( m_TreeCtrl.m_hWnd, GCL_HCURSOR, 0);
	}
}


void i3TDKSceneGraphTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDragging)
	{
		HTREEITEM hNew;

		hNew = m_TreeCtrl.HitTest( point, nullptr);

		if( hNew != nullptr)
		{
			i3ElementBase * pTargetObj; 
			i3ElementBase * pSrcObj; 

			m_bDropEnable = false;

			{
				if( m_hTargetItem != nullptr)
				{
					m_TreeCtrl.SetItemState( m_hTargetItem, 0x00000000, TVIS_DROPHILITED);
				}

				m_hTargetItem = hNew;
				m_TreeCtrl.SetItemState( m_hTargetItem, 0xFFFFFFFF, TVIS_DROPHILITED);
			}

			pTargetObj = (i3ElementBase *) m_TreeCtrl.GetItemData( m_hTargetItem);
			pSrcObj = (i3ElementBase *) m_TreeCtrl.GetItemData( m_hSrcItem);

			if( i3::kind_of<i3Node*>(pSrcObj))
			{
				if( i3::kind_of<i3Node*>(pTargetObj))
					m_bDropEnable = TRUE;
			}
			else if( i3::kind_of<i3RenderAttr* >(pSrcObj))
			{
				if( i3::kind_of<i3GeometryAttr* >(pSrcObj) ||
					i3::kind_of<i3SpriteAttr* >(pSrcObj) ||
					i3::kind_of<i3BillboardAttr* >(pSrcObj) ||
					i3::kind_of<i3QuadAttr* >(pSrcObj) ||
					i3::kind_of<i3PointSpriteAttr* >(pSrcObj))
				{
					if( i3::kind_of<i3Geometry* >(pTargetObj))
						m_bDropEnable = true;
				}
				else
				{
					if( i3::kind_of<i3AttrSet* >(pTargetObj))
						m_bDropEnable = true;
				}
			}
		}

		if( m_bDropEnable == true)	SetCursor( m_hDropCursor);
		else						SetCursor( m_hNoDropCursor);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void i3TDKSceneGraphTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bDragging)
	{
		if((m_hTargetItem != nullptr) && (m_hSrcItem != nullptr) && (m_bDropEnable == true))
		{
			if( nFlags & MK_SHIFT)
				DragDrop( m_hSrcItem, m_hTargetItem, true);
			else
				DragDrop( m_hSrcItem, m_hTargetItem, false);
		}

		m_bDragging = FALSE; 
		ReleaseCapture();

		::SetClassLong( m_TreeCtrl.m_hWnd, GCL_HCURSOR, (LONG) m_OldClassCursor);
	}

	CWnd::OnLButtonUp(nFlags, point);
}


void i3TDKSceneGraphTreeCtrl::DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone)
{
	i3ElementBase * pSrcObj, * pTargetObj;

	pSrcObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hSrcItem);
	pTargetObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hTargetItem);

	if( i3::kind_of<i3Node*>(pSrcObj))
	{
		i3Node * pSrcNode = (i3Node *) pSrcObj;
		i3Node * pTargetNode = (i3Node *) pTargetObj;

		if( bClone)
		{
			pSrcNode = i3Scene::CreateClone( pSrcNode, I3_COPY_INSTANCE);
		}

		pSrcNode->ChangeParent(pTargetNode);

		Rec_SetTree( hTargetItem, pSrcNode);
	}
	else if( i3::kind_of<i3RenderAttr* >(pSrcObj))
	{
		i3RenderAttr * pSrcAttr = (i3RenderAttr *) pSrcObj;

		if( bClone)
		{
		//	i3ClassMeta * pMeta = pSrcAttr->GetMeta();
			i3RenderAttr * pNewAttr = pSrcAttr->create_instance(); //(i3RenderAttr *) CREATEINSTANCE( pMeta);
			pSrcAttr->CopyTo( pNewAttr, I3_COPY_INSTANCE);

			pSrcAttr = pNewAttr;
		}

		if( i3::kind_of<i3Geometry*>(pTargetObj))
		{
			i3Geometry * pGeo = (i3Geometry *) pTargetObj;

			pGeo->AppendGeometryAttr( pSrcAttr);
		}
		else
		{
			i3AttrSet * pAttrSet = (i3AttrSet *) pTargetObj;

			pAttrSet->AddAttr( pSrcAttr);
		}

		AddTreeItem( pSrcAttr, hTargetItem);
	}

	if( bClone == FALSE)
	{
		m_TreeCtrl.DeleteItem( hSrcItem);
	}
}

void i3TDKSceneGraphTreeCtrl::OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew)
{
}

void i3TDKSceneGraphTreeCtrl::OnSetSceneGraph(void)
{
}

void i3TDKSceneGraphTreeCtrl::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTREEVIEW * pNMTV = (NMTREEVIEW *) pNMHDR;

	OnChangeSelectItem( pNMTV->itemOld.hItem, pNMTV->itemNew.hItem);

	i3ElementBase * pOldObj = (i3ElementBase *) pNMTV->itemOld.lParam;
	i3ElementBase * pNewObj = (i3ElementBase *) pNMTV->itemNew.lParam;

	if( pOldObj != nullptr)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pOldObj);
	}

	if( pNewObj != nullptr)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pNewObj);
	}

	*pResult = 0;
}

void i3TDKSceneGraphTreeCtrl::OnTVNKeyDown( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTVKEYDOWN * pNMTV = (NMTVKEYDOWN *) pNMHDR;
	
	switch( pNMTV->wVKey)
	{
		case 'D' :
			{
				HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

				if( hItem != nullptr)
				{
					i3ElementBase * pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hItem);

					if( i3::kind_of<i3Node*>(pObj))
					{
						i3Node * pNode = (i3Node *) pObj;

						if( pNode->GetFlag() & I3_NODEFLAG_DISABLE)
							pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
						else
							pNode->AddFlag( I3_NODEFLAG_DISABLE);

						Invalidate();
					}
				}
			}
			break;
	}

	*pResult = 0;
}


void i3TDKSceneGraphTreeCtrl::ImportNode( HTREEITEM hParentItem)
{
	CString FileName;
	i3Node * pScene = nullptr;
	i3Node * SelectNode; 

	CFileDialog	Dlg( TRUE, _T("i3s"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("I3Engine File(*.i3s)|*.i3s||"), this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	FileName = Dlg.GetPathName();
#ifdef _UNICODE
	i3::stack_string	strFilename;	i3::utf16_to_mb(FileName, strFilename);
	const char* szFilename = strFilename.c_str();
#else
	const char* szFilename = FileName;
#endif
	{
		i3SceneFile file; 

		if( file.Load(szFilename) == STREAM_ERR)
		{			
			return; 
		}
		pScene =  file.GetSceneGraph(); 

		if(pScene == nullptr) 
		{
			//g_pMainWnd->Log( I3GRAP_LOG_ERROR,"File Load Fail - %s ", (char *)LPCTSTR(FileName) ); 
			return; 
		}

		I3_MUST_ADDREF(pScene);
	}

	SelectNode = (i3Node *) m_TreeCtrl.GetItemData( hParentItem);
	if( SelectNode != nullptr)
	{
		SelectNode->AddChild( pScene);
		SelectNode->Instantiate();

		i3Scene::ModifyDynamicState( pScene, I3_PHYSIX_STATE_KINEMATIC, 0);
	}

	I3_MUST_RELEASE(pScene);

	Rec_SetTree( hParentItem, pScene);
}

void i3TDKSceneGraphTreeCtrl::ExportNode(HTREEITEM hSelItem)
{
	CString FileName;
	i3Node * SelectNode; 

	CFileDialog	Dlg( FALSE, _T("i3s"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		_T("I3Engine Scene File(*.i3s)|*.i3s||"), this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	FileName = Dlg.GetPathName();

#ifdef _UNICODE
	i3::stack_string	strFilename;	i3::utf16_to_mb(FileName, strFilename);
	const char* szFilename = strFilename.c_str();
#else
	const char* szFilename = FileName;
#endif


	SelectNode = (i3Node *) m_TreeCtrl.GetItemData( hSelItem); 

	i3ResourceFile file;

	//Set I3 File
	i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
	pInfo->setInstanceSg( SelectNode);
	file.setKeyObject( pInfo);

	//File Save
	if(file.Save(szFilename) == STREAM_ERR)
	{
		//g_pMainWnd->Log( I3GRAP_LOG_ERROR,"File Save Fail - %s ", (char *)LPCTSTR(FileName) ); 		
		return; 
	}
	//g_pMainWnd->Log( I3GRAP_LOG_NORMAL,"File Save finish - %s ", (char *)LPCTSTR(FileName) ); 

	return; 
}

//void i3TDKSceneGraphTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//
//	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
//}

//void i3TDKSceneGraphTreeCtrl::OnSetFocus(CWnd* pOldWnd)
//{
//	CWnd::OnSetFocus(pOldWnd);
//
//	SetFocus();
//}

//BOOL i3TDKSceneGraphTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	NMHDR * pHdr = (NMHDR *) lParam;
//
//	switch( pHdr->code)
//	{
//		case NM_KEYDOWN :
//			{
//				NMKEY * pKeyInfo = (NMKEY *) pHdr;
//
//				switch( pKeyInfo->nVKey)
//				{
//					case 'D' :
//						{
//							HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
//
//							if( hItem != nullptr)
//							{
//								i3ElementBase * pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hItem);
//
//								if((pObj != nullptr) && pObj->IsTypeOf( i3Node::GetClassMeta()))
//								{
//
//									i3Node * pNode = (i3Node *) pObj;
//
//									if( pNode->GetFlag() & I3_NODEFLAG_DISABLE)
//										pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
//									else
//										pNode->AddFlag( I3_NODEFLAG_DISABLE);
//
//									Invalidate();
//								}
//							}
//						}
//						break;
//				}
//			}
//			break;
//	}
//
//	return TRUE;
//}

//LRESULT i3TDKSceneGraphTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if( message == WM_NOTIFY)
//	{
//		OnNotify( wParam, lParam, nullptr);
//	}
//
//	return CWnd::WindowProc(message, wParam, lParam);
//}

HTREEITEM	i3TDKSceneGraphTreeCtrl::_FindTreeItemByObj( i3ElementBase * pObj)
{
	HTREEITEM hItem = i3TDK::FindTreeItemByData( &m_TreeCtrl, pObj);

	return hItem;
}

void i3TDKSceneGraphTreeCtrl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		if( pInfo->m_pObject != nullptr)
		{
			HTREEITEM hItem = _FindTreeItemByObj( pInfo->m_pObject);

			if( hItem != nullptr)
			{
				TVITEM info;

#ifdef _UNICODE
				i3::stack_wstring strMeta;	i3::mb_to_utf16(pInfo->m_pObject->meta()->class_name(), strMeta);
#else
				const i3::fixed_string& strMeta = pInfo->m_pObject->meta()->class_name();
#endif
				_SetTreeItem( &info, pInfo->m_pObject, strMeta.c_str(), hItem);

				m_TreeCtrl.SetItem( &info);
			}
		}
	}
}

LRESULT i3TDKSceneGraphTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

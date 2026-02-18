// i3Tree.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "i3Tree.h"
#include ".\i3tree.h"
#include "i3GraphDlg.h"
#include "TreeAddNode.h"
#include "TreeAddAttr.h"
#include "SelectBoxFaceDialog.h"
#include "DeleteNameDlg.h"

IMPLEMENT_DYNAMIC(Ci3Tree, CDialog)
Ci3Tree::Ci3Tree(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3Tree::IDD, pParent)
{
	m_pSelElement	= NULL; 
	m_pScene		= NULL; 
}

Ci3Tree::~Ci3Tree()
{
}

void Ci3Tree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SCENE, m_TreeControl);
}


BEGIN_MESSAGE_MAP(Ci3Tree, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SCENE, OnTvnSelchangedTreeScene)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_SCENE, OnNMRclickTreeScene)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_SCENE, OnTvnBegindragTreeScene)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_COMMAND(ID_NODE_GEOMETRYTOOL, OnNodeGeometrytool)
END_MESSAGE_MAP()


// Ci3Tree message handlers

int Ci3Tree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

void Ci3Tree::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
//	OnOK();
}

void Ci3Tree::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
//	OnCancel();
}

BOOL Ci3Tree::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//Set Value

	//Set Tree Control 
	CRect rect;
	GetClientRect( &rect);
	m_TreeControl.MoveWindow(&rect , TRUE);

	//Set Image Icon
	HICON hIcon;
	m_ImageList.Create( 16, 16, ILC_COLOR8, 10, 0);

	// Node Icon
	{
		hIcon = (HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_NODE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);
		m_ImageList.Add( hIcon);

		hIcon = (HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_SELNODE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);
		m_ImageList.Add( hIcon);

		hIcon = (HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_ATTR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);
		m_ImageList.Add( hIcon);

		hIcon = (HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_SELATTR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);
		m_ImageList.Add( hIcon);
	}

	m_TreeControl.SetImageList( &m_ImageList, TVSIL_NORMAL);


	{
		m_hDragCursor = (HCURSOR) ::LoadImage( AfxGetApp()->m_hInstance, 
			MAKEINTRESOURCE( IDC_POINTER), IMAGE_CURSOR,
			32, 32, LR_VGACOLOR);

		m_hNoDropCursor = (HCURSOR) ::LoadImage( AfxGetApp()->m_hInstance,
			MAKEINTRESOURCE( IDC_NODROP), IMAGE_CURSOR,
			32, 32, LR_DEFAULTCOLOR);
	}
	
	m_hDragItem = NULL; 
	m_hDropItem = NULL; 
	m_Dragging	= FALSE; 
	m_DropEnable= FALSE; 

	return TRUE; 
}

void Ci3Tree::OnTvnSelchangedTreeScene(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	//Get Sellected Item 
	HTREEITEM hSelItem; 
	UINT Flag; 
	POINT Pt; 

	GetCursorPos( &Pt ); 

	hSelItem = m_TreeControl.GetSelectedItem(); 
    
	if(hSelItem == NULL)
	{
		hSelItem = m_TreeControl.HitTest( Pt, &Flag);

		if( (hSelItem != NULL) && (Flag & TVHT_ONITEM))
		{
			m_TreeControl.Select( hSelItem, Flag);
		}
	}
	
	if(hSelItem != NULL)
	{
		//goat여기에서 선택된것을 넣는다. 		
		m_pSelElement = (i3ElementBase *) m_TreeControl.GetItemData( hSelItem);
		g_pMainWnd->SetPropertyWnd(m_pSelElement); 
	}
		
	*pResult = 0;
}

void Ci3Tree::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if( !IsWindow( m_TreeControl.m_hWnd ) )	return; 
	m_TreeControl.MoveWindow(0, 0, cx, cy, TRUE);
}

/*=============================================================
//Name : void Ci3Tree::SetSceneGraph(i3Node * pNode)
//Desc :
=============================================================*/
void Ci3Tree::SetSceneGraph(i3Node * pNode)
{
	m_pScene = pNode; 
	AssignSceneGraphToTree(); 
	return; 
}

/*=============================================================
//Name : void Ci3Tree::AssignSceneGraphToTree(void)
//Desc :
=============================================================*/
void Ci3Tree::AssignSceneGraphToTree(void)
{
	//Delete All Tree Info	
	m_TreeControl.DeleteAllItems(); 

	if(m_pScene == NULL)return; 

	m_hParent = TVI_ROOT;	
	i3SceneGraphTree(m_pScene); 	
}

/*=============================================================
//Name : void Ci3Tree::PutTreeNode(HTREEITEM hItem, i3ElementBase * pElement)
//Desc :
=============================================================*/
void Ci3Tree::PutTreeNode(HTREEITEM hItem, i3ElementBase * pElement)
{
	TVITEM			Item; 
	INT32			Icon; 
	i3ClassMeta		* pMeta; 
	i3RenderAttr	* pAttr;
	i3Node			* pNode; 
	i3Geometry		* pGeo; 
	i3AttrSet		* pAttrSet; 
	INT32			ChildCount; 
	char			s_temp[256];

	//Get Meta
	pMeta = pElement->GetMeta(); 

	//For String 
	strcpy(s_temp, ""); 

	
	if(pElement->IsTypeOf(i3RenderAttr::GetClassMeta()))
	{//ATTR
		pAttr = (i3RenderAttr *)pElement; 
		Icon = 2; 
		strcpy(s_temp, pAttr->GetMeta()->GetClassName()); 		
		ChildCount = 0; 
	}
	else if( pElement->IsTypeOf( i3Node::GetClassMeta()))
	{
		//NODE
		pNode = (i3Node *)pElement; 
		if( pNode->GetName() != NULL)
		{			
			strcpy(s_temp, pNode->GetName());
		}
		Icon = 0; 
		sprintf(s_temp,"%s [%s]",s_temp, pNode->GetMeta()->GetClassName()); 

		//For ChildCount
		if(pElement->GetMeta() == i3Geometry::GetClassMeta())
		{
			pGeo = (i3Geometry *)pElement; 
			ChildCount = pGeo->GetChildCount() + pGeo->GetGeometryAttrCount(); 
		}
		else if(pElement->GetMeta() ==  i3AttrSet::GetClassMeta())
		{
			pAttrSet = (i3AttrSet *)pElement; 
			ChildCount = pAttrSet->GetChildCount() + pAttrSet->GetAttrCount(); 
		}
		else if( pElement->IsTypeOf( i3ColliderSet::GetClassMeta()))
		{
			i3ColliderSet * pColl = (i3ColliderSet *) pElement;
			ChildCount = pNode->GetChildCount() + pColl->GetColliderCount(); 
		}
		else
		{
			ChildCount = pNode->GetChildCount();
		}
		//ChildCount ++;
	}
	else if( pElement->IsTypeOf( i3Collider::GetClassMeta()))
	{
		Icon = 2; 
		strcpy(s_temp, pElement->GetMeta()->GetClassName()); 		
		ChildCount = 1; 
	}
	else
	{
		Icon = 2; 
		strcpy(s_temp, pElement->GetMeta()->GetClassName()); 		
		ChildCount = 0; 
	}

	if (ChildCount < 0) ChildCount = 0;     
	Item.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	Item.hItem = hItem;
	Item.pszText = s_temp;
	Item.iImage = Icon;
	Item.iSelectedImage = Icon + 1;
	Item.cChildren = ChildCount;
	//	Item.cChildren = 0;
	Item.lParam =  (LPARAM) pElement;
	
	m_TreeControl.SetItem( &Item);

	return; 
}

/*=============================================================
//Name : void Ci3Tree::i3SceneGraphTree(i3Node * pNode)
//Desc :
=============================================================*/
void Ci3Tree::i3SceneGraphTree(i3Node * pNode)
{
	INT32 i;
	i3ClassMeta * pMeta = pNode->GetMeta();
	HTREEITEM hOldParent = m_hParent;
	HTREEITEM hItem;

	{
		m_hParent = m_TreeControl.InsertItem("", m_hParent);
		PutTreeNode( m_hParent, (i3ElementBase *)pNode);
	}

	if( pNode->IsTypeOf( i3AttrSet::GetClassMeta()))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			hItem = m_TreeControl.InsertItem( "", m_hParent);
			PutTreeNode( hItem, (i3ElementBase *)pAttrSet->GetAttr(i));
		}
	}
	else if( pNode->IsTypeOf( i3Geometry::GetClassMeta()))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr(i);
			hItem = m_TreeControl.InsertItem( "", m_hParent);			
			PutTreeNode( hItem, (i3ElementBase *)pAttr);
		}
	}
	else if( pNode->IsTypeOf( i3ColliderSet::GetClassMeta()))
	{
		i3ColliderSet * pSet = (i3ColliderSet *) pNode;

		for( i = 0; i < pSet->GetColliderCount(); i++)
		{
			i3Collider * pCollider = pSet->GetCollider( i);

			hItem = m_TreeControl.InsertItem( "", m_hParent);
			PutTreeNode( hItem, pCollider);

			hItem = m_TreeControl.InsertItem( "", hItem);
			PutTreeNode( hItem, pCollider->GetCollidee());
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3SceneGraphTree( pNode->GetChild( i));
	}

	m_hParent = hOldParent;
}

/*=============================================================
//Name : void Ci3Tree::OnNMRclickTreeScene(NMHDR *pNMHDR, LRESULT *pResult)
//Desc : 
=============================================================*/
void Ci3Tree::OnNMRclickTreeScene(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	POINT		Pt, Pt2;
	CMenu		Menu, * pPopUp; 	
	UINT		Flag, Cmd;
	HTREEITEM	hSelItem; 

	GetCursorPos( &Pt);
	Pt2 = Pt;
	m_TreeControl.ScreenToClient( &Pt2);

	hSelItem = m_TreeControl.HitTest( Pt2, &Flag);			

	if( (hSelItem != NULL) && (Flag & TVHT_ONITEM))
	{
		m_TreeControl.Select( hSelItem, TVGN_CARET);
	}
	
	if( hSelItem != NULL)
	{
		m_pSelElement = (i3ElementBase*) m_TreeControl.GetItemData( hSelItem ); 	
		g_pMainWnd->SetPropertyWnd(m_pSelElement); 
	}else
	{
		*pResult = 0;
		return; 
	}

	//GetCursorPos(&Pt); 
	Menu.LoadMenu( MAKEINTRESOURCE( IDR_MENU_POPUP ) );
	pPopUp = Menu.GetSubMenu( 0 ); 

	if(m_pSelElement->IsTypeOf(i3RenderAttr::GetClassMeta()))
	{
		//Attr 
		pPopUp->EnableMenuItem( ID_POPUP_EXPENDNODE,	MF_GRAYED | MF_DISABLED ); 
		pPopUp->EnableMenuItem( ID_POPUP_COLLAPSENODE,	MF_GRAYED | MF_DISABLED ); 

		pPopUp->EnableMenuItem( ID_POPUP_INSERTNODE,	MF_GRAYED | MF_DISABLED ); 
		pPopUp->EnableMenuItem( ID_POPUP_IMPORTNODE,	MF_GRAYED | MF_DISABLED ); 
		pPopUp->EnableMenuItem( ID_POPUP_EXPORTNODE,	MF_GRAYED | MF_DISABLED ); 

		pPopUp->EnableMenuItem( ID_POPUP_OPTDELETENAME,		MF_GRAYED | MF_DISABLED ); 
		pPopUp->EnableMenuItem( ID_POPUP_OPTDELETENAME_DES,	MF_GRAYED | MF_DISABLED ); 
	}
	
	if( m_pSelElement->GetMeta() != i3AttrSet::GetClassMeta())
	{
		pPopUp->EnableMenuItem( ID_POPUP_INSERTATTR, MF_GRAYED | MF_DISABLED);
	}

	if( m_pSelElement->GetMeta() != i3Transform::GetClassMeta() )
	{
		pPopUp->EnableMenuItem( ID_POPUP_OPTTRANFORM, MF_GRAYED | MF_DISABLED);		
	}

	Cmd = ::TrackPopupMenuEx( pPopUp->GetSafeHmenu(), TPM_VERTICAL | TPM_RETURNCMD, Pt.x, Pt.y, m_hWnd, NULL);
	
	switch(Cmd)
	{
	case ID_POPUP_EXPENDNODE		:	ContorlNode( hSelItem ,TVE_EXPAND);			break; 
	case ID_POPUP_COLLAPSENODE		:	ContorlNode( hSelItem ,TVE_COLLAPSE);		break; 
	case ID_POPUP_DELETE			: 	DeleteElement(hSelItem);					break; 
	case ID_POPUP_INSERTNODE		:	InserNode(hSelItem);						break; 
	case ID_POPUP_INSERTATTR		:	InserAttr(hSelItem);						break; 
	case ID_POPUP_IMPORTNODE		:	ImportNode(hSelItem);						break; 
	case ID_POPUP_EXPORTNODE		:	ExportNode(hSelItem);						break; 
	case ID_POPUP_SETBOUNDBOX		:	SetBoundBox(hSelItem);						break;
	case ID_POPUP_REMOVEBOUNDBOX	:	RemoveBoundBox( hSelItem);					break;
	case ID_APPENDCOLLIIDER_BOX		:	AppendColliderBox( hSelItem);				break;
	case ID_APPENDCOLLIIDER_SPHERE	:	AppendColliderSphere( hSelItem);			break;
	case ID_POPUP_OPTTRANFORM		:	Otitransform( hSelItem );					break; 
	case ID_POPUP_OPTDELETENAME		:	DeleteName( hSelItem );						break; 
	case ID_POPUP_OPTDELETENAME_DES	:	DeleteNameDes( hSelItem );					break; 

	default: break; 
	}

	*pResult = 0;
}

void Ci3Tree::DeleteName( HTREEITEM hItem )
{
	i3Node	*	pNode; 
	pNode = (i3Node *)m_TreeControl.GetItemData( hItem ); 
	pNode->SetName(NULL);	
	m_hParent = m_TreeControl.GetParentItem( hItem );


	// 다시 만들기 		
	m_hParent = m_TreeControl.GetParentItem( hItem );
	i3SceneGraphTree( pNode );

	//그전거를 지운다. 
	m_TreeControl.DeleteItem( hItem );

	return; 
}

void Ci3Tree::DeleteNameDes( HTREEITEM hItem )
{	
	i3Node	*	pNode; 
	CDeleteNameDlg NodeDlg; 
	
	if(NodeDlg.DoModal() == IDOK)
	{
		pNode = (i3Node *)m_TreeControl.GetItemData( hItem ); 
		reDeleteName(pNode, NodeDlg.GetList() ); 

		// 다시 만들기 		
		m_hParent = m_TreeControl.GetParentItem( hItem );
		i3SceneGraphTree( pNode );

		//그전거를 지운다. 
		m_TreeControl.DeleteItem( hItem );
	}


	return; 
}

void Ci3Tree::reDeleteName(i3Node * pNode, i3List * pList)
{
	INT32 i; 	
	i3ClassMeta * pMeta; 
	i3Node		* pChildNode; 

	//DelteName 
	for(i = 0; i < pList->GetCount(); i++)
	{
		pMeta = (i3ClassMeta * )pList->GetItem(i); 
		if(pNode->IsExactTypeOf(pMeta) == TRUE)
		{
			pNode->SetName(NULL); 
			break; 
		}
	}	
	
	for(i = 0; i < pNode->GetChildCount() ; i++)
	{
		pChildNode = pNode->GetChild(i); 
		reDeleteName( pChildNode, pList); 
	}

	return;
}

void Ci3Tree::Otitransform(HTREEITEM hItem)
{
	i3Transform * pNodeTrans = (i3Transform *) m_TreeControl.GetItemData( hItem);
	i3Node		* pParentNode; 
	HTREEITEM hParentItem; 
	i3List		ChildList; 
	INT32		i; 
	i3OptCollapseTransform opt;

	//0. 변수 세팅
	hParentItem = m_TreeControl.GetParentItem( hItem );
	if( hParentItem == NULL)
	{
		//이거는 루트 입니다. 
		return; 
	}else
	{
		pParentNode = (i3Node *)m_TreeControl.GetItemData( hParentItem ); 
	}

	//1. Scene 연결을 다시 함	
	i3Node * pChildNode; 

	for( i = 0; i < pNodeTrans->GetChildCount(); i++)
	{
		pChildNode = pNodeTrans->GetChild(i); 
		ChildList.Add(pChildNode); 
	}
	
	for( i = 0; i < ChildList.GetCount(); i++)
	{
		pChildNode = (i3Node *) ChildList.GetItem(i); 

		pChildNode->AddRef();
		pNodeTrans->RemoveChild( pChildNode);
		pParentNode->AddChild( pChildNode ); 
		pChildNode->Release();
	}

	MATRIX TransMat; 	

	i3Matrix::Copy( &TransMat, pNodeTrans->GetMatrix()); 	

	pParentNode->RemoveChild(pNodeTrans); 

	//TransForm아래 있던 모든 노드들에 메트릭스 연산하기. 
	for( i = 0; i < ChildList.GetCount(); i++)
	{
		pChildNode = (i3Node *) ChildList.GetItem(i); 
		opt.CollapseTransform( &TransMat, pChildNode);
	}

	{
		// 다시 만들기 		
		m_hParent = m_TreeControl.GetParentItem( hParentItem );
		i3SceneGraphTree( pParentNode ); 

		//그전거를 지운다. 
		m_TreeControl.DeleteItem( hParentItem );
	}

	return; 
}

void Ci3Tree::AppendColliderBox( HTREEITEM hItem)
{
	i3Node * pNode = (i3Node *) m_TreeControl.GetItemData( hItem);
	i3BoundBox * pBox;
	CSelectBoxFaceDialog dlg;
	INT32 tricount = 0;

	if( dlg.DoModal() == IDCANCEL)
		return;

	if( dlg.m_bLeft)		tricount += 2;
	if( dlg.m_bRight)		tricount += 2;
	if( dlg.m_bFront)		tricount += 2;
	if( dlg.m_bBack)		tricount += 2;
	if( dlg.m_bTop)			tricount += 2;
	if( dlg.m_bBottom)		tricount += 2;

	i3ColliderSet * pColSet = i3ColliderSet::NewObjectRef();

	pBox = i3BoundBox::NewObject();
	pBox->Reset();

	pNode->GetWrappingBound( pBox);

	{
		i3Collider * pCollider = i3Collider::NewObjectRef();
		i3CollideeMesh * pColl = i3CollideeMesh::NewObjectRef();
		i3Vector3Array * pVA;
		REAL32 x1, y1, x2, y2, z1, z2;

		x1 = i3Vector::GetX( pBox->GetMin());
		y1 = i3Vector::GetY( pBox->GetMin());
		z1 = i3Vector::GetZ( pBox->GetMin());

		x2 = i3Vector::GetX( pBox->GetMax());
		y2 = i3Vector::GetY( pBox->GetMax());
		z2 = i3Vector::GetZ( pBox->GetMax());

		// Position
		{
			pColl->SetPositionCount( 8);
			pVA = pColl->GetPositionArray();
			pVA->SetVector( 0, x1, y2, z1);
			pVA->SetVector( 1, x2, y2, z1);
			pVA->SetVector( 2, x1, y2, z2);
			pVA->SetVector( 3, x2, y2, z2);
			pVA->SetVector( 4, x1, y1, z1);
			pVA->SetVector( 5, x2, y1, z1);
			pVA->SetVector( 6, x1, y1, z2);
			pVA->SetVector( 7, x2, y1, z2);
		}

		// Normal
		{
			pColl->SetNormalCount( 6);
			pVA = pColl->GetNormalArray();
			pVA->SetVector( 0, -1.0f, 0.0f, 0.0f);			// Left
			pVA->SetVector( 1, 1.0f, 0.0f, 0.0f);			// Right
			pVA->SetVector( 2, 0.0f, 0.0f, -1.0f);			// Front
			pVA->SetVector( 3, 0.0f, 0.0f, 1.0f);			// Back
			pVA->SetVector( 4, 0.0f, 1.0f, 0.0f);			// Up
			pVA->SetVector( 5, 0.0f, -1.0f, 0.0f);			// Down

		}

		// Triangle
		{
			INT32 idx = 0;

			pColl->SetTriangleCount( tricount);

			if( dlg.m_bLeft)
			{
				pColl->SetTriangle( idx,	0, 4, 2,	0);		// Left Plane
				pColl->SetTriangle( idx+1,	2, 4, 6,	0);
				idx += 2;
			}

			if( dlg.m_bRight)
			{
				pColl->SetTriangle( idx,	3, 7, 1,	1);		// Right Plane
				pColl->SetTriangle( idx+1,	1, 7, 5,	1);
				idx += 2;
			}

			if( dlg.m_bFront)
			{
				pColl->SetTriangle( idx,	1, 5, 0,	2);		// Front Plane
				pColl->SetTriangle( idx+1,	0, 5, 4,	2);
				idx += 2;
			}

			if( dlg.m_bBack)
			{
				pColl->SetTriangle( idx,	2, 6, 3,	3);		// Back Plane
				pColl->SetTriangle( idx+1,	3, 6, 7,	3);
				idx += 2;
			}

			if( dlg.m_bTop)
			{
				pColl->SetTriangle( idx,	0, 2, 1,	4);		// Up Plane
				pColl->SetTriangle( idx+1,	1, 2, 3,	4);
				idx += 2;
			}

			if( dlg.m_bBottom)
			{
				pColl->SetTriangle( idx,	6, 4, 7,	5);		// Down Plane
				pColl->SetTriangle( idx+1,	7, 4, 5,	5);
			}
		}

		pCollider->SetCollidee( pColl);

		pColSet->AddCollider( pCollider);
		pColSet->SetName( "BoxColl");

		pNode->AddChild( pColSet);
	}

	m_hParent = hItem; 
	i3SceneGraphTree( pColSet);	
}

void Ci3Tree::AppendColliderSphere( HTREEITEM hItem)
{
	i3Node * pNode = (i3Node *) m_TreeControl.GetItemData( hItem);
	i3BoundBox * pBox;
	i3ColliderSet * pColSet = i3ColliderSet::NewObjectRef();

	pBox = i3BoundBox::NewObject();
	pBox->Reset();

	pNode->GetWrappingBound( pBox);

	{
		i3Collider * pCollider = i3Collider::NewObjectRef();
		i3CollideeSphere * pColl = i3CollideeSphere::NewObjectRef();
		VEC3D center, size;
		REAL32 r;

		pBox->GetCenter( &center);
		pColl->SetCenter( &center);

		i3Vector::Sub( &size, pBox->GetMax(), pBox->GetMin());
		r = MAX( i3Vector::GetX(&size), i3Vector::GetY( &size));
		r = MAX( r, i3Vector::GetZ( &size));
		
		pColl->SetRadius( r * 0.5f);

		pCollider->SetCollidee( pColl);

		pColSet->AddCollider( pCollider);
		pColSet->SetName( "SphereColl");

		pNode->AddChild( pColSet);
	}

	m_hParent = hItem; 
	i3SceneGraphTree( pColSet);	
}

void Ci3Tree::SetBoundBox(HTREEITEM hItem)
{
	i3ElementBase * pObj = (i3ElementBase *) m_TreeControl.GetItemData( hItem);

	if( pObj->IsTypeOf( i3Node::GetClassMeta()))
	{
		i3BoundBox * pBox = i3BoundBox::NewObjectRef();
		i3Node * pNode = (i3Node *) pObj;

		pBox->Reset();
		pNode->GetWrappingBound( pBox);

		pNode->SetBound( pBox);
	}
	else
	{
		AfxMessageBox( "선택할 항목은 i3Node 객체가 아닙니다.");
	}
}

void Ci3Tree::RemoveBoundBox(HTREEITEM hItem)
{
	i3ElementBase * pObj = (i3ElementBase *) m_TreeControl.GetItemData( hItem);

	if( pObj->IsTypeOf( i3Node::GetClassMeta()))
	{
		i3Node * pNode = (i3Node *) pObj;

		if( pNode->GetBound() != NULL)
		{
			pNode->SetBound( NULL);
		}
	}
	else
	{
		AfxMessageBox( "선택할 항목은 i3Node 객체가 아닙니다.");
	}
}

void Ci3Tree::InserNode(HTREEITEM hItem) 
{
	CTreeAddNode CInserDialog; 

	if( IDOK ==CInserDialog.DoModal() )
	{
		i3ClassMeta * pMeta;

		pMeta = i3ClassMeta::FindClassMetaByName((LPCTSTR)CInserDialog.m_SelectedNode);
		{
			i3Node * pNode, * pParentNode;
			pNode = (i3Node *) CREATEINSTANCE( pMeta);
			pParentNode = (i3Node *)m_pSelElement; 
			pParentNode->AddChild(pNode); 
			m_hParent = m_TreeControl.GetParentItem( hItem); 
			m_TreeControl.DeleteItem( hItem);
			i3SceneGraphTree( pParentNode);	
		}
	}
}

void Ci3Tree::InserAttr(HTREEITEM hItem)
{		
	CTreeAddAttr CAttrInsert;

	if( IDOK == CAttrInsert.DoModal() )	
	{
		i3ClassMeta * pMeta;
		pMeta = i3ClassMeta::FindClassMetaByName((LPCTSTR)CAttrInsert.m_SelectedAttr);

		i3RenderAttr * pAttr; 			
		pAttr = (i3RenderAttr *) CREATEINSTANCE( pMeta);
		i3AttrSet * pParentNode; 
		pParentNode = (i3AttrSet * )m_pSelElement;

		pParentNode->AddAttr(pAttr); 
		m_hParent = m_TreeControl.GetParentItem( hItem); 
		m_TreeControl.DeleteItem( hItem);
		i3SceneGraphTree( pParentNode);	
	}	
}

void Ci3Tree::ImportNode(HTREEITEM hItem)
{
	CString FileName;
	i3Node * pScene = NULL;
	i3Node * SelectNode; 

	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Engine File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	FileName = Dlg.GetFileName();

	{
		i3SceneFile file; 

		if (file.Load(FileName) == STREAM_ERR)
		{			
			return; 
		}
		pScene =  file.GetSceneGraph(); 
	}

	if(pScene == NULL) 
	{
		g_pMainWnd->Log( I3GRAP_LOG_ERROR,"File Load Fail - %s ", (char *)LPCTSTR(FileName) ); 		
		return; 
	}else g_pMainWnd->Log(I3GRAP_LOG_NORMAL, "File Load "" %s "" ", FileName); 	

	SelectNode = (i3Node *)m_pSelElement; 
	SelectNode->AddChild(pScene); 
	pScene->Release(); 

	m_pSelElement	=	(i3ElementBase *) pScene;
	g_pMainWnd->SetPropertyWnd(m_pSelElement); 	

	//For Tree View		
	m_hParent = m_TreeControl.GetParentItem( hItem); 
	m_TreeControl.DeleteItem( hItem);
	i3SceneGraphTree( SelectNode);	

	//For Proeprty View	
//	Invalidate(TRUE);

}

void Ci3Tree::ExportNode(HTREEITEM hItem)
{
	CString FileName;
	i3Node * SelectNode; 

	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"I3Engine File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	FileName = Dlg.GetFileName();

	SelectNode = (i3Node *)m_pSelElement; 

	i3SceneFile file;

	//Set I3 File
	file.SetSceneGraph(SelectNode);

	//File Save
	if(file.Save(FileName) == STREAM_ERR)
	{
		g_pMainWnd->Log( I3GRAP_LOG_ERROR,"File Save Fail - %s ", (char *)LPCTSTR(FileName) ); 		
		return; 
	}
	g_pMainWnd->Log( I3GRAP_LOG_NORMAL,"File Save finish - %s ", (char *)LPCTSTR(FileName) ); 

	return; 
}

/*=============================================================
//Name : void Ci3Tree::ContorlNode(HTREEITEM hItem, UINT Cmd)
//Desc : 
=============================================================*/
void Ci3Tree::ContorlNode(HTREEITEM hItem, UINT Cmd)
{
	HTREEITEM hChild;

	m_TreeControl.Expand(hItem, Cmd);

	if( hItem == NULL)
	{
		hItem = m_TreeControl.GetRootItem();
	}

	hChild = m_TreeControl.GetChildItem( hItem);

	while( hChild != NULL)
	{
		ContorlNode(hChild, Cmd);

		hChild = m_TreeControl.GetNextSiblingItem( hChild);
	}
}

/*=============================================================
//Name : void Ci3Tree::DeleteElement(HTREEITEM hItem)
//Desc : 
=============================================================*/
void Ci3Tree::DeleteElement(HTREEITEM hItem)
{
	i3ElementBase *	pSellElement; 
	pSellElement = (i3ElementBase *)m_TreeControl.GetItemData(hItem);
		
	if( pSellElement->IsTypeOf(i3RenderAttr::GetClassMeta()) )
	{
		//Attr			
		i3Node * pParent;

		HTREEITEM hParentItem;	
		hParentItem = m_TreeControl.GetParentItem( hItem);
		pParent = (i3Node *)m_TreeControl.GetItemData( hParentItem);
				
		if(pParent->GetMeta() == i3Geometry::GetClassMeta())
		{
			i3Geometry		* pGeo; 
			pGeo = (i3Geometry *)pParent; 
			pGeo->RemoveGeometryAttr((i3GeometryAttr*)pSellElement); 

		}else if(pParent->GetMeta() ==  i3AttrSet::GetClassMeta())
		{
			i3AttrSet * pAttrSet; 			
			pAttrSet = (i3AttrSet *)pParent; 
			pAttrSet->RemoveAttr((i3RenderAttr *)pSellElement); 
		}

	}else if( pSellElement->IsTypeOf(i3Node::GetClassMeta()) ) 
	{
		//Node		
		i3Node * pNode, * pParent;

		pNode		= (i3Node *)pSellElement;
		pParent		= pNode->GetParent();		

		if(pParent == NULL)
		{
			//Message 
			return; 
		}
		
		pParent->RemoveChild(pNode);
	}else
	{
		ASSERT(0); 
	}

	//Delete From Tree
	m_TreeControl.DeleteItem( hItem);

}

void Ci3Tree::OnDestroy()
{
	CDialog::OnDestroy();	
	// TODO: Add your message handler code here

	if( m_hDragCursor != NULL)
	{
		::DestroyCursor( m_hDragCursor);
	}

	if( m_hNoDropCursor != NULL)
	{
		::DestroyCursor( m_hNoDropCursor);
	}	
}

void Ci3Tree::OnTvnBegindragTreeScene(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//Get Drag item
	m_hDragItem = pNMTreeView->itemNew.hItem;
	if( m_hDragItem == NULL)return;

	m_TreeControl.Select( m_hDragItem, TVGN_CARET);
	m_Dragging		= TRUE;
	m_DropEnable	= TRUE;
	m_hDropItem		= NULL;

	SetCapture();
	{
		// Set class cursor to NULL, so that change mouse cursor image.
		m_OldClassCursor = ::GetClassLong( m_TreeControl.m_hWnd, GCL_HCURSOR);
		::SetClassLong( m_TreeControl.m_hWnd, GCL_HCURSOR, NULL);
	}
}

void Ci3Tree::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_Dragging)
	{
		POINT Pt;
		GetCursorPos( &Pt);
		m_DropEnable = FALSE; 
		m_TreeControl.ScreenToClient( &Pt);
		m_hDropItem = m_TreeControl.HitTest(Pt, NULL);
		if (m_hDropItem != NULL)
		{
			i3ElementBase * pDropElement; 
			i3ElementBase * pDragElement; 

			pDropElement = (i3ElementBase *)m_TreeControl.GetItemData( m_hDropItem);
			pDragElement = (i3ElementBase *)m_TreeControl.GetItemData( m_hDragItem);
			if(pDropElement->IsTypeOf( i3Node::GetClassMeta() ) )
			{
				//3 가지 경우가 있습니다. 
				//DropNode가 
				if( pDropElement->IsTypeOf( i3Geometry::GetClassMeta() ) )
				{
					//i3Geometry Node 일때 : pDrag가 i3Geometry 와 Node일때 된다. 
					if( pDragElement->IsTypeOf( i3GeometryAttr::GetClassMeta() ) ||
						pDragElement->IsTypeOf( i3Node::GetClassMeta() ) )
					{
						m_DropEnable = TRUE;
					}

				}else if( pDropElement->IsTypeOf( i3AttrSet::GetClassMeta() ) )
				{
                    //i3AttrSet 일때 : pDrag가 i3GeometryAttr만 아니면 다 된다. 
					if( !pDragElement->IsTypeOf( i3GeometryAttr::GetClassMeta() ) )
					{
						m_DropEnable = TRUE;
					}
				}else
				{
					//그외 노드 일때 : pDrag가 노드일때만 가능하다. 
					if( pDragElement->IsTypeOf( i3Node::GetClassMeta() ) )
					{
						m_DropEnable = TRUE;
					}
				}				
			}
		}		
		if( m_DropEnable == TRUE)	SetCursor( m_hDragCursor);
		else						SetCursor( m_hNoDropCursor);

	}

	CDialog::OnMouseMove(nFlags, point);
}

void Ci3Tree::DragMoveItem(HTREEITEM hDrophItem, HTREEITEM hDraghItem)
{	
	HTREEITEM		hOldParentItem;
	i3ElementBase	* pDragElement; 	
	i3Node			* pOldParentNode;
	

	if(hDraghItem == hDrophItem)
	{
		g_pMainWnd->Log(I3GRAP_LOG_NORMAL, "같은 곳으로 이동합니다."); 	
		return; 
	}

	hOldParentItem	= m_TreeControl.GetParentItem( hDraghItem ); 
	if(hOldParentItem == NULL)
	{
		//SetStateBar("루트를 어데로 이동하라고.바부탱아."); 
		g_pMainWnd->Log(I3GRAP_LOG_NORMAL, "Scene Root 이므로 이동할 수 없습니다.");
		return; 
	}

	pOldParentNode	= (i3Node *)m_TreeControl.GetItemData( hOldParentItem );	
	pDragElement	= (i3ElementBase *)m_TreeControl.GetItemData( hDraghItem );		

	//여기에서 이동을 한다. 
	if( pDragElement->IsTypeOf( i3Node::GetClassMeta() ) )
	{
		// Drag가 Node 일때		
		i3Node	* pDropNode = (i3Node *)m_TreeControl.GetItemData( hDrophItem );
		i3Node	* pDragNode = (i3Node *)pDragElement; 

		pDropNode->AddChild( pDragNode ); 		
		pOldParentNode->RemoveChild( pDragNode );  
		pDragNode->SetParent(pDropNode); 	

		//Delete Node 이동된 노드 지우기.		
		m_TreeControl.DeleteItem( hDraghItem );		
		
		// 다시 만들기 		
		m_hParent = m_TreeControl.GetParentItem( hDrophItem );
		i3SceneGraphTree( pDropNode ); 
		//그전거를 지운다. 
		m_TreeControl.DeleteItem( hDrophItem );
	}else
	{
		// Drag가 AttrSet 일때	
		if(pDragElement->IsTypeOf( i3GeometryAttr::GetClassMeta() ) )
		{
			//Geometry AttrSet 
			i3Geometry		* pDropNode = (i3Geometry *)m_TreeControl.GetItemData( hDrophItem );
			i3Geometry		* pOldeNode = (i3Geometry *)pOldParentNode; 
			i3RenderAttr	* pDragAttr = (i3RenderAttr *)pDragElement;

			pDropNode->AppendGeometryAttr(pDragAttr); 			
			pOldeNode->RemoveGeometryAttr(pDragAttr); 

			m_TreeControl.DeleteItem(hDraghItem);
			//다시 만들기 
			m_hParent = m_TreeControl.GetParentItem( hDrophItem );
			i3SceneGraphTree( pDropNode ); 
			//그전거를 지운다. 
			m_TreeControl.DeleteItem( hDrophItem );			
		}else
		{
			//Normal AttrSet 
			i3AttrSet		* pDropNode = (i3AttrSet *)m_TreeControl.GetItemData( hDrophItem );
			i3AttrSet		* pOldeNode = (i3AttrSet *)pOldParentNode; 
			i3RenderAttr	* pDragAttr = (i3RenderAttr *)pDragElement;

			pDropNode->AddAttr(pDragAttr); 
			pOldeNode->RemoveAttr(pDragAttr); 

			m_TreeControl.DeleteItem(hDraghItem);
			//다시 만들기 
			m_hParent = m_TreeControl.GetParentItem( hDrophItem );
			i3SceneGraphTree( pDropNode ); 
			//그전거를 지운다. 
			m_TreeControl.DeleteItem( hDrophItem );
		}
	}

	return; 
}

void Ci3Tree::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_Dragging)
	{
		if((m_hDropItem != NULL) && (m_hDragItem != NULL) && (m_DropEnable == TRUE))
		{
			DragMoveItem( m_hDropItem, m_hDragItem);
		}

		m_Dragging = FALSE; 
		ReleaseCapture();
		::SetClassLong( m_TreeControl.m_hWnd, GCL_HCURSOR, m_OldClassCursor);
	}

	CDialog::OnLButtonUp(nFlags, point);
}

//void Ci3Tree::OnNodeGeometrytool()
//{
//	
//}

void Ci3Tree::BuildRenderableColliderMesh(void)
{
	i3List List;
	INT32 i;
	i3ColliderSet * pCollSet;
	i3AttrSet * pAttrSet;
	i3Node * pNode;

	List.SetOnceMode( TRUE);
	i3Scene::FindNodeByType( m_pScene, i3ColliderSet::GetClassMeta(), &List);

	for( i = 0; i < List.GetCount(); i++)
	{
		pCollSet = (i3ColliderSet *) List.Items[i];

		pAttrSet = i3AttrSet::NewObjectRef();

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();
			pAttr->Set( TRUE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::NewObjectRef();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::NewObjectRef();

			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::NewObjectRef();
			COLOR col;

			i3Color::Set( &col, 255, 255, 128, 128);

			pAttr->Set( &col);
			pAttrSet->AddAttr( pAttr);
		}

		pNode = i3SceneUtil::BuildRendableCollider( pCollSet, TRUE, TRUE);

		pAttrSet->AddChild( pNode);

		pCollSet->AddChild( pAttrSet);
	}

	AssignSceneGraphToTree();
}

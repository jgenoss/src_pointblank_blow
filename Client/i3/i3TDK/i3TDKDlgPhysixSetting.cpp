// i3TDKDlgPhysixSetting.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgPhysixSetting.h"
#include "i3TDKDlgBuildShape.h"
#include "i3TDKUtilCommon.h"
#include ".\i3tdkdlgphysixsetting.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"
#include "i3Base/string/ext/atoi.h"

// i3TDKDlgPhysixSetting dialog

IMPLEMENT_DYNAMIC(i3TDKDlgPhysixSetting, CDialog)

i3TDKDlgPhysixSetting::~i3TDKDlgPhysixSetting()
{
	I3_SAFE_RELEASE( m_pScene);
}

void i3TDKDlgPhysixSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_PHYSIXTREE, m_TreeCtrl);
	DDX_Control(pDX, IDC_TDK_CB_USAGE, m_UsageCtrl);
	DDX_Control(pDX, IDC_TDK_CB_GROUP, m_CollGroupCtrl);
	DDX_Control(pDX, IDC_TDK_ED_RESTITUTION, m_RestitutionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_STATIC_FRICTION, m_StaticFrictionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_DYNAMIC_FRICTION, m_DynamicFrictionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_DYNAMIC_FRICTION2, m_MassCtrl);
	DDX_Control(pDX, IDC_TDK_SHAPELIST, m_ShapeListCtrl);
	DDX_Control(pDX, IDC_TDK_ED_LINEAR_DAMPING, m_ED_LinearDamping);
	DDX_Control(pDX, IDC_TDK_ED_ANGULAR_DAMPING, m_ED_AngularDamping);
	DDX_Control(pDX, IDC_TDK_ED_SKIN_WIDTH, m_SkinWidthCtrl);
	DDX_Control(pDX, IDC_CB_TERRAIN_TYPE, m_CB_TerrainType);
	DDX_Control(pDX, IDC_EDIT_THICKNESS, m_ED_Thickness);
}

#define		ICO_OTHER			0
#define		ICO_TRANS			1
#define		ICO_SHAPESET		2

void i3TDKDlgPhysixSetting::_Rec_BuildTree( i3Node * pNode, HTREEITEM hParent)
{
	HTREEITEM hItem;
	TCHAR szName[MAX_PATH];
	INT32 idxImage = ICO_OTHER;
	bool bAdd = false;

	if( pNode->IsFlag( I3_NODEFLAG_VOLATILE))
		return;

	if( i3::kind_of<i3Transform*>(pNode))
	{
		idxImage = ICO_TRANS;

		if( i3::same_of<i3Transform2* >(pNode))
		{
			i3Transform2 * pTrans2 = (i3Transform2 *) pNode;

			if( pTrans2->getShapeSet() != nullptr)
			{
				idxImage = ICO_SHAPESET;
			}
		}

		bAdd = true;
	}
	else if( i3::kind_of<i3ShapeSetContainer* >(pNode))
	{
		idxImage = ICO_SHAPESET;
		bAdd = true;
	}
	else if( i3::same_of<i3Body* >(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();
		
		if( pLOD->getSkeleton()->isShapeSet( 0))
			idxImage = ICO_SHAPESET;

		bAdd = true;
	}
	else if( i3::kind_of<i3Geometry* >(pNode))
	{
		bAdd = true;
	}

	if( bAdd)
	{
		if( pNode->hasName())
		{
			i3::snprintf( szName, _countof(szName), _T("%s [%s]"), pNode->meta()->class_name(), pNode->GetName());
		}
		else
		{
			i3::snprintf( szName, _countof(szName), _T("%s"), pNode->meta()->class_name());
		}

		{
			TVINSERTSTRUCT info;

			info.hInsertAfter			= TVI_LAST;
			info.hParent				= hParent;
			info.item.mask				= TVIF_IMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_SELECTEDIMAGE;
			info.item.hItem				= 0;
			info.item.state				= 0;
			info.item.stateMask			= 0;
			info.item.pszText			= szName;
			info.item.cchTextMax		= 0;
			info.item.iImage			= idxImage;
			info.item.iSelectedImage	= idxImage;
			info.item.cChildren			= 0;
			info.item.lParam			= (LPARAM) pNode;

			hItem = m_TreeCtrl.InsertItem( &info);
		}

		// Parent Node에 대한 cChildren 조정
		{
			TVITEM item;

			item.mask					= TVIF_CHILDREN | TVIF_HANDLE;
			item.hItem					= hParent;
			item.cChildren				= 1;

			m_TreeCtrl.SetItem( &item);
		}
	}
	else
	{
		hItem = hParent;
	}
	
	{
		INT32 i;
		i3Node * pChild;

		for( i = 0; i < pNode->GetChildCount(); i++)
		{
			pChild = pNode->GetChild( i);

			_Rec_BuildTree( pChild, hItem);
		}
	}
}

bool i3TDKDlgPhysixSetting::Execute( i3Node * pNode)
{
	INT32 id;

	I3_REF_CHANGE( m_pScene, pNode);

	i3TDK::SetResInstance();

	id = DoModal();

	i3TDK::RestoreResInstance();

	if( id == IDCANCEL)
	{
		return false;
	}

	return true;
}

inline void _AddIcon( CImageList * pImageList, INT32 id)
{
	HICON hIcon;

	hIcon = ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));
	pImageList->Add( hIcon);
	::DestroyIcon( hIcon);
}

void	i3TDKDlgPhysixSetting::_SelectItem( HTREEITEM hItem)
{
	i3Node * pNode = (i3Node *) m_TreeCtrl.GetItemData( hItem);
	INT32	Usage = -1;
	INT32	CollGroup = -1, idx;
	TCHAR szShapeType[64];
	TCHAR conv[128];
	INT32 FaceCount = -1;
	INT32 i;
	REAL32 mass = 0.0f;
	i3PhysixShapeSet * pShapeSet = nullptr;

	MATRIX mat;
	
	m_hSelItem = hItem;

	m_ShapeListCtrl.DeleteAllItems();

	if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans2 = (i3Transform2 *) pNode;

		pShapeSet = pTrans2->getShapeSet();
	}
	else if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;

		pShapeSet = pContainer->getShapeSet();

		Usage = (INT32) pContainer->getUsage();
	}

	m_pShapeSet = pShapeSet;

	if( pShapeSet == nullptr)
		return;

	if( pShapeSet->getNxActor() == nullptr )
	{
		i3MatrixStack stack;
		I3_ONINSTANCING_INFO info;
		info._pBase = m_pShapeSet->getEventReceiver();
		info._pMatrixStack = &stack; 
		pShapeSet->OnInstancing( &info);
	}

	mass = m_pShapeSet->getMass();

	for( i = 0; i < pShapeSet->getNxShapeCount(); i++)
	{
		NxShape * pShape = pShapeSet->getNxShape( i);

		// Type
		switch( pShape->getType())
		{
		case NX_SHAPE_SPHERE		:		i3::generic_string_copy( szShapeType, _T("Sphere") );	break;
			case NX_SHAPE_CAPSULE		:		i3::generic_string_copy( szShapeType, _T("Capsule") ); break;
			case NX_SHAPE_BOX			:		i3::generic_string_copy( szShapeType, _T("Box") );	break;

			case NX_SHAPE_CONVEX :	
				i3::generic_string_copy( szShapeType, _T("Convex") ); 

				{
					NxConvexShape * pConvex = (NxConvexShape *) pShape;

					FaceCount = pConvex->getConvexMesh().getCount( 0, NX_ARRAY_HULL_POLYGONS);
				}
				break;

			case NX_SHAPE_MESH :		
				i3::generic_string_copy( szShapeType, _T("Triangle Mesh") );	

				{
					NxTriangleMeshShape * pMesh = (NxTriangleMeshShape *) pShape;

					FaceCount = pMesh->getTriangleMesh().getCount( 0, NX_ARRAY_TRIANGLES);
				}
				break;

			default :							i3::generic_string_copy( szShapeType, _T("(Unknown)"));	break;
		}

		idx = m_ShapeListCtrl.InsertItem( i, szShapeType, 0);

		i3::snprintf( conv, _countof(conv), _T("%d"), FaceCount);
		m_ShapeListCtrl.SetItemText( idx, 1, conv);
	}

	// Usage
	m_UsageCtrl.SetCurSel( Usage);

	// Coll Group
	CollGroup = pShapeSet->getShapeGroup();

	// Collision Group
	m_CollGroupCtrl.SetCurSel( CollGroup);

	m_UsageCtrl.EnableWindow( (Usage != -1) && (pShapeSet != nullptr));
	m_CollGroupCtrl.EnableWindow( pShapeSet != nullptr);

	// Mass
	i3::ftoa( mass, conv);
	m_MassCtrl.SetWindowText( conv);
	m_MassCtrl.EnableWindow( (pShapeSet != nullptr) && (pShapeSet->isDynamicActor()));
	m_MassCtrl.SetModify( FALSE);

	// Linear Damping
	i3::ftoa( pShapeSet->getNxActor()->getLinearDamping(), conv);
	m_ED_LinearDamping.SetWindowText( conv);
	m_ED_LinearDamping.SetModify( FALSE);

	// Angular Damping
	i3::ftoa( pShapeSet->getNxActor()->getAngularDamping(), conv);
	m_ED_AngularDamping.SetWindowText( conv);
	m_ED_AngularDamping.SetModify( FALSE);

	// Terrain Type
	if( pShapeSet->getTerrainArray() != nullptr )
	{	
		m_CB_TerrainType.SetCurSel( pShapeSet->getTerrain(0)->m_Terrain );
		i3::ftoa( (REAL32) pShapeSet->getTerrain(0)->m_Thickness, conv);
		m_ED_Thickness.SetWindowText( conv );
		m_ED_Thickness.SetModify( FALSE);
	}
}

void	i3TDKDlgPhysixSetting::_SelectShape( INT32 idx)
{
	TCHAR conv[256];
	i3PhysixShapeSet * pShapeSet = m_pShapeSet;
	REAL32 restitution = 0.0f;
	REAL32 sFriction = 0.0f;
	REAL32 dFriction = 0.0f;

	if( pShapeSet != nullptr)
	{
		restitution = pShapeSet->getRestitution();
		sFriction	= pShapeSet->getStaticFriction();
		dFriction	= pShapeSet->getDynamicFriction();
		
	}

	// Restitution
	i3::ftoa( restitution, conv);
	m_RestitutionCtrl.SetWindowText( conv);

	// Static Friction
	i3::ftoa( sFriction, conv);
	m_StaticFrictionCtrl.SetWindowText( conv);

	// Dynamic Friction
	i3::ftoa( dFriction, conv);
	m_DynamicFrictionCtrl.SetWindowText( conv);


	m_RestitutionCtrl.EnableWindow( pShapeSet != nullptr);
	m_StaticFrictionCtrl.EnableWindow( pShapeSet != nullptr);
	m_DynamicFrictionCtrl.EnableWindow( pShapeSet != nullptr);

	m_RestitutionCtrl.SetModify( FALSE);
	m_StaticFrictionCtrl.SetModify( FALSE);
	m_DynamicFrictionCtrl.SetModify( FALSE);

	NxShape * pShape = m_pShapeSet->getNxShape( idx);

	i3::ftoa( pShape->getSkinWidth(), conv);
	m_SkinWidthCtrl.SetWindowText( conv);

	if( (pShapeSet != nullptr) && (pShapeSet->getTerrainArray() != nullptr ))
	{	
		INT32 idxTerrain = min( idx, pShapeSet->getTerrainCount() - 1);

		if( pShapeSet->getTerrain(idxTerrain) != nullptr )
			m_CB_TerrainType.SetCurSel( pShapeSet->getTerrain(idxTerrain)->m_Terrain );
	}
}

i3PhysixShapeSet *	i3TDKDlgPhysixSetting::_getSelectedShapeSet(void)
{
	if( m_hSelItem == nullptr)
		return nullptr;

	i3Node * pNode = (i3Node *) m_TreeCtrl.GetItemData( m_hSelItem);

	if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;

		return pTrans->getShapeSet();
	}
	else if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;

		return pContainer->getShapeSet();
	}

	return nullptr;
}

BEGIN_MESSAGE_MAP(i3TDKDlgPhysixSetting, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TDK_PHYSIXTREE, OnTvnSelchangedTdkPhysixtree)
	ON_CBN_SELCHANGE(IDC_TDK_CB_USAGE, OnCbnSelchangeTdkCbUsage)
	ON_CBN_SELCHANGE(IDC_TDK_CB_GROUP, OnCbnSelchangeTdkCbGroup)
	ON_EN_KILLFOCUS(IDC_TDK_ED_RESTITUTION, OnEnKillfocusTdkEdRestitution)
	ON_EN_KILLFOCUS(IDC_TDK_ED_STATIC_FRICTION, OnEnKillfocusTdkEdStaticFriction)
	ON_EN_KILLFOCUS(IDC_TDK_ED_DYNAMIC_FRICTION, OnEnKillfocusTdkEdDynamicFriction)
	ON_EN_KILLFOCUS(IDC_TDK_ED_DYNAMIC_FRICTION2, OnEnKillfocusTdkEdDynamicFriction2)
	ON_NOTIFY(NM_RCLICK, IDC_TDK_PHYSIXTREE, OnNMRclickTdkPhysixtree)
	ON_NOTIFY(NM_CLICK, IDC_TDK_SHAPELIST, OnNMClickTdkShapelist)
	ON_EN_KILLFOCUS(IDC_TDK_ED_LINEAR_DAMPING, OnEnKillfocusTdkEdLinearDamping)
	ON_EN_KILLFOCUS(IDC_TDK_ED_ANGULAR_DAMPING, OnEnKillfocusTdkEdAngularDamping)
	ON_CBN_SELCHANGE(IDC_CB_TERRAIN_TYPE, OnCbnSelchangeCbTerrainType)
	ON_EN_KILLFOCUS(IDC_EDIT_THICKNESS, OnEnKillfocusEditThickness)
END_MESSAGE_MAP()

BOOL i3TDKDlgPhysixSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_ImageList.Create( 16, 16, ILC_COLOR8 | ILC_MASK, 2, 0);

		_AddIcon( &m_ImageList, IDI_TDK_PHS_OTHER);
		_AddIcon( &m_ImageList, IDI_TDK_PHS_TRANS);
		_AddIcon( &m_ImageList, IDI_TDK_PHS_SHAPESET);

		m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL);
	}

	{
		m_ShapeListCtrl.InsertColumn( 0, _T("Type"), LVCFMT_LEFT, 80);
		m_ShapeListCtrl.InsertColumn( 1, _T("# Face"), LVCFMT_LEFT, 80);
	}

	{
		INT32 i;
		
		for( i = 0; i < i3TDK::getTerrainTypeCount(); i++)
		{
			INT32 idx;
			
			idx = m_CB_TerrainType.AddString( i3TDK::getTerrainTypeName( (I3_TERRAIN_TYPE) i));

			m_CB_TerrainType.SetItemData( idx, (DWORD_PTR) i);
		}
	}

	if( m_pScene != nullptr)
	{
		_Rec_BuildTree( m_pScene, TVI_ROOT);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlgPhysixSetting::OnOK()
{
	if( m_RestitutionCtrl.GetModify())
	{
		OnEnKillfocusTdkEdRestitution();
	}

	if( m_StaticFrictionCtrl.GetModify())
	{
		OnEnKillfocusTdkEdStaticFriction();
	}

	if( m_DynamicFrictionCtrl.GetModify())
	{
		OnEnKillfocusTdkEdDynamicFriction();
	}

	CDialog::OnOK();
}

void i3TDKDlgPhysixSetting::OnTvnSelchangedTdkPhysixtree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	_SelectItem( pNMTreeView->itemNew.hItem);

	*pResult = 0;
}

void i3TDKDlgPhysixSetting::OnCbnSelchangeTdkCbUsage()
{
	i3Node * pNode;
	i3ShapeSetContainer * pContainer;

	if( m_hSelItem == nullptr)
		return;

	pNode = (i3Node *) m_TreeCtrl.GetItemData( m_hSelItem);

	if( !i3::kind_of< i3ShapeSetContainer* >(pNode))
		return;

	pContainer = (i3ShapeSetContainer *) pNode;

	pContainer->setUsage( (i3ShapeSetContainer::USAGE) m_UsageCtrl.GetCurSel());
}

void i3TDKDlgPhysixSetting::OnCbnSelchangeTdkCbGroup()
{
	i3PhysixShapeSet * pShapeSet = _getSelectedShapeSet();

	if( pShapeSet != nullptr)
	{
		pShapeSet->setShapeGroup( (UINT16) m_CollGroupCtrl.GetCurSel());
	}
}


void i3TDKDlgPhysixSetting::OnEnKillfocusTdkEdRestitution()
{
	i3PhysixShapeSet * pShapeSet = _getSelectedShapeSet();

	if( pShapeSet != nullptr)
	{
		TCHAR conv[256];

		m_RestitutionCtrl.GetWindowText( conv, _countof(conv) - 1);
		m_RestitutionCtrl.SetModify( FALSE);

		pShapeSet->setRestitution( (REAL32) i3::atof( conv));
	}
}

void i3TDKDlgPhysixSetting::OnEnKillfocusTdkEdStaticFriction()
{
	i3PhysixShapeSet * pShapeSet = _getSelectedShapeSet();

	if( pShapeSet != nullptr)
	{
		TCHAR conv[256];

		m_StaticFrictionCtrl.GetWindowText( conv, _countof(conv) - 1);
		m_StaticFrictionCtrl.SetModify( FALSE);

		pShapeSet->setFriction( (REAL32) i3::atof( conv), pShapeSet->getDynamicFriction());
	}
}

void i3TDKDlgPhysixSetting::OnEnKillfocusTdkEdDynamicFriction()
{
	i3PhysixShapeSet * pShapeSet = _getSelectedShapeSet();

	if( pShapeSet != nullptr)
	{
		TCHAR conv[256];

		m_DynamicFrictionCtrl.GetWindowText( conv, _countof(conv) - 1);
		m_DynamicFrictionCtrl.SetModify( FALSE);

		pShapeSet->setFriction( pShapeSet->getStaticFriction(), (REAL32) i3::atof( conv));
	}
}

void i3TDKDlgPhysixSetting::OnEnKillfocusTdkEdDynamicFriction2()
{
	i3PhysixShapeSet * pShapeSet = _getSelectedShapeSet();

	if((pShapeSet != nullptr) && (pShapeSet->isDynamicActor()))
	{
		TCHAR conv[256];

		m_MassCtrl.GetWindowText( conv, _countof(conv) - 1);
		m_MassCtrl.SetModify( FALSE);

		pShapeSet->getNxActor()->setMass( (REAL32) i3::atof( conv));
	}
}

void i3TDKDlgPhysixSetting::_OnLoadNewShapes(void)
{
	CFileDialog	Dlg( TRUE, _T("i3s"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("I3Engine File(*.i3s)|*.i3s||"), this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3SceneFile file;
	i3::stack_string strDlgPathName;
#ifdef _UNICODE
	i3::utf16_to_mb(Dlg.GetPathName(), strDlgPathName);

#else
	strDlgPathName = (const char*)Dlg.GetPathName();
#endif

	if( file.Load(strDlgPathName.c_str()) == STREAM_ERR)
	{
		NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", LPCTSTR( Dlg.GetPathName()));
		return;
	}

	i3TDKDlgBuildShape dlg;

	if( dlg.DoModal() == IDCANCEL)
		return;

	{
		i3OptBuildTerrainMesh opt;
		i3Node * pAdd = file.getSceneGraphInfo()->getInstanceSg();

		I3_MUST_ADDREF(pAdd);

		if( dlg.m_ShapeType == I3_PHYSIX_SHAPE_TRIMESH )
		{
			opt.setSg( file.getSceneGraphInfo() );
			opt.setDynamic( true);
			opt.setMass( dlg.m_Mass);

			if( dlg.m_Mass != 0.0f)
				opt.setDensity( 0.0f);
			else
				opt.setDensity( 1.0f);

			opt.Trace( pAdd);

			if( opt.getTraceRoot() != nullptr )
			{
				I3_REF_CHANGE( pAdd, opt.getTraceRoot());
			}

			if( dlg.m_Usage != i3ShapeSetContainer::USAGE_SIMULATION)
			{
				i3::vector<i3Node*>	list;
				i3Transform2 * pTrans2;
				i3Transform * pNewTrans;
				i3ShapeSetContainer * pContainer;

				// i3Transform2가 있는지를 확인하고, 있다면 i3ShapeSetContainer로 대체한다.
				i3Scene::FindNodeByType( pAdd, i3Transform2::static_meta(), list);

				for(size_t i = 0; i < list.size(); i++)
				{
					pTrans2 = (i3Transform2 *) list[i];

					pContainer = i3ShapeSetContainer::new_object_ref();

					if( pTrans2->hasName())
						pContainer->SetName( pTrans2->GetName());

					pContainer->setShapeSet( pTrans2->getShapeSet());
					pContainer->setUsage( dlg.m_Usage);

					i3Scene::MoveChildren( pContainer, pTrans2);

					i3Node * pAttachNode ;

					if( i3Matrix::IsIdentity( pTrans2->GetMatrix()) == FALSE)
					{
						pNewTrans = i3Transform::new_object_ref();

						pNewTrans->SetMatrix( pTrans2->GetMatrix());
						pNewTrans->AddChild( pContainer);

						pAttachNode = pNewTrans;
					}
					else
					{
						pAttachNode = pContainer;
					}

					if( pTrans2->GetParent() != nullptr)
					{
						i3Node * pParent = pTrans2->GetParent();

						pParent->RemoveChild( pTrans2);
						pParent->AddChild( pAttachNode);
					}
					else
					{
						I3_REF_CHANGE( pAdd, pAttachNode);
					}
				}
			}
		}
	
		m_pScene->AddChild( pAdd);

		I3_MUST_RELEASE(pAdd);
	}

	{
		m_TreeCtrl.DeleteAllItems();
		_Rec_BuildTree( m_pScene, TVI_ROOT);
	}
}

void i3TDKDlgPhysixSetting::_OnInsertNewShapes(void)
{
	i3Node * pNode;
	i3PhysixShapeSet * pShapeSet;
	i3TDKDlgBuildShape dlg;

	if( m_hSelItem != nullptr)
	{
		pNode = (i3Node *) m_TreeCtrl.GetItemData( m_hSelItem);
	}
	else
	{
		pNode = m_pScene;
	}

	// PhysixShapeSet을 생성
	{
		if( dlg.DoModal() == IDCANCEL)
			return;

		i3OptBuildHull opt;

		opt.setShapeGroup( dlg.m_Group);
		opt.setDynamicMode( dlg.m_bDynamic);
		opt.setElasticity( dlg.m_Restitution);
		opt.setStaticFriction( dlg.m_StaticFriction);
		opt.setDynamicFriction( dlg.m_DynamicFriction);

		pShapeSet = opt.CreateConvexHull( pNode);
	}

	if( i3::kind_of< i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans2 = (i3Transform2 *) pNode;

		switch( dlg.m_Usage)
		{
			case i3ShapeSetContainer::USAGE_SIMULATION :
				// 기존 ShapeSet을 새로운 ShapeSet을 대체
				pTrans2->setShapeSet( pShapeSet);
				break;

			default :
				// 다른 모든 경우에 대해선 i3ShapeSetContainer node를 Child로 만들어 붙인다.
				{
					i3ShapeSetContainer * pContainer = i3ShapeSetContainer::new_object_ref();

					pContainer->setShapeSet( pShapeSet);
					pContainer->setUsage( dlg.m_Usage);

					switch( dlg.m_Usage)
					{
						case i3ShapeSetContainer::USAGE_RAYCAST :	pContainer->SetName( "Raycast");	break;
						case i3ShapeSetContainer::USAGE_TRIGGER :	pContainer->SetName( "Trigger");	break;
						case i3ShapeSetContainer::USAGE_NA :		pContainer->SetName( "NA");	break;
					}

					pTrans2->AddChild( pContainer);
				}
				break;
		}
	}
	else if( i3::kind_of<i3ShapeSetContainer*>(pNode))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;

		switch( dlg.m_Usage)
		{
			case i3ShapeSetContainer::USAGE_SIMULATION :
				// i3ShapeSetContainer Node에 Simulation을 추가한 경우...
				// ShapeSet Container Node는 Simulation Update 기능이 없기 때문에
				// 다른 모든 Node와 동일하게 간주.
				_InsertShapeSet( pNode, pShapeSet, dlg.m_Usage);
				break;

			default :
				pContainer->setUsage( dlg.m_Usage);
				pContainer->setShapeSet( pShapeSet);
				break;
		}
	}
	else
	{
		_InsertShapeSet( pNode, pShapeSet, dlg.m_Usage);
	}

	{
		m_TreeCtrl.DeleteAllItems();

		_Rec_BuildTree( m_pScene, TVI_ROOT);
	}
}

void i3TDKDlgPhysixSetting::_InsertShapeSet( i3Node * pNode, i3PhysixShapeSet * pShapeSet, i3ShapeSetContainer::USAGE usage)
{
	i3Transform2 * pTrans2 = nullptr;
	i3Node * pTemp = nullptr;

	// 상위로 올라가며 i3Transform Node를 찾는다.
	{
		pTemp = pNode;

		do
		{
			if( i3::kind_of< i3Transform2* >(pTemp))
			{
				pTrans2 = (i3Transform2 *) pTemp;
				break;
			}

			pTemp = pTemp->GetParent();

		} while( pTemp != nullptr);
	}

	if( pTemp == nullptr)
	{
		// 완전히 새롭게 Usage에 맞게 Node를 붙인다.
		pTrans2 = i3Transform2::new_object_ref();

		pTrans2->AddChild( m_pScene);

		// 새로운 Root로 설정.
		I3_REF_CHANGE( m_pScene, pTrans2);
	}

	if( usage == i3ShapeSetContainer::USAGE_SIMULATION)
	{
		// Simulation용은 반드시 i3Transform2 Node에 설정되어야 하기 때문에...
		// 기존의 ShapeSet을 대체한다.
		pTrans2->setShapeSet( pShapeSet);
	}
	else
	{
		// i3ShapeSetContainer Node를 새로 생성해, pTrans2의 Child로 붙인다.
		i3ShapeSetContainer * pContainer = i3ShapeSetContainer::new_object_ref();

		pContainer->setShapeSet( pShapeSet);
		pContainer->setUsage( usage);
		
		pTrans2->AddChild( pContainer);
	}
}

void i3TDKDlgPhysixSetting::_OnRemoveShapeSetOnly(void)
{
}

void i3TDKDlgPhysixSetting::_OnRemoveNode(void)
{
	i3Node * pParent, * pNode, * pChild;
	INT32 i;
	HTREEITEM hParent;

	pNode = (i3Node *) m_TreeCtrl.GetItemData( m_hSelItem);
	I3ASSERT( pNode != nullptr);

	pParent = pNode->GetParent();
	if( pParent == nullptr)
	{
		NotifyBox( m_hWnd, "Root Node이기 때문에 삭제할 수 없습니다.");
		return;
	}

	hParent = m_TreeCtrl.GetParentItem( m_hSelItem);

	m_TreeCtrl.DeleteItem( m_hSelItem);

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		I3_MUST_ADDREF(pChild);

		pNode->RemoveChild( pChild);
		pParent->AddChild( pChild);

		_Rec_BuildTree( pChild, hParent);

		I3_MUST_RELEASE(pChild);
	}

	pParent->RemoveChild( pNode);
}

void i3TDKDlgPhysixSetting::OnNMRclickTdkPhysixtree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSel;
	CPoint spt, cpt;
	UINT32 flag;
	i3PhysixShapeSet * pShapeSet;
	CMenu menu, * pPopup;
	UINT32 cmd;

	GetCursorPos( &spt);
	cpt = spt;
	ScreenToClient( &cpt);

	hSel = m_TreeCtrl.HitTest( cpt, &flag);

	if( !(flag & TVHT_ONITEM))
	{
		hSel = nullptr;
	}

	if( hSel != nullptr)
	{
		m_TreeCtrl.Select( hSel, TVGN_CARET);
		_SelectItem( hSel);
		pShapeSet = _getSelectedShapeSet();
	}
	else
	{
		pShapeSet = nullptr;
	}

	menu.LoadMenu( MAKEINTRESOURCE( IDR_TDK_POPUP_SHAPESET));
	pPopup = menu.GetSubMenu( 0);

	{
		if( pShapeSet == nullptr)
		{
			pPopup->EnableMenuItem( ID_SHAPESET_REMOVESHAPESONLY, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}

		if( hSel == nullptr)
		{
			pPopup->EnableMenuItem( ID_SHAPESET_REMOVENODE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				spt.x, spt.y, (CWnd *) this, nullptr);

	switch( cmd)
	{
		case ID_SHAPESET_REMOVESHAPESONLY :			_OnRemoveShapeSetOnly();	break;
		case ID_SHAPESET_REMOVENODE :				_OnRemoveNode(); break;
		case ID_SHAPESET_INSERTNEWCONVEX :			_OnInsertNewShapes();	break;
		case ID_SHAPESET_LOADNEWCOLLISIONSHAPE :	_OnLoadNewShapes();	break;
	}

	*pResult = 1;
}

void i3TDKDlgPhysixSetting::OnNMClickTdkShapelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_ShapeListCtrl.GetFirstSelectedItemPosition();

	if( pos != nullptr)
	{
		INT32 idx = m_ShapeListCtrl.GetNextSelectedItem( pos);

		_SelectShape( idx);
	}

	*pResult = 0;
}

void i3TDKDlgPhysixSetting::OnEnKillfocusTdkEdLinearDamping()
{
	TCHAR conv[256];

	m_ED_LinearDamping.GetWindowText( conv, _countof(conv) - 1);
	m_ED_LinearDamping.SetModify( FALSE);
	m_pShapeSet->getNxActor()->setLinearDamping( (REAL32) i3::atof( conv));
}

void i3TDKDlgPhysixSetting::OnEnKillfocusTdkEdAngularDamping()
{
	TCHAR conv[256];

	m_ED_AngularDamping.GetWindowText( conv, _countof(conv) - 1);
	m_ED_AngularDamping.SetModify( FALSE);
	m_pShapeSet->getNxActor()->setAngularDamping( (REAL32) i3::atof( conv));
}

void i3TDKDlgPhysixSetting::OnCbnSelchangeCbTerrainType()
{	
	i3Node * pNode;
	i3MemoryBuffer * pBuff;
	I3_PHYSIX_TERRAIN * pDest;

	i3PhysixShapeSet * pShapeSet = nullptr;
	i3Transform2 * pTransform2 = nullptr;
	i3ShapeSetContainer * pContainer = nullptr;

	if( m_hSelItem == nullptr)
		return;

	pNode = (i3Node *) m_TreeCtrl.GetItemData( m_hSelItem);

	if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		pContainer = (i3ShapeSetContainer *) pNode;
		pShapeSet = pContainer->getShapeSet();
	}
	else if( i3::same_of<i3Transform2* >(pNode))
	{
		pTransform2 = (i3Transform2*) pNode;
		pShapeSet = pTransform2->getShapeSet();
	}
	else
		return;


	if((pShapeSet->getTerrainArray() == nullptr) || ((UINT32)pShapeSet->getTerrainCount() != pShapeSet->getNxActor()->getNbShapes()))
	{
		I3ASSERT( pShapeSet->getNxActor() != nullptr );
		pShapeSet->setTerrainCount( pShapeSet->getNxActor()->getNbShapes() );

		pBuff = i3MemoryBuffer::new_object();
		pBuff->Create( sizeof(I3_PHYSIX_TERRAIN) * pShapeSet->getTerrainCount() );
		pShapeSet->setTerrainArray( pBuff );

		pDest = (I3_PHYSIX_TERRAIN *) pBuff->getBuffer();

		i3mem::FillZero( pDest, sizeof(I3_PHYSIX_TERRAIN) * pShapeSet->getTerrainCount() );
	}

	{
		POSITION pos = m_ShapeListCtrl.GetFirstSelectedItemPosition();
		
		if( pos != nullptr )
		{
			INT32 idx = m_ShapeListCtrl.GetNextSelectedItem( pos);

			if( idx != -1 )
			{
				pDest = pShapeSet->getTerrain( idx );
				pDest->m_Terrain = (UINT16) m_CB_TerrainType.GetCurSel();
			}
		}
		else
		{
			// shape의 이름을 보고 Terrain을 판단한다.
			for( INT32 i = 0; i < pShapeSet->getTerrainCount(); i++ )
			{
				pDest = pShapeSet->getTerrain( i );
				pDest->m_Terrain = (UINT16) m_CB_TerrainType.GetCurSel();
			}
		}
	}
}

void i3TDKDlgPhysixSetting::OnEnKillfocusEditThickness()
{
	i3Node * pNode;
	i3MemoryBuffer * pBuff;
	I3_PHYSIX_TERRAIN * pDest;

	TCHAR conv[256] = {0,};

	i3PhysixShapeSet * pShapeSet = nullptr;
	i3Transform2 * pTransform2 = nullptr;
	i3ShapeSetContainer * pContainer = nullptr;

	if( m_hSelItem == nullptr)
		return;

	pNode = (i3Node *) m_TreeCtrl.GetItemData( m_hSelItem);

	if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		pContainer = (i3ShapeSetContainer *) pNode;
		pShapeSet = pContainer->getShapeSet();
	}
	else if( i3::same_of<i3Transform2* >(pNode))
	{
		pTransform2 = (i3Transform2*) pNode;
		pShapeSet = pTransform2->getShapeSet();
	}
	else
		return;

	m_ED_Thickness.GetWindowText( conv, _countof(conv) - 1);
	m_ED_Thickness.SetModify( FALSE);
	INT32 nThickness = i3::atoi( conv);


	if( pShapeSet->getTerrainArray() == nullptr )
	{
		I3ASSERT( pShapeSet->getNxActor() != nullptr );
		pShapeSet->setTerrainCount( pShapeSet->getNxActor()->getNbShapes() );

		pBuff = i3MemoryBuffer::new_object();
		pBuff->Create( sizeof(I3_PHYSIX_TERRAIN) * pShapeSet->getTerrainCount() );
		pShapeSet->setTerrainArray( pBuff );

		pDest = (I3_PHYSIX_TERRAIN *) pBuff->getBuffer();

		i3mem::FillZero( pDest, sizeof(I3_PHYSIX_TERRAIN) * pShapeSet->getTerrainCount() );
	}

	{
		POSITION pos = m_ShapeListCtrl.GetFirstSelectedItemPosition();
		
		if( pos != nullptr )
		{
			INT32 idx = m_ShapeListCtrl.GetNextSelectedItem( pos);

			if( idx != -1 )
			{
				pDest = pShapeSet->getTerrain( idx );

				pDest->m_Thickness = nThickness;
			}
		}
		else
		{
			// shape의 이름을 보고 Terrain을 판단한다.
			for( INT32 i = 0; i < pShapeSet->getTerrainCount(); i++ )
			{
				pDest = pShapeSet->getTerrain( i );

				pDest->m_Thickness = nThickness;
			}
		}
	}
}

// i3TDKDlg_Physix_Tool.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlg_Physix_Tool.h"
#include "i3Base/string/ext/ftoa.h"

// i3TDKDlg_Physix_Tool dialog

IMPLEMENT_DYNAMIC(i3TDKDlg_Physix_Tool, CDialog)

bool i3TDKDlg_Physix_Tool::Execute( i3Node * pNode)
{
	m_pRoot = pNode;

	i3TDK::SetResInstance();
	INT32 rv = DoModal();
	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL)
		return false;

	return true;
}

void i3TDKDlg_Physix_Tool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_LIST_SHAPESET, m_List_ShapeSet);
	DDX_Control(pDX, IDC_TDK_LIST_SHAPES, m_List_Shapes);
	DDX_Control(pDX, IDC_TDK_ST_SUM, m_ST_Sum);
}


BEGIN_MESSAGE_MAP(i3TDKDlg_Physix_Tool, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_TDK_LIST_SHAPESET, &i3TDKDlg_Physix_Tool::OnNMClickTdkListShapeset)
END_MESSAGE_MAP()


// i3TDKDlg_Physix_Tool message handlers

BOOL i3TDKDlg_Physix_Tool::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_List_ShapeSet.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_List_ShapeSet.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 110);
		m_List_ShapeSet.InsertColumn( 1, _T("Dynamic"), LVCFMT_LEFT, 70);
		m_List_ShapeSet.InsertColumn( 2, _T("Group"), LVCFMT_LEFT, 60);
		m_List_ShapeSet.InsertColumn( 3, _T("Type"), LVCFMT_LEFT, 90);
		m_List_ShapeSet.InsertColumn( 4, _T("# Shapes"), LVCFMT_LEFT, 70);
		m_List_ShapeSet.InsertColumn( 5, _T("Joint"), LVCFMT_LEFT, 50);
	}

	{
		m_List_Shapes.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_List_Shapes.InsertColumn( 0, _T("Type"), LVCFMT_LEFT, 60);
		m_List_Shapes.InsertColumn( 1, _T("Group"), LVCFMT_LEFT, 60);
		m_List_Shapes.InsertColumn( 2, _T("SkinWidth"), LVCFMT_LEFT, 80);
		m_List_Shapes.InsertColumn( 3, _T("Faces"), LVCFMT_LEFT, 80);
		m_List_Shapes.InsertColumn( 4, _T("CCD"), LVCFMT_LEFT, 60);
	}

	UpdateAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlg_Physix_Tool::UpdateAll(void)
{
	i3::vector<i3Node*> list;
	
	m_List_ShapeSet.DeleteAllItems();
	m_List_Shapes.DeleteAllItems();

	m_ActorCount = m_ShapeCount = 0;

	if( m_pRoot == nullptr)
		return;

	i3Scene::FindNodeByType( m_pRoot, i3Transform2::static_meta(), list);
	i3Scene::FindNodeByType( m_pRoot, i3ShapeSetContainer::static_meta(), list);
	i3Scene::FindNodeByType( m_pRoot, i3Body::static_meta(), list);

	for(size_t i = 0; i < list.size(); i++)
	{
		i3ElementBase * pObj = (i3ElementBase *) list[i];

		if( i3::same_of<i3Transform2*>(pObj))
		{
			i3Transform2 * pNode = (i3Transform2 *) pObj;

			if( pNode->getShapeSet() != nullptr)
				AddShapeSet( pNode->getShapeSet(), ATTACH_TRANSFORM2);
		}
		else if( i3::same_of<i3ShapeSetContainer* >(pObj))
		{
			i3ShapeSetContainer * pNode = (i3ShapeSetContainer *) pObj;

			if( pNode->getShapeSet() != nullptr)
				AddShapeSet( pNode->getShapeSet(), ATTACH_SHAPESETCONTAINER);
		}
		else if( i3::same_of<i3Body* >(pObj))
		{
			INT32 j;
			i3Body * pBody = (i3Body *) pObj;

			i3LOD * pLOD = pBody->getFrameLOD();
			if( pLOD != nullptr)
			{
				i3Skeleton * pSkel = pLOD->getSkeleton();

				if( pSkel != nullptr)
				{
					for( j = 0; j < pSkel->getBoneCount(); j++)
					{
						if( pSkel->isShapeSet( j))
						{
							AddShapeSet( pSkel->getShapeSet( j), ATTACH_BODY);
						}
					}
				}
			}
		}
	}

	{
		TCHAR conv[256];

		i3::snprintf( conv, _countof( conv), _T("Total %d Actors, %d Shapes"), m_ActorCount, m_ShapeCount);

		m_ST_Sum.SetWindowText( conv);
	}
}

void i3TDKDlg_Physix_Tool::AddShapeSet( i3PhysixShapeSet * pShapeSet, ATTACH attach)
{
#ifdef _UNICODE
	i3::stack_wstring wstrShapeSetName;	i3::mb_to_utf16(pShapeSet->GetName(), wstrShapeSetName);
	const wchar_t* szShapeSetName = wstrShapeSetName.c_str();
#else
	const char* szShapeSetName = pShapeSet->GetName();
#endif

	INT32 idx = m_List_ShapeSet.InsertItem( m_List_ShapeSet.GetItemCount(), szShapeSetName );

	UpdateShapeSet( idx, pShapeSet, attach);
}

void i3TDKDlg_Physix_Tool::UpdateShapeSet( INT32 idx, i3PhysixShapeSet * pShapeSet, ATTACH attach)
{
	m_List_ShapeSet.SetItemData( idx, (DWORD_PTR) pShapeSet);
	NxActor * pActor = pShapeSet->getNxActor();
	TCHAR conv[256];

	m_ActorCount++;

	m_ShapeCount += pShapeSet->getNxShapeCount();

	// Name
	switch( attach)
	{
		case ATTACH_TRANSFORM2 :		i3::snprintf( conv, _countof( conv), _T("%s (Trans)"), pShapeSet->GetName());	break;
		case ATTACH_SHAPESETCONTAINER :	i3::snprintf( conv, _countof( conv), _T("%s (Container)"), pShapeSet->GetName());	break;
		default :						i3::snprintf( conv, _countof( conv), _T("%s (Body)"), pShapeSet->GetName());	break;
	}
	m_List_ShapeSet.SetItemText( idx, 0, conv);

	// Dynamic
	if( pActor->isDynamic())
	{
		if( pActor->readBodyFlag( NX_BF_KINEMATIC))
			m_List_ShapeSet.SetItemText( idx, 1, _T("Kinematic"));
		else
			m_List_ShapeSet.SetItemText( idx, 1, _T("Dynamic"));
	}
	else
	{
		m_List_ShapeSet.SetItemText( idx, 1, _T("Static"));
	}

	// Group
	i3::snprintf( conv, _countof( conv), _T("%d"), pShapeSet->getGroup());
	m_List_ShapeSet.SetItemText( idx, 2, conv);

	// Type
	switch( pShapeSet->getShapeType())
	{
		case I3_PHYSIX_SHAPE_SPHERE :		i3::safe_string_copy( conv, _T("Sphere"),   256);	break;
		case I3_PHYSIX_SHAPE_CAPSULE :		i3::safe_string_copy( conv, _T("Capsule"),  256);	break;
		case I3_PHYSIX_SHAPE_CONVEXMESH :	i3::safe_string_copy( conv, _T("Convex"),   256);	break;
		case I3_PHYSIX_SHAPE_TRIMESH :		i3::safe_string_copy( conv, _T("Tri-Mesh"), 256);	break;
		case I3_PHYSIX_SHAPE_BOX :			i3::safe_string_copy( conv, _T("Box"),      256);	break;
		default :							i3::safe_string_copy( conv, _T("N/A"),      256);	break;
	}
	m_List_ShapeSet.SetItemText( idx, 3, conv);

	// # Shapes
	i3::snprintf( conv, _countof( conv), _T("%d"), pActor->getNbShapes());
	m_List_ShapeSet.SetItemText( idx, 4, conv);

	// Joint
	if( pShapeSet->getNxJoint() != nullptr)
		m_List_ShapeSet.SetItemText( idx, 5, _T("O") );
	else
		m_List_ShapeSet.SetItemText( idx, 5, _T("-") );
}

void i3TDKDlg_Physix_Tool::UpdateAllShape( i3PhysixShapeSet * pShapeSet)
{
	m_List_Shapes.DeleteAllItems();
	NxShape * pShape;
	INT32 i, idx;

	for( i = 0; i < pShapeSet->getNxShapeCount(); i++)
	{
		pShape = pShapeSet->getNxShape( i);

#ifdef _UNICODE
		i3::stack_wstring wstrShapeName;	i3::mb_to_utf16(pShape->getName(), wstrShapeName);
		const wchar_t* shapeName = wstrShapeName.c_str();
#else
		const char* shapeName = pShape->getName();
#endif
		idx = m_List_Shapes.InsertItem( m_List_Shapes.GetItemCount(), shapeName);

		UpdateShape( idx, pShape);
	}
}

void i3TDKDlg_Physix_Tool::UpdateShape( INT32 idx, NxShape * pShape)
{
	TCHAR conv[256];

	m_List_Shapes.SetItemData( idx, (DWORD_PTR) pShape);

	// Type
	switch( pShape->getType())
	{
		case NX_SHAPE_PLANE :	i3::safe_string_copy( conv, _T("Plane"),   _countof(conv) );	break;
		case NX_SHAPE_SPHERE :	i3::safe_string_copy( conv, _T("Sphere"),  _countof(conv));	break;
		case NX_SHAPE_BOX :		i3::safe_string_copy( conv, _T("Box"),     _countof(conv));	break;
		case NX_SHAPE_CAPSULE :	i3::safe_string_copy( conv, _T("Capsule"), _countof(conv));	break;
		case NX_SHAPE_WHEEL :	i3::safe_string_copy( conv, _T("Wheel"),   _countof(conv));	break;
		case NX_SHAPE_CONVEX :	i3::safe_string_copy( conv, _T("Convex"),  _countof(conv));	break;
		case NX_SHAPE_MESH :	i3::safe_string_copy( conv, _T("Mesh"),    _countof(conv));	break;
		case NX_SHAPE_HEIGHTFIELD :	i3::safe_string_copy( conv, _T("Heightfield"), _countof(conv) );	break;
		default :				i3::safe_string_copy( conv, _T("Unknown"), _countof(conv) );	break;
	}
	m_List_Shapes.SetItemText( idx, 0, conv);

	// Group
	i3::snprintf( conv, _countof( conv), _T("%d"), pShape->getGroup());
	m_List_Shapes.SetItemText( idx, 1, conv);

	// SkinWidth
	i3::ftoa( pShape->getSkinWidth(), conv);
	m_List_Shapes.SetItemText( idx, 2, conv);

	// Faces
	if( pShape->getType() == NX_SHAPE_CONVEX)
	{
		NxConvexShape * pConvex = (NxConvexShape *) pShape;
		NxConvexMeshDesc desc;

		pConvex->getConvexMesh().saveToDesc( desc);

		i3::snprintf( conv, _countof( conv), _T("%d"), desc.numTriangles);
		m_List_Shapes.SetItemText( idx, 3, conv);
	}
	else if( pShape->getType() == NX_SHAPE_MESH)
	{
		NxTriangleMeshShape * pMesh = (NxTriangleMeshShape *) pShape;
		NxTriangleMeshDesc desc;

		pMesh->getTriangleMesh().saveToDesc( desc);

		i3::snprintf( conv, _countof( conv), _T("%d"), desc.numTriangles);
		m_List_Shapes.SetItemText( idx, 3, conv);
	}
	else
	{
		m_List_Shapes.SetItemText( idx, 3, _T("") );
	}

	// CCD
	if( pShape->getCCDSkeleton() != nullptr)
	{
		m_List_Shapes.SetItemText( idx, 4, _T("O") );
	}
	else
	{
		m_List_Shapes.SetItemText( idx, 4, _T("") );
	}

}

void i3TDKDlg_Physix_Tool::OnNMClickTdkListShapeset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pInfo = (NMITEMACTIVATE *) (pNMHDR);

	*pResult = 0;
	
	if( pInfo->iItem == -1)
		return;
	
	i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) m_List_ShapeSet.GetItemData( pInfo->iItem);

	UpdateAllShape( pShapeSet);
}

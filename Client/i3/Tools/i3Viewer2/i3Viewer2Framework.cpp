#include "stdafx.h"
#include "i3Viewer2Framework.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/resolve_path_from_environment_variable.h"
#include "MainFrm.h"
#include "i3Base/string/ext/get_exec_path.h"
#include "i3Base/string/ext/remove_filename.h"
#include "i3tdkspec_transform.h"
#include "Panel_Spec.h"
#include "i3TDKSpec_Body.h"

I3_CLASS_INSTANCE( i3Viewer2Framework); //, i3ViewerFramework);

void GetTransform( i3Node * pNode, MATRIX * pMtx)
{
	i3::vector<i3Node*> parentList;

	pNode = pNode->GetParent();

	while((pNode != NULL))
	{
		parentList.push_back( pNode);
		pNode = pNode->GetParent();
	}


	//	i3Body * pBody = NULL;

	i3Matrix::Identity( pMtx);

	for( size_t i = 0; i < parentList.size(); i++)
	{
		pNode = parentList[i];

		if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::static_meta()))
		{
			i3Transform * pTrans = (i3Transform *) pNode;

			i3Matrix::Mul( pMtx, pMtx, pTrans->GetMatrix());
		}
		else if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::static_meta()))
		{
		}
	}
}


void ResetBoundMat(i3Node* pNode)
{
	i3BoundBox * pBox = pNode->GetBound();
	VEC3D diff;
	MATRIX mtx;

	GetTransform( pNode, &mtx);

	i3Vector::Sub( &diff, pBox->GetMax(), pBox->GetMin());

	i3Matrix::SetScale( m_pSelBoundRoot->GetMatrix(), i3Math::abs( diff.x), i3Math::abs( diff.y), i3Math::abs( diff.z));

	pBox->GetCenter( &diff);
	i3Matrix::PostTranslate( m_pSelBoundRoot->GetMatrix(), &diff);

	i3Matrix::Mul( m_pSelBoundRoot->GetMatrix(), m_pSelBoundRoot->GetMatrix(), &mtx);

}
void * getResetBoundMatFunc(void)
{

	return ResetBoundMat;
}

i3Viewer2Framework::i3Viewer2Framework(void)
{

	m_EditMode = I3_VIEWER_EDIT_MODE_NORMAL;
	m_pSelBoundRoot = NULL;
	m_pReflectMapAttr = NULL;
	m_pOctreeBoundRoot = NULL;
	m_pBSPPlane = NULL;
	m_pSgInfo = NULL;

	m_pNormalViewRoot = NULL;
	m_pMeshViewRoot = NULL;
	m_pCollViewRoot = NULL;
	m_pWireViewRoot = NULL;

	m_pInterLinkNode = NULL;

	m_pSelNode = NULL;
	m_pAxis = NULL;
	m_pLightAxis = NULL;
	m_bLightAxisVisible = TRUE;
}

i3Viewer2Framework::~i3Viewer2Framework(void)
{
	I3_SAFE_NODE_RELEASE( m_pSelBoundRoot);
	I3_SAFE_NODE_RELEASE( m_pOctreeBoundRoot);
	I3_SAFE_NODE_RELEASE( m_pBSPPlane);
	I3_SAFE_NODE_RELEASE( m_pInterLinkNode);
	I3_SAFE_RELEASE( m_pReflectMapAttr);

	I3_SAFE_RELEASE( m_pSgInfo);

	I3_SAFE_RELEASE( m_pNormalViewRoot);
	I3_SAFE_RELEASE( m_pMeshViewRoot);
	I3_SAFE_RELEASE( m_pCollViewRoot);
	I3_SAFE_RELEASE( m_pWireViewRoot);

	I3_SAFE_RELEASE( m_pAxis);
	I3_SAFE_RELEASE( m_pLightAxis);
}

void i3Viewer2Framework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	i3ResourceFile::setSearchExternRefEnable( true);

	//
	{
		m_pSelBoundRoot = i3Transform::new_object();
		m_pSelBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);

		Get3DRoot()->AddChild( m_pSelBoundRoot);

		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
		m_pSelBoundRoot->AddChild( pAttrSet);

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			COLORREAL col;

			i3Color::Set( &col, 1.0f, 0.8f, 0.2f, 1.0f);

			pAttr->Set( &col);
			pAttrSet->AddAttr( pAttr);
		}

		i3Geometry * pGeo = i3Geometry::new_object_ref();
		pAttrSet->AddChild( pGeo);

		i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		{
			i3VertexArray * pVA;
			i3VertexFormat fmt;

			pVA = i3VertexArray::new_object_ref();
			pVA->Create( &fmt, 24, 0);

			pVA->Lock();
	
			VEC3D pos[24];
			i3mem::FillZero(pos, sizeof(pos));

			i3Vector::Set( &pos[ 0], -0.5f, -0.5f, -0.5f);		// 아래쪽 사각형
			i3Vector::Set( &pos[ 1],  0.5f, -0.5f, -0.5f);
			i3Vector::Set( &pos[ 2],  0.5f, -0.5f, -0.5f);
			i3Vector::Set( &pos[ 3],  0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 4],  0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 5], -0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 6], -0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[ 7], -0.5f, -0.5f, -0.5f);

			i3Vector::Set( &pos[ 8], -0.5f,  0.5f, -0.5f);		// 위쪽 사각형
			i3Vector::Set( &pos[ 9],  0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[10],  0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[11],  0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[12],  0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[13], -0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[14], -0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[15], -0.5f,  0.5f, -0.5f);

			i3Vector::Set( &pos[16], -0.5f, -0.5f, -0.5f);		// 세로 선
			i3Vector::Set( &pos[17], -0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[18],  0.5f, -0.5f, -0.5f);
			i3Vector::Set( &pos[19],  0.5f,  0.5f, -0.5f);
			i3Vector::Set( &pos[20],  0.5f, -0.5f,  0.5f);
			i3Vector::Set( &pos[21],  0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[22], -0.5f,  0.5f,  0.5f);
			i3Vector::Set( &pos[23], -0.5f, -0.5f,  0.5f);

			pVA->SetPositions( 0, 24, pos);
			pVA->Unlock();
			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 12);
		}
	}

	{
		_CreateOctreeBound();
	}

	// Default ReflectMap
	{
		m_pReflectMapAttr = i3ReflectMapBindAttr::new_object();

		m_pAttrSet->AddAttr( m_pReflectMapAttr);
	}

	{
		m_pBSPPlane = i3Transform::new_object();
		m_pBSPPlane->AddFlag( I3_NODEFLAG_DISABLE);

		Get3DRoot()->AddChild( m_pBSPPlane);

		// Plane
		{
			i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
			m_pBSPPlane->AddChild( pAttrSet);

			{
				i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
				pAttr->Set( TRUE);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
				pAttr->SetSource( I3G_BLEND_SRCALPHA);
				pAttr->SetDestination( I3G_BLEND_ONE);
				pAttrSet->AddAttr( pAttr);
			}

			{
				i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
				pAttr->Set( FALSE);
				pAttrSet->AddAttr( pAttr);
			}

			i3Geometry * pGeo = i3Geometry::new_object_ref();
			pAttrSet->AddChild( pGeo);

			i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();
			pGeo->AppendGeometryAttr( pGeoAttr);

			{
				i3VertexFormat fmt;
				i3VertexArray * pVA = i3VertexArray::new_object_ref();

				fmt.SetHasPosition();
				fmt.SetHasColor( true);

				pVA->Create( &fmt, 6, 0);
				pVA->Lock();

				{
					VEC3D pos[6];
					
					i3Vector::Set( &pos[0], -0.5f, 0.5f, 0.0f);
					i3Vector::Set( &pos[1], -0.5f, -0.5f, 0.0f);
					i3Vector::Set( &pos[2],  0.5f, 0.5f, 0.0f);
					i3Vector::Set( &pos[3],  0.5f, 0.5f, 0.0f);
					i3Vector::Set( &pos[4], -0.5f, -0.5f, 0.0f);
					i3Vector::Set( &pos[5],  0.5f, -0.5f, 0.0f);

					pVA->SetPositions( 0, 6, pos);
				}

				{
					COLOR col;

					i3Color::Set( &col, (UINT8)255, (UINT8)128, (UINT8)64, (UINT8)128);
					pVA->SetColor( 0, 6, &col);
				}

				pVA->Unlock();

				pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
			}
		}
	}

	{
		m_pMeshViewRoot = i3AttrSet::new_object();
		m_pCollViewRoot = i3AttrSet::new_object();
		m_pWireViewRoot = i3AttrSet::new_object();

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set( FALSE);

			m_pMeshViewRoot->AddAttr( pAttr);
			m_pCollViewRoot->AddAttr( pAttr);
			m_pWireViewRoot->AddAttr( pAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

			pAttr->Set( FALSE);

			m_pMeshViewRoot->AddAttr( pAttr);
			m_pCollViewRoot->AddAttr( pAttr);
			m_pWireViewRoot->AddAttr( pAttr);
		}
	}

	{
		m_pInterLinkNode = i3Node::new_object();

		m_pInterLinkNode->SetFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);
		m_pInterLinkNode->SetName( "_InterLinkNode_");
	}

	{
		m_pAxis = i3Viewer2Axis::new_object();
		Get3DRoot()->AddChild(m_pAxis);
		m_pAxis->CreateAxis();
		m_pAxis->Disable();
	}

	{
		m_pLightAxis = i3Viewer2LightAxis::new_object();
		m_pLightAxis->CreateLightAxis();
		Get3DRoot()->AddChild(m_pLightAxis); 
	}

	ApplyOption();
}

void i3Viewer2Framework::OnUpdate(REAL32 timeDelta)
{
	i3ViewerFramework::OnUpdate(timeDelta);

	//오브젝트 Rotate모드에서는 고정임~
//	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

// 	if (pMainWnd != NULL)
// 	{
// 		if( (i3TDKSpec_Body*)pMainWnd->GetPanel_Spec() && (i3TDKSpec_Body*)pMainWnd->GetPanel_Spec()->GetBody())
// 		{
// 			i3TDKSpec_Body* pTDK_Body = (i3TDKSpec_Body*)pMainWnd->GetPanel_Spec()->GetBody();
// 		
// 			i3Camera * pCam = GetCurrentCamera();
// 			MATRIX * pCamera = pCam->GetMatrix();
// 
// 			if( pTDK_Body )
// 			{
// 				bool bCheck = ((CButton *)pTDK_Body->GetDlgItem(IDC_TDK_CHECK_USEOBJECT))->GetCheck();
// 				if( bCheck )
// 				{
// 					if( pTDK_Body->GetDist() )
// 						i3Matrix::SetTranslate(pCamera, 0.f, 0.f, pTDK_Body->GetDist());
// 				}
// 			}
// 		}
// 	}

	ResizeLightAxis();
}

void i3Viewer2Framework::_CreateOctreeBound(void)
{
	m_pOctreeBoundRoot = i3AttrSet::new_object();
	m_pOctreeBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);

	Get3DRoot()->AddChild( m_pOctreeBoundRoot);

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( FALSE);

		m_pOctreeBoundRoot->AddAttr( pAttr);
	}

	{
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
		COLORREAL col;

		i3Color::Set( &col, 1.0f, 0.0f, 0.0f, 1.0f);

		pAttr->Set( &col);

		m_pOctreeBoundRoot->AddAttr( pAttr);
	}

	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
		pAttr->Set( FALSE);
		m_pOctreeBoundRoot->AddAttr( pAttr);
	}

	i3Geometry * pGeo = i3Geometry::new_object_ref();
	m_pOctreeBoundRoot->AddChild( pGeo);

	i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();
	pGeo->AppendGeometryAttr( pGeoAttr);

	i3VertexFormat fmt;

	fmt.SetHasPosition( true);
	//fmt.SetHasColor( true);

	m_pOctreeBoundLineVA = i3VertexArray::new_object_ref();
	m_pOctreeBoundLineVA->Create( &fmt, 14, 0);

	pGeoAttr->SetVertexArray( m_pOctreeBoundLineVA, I3G_PRIM_LINELIST, 15);

	i3IndexArray * pIA = i3IndexArray::new_object_ref();
	pIA->Create( 30, 0, I3G_IDX_16BITS);
	pIA->Lock();

	// Upper X-Z rectangle ( 0, 1, 2, 3)
	// Lower X-Z ractangle ( 4, 5, 6, 7)
	// Center of Left/Right ( 8, 9)
	// Center of Front/Back ( 10, 11)
	// Center of Top/Bottom ( 12, 13)

	// Upper rectangle
	pIA->SetIndex( 0,	0);
	pIA->SetIndex( 1,	1);	
	pIA->SetIndex( 2,	1);
	pIA->SetIndex( 3,	2);
	pIA->SetIndex( 4,	2);
	pIA->SetIndex( 5,	3);
	pIA->SetIndex( 6,	3);
	pIA->SetIndex( 7,	0);

	// Lower rectnagle
	pIA->SetIndex( 8,	4);
	pIA->SetIndex( 9,	5);	
	pIA->SetIndex( 10,	5);
	pIA->SetIndex( 11,	6);
	pIA->SetIndex( 12,	6);
	pIA->SetIndex( 13,	7);
	pIA->SetIndex( 14,	7);
	pIA->SetIndex( 15,	4);

	// Vertical Line
	pIA->SetIndex( 16,	0);
	pIA->SetIndex( 17,	4);	
	pIA->SetIndex( 18,	1);
	pIA->SetIndex( 19,	5);
	pIA->SetIndex( 20,	2);
	pIA->SetIndex( 21,	6);
	pIA->SetIndex( 22,	3);
	pIA->SetIndex( 23,	7);

	// Cross lines
	pIA->SetIndex( 24,	8);
	pIA->SetIndex( 25,	9);	
	pIA->SetIndex( 26,	10);
	pIA->SetIndex( 27,	11);
	pIA->SetIndex( 28,	12);
	pIA->SetIndex( 29,	13);

	pIA->Unlock();

	pGeoAttr->SetIndexArray( pIA);
}

void i3Viewer2Framework::_SelectOctree( i3OcTreeNode * pNode)
{
	if( pNode == NULL)
		m_pOctreeBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);
	else
	{
		m_pOctreeBoundRoot->RemoveFlag( I3_NODEFLAG_DISABLE);

		VEC3D pos[14], * pMin, *pMax, *pCenter;
		VEC3D vmin, vmax, dir;

		i3Vector::Copy( &vmin, pNode->getMin());
		i3Vector::Copy( &vmax, pNode->getMax());

		i3Vector::Sub( &dir, &vmax, &vmin);
		i3Vector::Scale( &dir, &dir, 0.5f * (pNode->getK() - 1.0f));

		i3Vector::Sub( &vmin, &vmin, &dir);
		i3Vector::Add( &vmax, &vmax, &dir);

		pMin = &vmin;
		pMax = &vmax;
		pCenter = pNode->getCenter();

		// Upper X-Z rectangle ( 0, 1, 2, 3)
		i3Vector::Set( &pos[0], pMin->x, pMax->y, pMin->z);
		i3Vector::Set( &pos[1], pMin->x, pMax->y, pMax->z);
		i3Vector::Set( &pos[2], pMax->x, pMax->y, pMax->z);
		i3Vector::Set( &pos[3], pMax->x, pMax->y, pMin->z);

		// Lower X-Z ractangle ( 4, 5, 6, 7)
		i3Vector::Set( &pos[4], pMin->x, pMin->y, pMin->z);
		i3Vector::Set( &pos[5], pMin->x, pMin->y, pMax->z);
		i3Vector::Set( &pos[6], pMax->x, pMin->y, pMax->z);
		i3Vector::Set( &pos[7], pMax->x, pMin->y, pMin->z);

		// Center of Left/Right ( 8, 9)
		i3Vector::Set( &pos[8], pMin->x, pCenter->y, pCenter->z);
		i3Vector::Set( &pos[9], pMax->x, pCenter->y, pCenter->z);

		// Center of Front/Back ( 10, 11)
		i3Vector::Set( &pos[10], pCenter->x, pCenter->y, pMin->z);
		i3Vector::Set( &pos[11], pCenter->x, pCenter->y, pMax->z);

		// Center of Top/Bottom ( 12, 13)
		i3Vector::Set( &pos[12], pCenter->x, pMin->y, pCenter->z);
		i3Vector::Set( &pos[13], pCenter->x, pMax->y, pCenter->z);

		m_pOctreeBoundLineVA->Lock();

		m_pOctreeBoundLineVA->SetPositions( 0, 14, pos);

		m_pOctreeBoundLineVA->Unlock();
	}
}

void i3Viewer2Framework::_GetFullTransform( i3Node * pNode, MATRIX * pMtx)
{
	i3::vector<i3Node*> parentList;

	pNode = pNode->GetParent();

	while((pNode != NULL))
	{
		parentList.push_back( pNode);
		pNode = pNode->GetParent();
	}

	
//	i3Body * pBody = NULL;

	i3Matrix::Identity( pMtx);

	for( size_t i = 0; i < parentList.size(); i++)
	{
		pNode = parentList[i];

		if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::static_meta()))
		{
			i3Transform * pTrans = (i3Transform *) pNode;

			i3Matrix::Mul( pMtx, pMtx, pTrans->GetMatrix());
		}
		else if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::static_meta()))
		{
		}
	}
}

void i3Viewer2Framework::GetFullTransform( i3Node * pNode, MATRIX * pMatrix)
{
	i3Matrix::Identity( pMatrix);

	while( pNode != NULL)
	{
		if( pNode->kind_of( i3Transform::static_meta()))
		{
			i3Transform * pTrans = (i3Transform *) pNode;

			i3Matrix::Mul( pMatrix, pMatrix, pTrans->GetMatrix());
		}
		pNode = pNode->GetParent();
	}
}

void i3Viewer2Framework::SelectNode( i3Node * pNode)
{
	m_pSelNode = pNode;	

	if( g_pOption->m_ViewerMode == VIEWER_MODE_SELONLY)
	{
		_SetIsolateMode( TRUE);
	}

	if(pNode != NULL)
	{
		if(pNode->GetBound() != NULL)
		{
			m_pSelBoundRoot->RemoveFlag( I3_NODEFLAG_DISABLE);

			i3BoundBox * pBox = pNode->GetBound();
			VEC3D diff;

			i3Vector::Sub( &diff, pBox->GetMax(), pBox->GetMin());

			i3Matrix::SetScale( m_pSelBoundRoot->GetMatrix(), i3Math::abs( diff.x), i3Math::abs( diff.y), i3Math::abs( diff.z));

			pBox->GetCenter( &diff);
			i3Matrix::PostTranslate( m_pSelBoundRoot->GetMatrix(), &diff);

			MATRIX mtxFull;

			GetFullTransform( pNode, &mtxFull);

			i3Matrix::Mul( m_pSelBoundRoot->GetMatrix(), m_pSelBoundRoot->GetMatrix(), &mtxFull);
		}
		else
		{
			m_pSelBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);
		}

		if(i3::kind_of<i3Transform*>(pNode))
		{
			m_pAxis->Enable();
			m_pSelBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);

			i3Matrix::Identity(m_pAxis->GetMatrix());

			//선택한 노드의 위치에 맞춰 m_pAxis의 포지션을 설정해준다.
			//i3Transform이 아니라면 m_pAxis를 Disable시켜야 함
			MATRIX mtxFull;
			MATRIX mtxTemp;
			VEC3D diff;
			i3Transform * pTrans = (i3Transform *) pNode;

			GetFullTransform(pNode, &mtxFull);
			//World Matrix 변환
			i3Matrix::Mul(m_pAxis->GetMatrix(), m_pAxis->GetMatrix(), &mtxFull);
			i3Matrix::Mul(&mtxTemp, pTrans->GetMatrix(), &mtxFull);
			i3Vector::Sub(&diff, i3Matrix::GetPos(&mtxTemp), i3Matrix::GetPos(m_pAxis->GetMatrix()));

			i3Matrix::PostTranslate(m_pAxis->GetMatrix(), &diff);
			ResizeAxis();
		}
		else
		{
			m_pAxis->Disable();
		}

		if( i3::kind_of<i3OcTreeNode*>(pNode))
		{
			_SelectOctree( (i3OcTreeNode *) pNode);
		}

		if( i3::same_of<i3BinaryPartitionNode*>(pNode))
		{
			i3BinaryPartitionNode * pBSPNode = (i3BinaryPartitionNode *) pNode;

			MATRIX mtx;
			i3BoundBox box;

			pBSPNode->GetPlaceMatrix( &mtx);

			pBSPNode->GetWrappingBound( &box);
			VEC3D diff;

			i3Vector::Sub( &diff, box.GetMax(), box.GetMin());

			i3Matrix::PreScale( &mtx, diff.x, diff.y, diff.z);

			VEC3D center;
			mtx, box.GetCenter( &center);
			i3Matrix::PostTranslate( &mtx, &center);

			m_pBSPPlane->SetMatrix( &mtx);

			m_pBSPPlane->RemoveFlag( I3_NODEFLAG_DISABLE);
		}
		else
		{
			m_pBSPPlane->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}
	else
	{
		m_pBSPPlane->AddFlag(I3_NODEFLAG_DISABLE);
		m_pAxis->Disable();
		m_pSelBoundRoot->AddFlag(I3_NODEFLAG_DISABLE);

		_SelectOctree( NULL);
	}
}

void i3Viewer2Framework::SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize)
{
	i3Light * pLight = m_pLightAttr->GetLightObject();

	if( pLight )
	{
		pLight->SetDiffuse( pDiff);
		pLight->SetSpecular( pSpec);
		pLight->setShadowMapSize( shadowSize);

		if( pLight->getStyle() & I3G_LIGHT_STYLE_SHADOWMAP )
		{
			pLight->CreateShadowMap();
		}
	}
}

bool i3Viewer2Framework::CloseI3S(void)
{
	i3ViewerFramework::CloseI3S();

	I3_SAFE_RELEASE( m_pSgInfo);
	m_pSelNode = NULL;
	I3_SAFE_RELEASE( m_pNormalViewRoot);

	m_pMeshViewRoot->RemoveAllChild();
	m_pCollViewRoot->RemoveAllChild();
	m_pWireViewRoot->RemoveAllChild();

	setViewMode( VIEWER_MODE_NORMAL, true);

	removeAllInitBone();

	

	return true;
}

void i3Viewer2Framework::RefineSg( bool bResetView)
{
	i3Node * pRoot = NULL;

	m_pSelNode = NULL;

	m_pInterLinkNode->RemoveFromParent();
	m_pInterLinkNode->RemoveAllChild();

	// CommonSg가 있는 경우, CommonSg.를 상위로, InstanceSg를 하위로 붙여주어야 올바른 Visual이 나온다.
	if( m_pSgInfo != NULL)
	{
		if( m_pSgInfo->getCommonSg() != NULL)
		{
			if( m_pSgInfo->getInstanceSg() != NULL)
			{
				i3Node * pAttachNode = m_pSgInfo->getCommonSg();

				while( pAttachNode->GetChildCount() > 0)
				{
					pAttachNode = pAttachNode->GetChild( pAttachNode->GetChildCount() - 1);
				}

				// CommonSg와 InstanceSg를 임시로 연결하기 위해 m_pInterLinkNode를 사용한다.
				{
					pAttachNode->AddChild( m_pInterLinkNode);

					m_pInterLinkNode->AddChild( m_pSgInfo->getInstanceSg());
				}
				
			}

			pRoot = m_pSgInfo->getCommonSg();
		}
		else
		{
			pRoot = m_pSgInfo->getInstanceSg();
		}
	}

	if( pRoot != NULL)
	{
		pRoot->Instantiate();
	}

	// Loading된 Sg.의 Physics 상태를 Kinematic으로 설정.
	SetDynamicState( false);

	I3_REF_CHANGE( m_pNormalViewRoot, pRoot);

	// Bone들의 초기값을 저장해둔다.
	BuildInitBone();

	setViewMode( VIEWER_MODE_NORMAL, bResetView);

	UpdateBound();

	{
		m_pSelBoundRoot->GetMatrix()->m00 = 0;
		m_pSelBoundRoot->GetMatrix()->m01 = 0;
		m_pSelBoundRoot->GetMatrix()->m02 = 0;
		m_pSelBoundRoot->GetMatrix()->m03 = 0;
		m_pSelBoundRoot->GetMatrix()->m10 = 0;
		m_pSelBoundRoot->GetMatrix()->m11 = 0;
		m_pSelBoundRoot->GetMatrix()->m12 = 0;
		m_pSelBoundRoot->GetMatrix()->m13 = 0;
		m_pSelBoundRoot->GetMatrix()->m20 = 0;
		m_pSelBoundRoot->GetMatrix()->m21 = 0;
		m_pSelBoundRoot->GetMatrix()->m22 = 0;
		m_pSelBoundRoot->GetMatrix()->m23 = 0;
		m_pSelBoundRoot->GetMatrix()->m30 = 0;
		m_pSelBoundRoot->GetMatrix()->m31 = 0;
		m_pSelBoundRoot->GetMatrix()->m32 = 0;
		m_pSelBoundRoot->GetMatrix()->m33 = 0;

		
	}

	i3Camera * pCam = GetCurrentCamera();
	MATRIX * pCamera = pCam->GetMatrix();
	TransFormZ = pCamera->m32;

	m_pSelBoundRoot->AddFlag( I3_NODEFLAG_DISABLE);
	m_pAxis->Disable();

}


bool i3Viewer2Framework::LoadI3S( const char * pszPath)
{
	i3SceneFile file;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		return false;
	}

	i3SceneGraphInfo * pSgInfo = file.getSceneGraphInfo();

	I3_REF_CHANGE( m_pSgInfo, pSgInfo);

	RefineSg( true);
	RePositionLightAxis();

	RESET_PROFILE();
	REPORT_PROFILE();

	return true;
}

void	i3Viewer2Framework::setSgInfo( i3SceneGraphInfo * pSgInfo)
{
	I3_REF_CHANGE( m_pSgInfo, pSgInfo);
	
	RefineSg( true);
}

bool i3Viewer2Framework::SaveI3S( const char * pszPath)
{
	i3SceneFile file;

	if( m_pSgInfo == NULL)
		return false;

	file.setSceneGraphInfo( m_pSgInfo);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		return false;
	}

	i3ResourceFile::SetProtectionCode( pszPath, "Zepetto Inc.", 12);

	return true;
}

bool	i3Viewer2Framework::LoadAnimation( const char * pszPath)
{
	i3Animation * pAnim = i3Animation::LoadFromFile( pszPath);

	if( pAnim == NULL)
		return false;

	if( m_pSgInfo == NULL)
		return false;

	if( m_pSgInfo->getInstanceSg() == NULL)
		return false;

	i3Body * pBody = (i3Body*) i3Scene::FindNodeByExactType( m_pSgInfo->getInstanceSg(), i3Body::static_meta());

	if( pBody == NULL)
		return false;

	pAnim->AddRef();
	pBody->PlayAnim( pAnim);

	I3_SAFE_RELEASE( pAnim);

	return true;
}

void i3Viewer2Framework::ApplyOptionColors(void)
{
	i3RenderContext * pCtx = GetViewer()->GetRenderContext();

	// Background Color
	{
		COLOR col;

		i3Color::Set( &col, & g_pOption->m_BackColor);

		pCtx->SetClearColor( &col);
	}

	// Ambient Color
	{
		getSgContext()->setAmbient( &g_pOption->m_Ambient);
	}

	// Light
	{
		COLORREAL	color;

		i3Color::Scale( &color, &g_pOption->m_LightDiffuse, g_pOption->m_LightIntensity);
		m_pLightAttr->setDiffuse( &color);

		i3Color::Scale( &color, &g_pOption->m_LightSpecular, g_pOption->m_LightIntensity);
		m_pLightAttr->setSpecular( &color);

		// Transform
		m_LightTheta	= g_pOption->m_LightTheta;
		m_LightRho		= g_pOption->m_LightRho;

		UpdateLightTx();
	}
}

bool i3Viewer2Framework::ApplyOptionReflectMap(void)
{
	i3TextureCube * pTex;
	i3ImageFile file;
	char szFull[ MAX_PATH];

	i3::stack_string strPath;

	if( g_pOption->m_szReflectMap[0] == 0)
	{
		i3::resolve_path_from_environment_variable( "$(I3_ROOT)/Bin/res", strPath);

		i3::snprintf( szFull, MAX_PATH, "%s/defl_cubemap.dds", strPath);

		if( i3System::IsFile( szFull) == FALSE)
		{
			i3::get_exec_path( strPath);

			i3::snprintf( szFull, MAX_PATH, "%s/res/defl_cubemap.dds", strPath);
		}
	}
	else
	{
		i3::safe_string_copy( szFull, g_pOption->m_szReflectMap, MAX_PATH );
	}

	pTex = (i3TextureCube *) file.Load( szFull);

	if( pTex != NULL)
	{
		m_pReflectMapAttr->SetTexture( pTex);

		pTex->Release();
	}
	else
	{
		NotifyBox( NULL, "%s 파일을 읽어들이지 못했습니다. 파일에 문제가 있거나, 지원하지 않는 형식(HDRI 포함)의 이미지일 수 있습니다.", szFull);
		return false;
	}

	return true;
}

void i3Viewer2Framework::ApplyOptionShadow(void)
{
	i3Light * pLight = m_pLightAttr->GetLightObject();

	if( g_pOption->m_bShadowEnable)
	{
		// Shadow On
		pLight->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);

		if( pLight->getShadowMapSize() != g_pOption->m_ShadowSize)
			pLight->setShadowMapSize( g_pOption->m_ShadowSize);		// ShadowMap Size
	}
	else
	{
		// Shadow Off
		pLight->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);
	}
}

void i3Viewer2Framework::ApplyOptionCamera(void)
{
	i3Camera * pCamera = GetCurrentCamera();
	pCamera->SetFOV(I3_DEG2RAD((REAL32)g_pOption->m_fovDegree));
}

void i3Viewer2Framework::ApplyOption(void)
{
	// Option에 맞추어 필요한 설정을 한다.
	{
		ApplyOptionColors();

		// Shadow
		ApplyOptionShadow();
	}

	setViewMode( g_pOption->m_ViewerMode);

	// Grid
	SetGridShow( g_pOption->m_bGridShow == TRUE);

	ApplyOptionReflectMap();
	ApplyOptionCamera();
}

void i3Viewer2Framework::setEditMode( I3_VIEWER_EDIT_MODE mode)
{
	m_EditMode = mode;

	switch( mode)
	{
		case I3_VIEWER_EDIT_MODE_NORMAL :
			break;

		case I3_VIEWER_EDIT_MODE_LIGHT :
			break;
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, g_pOption);
}

struct ISOLATE_INFO
{
	i3Node *		m_pNode = nullptr;
	bool			m_bStatus = false;
} ;

static I3SG_TRAVERSAL_RESULT _IsolateNodeProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	ISOLATE_INFO * pInfo = (ISOLATE_INFO *) pData;

	bool bVisible = ! pInfo->m_bStatus;

	// 부모 노드에서 대상 Node가 있는지 확인.
	{
		i3Node * pParent = pNode;

		while( pParent != NULL)
		{
			if( pParent == pInfo->m_pNode)
			{
				bVisible = true;
				break;
			}

			pParent = pParent->GetParent();
		}
	}

	if( bVisible)
		pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
	else
		pNode->AddFlag( I3_NODEFLAG_DISABLE);

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Viewer2Framework::_SetIsolateMode( bool bFlag)
{
	if( m_pNormalViewRoot == NULL)
		return;

	//파티클i3s일때 처음에 로드하자마자 이펙트를 draw하는 경우가있어서
	//node flag셋팅을 바꾸면 안됨
	for (INT32 i = 0; i < m_pNormalViewRoot->GetChildCount(); i++)
	{
		if (i3::kind_of<i3Particle*>(m_pNormalViewRoot->GetChild(i)))
			return;
	}


	ISOLATE_INFO info;

	info.m_pNode = m_pSelNode;
	info.m_bStatus = bFlag;

	i3Scene::Traverse( m_pNormalViewRoot, _IsolateNodeProc, &info);

	// 부모부터 상위 Root까지 모두 Enable
	i3Node * pTemp = m_pSelNode;
	
	while( pTemp != NULL)
	{
		pTemp->RemoveFlag( I3_NODEFLAG_DISABLE);

		pTemp = pTemp->GetParent();
	}
}

void i3Viewer2Framework::setViewMode( VIEWER_MODE mode, bool bReset)
{
	i3Node *	pRoot = NULL;

	g_pOption->m_ViewerMode = mode;

	switch( mode)
	{
		case VIEWER_MODE_NORMAL :
			pRoot = m_pNormalViewRoot;

			_SetIsolateMode( false);
			break;

		case VIEWER_MODE_SELONLY :
			pRoot = m_pNormalViewRoot;

			_SetIsolateMode( true);
			break;

		case VIEWER_MODE_MESH :
			if(( m_pNormalViewRoot != NULL) && (m_pMeshViewRoot->GetChildCount() == 0))
			{
				i3Node * pTemp = i3SceneUtil::BuildInstanceMesh( m_pNormalViewRoot, FALSE);

				m_pMeshViewRoot->AddChild( pTemp);
			}

			pRoot = m_pMeshViewRoot;
			break;

		case VIEWER_MODE_COLL :

			m_pCollViewRoot->RemoveAllChild();
			if(( m_pNormalViewRoot != NULL) && (m_pCollViewRoot->GetChildCount() == 0))
			{
//				i3SceneUtil::BuildLogger generateStatus;
				
				i3Node * pTemp = i3SceneUtil::BuildRendableCollider(m_pNormalViewRoot,
					NULL, TRUE, FALSE, true, NULL, NULL, false, 
					NULL, //&generateStatus, 
					0, NULL);
				
				/*std::string msg;
				char bufST[MAX_PATH] = {0};

				if(generateStatus.GetNumGenColliders())
				{
					i3SceneUtil::BuildLogger::MsgStatus st;
					generateStatus.GenerateStatusMsg(&st, i3SceneUtil::BuildLogger::ELF_INC_PHYSHAPE_GEN);

					sprintf_s(bufST, MAX_PATH, "[Generated Colliders] Total=%d \n", generateStatus.GetNumGenColliders());
					msg = msg + bufST;

					for(INT32 i=0; i<st.size(); i++)
					{
						if(*(st[i].c_str()) != '@')
							msg = msg + st[i].c_str() + "\n";
					}

					msg = msg + "\n\n";
				}

				if(generateStatus.GetNumGenFailColliders())
				{
					i3SceneUtil::BuildLogger::MsgStatus st;
					generateStatus.GenerateStatusMsg(&st, i3SceneUtil::BuildLogger::ELF_INC_PHYSHAPE_MISS);

					sprintf_s(bufST, MAX_PATH, "[Failed to generate colliders ] Total=%d \n", 
						generateStatus.GetNumGenFailColliders());
					msg = msg + bufST;

					for(INT32 i=0; i<st.size(); i++)
					{
						if(*(st[i].c_str()) != '@')
							msg = msg + st[i].c_str() + "\n";
					}

					msg = msg + "\n\n";
				}

				if(msg.empty())
					AfxMessageBox("No Colliders is in Scene", MB_OK);
				else
					AfxMessageBox(msg.c_str(), generateStatus.GetNumGenFailColliders() ? MB_ICONERROR : MB_OK);*/

				m_pCollViewRoot->AddChild( pTemp);

				if(pTemp)
					pTemp->Release(); 
			}

			pRoot = m_pCollViewRoot;
			break;

		case VIEWER_MODE_WIRE :
			if(( m_pNormalViewRoot != NULL) && (m_pWireViewRoot->GetChildCount() == 0))
			{
				//i3Node * pTemp = i3SceneUtil::BuildWireMesh( m_pNormalViewRoot);
				i3RenderModeAttr * pAttr = i3RenderModeAttr::new_object_ref();

				pAttr->Set( I3G_RENDER_LINE);

				m_pWireViewRoot->AddAttr( pAttr);

				m_pWireViewRoot->AddChild( m_pNormalViewRoot);
			}

			pRoot = m_pWireViewRoot;
			break;
	}

	SetSceneGraph( pRoot, bReset);

	i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, g_pOption);
}

void i3Viewer2Framework::RecordCam( INT32 idx)
{
	MATRIX * pMatrix = 	m_pCurCamera->GetMatrix();

	i3Vector::Copy( &g_pOption->m_CamPos[idx], i3Matrix::GetPos( pMatrix));
	i3Matrix::GetRotateQuat( &g_pOption->m_CamRot[idx], pMatrix);

	I3TRACE( "Record %d camera\n", idx + 1);
}

void i3Viewer2Framework::SetCamPos( INT32 idx)
{
	MATRIX * pMatrix = 	m_pCurCamera->GetMatrix();

	i3Matrix::SetRotateQuat( pMatrix, & g_pOption->m_CamRot[idx]);
	i3Matrix::SetPos( pMatrix, &g_pOption->m_CamPos[idx]);
}

INIT_BONE_INFO * i3Viewer2Framework::addInitBone( const char * pszBone, MATRIX * pMatrix)
{
	INIT_BONE_INFO * pInfo;

	pInfo = new INIT_BONE_INFO;

	i3::safe_string_copy( pInfo->m_szName, pszBone, 256 );
	i3Matrix::Copy( &pInfo->m_Matrix, pMatrix);

	m_InitBoneList.push_back( pInfo);

	return pInfo;
}

INIT_BONE_INFO * i3Viewer2Framework::findInitBone( const char * pszBone)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		if( i3::generic_is_iequal( pInfo->m_szName, pszBone) )
			return pInfo;
	}

	return NULL;
}

void i3Viewer2Framework::removeAllInitBone(void)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		delete pInfo;
	}

	m_InitBoneList.clear();
}

void i3Viewer2Framework::BuildInitBone(void)
{
	// Sg.를 돌면서 Bone으로 간주되는 노드들의 초기 Matrix를 저장해 둔다.
	removeAllInitBone();

	if( m_pNormalViewRoot != NULL)
	{
		i3Scene::Traverse( m_pNormalViewRoot, _BuildInitBoneProc, this);
	}
}

I3SG_TRAVERSAL_RESULT i3Viewer2Framework::_BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	i3Viewer2Framework * pFramework = (i3Viewer2Framework *) pUserData;

	if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				pFramework->addInitBone( pSkel->getName( i), pSkel->getMatrix( i));
			}
		}
	}
	else if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::static_meta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		pFramework->addInitBone( pTrans->GetName(), pTrans->GetMatrix());
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Viewer2Framework::ApplyInitBone(void)
{
	// // Sg.를 돌면서 Bone으로 간주되는 노드들오 초기 Matrix를 설정해준다.
	if( m_pNormalViewRoot != NULL)
		i3Scene::Traverse( m_pNormalViewRoot, _ApplyInitBoneProc, this);
}

I3SG_TRAVERSAL_RESULT i3Viewer2Framework::_ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	i3Viewer2Framework * pFramework = (i3Viewer2Framework *) pUserData;

	if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				INIT_BONE_INFO * pInfo = pFramework->findInitBone( pSkel->getName( i));

				if( pInfo != NULL)
					pSkel->setMatrix( i, &pInfo->m_Matrix);
			}
		}
	}
	else if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::static_meta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		INIT_BONE_INFO * pInfo = pFramework->findInitBone( pTrans->GetName());

		if( pInfo != NULL)
			pTrans->SetMatrix( &pInfo->m_Matrix);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Viewer2Framework::SetDynamicState( bool bFlag)
{
	if( m_pNormalViewRoot == NULL)
		return;

	UINT32 addState, remState;

	addState = remState = 0;

	if( bFlag)
	{
		remState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}
	else
	{
		addState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}

	VEC3D vel;

	i3Vector::Set( &vel, 0.0f, 0.0f, 0.0f);

	i3Scene::ModifyDynamicState( m_pNormalViewRoot, addState, remState);
	i3Scene::SetDynamicVelocity( m_pNormalViewRoot, NULL, &vel, &vel);
}


void i3Viewer2Framework :: OnMouseWheel( UINT nFlag, short zDelta, POINT point )
{
	i3Framework::OnMouseWheel( nFlag, zDelta, point);
	if( getFocus() != NULL)
		return;

	VEC3D vec;
	i3Camera * pCam = GetCurrentCamera();
	MATRIX * pCamera = pCam->GetMatrix();
	MATRIX tmp;
	REAL32 len;

	if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_3DSMAX)
	{
		i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera));
		i3Vector::Normalize( &vec, &vec);

		if( nFlag & MK_CONTROL)
			len = 0.01f;
		else
			len = 0.05f;

		if( zDelta > 0)
		{
			i3Vector::Scale( &vec, &vec, -m_CamDist * len);
		}
		else
		{
			i3Vector::Scale( &vec, &vec, m_CamDist * len);
		}

		i3Matrix::SetTranslate(&tmp, getX(&vec), getY(&vec), getZ(&vec));

		i3Matrix::Mul( pCamera, pCamera, &tmp);
		
		OnUpdateCameraTransform();
		OnUpdateCameraProjection();
	}

	ResizeAxis();
}

void i3Viewer2Framework::Optimize(i3SceneOptimizer * pOpt, CSceneGraphTreeCtrl * pSceneGraphCtrl)
{
	//Root 노드에서부터 Optimize를 해준다.
	//기본적으로 Optimizer들은 Root 노드에서 그냥 Opt를 하는 경우가 대다수이기 때문이다. 

	if(i3TDK::PopupOptimizerSpecDialog(pOpt, pSceneGraphCtrl) == FALSE)
	{
		return;
	}

	if(pOpt->isSupportProgress())
	{
		pSceneGraphCtrl->PopupOptProgress(pOpt);
	}

	//Do Optimize
	pOpt->Optimize(pSceneGraphCtrl->getSgInfo());

	RefineSg( false);

	i3TDK::Update( NULL, I3_TDK_UPDATE_SCENE, pSceneGraphCtrl->getSgInfo());
}

void i3Viewer2Framework::UpdateLightTx()
{
	i3ViewerFramework::UpdateLightTx();
	RePositionLightAxis();
}

void i3Viewer2Framework::ResizeAxis()
{
	ResizeAxisByBound(m_pAxis);
}

void i3Viewer2Framework::ResizeLightAxis()
{
	ResizeAxisByBound(m_pLightAxis);
}

void i3Viewer2Framework::RePositionLightAxis()
{
	if(m_pLightAxis != NULL)
	{
		MATRIX * pMat = m_pLightAxis->GetMatrix();
		
		i3Matrix::SetTranslate( pMat, 0.0f, 0.0f, m_LightDist);
		i3Matrix::PostRotateXDeg( pMat, m_LightRho);
		i3Matrix::PostRotateYDeg( pMat, m_LightTheta);
	}
}


void i3Viewer2Framework::ResizeAxisByBound(i3Transform * pTrans)
{
	if(m_pCurCamera != NULL)
	{
		MATRIX matViewProj;
		MATRIX matInvViewProj;
		MATRIX matFull;
		MATRIX matAxisWorld;
		VEC3D vBoundScale;
		VEC3D vAxisScreenPos;
		VEC3D vAxisWorldPos;

		//pCamera는 아무데도 붙지 않으므로, GetPos가 World 상의 위치와 동일하다.

		i3Matrix::Mul(&matViewProj, m_pCurCamera->GetViewMatrix(), m_pCurCamera->GetProjectionMatrix());
		i3Matrix::Inverse(&matInvViewProj, NULL, &matViewProj);

		GetFullTransform(pTrans, &matFull);
		i3Vector::Copy(&vAxisWorldPos, i3Matrix::GetPos(&matFull));

		//Axis의 screen space 좌표.
		i3Vector::TransformCoord(&vAxisScreenPos, i3Matrix::GetPos(&matFull), &matViewProj);

		//Axis BoundSet의 설정
		VEC3D vBoundUpper, vBoundLower;
		i3Vector::Set(&vBoundUpper, vAxisScreenPos.x + 0.1f, vAxisScreenPos.y + 0.1f, vAxisScreenPos.z);
		i3Vector::Set(&vBoundLower, vAxisScreenPos.x - 0.1f, vAxisScreenPos.y - 0.1f, vAxisScreenPos.z);

		//Axis BoundSet의 World Space 좌표
		i3Vector::TransformCoord(&vBoundUpper, &vBoundUpper, &matInvViewProj);
		i3Vector::TransformCoord(&vBoundLower, &vBoundLower, &matInvViewProj);

		//Axis의 크기 조절
		//Identity로 설정 후, Bound 사이즈에 맞게 스케일링을 한다.

		i3Vector::Set(&vBoundScale, vBoundLower.x, vBoundUpper.y, vBoundUpper.z);
		i3Vector::Sub(&vBoundScale, &vBoundScale, &vAxisWorldPos);
		
		if (!vBoundScale.isValid()) return;
		
		REAL32 fScale = i3Vector::Length(&vBoundScale) * 0.1f;

		//Local Space의 scale의 크기만 바꿔준다.
		{
			VEC3D * pAxis = i3Matrix::GetRight( pTrans->GetMatrix());

			pAxis->Normalize();
			*pAxis *= fScale;

			pAxis = i3Matrix::GetUp( pTrans->GetMatrix());
			pAxis->Normalize();
			*pAxis *= fScale;

			pAxis = i3Matrix::GetAt( pTrans->GetMatrix());
			pAxis->Normalize();
			*pAxis *= fScale;
		}
	}
}

void i3Viewer2Framework::SetLightRho(REAL32 fVal)
{
	m_LightRho = fVal; 
	UpdateLightTx(); 
}

void i3Viewer2Framework::SetLightTheta(REAL32 fVal)
{
	m_LightTheta = fVal; 
	UpdateLightTx(); 
}

void i3Viewer2Framework::SetLightAxisVisible(bool bVisible)
{
	if(m_bLightAxisVisible != bVisible)
	{
		m_pLightAxis->SetVisible(bVisible);
		m_bLightAxisVisible = bVisible;
	}
}
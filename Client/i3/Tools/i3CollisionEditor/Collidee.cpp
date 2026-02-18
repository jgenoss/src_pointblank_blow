#include "stdafx.h"
#include "Collidee.h"
#include "ColliderGroup.h"
#include "GlobalVar.h"

I3_CLASS_INSTANCE( Collidee, i3ElementBase);

Collidee::Collidee(void)
{
	m_State = 0;
	m_pColliderSet = NULL;
	m_pCollidee = NULL;
	m_pTransPosArray = NULL;

	m_pVisibleMesh = i3Node::NewObject();
	m_pVisibleMesh->AddFlag( I3_NODEFLAG_VOLATILE);

	//////////
	m_pTriGeoRoot = i3AttrSet::NewObjectRef();
	m_pTriGeoRoot->AddFlag( I3_NODEFLAG_VOLATILE);

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::NewObjectRef();
		pAttr->Set( TRUE);
		m_pTriGeoRoot->AddAttr( pAttr);
	}

	{
		i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::NewObjectRef();
		pAttr->Set( I3G_FACE_CULL_NONE);
		m_pTriGeoRoot->AddAttr( pAttr);
	}

	//////////
	m_pLineGeoRoot = i3AttrSet::NewObjectRef();
	m_pLineGeoRoot->AddFlag( I3_NODEFLAG_VOLATILE);

	m_pVisibleMesh->AddChild( m_pLineGeoRoot);
	m_pVisibleMesh->AddChild( m_pTriGeoRoot);
	
	{
		i3ZBiasAttr * pAttr = i3ZBiasAttr::NewObjectRef();

		pAttr->setBias( 0.102f);
		m_pLineGeoRoot->AddAttr( pAttr);
	}
}

Collidee::~Collidee(void)
{
	I3_SAFE_RELEASE( m_pTransPosArray);
	I3_SAFE_RELEASE( m_pColliderSet);
	I3_SAFE_RELEASE( m_pCollidee);
	I3_SAFE_RELEASE( m_pVisibleMesh);
}

void Collidee::Select(void)
{
	addState ( COLL_STATE_SELECTED);

}

void Collidee::Unselect(void)
{
	removeState( COLL_STATE_SELECTED);
}

inline bool isValidPos( VEC3D * pPos)
{
	if((pPos->x < -1.0f) || (pPos->x > 1.0f))
		return false;

	if((pPos->y < -1.0f) || (pPos->y > 1.0f))
		return false;

	return true;
}

bool Collidee::_IsTouchTri( I3_COLLIDEE_TRI32 * pTri, I3RECT * pRect, BOOL bBothSide, VEC3D * pNorm, REAL32 * pDist)
{
	INT32 j;
	VEC3D * pos[3];
	VEC3D rectPts[4];
	bool bValid[3];

	i3Vector::Set( &rectPts[0], pRect->left, pRect->top, 0.0f);
	i3Vector::Set( &rectPts[1], pRect->right, pRect->top, 0.0f);
	i3Vector::Set( &rectPts[2], pRect->left, pRect->bottom, 0.0f);
	i3Vector::Set( &rectPts[3], pRect->right, pRect->bottom, 0.0f);

	pos[0] = m_pTransPosArray->GetVector( pTri->m_Index[0]);
	bValid[0] = true; //isValidPos( pos[0]);

	pos[1] = m_pTransPosArray->GetVector( pTri->m_Index[1]);
	bValid[1] = true; // isValidPos( pos[1]);

	pos[2] = m_pTransPosArray->GetVector( pTri->m_Index[2]);
	bValid[2] = true; //isValidPos( pos[2]);

	if((bValid[0] == NULL) && (bValid[1] == NULL) && (bValid[2] == NULL))
		return false;

	// 삼각형의 3 꼭지점 중, 하나라도 Rect 안에 있다면 Hit...
	for( j = 0; j < 3; j++)
	{
		if((pos[j] != NULL) && ( i3Math::PtInRect( pRect, (VEC2D *) pos[j])))
		{
			*pDist = pos[j]->z;
			return true;
		}
	}

	// Rect의 각 꼭지점 중에 하나라도 Triangle 안에 있다면 hit
	for( j = 0; j < 4; j++)
	{
		if( i3Math::IsPtInTri( (VEC2D *) pos[0], (VEC2D *)pos[1], (VEC2D *)pos[2], (VEC2D *) &rectPts[j]))
		{
			{
				VEC3D s, e, inters;

				i3Vector::Copy( &s, &rectPts[j]);		i3Vector::SetZ( &s, 0.0f);
				i3Vector::Copy( &e, &rectPts[j]);		i3Vector::SetZ( &e, 1.0f);

				IntersectTriLine( &s, &e, pNorm, pos[0], &inters, pDist);

				//*pDist = pDist;
			}
			
			return true;
		}
	}

	// 삼각형의 각 변이, Rect와 교점이 있다면...
	if( i3Math::IsTouchLineRect( (VEC2D *) pos[0], (VEC2D *) pos[1], pRect))
	{
		*pDist = pos[0]->z;
		return true;
	}

	if( i3Math::IsTouchLineRect( (VEC2D *) pos[1], (VEC2D *) pos[2], pRect))
	{
		*pDist = pos[1]->z;
		return true;
	}

	if( i3Math::IsTouchLineRect( (VEC2D *) pos[2], (VEC2D *) pos[0], pRect))
	{
		*pDist = pos[2]->z;
		return true;
	}

	return false;
}

INT32	Collidee::GetTouchedTri( I3RECT * pRect, MATRIX * pViewProj, BOOL bBothSide, i3List * pList)
{
	MATRIX mtx;
	i3Vector3Array * pSrcVA;
	INT32 i, HitCount = 0;
	I3_COLLIDEE_TRI32 * pTri;
	bool bHit;

	if( m_pTransPosArray == NULL)
		return 0;

	if( m_pCollidee->IsTypeOf( i3CollideeMesh::GetClassMeta()) == FALSE)
		return 0;

	i3CollideeMesh * pMesh = (i3CollideeMesh *) m_pCollidee;
	pSrcVA = pMesh->GetPositionArray();

	// 모든 Vertex들을 Screen-Space로 Transform한다.
	i3Matrix::Mul( &mtx, &m_Matrix, pViewProj);

	i3Matrix::TransformPoints( m_pTransPosArray->GetVectorArray(), pSrcVA->GetVectorArray(), pSrcVA->GetVectorCount(), &mtx);

	// 검사를 2D로 변환하기 위해, Z 값을 0으로 일괄 설정한다.
	/*
	for( i = 0; i < (INT32) m_pTransPosArray->GetVectorCount(); i++)
	{
		i3Vector::SetZ( m_pTransPosArray->GetVector( i), 0.0f);
	}
	*/

	// Triangle을 하나씩 확인한다.
	for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
	{
		VEC3D norm;
		REAL32 dist;

		pTri = pMesh->GetTriangle( i);

		i3Vector::TransformNormal( &norm, pMesh->GetNormal( pTri->m_NormalIndex), &mtx);

		if( bBothSide == FALSE)
		{
			if( norm.z > 0)
				continue;
		}

		bHit = _IsTouchTri( pTri, pRect, bBothSide, &norm, &dist);

		if( bHit && (dist > 0.0f))
		{
			SELECT_TRI_INFO * pSelInfo = new SELECT_TRI_INFO;

			pSelInfo->m_pCollidee = this;
			pSelInfo->m_TriIndex = i;
			pSelInfo->m_Distance = dist;

			pList->Add( pSelInfo);

			HitCount ++;
		}
	}

	return HitCount;
}

void	Collidee::SelectTri( INT32 idx, BOOL bSelect)
{
	if( m_pCollidee->IsTypeOf( i3CollideeMesh::GetClassMeta()) == FALSE)
		return;

	i3CollideeMesh * pMesh = (i3CollideeMesh *) m_pCollidee;
	I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( idx);

	pTri->pad = (UINT8) bSelect;

	_UpdateAllTriColor();
}

void	Collidee::SelectAllTri(void)
{
	UINT32 i;
	i3CollideeMesh * pMesh;

	if( m_pCollidee->IsTypeOf( i3CollideeMesh::GetClassMeta()) == FALSE)
		return;

	pMesh = (i3CollideeMesh *) m_pCollidee;

	for( i = 0; i < pMesh->GetTriangleCount(); i++)
	{
		I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( i);

		pTri->pad = TRUE;
	}

	_UpdateAllTriColor();
}

void	Collidee::UnselectAllTri(void)
{
	UINT32 i;
	i3CollideeMesh * pMesh;

	if( m_pCollidee->IsTypeOf( i3CollideeMesh::GetClassMeta()) == FALSE)
		return;

	pMesh = (i3CollideeMesh *) m_pCollidee;

	for( i = 0; i < pMesh->GetTriangleCount(); i++)
	{
		I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( i);

		pTri->pad = FALSE;
	}

	_UpdateAllTriColor();
}

void Collidee::SetCollidee( i3ColliderSet * pCollSet, i3Collidee * pColl, MATRIX * pMatrix)
{
	if( m_pColliderSet != NULL)
	{
		m_pColliderSet->RemoveChild( m_pVisibleMesh);

		m_pTriGeoRoot->RemoveAllChild();
		m_pLineGeoRoot->RemoveAllChild();
	}

	I3_REF_CHANGE( m_pColliderSet, pCollSet);
	I3_REF_CHANGE( m_pCollidee, pColl);

	m_pColliderSet->AddChild( m_pVisibleMesh);

	i3Matrix::Copy( &m_Matrix, pMatrix);

	if( pColl == NULL)
		return;

	if( pColl->IsTypeOf( i3CollideeMesh::GetClassMeta()))
	{
		i3CollideeMesh * pMesh = (i3CollideeMesh *) pColl;

		m_pTransPosArray = i3Vector3Array::NewObject();
		m_pTransPosArray->SetVectorCount( pMesh->GetPositionCount());

		INT32 i;

		for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
		{
			I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( i);

			pTri->pad = 0;
		}
	}

	// Triangle Mesh
	{
		i3Node * pNode;

		pNode = i3SceneUtil::BuildCollideeMesh( pColl, TRUE, FALSE, FALSE, TRUE);
		
		m_pTriGeoRoot->AddChild( pNode);

		m_pTriGeoAttr = (i3GeometryAttr *) i3Scene::FindAttrByExactType( pNode, i3GeometryAttr::GetClassMeta());
		I3ASSERT( m_pTriGeoAttr != NULL);
	}

	// Line Mesh
	{
		i3Node * pNode;

		pNode = i3SceneUtil::BuildCollideeMesh( pColl, FALSE, TRUE, FALSE, TRUE);
		
		m_pLineGeoRoot->AddChild( pNode);

		m_pLineGeoAttr = (i3GeometryAttr *) i3Scene::FindAttrByExactType( pNode, i3GeometryAttr::GetClassMeta());
		I3ASSERT( m_pLineGeoAttr != NULL);
	}

	{
		_UpdateAllTriColor();
		_UpdateAllLineColor();
	}
		
}

void Collidee::_UpdateAllTriColor(void)
{
	INT32 i, idx;
	ColliderGroup * pGroup;
	i3VertexArray * pVA = m_pTriGeoAttr->GetVertexArray();
	COLOR selColor;

	i3Color::Set( &selColor, 255, 0, 0, 128);

	pVA->Lock();

	if( m_pCollidee->IsTypeOf( i3CollideeMesh::GetClassMeta()))
	{
		i3CollideeMesh * pMesh = (i3CollideeMesh *) m_pCollidee;

		// Triangle 정보를 하나씩 확인한 후, 실질적인 Group에 따라 색상을 조절해 준다.
		for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
		{
			I3_COLLIDEE_TRI32  * pTri = pMesh->GetTriangle( i);

			if( pTri->pad == 0)
			{
				idx = g_pProject->FindGroupByID( pTri->m_Terrain);
				if( idx == -1)
				{
					// 없던 Group이기 때문에 새로 추가.
					idx = g_pProject->AddGroup( pTri->m_Terrain);
				}

				pGroup = g_pProject->getGroup( idx);

				pVA->SetColor( ( i * 3), 3, pGroup->getColor());
			}
			else
			{
				pVA->SetColor( (i * 3), 3, &selColor);
			}
		}
	}
	else
	{
		COLOR col;

		i3Color::Set( &col, 64, 64, 64, 128);

		pVA->SetColor( 0, pVA->GetCount(), &col);
	}

	pVA->Unlock();
}

void Collidee::_UpdateAllLineColor(void)
{
}

void Collidee::Update(void)
{
	_UpdateAllTriColor();
}

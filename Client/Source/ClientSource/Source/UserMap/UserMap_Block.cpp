#include "pch.h"
#include "UserMap_Block.h"

I3_CLASS_INSTANCE( CUserMap_Block, i3ElementBase);

CUserMap_Block::CUserMap_Block(void)
{
}

CUserMap_Block::~CUserMap_Block(void)
{
	RemoveAll();
}

void CUserMap_Block::_Rec_Create( i3Node * pNode)
{
	INT32 i;

	if( pNode->IsExactTypeOf( i3Geometry::GetClassMeta()))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;
		i3RenderAttr * pAttr;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr(i);

			if( pAttr->IsExactTypeOf( i3GeometryAttr::GetClassMeta()))
			{
				CUserMap_Block_Geometry * pBlockGeo = CUserMap_Block_Geometry::NewObject();

				pBlockGeo->Create( (i3GeometryAttr *) pAttr);

				m_GeoList.Add( pBlockGeo);
			}
		}
	}

	i3Node * pChild;

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		_Rec_Create( pChild);
	}
}

void CUserMap_Block::Create( i3Node * pNode)
{
	_Rec_Create( pNode);
}

void CUserMap_Block::_ResizeGeometry( i3GeometryAttr * pGeoAttr, VEC3D * pScale, VEC3D * pVMin, VEC3D * pVMax)
{
	i3VertexArray * pVA, * pSrcVA;
	CUserMap_Block_Geometry * pBlockGeo;
	INT32 i;
	VEC3D vec;
	VEC2D uv;
	
	pBlockGeo = getBlockGeometry( m_idxApply);
	pSrcVA = pBlockGeo->getGeometry()->GetVertexArray();

	pVA = pGeoAttr->GetVertexArray();

	if( pVA == pSrcVA)
	{
		i3VertexFormat fmt;

		// CopyTo에 의해 복사되었지만, VertexArray를 아직 공유하고 있는 상황이다.
		// 새로운 VertexArray를 만들어 설정해 주어야 한다.

		fmt.SetHasPosition( true);
		fmt.SetHasNormal( true);
		fmt.SetTextureCoordSetCount( 1);
		fmt.SetTextureCoordCount( 2);

		pVA = i3VertexArray::NewObjectRef();
		pVA->Create( &fmt, pSrcVA->GetCount(), 0);

		pGeoAttr->SetVertexArray( pVA);
	}

	pVA->Lock();

	for( i = 0; i < pBlockGeo->getVtxCount(); i++)
	{
		BLOCKVTX * pVtx = pBlockGeo->getVtx( i);

		vec.x = pVtx->m_Pos.x * pScale->x;
		vec.y = pVtx->m_Pos.y * pScale->y;
		vec.z = pVtx->m_Pos.z * pScale->z;

		if( i == 0)
		{
			i3Vector::Copy( pVMin, &vec);
			i3Vector::Copy( pVMax, &vec);
		}
		else
		{
			i3Vector::Minimize( pVMin, pVMin, &vec);
			i3Vector::Maximize( pVMax, pVMax, &vec);
		}

		uv.x = pVtx->m_Tex.x * i3Vector::Dot( &pVtx->m_Tangent, pScale);
		uv.y = pVtx->m_Tex.y * i3Vector::Dot( &pVtx->m_Binormal, pScale);

		pVA->SetPosition( i, &vec);
		pVA->SetNormal( i, &pVtx->m_Normal);
		pVA->SetTextureCoord( 0, i, &uv);
	}

	pVA->Unlock();
}

void CUserMap_Block::_Rec_Resize( i3Node * pNode, VEC3D * pScale)
{
	INT32 i;

	if( pNode->IsExactTypeOf( i3Geometry::GetClassMeta()))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;
		VEC3D vmin, vmax, tempmin, tempmax;
		bool bFirst = true;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( i);

			if( pAttr->IsExactTypeOf( i3GeometryAttr::GetClassMeta()))
			{
				_ResizeGeometry( (i3GeometryAttr *) pAttr, pScale, &tempmin, &tempmax);

				if( bFirst)
				{
					i3Vector::Copy( &vmin, &tempmin);
					i3Vector::Copy( &vmax, &tempmax);
				}
				else
				{
					i3Vector::Minimize( &vmin, &vmin, &tempmin);
					i3Vector::Maximize( &vmax, &vmax, &tempmax);
				}

				m_idxApply++;
			}
		}

		if( pNode->GetBound() != NULL)
		{
			pNode->GetBound()->SetMinMax( &vmin, &vmax);
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		_Rec_Resize( pChild, pScale);
	}
}

void CUserMap_Block::Resize( i3Node * pNode, VEC3D * pScale)
{
	m_idxApply = 0;

	_Rec_Resize( pNode, pScale);
}

void CUserMap_Block::RemoveAll(void)
{
	INT32 i;
	CUserMap_Block_Geometry * pGeo;

	for( i = 0; i < m_GeoList.GetCount(); i++)
	{
		pGeo = getBlockGeometry( i);

		pGeo->Release();
	}

	m_GeoList.Clear();
}

#include "stdafx.h"
#include "I3SChecker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void I3SChecker::_CHK_Geometry( i3Geometry * pGeo)
{
	INT32 i;
	i3RenderAttr * pAttr;
	i3NormalMapEnableAttr * pNormalMapEnableAttr;
	i3LuxMapEnableAttr * pLuxMapEnableAttr;

	pNormalMapEnableAttr = (i3NormalMapEnableAttr *) i3Scene::FindLastAttr( pGeo, i3NormalMapEnableAttr::static_meta());
	pLuxMapEnableAttr = (i3LuxMapEnableAttr *) i3Scene::FindLastAttr( pGeo, i3LuxMapEnableAttr::static_meta());
	
	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = pGeo->GetGeometryAttr( i);

		if( i3::kind_of<i3GeometryAttr*>(pAttr)) //->IsTypeOf( i3GeometryAttr::static_meta()))
		{
			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;
			i3VertexArray * pVA = pGeoAttr->GetVertexArray();

			if( m_bCheckIndexedGeometry )
			{
				// Indexed Geometry
				if( pGeoAttr->GetIndexArray() == NULL)
					_AddLog( "Non-Indexed Geometry (%s)\n", pGeo->GetName());
			}

			if( m_bCheckTangentBasis)
			{
				// Tangent Basis
				if( (pNormalMapEnableAttr != NULL) && (pNormalMapEnableAttr->Get() == TRUE))
				{
					if( (pVA->GetFormat()->GetHasTangent() == FALSE) || (pVA->GetFormat()->GetHasBinormal() == FALSE))
						_AddLog( "Normal Map이 활성화되었으나, Tangent 및 Binormal Component가 없음 (%s)\n", pGeo->GetName());
				}
			}

			if( m_bCheckVertexBlend)
			{
				// Vertex Blend Bone 검사
				if( pVA->GetFormat()->GetBlendIndexCount() > 2)
					_AddLog( "Vertex Blend Bone의 개수가 2개 이상임. (%s)\n", pGeo->GetName());
			}

			if( m_bCheckBoundSphere)
			{
				// Bound Sphere
				if( pVA->isValidBoundSphere() == false)
				{
					_AddLog( "Bound Sphere가 올바르게 설정되어 있지 않음. Lighting이 제대로 되지 않을 수 있음. (%s)\n", pGeo->GetName());
				}
			}

			if( m_bCheckTexCoordSet)
			{
				// Texture 좌표 개수 검사
				if( pVA->GetFormat()->GetTextureCoordSetCount() > 2)
				{
					_AddLog( "Texture 좌표의 개수가 너무 많음. (%d 개) (%s)\n", pVA->GetFormat()->GetTextureCoordSetCount(), pGeo->GetName());
				}
				else
				{
					if( (pLuxMapEnableAttr != NULL) && (pLuxMapEnableAttr->Get() == TRUE))
					{
						if( pVA->GetFormat()->GetTextureCoordSetCount() != 2)
							_AddLog( "LuxMap을 사용하고 있지만 Texture 좌표의 개수가 2개가 아님. (%s)\n", pGeo->GetName());
					}
					else
					{
						if( pVA->GetFormat()->GetTextureCoordSetCount() > 1)
							_AddLog( "Texture 좌표의 개수가 너무 많음. (%d 개) (%s)\n", pVA->GetFormat()->GetTextureCoordSetCount(), pGeo->GetName());
					}
				}
			}

			if( m_bCheckVertexColor)
			{
				// Vertex Color 확인
				if( pVA->GetFormat()->GetHasColor())
				{
					_AddLog( "Vertex Color가 있음. 불필요한 것일 수 있음.(%s)\n", pGeo->GetName());
				}
			}

			{
				i3IndexArray * pIA = pGeoAttr->GetIndexArray();
				INT32 primCount, primCountGeo;

				if( pIA != NULL)
					primCount = pIA->GetCount();
				else
					primCount = pVA->GetCount();

				primCount = i3Gfx::CalcPrimCount( pGeoAttr->GetPrimitiveType(), primCount);

				if( pGeoAttr->GetPrimitiveLengthArray() != NULL)
				{
					primCountGeo = 0;

					for( UINT32 k = 0; k < pGeoAttr->GetPrimitiveCount(); k++)
					{
						primCountGeo += pGeoAttr->GetPrimitiveLength( k);
					}
				}
				else
				{
					primCountGeo = pGeoAttr->GetPrimitiveCount();
				}

				if( primCount != primCountGeo)
				{
					if( pIA != NULL)
					{
						_AddLog( "IndexArray의 개수(%d)가 지정된 Primitive Count(%d)와 부합하지 않음. (%s)\n", primCount, primCountGeo, pGeo->GetName());
					}
					else
					{
						_AddLog( "VertexArray의 개수(%d)가 지정된 Primitive Count(%d)와 부합하지 않음. (%s)\n", primCount, primCountGeo, pGeo->GetName());
					}
				}
			}
		}
	}
}

void I3SChecker::_CHK_BoundBox(void)
{
	if( m_BBoxCount == 0)
	{
		_AddLog( "BoundBox가 전혀 발견되지 않음. Optimize가 안된 놈?\n");
	}
}

void I3SChecker::_CHK_Texture( i3Texture * pTex)
{
	if( m_bCheckTextureFormat == false)
		return;

	if( i3Gfx::GetImageFormatBitCount( pTex->GetFormat()) > 8)
	{
		_AddLog( "Texture Format이 압축되지 않았음. (%s)\n", pTex->GetName());
	}
}

void I3SChecker::_CHK_AttrSet( i3AttrSet * pAttrSet)
{
	INT32 i;
	i3RenderAttr * pAttr;

	for( i = 0; i < pAttrSet->GetAttrCount(); i++)
	{
		pAttr = pAttrSet->GetAttr( i);

		if( i3::kind_of<i3TextureBindAttr*>(pAttr)) //->IsTypeOf( i3TextureBindAttr::static_meta()))
		{
			i3TextureBindAttr * pBindAttr = (i3TextureBindAttr *) pAttr;

			if( pBindAttr->GetTexture() != NULL)
			{
				_CHK_Texture( pBindAttr->GetTexture());
			}
		}
	}
}

void I3SChecker::_CHK_StaticTransform( i3Transform * pTrans)
{
	if( m_bCheckStaticTransform == false)
		return;

	if( pTrans->GetFlag() & I3_NODEFLAG_STATIC)
	{
		_AddLog( "불필요한 Transform이 남아 있음. (%s)\n", pTrans->GetName());
	}
}

void I3SChecker::_CHK_Body( i3Body * pBody)
{
	i3LOD * pLOD = pBody->getFrameLOD();

	if( pLOD != NULL)
	{
		i3Skeleton * pSkel = pLOD->getSkeleton();
		INT32 i;

		if( pSkel != NULL)
		{
			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				if( pSkel->isShapeSet( i))
				{
					_CHK_PhysixShapeSet( pSkel->getShapeSet( i));
				}
			}
		}
	}
}

void I3SChecker::_CHK_PhysixShapeSet( i3PhysixShapeSet * pShapeSet)
{
	NxActor * pActor = pShapeSet->getNxActor();

	if( pActor == NULL)
	{
		_AddLog( "Actor가 없은 ShapeSet이 있음. (%s)\n", pShapeSet->GetName());
		return;
	}

	if( pActor->isDynamic() && (pShapeSet->getShapeType() == I3_PHYSIX_SHAPE_TRIMESH))
	{
		_AddLog( "Dynamic Triangle Mesh가 있음. (%s)\n", pShapeSet->GetName());
	}

	if( pShapeSet->getFormat() == i3PhysixShapeSet::FORMAT_XML)
	{
		_AddLog( "PhysX Data가 XML 형식으로 저장되어 있음. (%s)\n", pShapeSet->GetName());
	}
}

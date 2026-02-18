#include "i3OptPCH.h"
#include "i3OptBuildTangentBasis.h"
#include "i3OptBuildIndexedGeometry.h"

I3_CLASS_INSTANCE( i3OptBuildTangentBasis);

i3OptBuildTangentBasis::i3OptBuildTangentBasis(void)
{
	m_Class = CLASS_GEOMETRY;
}


void i3OptBuildTangentBasis::_BuildIndexedTriList( i3GeometryAttr * pGeoAttr)
{
	INT32 i, idx1, idx2, idx3, faceCount, j;
	VEC3D pos1, pos2, pos3, temp;
	VEC2D uv1, uv2, uv3;
	i3VertexArray * pSrcVA = pGeoAttr->GetVertexArray();
	i3VertexArray * pDestVA;
	i3Vector3Array	faceTangents;
	i3Vector3Array	faceBinormals;
	i3IndexArray * pIA = pGeoAttr->GetIndexArray();

	I3ASSERT( pIA != nullptr);

	if( pSrcVA->GetCount() <= 0)
		return;

	// Texture 좌표가 없다면, Tangent와 Binormal Vector를 생성할 수 없다.
	if( pSrcVA->GetFormat()->GetTextureCoordSetCount() <= 0)
		return;

	{
		i3VertexFormat fmt;

		fmt = * pSrcVA->GetFormat();

		fmt.SetHasTangent(true);
		fmt.SetHasBinormal(true);

		pDestVA = i3VertexArray::new_object_ref();
		pDestVA->Create( &fmt, pSrcVA->GetCount(), pSrcVA->GetUsageFlag());

		// 원본 데이터를 복사한다.
		pSrcVA->CopyTo( pDestVA, I3_COPY_INSTANCE);
	}

	pDestVA->Lock( 0);
	pIA->Lock();

	faceCount = pIA->GetCount() / 3;
	faceTangents.SetVectorCount( faceCount);
	faceBinormals.SetVectorCount( faceCount);

	I3ASSERT( (pIA->GetCount() % 3) == 0);

	for( i = 0; i < (INT32) pIA->GetCount(); i += 3)
	{
		idx1 = pIA->GetIndex( i);
		idx2 = pIA->GetIndex( i + 1);
		idx3 = pIA->GetIndex( i + 2);

		I3ASSERT( idx1 < (INT32) pDestVA->GetCount());
		I3ASSERT( idx2 < (INT32) pDestVA->GetCount());
		I3ASSERT( idx3 < (INT32) pDestVA->GetCount());

		pDestVA->GetPosition( idx1, &pos1);
		pDestVA->GetTextureCoord( 0, idx1, &uv1);

		pDestVA->GetPosition( idx2, &pos2);
		pDestVA->GetTextureCoord( 0, idx2, &uv2);

		pDestVA->GetPosition( idx3, &pos3);
		pDestVA->GetTextureCoord( 0, idx3, &uv3);

		VEC3D faceT, faceB;

		i3Math::CalcTangentBasis( &pos1, &pos2, &pos3, &uv1, &uv2, &uv3, &faceT, &faceB);

		I3ASSERT( (i / 3) < (INT32) faceTangents.GetVectorCount());

		faceTangents.SetVector( i / 3, &faceT);
		faceBinormals.SetVector( i / 3, &faceB);
	}

	// 각 Vertex의 Tangent와 Binormal을 Zero Vector로 Clear
	{
		VEC3D zero;

		i3Vector::Set( &zero, 0.0f, 0.0f, 0.0f);

		for( i = 0; i < (INT32) pDestVA->GetCount(); i++)
		{
			pDestVA->SetTangent( i, &zero);
			pDestVA->SetBinormal( i, &zero);
		}
	}

	// set tangent spaces to be area-weighted averages of all adjacent spaces
	for( i = 0; i < faceCount; i++)
	{
		for( j = 0; j < 3; j++)
		{
			idx1 = pIA->GetIndex( (i * 3) + j);

			pDestVA->GetTangent( idx1, &temp);
			i3Vector::Add( &temp, &temp, faceTangents.GetVector(i));
			pDestVA->SetTangent( idx1, &temp);

			pDestVA->GetBinormal( idx1, &temp);
			i3Vector::Add( &temp, &temp, faceBinormals.GetVector(i));
			pDestVA->SetBinormal( idx1, &temp);
		}
	}

	// re-normalize and orthogonalize the tangent space
	for( i = 0; i < (INT32) pDestVA->GetCount(); i++)
	{
		VEC3D n, b, t;

		pDestVA->GetNormal( i, &n);
		i3Vector::Normalize( &n, &n);
		pDestVA->SetNormal( i, &n);

		pDestVA->GetBinormal( i, &b);
		i3Vector::Copy( &temp, &b);
		i3Vector::Cross( &t, &n, &b);		// Tangent
		i3Vector::Cross( &b, &n, &t);
		i3Vector::Sub( &b, &temp, &b);
		i3Vector::Normalize( &b, &b);
		pDestVA->SetBinormal( i, &b);

		pDestVA->GetTangent( i, &t);
		i3Vector::Copy( &temp, &t);
		i3Vector::Cross( &b, &n, &t);		// Tangent
		i3Vector::Cross( &t, &n, &b);
		i3Vector::Sub( &t, &temp, &t);
		i3Vector::Normalize( &t, &t);
		pDestVA->SetTangent( i, &t);
	}

	pDestVA->Unlock();
	pIA->Unlock();

	pDestVA->SetPersistData(nullptr);

	// 새로운 Vertex Array로 변경한다.
	pGeoAttr->SetVertexArray( pDestVA);


}

void i3OptBuildTangentBasis::_BuildIndexedTriStrip( i3GeometryAttr * pGeoAttr)
{
	I3ASSERT_0;
}

void i3OptBuildTangentBasis::Build( i3GeometryAttr * pGeoAttr)
{
	// 올바른 Converting을 위해서는 반드시 Indexed-Geometry가 되어야 한다.
	//
	if( pGeoAttr->GetIndexArray() == nullptr)
	{
		i3OptBuildIndexedGeometry opt;

		opt.ConvertVertex2Indexed( pGeoAttr);
	}

	switch( pGeoAttr->GetPrimitiveType())
	{
		case I3G_PRIM_TRILIST :		_BuildIndexedTriList( pGeoAttr);	break;
		default :					_BuildIndexedTriStrip( pGeoAttr);	break;
	}
}

bool i3OptBuildTangentBasis::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Geometry*>(pNode))
	{
		INT32 i;
		i3Geometry * pGeo = (i3Geometry *) pNode;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( i);

			if( i3::same_of<i3GeometryAttr* >(pAttr))
			{
				m_GeoAttrList.push_back(static_cast<i3GeometryAttr*>(pAttr));
			}
		}
	}

	return true;
}

void i3OptBuildTangentBasis::Trace( i3Node * pRoot)
{
//	m_GeoAttrList.SetOnceMode(true);		// 이 뒤로 Add가 없어보여, 효과가 없는 것으로 보여 우선 주석처리한다...

	i3SceneOptimizer::Trace( pRoot);

	for( size_t i = 0; i < m_GeoAttrList.size(); i++)
	{
		Build( m_GeoAttrList[i]);
	}
}

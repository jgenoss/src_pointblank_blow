#include "i3OptPCH.h"
#include "i3OptBuildCollisionMesh.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3OptBuildCollisionMesh);

i3OptBuildCollisionMesh::i3OptBuildCollisionMesh(void)
{
	m_Class = CLASS_PHYSICS;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;

	m_pStack = i3MatrixStack::new_object();
}

i3OptBuildCollisionMesh::~i3OptBuildCollisionMesh(void)
{
	I3_SAFE_RELEASE( m_pStack);
}

INT32 i3OptBuildCollisionMesh::FindPos( i3Vector3Array * pArray, INT32 Count, VEC3D * pPos)
{
	INT32 i;

	for( i = 0; i < (INT32) Count; i++)
	{
		if( (i3Vector::GetX( pArray->GetVector( i)) == i3Vector::GetX( pPos)) &&
			(i3Vector::GetY( pArray->GetVector( i)) == i3Vector::GetY( pPos)) &&
			(i3Vector::GetZ( pArray->GetVector( i)) == i3Vector::GetZ( pPos)))
		{
			return i;
		}
	}

	return -1;
}

void i3OptBuildCollisionMesh::OptimizePosition( i3CollideeMesh * pMesh, i3Vector3Array * pArray, INT32 Count)
{
	INT32 i;
	i3Vector3Array * pNew = i3Vector3Array::new_object_ref();

	pNew->SetVectorCount( Count);

	for( i = 0; i < Count; i++)
	{
		pNew->SetVector( i, pArray->GetVector( i));
	}

	pMesh->SetPositionArray( pNew);
}

void i3OptBuildCollisionMesh::OptimizeNormal( i3CollideeMesh * pMesh, i3Vector3Array * pArray, INT32 Count)
{
	INT32 i;
	i3Vector3Array * pNew = i3Vector3Array::new_object_ref();

	pNew->SetVectorCount( Count);

	for( i = 0; i < Count; i++)
	{
		pNew->SetVector( i, pArray->GetVector( i));
	}

	pMesh->SetNormalArray( pNew);
}

i3CollideeMesh * i3OptBuildCollisionMesh::ConvertIndexTriList( i3IndexArray * pIA, i3VertexArray * pVA, INT32 * pIndexTable)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3, normidx, sidx1, sidx2, sidx3;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray;
	i3Vector3Array * pNormalArray;
	i3CollideeMesh * pMesh = nullptr;
	I3_COLLIDEE_TRI32 * pTri;

	if( pIA->GetCount() <= 0)
		return nullptr;

	if( pVA->GetCount() <= 0)
		return nullptr;

	pIA->Lock();
	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = pIA->GetCount() / 3;

	pTri = new I3_COLLIDEE_TRI32[ TriCount];
	
	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3);

	pNormalArray = i3Vector3Array::new_object();
	pNormalArray->SetVectorCount( TriCount);


	for( i = 0, TriIndex = 0; i < (INT32) pIA->GetCount(); i += 3)
	{
		sidx1 = pIA->GetIndex( i);
		sidx2 = pIA->GetIndex( i + 1);
		sidx3 = pIA->GetIndex( i + 2);

		pVA->GetPosition( sidx1, &pos1);
		pVA->GetPosition( sidx2, &pos2);
		pVA->GetPosition( sidx3, &pos3);

		if( pVA->GetFormat()->GetHasColor())
		{
			COLORREAL col;

			pVA->GetColor( sidx1, &col);
			i3Color::Set( &m_LastColor, &col);
		}

		if( m_bCollapseTransform == true)
		{
			i3Vector::TransformCoord( &pos1, &pos1, m_pStack->GetTop());
			i3Vector::TransformCoord( &pos2, &pos2, m_pStack->GetTop());
			i3Vector::TransformCoord( &pos3, &pos3, m_pStack->GetTop());
		}

		i3Vector::Normal( &norm, &pos1, &pos2, &pos3);
		if( i3Vector::Length( &norm ) < 0.00001f)
			continue;

		// Position 1 Index
		idx1 = FindPos( pArray, PosCount, &pos1);
		if( idx1 == -1)
		{
			pArray->SetVector( PosCount, &pos1);
			idx1 = PosCount;
			PosCount++;
		}

		// Position 2 Index
		idx2 = FindPos( pArray, PosCount, &pos2);
		if( idx2 == -1)
		{
			pArray->SetVector( PosCount, &pos2);
			idx2 = PosCount;
			PosCount++;
		}

		// Position 3 Index
		idx3 = FindPos( pArray, PosCount, &pos3);
		if( idx3 == -1)
		{
			pArray->SetVector( PosCount, &pos3);
			idx3 = PosCount;
			PosCount++;
		}

		if( pIndexTable != nullptr)
		{
			pIndexTable[ sidx1] = idx1;
			pIndexTable[ sidx2] = idx2;
			pIndexTable[ sidx3] = idx3;
		}

		// Normal
		{
			normidx = FindPos( pNormalArray, NormCount, &norm);
			if( normidx == -1)
			{
				pNormalArray->SetVector( NormCount, &norm);
				normidx = NormCount;
				NormCount++;
			}
		}	

		I3ASSERT( (idx1 >= 0) && (idx1 < 65536));
		I3ASSERT( (idx2 >= 0) && (idx2 < 65536));
		I3ASSERT( (idx3 >= 0) && (idx3 < 65536));
		I3ASSERT( (normidx >= 0) && (normidx < 65536));

		// Triangle Indices
		pTri[ TriIndex].m_Index[0] = (UINT16) idx1;
		pTri[ TriIndex].m_Index[1] = (UINT16) idx2;
		pTri[ TriIndex].m_Index[2] = (UINT16) idx3;
		pTri[ TriIndex].m_NormalIndex = (UINT16) normidx;
		pTri[ TriIndex].m_Color[0] = (UINT8) i3Color::GetR( &m_LastColor);
		pTri[ TriIndex].m_Color[1] = (UINT8) i3Color::GetG( &m_LastColor);
		pTri[ TriIndex].m_Color[2] = (UINT8) i3Color::GetB( &m_LastColor);
		pTri[ TriIndex].m_Color[3] = (UINT8) i3Color::GetA( &m_LastColor);
		pTri[ TriIndex].m_Terrain	= (UINT8) m_TerrainType;

		if( m_pLastReflectMap != nullptr)
			pTri[ TriIndex].m_ReflectMapIndex = 0;
		else
			pTri[ TriIndex].m_ReflectMapIndex = -1;

		TriIndex++;
	}

	pIA->Unlock();
	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3CollideeMesh::new_object_ref();

		pMesh->SetTriangleCount( TriIndex);

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		OptimizePosition( pMesh, pArray, PosCount);

		// 개수를 맞춘 Normal Array를 새롭게 작성한다.
		OptimizeNormal( pMesh, pNormalArray, NormCount);

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[i]);
		}
	}

	I3_MUST_RELEASE(pArray);
	I3_MUST_RELEASE(pNormalArray);
	delete [] pTri;

	return pMesh;
}

i3CollideeMesh * i3OptBuildCollisionMesh::ConvertTriList( i3VertexArray * pVA, INT32 * pIndexTable)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3, normidx;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray;
	i3Vector3Array * pNormalArray;
	i3CollideeMesh * pMesh = nullptr;
	I3_COLLIDEE_TRI32 * pTri;

	if( pVA->GetCount() <= 0)
		return nullptr;

	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = pVA->GetCount() / 3;

	pTri = new I3_COLLIDEE_TRI32[TriCount];
	I3ASSERT( pTri != nullptr);
	
	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3);

	pNormalArray = i3Vector3Array::new_object();
	pNormalArray->SetVectorCount( TriCount);

	for( i = 0, TriIndex = 0; i < (INT32) pVA->GetCount(); i += 3)
	{
		pVA->GetPosition( i, &pos1);
		pVA->GetPosition( i + 1, &pos2);
		pVA->GetPosition( i + 2, &pos3);

		if( m_bCollapseTransform == true)
		{
			i3Vector::TransformCoord( &pos1, &pos1, m_pStack->GetTop());
			i3Vector::TransformCoord( &pos2, &pos2, m_pStack->GetTop());
			i3Vector::TransformCoord( &pos3, &pos3, m_pStack->GetTop());
		}

		if( pVA->GetFormat()->GetHasColor())
		{
			COLORREAL col;

			pVA->GetColor( i, &col);
			i3Color::Set( &m_LastColor, &col);
		}

		i3Vector::Normal( &norm, &pos1, &pos2, &pos3);

		if( i3Vector::Length( &norm ) < 0.00001f)
		{
			I3TRACE( "Empty Triangle\n");
			continue;
		}

		// Position 1 Index
		idx1 = FindPos( pArray, PosCount, &pos1);
		if( idx1 == -1)
		{
			pArray->SetVector( PosCount, &pos1);
			idx1 = PosCount;
			PosCount++;
		}

		// Position 2 Index
		idx2 = FindPos( pArray, PosCount, &pos2);
		if( idx2 == -1)
		{
			pArray->SetVector( PosCount, &pos2);
			idx2 = PosCount;
			PosCount++;
		}

		// Position 3 Index
		idx3 = FindPos( pArray, PosCount, &pos3);
		if( idx3 == -1)
		{
			pArray->SetVector( PosCount, &pos3);
			idx3 = PosCount;
			PosCount++;
		}

		if( pIndexTable != nullptr)
		{
			pIndexTable[     i] = idx1;
			pIndexTable[ i + 1] = idx2;
			pIndexTable[ i + 2] = idx3;
		}

		// Normal
		{
			normidx = FindPos( pNormalArray, NormCount, &norm);
			if( normidx == -1)
			{
				pNormalArray->SetVector( NormCount, &norm);
				normidx = NormCount;
				NormCount++;
			}
		}

		I3ASSERT( (idx1 >= 0) && (idx1 < 65536));
		I3ASSERT( (idx2 >= 0) && (idx2 < 65536));
		I3ASSERT( (idx3 >= 0) && (idx3 < 65536));
		I3ASSERT( (normidx >= 0) && (normidx < 65536));

		// Triangle Indices
		pTri[ TriIndex].m_Index[0] = (UINT16) idx1;
		pTri[ TriIndex].m_Index[1] = (UINT16) idx2;
		pTri[ TriIndex].m_Index[2] = (UINT16) idx3;
		pTri[ TriIndex].m_NormalIndex = (UINT16) normidx;
		pTri[ TriIndex].m_Color[0] = (UINT8) i3Color::GetR( &m_LastColor);
		pTri[ TriIndex].m_Color[1] = (UINT8) i3Color::GetG( &m_LastColor);
		pTri[ TriIndex].m_Color[2] = (UINT8) i3Color::GetB( &m_LastColor);
		pTri[ TriIndex].m_Color[3] = (UINT8) i3Color::GetA( &m_LastColor);
		pTri[ TriIndex].m_Terrain	= (UINT8) m_TerrainType;

		if( m_pLastReflectMap != nullptr)
			pTri[ TriIndex].m_ReflectMapIndex = 0;
		else
			pTri[ TriIndex].m_ReflectMapIndex = -1;

		//i3Color::Dump( "", &m_LastColor);

		TriIndex++;
	}


	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3CollideeMesh::new_object_ref();

		pMesh->SetTriangleCount( TriIndex);

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		{
			OptimizePosition( pMesh, pArray, PosCount);
		}

		// 개수를 맞춘 Normal Array를 새롭게 작성한다.
		{
			OptimizeNormal( pMesh, pNormalArray, NormCount);
		}

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[i]);
		}
	}

	I3_MUST_RELEASE(pArray);
	I3_MUST_RELEASE(pNormalArray);
	delete [] pTri;

	return pMesh;
}

i3CollideeMesh * i3OptBuildCollisionMesh::ConvertIndexTriStrip( i3GeometryAttr * pGeoAttr, INT32 * pIndexTable)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3, normidx;
	INT32 vidx1, vidx2, vidx3;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray;
	i3Vector3Array * pNormalArray;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	INT32 j, accm, PrimLen;
	i3CollideeMesh * pMesh = nullptr;
	I3_COLLIDEE_TRI32 * pTri;

	pIA = pGeoAttr->GetIndexArray();
	pVA = pGeoAttr->GetVertexArray();

	if( pVA->GetCount() <= 0)
		return nullptr;

	pIA->Lock();
	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = 0;

	if( pGeoAttr->GetPrimitiveCount() == 1)
	{
		TriCount = pIA->GetCount() - 2;
	}
	else
	{
		for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
		{
			TriCount += pGeoAttr->GetPrimitiveLength( i);
		}
	}
	
	pTri = new I3_COLLIDEE_TRI32[ TriCount];
	
	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3);

	pNormalArray = i3Vector3Array::new_object();
	pNormalArray->SetVectorCount( TriCount);

	accm = 0;
	TriIndex = 0;

	for( j = 0; j < (INT32) pGeoAttr->GetPrimitiveCount(); j++)
	{
		vidx1 = pIA->GetIndex( accm);		accm++;
		vidx2 = pIA->GetIndex( accm);		accm++;
		vidx3 = pIA->GetIndex( accm);		accm++;

		if( pGeoAttr->GetPrimitiveCount() == 1)
		{
			PrimLen = pIA->GetCount() - 2;
		}
		else
		{
			PrimLen = pGeoAttr->GetPrimitiveLength( j);
		}

		for( i = 0; i < (INT32) PrimLen; i++)
		{
			pVA->GetPosition( vidx1, &pos1);
			pVA->GetPosition( vidx2, &pos2);
			pVA->GetPosition( vidx3, &pos3);

			if( pVA->GetFormat()->GetHasColor())
			{
				COLORREAL col;

				pVA->GetColor( vidx1, &col);
				i3Color::Set( &m_LastColor, &col);
			}

			if( m_bCollapseTransform == true)
			{
				i3Vector::TransformCoord( &pos1, &pos1, m_pStack->GetTop());
				i3Vector::TransformCoord( &pos2, &pos2, m_pStack->GetTop());
				i3Vector::TransformCoord( &pos3, &pos3, m_pStack->GetTop());
			}

			i3Vector::Normal( &norm, &pos1, &pos2, &pos3);
			if( i3Vector::Length( &norm ) < 0.00001f)
				continue;

			// Position 1 Index
			idx1 = FindPos( pArray, PosCount, &pos1);
			if( idx1 == -1)
			{
				pArray->SetVector( PosCount, &pos1);
				idx1 = PosCount;
				PosCount++;
			}

			// Position 2 Index
			idx2 = FindPos( pArray, PosCount, &pos2);
			if( idx2 == -1)
			{
				pArray->SetVector( PosCount, &pos2);
				idx2 = PosCount;
				PosCount++;
			}

			// Position 3 Index
			idx3 = FindPos( pArray, PosCount, &pos3);
			if( idx3 == -1)
			{
				pArray->SetVector( PosCount, &pos3);
				idx3 = PosCount;
				PosCount++;
			}

			if( pIndexTable != nullptr)
			{
				pIndexTable[ vidx1] = idx1;
				pIndexTable[ vidx2] = idx2;
				pIndexTable[ vidx3] = idx3;
			}

			// Normal
			{
				normidx = FindPos( pNormalArray, NormCount, &norm);
				if( normidx == -1)
				{
					pNormalArray->SetVector( NormCount, &norm);
					normidx = NormCount;
					NormCount++;
				}
			}	

			// Triangle Indices
			I3ASSERT( (idx1 >= 0) && (idx1 < 65536));
			I3ASSERT( (idx2 >= 0) && (idx2 < 65536));
			I3ASSERT( (idx3 >= 0) && (idx3 < 65536));
			I3ASSERT( (normidx >= 0) && (normidx < 65536));

			pTri[ TriIndex].m_Index[0] = (UINT16) idx1;
			pTri[ TriIndex].m_Index[1] = (UINT16) idx2;
			pTri[ TriIndex].m_Index[2] = (UINT16) idx3;
			pTri[ TriIndex].m_NormalIndex = (UINT16) normidx;
			pTri[ TriIndex].m_Color[0] = (UINT8) i3Color::GetR( &m_LastColor);
			pTri[ TriIndex].m_Color[1] = (UINT8) i3Color::GetG( &m_LastColor);
			pTri[ TriIndex].m_Color[2] = (UINT8) i3Color::GetB( &m_LastColor);
			pTri[ TriIndex].m_Color[3] = (UINT8) i3Color::GetA( &m_LastColor);
			pTri[ TriIndex].m_Terrain	= (UINT8) m_TerrainType;

			if( m_pLastReflectMap != nullptr)
				pTri[ TriIndex].m_ReflectMapIndex = 0;
			else
				pTri[ TriIndex].m_ReflectMapIndex = -1;

			// Swap Index
			if( (i & 1))
			{
				// 홍수
				vidx1 = vidx3;
				vidx3 = pIA->GetIndex( accm);		accm++;
			}
			else
			{
				// 짝수
				vidx1 = vidx2;
				vidx2 = pIA->GetIndex( accm);		accm++;
			}
			TriIndex++;
		}
	}

	pIA->Unlock();
	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3CollideeMesh::new_object_ref();

		pMesh->SetTriangleCount( TriIndex);

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		{
			OptimizePosition( pMesh, pArray, PosCount);
		}

		// 개수를 맞춘 Normal Array를 새롭게 작성한다.
		{
			OptimizeNormal( pMesh, pNormalArray, NormCount);
		}

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[ i]);
		}
	}

	I3_MUST_RELEASE(pArray);
	I3_MUST_RELEASE(pNormalArray);
	delete [] pTri;

	return pMesh;
}

i3CollideeMesh *i3OptBuildCollisionMesh::ConvertTriStrip( i3GeometryAttr * pGeoAttr, INT32 * pIndexTable)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3, normidx;
	INT32 vidx1, vidx2, vidx3;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray;
	i3Vector3Array * pNormalArray;
	i3VertexArray * pVA;
	INT32 j, accm, PrimLen;
	i3CollideeMesh * pMesh = nullptr;
	I3_COLLIDEE_TRI32 * pTri;

	pVA = pGeoAttr->GetVertexArray();

	if( pVA->GetCount() <= 0)
		return nullptr;

	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = 0;

	if( pGeoAttr->GetPrimitiveCount() == 1)
	{
		TriCount = pVA->GetCount() - 2;
	}
	else
	{
		for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
		{
			TriCount += pGeoAttr->GetPrimitiveLength( i);
		}
	}

	pTri = new I3_COLLIDEE_TRI32[ TriCount];
	I3ASSERT( pTri != nullptr);
	
	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3);

	pNormalArray = i3Vector3Array::new_object();
	pNormalArray->SetVectorCount( TriCount);

	accm = 0;
	TriIndex = 0;

	for( j = 0; j < (INT32) pGeoAttr->GetPrimitiveCount(); j++)
	{
		vidx1 = accm;		accm++;
		vidx2 = accm;		accm++;
		vidx3 = accm;		accm++;

		if( pGeoAttr->GetPrimitiveCount() == 1)
		{
			PrimLen = pVA->GetCount() - 2;
		}
		else
		{
			PrimLen = pGeoAttr->GetPrimitiveLength( j);
		}

		for( i = 0; i < (INT32) PrimLen; i++)
		{
			pVA->GetPosition( vidx1, &pos1);
			pVA->GetPosition( vidx2, &pos2);
			pVA->GetPosition( vidx3, &pos3);

			if( pVA->GetFormat()->GetHasColor())
			{
				COLORREAL col;

				pVA->GetColor( vidx1, &col);
				i3Color::Set( &m_LastColor, &col);
			}

			if( m_bCollapseTransform == true)
			{
				i3Vector::TransformCoord( &pos1, &pos1, m_pStack->GetTop());
				i3Vector::TransformCoord( &pos2, &pos2, m_pStack->GetTop());
				i3Vector::TransformCoord( &pos3, &pos3, m_pStack->GetTop());
			}

			i3Vector::Normal( &norm, &pos1, &pos2, &pos3);
			if( i3Vector::Length( &norm ) < 0.00001f)
				continue;

			// Position 1 Index
			idx1 = FindPos( pArray, PosCount, &pos1);
			if( idx1 == -1)
			{
				pArray->SetVector( PosCount, &pos1);
				idx1 = PosCount;
				PosCount++;
			}

			// Position 2 Index
			idx2 = FindPos( pArray, PosCount, &pos2);
			if( idx2 == -1)
			{
				pArray->SetVector( PosCount, &pos2);
				idx2 = PosCount;
				PosCount++;
			}

			// Position 3 Index
			idx3 = FindPos( pArray, PosCount, &pos3);
			if( idx3 == -1)
			{
				pArray->SetVector( PosCount, &pos3);
				idx3 = PosCount;
				PosCount++;
			}

			if( pIndexTable != nullptr)
			{
				pIndexTable[ vidx1] = idx1;
				pIndexTable[ vidx2] = idx2;
				pIndexTable[ vidx3] = idx3;
			}

			// Normal
			{
				normidx = FindPos( pNormalArray, NormCount, &norm);
				if( normidx == -1)
				{
					pNormalArray->SetVector( NormCount, &norm);
					normidx = NormCount;
					NormCount++;
				}
			}	

			I3ASSERT( (idx1 >= 0) && (idx1 < 65536));
			I3ASSERT( (idx2 >= 0) && (idx2 < 65536));
			I3ASSERT( (idx3 >= 0) && (idx3 < 65536));
			I3ASSERT( (normidx >= 0) && (normidx < 65536));

			// Triangle Indices
			pTri[ TriIndex].m_Index[0] = (UINT16) idx1;
			pTri[ TriIndex].m_Index[1] = (UINT16) idx2;
			pTri[ TriIndex].m_Index[2] = (UINT16) idx3;
			pTri[ TriIndex].m_NormalIndex = (UINT16) normidx;
			pTri[ TriIndex].m_Color[0] = (UINT8) i3Color::GetR( &m_LastColor);
			pTri[ TriIndex].m_Color[1] = (UINT8) i3Color::GetG( &m_LastColor);
			pTri[ TriIndex].m_Color[2] = (UINT8) i3Color::GetB( &m_LastColor);
			pTri[ TriIndex].m_Color[3] = (UINT8) i3Color::GetA( &m_LastColor);
			pTri[ TriIndex].m_Terrain	= (UINT8) m_TerrainType;

			if( m_pLastReflectMap != nullptr)
				pTri[ TriIndex].m_ReflectMapIndex = 0;
			else
				pTri[ TriIndex].m_ReflectMapIndex = -1;

			// Swap Index
			if( (i & 1))
			{
				// 홍수
				vidx1 = vidx3;
				vidx3 = accm;		accm++;
			}
			else
			{
				// 짝수
				vidx1 = vidx2;
				vidx2 = accm;		accm++;
			}
			TriIndex++;
		}
	}

	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3CollideeMesh::new_object_ref();

		pMesh->SetTriangleCount( TriIndex);

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		OptimizePosition( pMesh, pArray, PosCount);

		// 개수를 맞춘 Normal Array를 새롭게 작성한다.
		OptimizeNormal( pMesh, pNormalArray, NormCount);

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[i]);
		}
	}

	I3_MUST_RELEASE(pArray);
	I3_MUST_RELEASE(pNormalArray);
	delete [] pTri;

	return pMesh;
}

void i3OptBuildCollisionMesh::TransformTri( i3CollideeMesh * pMesh)
{
	/*
	INT32 i;
	i3Vector3Array * pArray;
	VEC3D vec;

	// Position
	pArray = pMesh->GetPositionArray();
	for( i = 0; i < (INT32) pArray->GetVectorCount(); i++)
	{
		i3Matrix::TransformPoints( &vec, pArray->GetVector( i), 1, m_pStack->GetTop());

		pArray->SetVector( i, &vec);
	}

	// Normal
	pArray = pMesh->GetNormalArray();
	for( i = 0; i < (INT32) pArray->GetVectorCount(); i++)
	{
		i3Vector::TransformNormal( &vec, pArray->GetVector( i), m_pStack->GetTop());
		i3Vector::Normalize( &vec, &vec);

		//pArray->SetVector( i, &vec);
	}
	*/
}

i3CollideeMesh * i3OptBuildCollisionMesh::ConvertTo( i3GeometryAttr * pGeoAttr, INT32 * pIndexTable)
{
	i3CollideeMesh * pMesh = nullptr;
	i3VertexArray * pVA;
	i3IndexArray * pIA;

	pVA = pGeoAttr->GetVertexArray();
	pIA = pGeoAttr->GetIndexArray();

	switch( pGeoAttr->GetPrimitiveType())
	{
		case I3G_PRIM_TRILIST :
			if( pIA != nullptr)
				pMesh = ConvertIndexTriList( pIA, pVA, pIndexTable);
			else
				pMesh = ConvertTriList( pVA, pIndexTable);

			if( pMesh != nullptr)
			{
				if( m_bCollapseTransform)
				{
					//TransformTri( pMesh);
				}
			}
			break;

		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :
			if( pIA != nullptr)
				pMesh = ConvertIndexTriStrip( pGeoAttr, pIndexTable);
			else
				pMesh = ConvertTriStrip( pGeoAttr, pIndexTable);

			if( pMesh != nullptr)
			{
				if( m_bCollapseTransform)
				{
					//TransformTri( pMesh);
				}
			}
			break;
	}

	return pMesh;
}

const char *		i3OptBuildCollisionMesh::GetTerrainNameByType( INT32 type)
{
	return NXU::GetTerrainNameByType( type);
}

I3_TERRAIN_TYPE i3OptBuildCollisionMesh::GetTerrainTypeByName( const char * pszName)
{
	return NXU::GetTerrainTypeByName( pszName);
}

void i3OptBuildCollisionMesh::_UpdateTerrainType( i3Node * pNode)
{
	if( pNode->hasName() == false)
	{
		m_TerrainType = I3_TERRAIN_NA;
		return;
	}

	m_TerrainType = GetTerrainTypeByName( pNode->GetName());
}

bool i3OptBuildCollisionMesh::OnPreNode( i3Node * pNode, i3Node * pParent)
{
	INT32 i;
	i3Node * pNew = nullptr;

	if( i3::kind_of<i3Transform*>(pNode))
	{
		if( m_bCollapseTransform)
		{
			pNew = i3Node::new_object_ref();
		}
		
		// Collapse Transform이 아닌 경우에는, 그냥 Transform Node인 상태로 내버려둔다.
	}
	else if( i3::kind_of<i3Geometry*>(pNode))
	{
		// 현재 노드에는 i3GeometryAttr이 있을 수 있으며,
		// 이것들은 모두 i3Collidee로 변경되어야 한다.
		i3Geometry * pGeo = (i3Geometry *) pNode;
		i3ColliderSet * pColSet;
		i3CollideeMesh * pMesh;

		_UpdateTerrainType( pGeo);

		pColSet = i3ColliderSet::new_object_ref();
		pNew = (i3Node *) pColSet;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( i);

			if( i3::kind_of<i3GeometryAttr*>(pAttr))
			{
				pMesh = ConvertTo( (i3GeometryAttr *) pAttr);
				
				if( pMesh != nullptr)
				{
					i3Collider * pCollider;

					pCollider = i3Collider::new_object_ref();

					pCollider->SetCollidee( pMesh);

					if( m_bCollapseTransform)
					{
						pCollider->SetStyle( I3_COLLIDER_STYLE_STATIC);
					}

					pColSet->AddCollider( pCollider);

					// Reflect Map
					if( m_pLastReflectMap != nullptr)
					{
						pMesh->addReflectMap( m_pLastReflectMap);
					}
				}
			}
		}
	}
	else if( i3::same_of<i3AttrSet*>(pNode))
	{
		INT32 i;
		i3RenderAttr * pAttr;
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( i3::same_of<i3MaterialAttr*>(pAttr) )
			{
				i3MaterialAttr * pMtl = (i3MaterialAttr *) pAttr;

				i3Color::Set( &m_LastColor, pMtl->GetDiffuse());
			}
			else if( i3::same_of<i3ReflectMapBindAttr* >(pAttr))
			{
				i3ReflectMapBindAttr * pReflect = (i3ReflectMapBindAttr *) pAttr;

				m_pLastReflectMap = (i3TextureCube *) pReflect->GetTexture();
			}
		}

		if( m_bReplaceNode)
		{
			pNew = i3Node::new_object_ref();
		}
	}

	if( pNew != nullptr)
	{
		if( pNode->hasName())
		{
			pNew->SetName( pNode->GetName());
		}

		AddReplaceInfo( pNode, pNew);
	}

	return true;
}

bool i3OptBuildCollisionMesh::OnPostNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of<i3AttrSet* >(pNode))
	{
		INT32 i;
		i3RenderAttr * pAttr;
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( i3::same_of<i3MaterialAttr* >(pAttr))
			{
				i3Color::Set( &m_LastColor, (UINT8) 255, 255, 255, 255);
			}
			else if( i3::same_of<i3ReflectMapBindAttr* >(pAttr))
			{
				m_pLastReflectMap = nullptr;
			}
		}
	}

	return i3SceneOptimizer::OnPostNode( pNode, pParent);
}


void i3OptBuildCollisionMesh::RemoveAllReplaceInfo(void)
{
	
	for(size_t i = 0; i < m_ReplaceList.size(); i++)
	{
		I3_OPT_REPLACENODE_INFO * pInfo	= m_ReplaceList[i];
		i3MemFree( pInfo);
	}

	m_ReplaceList.clear();
}

void i3OptBuildCollisionMesh::AddReplaceInfo( i3Node * pSrc, i3Node * pDest)
{
	I3_OPT_REPLACENODE_INFO * pNew;

	//INT32 idx; 
	//idx = i3MemAlloc( sizeof(I3_OPT_REPLACENODE_INFO));
	pNew = (I3_OPT_REPLACENODE_INFO *) i3MemAlloc( sizeof(I3_OPT_REPLACENODE_INFO));
	I3ASSERT( pNew != nullptr);

	pNew->m_pSrc = pSrc;
	pNew->m_pDest = pDest;

	m_ReplaceList.push_back( pNew);
}

void i3OptBuildCollisionMesh::Trace( i3Node * pRoot)
{
	i3Node * pSrc, * pDest, * pChild;
	I3_OPT_REPLACENODE_INFO * pInfo;

	i3Color::Set( &m_LastColor, (UINT8) 255, 255, 255, 255);
	SetMatrixStackEnable(true);

	i3SceneOptimizer::Trace( pRoot);

	// Replace Node Info.가 모두 생성됐다...
	for(size_t i = 0; i < m_ReplaceList.size(); i++)
	{
		pInfo = m_ReplaceList[i];

		pSrc = pInfo->m_pSrc;
		pDest = pInfo->m_pDest;

		if( m_bReplaceNode)
		{
			// 원래의 노드와 Child, Parent 관계를 새롭게 연결한다.
			while( pSrc->GetChildCount() > 0)
			{
				pChild = pSrc->GetChild( 0);
				pChild->ChangeParent( pDest);
			}

			// Parent와의 관계 재설정
			{
				i3Node * pParent = pSrc->GetParent();

				if( pParent != nullptr)
				{
					pParent->RemoveChild( pSrc);
					pParent->AddChild( pDest);
				}
				else
				{
					I3_MUST_ADDREF( pDest);
					SetTraceRoot( pDest);
				}
			}
		}
	}

	RemoveAllReplaceInfo();

	BuildBoundBox( getTraceRoot());
}

void i3OptBuildCollisionMesh::BuildBoundBox( i3Node * pRoot)
{
	INT32 i;

	{
		i3BoundBox * pBox = i3BoundBox::new_object();

		pRoot->GetWrappingBound( pBox);

		if( pBox->IsEmpty() == false)
			pRoot->SetBound( pBox);
		else
			pRoot->SetBound(nullptr);

		I3_MUST_RELEASE(pBox);
	}

	for( i = 0; i < pRoot->GetChildCount(); i++)
	{
		BuildBoundBox( pRoot->GetChild(i));
	}
}

void i3OptBuildCollisionMesh::SetProperty( char * pszFieldName, char * pszValue)	//	add by KOMET
{
	if( i3::generic_is_iequal( pszFieldName, "CollapseTransformEnable") )
	{
		if( i3::generic_is_niequal(pszValue, "T", 1) ||
			 i3::generic_is_niequal(pszValue, "Y", 1) )
			SetCollapseTransformEnable(true);
		else if( i3::generic_is_niequal(pszValue, "F", 1) ||
			 i3::generic_is_niequal(pszValue, "N", 1) )
			SetCollapseTransformEnable(false);
		else
		{
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);
			return;
		}

		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, "ReplaceNodeEnable") )
	{
		if( i3::generic_is_niequal(pszValue, "T", 1) ||
			 i3::generic_is_niequal(pszValue, "Y", 1) )
			SetReplaceNodeEnable(true);		
		else if( i3::generic_is_niequal(pszValue, "F", 1) ||
			 i3::generic_is_niequal(pszValue, "N", 1) )
			SetReplaceNodeEnable(false);
		else
		{
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);
			return;
		}

		return;
	}

	I3TRACE("(%s) SetProperty : Unknown field name (%s = %s)\n", this->static_meta()->class_name(), pszFieldName, pszValue);	
}

const char *	i3OptBuildCollisionMesh::getDesc(void)
{
	return "i3Engine용 Collision Mesh 생성 (구 Collision System)";
}

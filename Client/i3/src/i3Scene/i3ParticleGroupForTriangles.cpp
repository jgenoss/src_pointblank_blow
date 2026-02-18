#include "i3SceneDef.h"
#include "i3ParticleGroupForTriangles.h"
#include "i3Decal.h"

I3_CLASS_INSTANCE( i3ParticleGroupForTriangles);

bool i3ParticleGroupForTriangles::SetCapacity( INT32 Count, bool bForce)
{
	if( bForce == false)
	{
		if( (m_MaxCount >= Count))
			return true;
	}

	m_MaxCount = Count;
	m_PrimCount = 0;

	// 용량을 증가시킨다.
	I3_SAFE_RELEASE( m_pGeoAttr);
	m_pGeoAttr = i3GeometryAttr::new_object();

	// Vertex Array
	{
		I3G_USAGE usage;

		usage = I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY;

		m_pVA = i3VertexArray::new_object_ref();
		m_pVA->Create( &m_VtxFmt, Count * 6, usage);
	}

	// Index Array
	{
		INT32 i;
		I3G_USAGE usage = 0;

		m_pIA = i3IndexArray::new_object_ref();
		m_pIA->Create( Count * 6, usage, m_pVA->GetCount());

		m_pIA->Lock();

		for( i = 0; i < (Count * 6); i += 3)
		{
			m_pIA->SetIndex( i + 0, (i + 0));
			m_pIA->SetIndex( i + 1, (i + 1));
			m_pIA->SetIndex( i + 2, (i + 2));
		}

		m_pIA->Unlock();
	}

	m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_TRILIST, 0, 0, false);
	m_pGeoAttr->SetIndexArray( m_pIA);

	return true;
}

void i3ParticleGroupForTriangles::_Lock(void)
{
	I3ASSERT( m_bLocked == false);

	m_bLocked = m_pVA->Lock( I3G_LOCK_OVERWRITE);
	m_pIA->Lock();
}

void i3ParticleGroupForTriangles::_Unlock(void)
{
	I3ASSERT( m_bLocked == true);

	m_pVA->Unlock();
	m_pIA->Unlock();

	m_bLocked = false;
}

void i3ParticleGroupForTriangles::AddForDecal( i3DecalDrawInfo * pInfo, UINT32 col, i3ParticleTransformMatrix* matSet)
{
	INT32 idxStart, cnt, i;
	VEC3D pos;

	cnt = pInfo->getTriangleCount();

	if( cnt <= 0)
		return;

	if( g_pRenderContext->IsReady() == false)
		return;

	idxStart = reserveQuad( cnt);
	if( idxStart == -1)
		return;

	if( m_bLocked == false)	_Lock();
	if( m_bLocked == false)
		return;

	//I3TRACE( "DECAL draw: %p\n", pInfo);

	// 기존에 버텍스버퍼 매핑 자체가 버그였었던 것이었음...(2015.04.23.수빈)
	// DecalDrawInfo의 버텍스갯수와 인덱스갯수는 m_pVA와 규칙성이 서로 다르므로..
	// idxStart * 3을 그대로 매핑하면 안된다... 일단 m_AccVertexCount를 추가하여 버그수정..

	const INT32 oldAccVertexCount = m_AccVertexCount;
	INT32 currVertIdx = oldAccVertexCount;

	const INT32 numVert_Info = pInfo->getVertexCount();
	for( i = 0; i < numVert_Info; ++i, ++currVertIdx )
	{
		i3Vector::TransformCoord( &pos, &pInfo->getVertices()[ i], &matSet->m_WorldTx);
		
		m_pVA->SetPosition(currVertIdx, &pos);
//		m_pVA->SetPosition( (idxStart * 3) + i, &pos);
	}
	
	m_AccVertexCount = currVertIdx;

	m_pVA->SetTextureCoords( 0, oldAccVertexCount, pInfo->getVertexCount(), pInfo->getUVArray());
	m_pVA->SetColor( oldAccVertexCount, pInfo->getVertexCount(), col);
	m_pIA->SetIndices( (idxStart * 3), cnt * 3, pInfo->getIndices(), oldAccVertexCount); //(idxStart * 3));
}

#define		_ROT( v, _x, _y) \
	v.x = ((c * (_x)) + (s * (_y)));\
	v.y = ((-s * (_x)) + (c * (_y)));\
	v.z = 0.0f

INT32 i3ParticleGroupForTriangles::AddForSprite( VEC3D * pCenter, VEC3D * pSize, REAL32 rot, bool bFace2Cam, i3ParticleTransformMatrix* matSet)
{
	REAL32 w, h, c, s;
	VEC3D norm;
	INT32 idx;

	if( g_pRenderContext->IsReady() == false)
		return -1;

	idx = reserveQuad( 2);
	if( idx == -1)
		return -1;

	if( m_bLocked == false)	_Lock();
	if( m_bLocked == false)
		return -1;

	w = pSize->x * 0.5f;
	h = pSize->y * 0.5f;
	c = i3Math::cos( rot);
	s = i3Math::sin( rot);

	VEC3D _pos[4];
	
	// 3DMAX
	_ROT( _pos[0], -w, +h);
	_ROT( _pos[1], +w, +h);
	_ROT( _pos[2], -w, -h);
	_ROT( _pos[3], +w, -h);

	if( bFace2Cam)
	{
		i3Matrix::TransformVectors( _pos, _pos, 4, &matSet->m_invCamTx);
	}
	
	i3Vector::Add( &_pos[0], &_pos[0], pCenter);
	i3Vector::Add( &_pos[1], &_pos[1], pCenter);
	i3Vector::Add( &_pos[2], &_pos[2], pCenter);
	i3Vector::Add( &_pos[3], &_pos[3], pCenter);

	i3Matrix::TransformPoints( _pos, _pos, 4, &matSet->m_WorldTx);

	const INT32 oldAccVertexCount = m_AccVertexCount;

	m_pVA->SetPositions( oldAccVertexCount, 4, _pos);
	
	m_AccVertexCount += 4;		// 4개씩 추가

	if( m_pVA->GetFormat()->GetHasNormal())
	{
		i3Vector::Set( &norm, 0.0f, 0.0f, 1.0f);

		i3Vector::TransformNormal( &norm, &norm, &matSet->m_invCamTx);
		i3Vector::Normalize( &norm, &norm);

		m_pVA->SetNormal( oldAccVertexCount + 0, &norm);
		m_pVA->SetNormal( oldAccVertexCount + 1, &norm);
		m_pVA->SetNormal( oldAccVertexCount + 2, &norm);
		m_pVA->SetNormal( oldAccVertexCount + 3, &norm);
	}

	UINT16 idxTable[6];

	idxTable[0] = (UINT16) ((idx * 2) + 0);
	idxTable[1] = (UINT16) ((idx * 2) + 2);
	idxTable[2] = (UINT16) ((idx * 2) + 1);

	idxTable[3] = (UINT16) ((idx * 2) + 1);
	idxTable[4] = (UINT16) ((idx * 2) + 2);
	idxTable[5] = (UINT16) ((idx * 2) + 3);

	m_pIA->SetIndices( (idx * 3), 6, idxTable);

	return idx;
}


// 아래 2개 함수는 Sprite에만 쓰여야 할것 같다..

void i3ParticleGroupForTriangles::SetColor( INT32 idx, UINT32 col)
{
	m_pVA->SetColor( (idx * 2), 4, col);
}

void i3ParticleGroupForTriangles::SetTextureCoord( INT32 idx, VEC2D * p1, VEC2D * p2)
{
	REAL32 u1, v1, u2, v2;

	u1 = i3Vector::GetU( p1);
	v1 = i3Vector::GetV( p1);
	u2 = i3Vector::GetU( p2);
	v2 = i3Vector::GetV( p2);

	{
		VEC2D _uv[4];

		i3Vector::Set( &_uv[0], u1, v1);
		i3Vector::Set( &_uv[1], u2, v1);
		i3Vector::Set( &_uv[2], u1, v2);
		i3Vector::Set( &_uv[3], u2, v2);

		m_pVA->SetTextureCoords( 0, idx * 2, 4, _uv);
	}
}

INT32 i3ParticleGroupForTriangles::GetTriangleCount(void) const
{
	return m_PrimCount;
}

INT32		i3ParticleGroupForTriangles::GetEndIndex() const
{
	return m_PrimCount * 3;
}

void	i3ParticleGroupForTriangles::PostTrace()
{
	i3ParticleGroup::PostTrace();
	m_AccVertexCount = 0;
}

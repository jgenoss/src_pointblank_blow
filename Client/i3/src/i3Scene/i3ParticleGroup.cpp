#include "i3SceneDef.h"
#include "i3ParticleGroup.h"
#include "i3Particle.h"
#include "i3ParticleGroupForTriangles.h"
#include "i3ParticleRender.h"
#include "i3GeometrySubsetAttr.h"

I3_CLASS_INSTANCE( i3ParticleGroup);

i3ParticleGroup::i3ParticleGroup(void)
{
	m_VtxFmt.SetHasPosition( true);
	m_VtxFmt.SetHasNormal( true);
	m_VtxFmt.SetHasColor( true);
	m_VtxFmt.SetTextureCoordSetCount( 1);
	m_VtxFmt.SetTextureCoordCount( 2);
}

i3ParticleGroup::~i3ParticleGroup(void)
{
	I3_SAFE_RELEASE( m_pGeoAttr);
	I3_SAFE_RELEASE( m_pInfo);
	I3_SAFE_RELEASE( m_pTexBind);
	I3_SAFE_RELEASE( m_pTexEnableAttr);
	I3_SAFE_RELEASE( m_pNormalMapBindAttr);
	I3_SAFE_RELEASE( m_pNormalMapEnableAttr);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pBlendModeAttr);
	I3_SAFE_RELEASE( m_pZWriteEnableAttr);
	I3_SAFE_RELEASE( m_pZTestEnableAttr);
	I3_SAFE_RELEASE( m_pZBiasAttr);
	I3_SAFE_RELEASE( m_pLightingEnableAttr);
}

bool i3ParticleGroup::Create( i3Particle * pParticle, INT32 maxCount)
{
	m_MaxCount = maxCount;

	SetParticle( pParticle);

	return true;
}

void i3ParticleGroup::Unbind( i3Particle * pParticle)
{
	if( pParticle->GetParticleInfo() == m_pInfo)
	{
		I3_SAFE_RELEASE( m_pInfo);
	}

	if( GetRefCount() <= 1)
	{
		if( g_pParticleRender != nullptr)
		{
			g_pParticleRender->RemoveGroup( this);
		}
	}
}

void i3ParticleGroup::SetParticle( i3Particle * pParticle, bool bRecreateGeom)
{
	m_pOrgParticle = pParticle;

	pParticle->setParticleGroup( this);

	I3_REF_CHANGE( m_pInfo, pParticle->GetParticleInfo());

	// Initialize
	{
		if( pParticle->getTexture() != nullptr)
		{
			if( m_pTexBind == nullptr)
				m_pTexBind = i3TextureBindAttr::new_object();

			if( m_pTexEnableAttr == nullptr)
				m_pTexEnableAttr = i3TextureEnableAttr::new_object();
		}

		if( pParticle->getNormalMap() != nullptr)
		{
			if( m_pNormalMapBindAttr == nullptr)
				m_pNormalMapBindAttr = i3NormalMapBindAttr::new_object();

			if( m_pNormalMapEnableAttr == nullptr)
				m_pNormalMapEnableAttr = i3NormalMapEnableAttr::new_object();
		}
		
		if( m_pBlendEnableAttr == nullptr)
			m_pBlendEnableAttr = i3BlendEnableAttr::new_object();

		if( m_pBlendModeAttr == nullptr)
			m_pBlendModeAttr = i3BlendModeAttr::new_object();

		if( m_pZWriteEnableAttr == nullptr)
			m_pZWriteEnableAttr = i3ZWriteEnableAttr::new_object();

		if( m_pZTestEnableAttr == nullptr)
			m_pZTestEnableAttr = i3ZTestEnableAttr::new_object();

		if( m_pZBiasAttr == nullptr)
			m_pZBiasAttr = i3ZBiasAttr::new_object();

		if( m_pLightingEnableAttr == nullptr)
			m_pLightingEnableAttr = i3LightingEnableAttr::new_object();
	}

	if( pParticle->getTexture() != nullptr)
	{
		m_pTexBind->SetTexture( pParticle->getTexture());
		m_pTexEnableAttr->Set( pParticle->isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE));
	}

	if( pParticle->getNormalMap() != nullptr)
	{
		m_pNormalMapBindAttr->SetTexture( pParticle->getNormalMap());
		m_pNormalMapEnableAttr->Set( pParticle->isParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE));
	}
	
	m_pBlendEnableAttr->Set( pParticle->isParticleStyle( I3_PARTICLE_STYLE_BLEND_ENABLE));

	m_pBlendModeAttr->SetSource( pParticle->getSrcBlend());
	m_pBlendModeAttr->SetDestination( pParticle->getDestBlend());
	m_pBlendModeAttr->SetBlendOperation( pParticle->getBlendOperation());

	m_pZWriteEnableAttr->Set( pParticle->isParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE));
	m_pZTestEnableAttr->Set( pParticle->isParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE));

	m_pZBiasAttr->setBias( pParticle->getZBias());
	m_pZBiasAttr->setSlope( pParticle->getZBiasSlope());

	m_pLightingEnableAttr->Set( pParticle->isParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE));

	UINT32 oldFormat = m_VtxFmt.GetFormat();
	UINT32 oldType = m_VtxFmt.GetTypes();

	if( m_pLightingEnableAttr->Get())
	{
		m_VtxFmt.SetHasNormal( true);
	}
	else
	{
		m_VtxFmt.SetHasNormal( false);
	}

	m_VtxFmt.SetHasColor( true);

	if( (m_pTexEnableAttr != nullptr) && (m_pTexEnableAttr->Get()))
	{
		m_VtxFmt.SetTextureCoordSetCount( 1);
		m_VtxFmt.SetTextureCoordCount( 2);
	}
	else
	{
		m_VtxFmt.SetTextureCoordSetCount( 0);
		m_VtxFmt.SetTextureCoordCount( 0);
	}

	if( m_pVA == nullptr)
		bRecreateGeom = true;

	if( ((oldFormat != m_VtxFmt.GetFormat()) || (oldType != m_VtxFmt.GetTypes())) && bRecreateGeom)
	{
		SetCapacity( m_MaxCount, true);
	}
}

bool i3ParticleGroup::SetCapacity( INT32 Count, bool bForce)
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
		m_pVA->Create( &m_VtxFmt, Count * 4, usage);
		m_pVA->setRestoreType( I3G_RESTORE_MEM);
	}

	// Index Array
	{
		INT32 i, idx;
		I3G_USAGE usage = 0;

		m_pIA = i3IndexArray::new_object_ref();
		m_pIA->Create( Count * 6, usage, m_pVA->GetCount());
		m_pIA->setRestoreType( I3G_RESTORE_MEM);

		m_pIA->Lock();

		for( i = 0, idx = 0; i < (Count * 6); i += 6, idx += 4)
		{
			m_pIA->SetIndex( i + 0, (idx + 0));
			m_pIA->SetIndex( i + 1, (idx + 2));
			m_pIA->SetIndex( i + 2, (idx + 1));

			m_pIA->SetIndex( i + 3, (idx + 1));
			m_pIA->SetIndex( i + 4, (idx + 2));
			m_pIA->SetIndex( i + 5, (idx + 3));
		}

		m_pIA->Unlock();
	}

	m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_TRILIST, 0, 0, false);
	m_pGeoAttr->SetIndexArray( m_pIA);

	return true;
}

bool i3ParticleGroup::IsSame( i3Particle * pParticle)
{
	// i3TextureBindAttr 조차 생성되지 않은 단계라면 비교할 가치도 없는 놈이다.
	if( pParticle->getTexture() != nullptr)
	{
		if( m_pTexBind == nullptr)
			return false;

		if( m_pTexBind->GetTexture() != pParticle->getTexture())
			return false;

		if( m_pTexEnableAttr->Get() != (pParticle->isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE)))
			return false;
	}
	else
	{
		if( m_pTexBind != nullptr)
			return false;
	}

	if( pParticle->getNormalMap() != nullptr)
	{
		if( m_pNormalMapEnableAttr->Get() != (pParticle->isParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE)))
			return false;

		if( m_pNormalMapBindAttr->GetTexture() != pParticle->getNormalMap())
			return false;
	}

	if( m_pBlendEnableAttr->Get() != (pParticle->isParticleStyle(I3_PARTICLE_STYLE_BLEND_ENABLE)))
		return false;

	if( (m_pBlendModeAttr->GetSource() != pParticle->getSrcBlend()) ||
		(m_pBlendModeAttr->GetDestination() != pParticle->getDestBlend()))
		return false;

	if( m_pZWriteEnableAttr->Get() != (pParticle->isParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE)))
		return false;

	if( m_pZTestEnableAttr->Get() != (pParticle->isParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE)))
		return false;

	if( (m_pZBiasAttr->getBias() != pParticle->getZBias()) ||
		(m_pZBiasAttr->getSlope() != pParticle->getZBiasSlope()))
		return false;

	if( m_pLightingEnableAttr->Get() != (pParticle->isParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE)))
		return false;

	return true;
}

void i3ParticleGroup::_Lock(void)
{
	I3ASSERT( m_bLocked == false);
	if( m_pVA != nullptr)
	{
		m_bLocked = m_pVA->Lock( I3G_LOCK_OVERWRITE);
	}
}

void i3ParticleGroup::_Unlock(void)
{
	I3ASSERT( m_bLocked == true);

	if( m_pVA != nullptr)
	{
		m_pVA->Unlock();
		m_bLocked = false;
	}
}

INT32 i3ParticleGroup::AddForQuad( VEC3D * pCenter, VEC3D * pSize, i3ParticleTransformMatrix* matSet)
{
	REAL32 cx, cy;
	INT32 idx;

	if( g_pRenderContext->IsReady() == false)
		return -1;

	idx = reserveQuad();
	if( idx == -1)
		return -1;

	if( m_bLocked == false)	_Lock();

	if( m_bLocked == false)
		return-1;

	cx = pSize->x * 0.5f;
	cy = pSize->y * 0.5f;

	VEC3D _pos[4];

	i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
	i3Vector::Set( &_pos[1], + cx, + cy, 0.0f);
	i3Vector::Set( &_pos[2], - cx, - cy, 0.0f);
	i3Vector::Set( &_pos[3], + cx, - cy, 0.0f);

	i3Matrix::TransformVectors( _pos, _pos, 4, &matSet->m_invCamTx);
	
	i3Vector::Add( &_pos[0], &_pos[0], pCenter);
	i3Vector::Add( &_pos[1], &_pos[1], pCenter);
	i3Vector::Add( &_pos[2], &_pos[2], pCenter);
	i3Vector::Add( &_pos[3], &_pos[3], pCenter);

	i3Matrix::TransformPoints( _pos, _pos, 4, &matSet->m_WorldTx);

	m_pVA->SetPositions( idx * 4, 4, _pos);

	return idx;
}

#define		_ROT( v, _x, _y) \
	v.x = ((c * (_x)) + (s * (_y)));\
	v.y = ((-s * (_x)) + (c * (_y)));\
	v.z = 0.0f

INT32 i3ParticleGroup::AddForSprite( VEC3D * pCenter, VEC3D * pSize, REAL32 rot, bool bFace2Cam, i3ParticleTransformMatrix* matSet)
{
	REAL32 w, h, c, s;
	VEC3D norm;
	INT32 idx;

	if( g_pRenderContext->IsReady() == false)
		return -1;

	idx = reserveQuad();
	if( idx == -1)
		return -1;

	if( m_bLocked == false)	_Lock();
	if( m_bLocked == false)
		return-1;

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

	m_pVA->SetPositions( idx * 4, 4, _pos);

	if( m_pVA->GetFormat()->GetHasNormal())
	{
		i3Vector::Set( &norm, 0.0f, 0.0f, 1.0f);

		i3Vector::TransformNormal( &norm, &norm, &matSet->m_invCamTx);
		i3Vector::Normalize( &norm, &norm);

		m_pVA->SetNormal( (idx * 4) + 0, &norm);
		m_pVA->SetNormal( (idx * 4) + 1, &norm);
		m_pVA->SetNormal( (idx * 4) + 2, &norm);
		m_pVA->SetNormal( (idx * 4) + 3, &norm);
	}

	return idx;
}

INT32 i3ParticleGroup::AddForBillboard( VEC3D * pStart, VEC3D * pEnd, VEC3D * pSize, i3ParticleTransformMatrix* matSet)
{
	REAL32 cy;
	VEC3D dir, viewAxis, By, spanAxis;
	INT32 idx;

	if( g_pRenderContext->IsReady() == false)
		return -1;

	idx = reserveQuad();
	if( idx == -1)
		return -1;

	if( m_bLocked == false)	_Lock();
	if( m_bLocked == false)
		return-1;

	i3Vector::Sub( &dir, pEnd, pStart);
	i3Vector::Normalize( &dir, &dir);

	i3Vector::Normalize( &viewAxis, i3Matrix::GetAt( &matSet->m_invCamTx));
	i3Vector::Cross( &spanAxis, &dir, &viewAxis);
	i3Vector::Normalize( &spanAxis, &spanAxis);

	cy = pSize->y * 0.5f;
	i3Vector::Scale( &By, &spanAxis, cy);

	VEC3D _pos[4];

	//   [0]           [1]
	//   +-------------+
	//   |             |
	//   +c            |
	//   |             |
	//   +-------------+
	//   [2]           [3]

	// 0 2 1 - 1 2 3
	
	i3Vector::Sub( &_pos[0], pStart,	&By);
	i3Vector::Sub( &_pos[1], pEnd,		&By);
	i3Vector::Add( &_pos[2], pStart,	&By);
	i3Vector::Add( &_pos[3], pEnd,		&By);

	i3Matrix::TransformPoints( _pos, _pos, 4, &matSet->m_WorldTx);

	m_pVA->SetPositions( idx * 4, 4, _pos);

	return idx;
}

void i3ParticleGroup::SetColor( INT32 idx, UINT32 col)
{
	m_pVA->SetColor( (idx * 4), 4, col);
}

void i3ParticleGroup::SetTextureCoord( INT32 idx, VEC2D * p1, VEC2D * p2)
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

		m_pVA->SetTextureCoords( 0, idx * 4, 4, _uv);
	}
}


void i3ParticleGroup::SetTextureCoordForBillboard( INT32 idx, VEC2D * p1, VEC2D * p2)
{
	REAL32 u1, v1, u2, v2;

	u1 = i3Vector::GetU( p1);
	v1 = i3Vector::GetV( p1);
	u2 = i3Vector::GetU( p2);
	v2 = i3Vector::GetV( p2);

	{
		VEC2D _uv[4];

		i3Vector::Set( &_uv[0], u1, v2);
		i3Vector::Set( &_uv[1], u1, v1);
		i3Vector::Set( &_uv[2], u2, v2);
		i3Vector::Set( &_uv[3], u2, v1);

		m_pVA->SetTextureCoords( 0, idx * 4, 4, _uv);
	}
}

INT32 i3ParticleGroup::GetTriangleCount(void) const
{
	return m_PrimCount * 2;
}

INT32		i3ParticleGroup::GetEndIndex() const
{
	return m_PrimCount * 6;
}

void		i3ParticleGroup::RenderBySort( i3SceneTracer* pTracer, i3GeometrySubsetAttr* geomSubsetAttr)
{
	
	i3AttrStackManager * pAttrStack = pTracer->GetAttrStackManager();
	i3RenderAttrListAttr *	pOutput = pTracer->GetOutput();

	if (geomSubsetAttr->GetPrimitiveCount() <= 0)
		return;

	// Attributes
	if( m_pTexBind != nullptr)
	{
		pAttrStack->PushAttr( m_pTexBind);
		pAttrStack->PushAttr( m_pTexEnableAttr);
	}

	if( m_pNormalMapBindAttr != nullptr)
	{
		pAttrStack->PushAttr( m_pNormalMapBindAttr);
		pAttrStack->PushAttr( m_pNormalMapEnableAttr);
	}

	pAttrStack->PushAttr( m_pBlendEnableAttr);
	pAttrStack->PushAttr( m_pBlendModeAttr);
	pAttrStack->PushAttr( m_pZWriteEnableAttr);
	pAttrStack->PushAttr( m_pZTestEnableAttr);
	pAttrStack->PushAttr( m_pZBiasAttr);
	pAttrStack->PushAttr( m_pLightingEnableAttr);

	//가산블렌딩 옵션이 들어가 있으면 fog를 끈다
	if (m_pBlendModeAttr->GetDestination() == I3G_BLEND_ONE)
		g_pParticleRender->PushAttr_FogDisable(pAttrStack);

	// Geometry	
	pAttrStack->FlushPushedAttrs( pOutput);

	pTracer->AddAttributeToOutput( geomSubsetAttr); //m_pGeoAttr);	m_pGeoAttr를 직접 쓰지 않아야 한다..

	if( m_pTexBind != nullptr)
	{
		pAttrStack->PopAttr( m_pTexEnableAttr->GetID());
		pAttrStack->PopAttr( m_pTexBind->GetID());
	}

	if( m_pNormalMapBindAttr != nullptr)
	{
		pAttrStack->PopAttr( m_pNormalMapEnableAttr->GetID());
		pAttrStack->PopAttr( m_pNormalMapBindAttr->GetID());
	}

	pAttrStack->PopAttr( m_pBlendEnableAttr->GetID());
	pAttrStack->PopAttr( m_pBlendModeAttr->GetID());
	pAttrStack->PopAttr( m_pZWriteEnableAttr->GetID());
	pAttrStack->PopAttr( m_pZTestEnableAttr->GetID());
	pAttrStack->PopAttr( m_pZBiasAttr->GetID());
	pAttrStack->PopAttr( m_pLightingEnableAttr->GetID());

	if (m_pBlendModeAttr->GetDestination() == I3G_BLEND_ONE)
		g_pParticleRender->PopAttr_FogDisable(pAttrStack);

	pAttrStack->FlushPushedAttrs( pOutput);
		
}

void	i3ParticleGroup::PostTrace()
{
	m_PrimCount = 0;

	if( m_bLocked)
		_Unlock();
}

void i3ParticleGroup::OnLostDevice( bool bLostDevice)
{
	if( m_pGeoAttr != nullptr)
	{
		if (m_bLocked)
		{
			_Unlock();
		}

		m_pGeoAttr->SetIndexArray( nullptr);
		m_pGeoAttr->SetVertexArray( nullptr, false);

		m_pVA = nullptr;
		m_pIA = nullptr;

		m_bLocked = false;
	}
}

void i3ParticleGroup::OnRevive( i3RenderContext * pCtx)
{
	//if((m_pGeoAttr != nullptr) && ( m_pGeoAttr->GetVertexArray() == nullptr))
	{
		SetCapacity( m_MaxCount, true);

		m_bLocked = false;
	}
}

#include "pch.h"
#include "Crosshair_Ring.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"

I3_CLASS_INSTANCE( Crosshair_Ring);//, Crosshair);

#define		NUM_SUB			20

Crosshair_Ring::Crosshair_Ring(void)
{
	m_pAttrSet = nullptr;
}

Crosshair_Ring::~Crosshair_Ring(void)
{
	I3_SAFE_NODE_RELEASE( m_pAttrSet);
}

void Crosshair_Ring::Create( void)
{
	REAL32 scrCX, scrCY;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	Crosshair::Create();

	m_pAttrSet = i3AttrSet::new_object();

	{
		i3Geometry * pGeo = i3Geometry::new_object_ref();
		m_pGeoAttr = i3GeometryAttr::new_object_ref();

		_CreateVertex();

		pGeo->AppendGeometryAttr( m_pGeoAttr);

		m_pAttrSet->AddChild( pGeo);
	}

	m_pSg->AddChild( m_pAttrSet);
}

void Crosshair_Ring::_CreateVertex(void)
{
	i3VertexFormat fmt;

	fmt.SetHasColor( true);

	m_pVA = i3VertexArray::new_object_ref();
	m_pVA->Create( &fmt, NUM_SUB * 2, I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY);

	m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_LINELIST, NUM_SUB);
}

void Crosshair_Ring::OnUpdate( REAL32 tm)
{
	INT32 i;
	
	REAL32 rmin, len, cx, cy;
	REAL32 scrCX, scrCY, rad = 0.0f, step = I3_PI * 2.0f / NUM_SUB;
	VEC3D pos;
	COLOR col;

	CWeaponInfo * pWeaponInfo = _GetCurrentWeaponInfo();
	if( pWeaponInfo == nullptr)
		return;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	CalcRminLen(rmin, len, scrCY, pWeaponInfo);

	cx = (rmin + len) / scrCX;
	cy = (rmin + len) / scrCY;

	m_pVA->Lock( I3G_LOCK_OVERWRITE);

	pos.z = 0.0f;

	i3Color::Set( &col, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
	m_pVA->SetColor( 0, NUM_SUB * 2, &col);

	for( i = 0; i < NUM_SUB * 2; i++)
	{
		pos.x = i3Math::sin_libm( rad) * cx;
		pos.y = i3Math::cos_libm( rad) * cy;
		m_pVA->SetPosition( i, &pos);

		rad += step;

		i++;
		pos.x = i3Math::sin_libm( rad) * cx;
		pos.y = i3Math::cos_libm( rad) * cy;
		m_pVA->SetPosition( i, &pos);
	}

	m_pVA->Unlock();

	len *= 0.5f;

	Crosshair::OnUpdate( tm);
}

bool Crosshair_Ring::OnLostDevice( bool bLostDevice)
{
	if( Crosshair::OnLostDevice( bLostDevice) == false)
		return false;

	m_pGeoAttr->SetVertexArray( nullptr);
	return true;
}

bool Crosshair_Ring::OnRevive( i3RenderContext * pCtx)
{
	if( Crosshair::OnRevive( pCtx) == false)
		return false;
	_CreateVertex();
	return true;
}

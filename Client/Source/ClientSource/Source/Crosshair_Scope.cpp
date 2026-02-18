#include "pch.h"
#include "Crosshair_Scope.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"

I3_CLASS_INSTANCE( Crosshair_Scope);//, Crosshair);

Crosshair_Scope::Crosshair_Scope(void)
{
	m_pAttrSet = nullptr;
}

Crosshair_Scope::~Crosshair_Scope(void)
{
	I3_SAFE_NODE_RELEASE( m_pAttrSet);
}

void Crosshair_Scope::Create( void)
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
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
		pAttr->Set( true);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3Geometry * pGeo = i3Geometry::new_object_ref();
		m_pGeoAttr = i3GeometryAttr::new_object_ref();

		_CreateVertex();

		pGeo->AppendGeometryAttr( m_pGeoAttr);

		m_pAttrSet->AddChild( pGeo);
	}

	m_pSg->AddChild( m_pAttrSet);
}

void Crosshair_Scope::_CreateVertex(void)
{
	i3VertexFormat fmt;

	fmt.SetHasColor( true);

	m_pVA = i3VertexArray::new_object_ref();
	m_pVA->Create( &fmt, 4 * 2, I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY);

	m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_LINELIST, 4);
}

void Crosshair_Scope::OnUpdate( REAL32 tm)
{
	REAL32 rmin, len, cx1, cy1, cx2, cy2, s, c;
	REAL32 scrCX, scrCY;
	VEC3D pos[8];
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

	cx1 = (rmin + len) / scrCX;
	cx2 = (rmin) / scrCX;

	cy1 = (rmin + len) / scrCY;
	cy2 = (rmin) / scrCY;

	m_pVA->Lock( I3G_LOCK_OVERWRITE);

	i3Color::Set( &col, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	col.a = (UINT8)(min( 1.0f, (rmin / 64.0f)) * 255);

	m_pVA->SetColor( 0, 4 * 2, &col);

	// 11시 방향
	s = i3Math::sin_libm( I3_DEG2RAD( 45.0f + 270.0f));
	c = i3Math::cos_libm( I3_DEG2RAD( 45.0f + 270.0f));
	i3Vector::Set( &pos[0], s * cx1, c * cy1, 0.0f);
	i3Vector::Set( &pos[1], s * cx2, c * cy2, 0.0f);

	// 2시 방향
	s = i3Math::sin_libm( I3_DEG2RAD( 45.0f));
	c = i3Math::cos_libm( I3_DEG2RAD( 45.0f));
	i3Vector::Set( &pos[2], s * cx1, c * cy1, 0.0f);
	i3Vector::Set( &pos[3], s * cx2, c * cy2, 0.0f);

	// 5시 방향
	s = i3Math::sin_libm( I3_DEG2RAD( 45.0f + 90.0f));
	c = i3Math::cos_libm( I3_DEG2RAD( 45.0f + 90.0f));
	i3Vector::Set( &pos[4], s * cx1, c * cy1, 0.0f);
	i3Vector::Set( &pos[5], s * cx2, c * cy2, 0.0f);

	// 8시 방향
	s = i3Math::sin_libm( I3_DEG2RAD( 45.0f + 180.0f));
	c = i3Math::cos_libm( I3_DEG2RAD( 45.0f + 180.0f));
	i3Vector::Set( &pos[6], s * cx1, c * cy1, 0.0f);
	i3Vector::Set( &pos[7], s * cx2, c * cy2, 0.0f);

	m_pVA->SetPositions( 0, 8, pos);

	m_pVA->Unlock();

	len *= 0.5f;

	Crosshair::OnUpdate( tm);
}

bool Crosshair_Scope::OnLostDevice( bool bLostDevice)
{
	if( Crosshair::OnLostDevice( bLostDevice) == false)
		return false;
	m_pGeoAttr->SetVertexArray( nullptr);
	return true;
}

bool Crosshair_Scope::OnRevive( i3RenderContext * pCtx)
{
	if( Crosshair::OnRevive( pCtx) == false)
		return false;
	_CreateVertex();
	return true;
}

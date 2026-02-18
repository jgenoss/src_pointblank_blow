#include "pch.h"
#include "Crosshair_Scope2.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"

I3_CLASS_INSTANCE( Crosshair_Scope2);//, Crosshair);

#define		NUM_SUB			20

Crosshair_Scope2::Crosshair_Scope2(void)
{
	m_pAttrSet = nullptr;
}

Crosshair_Scope2::~Crosshair_Scope2(void)
{
	I3_SAFE_NODE_RELEASE( m_pAttrSet);
}

void Crosshair_Scope2::Create( void)
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
		pGeo->AppendGeometryAttr(m_pGeoAttr);

		_CreateVertex();
		
		m_pAttrSet->AddChild( pGeo);
	}

	m_pSg->AddChild( m_pAttrSet);
}

void Crosshair_Scope2::_CreateVertex(void)
{
	i3VertexFormat fmt;

	fmt.SetHasColor( true);

	m_pVA = i3VertexArray::new_object_ref();
	m_pVA->Create( &fmt, NUM_SUB * 2, I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY);

	m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_LINELIST, NUM_SUB);
}


void Crosshair_Scope2::OnUpdate( REAL32 tm)
{
	INT32 i;
	REAL32 rmin, len, cx, cy;
	REAL32 scrCX, scrCY, deg, step;
	VEC3D pos1, pos2;
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

	// I3TRACE("rmin %f len %f scrCY %f\n", rmin, len, scrCY);

	cx = (rmin + len) / scrCX;
	cy = (rmin + len) / scrCY;

	m_pVA->Lock( I3G_LOCK_OVERWRITE);

	pos1.z = 0.0f;
	pos2.z = 0.0f;

	i3Color::Set( &col, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	col.a = 255; //(UINT8)(min( 1.0f, (rmin / 64.0f)) * 255);

	m_pVA->SetColor( 0, NUM_SUB * 2, &col);

	deg = 20.0f;
	step = (90.0f - (deg * 2.0f)) / (NUM_SUB >> 2);

	for( i = 0; i < (NUM_SUB >> 2); i++)
	{
		pos1.x = i3Math::sin_libm( I3_DEG2RAD( deg)) * cx;
		pos1.y = i3Math::cos_libm( I3_DEG2RAD( deg)) * cy;
		deg += step;
		pos2.x = i3Math::sin_libm( I3_DEG2RAD( deg)) * cx;
		pos2.y = i3Math::cos_libm( I3_DEG2RAD( deg)) * cy;

		// 1/4 분면 ( +, +)
		m_pVA->SetPosition( (i * 8) + 0, &pos1);
		m_pVA->SetPosition( (i * 8) + 1, &pos2);

		// 2/4 분면 ( +, -)
		pos1.y = -pos1.y;		pos2.y = -pos2.y;
		m_pVA->SetPosition( (i * 8) + 2, &pos1);
		m_pVA->SetPosition( (i * 8) + 3, &pos2);

		// 4/4 분면 ( -, -)
		pos1.x = -pos1.x;		pos2.x = -pos2.x;
		m_pVA->SetPosition( (i * 8) + 4, &pos1);
		m_pVA->SetPosition( (i * 8) + 5, &pos2);

		// 4/4 분면 ( -, +)
		pos1.y = -pos1.y;		pos2.y = -pos2.y;
		m_pVA->SetPosition( (i * 8) + 6, &pos1);
		m_pVA->SetPosition( (i * 8) + 7, &pos2);
	}

	m_pVA->Unlock();

	len *= 0.5f;

	Crosshair::OnUpdate( tm);
}

bool Crosshair_Scope2::OnLostDevice( bool bLostDevice)
{
	if( Crosshair::OnLostDevice( bLostDevice) == false)
		return false;
	m_pGeoAttr->SetVertexArray( nullptr);
	return true;
}

bool Crosshair_Scope2::OnRevive( i3RenderContext * pCtx)
{
	if( Crosshair::OnRevive( pCtx) == false)
		return false;
	_CreateVertex();
	return true;
}

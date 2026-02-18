#include "pch.h"
#include "Crosshair_Dot.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( Crosshair_Dot);//, Crosshair);

Crosshair_Dot::Crosshair_Dot(void)
{
	m_pAttrSet = nullptr;
}

Crosshair_Dot::~Crosshair_Dot(void)
{
	I3_SAFE_NODE_RELEASE( m_pAttrSet);
}

void Crosshair_Dot::Create( void)
{
	Crosshair::Create();

	m_pAttrSet = i3AttrSet::new_object();

	{
		COLOR col;

		m_pColorAttr = i3ColorAttr::new_object_ref();
		m_pAttrSet->AddAttr( m_pColorAttr);

		i3Color::Set( &col, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
		m_pColorAttr->Set( &col);
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

void Crosshair_Dot::_CreateVertex(void)
{
	i3VertexFormat fmt;
	i3VertexArray * pVA;
	REAL32 cx, cy;
	VEC3D pos[6];

	REAL32 scrCX, scrCY;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}
	
	cx = 3.0f / scrCX;
	cy = 3.0f / scrCY;

	pVA = i3VertexArray::new_object_ref();
	pVA->Create( &fmt, 6, 0);

	pVA->Lock();

	i3Vector::Set( &pos[0],	0.0f, -cy, 0.0f);
	i3Vector::Set( &pos[1],	-cx, 0.0f, 0.0f);
	i3Vector::Set( &pos[2],	+cx, 0.0f, 0.0f);
	
	i3Vector::Set( &pos[3],	+cx, 0.0f, 0.0f);
	i3Vector::Set( &pos[4],	-cx, 0.0f, 0.0f);
	i3Vector::Set( &pos[5],	0.0f, +cy, 0.0f);

	pVA->SetPositions( 0, 6, pos);

	pVA->Unlock();

	m_pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
}

void Crosshair_Dot::OnUpdate( REAL32 tm)
{
	Crosshair::OnUpdate( tm);

	COLOR col;

	i3Color::Set( &col, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
	m_pColorAttr->Set( &col);
}

bool Crosshair_Dot::OnLostDevice( bool bLostDevice)
{
	if( Crosshair::OnLostDevice( bLostDevice) == false)
		return false;

	m_pGeoAttr->SetVertexArray( nullptr);
	return true;
}

bool Crosshair_Dot::OnRevive( i3RenderContext * pCtx)
{
	if( Crosshair::OnRevive( pCtx) == false)
		return false;

	_CreateVertex();
	return true;
}

#include "pch.h"
#include "Grid.h"

I3_CLASS_INSTANCE( CGrid, i3GameObj);

CGrid::CGrid(void)
{
	m_pAttrSet = NULL;
	m_pGeo = NULL;
	m_pGeoAttr = NULL;
	m_pVA = NULL;
}

CGrid::~CGrid(void)
{
	I3_SAFE_RELEASE( m_pAttrSet);
}

void CGrid::Create(void)
{
	{
		m_pAttrSet = i3AttrSet::NewObject();

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();

			pAttr->Set( FALSE);

			m_pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZBiasAttr * pAttr = i3ZBiasAttr::NewObjectRef();

			pAttr->setBias( -0.01f);
			//pAttr->setSlope( 
			m_pAttrSet->AddAttr( pAttr);
		}
	}

	{
		m_pGeo = i3Geometry::NewObjectRef();

		m_pAttrSet->AddChild( m_pGeo);
	}

	{
		m_pGeoAttr = i3GeometryAttr::NewObjectRef();

		m_pGeo->AppendGeometryAttr( m_pGeoAttr);
	}

	{
		i3VertexFormat fmt;

		fmt.SetHasPosition( true);
		fmt.SetHasColor( true);

		m_pVA = i3VertexArray::NewObjectRef();

		m_pVA->Create( &fmt, 101 * 101 * 2, 0);

		m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_LINELIST, 0);
	}

	SetInstanceSg( m_pAttrSet);
}

void CGrid::SetSize( REAL32 sx, REAL32 sy)
{
	INT32 i, idx = 0;
	INT32 cx, cy;
	VEC3D pos;
	COLOR hiline, normline;

	cx = (INT32)sx + 1;
	cy = (INT32)sy + 1;

	I3ASSERT( (cx * cy * 2) <= (INT32) m_pVA->GetCount());
	
	m_pVA->Lock( 0, 0, cx * cy);

	i3Vector::Set( &pos, 0.0f, 0.0f, 0.0f);

	i3Color::Set( &hiline, 128, 128, 128, 255);
	i3Color::Set( &normline, 64, 64, 64, 255);

	// 가로 방향 (0->+, 원점 포함)
	pos.x = -(sx * 0.5f);
	for( i = 0; i < cx; i++)
	{
		pos.z = sy * 0.5f;
		m_pVA->SetPosition( idx, &pos);	idx++;

		pos.z = -sy * 0.5f;
		m_pVA->SetPosition( idx, &pos); idx++;

		if( (i % 5) == 0)
			m_pVA->SetColor( idx - 2, 2, &hiline);
		else
			m_pVA->SetColor( idx - 2, 2, &normline);

		pos.x += 1.0f;
	}

	// 가로 방향 (0->+, 원점 포함)
	pos.z = -(sy * 0.5f);
	for( i = 0; i < cy; i++)
	{
		pos.x = sx * 0.5f;
		m_pVA->SetPosition( idx, &pos); idx++;

		pos.x = -sx * 0.5f;
		m_pVA->SetPosition( idx, &pos); idx++;

		if( (i % 5) == 0)
			m_pVA->SetColor( idx - 2, 2, &hiline);
		else
			m_pVA->SetColor( idx - 2, 2, &normline);

		pos.z += 1.0f;
	}

	m_pVA->Unlock();

	m_pGeoAttr->SetPrimitiveCount( cx * cy);
}

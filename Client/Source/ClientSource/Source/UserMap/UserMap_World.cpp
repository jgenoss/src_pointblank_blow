#include "pch.h"
#include "UserMap_World.h"

I3_CLASS_INSTANCE( CUserMap_World, i3GameObj);

CUserMap_World::CUserMap_World(void)
{
	m_pAttrSet		= NULL;
	m_pTexBindAttr	= NULL;
	m_pGeoAttr		= NULL;
	m_pVA			= NULL;
	m_pSky			= NULL;
	m_pGroundCvx	= NULL;
	m_SkyType		= 0;
	m_pLight		= NULL;
}

CUserMap_World::~CUserMap_World(void)
{
	I3_SAFE_RELEASE( m_pAttrSet);
	I3_SAFE_RELEASE( m_pSky);
	I3_SAFE_RELEASE( m_pGroundCvx);

	if( m_pLight != NULL)
	{
		i3SceneGraphContext *	pCtx = g_pFramework->getSgContext();
		I3ASSERT( pCtx != NULL);

		pCtx->removeLight( m_pLight);

		m_pLight->Release();
		m_pLight = NULL;
	}
}

void CUserMap_World::Create(void)
{
	{
		m_pAttrSet = i3AttrSet::NewObject();

		getAttachNode()->AddChild( m_pAttrSet);

		{
			i3GameResTexture * pRes = (i3GameResTexture *) g_pFramework->FindResourceByOnlyName( "deftex_grass01.i3i");
			I3ASSERT( pRes != NULL);

			m_pTexBindAttr = i3TextureBindAttr::NewObjectRef();

			m_pTexBindAttr->SetTexture( pRes->getTexture());
			m_pAttrSet->AddAttr( m_pTexBindAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::NewObjectRef();

			pAttr->Set( TRUE);
			m_pAttrSet->AddAttr( pAttr);
		}

		{
			i3MaterialAttr * pAttr = i3MaterialAttr::NewObjectRef();
			
			pAttr->SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f);
			pAttr->SetSpecular( 0.3f, 0.3f, 0.3f, 1.0f);
			pAttr->SetShininess( 64.0f);

			m_pAttrSet->AddAttr( pAttr);
		}
	}

	// 지형 충돌용
	{
		VEC3D c, sz;

		i3Vector::Set( &c, 0.0f, -1.0f, 0.0f);
		i3Vector::Set( &sz, 200.0f, 2.0f, 200.0f);

		m_pGroundCvx = i3PhysixShapeSet::NewObject();

		m_pGroundCvx->CreateBoxShape( &c, &sz, CGRP_WORLD, false, NULL, I3_TERRAIN_GROUND);
	}

	{
		i3VertexFormat fmt;

		fmt.SetHasPosition( true);
		fmt.SetHasNormal( true);
		fmt.SetTextureCoordSetCount( 1);
		fmt.SetTextureCoordCount( 2);

		m_pVA = i3VertexArray::NewObjectRef();
		m_pVA->Create( &fmt, 6, 0);
	}

	{
		m_pGeoAttr = i3GeometryAttr::NewObjectRef();

		m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_TRILIST, 2);
	}

	{
		i3Geometry * pGeo = i3Geometry::NewObjectRef();

		m_pAttrSet->AddChild( pGeo);

		pGeo->AppendGeometryAttr( m_pGeoAttr);
	}

	{
		m_pLight = i3Light::NewObject();

		i3SceneGraphContext * pCtx = g_pFramework->getSgContext();

		pCtx->addLight( m_pLight);
	}

	SetSize( 200.0f, 200.0f);
	SetSky( 0);
}

void CUserMap_World::SetSky( USMAP::SKY::TYPE type)
{
	USMAP::SKY::INFO * pInfo = USMAP::SKY::getInfo( type);

	m_SkyType = type;

	if( m_pSky != NULL)
	{
		m_pSky->RemoveFromParent();
	}

	I3_REF_CHANGE( m_pSky, pInfo->m_pSg->getInstanceSg());

	getAttachNode()->AddChild( m_pSky);

	// Light Direction
	{
		MATRIX mtx;

		i3Matrix::MakeWithAt( &mtx, &pInfo->m_LightDir, &I3_YAXIS);
		i3Matrix::PostTranslate( &mtx, 0.0f, 100.0f, 0.0f);

		m_pLight->SetWorldSpaceTransform( &mtx);
	}

	// Light Diffuse
	{
		m_pLight->SetDiffuse( & pInfo->m_LightDiffuse);
		m_pLight->SetSpecular( & pInfo->m_LightSpecular);
	}
}

void CUserMap_World::SetSize( REAL32 cx, REAL32 cz)
{
	I3ASSERT( m_pVA != NULL);
	VEC3D pos[6], norm[6];
	VEC2D uv[6];
	REAL32 minx, maxx, minz, maxz;

	minx = -cx * 0.5f;
	maxx = cx * 0.5f;
	minz = -cz * 0.5f;
	maxz = cz * 0.5f;

	i3Vector::Set( &pos[0],		minx,	0.0f,	minz);
	i3Vector::Set( &pos[1],		minx,	0.0f,	maxz);
	i3Vector::Set( &pos[2],		maxx,	0.0f,	minz);
	i3Vector::Set( &pos[3],		maxx,	0.0f,	minz);
	i3Vector::Set( &pos[4],		minx,	0.0f,	maxz);
	i3Vector::Set( &pos[5],		maxx,	0.0f,	maxz);

	i3Vector::Set( &norm[0],	0.0f,	1.0f,	0.0f);
	i3Vector::Set( &norm[1],	0.0f,	1.0f,	0.0f);
	i3Vector::Set( &norm[2],	0.0f,	1.0f,	0.0f);
	i3Vector::Set( &norm[3],	0.0f,	1.0f,	0.0f);
	i3Vector::Set( &norm[4],	0.0f,	1.0f,	0.0f);
	i3Vector::Set( &norm[5],	0.0f,	1.0f,	0.0f);

	i3Vector::Set( &uv[0],		0.0f,	cz);
	i3Vector::Set( &uv[1],		0.0f,	0.0f);
	i3Vector::Set( &uv[2],		cx,		cz);
	i3Vector::Set( &uv[3],		cx,		cz);
	i3Vector::Set( &uv[4],		0.0f,	0.0f);
	i3Vector::Set( &uv[5],		cx,		0.0f);

	m_pVA->Lock();

	m_pVA->SetPositions( 0, 6, pos);
	m_pVA->SetNormals( 0, 6, norm);
	m_pVA->SetTextureCoords( 0, 0, 6, uv);

	m_pVA->Unlock();
}


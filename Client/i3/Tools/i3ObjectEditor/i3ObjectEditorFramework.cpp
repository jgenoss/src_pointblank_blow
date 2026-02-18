#include "stdafx.h"
#include "resource.h"
#include "i3ObjectEditorFramework.h"
#include "GlobalVar.h"
#include "MainFrm.h"

I3_CLASS_INSTANCE( i3ObjectEditorFramework); //, i3ViewerFramework);

i3ObjectEditorFramework::i3ObjectEditorFramework(void)
{
	m_pCaster = NULL;
	m_pReceiver = NULL;
	m_pFloorRoot = NULL;
	m_pShapeSet = NULL;
	m_pEmitEffectIcon = NULL;
}

i3ObjectEditorFramework::~i3ObjectEditorFramework(void)
{
	I3_SAFE_RELEASE( m_pFloorRoot);
	I3_SAFE_RELEASE( m_pReceiver);
	I3_SAFE_RELEASE( m_pCaster);
	I3_SAFE_RELEASE( m_pShapeSet);
	I3_SAFE_RELEASE( m_pEmitEffectIcon);
}

#define		SHADOWMAP_SZ		256

void i3ObjectEditorFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	m_pCaster = i3Node::new_object();

	m_pReceiver = i3Node::new_object();
	m_pReceiver->AddChild( m_pCaster);

	m_pAttrSet->AddChild( m_pReceiver);

	{
		i3ShadeModeAttr * pAttr = i3ShadeModeAttr::new_object_ref();

		pAttr->Set( I3G_SHADE_PHONG);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		VEC3D pos[6], norm[6];
		VEC2D tex[6];
		i3GeometryAttr * pGeoAttr;
		i3VertexFormat fmt;
		i3VertexArray * pVA;
		i3Geometry * pGeo;
		i3AttrSet * pAttrSet;

		i3Vector::Set( &pos[0],	-0.5f, 0.0f, -0.5f);
		i3Vector::Set( &pos[1],	-0.5f, 0.0f, 0.5f);
		i3Vector::Set( &pos[2],	0.5f, 0.0f, -0.5f);
		i3Vector::Set( &pos[3],	0.5f, 0.0f, -0.5f);
		i3Vector::Set( &pos[4],	-0.5f, 0.0f, 0.5f);
		i3Vector::Set( &pos[5],	0.5f, 0.0f, 0.5f);

		i3Vector::Set( &norm[0], 0.0f, 1.0f, 0.0f);
		i3Vector::Set( &norm[1], 0.0f, 1.0f, 0.0f);
		i3Vector::Set( &norm[2], 0.0f, 1.0f, 0.0f);
		i3Vector::Set( &norm[3], 0.0f, 1.0f, 0.0f);
		i3Vector::Set( &norm[4], 0.0f, 1.0f, 0.0f);
		i3Vector::Set( &norm[5], 0.0f, 1.0f, 0.0f);

		i3Vector::Set( &tex[0], 0.0f, 0.0f);
		i3Vector::Set( &tex[1], 0.0f, 0.0f);
		i3Vector::Set( &tex[2], 0.0f, 0.0f);
		i3Vector::Set( &tex[3], 0.0f, 0.0f);
		i3Vector::Set( &tex[4], 0.0f, 0.0f);
		i3Vector::Set( &tex[5], 0.0f, 0.0f);

		fmt.SetHasPosition( TRUE);
		fmt.SetHasNormal( TRUE);
		fmt.SetTextureCoordSetCount( 1);
		fmt.SetTextureCoordCount( 2);

		pVA = i3VertexArray::new_object_ref();

		pVA->Create( &fmt, 6, 0);
		pVA->Lock();
		pVA->SetPositions( 0, 6, pos);
		pVA->SetNormals( 0, 6, norm);
		pVA->SetTextureCoords( 0, 0, 6, tex);
		pVA->Unlock();

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);
		pGeo->AddFlag( I3_NODEFLAG_DISABLE);

		pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->AddChild( pGeo);

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
			pAttr->Set( TRUE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3TextureBindAttr * pTexBind = i3TextureBindAttr::new_object_ref();
			i3Texture * pTex = i3Texture::new_object_ref();

			pTex->Create( 32, 32, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);
			pTex->Lock( 0);

			memset( pTex->GetLockedBuffer(0), 0xFF, i3Gfx::CalcRasterBufferSize( 32, 32, 1, pTex->GetFormat()));
			pTex->Unlock(0);

			pTexBind->SetTexture( pTex);

			pAttrSet->AddAttr( pTexBind);
		}

		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			pAttr->SetDiffuse( 0.6f, 0.6f, 0.6f, 0.6f);
			pAttr->SetSpecular( 1.0f, 1.0f, 1.0f, 1.0f);
			//pAttr->SetShininess( 0.0f);
			pAttrSet->AddAttr( pAttr);
		}

		m_pFloorRoot = i3Transform2::new_object();
		m_pFloorRoot->AddChild( pAttrSet);

		m_pFloorRoot->SetMatrixObject( m_pGrid->GetMatrixObject());
		m_pReceiver->AddChild( m_pFloorRoot);

		_CreateFloor();
	}

	// Emit Effect
	{
		COLORREAL color;

		i3Color::Set( &color, (UINT8) 250, 182, 80, 255);

		i3Node * pAxisMesh = i3SceneUtil::BuildAxisMesh(20, 1.0f, 0.05f, TRUE, TRUE, &color, &color);
		// BuildAxisMesh는 레퍼런스카운팅을 가지므로, Release() 처리 요구됨..(12.09.18.수빈)
		i3Transform * pRotTrans = i3Transform::new_object_ref();
		pRotTrans->AddChild( pAxisMesh);		pAxisMesh->Release();

		i3Matrix::SetRotateZDeg( pRotTrans->GetMatrix(), 90.0f);

		m_pEmitEffectIcon = i3Transform::new_object();
		m_pEmitEffectIcon->AddChild( pRotTrans);
		m_pEmitEffectIcon->AddFlag( I3_NODEFLAG_DISABLE);

		Get3DRoot()->AddChild( m_pEmitEffectIcon);
	}
}

void i3ObjectEditorFramework::setEmitEffectIconEnable( bool bFlag)
{
	if( bFlag)
		m_pEmitEffectIcon->RemoveFlag( I3_NODEFLAG_DISABLE);
	else
		m_pEmitEffectIcon->AddFlag( I3_NODEFLAG_DISABLE);
}

void i3ObjectEditorFramework::setEmitEffectIconPos( MATRIX * pMtx)
{
	m_pEmitEffectIcon->SetMatrix( pMtx);
}

void i3ObjectEditorFramework::_CreateFloor(void)
{
	I3_SAFE_RELEASE( m_pShapeSet);
	m_pShapeSet = i3PhysixShapeSet::new_object();

	VEC3D center, size;

	i3Vector::Set( &size, 1.0f, 1.0f, 1.0f);
	i3Vector::Set( &center, 0.0f, 0.0f, 0.0f);

	m_pShapeSet->CreateBoxShape( &center, &size, 0, true);

	m_pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE, 0);

	INT32 i;
	
	for( i = 0; i < 32; i++)
	{
		g_pPhysixCtx->setGroupCollisionEnable( 0, (UINT16)i, true);
	}
}

void i3ObjectEditorFramework::_ResizeFloor(void)
{
	NxBoxShape * pBox;

	if( m_pShapeSet == NULL)
	{
		_CreateFloor();
	}

	pBox = (NxBoxShape *) m_pShapeSet->getNxShape( 0);

	VEC3D size, center;

	i3Vector::Sub( &size, &m_BoundMax, &m_BoundMin);
	i3Vector::Scale( &size, &size, 10.0f);
	i3Vector::Add( &center, &m_BoundMax, &m_BoundMin);
	i3Vector::Scale( &center, &center, 0.5f);
	i3Vector::AddY( &center, -3.5f);

	NxVec3 sz, c;

	sz.x = size.x;
	sz.y = 1.0f;
	sz.z = size.z;

	c.x = center.x;
	c.y = center.y;
	c.z = center.z;

	pBox->setDimensions( sz);
	pBox->setLocalPosition( c);
}

i3Node * i3ObjectEditorFramework::OnQueryAttachNode( i3GameRes * pRes)
{
	return m_pCaster;
}

void i3ObjectEditorFramework::OnChangeBound(void)
{
	i3ViewerFramework::OnChangeBound();

	VEC3D pos;
	MATRIX * pLightTx = m_pLightTx->GetMatrix();

	i3Vector::Set( &pos, 0.0f, 0.0f, -m_CamDist * 4.0f);

	i3Matrix::SetTranslate( pLightTx, &pos);
	i3Matrix::PostRotateXDeg( pLightTx, 136.0f);
	i3Matrix::PostRotateYDeg( pLightTx, 30.0f);

	i3Matrix::PostTranslate( pLightTx, &m_BasePos);

	_ResizeFloor();
}

void i3ObjectEditorFramework::OnUpdate( REAL32 tm)
{
	i3ViewerFramework::OnUpdate( tm);

	if( g_pObject != NULL)
	{
		g_pObject->OnUpdate( tm);
	}
}

void i3ObjectEditorFramework::OnChangeTime( REAL32 tm)
{
	i3ViewerFramework::OnChangeTime( tm);

	CMainFrame * pWnd = (CMainFrame *) AfxGetMainWnd();

	pWnd->OnChangeTime( tm);
}

bool i3ObjectEditorFramework::OnStartPlay(void)
{
	CMainFrame * pWnd = (CMainFrame *) AfxGetMainWnd();

	pWnd->OnStartPlay();

	return true;
}

bool i3ObjectEditorFramework::OnStartPause(void)
{
	CMainFrame * pWnd = (CMainFrame *) AfxGetMainWnd();

	pWnd->OnStartPause();

	return true;
}


#include "stdafx.h"
#include "i3CharaEditorFramework.h"
#include "i3Base/string/ext/resolve_path_from_environment_variable.h"
#include "i3Base/string/ext/get_exec_path.h"

I3_CLASS_INSTANCE( i3CharaEditorFramework); //, i3ViewerFramework);

i3CharaEditorFramework::i3CharaEditorFramework(void)
{
//	m_pShadowMap = NULL;
	m_pCaster = NULL;
	m_pReceiver = NULL;
	m_pFloorRoot = NULL;
	m_pReflectMapAttr = NULL;
}

i3CharaEditorFramework::~i3CharaEditorFramework(void)
{
	I3_SAFE_RELEASE( m_pFloorRoot);
	I3_SAFE_RELEASE( m_pReceiver);
	I3_SAFE_RELEASE( m_pCaster);
	I3_SAFE_RELEASE( m_pReflectMapAttr);

}

//#define		SHADOWMAP_SZ		1024

void i3CharaEditorFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();
	
	m_pCaster = i3Node::new_object();

	m_pReceiver = i3Node::new_object();
	m_pReceiver->AddChild( m_pCaster);

	m_pAttrSet->AddChild( m_pReceiver);
	
	{
		i3Light * pLight = m_pLightAttr->GetLightObject();

		pLight->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);
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

		pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->AddChild( pGeo);
		pAttrSet->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set( TRUE);
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

			memset( pTex->GetLockedBuffer(0), 0x7F, i3Gfx::CalcRasterBufferSize( 32, 32, 1, pTex->GetFormat()));
			pTex->Unlock(0);

			pTexBind->SetTexture( pTex);

			pAttrSet->AddAttr( pTexBind);
		}

		// Default ReflectMap
		{
			i3TextureCube * pTex;
			i3ImageFile file;
			char szFull[ MAX_PATH];
			
			i3::stack_string strPath;
		//	i3System::ResolvePath( "$(I3_ROOT)/Bin/res", szPath, sizeof(szPath));
			i3::resolve_path_from_environment_variable( "$(I3_ROOT)/Bin/res", strPath);


			i3::snprintf( szFull, MAX_PATH ,"%s/defl_cubemap.dds", strPath);

			if( i3System::IsFile( szFull) == FALSE)
			{
			//	i3System::GetExecPath( szPath);
				i3::get_exec_path(strPath);

				i3::snprintf( szFull, MAX_PATH, "%s/res/defl_cubemap.dds", strPath);
			}

			pTex = (i3TextureCube *) file.Load( szFull);

			if( pTex != NULL)
			{
				m_pReflectMapAttr = i3ReflectMapBindAttr::new_object();

				m_pReflectMapAttr->SetTexture( pTex);

				pTex->Release();

				m_pAttrSet->AddAttr( m_pReflectMapAttr);
			}
		}

		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			//pAttr->SetAmbient( 0.1f, 0.1f, 0.1f, 0.1f);
			pAttr->SetDiffuse( 0.6f, 0.6f, 0.6f, 0.6f);
			pAttr->SetSpecular( 0.0f, 0.0f, 0.0f, 0.0f);
			//pAttr->SetShininess( 0.0f);
			pAttrSet->AddAttr( pAttr);
		}

		m_pFloorRoot = i3Transform::new_object();
		m_pFloorRoot->AddChild( pAttrSet);

		m_pFloorRoot->SetMatrixObject( m_pGrid->GetMatrixObject());
		m_pReceiver->AddChild( m_pFloorRoot);
		{
			i3BoundBox * pBox = i3BoundBox::new_object_ref();

			pGeo->GetWrappingBound( pBox);
			pGeo->SetBound( pBox);
		}

		this->SetGridShow( false);
	}

	#if 0
	{
		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();

		{
			i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		m_pShadowMapView = i3Sprite2D::new_object();
		m_pShadowMapView->Create( 2, TRUE, FALSE);
		m_pShadowMapView->SetTexture( m_pShadowMap->getShadowMap());

		//m_pShadowMapView->AddFlag( I3_NODEFLAG_DISABLE);

		m_pShadowMapView->SetEnable( 0, TRUE);
		m_pShadowMapView->SetRect( 0, 0.0f, 0.0f, 255.0f, 255.0f);
		//m_pShadowMapView->SetColor( 0, 255, 255, 255, 255);
		m_pShadowMapView->SetTextureCoord( 0, 0.0f, 0.0f, (REAL32) SHADOWMAP_SZ, (REAL32) SHADOWMAP_SZ);
		
		pAttrSet->AddChild( m_pShadowMapView);

		Get2DRoot()->AddChild( pAttrSet);
	}
	#endif
}

i3Node * i3CharaEditorFramework::OnQueryAttachNode( i3GameRes * pRes)
{
	return m_pCaster;
}

void i3CharaEditorFramework::OnChangeBound(void)
{
	i3ViewerFramework::OnChangeBound();

	VEC3D pos;
	MATRIX * pLightTx = m_pLightTx->GetMatrix();

	i3Vector::Set( &pos, 0.0f, 0.0f, -m_CamDist * 4.0f);

	i3Matrix::SetTranslate( pLightTx, &pos);
	i3Matrix::PostRotateXDeg( pLightTx, -65.0f);
	i3Matrix::PostRotateYDeg( pLightTx, 30.0f);

	i3Matrix::PostTranslate( pLightTx, &m_BasePos);


/*	if( m_pShadowMap != NULL)
	{
		m_pShadowMap->setLight( m_pLightAttr->GetLightObject(), I3_DEG2RAD( 25.0f), 1.0f, m_CamFar);
		m_pShadowMap->SetLightTransform( pLightTx);
		m_pShadowMap->setAutoFitCamDist( m_CamDist);

		if( m_pShadowMapView != NULL)
		{
			m_pShadowMapView->SetEnable( 0, TRUE);
			m_pShadowMapView->SetRect( 0, 0.0f, 0.0f, 255.0f, 255.0f);
		}
	}
	*/
}

void	i3CharaEditorFramework::OnKeyDown( UINT32 nKey)
{
	i3ViewerFramework::OnKeyDown( nKey);

//	MATRIX * pLightTx = m_pLightTx->GetMatrix();
//	m_pShadowMap->SetLightTransform( pLightTx);
}
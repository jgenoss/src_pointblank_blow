#include "stdafx.h"
#include "i3LevelCameraObject.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelCameraObject);

i3LevelCameraObject::i3LevelCameraObject(void)
{
	m_pCameraObject = i3CameraObject::new_object();
	m_pCameraObject->setCamera( m_pCamera);

	m_pTrans = i3Transform::new_object();

	{
		{
			m_pFrustumAttrSet = i3AttrSet::new_object();
			m_pTrans->AddChild( m_pFrustumAttrSet);

			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
				COLORREAL col;

				i3Color::Set( &col, (UINT8) 246, 223, 153, 255);
				pAttr->Set( &col);
				m_pFrustumAttrSet->AddAttr( pAttr);
			}

			{
				i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

				pAttr->Set( FALSE);
				m_pFrustumAttrSet->AddAttr( pAttr);
			}

			{
				i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

				pAttr->Set( FALSE);
				m_pFrustumAttrSet->AddAttr( pAttr);
			}

			i3Geometry * pGeom = i3Geometry::new_object_ref();		// 임시변수로 Add말고 없어서 ref로 교체.(12.09.18.수빈)

			i3GeometryAttr * pGeomAttr = i3GeometryAttr::new_object_ref();// 임시변수로 Add말고 없어서 ref로 교체.(12.09.18.수빈)
			pGeom->AppendGeometryAttr( pGeomAttr);

			m_pFrustumAttrSet->AddChild( pGeom);
			m_pFrustumAttrSet->AddFlag( I3_NODEFLAG_DISABLE);

			{
				VEC3D	vZero;

				i3VertexFormat fmt;
				m_pVA = i3VertexArray::new_object();
				
				fmt.SetHasPosition( true);
				
				m_pVA->Create( &fmt, 8, 0);
				m_pVA->Lock();

				for( INT32 i = 0;i < 8; ++i)
				{
					m_pVA->SetPosition( i, &vZero);
				}

				i3IndexArray * pIA = i3IndexArray::new_object_ref();// 임시변수로 SetIndexArray말고 없어서 ref로 교체.(12.09.18.수빈)
				pIA->Create( 24, 0, m_pVA->GetCount());
				pIA->Lock();
				{
					//near
					pIA->SetIndex(0, 0);	pIA->SetIndex(1, 1); pIA->SetIndex(2, 1); pIA->SetIndex(3, 2);
					pIA->SetIndex(4, 2);	pIA->SetIndex(5, 3); pIA->SetIndex(6, 3); pIA->SetIndex(7, 0);

					//middle
					pIA->SetIndex(8, 0);	pIA->SetIndex(9, 4); pIA->SetIndex(10, 1); pIA->SetIndex(11, 5);
					pIA->SetIndex(12, 2);	pIA->SetIndex(13, 6); pIA->SetIndex(14, 3); pIA->SetIndex(15, 7);

					//far
					pIA->SetIndex(16, 4);	pIA->SetIndex(17, 5); pIA->SetIndex(18, 5); pIA->SetIndex(19, 6);
					pIA->SetIndex(20, 6);	pIA->SetIndex(21, 7); pIA->SetIndex(22, 7); pIA->SetIndex(23, 4);
				}
				pIA->Unlock();

				pGeomAttr->SetVertexArray( m_pVA, I3G_PRIM_LINELIST, 12, 0);
				pGeomAttr->SetIndexArray( pIA);
			}
		}
	}
}

i3LevelCameraObject::~i3LevelCameraObject(void)
{
	I3_SAFE_RELEASE(m_pFrustumAttrSet);	// 릴리즈부분이 보이지 않음...릭버그로 간주...(12.09.18.수빈)
	I3_SAFE_RELEASE(m_pTrans);			// 릴리즈부분이 보이지 않음...릭버그로 간주...(12.09.18.수빈)
	I3_SAFE_RELEASE(m_pCameraObject);	// 릴리즈부분이 보이지 않음...릭버그로 간주...(12.09.18.수빈)
	I3_SAFE_RELEASE( m_pVA);
}

void i3LevelCameraObject::BindRes( i3LevelRes * pRes)
{
	i3::safe_string_copy( m_szInstanceClass, "i3CameraObject", 128);

	i3Node * pNode = i3Scene::CreateClone( g_pScene->GetCommonCameraResource()->getSceneGraphInfo()->getInstanceSg(), I3_COPY_INSTANCE);
	I3ASSERT( pNode != nullptr);

	m_pTrans->AddChild( pNode);

	SetSymbol( m_pTrans);

	_UpdateFrustum();
}

void i3LevelCameraObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelCamera::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3LevelCameraObject*>(pDest));

	i3LevelCameraObject * pCamObj = (i3LevelCameraObject*)pDest;

	if( m_pCameraObject != nullptr)
	{
		m_pCameraObject->CopyTo( pCamObj->getCameraObject(), method);
	}
}

i3GameObj	*	i3LevelCameraObject::CreateGameInstance(void)
{
	I3_SAFE_ADDREF(m_pCameraObject);

	return m_pCameraObject;
}


UINT32	i3LevelCameraObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Result = i3LevelCamera::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = m_pCameraObject->OnSave( pResFile);
	if( Rc == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

bool i3LevelCameraObject::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelCamera::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	{
		i3XMLElement * pChild = new i3XMLElement( "CameraObject");

		pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
		I3ASSERT( pChild != nullptr);

		rv = m_pCameraObject->OnSaveXML( pFile, pChild);
		if( rv == false)
			return false;
	}
	
	return rv;
}

UINT32 i3LevelCameraObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Result = i3LevelCamera::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	
	Rc = m_pCameraObject->OnLoad( pResFile);
	if( Rc == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	Result += Rc;

	BindRes( nullptr);

	return Result;
}

bool i3LevelCameraObject::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelCamera::OnLoadXML( pFile, pXML);
	if( rv == false)
	{
		return false;
	}

	{
		i3XMLElement * pChild = (i3XMLElement *) pXML->FirstChild( "CameraObject");

		if( pChild == nullptr)
		{
			rv = m_pCameraObject->OnLoadXML( pFile, pXML);
		}
		else
		{
			rv = m_pCameraObject->OnLoadXML( pFile, pChild);
		}

		if( rv == false)
			return false;
	}

	BindRes( nullptr);

	return true;
}

void i3LevelCameraObject::Select(void)
{
	i3LevelCamera::Select();

	_UpdateFrustum();

	m_pFrustumAttrSet->RemoveFlag( I3_NODEFLAG_DISABLE);
}

void i3LevelCameraObject::Unselect(void)
{
	i3LevelCamera::Unselect();

	m_pFrustumAttrSet->AddFlag( I3_NODEFLAG_DISABLE);
}
	
void i3LevelCameraObject::_UpdateFrustum()
{
	const VEC3D	vFrust[8] = 
	{
		VEC3D( -1.0f, -1.0f, -1.0f),
		VEC3D(  1.0f, -1.0f, -1.0f),
		VEC3D(  1.0f,  1.0f, -1.0f),
		VEC3D( -1.0f,  1.0f, -1.0f),
		VEC3D( -1.0f, -1.0f,  1.0f),
		VEC3D(  1.0f, -1.0f,  1.0f),
		VEC3D(  1.0f,  1.0f,  1.0f),
		VEC3D( -1.0f,  1.0f,  1.0f)
	};

	//
	{
		m_pCamera->SetPerspective( m_pCamera->GetFOV(), (REAL32)m_pCamera->GetWindowWidth(), (REAL32)m_pCamera->GetWindowHeight(),
				m_pCamera->GetNear(), m_pCamera->GetFar());

	}

	MATRIX * pProjectionMatrix = m_pCamera->GetProjectionMatrix();

	MATRIX invProj;

	i3Matrix::Inverse( &invProj, nullptr, pProjectionMatrix);
	
	m_pVA->Lock();
	for( INT32 i = 0; i < 8; ++i)
	{
		VEC3D vPos;

		i3Vector::TransformCoord( &vPos, &vFrust[i], &invProj);
		m_pVA->SetPosition( i, &vPos);
	}
	m_pVA->Unlock();
}
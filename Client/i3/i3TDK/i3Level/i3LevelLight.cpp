#include "stdafx.h"
#include "i3LevelLight.h"
#include "i3LevelResLight.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelLight);

i3LevelLight::i3LevelLight(void)
{
	AddStyle( I3_LEVEL_STYLE_TOUCH_BBOXONLY);

	m_pLight = i3LightObject::new_object();
	m_pLight->Create((i3GameResSceneGraph*)nullptr);

	setRefObject( m_pLight);
	
	m_pTrans = i3Transform::new_object();
	MATRIX *	pMat = m_pTrans->GetMatrix();

	i3Matrix::SetScale( pMat, 10.0f, 10.0f, 10.0f);
		
	VEC3D vPos;
	i3Vector::Set( &vPos, 0.0f, -15.0f, 0.0f);
	i3Matrix::SetPos( pMat, &vPos);

	//i3Matrix::PostRotateAxis( pMat, &I3_XAXIS, I3_DEG2RAD( 180.0f));

	{
		COLORREAL colDiffuse, colAmbient, colLine;

		i3Color::Set( &colDiffuse, 1.0f, 1.0f, 0.3f, 0.5f);
		i3Color::Set( &colAmbient, 1.0f, 1.0f, 0.3f, 0.5f);

		i3Color::Set( &colLine, 0.99f, 0.66f, 0.33f, 0.0f);

		i3Node * pCapsule = i3SceneUtil::BuildCapsuleMesh( 1.0f, 5.0f, FALSE, TRUE, 8, 6, &colLine, &colAmbient, &colDiffuse );

		m_pTrans->AddChild( pCapsule);

		I3_MUST_RELEASE(pCapsule);			// 반드시 필요하다...(누락되어 릭 발생..2012.09.11.수빈)
	}

	
/*
	m_rLightDist	= - 50.0f;
	i3Vector::Set( &m_vLightRot, 135.0f, 45.0f, 0.0f);
	i3Vector::Set( &m_vLightTargetPos, 0.0f, 0.0f, 0.0f);

	m_pIcon = nullptr;
	{
		m_pIcon = i3LevelIcon::new_object();
		m_pIcon->SetOwner( this);
		m_pIcon->SetStyle( I3_LEVEL_STYLE_VOLATILE | I3_LEVEL_STYLE_CONTROL | I3_LEVEL_STYLE_NOMOVE | I3_LEVEL_STYLE_NOSCALE | I3_LEVEL_STYLE_NOROTATE);
		m_pIcon->SetName( GetName());
		{
			i3LevelResSceneGraph * pRes = nullptr;
			i3LevelResSceneGraph * pNewRes = i3LevelResSceneGraph::new_object();
			pRes = g_pScene->GetCommonCameraResource();
			pRes->CopyTo( pNewRes, I3_COPY_INSTANCE);
			m_pIcon->BindRes( pNewRes);
			pNewRes->Release();
		}

		g_pScene->AddElement( m_pIcon, "Light");
		m_pIcon->SetScale( 10.0f, 10.0f, 10.0f);
		UpdateTrans();
	}
*/
	i3::safe_string_copy( m_szInstanceClass, "i3LightObject", 128);
}

i3LevelLight::~i3LevelLight(void)
{
	I3_SAFE_RELEASE( m_pLight);

	I3_SAFE_RELEASE( m_pTrans);
/*
	if(m_pIcon != nullptr)
	{
		if( g_pScene != nullptr)
			g_pScene->RemoveElement( m_pIcon);

		I3_SAFE_RELEASE( m_pIcon);
	}
	*/
}

void i3LevelLight::UpdateTrans()
{
	/*
	I3ASSERT( m_pLight != nullptr);
	MATRIX mtx;
	
	i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, m_rLightDist);
	i3Matrix::PostRotateXDeg( &mtx, i3Vector::GetX( &m_vLightRot));
	i3Matrix::PostRotateYDeg( &mtx, i3Vector::GetY( &m_vLightRot));
	i3Matrix::PostTranslate( &mtx, &m_vLightTargetPos);

	m_pLight->SetMatrix( &mtx);
	if( m_pIcon != nullptr)
	{
		m_pIcon->SetPos( i3Matrix::GetPos( &mtx));
	}*/
}

i3GameObj *	i3LevelLight::CreateGameInstance(void)
{
	I3_SAFE_ADDREF(m_pLight);

	return m_pLight;
}

void i3LevelLight::Select()
{
	//이전 상태와 다른상태로의 변경을 요구하는지 검사
	m_pTrans->RemoveFlag( I3_NODEFLAG_DISABLE);

	i3LevelElement3D::Select();
}

void	i3LevelLight::Unselect()
{
	m_pTrans->AddFlag( I3_NODEFLAG_DISABLE);

	i3LevelElement3D::Unselect();
}

void	i3LevelLight::BindRes( i3LevelRes * pRes)
{
	I3ASSERT( i3::kind_of<i3LevelResLight*>(pRes));

	i3LevelElement3D::BindRes( pRes);

	i3LevelResLight * pResLight = (i3LevelResLight * )pRes;

	if( pResLight->getLightObject() == nullptr)
	{
		pResLight->setLightObject( m_pLight);
	}
	else
	{
		I3_REF_CHANGE( m_pLight, pResLight->getLightObject());
	}

	i3SceneGraphInfo * pInfo = pResLight->getScene();
	if( pInfo != nullptr)
	{
		i3Node * pNode = i3Scene::CreateClone( pInfo->getInstanceSg(), I3_COPY_INSTANCE);

		SetSymbol( pNode);

		m_pSymbol->AddChild( m_pTrans);
	}
}

/*
void	i3LevelLight::OnAttachScene( i3LevelScene * pScene)
{
	if( m_pIcon != nullptr)
	{
		pScene->AddElement( m_pIcon, "Light");
	}

	i3LevelElement3D::OnAttachScene( pScene);
}

void	i3LevelLight::OnDetachScene( i3LevelScene * pScene)
{
	if( m_pIcon != nullptr)
	{
		pScene->RemoveElement( m_pIcon);
	}

	i3LevelElement3D::OnDetachScene( pScene);
}
*/

void i3LevelLight::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelElement3D::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelLight*>(pDest));

	i3LevelLight * pLight = (i3LevelLight*)pDest;

	pLight->setLightDist( getLightDist());
	pLight->setLightRotation( getLightRotation());
	pLight->setLightTargetPos( getLightTargetPos());

	if( getLightObject() != nullptr)
	{
		getLightObject()->CopyTo( pLight->getLightObject(), method);
	}
}

void	i3LevelLight::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pLight != nullptr)
	{
		m_pLight->OnBuildObjectList( List);
	}

	i3LevelElement3D::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct LIGHT
	{
		UINT8			m_ID[4] = { 'L', 'G', 'T', '1' };
		OBJREF64		m_pLight = 0;

		REAL32			m_rLightDist = 0.0f;
		i3::pack::VEC3D	m_vLightRot;
		i3::pack::VEC3D	m_vLightTargetPos;

		UINT32		pad[6] = { 0 };
	};
}

#if defined ( I3_COMPILER_VC)
#pragma pack (pop)
#endif

UINT32	i3LevelLight::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::LIGHT data;

	Result = i3LevelElement3D::OnSave( pResFile);
	I3_CHKIO( Result);


	data.m_pLight				= (OBJREF64)pResFile->GetObjectPersistID( m_pLight);
	data.m_rLightDist			= m_rLightDist;

	data.m_vLightRot.x			= getX( &m_vLightRot);
	data.m_vLightRot.y			= getY( &m_vLightRot);
	data.m_vLightRot.z			= getZ( &m_vLightRot);

	data.m_vLightTargetPos.x	= getX( &m_vLightTargetPos);
	data.m_vLightTargetPos.y	= getY( &m_vLightTargetPos);
	data.m_vLightTargetPos.z	= getZ( &m_vLightTargetPos);

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3LevelLight::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::LIGHT data;

	Result = i3LevelElement3D::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pLight != 0)
	{
		/*
		i3LightObject * pLight = (i3LightObject *)pResFile->FindObjectByID( (OBJREF)data.m_pLight);
		I3ASSERT( pLight != nullptr);
		
		I3_REF_CHANGE( m_pLight, pLight);
		*/
	}

	m_rLightDist = data.m_rLightDist;

	setX( &m_vLightRot, data.m_vLightRot.x);
	setY( &m_vLightRot, data.m_vLightRot.y);
	setZ( &m_vLightRot, data.m_vLightRot.z);

	setX( &m_vLightTargetPos, data.m_vLightTargetPos.x);
	setY( &m_vLightTargetPos, data.m_vLightTargetPos.y);
	setZ( &m_vLightTargetPos, data.m_vLightTargetPos.z);

	UpdateTrans();

	return Result;
}

bool i3LevelLight::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelElement3D::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->addAttr( "Light",		pFile->GetObjectPersistID( m_pLight));
	pXML->addAttr( "Distance",	m_rLightDist);

	i3Vector::SetToXML( pXML, "LightRot", & m_vLightRot);
	i3Vector::SetToXML( pXML, "LightTargetPos",	&m_vLightTargetPos);

	return true;
}

bool i3LevelLight::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;
	UINT32 uval;

	rv = i3LevelElement3D::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->getAttr( "Light",	&uval);
	if( uval != 0)
	{
		/*
		i3Light * pLight = (i3Light *) pFile->FindObjectByID( uval);
		I3ASSERT( pLight != nullptr);

		I3_REF_CHANGE( m_pLight, pLight);
		*/
	}

	pXML->getAttr( "Distance", &m_rLightDist);
	
	i3Vector::GetFromXML( pXML, "LightRot", &m_vLightRot);
	i3Vector::GetFromXML( pXML, "LightTargetPos", &m_vLightTargetPos);

	return true;
}

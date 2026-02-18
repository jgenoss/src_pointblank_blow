#include "stdafx.h"
#include "i3LevelResPrim.h"
#include "i3LevelPrimitive.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelResPrimitive);

///////////////////////////////////////////////////////////////////////////////
i3LevelResPrimitive::i3LevelResPrimitive()
{
	m_ResType	= I3LV_TYPE_PRIMITIVE;
}

i3LevelResPrimitive::~i3LevelResPrimitive()
{
	I3_SAFE_RELEASE( m_pScene);
}

bool i3LevelResPrimitive::OnValidateBrief( bool bForce)
{
	I3ASSERT( m_pScene == nullptr);

	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;
	}

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool i3LevelResPrimitive::OnValidate( i3LevelScene * pScene, bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	m_pScene = i3AttrSet::new_object();

	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		m_pScene->AddAttr( pAttr);
	}

	{
		i3TextureBindAttr * pAttr = i3TextureBindAttr::new_object_ref();

		pAttr->SetTexture( nullptr);
		m_pScene->AddAttr( pAttr);
	}

	i3Node * pNode = _BuildScene();
	I3ASSERT( pNode != nullptr);

	m_pScene->AddChild( pNode);

	addResState( I3LV_RES_STATE_LOADED);

	return true;
}

void i3LevelResPrimitive::OnQueryExportPath( i3LevelScene * pScene, i3LevelElement3D * pObj, char * pszPath, UINT16 nBufSize)
{
	// Level_Primitive들은 모두 자기 자신만의 Resource를 가지고 있으며,
	// 여기 호출된 녀석도 그렇게 하나의 Level_Primitive만을 위해 생성된 것일 가능성이 있다.
}

bool i3LevelResPrimitive::OnExport( i3LevelScene * pScene, const char * pszPath)
{
	i3SceneGraphInfo * pInfo;

	pInfo = i3SceneGraphInfo::new_object();

	pInfo->setInstanceSg( m_pScene);
	pInfo->SetResourcePath( getPath());
	
	i3SceneFile file;

	file.setKeyObject( pInfo);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		I3_MUST_RELEASE(pInfo);
		return false;
	}

	I3_MUST_RELEASE(pInfo);

	return true;
}

i3LevelElement3D *	i3LevelResPrimitive::CreateInstance( i3LevelScene * pScene)
{
	i3LevelPrimitive * pObj;
	char szUniqueName[ 256];

	OnValidate( pScene, false);

	// Level_Primitive Resource는 실제 Resource가 아니라, Runtime-Generation을 의미하는 Resource라
	// 할 수 있다. 그렇기 때문에, Level_Primitive의 Instance는 별도의 i3LevelResSceneGraph를 생성해
	// 새로운 Resource로써 DB에 등록하고, 그것을 사용하는 Element Instance를 생성한다.

	{
		i3ClassMeta * pInstMeta = GetInstanceMeta();
		pObj = (i3LevelPrimitive *) pInstMeta->create_instance(); //CREATEINSTANCE( pInstMeta);

		pObj->BindRes( this);

		pScene->GetUniqueElementName( GetName(), szUniqueName);

		pObj->SetName( szUniqueName);
		
		{
			REAL32 r, g, b;
			COLORREAL col;

			r = 0.3f + (i3Math::Randf() * 0.7f);
			g = 0.3f + (i3Math::Randf() * 0.7f);
			b = 0.3f + (i3Math::Randf() * 0.7f);

			i3Color::Set( &col, r, g, b, 0.5f);

			pObj->setMaterialColor( &col);
		}

		pScene->AddElement( pObj, pObj->GetName());
	}

	return pObj;
}

i3Node *	i3LevelResPrimitive::GetElementAttachNode( i3LevelScene * pScene)
{
	return nullptr;
}

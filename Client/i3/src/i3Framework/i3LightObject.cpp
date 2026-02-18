#include "i3FrameworkPCH.h"
#include "i3LightObject.h"

I3_CLASS_INSTANCE( i3LightObject);

i3LightObject::i3LightObject(void)
{
	SetNode( m_pLightSet);
}

bool	i3LightObject::Create( i3Node *pParentNode, bool bEnable)
{
	m_pLightAttr = i3LightAttr::new_object_ref();
	m_pLightSet->AddLightAttr( m_pLightAttr);

	if( pParentNode != nullptr)
	{
		pParentNode->AddChild( m_pLightSet);
	}

	SetEnable( bEnable);

	return true;
}

void	i3LightObject::Create( i3GameResSceneGraph * pRes)
{
	m_pLightAttr = i3LightAttr::new_object_ref();
	m_pLightSet->AddLightAttr( m_pLightAttr);

	I3_REF_CHANGE( m_pGameRes, pRes);

	OnBindResource();
}

void i3LightObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameObj::CopyTo( pDest, method);

	i3LightObject * pLight = (i3LightObject *) pDest;

	if( m_pLightAttr != nullptr)
	{
		if( pLight->getLightAttr() == nullptr)
		{
			pLight->Create( (i3GameResSceneGraph*)nullptr);
		}

		m_pLightAttr->CopyTo( pLight->getLightAttr(), method);
	}
}

void i3LightObject::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pLightAttr != nullptr)
	{
		m_pLightAttr->OnBuildObjectList( List);
	}

	i3GameObj::OnBuildObjectList( List);
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 LIGHT_OBJECT
	{
		UINT8		m_ID[4] = { 'L', 'T', 'O', '1' };
		OBJREF64	m_pLightAttr = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LightObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::LIGHT_OBJECT data;

	result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_pLightAttr =		(OBJREF64) pResFile->GetObjectPersistID( m_pLightAttr);

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3LightObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::LIGHT_OBJECT data;

	result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pLightAttr != 0)
	{
		i3LightAttr * pAttr = (i3LightAttr *) pResFile->FindObjectByID( (OBJREF) data.m_pLightAttr);
		I3ASSERT( pAttr != nullptr);

		if( m_pLightAttr != nullptr)
		{
			m_pLightSet->RemoveLightAttr( m_pLightAttr);
			m_pLightAttr = nullptr;
		}

		if( pAttr != nullptr)
		{
			m_pLightAttr = pAttr;
			m_pLightSet->AddLightAttr( m_pLightAttr);
		}
	}

	return result;
}

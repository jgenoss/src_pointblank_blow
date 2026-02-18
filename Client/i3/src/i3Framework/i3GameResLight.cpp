#include "i3FrameworkPCH.h"
#include "i3GameResLight.h"

I3_CLASS_INSTANCE( i3GameResLight);

i3GameResLight::i3GameResLight(void)
{
	m_Type	 = I3_GAMERES_LIGHT;
}

i3GameResLight::~i3GameResLight(void)
{
	I3_SAFE_RELEASE( m_pLight);
}

void i3GameResLight::Destroy( void)
{
	I3_SAFE_RELEASE( m_pLight);
}

void	i3GameResLight::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pLight != nullptr)
	{
		m_pLight->OnBuildObjectList( List);
	}

	i3GameRes::OnBuildObjectList( List);
}

void	i3GameResLight::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pObj, method);

	i3GameResLight * pLightRes = (i3GameResLight *) pObj;

	if( getLightObject() == nullptr)
		return;

	if( method == I3_COPY_REF)
	{
		pLightRes->setLightObject( getLightObject());
	}
	else
	{
		if( pLightRes->getLightAttr() == nullptr)
		{
			i3LightObject * pNew = i3LightObject::new_object_ref();

			pLightRes->setLightObject( pNew);
		}

		getLightObject()->CopyTo( pLightRes->getLightObject(), method);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 GAMERES_LIGHT
	{
		UINT8		m_ID[4] = { 'G', 'R', 'L', '1' };
		OBJREF		m_pLight = 0;

		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3GameResLight::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::GAMERES_LIGHT data;

	result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_pLight	= pResFile->GetObjectPersistID( m_pLight);

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3GameResLight::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::GAMERES_LIGHT data;

	result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pLight != 0)
	{
		i3LightObject * pAttr = (i3LightObject *) pResFile->FindObjectByID( data.m_pLight);

		setLightObject( pAttr);
	}

	return result;
}


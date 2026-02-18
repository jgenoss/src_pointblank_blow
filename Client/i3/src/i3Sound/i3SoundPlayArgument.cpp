#include "stdafx.h"
#include "i3SoundPlayArgument.h"
#include "i3SoundPlayState.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3SoundPlayArgument);

i3SoundPlayArgument::~i3SoundPlayArgument()
{
	I3_SAFE_RELEASE( m_pProperty);

	I3_SAFE_RELEASE( m_pPlayState);
}

void i3SoundPlayArgument::setPlayState( i3SoundPlayState * pState)
{
	I3_REF_CHANGE( m_pPlayState, pState);
}

void	i3SoundPlayArgument::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	if( i3::same_of<i3SoundPlayArgument*>(pDest))
		return ;

	i3SoundPlayArgument * pArg = (i3SoundPlayArgument*)pDest;

	pArg->setVelocity( getPosition());
	pArg->setVelocity( getDirection());
	pArg->setVelocity( getVelocity());

	pArg->setSoundProperty( getSoundProperty());
	pArg->set3DMode( get3DMode());
	pArg->setLoopPlay( isLoopPlay());

	pArg->setInsideConeAngle( getInsideConeAngle());
	pArg->setOutsideConeAngle( getOutsideConeAngle());
	pArg->setOutsideVolume( getOutsideVolume());

	pArg->setUpdateInterval( getUpdateInterval());
}

void	i3SoundPlayArgument::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pProperty != nullptr)
	{
		m_pProperty->OnBuildObjectList( List);
	}

	i3NamedElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PLAYARGUMENT
	{
		UINT8				m_ID[4] = { 'S', 'P', 'A', '1' };
		UINT32				m_nVersion = 1;

		UINT32				m_bLoopPlay = 0;
		VEC3D				m_vPosition;
		VEC3D				m_vDirection;
		VEC3D				m_vVelocity;

		//Persistant members
		OBJREF64			m_pProperty = 0;

		UINT32				m_n3DMode = 0;

		INT32				m_nInsideConeAngle = 0;
		INT32				m_nOutsideConeAngle = 0;
		INT32				m_nOutsideVolume = 0;

		REAL32				m_rUpdateInterval = 0.0f;
		UINT32				pad[10] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3SoundPlayArgument::OnSave( i3ResourceFile * pResFile)
{
	UINT32 result , rc = 0;

	result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( result);

	pack::PLAYARGUMENT data;
	
	data.m_bLoopPlay	= m_bLoopPlay ? 1 : 0;

	data.m_vPosition.x = getX( &m_vPosition);
	data.m_vPosition.y = getY( &m_vPosition);
	data.m_vPosition.z = getZ( &m_vPosition);

	data.m_vDirection.x = getX( &m_vDirection);
	data.m_vDirection.y = getY( &m_vDirection);
	data.m_vDirection.z = getZ( &m_vDirection);

	data.m_vVelocity.x = getX( &m_vVelocity);
	data.m_vVelocity.y = getY( &m_vVelocity);
	data.m_vVelocity.z = getZ( &m_vVelocity);

	data.m_pProperty			= (OBJREF64)pResFile->GetObjectPersistID( m_pProperty);

	data.m_n3DMode				= m_n3DMode;
	data.m_nInsideConeAngle		= m_nInsideConeAngle;
	data.m_nOutsideConeAngle	= m_nOutsideConeAngle;
	data.m_nOutsideVolume		= (INT32)m_nOutsideVolume;

	data.m_rUpdateInterval		= m_rUpdateInterval;

	rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3SoundPlayArgument::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 result , rc = 0;

	result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( result);

	pack::PLAYARGUMENT data;
	
	rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( rc);
	result += rc;

//	if( i3String::NCompare( (char*)data.m_ID, "SPA1", 4) != 0)
	if (i3::generic_is_niequal((char*)data.m_ID, "SPA1", 4) == false)
	{
		I3TRACE("not a valid soundplayargument\n");
		return STREAM_ERR;
	}

	if( data.m_nVersion != 1)
	{
		I3TRACE("not a valid soundplayargument Version\n");
		return STREAM_ERR;
	}

	m_bLoopPlay = ( data.m_bLoopPlay > 0)? true : false;

	i3Vector::Set( &m_vPosition, data.m_vPosition.x, data.m_vPosition.y, data.m_vPosition.z);
	i3Vector::Set( &m_vDirection, data.m_vDirection.x, data.m_vDirection.y, data.m_vDirection.z);

	if( data.m_pProperty > 0)
	{
		i3SoundProperty * pProp = (i3SoundProperty *)pResFile->FindObjectByID((OBJREF) data.m_pProperty);
		I3ASSERT( pProp != nullptr);

		setSoundProperty( pProp);
	}

	set3DMode( data.m_n3DMode);
	setInsideConeAngle( data.m_nInsideConeAngle);
	setOutsideConeAngle( data.m_nOutsideConeAngle);
	setOutsideVolume( (INT8)data.m_nOutsideVolume);

	setUpdateInterval( data.m_rUpdateInterval);
	
	return result;
}
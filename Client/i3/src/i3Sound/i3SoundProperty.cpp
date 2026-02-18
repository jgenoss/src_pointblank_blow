#include "stdafx.h"
#include "i3SoundProperty.h"

I3_CLASS_INSTANCE( i3SoundProperty);

i3SoundProperty::~i3SoundProperty()
{
	I3_SAFE_RELEASE( m_pSourceSound);
}

void	i3SoundProperty::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	if( i3::same_of<i3SoundProperty*>(pDest))
		return;

	i3SoundProperty * pInfo = (i3SoundProperty*)pDest;

	pInfo->setSourceSound( getSourceSound());
	pInfo->setSoundType( getSoundType());
	pInfo->setStreamming( isStream());
	pInfo->setSourceVolume( getSourceVolume());
	pInfo->setListenerVolume( getListenerVolume());
	pInfo->setPositionalVolume( getPositionalVolume());
	pInfo->setPriorityType( getPriorityType());
	pInfo->setDistanceInfo( getMaxDistance(), getMinDistance(), isMuteAtMaxDistance());
}

void	i3SoundProperty::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pSourceSound != nullptr)
	{
		m_pSourceSound->OnBuildObjectList( List);
	}

	i3ResourceObject::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4	PROPERTY
	{
		UINT8				m_ID[8] = { 'S', 'P', 'I', '2' };
		UINT32				m_nVersion = 1;
		OBJREF64			m_pSourceSound = 0;
		UINT32				m_nSoundType = 0;
		INT32				m_bStream = 0;

		REAL32				m_rMinDistance = 0.0f;
		REAL32				m_rMaxDistance = 0.0f;
		INT32				m_bMuteAtMaxDistance = 0;

		INT32				m_nSourceVolume = 0;			// 0 ~ 100
		INT32				m_nListenerVolume = 0;			// 0 ~ 100%
		INT32				m_nPositionalVolume = 0;		// 0 ~ 100%

		UINT32				m_nPriorityType = 0;
		INT32				m_nPriorityLevel = 0;
		UINT32				pad[20] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3SoundProperty::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;

	result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( result);

	pack::PROPERTY data;

	data.m_pSourceSound			= (OBJREF64)pResFile->GetObjectPersistID( m_pSourceSound);
	data.m_bStream				= (INT32)m_bStream;
	data.m_rMinDistance			= m_rMinDistance;
	data.m_rMaxDistance			= m_rMaxDistance;
	data.m_bMuteAtMaxDistance	= (INT32)m_bMuteAtMaxDistance;
	data.m_nSourceVolume		= (INT32)m_nSourceVolume;
	data.m_nListenerVolume		= (INT32)m_nListenerVolume;
	data.m_nPositionalVolume	= (INT32)m_nPositionalVolume;

	data.m_nPriorityType		= m_nPriorityType;
	data.m_nPriorityLevel		= m_nPriorityLevel;

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3SoundProperty::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;

	result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( result);

	pack::PROPERTY data;
	rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_nVersion != 1)
		return STREAM_ERR;

	if( data.m_pSourceSound	!= 0)
	{
		i3Sound * pSourceSound = (i3Sound*)pResFile->FindObjectByID( (OBJREF)data.m_pSourceSound);
		I3ASSERT( pSourceSound != nullptr);

		setSourceSound( pSourceSound);
	}

	setStreamming( (data.m_bStream > 0) ? true : false);

	setDistanceInfo( data.m_rMaxDistance, data.m_rMinDistance, ((data.m_bMuteAtMaxDistance > 0) ? true : false));
	setSourceVolume( (UINT8)data.m_nSourceVolume);
	setListenerVolume( (UINT8)data.m_nListenerVolume);
	setPositionalVolume( (UINT8)data.m_nPositionalVolume);

	setPriorityType( data.m_nPriorityType);
	setPriorityLevel( data.m_nPriorityLevel);

	return result;
}

/*
UINT32 i3SoundProperty::OnLoad0( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;

	I3SND_PERSIST_PLAY_INFO data;
	result = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( result);

	if( data.m_pSourceSound != 0)
	{
		i3Sound * pSound = (i3Sound*)pResFile->FindObjectByID( data.m_pSourceSound);
		I3ASSERT( pSound != nullptr);

		setSourceSound( pSound);
	}

	setDistanceInfo( data.m_rMaxDistance, data.m_rMinDistance, true);

	setStreamming( false);
	
	setSourceVolume( data.m_nDefaultVolume);
	setListenerVolume( 100);
	setPositionalVolume( 100);

	setPriorityType( 0);
	setPriorityLevel( 0);

	return Result;
}
*/
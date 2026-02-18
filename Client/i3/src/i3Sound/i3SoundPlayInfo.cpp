#include "stdafx.h"
#include "i3SoundPlayInfo.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3SoundPlayInfo);

i3SoundPlayInfo::~i3SoundPlayInfo()
{
	I3_SAFE_RELEASE( m_pSound);
}

void i3SoundPlayInfo::setSound( i3Sound * pSound)
{
	I3_REF_CHANGE( m_pSound, pSound);
}

void i3SoundPlayInfo::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	i3SoundPlayInfo * pSnd = (i3SoundPlayInfo *) pDest;

	pSnd->setSound( getSound());
	pSnd->SetStyle( GetStyle());
	pSnd->AddStyle( GetLoopMode());
	pSnd->SetDefaultVolume( GetDefaultVolume());
	pSnd->setListenerVolume( getListenerVolume());
	pSnd->SetPriorityLevel( GetPriorityLevel());
	pSnd->SetDistanceInfo( m_rMaxDistance, m_rMinDistance, m_nVolType);

	pSnd->setMuteAtMaxDistance( isMuteAtMaxDistance());
	pSnd->setStopOnReset( isStopOnReset());
	pSnd->setPosition( getPosition());
	pSnd->setDirection( getDirection());
	pSnd->setVelocity( getVelocity());

	pSnd->setInsideConeAngle( getInsideConeAngle());
	pSnd->setOutsideConeAngle( getOutsideConeAngle());
	pSnd->setOutsideVolume( getOutsideVolume());

	pSnd->setUpdateInterval( getUpdateInterval());

	pSnd->setFrequency( getFrequency());
	pSnd->ApplyI3DL2ReverbPreset( getI3DL2ReverbPreset());
}

void i3SoundPlayInfo::SetDefaultVolume(INT32 nVolume)
{
	m_nDefaultVolume = nVolume;
}

void i3SoundPlayInfo::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pSound != nullptr)
		m_pSound->OnBuildObjectList( List);

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8	PLAY_INFO
	{
		UINT32	m_nStyle = 0;
		INT32	m_nDefaultVolume = 0;
		INT32	m_nPriorityLevel = 0;
		OBJREF	m_pSound = 0;

		REAL32	m_rMinDistance = 0.0f;
		REAL32	m_rMaxDistance = 0.0f;
		INT32	m_nVolType = 0;
		INT32	m_nListenerVolume = 0;
		UINT32	pad[2] = { 0 };
	};

	struct ALIGN1 PLAY_INFO_2
	{
		UINT8			nID[4] = { 'S', 'P', 'I', '1' };
		UINT8			nVersion = 2;
		UINT8			bMuteAtMaxDistance = 0;
		UINT8			bStopOnReset = 0;
		UINT8			pad1 = 0;

		UINT32			nStyle = 0;
		OBJREF64		pSound = 0;
		INT32			nDefaultVolume = 0;
		INT32			nListenerVolume = 0;
		INT32			nPriorityLevel = 0;

		REAL32			rMinDistance = 0.0f;
		REAL32			rMaxDistance = 0.0f;
		INT32			nVolType = 0;

		i3::pack::VEC3D	vPosition;
		i3::pack::VEC3D	vDirection;
		i3::pack::VEC3D	vVelocity;

		INT32			nInsideConeAngle = 0;
		INT32			nOutsideConeAngle = 0;
		INT32			nOutsideVolume = 0;

		REAL32			rUpdateInterval = 0.0f;
		INT32			nFrequency = 0;
		INT32			nI3DL2RvbPreset = 0;

		UINT32			pad2[6] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3SoundPlayInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::PLAY_INFO_2 data;
	
	data.nStyle				= m_nStyle;
	data.nDefaultVolume		= m_nDefaultVolume;
	data.nListenerVolume	= m_nListenerVolume;
	data.nPriorityLevel		= m_nPriorityLevel;

	data.bMuteAtMaxDistance	= (UINT8)( m_bMuteAtMaxDistance ) ? 1 : 0;
	data.bStopOnReset		= (UINT8)( m_bStopOnReset) ? 1 : 0;
	data.pSound				= (OBJREF64)pResFile->GetObjectPersistID( m_pSound);

	data.rMinDistance		= m_rMinDistance;
	data.rMaxDistance		= m_rMaxDistance;
	data.nVolType			= m_nVolType;

	data.vPosition.x		= getX( getPosition());
	data.vPosition.y		= getY( getPosition());
	data.vPosition.z		= getZ( getPosition());

	data.vDirection.x		= getX( getDirection());
	data.vDirection.y		= getY( getDirection());
	data.vDirection.z		= getZ( getDirection());

	data.vVelocity.x		= getX( getVelocity());
	data.vVelocity.y		= getY( getVelocity());
	data.vVelocity.z		= getZ( getVelocity());

	data.nInsideConeAngle	= m_nInsideConeAngle;
	data.nOutsideConeAngle	= m_nOutsideConeAngle;
	data.nOutsideVolume		= m_nOutsideVolume;

	data.rUpdateInterval	= m_rUpdateInterval;
	data.nFrequency			= m_nFrequency;
	data.nI3DL2RvbPreset	= m_nI3DL2RvbPreset;

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3SoundPlayInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	UINT32	nStreamPos = pStream->GetPosition();

	UINT8 nID[4];
	Rc = pResFile->Read( nID, 4);
	I3_CHKIO( Rc);

	INT32 nVersion = 1;
//	if( i3String::NCompare( (char*)nID, "SPI1", 4) == 0)
	if (i3::generic_is_niequal((char*)nID, "SPI1", 4) )
	{
		nVersion = 2;
	}

	pStream->SetPosition( nStreamPos, STREAM_BEGIN);

	if( nVersion == 1)
	{
		pack::PLAY_INFO data;

		Result = pStream->Read( &data, sizeof(data));
		I3_CHKIO( Result);

		m_nStyle			= data.m_nStyle;
		m_nDefaultVolume	= data.m_nDefaultVolume;
		m_nPriorityLevel	= data.m_nPriorityLevel;
		m_rMinDistance		= data.m_rMinDistance;
		m_rMaxDistance		= data.m_rMaxDistance;
		m_nVolType			= data.m_nVolType;
		m_nListenerVolume	= data.m_nListenerVolume;

		if( data.m_pSound != 0)
		{
			i3Sound * pSound = (i3Sound*)pResFile->FindObjectByID( data.m_pSound);
			I3ASSERT( pSound != nullptr);

			setSound( pSound);
		}
	}
	else if( nVersion == 2)
	{
		pack::PLAY_INFO_2 data;

		Result = pResFile->Read( &data, sizeof( data));
		I3_CHKIO( Result);

		I3ASSERT( data.nVersion == 2);

		m_nStyle				= data.nStyle;
		m_nDefaultVolume		= data.nDefaultVolume;
		m_nListenerVolume		= data.nListenerVolume;
		m_nPriorityLevel		= data.nPriorityLevel;
															
		m_bMuteAtMaxDistance	= ( data.bMuteAtMaxDistance >= 1) ? true : false;
		m_bStopOnReset			= ( data.bStopOnReset >= 1) ? true : false;
		if( data.pSound != 0)
		{
			i3Sound * pSound = (i3Sound*)pResFile->FindObjectByID( (OBJREF)data.pSound);
			if( pSound != nullptr)
				setSound( pSound);
			else
				m_SoundRef = data.pSound;
		}

		SetDistanceInfo( data.rMaxDistance, data.rMinDistance, data.nVolType);

		i3Vector::Set( &m_vPosition, data.vPosition.x, data.vPosition.y, data.vPosition.z);
		i3Vector::Set( &m_vDirection, data.vDirection.x, data.vDirection.y, data.vDirection.z);
		i3Vector::Set( &m_vVelocity, data.vVelocity.x, data.vVelocity.y, data.vVelocity.z);
															
		m_nInsideConeAngle	=	data.nInsideConeAngle;
		m_nOutsideConeAngle	=	data.nOutsideConeAngle;
		m_nOutsideVolume	=	(UINT8)data.nOutsideVolume;		
															
		m_rUpdateInterval	=	data.rUpdateInterval;	
		m_nFrequency		=	data.nFrequency;			
		m_nI3DL2RvbPreset	=	data.nI3DL2RvbPreset;	
	}

	return Result;
}

bool i3SoundPlayInfo::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr( "Version", 2);
	
	pXML->addAttr( "Style", m_nStyle);
	pXML->addAttr( "DefaultVolume", m_nDefaultVolume);
	pXML->addAttr( "ListenerVolume", m_nListenerVolume);
	pXML->addAttr( "PriorityLevel", m_nPriorityLevel);

	pXML->addAttr( "MuteAtMaxDistance", (INT32)m_bMuteAtMaxDistance);
	pXML->addAttr( "StopOnReset",	(INT32)m_bStopOnReset);

	OBJREF64 iVal = (OBJREF64)pFile->GetObjectPersistID(m_pSound);
	pXML->addAttr("Sound", iVal);
	pXML->addAttr("MinDist", m_rMinDistance);
	pXML->addAttr("MaxDist", m_rMaxDistance);
	pXML->addAttr("VolType", m_nVolType);

	i3Vector::SetToXML( pXML, "Position", getPosition());
	i3Vector::SetToXML( pXML, "Direction", getDirection());
	i3Vector::SetToXML( pXML, "Velocity", getVelocity());

	pXML->addAttr("InsideConeAngle", m_nInsideConeAngle);
	pXML->addAttr("OutsideConeAngle", m_nOutsideConeAngle);
	pXML->addAttr("OutsideVolume", m_nOutsideVolume);

	pXML->addAttr("UpdateInterval", m_rUpdateInterval);
	pXML->addAttr("Frequency", m_nFrequency);
	pXML->addAttr("nI3DL2RvbPreset", m_nI3DL2RvbPreset);

	return true;
}

bool i3SoundPlayInfo::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 version;

	pXML->getAttr( "Version", &version);
	
	pXML->getAttr( "Style", &m_nStyle);
	pXML->getAttr( "DefaultVolume", &m_nDefaultVolume);
	pXML->getAttr( "ListenerVolume", &m_nListenerVolume);
	pXML->getAttr( "PriorityLevel", &m_nPriorityLevel);

	INT32 muteatmax;
	pXML->getAttr( "MuteAtMaxDistance", &muteatmax);

	m_bMuteAtMaxDistance = ( muteatmax > 0)?  true : false ;

	INT32 stoponReset;
	pXML->getAttr( "StopOnReset", &stoponReset);

	m_bStopOnReset = ( stoponReset > 0)?  true : false ;
	OBJREF64 temp;
	pXML->getAttr("Sound", &temp);
	if(temp > 0)
	{
		i3Sound * pSound = (i3Sound*)pFile->FindObjectByID((OBJREF)temp);
		I3ASSERT( pSound != nullptr);

		setSound( pSound);
	}
	
	pXML->getAttr("MinDist", &m_rMinDistance);
	pXML->getAttr("MaxDist", &m_rMaxDistance);
	pXML->getAttr("VolType", &m_nVolType);

	i3Vector::GetFromXML( pXML, "Position", getPosition());
	i3Vector::GetFromXML( pXML, "Direction", getDirection());
	i3Vector::GetFromXML( pXML, "Velocity", getVelocity());

	pXML->getAttr("InsideConeAngle", &m_nInsideConeAngle);
	pXML->getAttr("OutsideConeAngle", &m_nOutsideConeAngle);

	INT32 vol;
	pXML->getAttr("OutsideVolume", &vol);
	m_nOutsideVolume = (UINT8)vol;
	
	pXML->getAttr("UpdateInterval", &m_rUpdateInterval);
	pXML->getAttr("Frequency", &m_nFrequency);
	pXML->getAttr("nI3DL2RvbPreset", &m_nI3DL2RvbPreset);

	return true;
}

void i3SoundPlayInfo::LoadFromBackground( i3ResourceFile * pFile)
{
	if( m_SoundRef != 0)
	{
		INT32 idx = pFile->FindObjectInfoByID( (OBJREF) m_SoundRef);
		if( idx != -1)
		{
			i3Sound * pSound = (i3Sound*) pFile->ResolveExtern( idx);
			pSound->OnInitAfterLoad();
			setSound( pSound);
		}
	}
}


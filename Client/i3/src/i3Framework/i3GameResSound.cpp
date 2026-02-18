#include "i3FrameworkPCH.h"
#include "i3GameResSound.h"
#include "i3GameNode.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/i3RSGHelper.h"

I3_CLASS_INSTANCE( i3GameResSound);

i3GameResSound::i3GameResSound(void)
{
	m_Type	= I3_GAMERES_SOUND;
}

i3GameResSound::~i3GameResSound(void)
{
	I3_SAFE_RELEASE( m_pSound);
}

void i3GameResSound::Destroy( void)
{
	if( m_pSound != nullptr)
	{
		m_pSound->setSound( nullptr);
	}
}

void i3GameResSound::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pDest, method);

	if( !i3::same_of<i3GameResSound* >(pDest))
		return;

	i3GameResSound * pDestSnd = (i3GameResSound *) pDest;

	if( method == I3_COPY_REF)
	{
		pDestSnd->setSound( getSound());
	}
	else
	{
		I3ASSERT( getSound() != nullptr);

		i3SoundPlayInfo * pInfo = i3SoundPlayInfo::new_object_ref();
		getSound()->CopyTo( pInfo, method);

		pDestSnd->setSound( pInfo);
	}
}

void	i3GameResSound::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( isExternal() == false)
	{
		if( m_pSound != nullptr)
		{
			m_pSound->OnBuildObjectList( List);
		}
	}

	i3GameRes::OnBuildObjectList( List);
}

UINT32 i3GameResSound::SaveToFile( i3Stream * pStream)
{
	UINT32 Result, Rc;

	Result = i3GameRes::SaveToFile( pStream);

	i3::pack::RSC_SOUND sound;
	i3mem::FillZero( &sound, sizeof( sound));

	{
		sound.m_rMaxDist = m_pSound->getMaxDistance();
		sound.m_rMinDist = m_pSound->getMinDistance();

		sound.m_nDefaultVolume  = m_pSound->GetDefaultVolume();
		sound.m_nListenerVolume = m_pSound->getListenerVolume();
	}

	Rc = pStream->Write( &sound, sizeof( sound));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GameResSound::LoadFromFile( i3Stream * pStream, INT32 nVersion)
{
	UINT32 nResult = 0;

	i3::pack::RSC_SOUND ResSnd;

	nResult = pStream->Read( &ResSnd, sizeof( ResSnd));
	I3_CHKIO( nResult);

	i3SoundPlayInfo * pSndInfo = i3SoundPlayInfo::new_object_ref();

	pSndInfo->SetDistanceInfo( ResSnd.m_rMaxDist, ResSnd.m_rMinDist, 0);
	pSndInfo->SetDefaultVolume( ResSnd.m_nDefaultVolume);
	pSndInfo->setListenerVolume( ResSnd.m_nListenerVolume);

	setSound( pSndInfo);

	return nResult;
}

UINT32 i3GameResSound::LoadFromFile(i3IniParser * pParser, INT32 nVersion, INT32 nType)
{
	UINT32 nResult = 0;

	i3::pack::RSC_SOUND ResSnd;

	//nResult = pStream->Read(&ResSnd, sizeof(ResSnd));
	//I3_CHKIO(nResult);

	i3::vector<i3::any>	_FieldData;
	_FieldData.clear();
	RSGParser(pParser, nType, _FieldData);
	ResSnd.m_rMaxDist = *i3::unsafe_any_cast<REAL32>(&_FieldData[1]);
	ResSnd.m_rMinDist = *i3::unsafe_any_cast<REAL32>(&_FieldData[2]);
	ResSnd.m_nDefaultVolume = *i3::unsafe_any_cast<INT32>(&_FieldData[3]);
	ResSnd.m_nListenerVolume = *i3::unsafe_any_cast<INT32>(&_FieldData[4]);

	i3SoundPlayInfo * pSndInfo = i3SoundPlayInfo::new_object_ref();

	pSndInfo->SetDistanceInfo(ResSnd.m_rMaxDist, ResSnd.m_rMinDist, 0);
	pSndInfo->SetDefaultVolume(ResSnd.m_nDefaultVolume);
	pSndInfo->setListenerVolume(ResSnd.m_nListenerVolume);

	setSound(pSndInfo);

	return nResult;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 GAMERES_SOUND
	{
		UINT8		m_ID[4] = { 'R', 'S', 'N', '1' };
		OBJREF64	m_pSound = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3GameResSound::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_SOUND data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_pSound = (OBJREF64) pResFile->GetObjectPersistID( getSound());

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResSound::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_SOUND data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( isExternal())
	{
	}
	else
	{
		if( data.m_pSound != 0)
		{
			i3SoundPlayInfo * pInfo = (i3SoundPlayInfo *) pResFile->FindObjectByID( (OBJREF) data.m_pSound);
			I3ASSERT( pInfo != nullptr);

			setSound( pInfo);
		}
	}

	return Result;
}

#include "stdafx.h"
#include "i3LevelResAnimation.h"

I3_CLASS_INSTANCE( i3LevelResAnimation);

i3LevelResAnimation::i3LevelResAnimation()
{
	m_ResType		= I3LV_TYPE_ANIMATION;
}

i3LevelResAnimation::~i3LevelResAnimation()
{
	I3_SAFE_RELEASE( m_pAnimation);
}

void i3LevelResAnimation::_UpdateInfo( i3Animation * pAnim)
{
	if( pAnim == nullptr)
	{
		m_TrackCount	= 0;
		m_Duration		= 0.0f;
	}
	else
	{
		m_TrackCount	= pAnim->GetTrackCount();
		m_Duration		= pAnim->GetDuration();
	}
}

bool i3LevelResAnimation::OnValidateBrief( bool bForce)
{
	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;
	}

	char szFull[ MAX_PATH];

	getFullPath( szFull);

	i3Animation * pAnimation = i3Animation::LoadFromFile( szFull);

	if( pAnimation == nullptr)
	{
		return false;
	}

//	pAnimation->AddRef();		// i3Animation::LoadFromFile()에 AddRef()들어있어서 여기서는 주석처리..(2012.09.11.수빈)

	_UpdateInfo( pAnimation);
	
	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	I3_MUST_RELEASE(pAnimation);

	return true;
}

bool i3LevelResAnimation::OnValidate( i3LevelScene * pScene, bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	char szFull[ MAX_PATH];

	getFullPath( szFull);

	BeginValidate();

		i3Animation * pAnimation = i3Animation::LoadFromFile( szFull);

		I3_REF_CHANGE( m_pAnimation, pAnimation);

	EndValidate();

	if( m_pAnimation == nullptr)
		return false;

//	m_pAnimation->AddRef();		// i3Animation::LoadFromFile()에 AddRef()들어있어서 여기서는 주석처리..(2012.09.11.수빈)
	m_pAnimation->SetExternRes( TRUE);

	_UpdateInfo( m_pAnimation);

	addResState( I3LV_RES_STATE_LOADED);
	
	return true;
}

i3GameRes * i3LevelResAnimation::CreateGameRes(void)
{
	i3GameResAnimation * pRes;

	pRes = i3GameResAnimation::new_object();
	pRes->setAnimation( m_pAnimation);

	return pRes;
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)			// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct RES_ANIMATION
	{
		UINT8		m_ID[4] = { 'R', 'S', 'A', '1' };
		INT32		m_TrackCount = 0;
		REAL32		m_Duration = 0.0f;
		UINT32		pad[8] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelResAnimation::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::RES_ANIMATION	data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_TrackCount	= m_TrackCount;
	data.m_Duration		= m_Duration;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}
	
UINT32 i3LevelResAnimation::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::RES_ANIMATION data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Result);
	Result += Rc;

	m_TrackCount	= data.m_TrackCount;
	m_Duration		= data.m_Duration;

	return Result;
}

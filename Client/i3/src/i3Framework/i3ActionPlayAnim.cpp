#include "i3FrameworkPCH.h"
#include "i3ActionPlayAnim.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionPlayAnim);


i3ActionPlayAnim::~i3ActionPlayAnim(void)
{
	I3_SAFE_RELEASE( m_pAnim);
}

void i3ActionPlayAnim::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionPlayAnim * pAction = (i3ActionPlayAnim *) pDest;

	pAction->setAnim( getAnim());
	pAction->setPlayMode( getPlayMode());
}

void i3ActionPlayAnim::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Play Animation", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Object 및 Character의 Animation을 설정합니다.", len);
	}
}

void	i3ActionPlayAnim::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pAnim != nullptr)
	{
		m_pAnim->OnBuildObjectList( List);
	}

	i3Action::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_PLAY_ANIM
	{
		UINT8	m_ID[4] = { 'A', 'P', 'A', 1 };
		OBJREF	m_pAnim = 0;
		UINT8	m_PlayMode = 0;
		UINT8	pad0[3] = { 0 };
		UINT32	pad[8] = { 0 };
	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)			// [test required] 패킹 팝 누락으로 추가합니다. 2017.02.23. soon9
#endif

UINT32 i3ActionPlayAnim::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_PLAY_ANIM data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_pAnim		= pResFile->GetObjectPersistID( m_pAnim);
	data.m_PlayMode		= (UINT8) getPlayMode();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionPlayAnim::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_PLAY_ANIM data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pAnim != 0)
	{
		i3Animation * pAnim = (i3Animation *) pResFile->FindObjectByID( data.m_pAnim);

		setAnim( pAnim);
	}

	setPlayMode( (I3_ANIM_PLAY_MODE) data.m_PlayMode);

	return result;
}

#include "i3FrameworkPCH.h"
#include "i3ActionPath.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionPath);


i3ActionPath::~i3ActionPath(void)
{
	I3_SAFE_RELEASE( m_pAnim);
}

void i3ActionPath::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionPath * pAction = (i3ActionPath *) pDest;

	pAction->setAnim( getAnim());
	pAction->setPlayMode( getPlayMode());
}

void i3ActionPath::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Path Animation", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Object 및 Character의 이동 경로를 설정합니다. Animation에 의해서 Control됩니다.", len);
	}
}

void	i3ActionPath::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
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
	struct ALIGN4 ACTION_PATH
	{
		UINT8	m_ID[4] = { 'A', 'P', 'T', '1' };
		OBJREF	m_pAnim = 0;
		UINT8	m_PlayMode = 0;
		UINT8	pad0[3] = { 0 };
		UINT32	pad[3] = { 0 };
	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)						// [test required] 패킹 팝 누락으로 추가합니다. 2017.02.23. soon9
#endif

UINT32 i3ActionPath::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_PATH data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_pAnim		= pResFile->GetObjectPersistID( m_pAnim);
	data.m_PlayMode		= (UINT8) getPlayMode();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionPath::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_PATH data;

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

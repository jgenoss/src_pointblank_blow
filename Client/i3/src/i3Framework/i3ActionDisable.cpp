#include "i3FrameworkPCH.h"
#include "i3ActionDisable.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionDisable);

void i3ActionDisable::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Disable", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "눈에 보이지않음, 업데이트하지 않음 상태로 설정합니다", len);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct ACTION_DISABLE
	{
		UINT8	m_ID[4] = { 'I', 'A', 'D', '1' };
		UINT32	m_bEnable = 0;
		REAL32	m_timeFade = 0.0f;
		UINT32  pad[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ActionDisable::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::ACTION_DISABLE data;

	Result = i3Action::OnSave( pResFile);
	
	data.m_bEnable			= (UINT32) (m_bEnable == true);
	data.m_timeFade			= getFadeTime();

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3ActionDisable::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::ACTION_DISABLE data;

	Result = i3Action::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_bEnable = (data.m_bEnable != 0);
	m_timeFade = data.m_timeFade;

	return Result;
}
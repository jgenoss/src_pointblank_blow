#include "i3FrameworkPCH.h"
#include "i3ActionVisible.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionVisible);


void i3ActionVisible::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Show/Hide", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "보이거나 보이지 않음, 업데이트 함 상태로 설정합니다.", len);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_VISIBLE
	{
		UINT8		m_ID[4] = { 'A', 'V', 'C', '1' };
		UINT32		m_bShow = 0;
		REAL32		m_rAlphaTime = 0.0f;
		UINT32		pad = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ActionVisible::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_VISIBLE data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_bShow = (UINT32) m_bShow;
	data.m_rAlphaTime = (REAL32) m_rAlphaTime;
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionVisible::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_VISIBLE data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setEnable( (data.m_bShow != 0));
	setAlphaTime( data.m_rAlphaTime );

	return result;
}

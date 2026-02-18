#include "i3FrameworkPCH.h"
#include "i3ActionDynamicState.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionDynamicState);

void i3ActionDynamicState::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionDynamicState * pEffect = (i3ActionDynamicState *) pDest;

	pEffect->setState( getState());
}

void i3ActionDynamicState::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Dynamic State 변경", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Object의 Dynamic State를 변경합니다.\r\nDynamic : 충돌 및 물리적 반응을 합니다.\r\nKinematic : Programming 또는 Animation에 의해서만 움직일 수 있으며, 다른 Object와 충돌은 가능하지만 반응해 움직이지 않습니다.\r\nStatic : 전혀 움직이지 않는 경우.", len);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_DYNAMICSTATE
	{
		UINT8	m_ID[4] = { 'A', 'D', 'S', '1' };
		UINT32	m_State = 0;
		UINT32	pad[4] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3ActionDynamicState::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_DYNAMICSTATE data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_State = (UINT32) getState();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionDynamicState::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_DYNAMICSTATE data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setState( (I3GAME_DYNAMIC_STATE) data.m_State);

	return result;
}

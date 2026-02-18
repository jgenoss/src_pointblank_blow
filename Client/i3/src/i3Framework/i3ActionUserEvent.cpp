#include "i3FrameworkPCH.h"
#include "i3ActionUserEvent.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

//I3_CLASS_INSTANCE( i3ActionUserEvent, i3Action);
I3_CLASS_INSTANCE( i3ActionUserEvent);

i3ActionUserEvent::i3ActionUserEvent(void)
{
	addStyle( I3_ACTION_STYLE_NEEDCOPYINST);
}

void i3ActionUserEvent::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "사용자-정의 이벤트", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "사용자-정의 이벤트를 발생시킵니다. 게임마다 각 이벤트에 대한 처리는 달라질 수 있습니다.", len);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_USER_EVENT
	{
		UINT8		m_ID[4] = { 'A', 'U', 'E', '1' };
		INT32		m_EventID = 0;
		INT32		m_Param1 = 0;
		INT32		m_Param2 = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack( pop)			// [test required] 패킹 팝 누락으로 추가합니다. 2017.02.23. soon9
#endif

UINT32 i3ActionUserEvent::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_USER_EVENT data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_EventID	= getEventID();
	data.m_Param1	= getParam1();
	data.m_Param2	= getParam2();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionUserEvent::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_USER_EVENT data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setEventID( data.m_EventID);
	setParam1( data.m_Param1);
	setParam2( data.m_Param2);

	return result;
}
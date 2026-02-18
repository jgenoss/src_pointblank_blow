#include "i3FrameworkPCH.h"
#include "i3ActionBreakObject.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionBreakObject);


void i3ActionBreakObject::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Object ÆÄ±«", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Object¸¦ ÆÄ±«½ÃÅµ´Ï´Ù.", len);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_BREAKOBJECT
	{
		UINT8		m_ID[4] = { 'A', 'B', 'O', '1' };
		REAL32		m_Damage = 0.0f;
		UINT32		pad[2] = { 0 };
	};
}

#if defined (I3_COMPILER_VC)&& defined(PACKING_RIGHT)
#pragma pack(pop)	// [test required] ÆÐÅ· ÆË ´©¶ôÀ¸·Î Ãß°¡ÇÕ´Ï´Ù. 2017.02.23. soon9
#endif

UINT32 i3ActionBreakObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_BREAKOBJECT data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_Damage = getDamage();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionBreakObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_BREAKOBJECT data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setDamage( data.m_Damage);

	return result;
}
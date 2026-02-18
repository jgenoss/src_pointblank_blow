#include "stdafx.h"
#include "i3LevelParserHelper.h"

BOOL GetBOOL( const char * pszStr)
{
	if( i3::generic_is_iequal( pszStr, "TRUE") )
		return TRUE;

	if( i3::generic_is_iequal( pszStr, "1") )
		return TRUE;

	if( i3::generic_is_iequal( pszStr, "T") )
		return TRUE;

	if( i3::generic_is_iequal( pszStr, "On") )
		return TRUE;

	return FALSE;
}

I3G_FOG_MODE GetFogMode( const char * pszStr)
{
	if( i3::generic_is_iequal( pszStr, "Linear"))
		return I3G_FOG_LINEAR;

	if( i3::generic_is_iequal( pszStr, "Exp"))
		return I3G_FOG_EXP;

	if( i3::generic_is_iequal( pszStr, "Exp2"))
		return I3G_FOG_EXP2;

	return I3G_FOG_NONE;
}

static char s_szBool[2][10] =
{
	"False",
	"True"
};

char * BOOL2Str( BOOL bValue)
{
	if( bValue)
		return s_szBool[1];

	return s_szBool[0];
}

static char s_szFogMode[3][10] =
{
	"Linear",
	"Exp",
	"Exp2"
};

char * FogMode2Str( I3G_FOG_MODE mode)
{
	switch( mode)
	{
		case I3G_FOG_LINEAR :		return s_szFogMode[0];	break;
		case I3G_FOG_EXP :			return s_szFogMode[1];	break;
	}

	return s_szFogMode[2];
}
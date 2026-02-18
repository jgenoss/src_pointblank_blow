#include "stdafx.h"
#include "i3LevelDefine.h"

static const char *		s_SpaceName[ I3LV_SPACE::SPACE_COUNT] =
{
	"World Space",
	"Local Space"
};

const char * I3LV_SPACE::getSpaceString( INT32 idx)
{
	I3_BOUNDCHK( idx, SPACE_COUNT);

	return s_SpaceName[ idx];
}

I3LV_SPACE::TYPE I3LV_SPACE::getSpaceByString( const char * pszString)
{
	INT32 i;

	for( i = 0; i < SPACE_COUNT; i++)
	{
		if( strcmp( s_SpaceName[i], pszString) == 0)
			return (TYPE) i;
	}

	return NA;
}

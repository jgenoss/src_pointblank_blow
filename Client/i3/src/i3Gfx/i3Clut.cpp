#include "i3GfxType.h"
#include "i3Clut.h"

INT32	i3Clut::FindNearestColor( COLOR * pColor)
{
	INT32 i, idx = 0;
	INT32 accm, minaccm = 0;
	COLOR col;

	for( i = 0; i < GetColorCount(); i++)
	{
		GetColor( i, &col);

		accm = abs( (INT32) i3Color::GetR( pColor) - i3Color::GetR( &col));
		accm += abs( (INT32) i3Color::GetG( pColor) - i3Color::GetG( &col));
		accm += abs( (INT32) i3Color::GetB( pColor) - i3Color::GetB( &col));
		accm += abs( (INT32) i3Color::GetA( pColor) - i3Color::GetA( &col)) << 8;		// weight to alpha
	
		if( i == 0)
			minaccm = accm;
		else
		{
			if( minaccm > accm)
			{
				minaccm = accm;
				idx = i;
			}
		}
	}

	return i;
}

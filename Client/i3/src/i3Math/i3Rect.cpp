#include "i3MathType.h"
#include "i3Rect.h"

TOUCH_TYPE	i3Rect::canContain( i3Rect * pSmall)
{
	INT32	diff[4], whole;

	diff[0] = pSmall->left - left;
	diff[1] = pSmall->top - top;
	diff[2] = right - pSmall->right;
	diff[3] = bottom - pSmall->bottom;

	whole = diff[0] | diff[1] | diff[2] | diff[3];

	if( (whole & 0x80000000) == 0)		// 모두 양수 또는 0이다.
		return TOUCH_WHOLE;

	if(( pSmall->left < left) && (pSmall->right < left))
		return TOUCH_NONE;

	if(( pSmall->left > right) && ( pSmall->right > right))
		return TOUCH_NONE;

	if(( pSmall->top < top) && ( pSmall->bottom < top))
		return TOUCH_NONE;

	if(( pSmall->top > bottom) && (pSmall->bottom > bottom))
		return TOUCH_NONE;

	return TOUCH_PARTIAL;
}

TOUCH_TYPE i3Rect::Intersect( i3Rect * pSmall, i3Rect * pIntersect)
{
	TOUCH_TYPE type;

	type = canContain( pSmall);

	if( type == TOUCH_NONE)
		return TOUCH_NONE;

	if( type == TOUCH_WHOLE)
	{
		pIntersect->left = pSmall->left;
		pIntersect->top  = pSmall->top;
		pIntersect->right = pSmall->right;
		pIntersect->bottom = pSmall->bottom;

		return TOUCH_WHOLE;
	}

	pIntersect->left	= MAX( pSmall->left, left);
	pIntersect->top		= MAX( pSmall->top, top);
	pIntersect->right	= MIN( pSmall->right, right);
	pIntersect->bottom	= MIN( pSmall->bottom, bottom);

	return TOUCH_PARTIAL;
}

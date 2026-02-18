#if !defined( __I3_RECT_H)
#define __I3_RECT_H

#include "i3MathUtil.h"

class I3_EXPORT_MATH i3Rect
{
public:
	INT32		left;
	INT32		top;
	INT32		right;
	INT32		bottom;

public:
	void		set( INT32 l, INT32 t, INT32 r, INT32 b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	const INT32		getWidth(void)						{ return (right - left) + 1; }
	const INT32		getHeight(void)						{ return (bottom - top) + 1; }

	bool		canContain( INT32 w, INT32 h)		{ return ( getWidth() >= w) && (getHeight() >= h); }
	bool		isSameSpan( INT32 w, INT32 h)		{ return ( getWidth() == w) && (getHeight() == h); }
	bool		isInsidePt( INT32 x, INT32 y)
	{
		if(( left <= x) && (right >= x) && (top <= y) && (bottom >= y))
			return true;

		return false;
	}

	void		Offset( INT32 x, INT32 y)
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
	}

	TOUCH_TYPE		canContain( i3Rect * pSmall);
	TOUCH_TYPE		Intersect( i3Rect * pSmall, i3Rect * pIntersect);
};

#endif

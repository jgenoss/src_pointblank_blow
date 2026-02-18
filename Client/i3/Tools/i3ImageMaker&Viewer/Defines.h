#if !defined( __DEFINES_H)
#define __DEFINES_H

#include <d3dx9tex.h>

namespace tex
{
	enum FILTER
	{
		FILTER_NONE = 0,
		FILTER_POINT,
		FILTER_LINEAR,
		FILTER_TRIANGLE,
		FILTER_BOX
	};

	UINT32		Convert( FILTER filter)
	{
		switch( filter)
		{
			case FILTER_NONE :		return D3DX_FILTER_NONE;
			case FILTER_POINT :		return D3DX_FILTER_POINT;
			case FILTER_LINEAR :	return D3DX_FILTER_LINEAR;
			case FILTER_TRIANGLE :	return D3DX_FILTER_TRIANGLE;
			default :				return D3DX_FILTER_BOX;
		}
	}
};

#endif

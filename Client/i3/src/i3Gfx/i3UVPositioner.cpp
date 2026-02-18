#include "i3GfxType.h"
#include "i3UVPositioner.h"


I3_CLASS_INSTANCE( i3UVPositioner);

void	i3UVPositioner::Set(const SIZE32 & Interval, const SIZE32 & TextureSize)
{
	m_Interval		= Interval;
	m_TextureSize	= TextureSize;

	m_Count.x		= TextureSize.w	/ Interval.w;
	m_Count.y		= TextureSize.h	/ Interval.h;
}


POINT32		i3UVPositioner::GetPositionIndex(UINT32 Index)
{
	POINT32	PosIndex = {};
	PosIndex.x	= Index % m_Count.x;
	PosIndex.y	= Index / m_Count.x;
	return PosIndex;
}

bool	i3UVPositioner::CalcuPosition(UINT32 Index, POINT32 & Pos)
{
	if ( Index >= static_cast<UINT32>(m_Count.x * m_Count.y) )
	{
		Pos.x	= Pos.y		= 0;
		return false;
	}

	POINT32	PosIndex = GetPositionIndex(Index);

	Pos.x = PosIndex.x * m_Interval.w;
	Pos.y = PosIndex.y * m_Interval.h;

	return true;
}


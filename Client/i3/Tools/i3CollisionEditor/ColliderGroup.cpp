#include "stdafx.h"
#include "ColliderGroup.h"

I3_CLASS_INSTANCE( ColliderGroup, i3ElementBase);

static UINT8	s_ColorTable[I3_TERRAIN_TYPE_COUNT][3] =
{
	{	32,		32,		32},		// None
	{	150,	150,	150},		// I3_TERRAIN_CONCRETE = 1,				// 能农府飘
	{	128,	255,	255},		// I3_TERRAIN_STEAL,						// 枚力
	{	48,		34,		16},		// I3_TERRAIN_GROUND,						// 入
	{	128,	64,		0},			// I3_TERRAIN_WOOD,						// 唱公
	{	255,	255,	255},		// I3_TERRAIN_SNOW,						// 传
	{	0,		43,		85},		// I3_TERRAIN_WATER_DEEP,					// 拱 (表篮)
	{	0,		116,	232},		// I3_TERRAIN_WATER_SHALLOW,				// 拱 (捐篮)
	{	128,	64,		64},		// I3_TERRAIN_WET_CLAY,					// 柳入
	{	128,	255,	0},		// I3_TERRAIN_GRASS,						// 儡叼
	{	128,	128,	192},		// I3_TERRAIN_MARBLE,						// 措府籍
};

ColliderGroup::ColliderGroup(void)
{
	INT32 i;

	for( i = 0; i < I3_TERRAIN_TYPE_COUNT; i++)
	{
		i3Color::Set( &m_Color[i],	s_ColorTable[i][0], s_ColorTable[i][1], s_ColorTable[i][2], 128);
	}

	m_Group = 0;
}

ColliderGroup::~ColliderGroup(void)
{
}

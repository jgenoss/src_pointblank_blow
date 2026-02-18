#include "stdafx.h"
#include "i3TDKUtilCommon.h"

I3_EXPORT_TDK
INT32		i3TDK::getTerrainTypeCount(void)
{
	return I3_TERRAIN_TYPE_COUNT;
}

static TCHAR s_TerrainTypeName[I3_TERRAIN_TYPE_COUNT][64] =
{
	_T("NA"),						// I3_TERRAIN_NA,							// Not-Assigned
	_T("콘크리트"),					// I3_TERRAIN_CONCRETE = 1,				// 콘크리트
	_T("금속"),						// I3_TERRAIN_STEEL,						// 철제
	_T("흙"),						// I3_TERRAIN_GROUND,						// 흙
	_T("나무"),						// I3_TERRAIN_WOOD,						// 나무
	_T("눈"),						// I3_TERRAIN_SNOW,						// 눈
	_T("물 (깊은)"),				// I3_TERRAIN_WATER_DEEP,					// 물 (깊은)
	_T("물 (얕은)"),				// I3_TERRAIN_WATER_SHALLOW,				// 물 (얕은)
	_T("흙 (진흙)"),				// I3_TERRAIN_WET_CLAY,					// 진흙
	_T("잔디"),						// I3_TERRAIN_GRASS,						// 잔디
	_T("대리석"),					// I3_TERRAIN_MARBLE,						// 대리석
	_T("낙엽"),						// I3_TERRAIN_FALLLEAF,					// 나뭇잎
	_T("콘크리트 (얇은)"),			// I3_TERRAIN_CONCRETE_THIN,				// 얇은 콘크리트
	_T("금속 (얇은)"),				// I3_TERRAIN_STEEL_THIN,					//
	_T("나무 (얇은)"),				// I3_TERRAIN_WOOD_THIN,
	_T("대리석 (얇은)"),			// I3_TERRAIN_MARBLE_THIN,
	_T("종이"),						// I3_TERRAIN_PAPER,						// 종이
	_T("유리"),						// I3_TERRAIN_GLASS,						// 유리
	_T("플라스틱"),					// I3_TERRAIN_PLASTIC,						// 플라스틱
	_T("고무"),						// I3_TERRAIN_RUBBER,						// 고무
	_T("옷감"),						// I3_TERRAIN_CLOTH,						// 천
	_T("흙 (얇은)"),				// I3_TERRAIN_GROUND_THIN,		(관통)
	_T("눈 (얇은)"),				// I3_TERRAIN_SNOW_THIN,		(관통)
	_T("흙 (진흙-얇은)"),			// I3_TERRAIN_WET_CLAY_THIN,	(관통)
	_T("잔디 (얇은)"),				// I3_TERRAIN_GRASS_THIN,		(관통)
	_T("종이 (얇은)"),				// I3_TERRAIN_PAPER_THIN,		(관통)
	_T("낙엽 (얇은)"),				// I3_TERRAIN_FALLLEAF_THIN,	(관통)
	_T("유리 (얇은)"),				// I3_TERRAIN_GLASS_THIN,		(관통)
	_T("플라스틱 (얇은)"),			// I3_TERRAIN_PLASTIC_THIN,		(관통)
	_T("고무 (얇은)"),				// I3_TERRAIN_RUBBER_THIN,		(관통)
	_T("옷감 (얇은)"),				// I3_TERRAIN_CLOTH_THIN,		(관통)
	_T("창살"),						// I3_TERRAIN_BARBEDWIRE		(관통)	
	_T("피"),						// 피
	_T("임시1"),					// 임시 1 ( 이것은 리니지)
	_T("임시2"),						// 임시 2
	_T("천 (데칼없는)"),			//I3_TERRAIN_CLOTH_NO_DECAL, // 천재질이나, 데칼을 사용하지 않음.
	_T("철사"),						//I3_TERRAIN_STEEL_WIRE,		(관통)
	_T("나무 줄기"),				//I3_TERRAIN_WOOD_WIRE,			(관통)
	_T("플라스틱(가는)"),			//I3_TERRAIN_PLASTIC_WIRE,		(관통)
	_T("NONE"),			//I3_TERRAIN_NONE,		(아무것도 없다)
};

I3_EXPORT_TDK
const TCHAR* i3TDK::getTerrainTypeName( I3_TERRAIN_TYPE idx)
{
	return s_TerrainTypeName[ idx];
}

I3_EXPORT_TDK
I3_TERRAIN_TYPE	i3TDK::getTerrainTypeValue( const TCHAR * pszName)
{

	for(INT32 i = 0; i < I3_TERRAIN_TYPE_COUNT; i++)
	{
		if( i3::generic_is_iequal( s_TerrainTypeName[i], pszName) )
			return (I3_TERRAIN_TYPE) i;
	}

	return I3_TERRAIN_NA;
}

///////////////////////////////////////////////////////////////////////////////////////////

struct _NODE_FLAG_INFO
{
	UINT32	m_Flag;
	TCHAR	m_Code[8];
} ;

void i3TDK::GetNodeFlagString( i3Node * pNode, TCHAR* outStr, INT32 outBuffSize )
{
	static _NODE_FLAG_INFO s_FlagTbl[] =
	{
		{	I3_NODEFLAG_DISABLE,				_T("D") },
		{	I3_NODEFLAG_INVISIBLE,				_T("IV") },
		{	I3_NODEFLAG_TRANSFORM,				_T("TX") },
		{	I3_NODEFLAG_TRANSPARENT,			_T("TR") },
		{	I3_NODEFLAG_STATIC,					_T("S") },
		{	I3_NODEFLAG_CACHEMATRIX,			_T("CH") },
		{	I3_NODEFLAG_VOLATILE,				_T("V") },
		{	I3_NODEFLAG_USER,					_T("USER") },
		{	I3_NODEFLAG_CONTROL,				_T("CTRL") },
		{	I3_NODEFLAG_DISABLE_SHADOW_CASTER,	_T("DSC")  },
		{	I3_NODEFLAG_DISABLE_SHADOW_RECEIVER, _T("DSR")  },
		{	0xFFFFFFFF,						0},
	};
	INT32 i, idx = 0;
	bool bFirst = true;

	if (outBuffSize < 2) return;

	outStr[idx] = '[';	idx++;
	outStr[idx] = 0;
	
	for( i = 0; s_FlagTbl[i].m_Flag != 0xFFFFFFFF; i++)
	{
		if( pNode->GetFlag() & s_FlagTbl[i].m_Flag)
		{
			if( bFirst)
			{
				bFirst = false;
			}
			else
			{
				i3::safe_string_cat( outStr, _T("_"), outBuffSize );
			}

			i3::safe_string_cat( outStr, s_FlagTbl[i].m_Code, outBuffSize);
		}
	}

	i3::safe_string_cat( outStr, _T("]"), outBuffSize );
}

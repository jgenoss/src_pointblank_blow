#include "pch.h"
#include "UserMap_Define.h"

void USMAP::STAGE::INFO::init(void)
{
	i3mem::FillZero( this, sizeof(USMAP::STAGE::INFO));

	i3mem::Copy( m_ID, "USI1", 4);
	
	m_Version			= 1;							// Version Number (Ordinal)

	m_Style				= STYLE_INTERCEPTABLE | STYLE_RESPAWN_USER;

	m_bFogEnable		= TRUE;						// Fog 가/불
	m_FogColor			= 0xFF7F7F7F;				// Fog Color
	m_FogNear			= 1.0f;						// Fog Near
	m_FogFar			= 100.0f;					// Fog Far

	m_Gravity			= 9.8f;						// 중력
	m_MaxRedUserCount	= 8;						// RED Team 최대 유저 수
	m_MaxBlueUserCount	= 8;						// BLUE Team 최대 유저 수
	m_TimeLimit			= 10;						// 제한 시간 (분 단위)
	m_RoundLimit		= 5;						// 제한 Round 수
	m_DamageFactor		= 1.0f;						// 데미지의 적용 수준. (0~1)
}

void USMAP::OBJECT::INFO::init(void)
{
	i3mem::FillZero( this, sizeof(USMAP::OBJECT::INFO));

	m_Scale[0] = 2;
	m_Scale[1] = 2;
	m_Scale[2] = 2;
}

void USMAP::OBJECT::INFO::setScale( REAL32 sx, REAL32 sy, REAL32 sz)
{
	m_Scale[0] = (INT8) i3Math::Clamp( (sx / 0.5f), -128.0f, 127.0f);
	m_Scale[1] = (INT8) i3Math::Clamp( (sy / 0.5f), -128.0f, 127.0f);
	m_Scale[2] = (INT8) i3Math::Clamp( (sz / 0.5f), -128.0f, 127.0f);
}

void USMAP::COND::INFO::init(void)
{
	i3mem::FillZero( this, sizeof(USMAP::COND::INFO));

	m_Style				= STYLE_ALWAYS;			// 유형
	m_AndLink			= -1;					// Single-Linked-List의 Next Index. (-1 == 연결 없음)
}

static USMAP::BLOCK::INFO		s_BlockInfo[] =
{
	{	// 0
		"큐브",	"MapEditor/res/block_box.i3Obj", USMAP::BLOCK::ID_CUBE,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 2.0f,		353.0f}
	},

	{	// 1
		"구", "MapEditor/res/block_sphere.i3Obj", USMAP::BLOCK::ID_SPHERE,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 43.0f,		353.0f}
	},

	{	// 2
		"빗면", "MapEditor/res/block_tri.i3Obj", USMAP::BLOCK::ID_SLOPE,	NULL, 	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 84.0f,		353.0f}
	},

	{	// 3
		"발판", "MapEditor/res/block_trap01.i3Obj", USMAP::BLOCK::ID_TRAP,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 125.0f,		353.0f}
	},

	{	// 4
		"피라미드",	"MapEditor/res/block_pyramid.i3Obj", USMAP::BLOCK::ID_PYRAMID,	NULL, NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 167.0f,		353.0f}
	},

	{	// 5
		"통로",	"MapEditor/res/block_road01.i3Obj", USMAP::BLOCK::ID_ROAD1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 208.0f,		353.0f}
	},

	{	// 6
		"환기구", "MapEditor/res/block_hole01.i3Obj", USMAP::BLOCK::ID_HOLE1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 249.0f,		353.0f}
	},

	{	// 7
		"벽(문)", "MapEditor/res/block_wall_door01.i3Obj", USMAP::BLOCK::ID_WALL_DOOR1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 289.0f,		353.0f}
	},

	{	// 8
		"벽(구멍)", "MapEditor/res/block_wall_hole01.i3Obj", USMAP::BLOCK::ID_WALL_HOLE1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 330.0f,		353.0f}
	},

	{	// 9
		"벽(창)", "MapEditor/res/block_wall_window01.i3Obj", USMAP::BLOCK::ID_WALL_WINDOW1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 371.0f,		353.0f}
	},

	{	// 10
		"계단1", "MapEditor/res/block_step01.i3Obj", USMAP::BLOCK::ID_STEP1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 2.0f,		434.0f}
	},

	{	// 11
		"계단2", "", USMAP::BLOCK::ID_STEP2,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 43.0f,		434.0f}
	},

	{	// 12
		"바리케이트", "MapEditor/res/block_baricate.i3Obj", USMAP::BLOCK::ID_BARICATE,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 84.0f,		434.0f}
	},

	{	// 13
		"자동차", "MapEditor/res/block_car01.i3Obj", USMAP::BLOCK::ID_CAR1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 125.0f,		434.0f}
	},

	{	// 14
		"드럼통", "MapEditor/res/block_drum01.i3Obj", USMAP::BLOCK::ID_DRUM1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 166.0f,		434.0f}
	},

	{	// 15
		"컨테이너", "MapEditor/res/block_container01.i3Obj", USMAP::BLOCK::ID_CONTAINER1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 207.0f,		434.0f}
	},

	{	// 16
		"상자(나무)", "MapEditor/res/block_woodbox.i3Obj", USMAP::BLOCK::ID_CONTAINER1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 247.0f,		434.0f}
	}
};

INT32 USMAP::BLOCK::getCount(void)
{
	return GET_ARRAY_COUNT(s_BlockInfo);
}

USMAP::BLOCK::INFO * USMAP::BLOCK::getInfo( INT32 idx)
{
	I3_BOUNDCHK( idx, getCount());

	return & s_BlockInfo[ idx];
}

USMAP::BLOCK::INFO * USMAP::BLOCK::FindInfo( UINT16 id)
{
	INT32 i;

	for( i = 0; i < getCount(); i++)
	{
		if( s_BlockInfo[i].m_ID == id)
			return & s_BlockInfo[i];
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////
//
// Sky
static USMAP::SKY::INFO		s_SkyTable[] =
{
	{	"MapEditor/res/Sky_CloudDayLight.i3s",		"한 낮 (구름)",	NULL,
		{ 1.2f,	1.2f, 1.2f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.2f, 0.8f, 0.2f }
	},

	{	"MapEditor/res/Sky_BecomingRedSky.i3s",		"늦은 오후",	NULL,
		{ 1.2f,	1.0f, 1.0f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.3f, -0.8f, 0.3f }
	},

	{	"MapEditor/res/Sky_BecomingRedSky2.i3s",		"늦은 오후 (노을)",	NULL,
		{ 1.0f,	0.8f, 0.8f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.2f, -0.8f, -0.4f }
	},

	{	"MapEditor/res/Sky_RedSky.i3s",				"노을",	NULL,
		{ 1.0f,	0.95f, 0.8f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.4f, -0.7f, -0.5f }
	},

	{	"",				"",	NULL,
		{ 0.0f,	0.0f, 0.8f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.4f, -0.7f, -0.5f }
	},
};

INT32 USMAP::SKY::getCount(void)
{
	INT32 i;

	for( i = 0; s_SkyTable[i].m_szName[0] != 0; i++);

	return i;
}

USMAP::SKY::INFO * USMAP::SKY::getInfo( INT32 idx)
{
	return & s_SkyTable[idx];
}

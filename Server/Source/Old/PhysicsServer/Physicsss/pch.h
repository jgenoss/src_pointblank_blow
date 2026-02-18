// pch.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once

#define NOMINMAX

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <process.h>
#include <iostream>
#include <tchar.h>
#include <math.h>
#include <mmsystem.h>
#include <assert.h>

#include "NxPhysics.h"
#include "NxCooking.h"

#include "i3Base.h"
#include "i3Math.h"
#include "i3Network.h"
#include "i3Scene\i3PhysixDefine.h"

enum I3_NETWORK_GAMEOBJECT_TYPE
{
	I3_NETWORK_GAMEOBJECT_TYPE_NONE = 0,
	I3_NETWORK_GAMEOBJECT_TYPE_STATIC,			// 고정되어 있다
	I3_NETWORK_GAMEOBJECT_TYPE_MOVE,			// 이동값만
	I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC,			// 이동 + 회전
	I3_NETWORK_GAMEOBJECT_TYPE_ANIM,			// 애니메이션 키프레임

	I3_NETWORK_GAMEOBJECT_TYPE_COUNT
};

enum I3_TERRAIN_TYPE
{
	I3_TERRAIN_NA,							// Not-Assigned
	I3_TERRAIN_CONCRETE = 1,				// 콘크리트
	I3_TERRAIN_STEEL,						// 철제
	I3_TERRAIN_GROUND,						// 흙
	I3_TERRAIN_WOOD,						// 나무
	I3_TERRAIN_SNOW,						// 눈
	I3_TERRAIN_WATER_DEEP,					// 물 (깊은)
	I3_TERRAIN_WATER_SHALLOW,				// 물 (얕은)
	I3_TERRAIN_WET_CLAY,					// 진흙
	I3_TERRAIN_GRASS,						// 잔디
	I3_TERRAIN_MARBLE,						// 대리석
	I3_TERRAIN_FALLLEAF,					// 나뭇잎
	I3_TERRAIN_CONCRETE_THIN,				// 얇은 콘크리트
	I3_TERRAIN_STEEL_THIN,					//
	I3_TERRAIN_WOOD_THIN,
	I3_TERRAIN_MARBLE_THIN,
	I3_TERRAIN_PAPER,						// 종이
	I3_TERRAIN_GLASS,						// 유리
	I3_TERRAIN_PLASTIC,						// 플라스틱
	I3_TERRAIN_RUBBER,						// 고무
	I3_TERRAIN_CLOTH,						// 천
	I3_TERRAIN_GROUND_THIN,
	I3_TERRAIN_SNOW_THIN,
	I3_TERRAIN_WET_CLAY_THIN,
	I3_TERRAIN_GRASS_THIN,
	I3_TERRAIN_PAPER_THIN,
	I3_TERRAIN_FALLLEAF_THIN,
	I3_TERRAIN_GLASS_THIN,
	I3_TERRAIN_PLASTIC_THIN,
	I3_TERRAIN_RUBBER_THIN,
	I3_TERRAIN_CLOTH_THIN,
	I3_TERRAIN_BARBEDWIRE,

	I3_TERRAIN_BLOOD,						// 피
	I3_TERRAIN_TEMP1,						// 임시 1 ( 이것은 리니지)
	I3_TERRAIN_TEMP2,						// 임시 2

	I3_TERRAIN_CLOTH_NO_DECAL,				// 천재질이나, 데칼을 사용하지 않음.

	I3_TERRAIN_TYPE_COUNT
};

#include "../../CommonSource/CommonDef.h"
#include "../../CommonServerSource/CommonServerDef.h"
#include "../../CommonSource/ProtocolDef.h"
#include "../../CommonSource/NetworkP2PDef.h"
#include "../../CommonSource/WeaponDefine.h"
#include "../../CommonSource/GameP2PDef.h"  
#include "../../CommonServerSource/Physics_Protocol.h"

#include "../PxDef.h"
#include "../Config.h"
#include "../ServerDef.h"
#include "../LogFile.h"
#include "../NxGlobal.h"
#include "../ServerStatistics.h"


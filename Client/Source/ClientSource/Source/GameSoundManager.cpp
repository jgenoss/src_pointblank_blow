#include "pch.h"
#include "GameSoundManager.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "i3Base/itl/range/generic_empty.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "MyRoomInfoContext.h"


//#define ENABLE_LOBBY_AMBIENT_SOUND		

I3_CLASS_INSTANCE( CGameSoundManager);//, i3ElementBase);

bool s_bDumpSndPlay = false;

#define SND_TRACE if(s_bDumpSndPlay) GlobalFunc::PB_TRACE
#define GSND_RADIO_MSG_CNT (STBL_IDX_RADIOMSG_AUTO_US2-STBL_IDX_RADIOMSG_CMD_KR+1)

const char * s_TerrainTypeSoundName[I3_TERRAIN_TYPE_COUNT][GTBT_MAX_COUNT] = 
{
	//GTBT_MOVE_LEFT_1				//GTBT_MOVE_LEFT_2			//GTBT_MOVE_RIGHT_1			//GTBT_MOVE_RIGHT_2,		//GTBT_JUMP_START,		//GTBT_JUMP_END,			//GTBT_WPN_HIT_1,			//GTBT_WPN_HIT_2,			//GTBT_WPN_SHOTGUN_HIT_1,			//GTBT_WPN_MELEE_HIT			//GTBT_WPN_CATRIDGE_1		//GTBT_WPN_SHOTGUN_EMPTYCATRIDGE//GTBT_BOUND_THROW				//GTBT_BOUND_HELMET						// GTBT_WPN_KUKRII_HIT					//GTBT_WPN_M1887_HIT		//GTBT_WPN_GH5007_SEC1_HIT			//GTBT_WPN_GH5007_SEC2_HIT				//	GTBT_WPN_MINIAXE_HIT	GTBT_BOUND_CANDY						 GTBT_WPN_BONEKNIFE_HIT					GTBT_CHINESECLEAVER					GTBT_SUMMEREVENT_WEAPON_HIT					GTBT_WATERGUN_HIT
	{	"CH_Concrete_Left_1.wav",	"CH_Concrete_Left_2.wav",	"CH_Concrete_Right_1.wav",	"CH_Concrete_Right_2.wav",	"CH_Concrete_Jump.wav",	"CH_Concrete_JumpEnd.wav",	"WM_Hit_Concrete_1.wav",	"WM_Hit_Concrete_2.wav",	"WM_Shotgun_Hit_Concrete_1.wav",	"WM_Knife_Hit_Concrete.wav",		"WM_Bullet_Concrete_1.wav",	"WM_SG_Bullet_Concrete.wav",	"WM_Throw_Bound_Concrete.wav"	,	"OB_Helmet_Bound_Concrete.wav", 	"WM_Kukri_Hit_Concrete.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Concrete_1.wav",		"WM_GH5007_Hit_Concrete_2.wav",	"WM_MiniAxe_Hit_Concrete.wav",	"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Concrete_01.wav",		"WM_MiniAxe_Hit_Concrete.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// 0,
	{	"CH_Concrete_Left_1.wav",	"CH_Concrete_Left_2.wav",	"CH_Concrete_Right_1.wav",	"CH_Concrete_Right_2.wav",	"CH_Concrete_Jump.wav",	"CH_Concrete_JumpEnd.wav",	"WM_Hit_Concrete_1.wav",	"WM_Hit_Concrete_2.wav",	"WM_Shotgun_Hit_Concrete_1.wav",	"WM_Knife_Hit_Concrete.wav",		"WM_Bullet_Concrete_1.wav",	"WM_SG_Bullet_Concrete.wav",	"WM_Throw_Bound_Concrete.wav"	,	"OB_Helmet_Bound_Concrete.wav",		"WM_Kukri_Hit_Concrete.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Concrete_1.wav",		"WM_GH5007_Hit_Concrete_2.wav",	"WM_MiniAxe_Hit_Concrete.wav",	"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Concrete_02.wav",		"WM_MiniAxe_Hit_Concrete.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_CONCRETE = 1,				// 콘크리트
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_2.wav",		"WM_BoneKnife_Hit_Metal_01.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_STEAL,					// 철제
	{	"CH_Ground_Left_1.wav",		"CH_Ground_Left_2.wav",		"CH_Ground_Right_1.wav",	"CH_Ground_Right_2.wav",	"CH_Ground_Jump.wav",	"CH_Ground_JumpEnd.wav",	"WM_Hit_Ground_1.wav",		"WM_Hit_Ground_2.wav",		"WM_Shotgun_Hit_Ground_1.wav",		"WM_Knife_Hit_Ground.wav",			"WM_Bullet_Ground_1.wav",	"WM_SG_Bullet_Ground.wav",		"WM_Throw_Bound_Ground.wav"		,	"OB_Helmet_Bound_Ground.wav",		"WM_Knife_Hit_Ground.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Ground_1.wav",		"WM_GH5007_Hit_Ground_2.wav",		"WM_Knife_Hit_Ground.wav",		"WM_Recovery_Candy_Throw_Bound_3.wav",		"WM_BoneKnife_Hit_Ground_01.wav",		"WM_Knife_Hit_Ground.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_GROUND,					// 흙
	{	"CH_Wood_Left_1.wav",		"CH_Wood_Left_2.wav",		"CH_Wood_Right_1.wav",		"CH_Wood_Right_2.wav",		"CH_Wood_Jump.wav",		"CH_Wood_JumpEnd.wav",		"WM_Hit_Wood_1.wav",		"WM_Hit_Wood_2.wav",		"WM_Shotgun_Hit_Wood_1.wav",		"WM_Knife_Hit_Wood.wav",			"WM_Bullet_Wood_1.wav",		"WM_SG_Bullet_Wood.wav",		"WM_Throw_Bound_Wood.wav"		,	"OB_Helmet_Bound_Wood.wav",			"WM_Knife_Hit_Wood.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Wood_1.wav",			"WM_GH5007_Hit_Wood_2.wav",			"WM_Knife_Hit_Wood.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Wood_01.wav"	,		"WM_Knife_Hit_Wood.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_WOOD,						// 나무
	{	"CH_Concrete_Left_1.wav",	"CH_Concrete_Left_2.wav",	"CH_Concrete_Right_1.wav",	"CH_Concrete_Right_2.wav",	"CH_Concrete_Jump.wav",	"CH_Concrete_JumpEnd.wav",	"WM_Hit_Concrete_1.wav",	"WM_Hit_Concrete_2.wav",	"WM_Shotgun_Hit_Concrete_1.wav",	"WM_Knife_Hit_Concrete.wav",		"WM_Bullet_Concrete_1.wav",	"WM_SG_Bullet_Concrete.wav",	"WM_Throw_Bound_Concrete.wav"	,	"OB_Helmet_Bound_Concrete.wav",		"WM_Kukri_Hit_Concrete.wav",		"WM_M1887_Melee_Hit.wav",	"WM_Knife_Hit_Water.wav",			"WM_Knife_Hit_Water.wav",		"WM_MiniAxe_Hit_Concrete.wav",	"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Concrete_02.wav",		"WM_MiniAxe_Hit_Concrete.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_SNOW,						// 눈
	{	"CH_Water_Left_1.wav",		"CH_Water_Left_2.wav",		"CH_Water_Right_1.wav",		"CH_Water_Right_2.wav",		"CH_Water_Jump.wav",	"CH_Water_JumpEnd.wav",		"WM_Hit_Water_1.wav",		"WM_Hit_Water_2.wav",		"WM_Shotgun_Hit_Water_1.wav",		"WM_Knife_Hit_Water.wav",			"WM_Bullet_Water_1.wav",	"WM_SG_Bullet_Water.wav",		"WM_Throw_Bound_Water.wav"		,	"OB_Helmet_Bound_Water.wav",		"WM_Knife_Hit_Water.wav",			"WM_M1887_Melee_Hit.wav",	"WM_Knife_Hit_Water.wav",			"WM_Knife_Hit_Water.wav",		"WM_Knife_Hit_Water.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Concrete_01.wav",		"WM_Knife_Hit_Water.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_WATER_DEEP,				// 물 (깊은)
	{	"CH_Water_Left_1.wav",		"CH_Water_Left_2.wav",		"CH_Water_Right_1.wav",		"CH_Water_Right_2.wav",		"CH_Water_Jump.wav",	"CH_Water_JumpEnd.wav",		"WM_Hit_Water_1.wav",		"WM_Hit_Water_2.wav",		"WM_Shotgun_Hit_Water_1.wav",		"WM_Knife_Hit_Water.wav",			"WM_Bullet_Water_1.wav",	"WM_SG_Bullet_Water.wav",		"WM_Throw_Bound_Water.wav"		,	"OB_Helmet_Bound_Water.wav",		"WM_Knife_Hit_Water.wav",			"WM_M1887_Melee_Hit.wav",	"WM_Knife_Hit_Water.wav",			"WM_Knife_Hit_Water.wav",		"WM_Knife_Hit_Water.wav",		"WM_Recovery_Candy_Throw_Bound_2.wav",		"WM_BoneKnife_Hit_Concrete_02.wav",		"WM_Knife_Hit_Water.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_WATER_SHALLOW,			// 물 (얕은)
	{	"CH_Water_Left_1.wav",		"CH_Water_Left_2.wav",		"CH_Water_Right_1.wav",		"CH_Water_Right_2.wav",		"CH_Water_Jump.wav",	"CH_Water_JumpEnd.wav",		"WM_Hit_Water_1.wav",		"WM_Hit_Water_2.wav",		"WM_Shotgun_Hit_Water_1.wav",		"WM_Knife_Hit_Water.wav",			"WM_Bullet_Water_1.wav",	"WM_SG_Bullet_Water.wav",		"WM_Throw_Bound_Water.wav"		,	"OB_Helmet_Bound_Water.wav",		"WM_Knife_Hit_Water.wav",			"WM_M1887_Melee_Hit.wav",	"WM_Knife_Hit_Water.wav",			"WM_Knife_Hit_Water.wav",		"WM_Knife_Hit_Water.wav",		"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Concrete_01.wav",		"WM_Knife_Hit_Water.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_WET_CLAY,					// 진흙
	{	"CH_Grass_Left_1.wav",		"CH_Grass_Left_2.wav",		"CH_Grass_Right_1.wav",		"CH_Grass_Right_2.wav",		"CH_Grass_Jump.wav",	"CH_Grass_JumpEnd.wav",		"WM_Hit_Grass_1.wav",		"WM_Hit_Grass_2.wav",		"WM_Shotgun_Hit_Grass_1.wav",		"WM_Knife_Hit_Grass.wav",			"WM_Bullet_Grass_1.wav",	"WM_SG_Bullet_Grass.wav",		"WM_Throw_Bound_Grass.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Grass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Grass_1.wav",		"WM_GH5007_Hit_Grass_2.wav",		"WM_Knife_Hit_Grass.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Grass_01.wav",		"WM_Knife_Hit_Grass.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_GRASS,					// 잔디
	{	"CH_Concrete_Left_1.wav",	"CH_Concrete_Left_2.wav",	"CH_Concrete_Right_1.wav",	"CH_Concrete_Right_2.wav",	"CH_Concrete_Jump.wav",	"CH_Concrete_JumpEnd.wav",	"WM_Hit_Concrete_1.wav",	"WM_Hit_Concrete_2.wav",	"WM_Shotgun_Hit_Concrete_1.wav",	"WM_Knife_Hit_Concrete.wav",		"WM_Bullet_Concrete_1.wav",	"WM_SG_Bullet_Concrete.wav",	"WM_Throw_Bound_Concrete.wav"	,	"OB_Helmet_Bound_Concrete.wav",		"WM_Kukri_Hit_Concrete.wav",	"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Concrete_1.wav",		"WM_GH5007_Hit_Concrete_2.wav",		"WM_MiniAxe_Hit_Concrete.wav",	"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Concrete_01.wav",		"WM_MiniAxe_Hit_Concrete.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_MARBLE,					// 대리석
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Metal_01.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_FALLLEAF
	{	"CH_Concrete_Left_1.wav",	"CH_Concrete_Left_2.wav",	"CH_Concrete_Right_1.wav",	"CH_Concrete_Right_2.wav",	"CH_Concrete_Jump.wav",	"CH_Concrete_JumpEnd.wav",	"WM_Hit_Concrete_1.wav",	"WM_Hit_Concrete_2.wav",	"WM_Shotgun_Hit_Concrete_1.wav",	"WM_Knife_Hit_Concrete.wav",		"WM_Bullet_Concrete_1.wav",	"WM_SG_Bullet_Concrete.wav",	"WM_Throw_Bound_Concrete.wav"	,	"OB_Helmet_Bound_Concrete.wav",		"WM_Kukri_Hit_Concrete.wav",	"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Concrete_1.wav",		"WM_GH5007_Hit_Concrete_2.wav",		"WM_MiniAxe_Hit_Concrete.wav",	"WM_Recovery_Candy_Throw_Bound_3.wav",		"WM_BoneKnife_Hit_Concrete_02.wav",		"WM_MiniAxe_Hit_Concrete.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_CONCRETE_THIN,			// 콘크리트
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Metal_01.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_STEEL_THIN,		
	{	"CH_Wood_Left_1.wav",		"CH_Wood_Left_2.wav",		"CH_Wood_Right_1.wav",		"CH_Wood_Right_2.wav",		"CH_Wood_Jump.wav",		"CH_Wood_JumpEnd.wav",		"WM_Hit_Wood_1.wav",		"WM_Hit_Wood_2.wav",		"WM_Shotgun_Hit_Wood_1.wav",		"WM_Knife_Hit_Wood.wav",			"WM_Bullet_Wood_1.wav",		"WM_SG_Bullet_Wood.wav",		"WM_Throw_Bound_Wood.wav"		,	"OB_Helmet_Bound_Wood.wav",			"WM_Knife_Hit_Wood.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Wood_1.wav",			"WM_GH5007_Hit_Wood_2.wav",			"WM_Knife_Hit_Wood.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Wood_01.wav",			"WM_Knife_Hit_Wood.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_WOOD_THIN,
	{	"CH_Concrete_Left_1.wav",	"CH_Concrete_Left_2.wav",	"CH_Concrete_Right_1.wav",	"CH_Concrete_Right_2.wav",	"CH_Concrete_Jump.wav",	"CH_Concrete_JumpEnd.wav",	"WM_Hit_Concrete_1.wav",	"WM_Hit_Concrete_2.wav",	"WM_Shotgun_Hit_Concrete_1.wav",	"WM_Knife_Hit_Concrete.wav",		"WM_Bullet_Concrete_1.wav",	"WM_SG_Bullet_Concrete.wav",	"WM_Throw_Bound_Concrete.wav"	,	"OB_Helmet_Bound_Concrete.wav",		"WM_Kukri_Hit_Concrete.wav",	"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Concrete_1.wav",		"WM_GH5007_Hit_Concrete_2.wav",		"WM_MiniAxe_Hit_Concrete.wav",	"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Concrete_01.wav",		"WM_MiniAxe_Hit_Concrete.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_MARBLE,
	{	"CH_Paper_Box_Left_1.wav",	"CH_Paper_Box_Left_2.wav",	"CH_Paper_Box_Right_1.wav",	"CH_Paper_Box_Right_2.wav",	"CH_Paper_Box_Jump.wav","CH_Paper_Box_JumpEnd.wav",	"WM_Hit_Paper_Box_1.wav",	"WM_Hit_Paper_Box_2.wav",	"WM_Shotgun_Hit_Paper_1.wav",		"WM_Knife_Hit_Paper_Box.wav",		"WM_Bullet_Paper_Box_1.wav","WM_SG_Bullet_Paper.wav",		"WM_Throw_Bound_Paper.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Paper_Box.wav",	"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Paper_Box_1.wav",	"WM_GH5007_Hit_Paper_Box_2.wav",	"WM_Knife_Hit_Paper_Box.wav",	"WM_Recovery_Candy_Throw_Bound_4.wav",		 "WM_BoneKnife_Hit_Paper_Box_01.wav",	"WM_Knife_Hit_Paper_Box.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_PAPER
	{	"CH_Glass_Left_1.wav",		"CH_Glass_Left_2.wav",		"CH_Glass_Right_1.wav",		"CH_Glass_Right_2.wav",		"CH_Glass_Jump.wav",	"CH_Glass_JumpEnd.wav",		"WM_Hit_Glass_1.wav",		"WM_Hit_Glass_2.wav",		"WM_Shotgun_Hit_Glass_1.wav",		"WM_Knife_Hit_Glass.wav",			"WM_Bullet_Glass_1.wav",	"WM_SG_Bullet_Glass.wav",		"WM_Throw_Bound_Glass.wav"		,	"OB_Helmet_Bound_Glass.wav",		"WM_Knife_Hit_Glass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Glass_1.wav",		"WM_GH5007_Hit_Glass_2.wav",		"WM_Knife_Hit_Glass.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Glass_01.wav",		"WM_Knife_Hit_Glass.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_GLASS
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Metal_02.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_PLASTIC
	{	"CH_Grass_Left_1.wav",		"CH_Grass_Left_2.wav",		"CH_Grass_Right_1.wav",		"CH_Grass_Right_2.wav",		"CH_Grass_Jump.wav",	"CH_Grass_JumpEnd.wav",		"WM_Hit_Grass_1.wav",		"WM_Hit_Grass_2.wav",		"WM_Shotgun_Hit_Grass_1.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Bullet_Grass_1.wav",	"WM_SG_Bullet_Grass.wav",		"WM_Throw_Bound_Grass.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Grass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Grass_1.wav",		"WM_GH5007_Hit_Grass_2.wav",		"WM_Knife_Hit_Carpet.wav",		"WM_Recovery_Candy_Throw_Bound_3.wav",		"WM_BoneKnife_Hit_Grass_01.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_RUBBER
	{	"CH_Carpet_Left_1.wav",		"CH_Carpet_Left_2.wav",		"CH_Carpet_Right_1.wav",	"CH_Carpet_Right_2.wav",	"CH_Carpet_Jump.wav",	"CH_Carpet_JumpEnd.wav",	"WM_Hit_Carpet_1.wav",		"WM_Hit_Carpet_2.wav",		"WM_Shotgun_Hit_Carpet_1.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Bullet_Carpet_1.wav",	"WM_SG_Bullet_Carpet.wav",		"WM_Throw_Bound_Carpet.wav"		,	"OB_Helmet_Bound_Ground.wav",		"WM_Knife_Hit_Carpet.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Carpet_1.wav",		"WM_GH5007_Hit_Carpet_2.wav",		"WM_Knife_Hit_Carpet.wav",		"WM_Recovery_Candy_Throw_Bound_3.wav",		"WM_BoneKnife_Hit_Carpet_01.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_CLOTH
	{	"CH_Ground_Left_1.wav",		"CH_Ground_Left_2.wav",		"CH_Ground_Right_1.wav",	"CH_Ground_Right_2.wav",	"CH_Ground_Jump.wav",	"CH_Ground_JumpEnd.wav",	"WM_Hit_Ground_1.wav",		"WM_Hit_Ground_2.wav",		"WM_Shotgun_Hit_Ground_1.wav",		"WM_Knife_Hit_Ground.wav",			"WM_Bullet_Ground_1.wav",	"WM_SG_Bullet_Ground.wav",		"WM_Throw_Bound_Ground.wav"		,	"OB_Helmet_Bound_Ground.wav",		"WM_Knife_Hit_Ground.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Ground_1.wav",		"WM_GH5007_Hit_Ground_2.wav",		"WM_Knife_Hit_Ground.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Ground_02.wav",		"WM_Knife_Hit_Ground.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_GROUND_THIN
	{	"CH_Ground_Left_1.wav",		"CH_Ground_Left_2.wav",		"CH_Ground_Right_1.wav",	"CH_Ground_Right_2.wav",	"CH_Ground_Jump.wav",	"CH_Ground_JumpEnd.wav",	"WM_Hit_Ground_1.wav",		"WM_Hit_Ground_2.wav",		"WM_Shotgun_Hit_Ground_1.wav",		"WM_Knife_Hit_Ground.wav",			"WM_Bullet_Ground_1.wav",	"WM_SG_Bullet_Ground.wav",		"WM_Throw_Bound_Ground.wav"		,	"OB_Helmet_Bound_Ground.wav",		"WM_Knife_Hit_Ground.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Ground_1.wav",		"WM_GH5007_Hit_Ground_2.wav",		"WM_Knife_Hit_Ground.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Ground_01.wav",		"WM_Knife_Hit_Ground.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_SNOW_THIN
	{	"CH_Water_Left_1.wav",		"CH_Water_Left_2.wav",		"CH_Water_Right_1.wav",		"CH_Water_Right_2.wav",		"CH_Water_Jump.wav",	"CH_Water_JumpEnd.wav",		"WM_Hit_Water_1.wav",		"WM_Hit_Water_2.wav",		"WM_Shotgun_Hit_Water_1.wav",		"WM_Knife_Hit_Water.wav",			"WM_Bullet_Water_1.wav",	"WM_SG_Bullet_Water.wav",		"WM_Throw_Bound_Water.wav"		,	"OB_Helmet_Bound_Water.wav",		"WM_Knife_Hit_Water.wav",			"WM_M1887_Melee_Hit.wav",	"WM_Knife_Hit_Water.wav",			"WM_Knife_Hit_Water.wav",		"WM_Knife_Hit_Water.wav",		"WM_Recovery_Candy_Throw_Bound_2.wav",		"WM_BoneKnife_Hit_Concrete_02.wav",		"WM_Knife_Hit_Water.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_WET_CLAY_THIN
	{	"CH_Grass_Left_1.wav",		"CH_Grass_Left_2.wav",		"CH_Grass_Right_1.wav",		"CH_Grass_Right_2.wav",		"CH_Grass_Jump.wav",	"CH_Grass_JumpEnd.wav",		"WM_Hit_Grass_1.wav",		"WM_Hit_Grass_2.wav",		"WM_Shotgun_Hit_Grass_1.wav",		"WM_Knife_Hit_Grass.wav",			"WM_Bullet_Grass_1.wav",	"WM_SG_Bullet_Grass.wav",		"WM_Throw_Bound_Grass.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Grass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Grass_1.wav",		"WM_GH5007_Hit_Grass_2.wav",		"WM_Knife_Hit_Grass.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Grass_01.wav",		"WM_Knife_Hit_Grass.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_GRASS_THIN,
	{	"CH_Grass_Left_1.wav",		"CH_Grass_Left_2.wav",		"CH_Grass_Right_1.wav",		"CH_Grass_Right_2.wav",		"CH_Grass_Jump.wav",	"CH_Grass_JumpEnd.wav",		"WM_Hit_Grass_1.wav",		"WM_Hit_Grass_2.wav",		"WM_Shotgun_Hit_Grass_1.wav",		"WM_Knife_Hit_Paper_Box.wav",		"WM_Bullet_Grass_1.wav",	"WM_SG_Bullet_Grass.wav",		"WM_Throw_Bound_Grass.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Grass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Paper_Box_1.wav",	"WM_GH5007_Hit_Paper_Box_2.wav",	"WM_Knife_Hit_Paper_Box.wav",	"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Grass_02.wav",		"WM_Knife_Hit_Paper_Box.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_PAPER_THIN,
	{	"CH_Grass_Left_1.wav",		"CH_Grass_Left_2.wav",		"CH_Grass_Right_1.wav",		"CH_Grass_Right_2.wav",		"CH_Grass_Jump.wav",	"CH_Grass_JumpEnd.wav",		"WM_Hit_Grass_1.wav",		"WM_Hit_Grass_2.wav",		"WM_Shotgun_Hit_Grass_1.wav",		"WM_Knife_Hit_Grass.wav",			"WM_Bullet_Grass_1.wav",	"WM_SG_Bullet_Grass.wav",		"WM_Throw_Bound_Grass.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Grass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Grass_1.wav",		"WM_GH5007_Hit_Grass_2.wav",		"WM_Knife_Hit_Grass.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Grass_01.wav",		"WM_Knife_Hit_Grass.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_FALLLEAF_THIN,
	{	"CH_Glass_Left_1.wav",		"CH_Glass_Left_2.wav",		"CH_Glass_Right_1.wav",		"CH_Glass_Right_2.wav",		"CH_Glass_Jump.wav",	"CH_Glass_JumpEnd.wav",		"WM_Hit_Glass_1.wav",		"WM_Hit_Glass_2.wav",		"WM_Shotgun_Hit_Glass_1.wav",		"WM_Knife_Hit_Glass.wav",			"WM_Bullet_Glass_1.wav",	"WM_SG_Bullet_Glass.wav",		"WM_Throw_Bound_Glass.wav"		,	"OB_Helmet_Bound_Glass.wav",		"WM_Knife_Hit_Glass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Glass_1.wav",		"WM_GH5007_Hit_Glass_2.wav",		"WM_Knife_Hit_Glass.wav",		"WM_Recovery_Candy_Throw_Bound_3.wav",		"WM_BoneKnife_Hit_Glass_01.wav",		"WM_Knife_Hit_Grass.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_GLASS_THIN,
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_2.wav",		"WM_BoneKnife_Hit_Metal_01.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_PLASTIC_THIN,
	{	"CH_Grass_Left_1.wav",		"CH_Grass_Left_2.wav",		"CH_Grass_Right_1.wav",		"CH_Grass_Right_2.wav",		"CH_Grass_Jump.wav",	"CH_Grass_JumpEnd.wav",		"WM_Hit_Grass_1.wav",		"WM_Hit_Grass_2.wav",		"WM_Shotgun_Hit_Grass_1.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Bullet_Grass_1.wav",	"WM_SG_Bullet_Grass.wav",		"WM_Throw_Bound_Grass.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Grass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Carpet_1.wav",		"WM_GH5007_Hit_Carpet_2.wav",		"WM_Knife_Hit_Carpet.wav",		"WM_Recovery_Candy_Throw_Bound_3.wav",		"WM_BoneKnife_Hit_Grass_01.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_RUBBER_THIN,
	{	"CH_Grass_Left_1.wav",		"CH_Grass_Left_2.wav",		"CH_Grass_Right_1.wav",		"CH_Grass_Right_2.wav",		"CH_Grass_Jump.wav",	"CH_Grass_JumpEnd.wav",		"WM_Hit_Grass_1.wav",		"WM_Hit_Grass_2.wav",		"WM_Shotgun_Hit_Grass_1.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Bullet_Grass_1.wav",	"WM_SG_Bullet_Grass.wav",		"WM_Throw_Bound_Grass.wav"		,	"OB_Helmet_Bound_Grass.wav",		"WM_Knife_Hit_Grass.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Carpet_1.wav",		"WM_GH5007_Hit_Carpet_2.wav",		"WM_Knife_Hit_Carpet.wav",		"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Grass_02.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_CLOTH_THIN,
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Metal_02.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_BARBEDWIRE,
	{	"CH_Ground_Left_1.wav",		"CH_Ground_Left_2.wav",		"CH_Ground_Right_1.wav",	"CH_Ground_Right_2.wav",	"CH_Ground_Jump.wav",	"CH_Ground_JumpEnd.wav",	"WM_Hit_Human_1.wav",		"WM_Hit_Human_2.wav",		"WM_Hit_Human_3.wav",				"WM_Knife_Hit_Human.wav",			"WM_Bullet_Ground_1.wav",	"WM_SG_Bullet_Ground.wav",		"WM_Throw_Bound_Ground.wav"		,	"OB_Helmet_Bound_Ground.wav",		"WM_Kukri_Hit_Human.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Human_1.wav",		"WM_GH5007_Hit_Human_2.wav",		"WM_MiniAxe_Hit_Human.wav",		"WM_Recovery_Candy_Throw_Bound_1.wav",		"WM_BoneKnife_Hit_Ground_01.wav",		"WM_ChineseCleaver_Hit_Human.wav",	"WM_SummerEvent_Weapon_Hit.wav",			"WM_Shotgun_Hit_WaterGun.wav",		},		// I3_TERRAIN_BLOOD,
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Temp1.wav",			"WM_Hit_Temp1.wav",			"WM_Hit_Temp1.wav",					"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_2.wav",		"WM_BoneKnife_Hit_Metal_02.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_TEMP1,
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Temp2.wav",			"WM_Hit_Temp2.wav",			"WM_Hit_Temp2.wav",					"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_3.wav",		"WM_BoneKnife_Hit_Metal_01.wav"	,		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_TEMP2,
	{	"CH_Carpet_Left_1.wav",		"CH_Carpet_Left_2.wav",		"CH_Carpet_Right_1.wav",	"CH_Carpet_Right_2.wav",	"CH_Carpet_Jump.wav",	"CH_Carpet_JumpEnd.wav",	"WM_Hit_Carpet_1.wav",		"WM_Hit_Carpet_2.wav",		"WM_Shotgun_Hit_Carpet_1.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Bullet_Carpet_1.wav",	"WM_SG_Bullet_Carpet.wav",		"WM_Throw_Bound_Carpet.wav"		,	"OB_Helmet_Bound_Ground.wav",		"WM_Knife_Hit_Carpet.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Carpet_1.wav",		"WM_GH5007_Hit_Carpet_2.wav",		"WM_Knife_Hit_Carpet.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Carpet_01.wav",		"WM_Knife_Hit_Carpet.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},		// I3_TERRAIN_CLOTH_NO_DECAL
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_2.wav",		"WM_BoneKnife_Hit_Metal_01.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},			//I3_TERRAIN_STEEL_WIRE,
	{	"CH_Wood_Left_1.wav",		"CH_Wood_Left_2.wav",		"CH_Wood_Right_1.wav",		"CH_Wood_Right_2.wav",		"CH_Wood_Jump.wav",		"CH_Wood_JumpEnd.wav",		"WM_Hit_Wood_1.wav",		"WM_Hit_Wood_2.wav",		"WM_Shotgun_Hit_Wood_1.wav",		"WM_Knife_Hit_Wood.wav",			"WM_Bullet_Wood_1.wav",		"WM_SG_Bullet_Wood.wav",		"WM_Throw_Bound_Wood.wav"		,	"OB_Helmet_Bound_Wood.wav",			"WM_Knife_Hit_Wood.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Wood_1.wav",			"WM_GH5007_Hit_Wood_2.wav",			"WM_Knife_Hit_Wood.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Wood_01.wav"	,		"WM_Knife_Hit_Wood.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},			//I3_TERRAIN_WOOD_WIRE,		
	{	"CH_Metal_Left_1.wav",		"CH_Metal_Left_2.wav",		"CH_Metal_Right_1.wav",		"CH_Metal_Right_2.wav",		"CH_Metal_Jump.wav",	"CH_Metal_JumpEnd.wav",		"WM_Hit_Metal_1.wav",		"WM_Hit_Metal_2.wav",		"WM_Shotgun_Hit_Metal_1.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Bullet_Metal_1.wav",	"WM_SG_Bullet_Metal.wav",		"WM_Throw_Bound_Metal.wav"		,	"OB_Helmet_Bound_Metal.wav",		"WM_Knife_Hit_Metal.wav",		"WM_M1887_Melee_Hit.wav",	"WM_GH5007_Hit_Metal_1.wav",		"WM_GH5007_Hit_Metal_2.wav",		"WM_Knife_Hit_Metal.wav",		"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_BoneKnife_Hit_Metal_02.wav",		"WM_Knife_Hit_Metal.wav",			"WM_Recovery_Candy_Throw_Bound_4.wav",		"WM_Shotgun_Hit_WaterGun.wav",},			//I3_TERRAIN_PLASTIC_WIRE,		
	{ "",		"",		"",		"",		"",	"",		"",		"",		"",		"",			"",	"",		""		,	"",		"",		"",	"",		"",		"",		"",		"",		"",			"",		"", },			//I3_TERRAIN_NONE,		
};																																																																																																																								

const char * s_TerrainTypeDinoSoundName[I3_TERRAIN_TYPE_COUNT][GTTBDTR_MAX_COUNT] = 
{
	//	GTTBDTR_MOVE_TREX_LEFT_1	GTTBDTR_MOVE_TREX_LEFT_2,	GTTBDTR_MOVE_TREX_RIGHT_1,	GTTBDTR_MOVE_TREX_RIGHT_2,	GTTBDTR_MOVE_RAPTOR_LEFT_1,	GTTBDTR_MOVE_RAPTOR_LEFT_2,	GTTBDTR_MOVE_RAPTOR_RIGHT_1,	GTTBDTR_MOVE_RAPTOR_RIGHT_2,	GTTBDTR_MOVE_RAPTOR_JUMP_START,	GTTBDTR_MOVE_RAPTOR_JUMP_END,	GTTBDTR_MOVE_TANK_LEFT_1,	GTTBDTR_MOVE_TANK_LEFT_2,	GTTBDTR_MOVE_TANK_RIGHT_1,	GTTBDTR_MOVE_TANK_RIGHT_2,	GTTBDTR_MOVE_ACID_LEFT_1,	GTTBDTR_MOVE_ACID_LEFT_2,	GTTBDTR_MOVE_ACID_RIGHT_1,		GTTBDTR_MOVE_ACID_RIGHT_2,		GTTBDTR_MOVE_ACID_JUMP_START,	GTTBDTR_MOVE_ACID_JUMP_END,	  //GTBT_WPN_DINO_HIT			 ATTACK_TREX             , GTBT_ATTACK_TANK_HIT
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Concrete_Left_1.wav",	"VR_Concrete_Left_2.wav",	"VR_Concrete_Right_1.wav",		"VR_Concrete_Right_2.wav",		"VR_Concrete_Jump.wav",			"VR_Concrete_JumpEnd.wav"	,	"TA_Concrete_Left_1.wav",	"TA_Concrete_Left_2.wav",	"TA_Concrete_Right_1.wav",	"TA_Concrete_Right_2.wav",	"AC_Concrete_Left_1.wav",	"AC_Concrete_Left_2.wav",	"AC_Concrete_Right_1.wav",		"AC_Concrete_Right_2.wav",		"AC_Concrete_Jump.wav",			"AC_Concrete_JumpEnd.wav"	, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_NA
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Concrete_Left_1.wav",	"VR_Concrete_Left_2.wav",	"VR_Concrete_Right_1.wav",		"VR_Concrete_Right_2.wav",		"VR_Concrete_Jump.wav",			"VR_Concrete_JumpEnd.wav"	,	"TA_Concrete_Left_1.wav",	"TA_Concrete_Left_2.wav",	"TA_Concrete_Right_1.wav",	"TA_Concrete_Right_2.wav",	"AC_Concrete_Left_1.wav",	"AC_Concrete_Left_2.wav",	"AC_Concrete_Right_1.wav",		"AC_Concrete_Right_2.wav",		"AC_Concrete_Jump.wav",			"AC_Concrete_JumpEnd.wav"	, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_CONCRETE
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_STEEL
	{	"TS_Ground_Left_1.wav",		"TS_Ground_Left_2.wav",		"TS_Ground_Right_1.wav",	"TS_Ground_Right_2.wav",	"VR_Ground_Left_1.wav",		"VR_Ground_Left_2.wav",		"VR_Ground_Right_1.wav",		"VR_Ground_Right_2.wav",		"VR_Ground_Jump.wav",			"VR_Ground_JumpEnd.wav"		,	"TA_Ground_Left_1.wav",		"TA_Ground_Left_2.wav",		"TA_Ground_Right_1.wav",	"TA_Ground_Right_2.wav",	"AC_Ground_Left_1.wav",		"AC_Ground_Left_2.wav",		"AC_Ground_Right_1.wav",		"AC_Ground_Right_2.wav",		"AC_Ground_Jump.wav",			"AC_Ground_JumpEnd.wav"		, "DM_VR_Hit_Ground.wav",		"DM_TS_Hit_Ground.wav"	, "DM_TA_Hit_Ground.wav"	},// I3_TERRAIN_GROUND
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Wood_Left_1.wav",		"VR_Wood_Left_2.wav",		"VR_Wood_Right_1.wav",			"VR_Wood_Right_2.wav",			"VR_Wood_Jump.wav",				"VR_Wood_JumpEnd.wav"		,	"TA_Wood_Left_1.wav",		"TA_Wood_Left_2.wav",		"TA_Wood_Right_1.wav",		"TA_Wood_Right_2.wav",		"AC_Wood_Left_1.wav",		"AC_Wood_Left_2.wav",		"AC_Wood_Right_1.wav",			"AC_Wood_Right_2.wav",			"AC_Wood_Jump.wav",				"AC_Wood_JumpEnd.wav"		, "DM_VR_Hit_Wood.wav",			"DM_TS_Hit_Wood.wav"	, "DM_TA_Hit_Wood.wav"		},// I3_TERRAIN_WOOD
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Concrete_Left_1.wav",	"VR_Concrete_Left_2.wav",	"VR_Concrete_Right_1.wav",		"VR_Concrete_Right_2.wav",		"VR_Concrete_Jump.wav",			"VR_Concrete_JumpEnd.wav"	,	"TA_Concrete_Left_1.wav",	"TA_Concrete_Left_2.wav",	"TA_Concrete_Right_1.wav",	"TA_Concrete_Right_2.wav",	"AC_Concrete_Left_1.wav",	"AC_Concrete_Left_2.wav",	"AC_Concrete_Right_1.wav",		"AC_Concrete_Right_2.wav",		"AC_Concrete_Jump.wav",			"AC_Concrete_JumpEnd.wav"	, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_SNOW
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Water_Left_1.wav",		"VR_Water_Left_2.wav",		"VR_Water_Right_1.wav",			"VR_Water_Right_2.wav",			"VR_Water_Jump.wav",			"VR_Water_JumpEnd.wav"		,	"TA_Water_Left_1.wav",		"TA_Water_Left_2.wav",		"TA_Water_Right_1.wav",		"TA_Water_Right_2.wav",		"AC_Water_Left_1.wav",		"AC_Water_Left_2.wav",		"AC_Water_Right_1.wav",			"AC_Water_Right_2.wav",			"AC_Water_Jump.wav",			"AC_Water_JumpEnd.wav"		, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_WATER_DEEP
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Water_Left_1.wav",		"VR_Water_Left_2.wav",		"VR_Water_Right_1.wav",			"VR_Water_Right_2.wav",			"VR_Water_Jump.wav",			"VR_Water_JumpEnd.wav"		,	"TA_Water_Left_1.wav",		"TA_Water_Left_2.wav",		"TA_Water_Right_1.wav",		"TA_Water_Right_2.wav",		"AC_Water_Left_1.wav",		"AC_Water_Left_2.wav",		"AC_Water_Right_1.wav",			"AC_Water_Right_2.wav",			"AC_Water_Jump.wav",			"AC_Water_JumpEnd.wav"		, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_WATER_SHALLOW
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Water_Left_1.wav",		"VR_Water_Left_2.wav",		"VR_Water_Right_1.wav",			"VR_Water_Right_2.wav",			"VR_Water_Jump.wav",			"VR_Water_JumpEnd.wav"		,	"TA_Water_Left_1.wav",		"TA_Water_Left_2.wav",		"TA_Water_Right_1.wav",		"TA_Water_Right_2.wav",		"AC_Water_Left_1.wav",		"AC_Water_Left_2.wav",		"AC_Water_Right_1.wav",			"AC_Water_Right_2.wav",			"AC_Water_Jump.wav",			"AC_Water_JumpEnd.wav"		, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_WET_CLAY
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Grass_Left_1.wav",		"VR_Grass_Left_2.wav",		"VR_Grass_Right_1.wav",			"VR_Grass_Right_2.wav",			"VR_Grass_Jump.wav",			"VR_Grass_JumpEnd.wav"		,	"TA_Grass_Left_1.wav",		"TA_Grass_Left_2.wav",		"TA_Grass_Right_1.wav",		"TA_Grass_Right_2.wav",		"AC_Grass_Left_1.wav",		"AC_Grass_Left_2.wav",		"AC_Grass_Right_1.wav",			"AC_Grass_Right_2.wav",			"AC_Grass_Jump.wav",			"AC_Grass_JumpEnd.wav"		, "DM_VR_Hit_Grass.wav",		"DM_TS_Hit_Grass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_GRASS
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Concrete_Left_1.wav",	"VR_Concrete_Left_2.wav",	"VR_Concrete_Right_1.wav",		"VR_Concrete_Right_2.wav",		"VR_Concrete_Jump.wav",			"VR_Concrete_JumpEnd.wav"	,	"TA_Concrete_Left_1.wav",	"TA_Concrete_Left_2.wav",	"TA_Concrete_Right_1.wav",	"TA_Concrete_Right_2.wav",	"AC_Concrete_Left_1.wav",	"AC_Concrete_Left_2.wav",	"AC_Concrete_Right_1.wav",		"AC_Concrete_Right_2.wav",		"AC_Concrete_Jump.wav",			"AC_Concrete_JumpEnd.wav"	, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_MARBLE
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_FALLLEAF
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Concrete_Left_1.wav",	"VR_Concrete_Left_2.wav",	"VR_Concrete_Right_1.wav",		"VR_Concrete_Right_2.wav",		"VR_Concrete_Jump.wav",			"VR_Concrete_JumpEnd.wav"	,	"TA_Concrete_Left_1.wav",	"TA_Concrete_Left_2.wav",	"TA_Concrete_Right_1.wav",	"TA_Concrete_Right_2.wav",	"AC_Concrete_Left_1.wav",	"AC_Concrete_Left_2.wav",	"AC_Concrete_Right_1.wav",		"AC_Concrete_Right_2.wav",		"AC_Concrete_Jump.wav",			"AC_Concrete_JumpEnd.wav"	, "DM_VR_Hit_Concret.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_CONCRETE_THIN
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_STEEL_THIN
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Wood_Left_1.wav",		"VR_Wood_Left_2.wav",		"VR_Wood_Right_1.wav",			"VR_Wood_Right_2.wav",			"VR_Wood_Jump.wav",				"VR_Wood_JumpEnd.wav"		,	"TA_Wood_Left_1.wav",		"TA_Wood_Left_2.wav",		"TA_Wood_Right_1.wav",		"TA_Wood_Right_2.wav",		"AC_Wood_Left_1.wav",		"AC_Wood_Left_2.wav",		"AC_Wood_Right_1.wav",			"AC_Wood_Right_2.wav",			"AC_Wood_Jump.wav",				"AC_Wood_JumpEnd.wav"		, "DM_VR_Hit_Wood.wav",			"DM_TS_Hit_Wood.wav"	, "DM_TA_Hit_Wood.wav"		},// I3_TERRAIN_WOOD_THIN
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Concrete_Left_1.wav",	"VR_Concrete_Left_2.wav",	"VR_Concrete_Right_1.wav",		"VR_Concrete_Right_2.wav",		"VR_Concrete_Jump.wav",			"VR_Concrete_JumpEnd.wav"	,	"TA_Concrete_Left_1.wav",	"TA_Concrete_Left_2.wav",	"TA_Concrete_Right_1.wav",	"TA_Concrete_Right_2.wav",	"AC_Concrete_Left_1.wav",	"AC_Concrete_Left_2.wav",	"AC_Concrete_Right_1.wav",		"AC_Concrete_Right_2.wav",		"AC_Concrete_Jump.wav",			"AC_Concrete_JumpEnd.wav"	, "DM_VR_Hit_Concret.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_MARBLE_THIN
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Paper_Box_Left_1.wav",	"VR_Paper_Box_Left_2.wav",	"VR_Paper_Box_Right_1.wav",		"VR_Paper_Box_Right_2.wav",		"VR_Paper_Box_Jump.wav",		"VR_Paper_Box_JumpEnd.wav"	,	"TA_Paper_Box_Left_1.wav",	"TA_Paper_Box_Left_2.wav",	"TA_Paper_Box_Right_1.wav",	"TA_Paper_Box_Right_2.wav",	"AC_Paper_Box_Left_1.wav",	"AC_Paper_Box_Left_2.wav",	"AC_Paper_Box_Right_1.wav",		"AC_Paper_Box_Right_2.wav",		"AC_Paper_Box_Jump.wav",		"AC_Paper_Box_JumpEnd.wav"	, "DM_VR_Hit_Paper_Box.wav",	"DM_TS_Hit_Paper_Box.wav","DM_TA_Hit_Paper_Box.wav"	},// I3_TERRAIN_PAPER
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Glass_Left_1.wav",		"VR_Glass_Left_2.wav",		"VR_Glass_Right_1.wav",			"VR_Glass_Right_2.wav",			"VR_Glass_Jump.wav",			"VR_Glass_JumpEnd.wav"		,	"TA_Glass_Left_1.wav",		"TA_Glass_Left_2.wav",		"TA_Glass_Right_1.wav",		"TA_Glass_Right_2.wav",		"AC_Glass_Left_1.wav",		"AC_Glass_Left_2.wav",		"AC_Glass_Right_1.wav",			"AC_Glass_Right_2.wav",			"AC_Glass_Jump.wav",			"AC_Glass_JumpEnd.wav"		, "DM_VR_Hit_Glass.wav",		"DM_TS_Hit_Glass.wav"	, "DM_TA_Hit_Glass.wav"		},// I3_TERRAIN_GLASS
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_PLASTIC
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Grass_Left_1.wav",		"VR_Grass_Left_2.wav",		"VR_Grass_Right_1.wav",			"VR_Grass_Right_2.wav",			"VR_Grass_Jump.wav",			"VR_Grass_JumpEnd.wav"		,	"TA_Grass_Left_1.wav",		"TA_Grass_Left_2.wav",		"TA_Grass_Right_1.wav",		"TA_Grass_Right_2.wav",		"AC_Grass_Left_1.wav",		"AC_Grass_Left_2.wav",		"AC_Grass_Right_1.wav",			"AC_Grass_Right_2.wav",			"AC_Grass_Jump.wav",			"AC_Grass_JumpEnd.wav"		, "DM_VR_Hit_Grass.wav",		"DM_TS_Hit_Grass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_RUBBER
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Carpet_Left_1.wav",		"VR_Carpet_Left_2.wav",		"VR_Carpet_Right_1.wav",		"VR_Carpet_Right_2.wav",		"VR_Carpet_Jump.wav",			"VR_Carpet_JumpEnd.wav"		,	"TA_Carpet_Left_1.wav",		"TA_Carpet_Left_2.wav",		"TA_Carpet_Right_1.wav",	"TA_Carpet_Right_2.wav",	"AC_Carpet_Left_1.wav",		"AC_Carpet_Left_2.wav",		"AC_Carpet_Right_1.wav",		"AC_Carpet_Right_2.wav",		"AC_Carpet_Jump.wav",			"AC_Carpet_JumpEnd.wav"		, "DM_VR_Hit_Carpet.wav",		"DM_TS_Hit_Carpet.wav"	, "DM_TA_Hit_Carpet.wav"	},// I3_TERRAIN_CLOTH
	{	"TS_Ground_Left_1.wav",		"TS_Ground_Left_2.wav",		"TS_Ground_Right_1.wav",	"TS_Ground_Right_2.wav",	"VR_Ground_Left_1.wav",		"VR_Ground_Left_2.wav",		"VR_Ground_Right_1.wav",		"VR_Ground_Right_2.wav",		"VR_Ground_Jump.wav",			"VR_Ground_JumpEnd.wav"		,	"TA_Ground_Left_1.wav",		"TA_Ground_Left_2.wav",		"TA_Ground_Right_1.wav",	"TA_Ground_Right_2.wav",	"AC_Ground_Left_1.wav",		"AC_Ground_Left_2.wav",		"AC_Ground_Right_1.wav",		"AC_Ground_Right_2.wav",		"AC_Ground_Jump.wav",			"AC_Ground_JumpEnd.wav"		, "DM_VR_Hit_Ground.wav",		"DM_TS_Hit_Ground.wav"	, "DM_TA_Hit_Ground.wav"	},// I3_TERRAIN_GROUND_THIN
	{	"TS_Ground_Left_1.wav",		"TS_Ground_Left_2.wav",		"TS_Ground_Right_1.wav",	"TS_Ground_Right_2.wav",	"VR_Ground_Left_1.wav",		"VR_Ground_Left_2.wav",		"VR_Ground_Right_1.wav",		"VR_Ground_Right_2.wav",		"VR_Ground_Jump.wav",			"VR_Ground_JumpEnd.wav"		,	"TA_Ground_Left_1.wav",		"TA_Ground_Left_2.wav",		"TA_Ground_Right_1.wav",	"TA_Ground_Right_2.wav",	"AC_Ground_Left_1.wav",		"AC_Ground_Left_2.wav",		"AC_Ground_Right_1.wav",		"AC_Ground_Right_2.wav",		"AC_Ground_Jump.wav",			"AC_Ground_JumpEnd.wav"		, "DM_VR_Hit_Ground.wav",		"DM_TS_Hit_Ground.wav"	, "DM_TA_Hit_Ground.wav"	},// I3_TERRAIN_SNOW_THIN
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Water_Left_1.wav",		"VR_Water_Left_2.wav",		"VR_Water_Right_1.wav",			"VR_Water_Right_2.wav",			"VR_Water_Jump.wav",			"VR_Water_JumpEnd.wav"		,	"TA_Water_Left_1.wav",		"TA_Water_Left_2.wav",		"TA_Water_Right_1.wav",		"TA_Water_Right_2.wav",		"AC_Water_Left_1.wav",		"AC_Water_Left_2.wav",		"AC_Water_Right_1.wav",			"AC_Water_Right_2.wav",			"AC_Water_Jump.wav",			"AC_Water_JumpEnd.wav"		, "DM_VR_Hit_Concrete.wav",		"DM_TS_Hit_Concrete.wav", "DM_TA_Hit_Concrete.wav"	},// I3_TERRAIN_WET_CLAY_THIN
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Grass_Left_1.wav",		"VR_Grass_Left_2.wav",		"VR_Grass_Right_1.wav",			"VR_Grass_Right_2.wav",			"VR_Grass_Jump.wav",			"VR_Grass_JumpEnd.wav"		,	"TA_Grass_Left_1.wav",		"TA_Grass_Left_2.wav",		"TA_Grass_Right_1.wav",		"TA_Grass_Right_2.wav",		"AC_Grass_Left_1.wav",		"AC_Grass_Left_2.wav",		"AC_Grass_Right_1.wav",			"AC_Grass_Right_2.wav",			"AC_Grass_Jump.wav",			"AC_Grass_JumpEnd.wav"		, "DM_VR_Hit_Grass.wav",		"DM_TS_Hit_Grass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_GRASS_THIN
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Grass_Left_1.wav",		"VR_Grass_Left_2.wav",		"VR_Grass_Right_1.wav",			"VR_Grass_Right_2.wav",			"VR_Grass_Jump.wav",			"VR_Grass_JumpEnd.wav"		,	"TA_Grass_Left_1.wav",		"TA_Grass_Left_2.wav",		"TA_Grass_Right_1.wav",		"TA_Grass_Right_2.wav",		"AC_Grass_Left_1.wav",		"AC_Grass_Left_2.wav",		"AC_Grass_Right_1.wav",			"AC_Grass_Right_2.wav",			"AC_Grass_Jump.wav",			"AC_Grass_JumpEnd.wav"		, "DM_VR_Hit_Grass.wav",		"DM_TS_Hit_Grass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_PAPER_THIN
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Grass_Left_1.wav",		"VR_Grass_Left_2.wav",		"VR_Grass_Right_1.wav",			"VR_Grass_Right_2.wav",			"VR_Grass_Jump.wav",			"VR_Grass_JumpEnd.wav"		,	"TA_Grass_Left_1.wav",		"TA_Grass_Left_2.wav",		"TA_Grass_Right_1.wav",		"TA_Grass_Right_2.wav",		"AC_Grass_Left_1.wav",		"AC_Grass_Left_2.wav",		"AC_Grass_Right_1.wav",			"AC_Grass_Right_2.wav",			"AC_Grass_Jump.wav",			"AC_Grass_JumpEnd.wav"		, "DM_VR_Hit_Grass.wav",		"DM_TS_Hit_Grass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_FALLLEAF_THIN
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Glass_Left_1.wav",		"VR_Glass_Left_2.wav",		"VR_Glass_Right_1.wav",			"VR_Glass_Right_2.wav",			"VR_Glass_Jump.wav",			"VR_Glass_JumpEnd.wav"		,	"TA_Glass_Left_1.wav",		"TA_Glass_Left_2.wav",		"TA_Glass_Right_1.wav",		"TA_Glass_Right_2.wav",		"AC_Glass_Left_1.wav",		"AC_Glass_Left_2.wav",		"AC_Glass_Right_1.wav",			"AC_Glass_Right_2.wav",			"AC_Glass_Jump.wav",			"AC_Glass_JumpEnd.wav"		, "DM_VR_Hit_Glass.wav",		"DM_TS_Hit_Glass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_GLASS_THIN
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_PLASTIC_THIN
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Grass_Left_1.wav",		"VR_Grass_Left_2.wav",		"VR_Grass_Right_1.wav",			"VR_Grass_Right_2.wav",			"VR_Grass_Jump.wav",			"VR_Grass_JumpEnd.wav"		,	"TA_Grass_Left_1.wav",		"TA_Grass_Left_2.wav",		"TA_Grass_Right_1.wav",		"TA_Grass_Right_2.wav",		"AC_Grass_Left_1.wav",		"AC_Grass_Left_2.wav",		"AC_Grass_Right_1.wav",			"AC_Grass_Right_2.wav",			"AC_Grass_Jump.wav",			"AC_Grass_JumpEnd.wav"		, "DM_VR_Hit_Grass.wav",		"DM_TS_Hit_Grass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_RUBBER_THIN
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Grass_Left_1.wav",		"VR_Grass_Left_2.wav",		"VR_Grass_Right_1.wav",			"VR_Grass_Right_2.wav",			"VR_Grass_Jump.wav",			"VR_Grass_JumpEnd.wav"		,	"TA_Grass_Left_1.wav",		"TA_Grass_Left_2.wav",		"TA_Grass_Right_1.wav",		"TA_Grass_Right_2.wav",		"AC_Grass_Left_1.wav",		"AC_Grass_Left_2.wav",		"AC_Grass_Right_1.wav",			"AC_Grass_Right_2.wav",			"AC_Grass_Jump.wav",			"AC_Grass_JumpEnd.wav"		, "DM_VR_Hit_Grass.wav",		"DM_TS_Hit_Grass.wav"	, "DM_TA_Hit_Grass.wav"		},// I3_TERRAIN_CLOTH_THIN
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_BARBEDWIRE
	{	"TS_Ground_Left_1.wav",		"TS_Ground_Left_2.wav",		"TS_Ground_Right_1.wav",	"TS_Ground_Right_2.wav",	"VR_Ground_Left_1.wav",		"VR_Ground_Left_2.wav",		"VR_Ground_Right_1.wav",		"VR_Ground_Right_2.wav",		"VR_Ground_Jump.wav",			"VR_Ground_JumpEnd.wav"		,	"TA_Ground_Left_1.wav",		"TA_Ground_Left_2.wav",		"TA_Ground_Right_1.wav",	"TA_Ground_Right_2.wav",	"AC_Ground_Left_1.wav",		"AC_Ground_Left_2.wav",		"AC_Ground_Right_1.wav",		"AC_Ground_Right_2.wav",		"AC_Ground_Jump.wav",			"AC_Ground_JumpEnd.wav"		, "DM_VR_Hit_Ground.wav",		"DM_TS_Hit_Ground.wav"	, "DM_TA_Hit_Ground.wav"	},// I3_TERRAIN_BLOOD
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_TEMP1
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_TEMP2
	{	"TS_Grass_Left_1.wav",		"TS_Grass_Left_2.wav",		"TS_Grass_Right_1.wav",		"TS_Grass_Right_2.wav",		"VR_Carpet_Left_1.wav",		"VR_Carpet_Left_2.wav",		"VR_Carpet_Right_1.wav",		"VR_Carpet_Right_2.wav",		"VR_Carpet_Jump.wav",			"VR_Carpet_JumpEnd.wav"		,	"TA_Carpet_Left_1.wav",		"TA_Carpet_Left_2.wav",		"TA_Carpet_Right_1.wav",	"TA_Carpet_Right_2.wav",	"AC_Carpet_Left_1.wav",		"AC_Carpet_Left_2.wav",		"AC_Carpet_Right_1.wav",		"AC_Carpet_Right_2.wav",		"AC_Carpet_Jump.wav",			"AC_Carpet_JumpEnd.wav"		, "DM_VR_Hit_Carpet.wav",		"DM_TS_Hit_Carpet.wav"	, "DM_TA_Hit_Carpet.wav"	},//I3_TERRAIN_CLOTH_NO_DECAL
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_STEEL_WIRE,
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Wood_Left_1.wav",		"VR_Wood_Left_2.wav",		"VR_Wood_Right_1.wav",			"VR_Wood_Right_2.wav",			"VR_Wood_Jump.wav",				"VR_Wood_JumpEnd.wav"		,	"TA_Wood_Left_1.wav",		"TA_Wood_Left_2.wav",		"TA_Wood_Right_1.wav",		"TA_Wood_Right_2.wav",		"AC_Wood_Left_1.wav",		"AC_Wood_Left_2.wav",		"AC_Wood_Right_1.wav",			"AC_Wood_Right_2.wav",			"AC_Wood_Jump.wav",				"AC_Wood_JumpEnd.wav"		, "DM_VR_Hit_Wood.wav",			"DM_TS_Hit_Wood.wav"	, "DM_TA_Hit_Wood.wav"		},// I3_TERRAIN_WOOD_WIRE,		
	{	"TS_Concrete_Left_1.wav",	"TS_Concrete_Left_2.wav",	"TS_Concrete_Right_1.wav",	"TS_Concrete_Right_2.wav",	"VR_Metal_Left_1.wav",		"VR_Metal_Left_2.wav",		"VR_Metal_Right_1.wav",			"VR_Metal_Right_2.wav",			"VR_Metal_Jump.wav",			"VR_Metal_JumpEnd.wav"		,	"TA_Metal_Left_1.wav",		"TA_Metal_Left_2.wav",		"TA_Metal_Right_1.wav",		"TA_Metal_Right_2.wav",		"AC_Metal_Left_1.wav",		"AC_Metal_Left_2.wav",		"AC_Metal_Right_1.wav",			"AC_Metal_Right_2.wav",			"AC_Metal_Jump.wav",			"AC_Metal_JumpEnd.wav"		, "DM_VR_Hit_Metal.wav",		"DM_TS_Hit_Metal.wav"	, "DM_TA_Hit_Metal.wav"		},// I3_TERRAIN_PLASTIC_WIRE,		
	{ "",	"",	"",	"",	"",		"",		"",			"",			"",			""		,	"",		"",		"",		"",		"",		"",		"",			"",			"",			""		, "",		""	, "" },// I3_TERRAIN_NONE,		
};																																																																																																																																																					  

const char * s_CharacterActionSoundName[2][GTBCA_MAX_COUNT] =	
{
	{ "CH_Thud_1.wav", "CH_SitDown_1.wav", "CH_StandUp_1.wav", "CH_Accessory_Run_1.wav", "CH_Accessory_Walk_1.wav", "CH_Cloth_Run_1.wav", "CH_Cloth_Walk_1.wav", "CH_Head_Hit_1.wav", "WM_Knife_Hit_Head.wav", "MV_Auto_K3.wav", "MV_Hit_Death.wav", "MV_Zoom_Breath.wav", "MV_Hit_Downfall.wav", "MV_Heart_Beat.wav", "OB_Warp.wav", "CH_Critical_Male.wav" , "Madness_MAN_Attack.wav", "Madness_MAN_Death.wav", "Madness_MAN_Idle.wav" },
	{ "CH_Thud_2.wav", "CH_SitDown_2.wav", "CH_StandUp_2.wav", "CH_Accessory_Run_2.wav", "CH_Accessory_Walk_2.wav", "CH_Cloth_Run_2.wav", "CH_Cloth_Walk_2.wav", "CH_Head_Hit_2.wav", "WM_Knife_Hit_Head.wav", "WV_Auto_K3.wav", "WV_Hit_Death.wav", "WV_Zoom_Breath.wav", "WV_Hit_Downfall.wav", "MV_Heart_Beat.wav", "",			  "CH_Critical_Female.wav", "Madness_WOMAN_Attack.wav", "Madness_WOMAN_Death.wav", "Madness_WOMAN_Idle.wav" }
};

const char * s_WeaponStateSoundName[3][GTBWS_MAX_COUNT] = 
{
	//GTBWS_ON_ZOOM			//GTBWS_ON_SCOPE	//GTBWS_ON_ASSAULTSCOPE		//GTBWS_ON_GRAZE_BULLET		//GTBWS_ON_EMPTY_BULLET	  //GTBWS_ON_HIT_HELMET_ATTACK,		//GTBWS_ON_HIT_HELMET_DEFENCE	 //GTBWS_ON_HIT_HUMAN  //GTBWS_ON_HIT_KNIFE_HUMAN //GTBWS_ON_HIT_THROW				//GTBWS_ON_HIT_HELMET_KNIFE	//GTBWS_ON_WILDHOG_ATTACK			//GTBWS_ON_SIEGE_ATTACK				//GTBWS_ON_SIEGE_ATTACK2	//GTBWS_ON_GRENADE_EXT			//GTBWS_ON_GRENADE_FIRE			//GTBWS_ON_C5			//GTBWS_M197				//GTBWS_ON_GRAZE_RPG7			//GTBWS_ON_HIT_KUKRII		//GTBWS_ON_HIT_GH5007_1				//GTBWS_ON_HIT_GH5007_2			//GTBWS_ON_HIT_MINIAXE		//GTBWS_ON_HIT_KNUCKLE_HUMAN_A	// GTBWS_ON_HIT_KNUCKLE_HUMAN_B						GTBWS_ON_HIT_PINDAD_KNIFE					GTBWS_ON_HIT_RAPTOR_BITE										//GTBWS_ON_HIT_FANG_BLADE_1		//GTBWS_ON_HIT_FANG_BLADE_2									//GTBWS_ON_HIT_BALLISTIC				//GTBWS_ON_HIT_BALLISTIC B			GTBWS_ON_HIT_KERIS_HUMAN_A   GTBWS_ON_HIT_KERIS_HUMAN_B		GTBWS_ON_HIT_SHOVEL_HUMAN_A,				GTBWS_ON_HIT_SHOVEL_HUMAN_B													GTBWS_ON_HIT_CHINESECLEAVER				GTBWS_ON_HIT_SUMMEREVENT_WEAPON   								GTBWS_ON_HIT_NUNCHAKU			GTBWS_ON_HIT_GOATHAMMER			
	{	"WM_Zoom_In.wav",	"WM_Scope_In.wav",	"WM_AssaultScope_In.wav",	"WM_Hit_GrazeBullet_1.wav", "WM_Bullet_Empty_1.wav"	, "OB_Helmet_Hit_Attack_1.wav" ,	"OB_Helmet_Hit_Defence_1.wav" , "WM_Hit_Human_1.wav", "WM_Knife_Hit_Human.wav"	, "WM_K400_Explosion_Damage.wav",	"OB_Helmet_Hit_Knife.wav",	"WildHogCannon_specialattack3.wav", "OB_SiegeGolem_specialattack.wav",	"OB_SiegeGolem_attack.wav",	"WM_K201_Extension_On.wav",		"WM_K201_Fire.wav",				"WM_C5_Count_A.wav",	"WM_M197_OverHeat.wav",		"WM_Hit_RPG7_GrazeBullet.wav",	"WM_Kukri_Hit_Human.wav",		"WM_GH5007_Hit_Human_1.wav",		"WM_GH5007_Hit_Human_2.wav",	"WM_MiniAxe_Hit_Human.wav",	"WM_Brass_Knuckle_Hit_Human_1.wav", "WM_Brass_Knuckle_Secondary_Hit_Human_1.wav",	"WM_Pindad_SS1-R5_Attack_Knife_Hit.wav",	"VR_Attack_Mouse_Hit.wav","WM_BoneKnife_Hit_Human_01.wav",		"WM_FangBlade_Hit_Human_1.wav",	"WM_FangBlade_Hit_Human_2.wav",	"DM_ST_Hit_Graze_1.wav",	"WM_BallisticKnife_A_Hit_Human.wav",	"WM_BallisticKnife_B_Hit_Human.wav","WM_Keris_A_Hit_Human.wav",	"WM_Keris_B_Hit_Human.wav",		"Entrenching_shovel_Attack_hit_A_1.wav",	"Entrenching_shovel_Attack_hit_B_1.wav",		"WM_Throw_Hold.wav",	"WM_ChineseCleaver_Hit_Human.wav",		"WM_SummerEvent_Weapon_Hit.wav",	"WM_Sheep_hammer_Hit.wav",	"WM_Nunchaku_Hit.wav", 	"WM_Halloween_Hammer_Hit.wav" },
	{	"WM_Zoom_In.wav",	"WM_Scope_In.wav",	"WM_AssaultScope_In.wav",	"WM_Hit_GrazeBullet_2.wav", "WM_Bullet_Empty_2.wav"	, "OB_Beanie_Hit_Attack.wav" ,		"OB_Beanie_Hit_Defence.wav" ,	"WM_Hit_Human_2.wav", ""						, "WM_Flash_Explosion_Damage.wav",	"OB_Beanie_Hit_Knife.wav",	"WM_K400_Explosion.wav",			"", 								"",							"WM_K201_Extension_Off.wav",	"WM_K201_EmptyCatridge.wav",	"WM_C5_Stick.wav",		"WM_M197_Turn_Off.wav",		"WM_RPG7_Zoom_In.wav",			"WM_Kukri_Hit_Concrete.wav",	"WM_GH5007_Hit_Concrete_1.wav",	"WM_GH5007_Hit_Concrete_2.wav",	"",							"WM_Brass_Knuckle_Hit_Human_2.wav",	"WM_Brass_Knuckle_Secondary_Hit_Human_2.wav",	"WM_Pindad_SS1-R5_Attack_Knife_Hit.wav",	"VR_Attack_Mouse_Hit.wav","WM_BoneKnife_Hit_Human_02.wav",		"",								"",								"DM_ST_Hit_Graze_1.wav",	""								,		""						   ,		""						  ,					   "",			"Entrenching_shovel_Attack_hit_A_2.wav",	"Entrenching_shovel_Attack_hit_B_2.wav",			"WM_Throw_Hold.wav",	"",										"",									"",						    "", 					"WM_Halloween_Hammer_Hit.wav" },
	{	"WM_Zoom_Out.wav",	"WM_Scope_Out.wav",	"WM_AssaultScope_Out.wav",	"WM_Hit_GrazeBullet_3.wav", ""						, "OB_Helmet_Hit_Attack_3.wav" ,	"OB_Helmet_Hit_Defence_1.wav" ,	"WM_Hit_Human_3.wav", ""						, "",                               "",							"WM_MedicKit_Recovery.wav",			"",									"",							"",							"",							"" ,							"WM_M197_Turn_On.wav",		"WM_RPG7_Zoom_Out.wav",			"",								"",								"",								"",							"WM_Brass_Knuckle_Hit_Human_3.wav",	"WM_Brass_Knuckle_Secondary_Hit_Human_3.wav",	"WM_Pindad_SS1-R5_Attack_Knife_Hit.wav",	"VR_Attack_Mouse_Hit.wav","WM_BoneKnife_Hit_Human_01.wav",		"",								"",								"DM_ST_Hit_Graze_1.wav",	""							   ,		""						   ,		""						  ,					   "",			"",											"",													"WM_Throw_Hold.wav",	"",										"",									"",							"", 					"WM_Halloween_Hammer_Hit.wav" },
};

const char * s_DinoActionSoundName[GTBD_MAX_COUNT][GTBDA_MAX_COUNT] = 
{
	//GTBDA_ON_BREATH_1,	GTBDA_ON_BREATH_2,	GTBDA_ON_BREATH_3,	GTBDA_ON_BREATH_4,	GTBDA_ON_DOWN,	GTBDA_ON_HIT,			GTBDA_ON_DAMAGE_LITTLE,	GTBDA_ON_DAMAGE_SERIOUSLY,	GTBDA_ON_DAMAGE_DOWNFALL,	GTBDA_ON_DEATH_HOWL,	GTBDA_ON_ATTACK_1_A,	GTBDA_ON_ATTACK_1_B,		GTBDA_ON_ATTACK_2,		GTBDA_ON_BITE_HIT,			GTBDA_ON_SCRATCH_HIT,		GTBDA_ON_EFFECT,
	{"TS_Breath_1.wav",		"TS_Breath_2.wav",	"TS_Breath_3.wav",	"TS_Breath_4.wav",	"TS_Down.wav",	"TS_Hit.wav",			"TS_Hit_Damage_1.wav",	"TS_Hit_Damage_Eyes.wav",	""					 ,		"TS_Death_Howl.wav",	"TS_Attack_Mouse.wav",	"",							"TS_Attack_Howl.wav",	"TS_Attack_Mouse_Hit.wav",	"DM_VR_Hit_Human_1.wav",	"",	},
	{"VR_Breath_1.wav",		"VR_Breath_2.wav",	"VR_Breath_3.wav",	"VR_Breath_4.wav",	"VR_Down.wav",	"VR_Hit.wav",			"VR_Hit_Damage_1.wav",	"VR_HIt_Damage_2.wav",		"VR_Hit_Downfall.wav",		"VR_Death_Howl.wav",	"VR_Attack_Hand_A.wav",	"VR_Attack_Hand_B.wav",		"VR_Attack_Mouse.wav",	"VR_Attack_Mouse_Hit.wav",	"DM_VR_Hit_Human_2.wav",	"",	},
	{"ST_Breath_1.wav",		"ST_Breath_2.wav",	"ST_Breath_3.wav",	"ST_Breath_4.wav",	"VR_Down.wav",	"ST_Hit_Damage_1.wav",	"VR_Hit_Damage_1.wav",	"VR_HIt_Damage_2.wav",		"VR_Hit_Downfall.wav",		"ST_Death_Howl_1.wav",	"ST_Attack_A.wav",		"",							"ST_Attack_B.wav",		"",							"ST_Attack_A_Shot.wav",		"",	},
	{"TA_Breath_1.wav",		"TA_Breath_2.wav",	"TA_Breath_3.wav",	"TA_Breath_4.wav",	"TA_Down.wav",	"TA_Hit_Damage_1.wav",	"TA_Hit_Damage_2.wav",	"TA_Hit_Damage_3.wav",		"VR_Hit_Downfall.wav",		"TA_Death_Howl_2.wav",	"TA_Attack_A.wav",		"",							"TA_Attack_B_1.wav",	"TA_Attack_B_2.wav",		"DM_TA_Hit_Human.wav",		"",	},
	{"AC_Breath_1.wav",		"AC_Breath_2.wav",	"AC_Breath_3.wav",	"",					"",				"AC_Hit_Damage_1.wav",	"AC_Hit_Damage_2.wav",	"AC_Hit_Damage_3.wav",		"VR_Hit_Downfall.wav",		"AC_Death_Howl_1.wav",	"AC_Attack_A_1.wav",	"AC_Attack_A_2.wav",		"AC_Attack_B_1.wav",	"AC_Attack_B_2.wav",		"AC_Attack_B_Boom.wav",		"AC_Attack_B_Effect.wav",	},
	{"EL_Breath_1.wav",		"EL_Breath_2.wav",	"EL_Breath_3.wav",	"EL_Breath_4.wav",	"",				"EL_Hit_Damage_1.wav",	"EL_Hit_Damage_2.wav",	"EL_Hit_Damage_3.wav",		"VR_Hit_Downfall.wav",		"EL_Death_Howl_1.wav",	"EL_Attack_A_1.wav",	"EL_Attack_A_2.wav",		"EL_Attack_B.wav",		"EL_Attack_B_1.wav",		"EL_Attack_A_hit_1.wav",		"",	},

};

const char * s_WeaponState_C4SoundName[GTFW_C4_MAX_COUNT] = 
{
	"WM_C4_Button.wav",
	"WM_C4_Installed.wav",
	"WM_C4_Count_A.wav",
	"WM_C4_Count_B.wav",
	"WM_C4_Count_C.wav",
	"WM_C4_Count_D.wav",
	"WM_C4_Count_E.wav",
	"WM_C4_Explosion.wav",
	"WM_C4_Take.wav",
	"WM_C4_Disarmed.wav"
};

const char * s_AmbientSoundFile[GSND_LOBBY_AMBIENT_COUNT] = 
{
	"OB_Exp_ch47_Ambient_out.wav",
	"OB_Library_Gunfire_Ambient.wav",
	"OB_Library_Gunfire_Ambient.wav",
	"OB_Truck_Ambient.wav",
	"OB_Wind_Howl_Ambient_3.wav",
	"Stormtube_Ambient_blue.wav"
};


const char * s_AnnounceSoundName[GTA_MAX_COUNT] = // GTA_MAX_COUNT] =  갯수가 맞지 않아서 C2078컴파일오류 아예 첨자를 날렸습니다... 43개를 초과한 모양... (11.08.05.수빈)
{
	"MV_Ann_Chain_Killer.wav",						// 연속 킬
	"MV_Ann_MassKill.wav",							//
	"MV_Ann_Double_Kill.wav",						// 더블 킬
	"MV_Ann_Chain_Stopper.wav",						//
	"MV_Ann_Triple_Kill.wav",						// 트리플 킬
	"MV_Ann_HeadShot.wav",							// 헤드샷
	"MV_Ann_Chain_Slugger.wav",						//
	"MV_Ann_Piercing.wav",							// 피어싱 샷
	"MV_Ann_Chain_HeadShot.wav",					// 연속 헤드샷
	"MV_Ann_HelmetProtection.wav",					// 헬멧 프로텍션
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//
	"",												//

													// 여기서부터는 킬메세지와 매칭하지 않습니다.
	"MV_Ann_Start_Mission_1.wav",					//
	"MV_Ann_Start_Mission_2.wav",					//
	"MV_Ann_Start_Mission_3.wav",					//
	"MV_Ann_RedTeam_Win.wav",						// 미사용? 사용 확인 필요
	"MV_Ann_BlueTeam_Win.wav",						// 미사용? 사용 확인 필요
	"MV_Ann_Bomb_Plated.wav",						// C4 설치
	"MV_Ann_Bomb_Exploded.wav",						// C4 폭파
	"MV_Ann_Bomb_Removed.wav",						// C4 제거
	"MV_Ann_Mission_Success.wav",					//
	"MV_Ann_Mission_Fail.wav",
	"MV_Ann_Mission_Warn_Helicopter.wav",
	"MV_Ann_Mission_Warn_Generator.wav",
	"UI_HUD_Training_Levelup.wav",
	"VR_Ann_Start_Mission.wav",
	"TS_Ann_Start_Mission.wav",
	"MV_Ann_Mission_Warn_Generator.wav",
	"MV_Ann_Mission_Warn_Generator.wav",
	"MV_Ann_Mission_Warn_Generator.wav",
	"MV_Ann_Mission_Warn_Generator.wav",
	"MV_Ann_PumpkinKnuckle_A_Hit_Human.wav",
	"MV_Ann_PumpkinKnuckle_B_Hit_Human.wav",
	"MV_Ann_PumpkinKnuckle_Chain_Killer.wav",
	"MV_Ann_GarenaKnuckle_A_Hit.wav",
	"UI_HUD_Revenge_Effect.wav",
	"UI_HUD_GETWatermelon_RED.wav",
	"UI_HUD_GETWatermelon_BLUE.wav",
	"",//"MV_Ann_Halloween_Kill.wav",				//GTA_HALLOWEEN_KILL
	"",//"WM_Cerberus_Shotgun_Kill.wav",				//GTA_CAREBERUS_KILL
	"WM_Convoy_FirstGet.wav",					//GTA_FIRSTGET_CONVOY
	"WM_Convoy_Get.wav",						//GTA_GET_CONVOY
	"Madness_DropWater_Ambient_ver1.wav",
	"Madness_DropWater_Ambient_ver2.wav",
	"Madness_Girlscream_Ambient_ver1.wav",
	"Madness_Girlscream_Ambient_ver2.wav",
	"WM_Convoy_Drop.wav",
	"WM_Convoy_Point.wav",
};

const char * s_RadioMessage[STBL_IDX_RADIOMSG_AUTO_US2-STBL_IDX_RADIOMSG_CMD_KR+1][3] = 
{
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR = "팀 메시지"
	{"MV_Radio_K1.wav",	"WV_Radio_K1.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR1 = "나이스 샷!"
	{"MV_Radio_K2.wav",	"WV_Radio_K2.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR2 = "와우!"
	{"MV_Radio_K3.wav",	"WV_Radio_K3.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR3 = "엄호하라"
	{"MV_Radio_K4.wav",	"WV_Radio_K4.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR4 = "선두를 맡아라"
	{"MV_Radio_K5.wav",	"WV_Radio_K5.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR5 = "이 위치를 사수하라"
	{"MV_Radio_K6.wav",	"WV_Radio_K6.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR6 = "팀을 재정비하라"
	{"MV_Radio_K7.wav",	"WV_Radio_K7.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR7 = "나를 따르라"
	{"MV_Radio_K8.wav",	"WV_Radio_K8.wav","DV_Radio_01.wav"},		//STBL_IDX_RADIOMSG_CMD_KR8 = "공격 받고 있다. 도움이 필요하다"
	{"","",""},									//STBL_IDX_RADIOMSG_CMD_KR9 = ""
	{"","",""},									//STBL_IDX_RADIOMSG_CMD_KR0 = "취소"

	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR1_1 = ""
	{"MV_Radio_E3.wav","WV_Radio_E3.wav",""},//STBL_IDX_RADIOMSG_CMD_KR2_1 = "WOW"
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR3_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR4_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR5_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR6_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR7_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR8_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_CMD_KR9_1 = ""


	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR = "요청 메시지"
	{"MV_Radio_Group_K1.wav",	"WV_Radio_Group_K1.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR1 =   "A 지역"
	{"MV_Radio_Group_K2.wav",	"WV_Radio_Group_K2.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR2 =	"B 지역"
	{"MV_Radio_Group_K3.wav",	"WV_Radio_Group_K3.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR3 =	"전진"
	{"MV_Radio_Group_K4.wav",	"WV_Radio_Group_K4.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR4 =	"후퇴"
	{"MV_Radio_Group_K5.wav",	"WV_Radio_Group_K5.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR5 =	"흩어지지 마라"
	{"MV_Radio_Group_K6.wav",	"WV_Radio_Group_K6.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR6 =	"위치를 잡아라"
	{"MV_Radio_Group_K7.wav",	"WV_Radio_Group_K7.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR7 =	"정면으로 돌격하라"
	{"MV_Radio_Group_K8.wav",	"WV_Radio_Group_K8.wav","DV_Radio_02.wav"},	//STBL_IDX_RADIOMSG_REQ_KR8 =	"상황을 보고하라"
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR9 =	""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR0 = "취소"

	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR1_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR2_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR3_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR4_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR5_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR6_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR7_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR8_1 = ""
	{"","",""},//STBL_IDX_RADIOMSG_REQ_KR9_1 = ""

	{"","",""},//STBL_IDX_RADIOMSG_STT_KR = "보고 메시지"
	{"MV_Radio_Answer_K1.wav","WV_Radio_Answer_K1.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR1 =   "알았다"
	{"MV_Radio_Answer_K2.wav","WV_Radio_Answer_K2.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR2 =	"적을 발견했다"
	{"MV_Radio_Answer_K3.wav","WV_Radio_Answer_K3.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR3 =	"지원이 필요하다"
	{"MV_Radio_Answer_K4.wav","WV_Radio_Answer_K4.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR4 =	"구역 확보 완료"
	{"MV_Radio_Answer_K5.wav","WV_Radio_Answer_K5.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR5 =	"위치에 도달했다"
	{"MV_Radio_Answer_K6.wav","WV_Radio_Answer_K6.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR6 =	"이상 없다"
	{"MV_Radio_Answer_K7.wav","WV_Radio_Answer_K7.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR7 =	"어서 피해, 곧 폭발한다!"
	{"MV_Radio_Answer_K8.wav","WV_Radio_Answer_K8.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR8 =	"거부한다"
	{"MV_Radio_Answer_K9.wav","WV_Radio_Answer_K9.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_KR9 =	"적을 사살했다"
	{"","",""},//STBL_IDX_RADIOMSG_STT_KR0 = "취소"

	{"MV_Radio_Answer_E2.wav","WV_Radio_Answer_E2.wav",""},	//STBL_IDX_RADIOMSG_STT_KR1_1 = "Roger that"
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR2_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR3_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR4_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR5_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR6_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR7_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR8_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_KR9_1 = ""

	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US = "Team message"
	{"MV_Radio_E1.wav","WV_Radio_E1.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US1 =	"Nice Shot!"
	{"MV_Radio_E2.wav","WV_Radio_E2.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US2 =	"Oh! yea"
	{"MV_Radio_E4.wav","WV_Radio_E4.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US3 =	"Cover me"
	{"MV_Radio_E5.wav","WV_Radio_E5.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US4 =	"You take point"
	{"MV_Radio_E6.wav","WV_Radio_E6.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US5 =	"Hold this position"
	{"MV_Radio_E7.wav","WV_Radio_E7.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US6 =	"Regroup team"
	{"MV_Radio_E8.wav","WV_Radio_E8.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US7 =	"Follow me"
	{"MV_Radio_E9.wav","WV_Radio_E9.wav","DV_Radio_01.wav"},					//STBL_IDX_RADIOMSG_CMD_US8 = "Taking fire. Need assistance!"
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US9 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US0 = "Cancel"

	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US1_1 = ""
	{"MV_Radio_E3.wav","WV_Radio_E3.wav",""},					//STBL_IDX_RADIOMSG_CMD_US2_1 = "Wow"
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US3_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US4_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US5_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US6_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US7_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US8_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_CMD_US9_1 = ""

	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US = "Request message"							
	{"MV_Radio_Group_E1.wav","WV_Radio_Group_E1.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US1 =	"A"
	{"MV_Radio_Group_E2.wav","WV_Radio_Group_E2.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US2 =	"B"
	{"MV_Radio_Group_E3.wav","WV_Radio_Group_E3.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US3 =	"Go Go Go"
	{"MV_Radio_Group_E4.wav","WV_Radio_Group_E4.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US4 =	"Team fall back"
	{"MV_Radio_Group_E5.wav","WV_Radio_Group_E5.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US5 =	"Stick together"
	{"MV_Radio_Group_E6.wav","WV_Radio_Group_E6.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US6 =	"Get in position"
	{"MV_Radio_Group_E7.wav","WV_Radio_Group_E7.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US7 =	"Storm the front"
	{"MV_Radio_Group_E8.wav","WV_Radio_Group_E8.wav","DV_Radio_02.wav"},		//STBL_IDX_RADIOMSG_REQ_US8 =	"Report in team"
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US9 =	""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US0 = "Cancel"

	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US1_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US2_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US3_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US4_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US5_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US6_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US7_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US8_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_REQ_US9_1 = ""

	{"","",""},//STBL_IDX_RADIOMSG_STT_US = "Report message"								
	{"MV_Radio_Answer_E1.wav","WV_Radio_Answer_E1.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US1 =	"Affirmative"
	{"MV_Radio_Answer_E3.wav","WV_Radio_Answer_E3.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US2 =	"Enemy spotted"
	{"MV_Radio_Answer_E4.wav","WV_Radio_Answer_E4.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US3 =	"Need back up"
	{"MV_Radio_Answer_E5.wav","WV_Radio_Answer_E5.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US4 =	"Sector clear"
	{"MV_Radio_Answer_E6.wav","WV_Radio_Answer_E6.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US5 =	"I'm in position"
	{"MV_Radio_Answer_E7.wav","WV_Radio_Answer_E7.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US6 =	"Reporting in"
	{"MV_Radio_Answer_E8.wav","WV_Radio_Answer_E8.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US7 =	"Get out of there, It`s gonna blow!"
	{"MV_Radio_Answer_E9.wav","WV_Radio_Answer_E9.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US8 =	"Negative"
	{"MV_Radio_Answer_E10.wav","WV_Radio_Answer_E10.wav","DV_Radio_03.wav"},	//STBL_IDX_RADIOMSG_STT_US9 =	"Enemy down"
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US0 = "Cancel"

	{"MV_Radio_Answer_E2.wav","WV_Radio_Answer_E2.wav",""},//STBL_IDX_RADIOMSG_STT_US1_1 = "Roger that"
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US2_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US3_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US4_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US5_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US6_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US7_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US8_1 = ""
	{"","",""},												//STBL_IDX_RADIOMSG_STT_US9_1 = ""

	{"","",""},//STBL_IDX_RADIOMSG_AUTO_KR = ""
	{"MV_Auto_K1.wav","WV_Auto_K1.wav",""},//STBL_IDX_RADIOMSG_AUTO_KR1 = "수류탄 투척!"
	{"MV_Auto_K2.wav","WV_Auto_K2.wav",""},//STBL_IDX_RADIOMSG_AUTO_KR2 = "작전 종료"

	{"","",""},//STBL_IDX_RADIOMSG_AUTO_US = ""
	{"MV_Auto_E1.wav","WV_Auto_E1.wav",""},//STBL_IDX_RADIOMSG_AUTO_US1 = "Fire in the hole!"
	{"MV_Auto_E2.wav","WV_Auto_E2.wav",""}//STBL_IDX_RADIOMSG_AUTO_US2 = "Good Job Team"
};

const char * s_ObjectSoundName[ GSND_OBJECT_COUNT] =
{
	"OB_Stormtube_RepairTrain.wav",
	"OB_Stormtube_RepairTrain_move.wav",
	"OB_Exp_Uh60_Missile_Alarm.wav",
	"OB_Exp_Uh60_Damage.wav",
	"Space_JumpHold_01.wav",
	"Space_Get_Item_05.wav"
};


const char * s_RouletteSoundName[ GSND_ROULETTE_COUNT] =
{
	"Lottery_Ggwang_sound.wav",
	"Lottery_Want_Sound.wav",
	"Lottery_Daebak_sound.wav",
	"Lottery_Button_Start.wav",
	"Lottery_Button_Shuffle.wav",
	"Lottery_Button_Clear.wav",
	"Lottery_Item_Click.wav",
	"Lottery_Roulette_Quit1.wav",
	"Lottery_Roulette_Quit2.wav",
	"Lottery_Roulette_Quit3.wav",
	"Lottery_Roulette_Quit4.wav",
	"Lottery_Roulette_Quit5.wav",
	"Lottery_Roulette_Quit6.wav",
	"Lottery_Roulette_Quit7.wav",
	"Lottery_Roulette_Quit8.wav",
	"Lottery_Roulette_Quit9.wav",
	"Lottery_Roulette_Quit10.wav",
	"Lottery_Roulette_Quit11.wav",
	"Lottery_Roulette_Quit12.wav",
	"Lottery_Roulette_Quit13.wav",
};

const char * s_LotterySoundName[GSND_LOTTERY_COUNT] = 
{
	"WM_lottery_WeaponLoad.wav",
	"WM_lottery_BuyyonBuy.wav",
	"WM_lottery_CapsuleDrop(One).wav",
	"WM_lottery_CapsuleDrop(Five).wav",
	"WM_lottery_CapsuleDrop(Ten).wav",
	"WM_lottery_Check.wav",
	"WM_lottery_Congratulation.wav",
	"WM_lottery_Lucky.wav",
};

CGameSoundManager::CGameSoundManager(void)
{
	m_pSndCtx				= nullptr;
	m_nCurReserveIndex		= 0;
	m_nInfoIndex			= 0;

	i3mem::FillZero( m_pBackGroundMusic, sizeof( m_pBackGroundMusic));
	i3mem::FillZero( m_ReserveInfo, sizeof( m_ReserveInfo));

	m_nCurrentRadioSndIndex	= 0;

	m_bEnableOutStandingSound = false;
	m_bInnerEnableOutStanding = false;

	m_bPlayBGM = false;

	INT32 i, j;

	for( i = 0; i < SNDINFO_READY_CNT; ++i)
	{
		m_RadioSndInfo[i].Init();
	}

	for( i = 0; i < I3_TERRAIN_TYPE_COUNT; ++i)
	{
		for( j = 0; j< GTBT_MAX_COUNT; ++j)
		{
			m_pSndByTerrainType[i][j] = nullptr;
		}
	}

	for( i = 0; i < 2; ++i)
	{
		for( j = 0; j < GTBCA_MAX_COUNT; ++j)
		{
			m_pSndByCharacterAction[i][j] = nullptr;
		}
	}

	for( i = 0; i < GTBD_MAX_COUNT; ++i)
	{
		for( j = 0; j < GTBDA_MAX_COUNT; ++j)
		{
			m_pSndDinoActionSound[i][j] = nullptr;
		}
	}


	for( i = 0; i < 3; ++i)
	{
		for( j = 0; j < GTBWS_MAX_COUNT; ++j)
		{
			m_pSndByWeaponState[i][j] = nullptr;
		}
	}

	for( i = 0; i < GTFW_C4_MAX_COUNT; ++i)
	{
		m_pSndByWeaponState_C4[i] = nullptr;
	}

	for( i = 0; i < GTA_MAX_COUNT; ++i)
	{
		m_pSndAnnounce[i] = nullptr;
	}

	for( i = 0; i < GSND_RADIO_MSG_CNT; ++i)
	{
		for( j = 0; j < 3; ++j)						// 2-->3 .. i3FixedMemory풀의 영향으로 기존에는 i3ElementBase기반의 많은 클래스들이 해당풀구현에 의해 자동 널값초기화되었었음..
		{											// 현재는 풀이 교체되는 상태이므로, 이런 널값 초기화를 기대하면 안됨...(2012.08.30.수빈)
			m_pSndRadioMsg[i][j] = nullptr;
		}
	}

	for( i = 0; i < SNDINFO_READY_CNT; ++i)
	{
		m_pInfo[i] = nullptr;
	}

	for( i = 0; i < 3; i++)
	{
		for( j = 0; j < GTBNPC_MAX_COUNT; j++)
			m_pSndByNPCAction[ i][ j] = nullptr;
	}

	for( i = 0; i < GSND_OBJECT_COUNT; i++)
	{
		m_pSndByObject[ i] = nullptr;
	}

	for( i = 0; i < GSND_ROULETTE_COUNT; i++)
	{
		m_pSndByRoulette[ i] = nullptr;
	}

	for ( i = 0; i < GSND_LOTTERY_COUNT; i++)
	{
		m_pSndByLottery[i] = nullptr;
	}

	for( i = 0; i < GSND_LOBBY_AMBIENT_COUNT; i++)
	{
		m_pSndByLobbyAmbient[ i] = nullptr;

		m_LobbyAmbientSndInfo[ i]._playInfo = nullptr;
		m_LobbyAmbientSndInfo[ i]._playState = nullptr;
	}

	for( i = 0; i < I3_TERRAIN_TYPE_COUNT; i++)
	{
		for( j = 0; j < GTTBDTR_MAX_COUNT; j++)
		{
			m_pSndDinoTerrainTypeSound[ i][ j] = nullptr;
		}
	}
}

CGameSoundManager::~CGameSoundManager(void)
{
	StopBGM(0.0f);

	ReleaseTerrainTypeSnd();
	ReleaseCharacterActionSnd();
	ReleaseWeaponStateSnd();
	ReleaseWeapon_C4StateSnd();
	ReleaseAnnounceSnd();
	ReleaseRadioMessage();
	ReleaseObjectSnd();
	ReleaseLobbyAmbientSnd();
	ReleaseDinoActionSound();
	ReleaseDinoTerrainTypeSound();
	ReleaseRouletteShopSound();
	ReleaseLotterySound();

	I3_SAFE_RELEASE( m_pSndCtx);

	ReleaseWeaponKillSound();

	for( INT32 i = 0; i < SNDINFO_READY_CNT; ++i)
	{
		I3_SAFE_RELEASE( m_pInfo[i]);
	}
}

void CGameSoundManager::Create( i3SoundContext * pCtx)
{
	I3_REF_CHANGE( m_pSndCtx, pCtx);
	
	m_nInfoIndex = 0;
	for( INT32 i = 0; i < SNDINFO_READY_CNT; ++i)
	{
		m_pInfo[i] = i3SoundPlayInfo::new_object();
	}

	CreateChannelGroup();

	// 기본 사운드는 로드해놓고 사용하자.
	InitTerrainTypeSnd();
}

void CGameSoundManager::CreateChannelGroup(void)
{
	if( m_pSndCtx == nullptr)
		return;

	INT32 idx = m_pSndCtx->ChannelGroup_Add( "Normal");
	I3ASSERT( idx == (INT32)PBSND_CHANNEL_NORMAL);

	idx = m_pSndCtx->ChannelGroup_Add( "OutStanding");
	I3ASSERT( idx == (INT32)PBSND_CHANNEL_OUTSTANDING);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		----	R E L E A S E   -----
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CGameSoundManager::ReleaseSoundFromRes( i3GameResSound * pRes)
{
	/*if( pRes != nullptr)
	{
		i3NamedResourceManager * pMng = i3ResourceFile::GetResourceManager( i3Sound::static_meta());
		for( UINT32 i = 0; i < pMng->GetResourceCount(); i++)
		{
			i3ResourceObject * pObj = pMng->GetResource( i);
			if( pObj == pRes->getSound()->getSound())
			{
				if( pObj->GetRefCount() <= 3 )
				{
					pRes->getSound()->setSound(nullptr);
					pMng->DeleteResource( i);

					return true;
				}
			}
		}
	}*/

	return false;
}

void CGameSoundManager::ReleaseSoundFromPlayInfo( i3SoundPlayInfo * pInfo)
{
	if( pInfo != nullptr)
	{
		i3ResourceManager * pResMng = g_pFramework->GetResourceManager();
		for( INT32 i = 0; i < pResMng->GetResourceCount(); i++)
		{
			i3ResourceObject * pObj = pResMng->GetResource( i);
			if( i3::same_of<i3GameResSound*>(pObj))
			{
				if( ((i3GameResSound*)pObj)->getSound()->getSound() == pInfo->getSound())
				{
					if( ReleaseSoundFromRes( (i3GameResSound*)pObj))	// i3Sound 삭제
					{
						pResMng->RemoveResource( (i3GameResSound*)pObj);
					}

					return;
				}
			}
		}
	}
}

void	CGameSoundManager::ReleaseTerrainTypeSnd()
{
	for( INT32 i = 0; i < I3_TERRAIN_TYPE_COUNT; ++i)
	{
		for( INT32 j = 0; j< GTBT_MAX_COUNT; ++j)
		{
			if( ReleaseSoundFromRes( m_pSndByTerrainType[i][j]) )
				g_pFramework->GetResourceManager()->RemoveResource( m_pSndByTerrainType[i][j]);

			I3_SAFE_RELEASE( m_pSndByTerrainType[i][j]);
		}
	}
}

void	CGameSoundManager::ReleaseDinoActionSound()
{
	for( INT32 i = 0;i < GTBD_MAX_COUNT; ++i)
	{
		for( INT32 j = 0; j < GTBDA_MAX_COUNT; ++j)
		{
			if( ReleaseSoundFromRes( m_pSndDinoActionSound[i][j]) )
				g_pFramework->GetResourceManager()->RemoveResource( m_pSndDinoActionSound[i][j]);

			I3_SAFE_RELEASE( m_pSndDinoActionSound[i][j]);
		}
	}
}

void	CGameSoundManager::ReleaseLobbyAmbientSnd(void)
{
#if !defined( ENABLE_LOBBY_AMBIENT_SOUND)
	return;
#else 
	for( INT32 i = 0; i< GSND_LOBBY_AMBIENT_COUNT; ++i)
	{
		I3_SAFE_RELEASE( m_pSndByLobbyAmbient[i]);
	}
#endif
}

void	CGameSoundManager::ReleaseCharacterActionSnd()
{
	INT32 i,j;

	for( i = 0; i < 2; ++i)
	{
		for( j = 0; j < GTBCA_MAX_COUNT; ++j)
		{
			if( m_pSndByCharacterAction[i][j] != nullptr)
			{
				if( ReleaseSoundFromRes( m_pSndByCharacterAction[i][j]) )
					g_pFramework->GetResourceManager()->RemoveResource( m_pSndByCharacterAction[i][j]);

				I3_SAFE_RELEASE( m_pSndByCharacterAction[i][j]);
			}
		}
	}
}

void	CGameSoundManager::ReleaseWeaponStateSnd()
{
	INT32 i, j;
	for( i = 0; i < 3; ++i)
	{
		for(j = 0; j < GTBWS_MAX_COUNT; ++j)
		{
			if( ReleaseSoundFromRes( m_pSndByWeaponState[i][j]) )
				g_pFramework->GetResourceManager()->RemoveResource( m_pSndByWeaponState[i][j]);

			I3_SAFE_RELEASE( m_pSndByWeaponState[i][j]);
		}
	}
}

void CGameSoundManager::ReleaseWeapon_C4StateSnd()
{
	for( INT32 i = 0; i < GTFW_C4_MAX_COUNT; ++i)
	{
		if( ReleaseSoundFromRes( m_pSndByWeaponState_C4[i]) )
			g_pFramework->GetResourceManager()->RemoveResource( m_pSndByWeaponState_C4[i]);

		I3_SAFE_RELEASE( m_pSndByWeaponState_C4[i]);
	}
}

void CGameSoundManager::ReleaseAnnounceSnd()
{
	for( INT32 i = 0; i < GTA_MAX_COUNT; ++i)
	{
		if( ReleaseSoundFromRes( m_pSndAnnounce[i]) )
			g_pFramework->GetResourceManager()->RemoveResource( m_pSndAnnounce[i]);

		I3_SAFE_RELEASE( m_pSndAnnounce[i]);
	}
}

void CGameSoundManager::ReleaseRadioMessage()
{
	for( INT32 i = 0; i < GSND_RADIO_MSG_CNT; ++i)
	{
		for( INT32 j = 0; j < 3; ++j)
		{
			if( ReleaseSoundFromRes( m_pSndRadioMsg[i][j]) )
				g_pFramework->GetResourceManager()->RemoveResource( m_pSndRadioMsg[i][j]);

			I3_SAFE_RELEASE( m_pSndRadioMsg[i][j]);
		}
	}
}

void CGameSoundManager::ReleaseRouletteShopSound( void)
{
	for( INT32 i = 0; i < GSND_ROULETTE_COUNT; i++)
	{
		if( ReleaseSoundFromRes( m_pSndByRoulette[ i]))
			g_pFramework->GetResourceManager()->RemoveResource( m_pSndByRoulette[ i]);

		I3_SAFE_RELEASE( m_pSndByRoulette[ i]);
	}
}

void CGameSoundManager::ReleaseLotterySound( void)
{
	for( INT32 i = 0; i < GSND_LOTTERY_COUNT; i++)
	{
		if( ReleaseSoundFromRes( m_pSndByLottery[ i]))
			g_pFramework->GetResourceManager()->RemoveResource( m_pSndByLottery[ i]);

		I3_SAFE_RELEASE( m_pSndByLottery[ i]);
	}
}

void CGameSoundManager::InitWeaponKillSound()
{
	const INT32 count = g_pWeaponInfoDataBase->GetWeaponKillSoundCount();

	for (INT32 i = 0; i < count; ++i)
	{
		i3GameResSound * pRes = (i3GameResSound*)g_pFramework->QuaryResource(g_pWeaponInfoDataBase->GetWeaponSkillSoundFileName(i).c_str());
		i3GameResSound * pTemp = nullptr;
		I3_REF_CHANGE(pTemp, pRes);

		m_vSndByWeaponKill.push_back(pTemp);
#if defined( I3_DEBUG)
		if (pRes == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", g_pWeaponInfoDataBase->GetWeaponSkillSoundFileName(i));
		}
#endif
	}
}

void CGameSoundManager::ReleaseWeaponKillSound()
{
	for (INT32 i = 0; i < (INT32)m_vSndByWeaponKill.size(); ++i)
	{
		if (ReleaseSoundFromRes(m_vSndByWeaponKill[i]))
			g_pFramework->GetResourceManager()->RemoveResource(m_vSndByWeaponKill[i]);

		I3_SAFE_RELEASE(m_vSndByWeaponKill[i]);
	}

	m_vSndByWeaponKill.clear();
}

void CGameSoundManager::ReleaseObjectSnd( void)
{
	for( INT32 i = 0; i < GSND_OBJECT_COUNT; i++)
	{
		if( ReleaseSoundFromRes( m_pSndByObject[ i]))
			g_pFramework->GetResourceManager()->RemoveResource( m_pSndByObject[ i]);

		I3_SAFE_RELEASE( m_pSndByObject[ i]);
	}
}

void CGameSoundManager::ReleaseDinoTerrainTypeSound( void)
{
	INT32 i,j;
	for( i = 0; i < I3_TERRAIN_TYPE_COUNT; i++)
	{
		for( j = 0; j < GTTBDTR_MAX_COUNT; j++)
		{
			if( ReleaseSoundFromRes( m_pSndDinoTerrainTypeSound[ i][ j]))
				g_pFramework->GetResourceManager()->RemoveResource( m_pSndDinoTerrainTypeSound[ i][ j]);

			I3_SAFE_RELEASE( m_pSndDinoTerrainTypeSound[ i][ j]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		----	I N I T I A L I Z E   -----
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameSoundManager::InitTerrainTypeSnd( void)
{
	for( INT32 i = 0; i < I3_TERRAIN_TYPE_COUNT; i++)
	{
		for( INT32 j = 0; j < GTBT_MAX_COUNT; j++)
		{
			if( i3::generic_empty( s_TerrainTypeSoundName[i][j]) == false)
			{
				i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( s_TerrainTypeSoundName[i][j]);
				
				I3_REF_CHANGE( m_pSndByTerrainType[i][j], pRes);
				if( m_pSndByTerrainType[i][j] != nullptr)
					m_pSndByTerrainType[i][j]->setCommonRes(true);

			#if defined( I3_DEBUG)
				if( pRes == nullptr)
				{
				#ifdef BREAK_LOAD_SOUND
					I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_TerrainTypeSoundName[i][j]);
				#else
					I3PRINTLOG(I3LOG_NOTICE,"Cannot find sound file %s", s_TerrainTypeSoundName[i][j]);
				#endif
				}
			#endif
			}
		}
	}
}

void CGameSoundManager::InitDinoActionSound(void)
{
	INT32 i;
	for( i = 0;i < GTBD_MAX_COUNT; ++i)
	{
		for( INT32 j = 0; j < GTBDA_MAX_COUNT; j++)
		{
			if( i3::generic_empty( s_DinoActionSoundName[i][j]) == false)
			{
				i3GameResSound * pRes = (i3GameResSound*)g_pFramework->QuaryResource( s_DinoActionSoundName[i][j]);

				I3_REF_CHANGE( m_pSndDinoActionSound[i][j], pRes);

			#if defined( I3_DEBUG)
				if( pRes == nullptr)
				{
				#ifdef BREAK_LOAD_SOUND
					I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_DinoActionSoundName[i][j]);
				#else
					I3PRINTLOG(I3LOG_NOTICE,"Cannot find sound file %s", s_DinoActionSoundName[i][j]);
				#endif
				}
			#endif
			}
		}
	}
}

void CGameSoundManager::InitDinoTerrainTypeSound(void)
{
	INT32 i;
	for( i = 0;i < I3_TERRAIN_TYPE_COUNT; ++i)
	{
		for( INT32 j = 0; j < GTTBDTR_MAX_COUNT; j++)
		{
			i3GameResSound * pRes = (i3GameResSound*)g_pFramework->QuaryResource( s_TerrainTypeDinoSoundName[i][j]);
			I3_REF_CHANGE( m_pSndDinoTerrainTypeSound[i][j], pRes);

		#if defined( I3_DEBUG)
			if( pRes == nullptr)
			{
			#ifdef BREAK_LOAD_SOUND
				I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_TerrainTypeDinoSoundName[i][j]);
			#else
				I3PRINTLOG(I3LOG_NOTICE,"Cannot find sound file %s", s_TerrainTypeDinoSoundName[i][j]);
			#endif
			}
		#endif
		}
	}
}

void CGameSoundManager::InitLobbyAmbientSnd(void)
{
#if !defined( ENABLE_LOBBY_AMBIENT_SOUND)
	return;

#else
	for(INT32 i=0; i<GSND_LOBBY_AMBIENT_COUNT; i++)
	{
		if( i3::generic_string_size( s_AmbientSoundFile[i]) > 0)
		{
			i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( s_AmbientSoundFile[i]);
			I3ASSERT( pRes != nullptr);

			I3_REF_CHANGE( m_pSndByLobbyAmbient[i], pRes);
		}

		I3ASSERT( m_LobbyAmbientSndInfo[i]._playInfo == nullptr);
		I3ASSERT( m_LobbyAmbientSndInfo[i]._playState == nullptr);

		m_LobbyAmbientSndInfo[i]._playInfo = nullptr;
		m_LobbyAmbientSndInfo[i]._playState = nullptr;
	}

#endif
}

void CGameSoundManager::InitCharacterActionSnd()
{
	for( INT32 i = 0; i < 2; ++i)
	{
		for( INT32 j = 0; j < GTBCA_MAX_COUNT; ++j)
		{
			if( i3::generic_empty( s_CharacterActionSoundName[i][j]) == false)
			{
				i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( s_CharacterActionSoundName[i][j]);
				I3_REF_CHANGE( m_pSndByCharacterAction[i][j], pRes);

			#if defined( I3_DEBUG)
				if( pRes == nullptr)
				{
				#ifdef BREAK_LOAD_SOUND
					I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_CharacterActionSoundName[i][j]);
				#else
					I3PRINTLOG(I3LOG_NOTICE,"Cannot find sound file %s", s_CharacterActionSoundName[i][j]);
				#endif
				}
			#endif
			}
		}
	}
}

void CGameSoundManager::InitWeaponStateSnd()
{
	for( INT32 i = 0; i < 3; ++i)
	{
		for( INT32 j = 0; j < GTBWS_MAX_COUNT; ++j)
		{
			if( i3::generic_empty( s_WeaponStateSoundName[i][j]) == false)
			{
				i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( s_WeaponStateSoundName[i][j]);
				I3_REF_CHANGE( m_pSndByWeaponState[i][j], pRes);

			#if defined( I3_DEBUG)
				if( pRes == nullptr)
				{
				#ifdef BREAK_LOAD_SOUND
					I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_WeaponStateSoundName[i][j]);
				#else
					I3PRINTLOG(I3LOG_NOTICE,"Cannot find sound file %s", s_WeaponStateSoundName[i][j]);
				#endif
				}
			#endif
			}
		}
	}
}

void CGameSoundManager::InitWeapon_C4StateSnd()
{
	for( INT32 i = 0; i < GTFW_C4_MAX_COUNT; ++i)
	{
		if( i3::generic_empty( s_WeaponState_C4SoundName[i]) == false)
		{
			i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( s_WeaponState_C4SoundName[i]);
			I3_REF_CHANGE( m_pSndByWeaponState_C4[i], pRes);

		#if defined( I3_DEBUG)
			if( pRes == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_WeaponState_C4SoundName[i]);
			}
		#endif
		}
	}
}

void CGameSoundManager::InitAnnounceSnd()
{
	for( INT32 i = 0;i < GTA_MAX_COUNT; ++i)
	{
		if( i3::generic_empty( s_AnnounceSoundName[i]) == false)
		{
			i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( s_AnnounceSoundName[i]);
			I3_REF_CHANGE( m_pSndAnnounce[i], pRes);

		#if defined( I3_DEBUG)
			if( pRes == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_AnnounceSoundName[i]);
			}
		#endif
		}
	}
}

void CGameSoundManager::InitRadioMessage()
{
	for( INT32 i = 0; i < GSND_RADIO_MSG_CNT; ++i)
	{
		for( INT32 j = 0; j < 3; ++j)
		{
			if( i3::generic_empty( (char*)s_RadioMessage[i][j]) == false)
			{
				i3GameResSound * pRes = (i3GameResSound*)g_pFramework->QuaryResource( (char*)s_RadioMessage[i][j]);
				I3_REF_CHANGE( m_pSndRadioMsg[i][j], pRes);

			#if defined( I3_DEBUG)
				if( pRes == nullptr)
				{
					I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_RadioMessage[i][j]);
				}
			#endif
			}
		}
	}
}


void CGameSoundManager::InitObjectSnd( STAGE_ID stageID )
{
	i3GameResSound * pRes;
	// 해당 스테이지에 진입시 로딩하도록 수정..
	if( StageID::GetStageUID(stageID) == STAGE_UID_STORMTUBE)
	{// StormTube map
		pRes = (i3GameResSound*) g_pFramework->QuaryResource( s_ObjectSoundName[ GSND_OBJECT_STORMTUBE_RT_IDLE]);
		I3_REF_CHANGE( m_pSndByObject[ GSND_OBJECT_STORMTUBE_RT_IDLE], pRes);

		pRes = (i3GameResSound*) g_pFramework->QuaryResource( s_ObjectSoundName[ GSND_OBJECT_STORMTUBE_RT_MOVE]);
		I3_REF_CHANGE( m_pSndByObject[ GSND_OBJECT_STORMTUBE_RT_MOVE], pRes);
	}
	else if( StageID::GetStageUID(stageID) == STAGE_UID_OUTPOST || 
			 StageID::GetStageUID(stageID) == STAGE_UID_HELISPOT )
	{// UH60
		pRes = (i3GameResSound*) g_pFramework->QuaryResource( s_ObjectSoundName[ GSND_OBJECT_UH60_ALARM]);
		I3_REF_CHANGE( m_pSndByObject[ GSND_OBJECT_UH60_ALARM], pRes);

		pRes = (i3GameResSound*) g_pFramework->QuaryResource( s_ObjectSoundName[ GSND_OBJECT_UH60_DAMAGE]);
		I3_REF_CHANGE( m_pSndByObject[ GSND_OBJECT_UH60_DAMAGE], pRes);
	}

	pRes = (i3GameResSound*)g_pFramework->QuaryResource(s_ObjectSoundName[GSND_OBJECT_JUMPHOLD]);
	I3_REF_CHANGE(m_pSndByObject[GSND_OBJECT_JUMPHOLD], pRes);
	pRes = (i3GameResSound*)g_pFramework->QuaryResource(s_ObjectSoundName[GSND_OBJECT_BATTLE_USE_ITEM]);
	I3_REF_CHANGE(m_pSndByObject[GSND_OBJECT_BATTLE_USE_ITEM], pRes);
}
void CGameSoundManager::InitRouletteShopSound(void)
{
	for( INT32 i = 0; i < GSND_ROULETTE_COUNT; i++)
	{
		if( i3::generic_empty( (char*) s_RouletteSoundName[ i] ) == false)
		{
			i3GameResSound * pRes = (i3GameResSound*) g_pFramework->QuaryResource( (char*) s_RouletteSoundName[ i]);
			I3_REF_CHANGE( m_pSndByRoulette[ i], pRes);

		#if defined( I3_DEBUG)
			if( pRes == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_RouletteSoundName[i]);
			}
		#endif
		}
	}


}

void CGameSoundManager::InitLotterySound(void)
{
	for( INT32 i = 0; i < GSND_LOTTERY_COUNT; i++)
	{
		if( i3::generic_empty( (char*) s_LotterySoundName[ i] ) == false)
		{
			i3GameResSound * pRes = (i3GameResSound*) g_pFramework->QuaryResource( (char*) s_LotterySoundName[ i]);
			I3_REF_CHANGE( m_pSndByLottery[ i], pRes);

#if defined( I3_DEBUG)
			if( pRes == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL, "Cannot find sound file %s", s_LotterySoundName[i]);
			}
#endif
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		----	P L A Y B A C K   -----
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
i3SoundPlayInfo	*	CGameSoundManager::_GetFreeSndInfo( INT32 * npCnt)
{
	if( m_nInfoIndex < 0 || m_nInfoIndex >= SNDINFO_READY_CNT)
	{
		I3PRINTLOG(I3LOG_WARN,  "SoundInfo의 인덱스가 맞지 않습니다. 계속하시려면 Continue");

		m_nInfoIndex = 0;
	}

	if( *npCnt >= SNDINFO_READY_CNT)
	{
		I3TRACE("현재 사용가능한 SndInfo가 없습니다.버퍼의 개수를 늘려주세요\n");
		return nullptr;
	}

	i3SoundPlayInfo * pInfo = m_pInfo[m_nInfoIndex];
	if( pInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "SoundInfo의 초기화가 제대로 이루어지지 않았습니다.");
		return nullptr;
	}

	INT32 nCnt = *npCnt;
	*npCnt = nCnt + 1;

	m_nInfoIndex = ( (m_nInfoIndex + 1) >= SNDINFO_READY_CNT) ? 0 : m_nInfoIndex + 1;

	if( pInfo->GetRefCount() > 1)
	{
		//I3PRINTLOG(I3LOG_NOTICE,"사용중인 버퍼");
		return _GetFreeSndInfo(npCnt);
	}

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		----	P L A Y B A C K   -----
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
i3SoundPlayState *	CGameSoundManager::StartTerrainTypeSnd( UINT32 nTerrain, UINT32 nIdx, VEC3D * vPos, bool bListener)
{
	if( (nTerrain >= I3_TERRAIN_TYPE_COUNT) || (nIdx >= GTBT_MAX_COUNT))
	{
		I3PRINTLOG(I3LOG_WARN,  "Cannot play Terrain Effect Sound. Invalid terrain type!! (65323)");

		return nullptr;
	}

	//I3ASSERT( nTerrain < I3_TERRAIN_TYPE_COUNT);
	//I3ASSERT( nIdx < GTBT_MAX_COUNT);

	i3GameResSound * pSndRes = m_pSndByTerrainType[nTerrain][nIdx];

	SND_TRACE( "%s : Listener: %d,  %f, %f, %f\n", s_TerrainTypeSoundName[nTerrain][nIdx], bListener, getX( vPos), getY( vPos), getZ( vPos));

	return _StartSound( pSndRes, vPos, bListener);
}

i3SoundPlayState *	CGameSoundManager::StartDinoActionSound( UINT32 type, UINT32 idx, VEC3D * vPos, bool bListener)
 {
/*
	 if( type < 0 || type >=2)
	{
		I3PRINTLOG(I3LOG_WARN, "cann't play dinoaction sound: type(%d), idx(%d)", type, idx);

		return nullptr;
	}
*/
	i3GameResSound * pSndRes = m_pSndDinoActionSound[type][idx];

	SND_TRACE( "%s : Listener: %d,  %f, %f, %f\n", s_DinoActionSoundName[type][idx], bListener, getX( vPos), getY( vPos), getZ( vPos));

	return _StartSound( pSndRes, vPos, bListener);
}

i3SoundPlayState *	CGameSoundManager::StartDinoTerrainTypeSnd( UINT32 nTerrain, UINT32 nIdx, VEC3D * vPos, bool bListener)
{
	if( (nTerrain >= I3_TERRAIN_TYPE_COUNT) || (nIdx >= GTTBDTR_MAX_COUNT))
	{
		I3PRINTLOG(I3LOG_WARN,  "Cannot play Terrain Effect Sound. Invalid terrain type!! (65323)");

		return nullptr;
	}

	i3GameResSound * pSndRes = m_pSndDinoTerrainTypeSound[nTerrain][nIdx];

	SND_TRACE( "%s : Listener: %d,  %f, %f, %f\n", s_TerrainTypeDinoSoundName[nTerrain][nIdx], bListener, getX( vPos), getY( vPos), getZ( vPos));

	return _StartSound( pSndRes, vPos, bListener);
}

i3SoundPlayState * CGameSoundManager::StartCharacterActionSnd( UINT32 nAction, UINT32 nIdx, VEC3D * vPos, bool bListener)
{
	I3ASSERT( nAction < GTBCA_MAX_COUNT);
	I3ASSERT( nIdx < 2);

	i3GameResSound * pSndRes = m_pSndByCharacterAction[nIdx][nAction];

	if( pSndRes == nullptr)
	{
		// Load해야한다.
		pSndRes = (i3GameResSound*) g_pFramework->QuaryResource( s_CharacterActionSoundName[nIdx][nAction]);
		I3_REF_CHANGE( m_pSndByCharacterAction[nIdx][nAction], pSndRes);
	}

	SND_TRACE( "%s : Listener: %d,  %f, %f, %f\n", s_CharacterActionSoundName[nIdx][nAction], bListener, getX( vPos), getY( vPos), getZ( vPos));

	return _StartSound( pSndRes, vPos, bListener);
}

i3SoundPlayState * CGameSoundManager::StartWeaponStateSnd( UINT32 nState, UINT32 nIdx, VEC3D * vPos, bool bListener)
{
	I3ASSERT( nState < GTBWS_MAX_COUNT);
	I3ASSERT( nIdx < 3);

	i3GameResSound * pSndRes = m_pSndByWeaponState[nIdx][nState];

	if( nState == GTBWS_ON_HIT_THROW)
	{
		if( m_pSndCtx == nullptr)
			return nullptr;

		i3SoundPlayInfo * pInfo = _PrepareStartSound( pSndRes, bListener, 0, 1);
		if( pInfo != nullptr)
		{
			return m_pSndCtx->Play( pInfo, vPos, PBSND_CHANNEL_OUTSTANDING);
		}

		return nullptr;
	}

	return _StartSound( pSndRes, vPos, bListener);
}

i3SoundPlayState * CGameSoundManager::StartWeapon_C4StateSnd( UINT32 nState, VEC3D * vPos, bool bListener, bool bLoop)
{
	I3ASSERT( nState < GTFW_C4_MAX_COUNT);
	
	i3GameResSound * pSndRes = m_pSndByWeaponState_C4[nState];

	SND_TRACE( "%s : Listener: %d,  %f, %f, %f\n", s_WeaponState_C4SoundName[nState], bListener, getX( vPos), getY( vPos), getZ( vPos));

	return _StartSound( pSndRes, vPos, bListener, bLoop);
}

i3SoundPlayState * CGameSoundManager::StartAnnounceMessage( UINT32 nIndex, VEC3D * vPos, bool bListener)
{
	I3ASSERT( nIndex < GTA_MAX_COUNT);

	i3GameResSound * pSndRes = m_pSndAnnounce[nIndex];

	//SND_TRACE( "%s : Listener: %d,  %f, %f, %f\n", s_AnnounceSoundName[nIndex], bListener, getX( vPos), getY( vPos), getZ( vPos));
	//SND_TRACE( "%s\n", s_AnnounceSoundName[nIndex]);

	return _StartSound( pSndRes, vPos, bListener, false, false);
}

void CGameSoundManager::ResetRadioMessageInfo()
{
	for( INT32 i = 0; i < SNDINFO_READY_CNT; ++i)
	{
		RADIO_MSGSND_INFO * pInfo = &m_RadioSndInfo[ i];
		pInfo->Init();
	}
}

i3SoundPlayState * CGameSoundManager::StartRadioMessage( UINT32 nIndex, UINT32 nSex, CGameCharaBase * pPlayer)
{
	I3ASSERT( nIndex < GSND_RADIO_MSG_CNT);
	I3ASSERT( pPlayer != nullptr);	// Radio Message는 항상 보내는 주체 대상이 있어야합니다.

	if( g_pEnvSet != nullptr && !g_pEnvSet->m_bEnableRadioMessage)	return nullptr;

	i3GameResSound * pResSnd = m_pSndRadioMsg[nIndex][nSex];

	RADIO_MSGSND_INFO * pInfo = &m_RadioSndInfo[m_nCurrentRadioSndIndex++];

	pInfo->m_rElapsedTime	= 0.0f;
	pInfo->pPlayer			= pPlayer;
	pInfo->pState			= _StartSound( pResSnd, (VEC3D*)nullptr, true, false, true);
	pInfo->m_rDuration		= 0.0f;
	pInfo->m_Sex			= nSex;

	if( pInfo->pState == nullptr)
	{
		m_nCurrentRadioSndIndex--;
		pInfo->Init();
	}
	else
	{
		pInfo->m_rDuration		= pResSnd->getSound()->getSound()->GetDuration();
	}

	//ring buffer
	if( m_nCurrentRadioSndIndex >= SNDINFO_READY_CNT)
	{
		m_nCurrentRadioSndIndex = 0;
	}

	return pInfo->pState;
}

void CGameSoundManager::_ProcessRadioMessage( REAL32 rDeltaTime, bool bForce)
{
	INT32 i;

	bool bProcessInterrupt = g_pFramework->IsBattleStage();

	for( i = 0; i < SNDINFO_READY_CNT; ++i)
	{
		RADIO_MSGSND_INFO * pInfo = &m_RadioSndInfo[ i];

		if( pInfo->pState != nullptr)
		{
			pInfo->m_rElapsedTime += rDeltaTime;

			if( pInfo->m_rElapsedTime > pInfo->m_rDuration)
			{
				pInfo->Init();
			}
			else
			{
				if( bProcessInterrupt)
				{
					if( (g_pFramework->GetState() == i3Framework::STATE_SCENE) &&
						(pInfo->pPlayer->isCharaStateMask( CHARA_STATE_DEATH)))
					{
						//라디오메시지 중에 캔슬되는 메시지가 따로 define되어잇지 않아 케릭터 액션사운드중에 하나를 추가하였심다
 						g_pSndMng->StartCharacterActionSnd( GTBCA_ON_RADIOINTERRUPT, pInfo->m_Sex, nullptr, true);

						pInfo->pState->Stop();
						pInfo->Init();
					}
				}
			}
		}
	}
}


i3SoundPlayState * CGameSoundManager::StartObjectSnd( GSND_OBJECT_TYPE nType, VEC3D * pPos, bool bLoop)
{
	I3ASSERT( nType < GSND_OBJECT_COUNT);

	i3GameResSound * pSndRes = m_pSndByObject[ nType];

	SND_TRACE( "%s : %f, %f, %f\n", m_pSndByObject[ nType], getX( pPos), getY( pPos), getZ( pPos));

	return _StartSound( pSndRes, pPos, false, bLoop);
}

i3SoundPlayState * CGameSoundManager::StartRouletteSnd( GSND_ROULETTE_TYPE nType, VEC3D * pPos, bool bLoop)
{
	I3ASSERT( nType < GSND_ROULETTE_COUNT);

	i3GameResSound * pSndRes = m_pSndByRoulette[ nType];

	SND_TRACE( "%s : %f, %f, %f\n", m_pSndByRoulette[ nType], getX( pPos), getY( pPos), getZ( pPos));

	return _StartSound( pSndRes, pPos, false, bLoop);
}

i3SoundPlayState * CGameSoundManager::StartLotterySnd( GSND_LOTTERY_TYPE nType, VEC3D * pPos, bool bLoop)
{
	I3ASSERT( nType < GSND_LOTTERY_COUNT);

	i3GameResSound * pSndRes = m_pSndByLottery[ nType];

	SND_TRACE( "%s : %f, %f, %f\n", m_pSndByLottery[ nType], getX( pPos), getY( pPos), getZ( pPos));

	return _StartSound( pSndRes, pPos, true, bLoop);
}

i3SoundPlayState * CGameSoundManager::StartWeaponKillSound(UINT32 nIndex, VEC3D * vPos, bool bListener)
{
	i3GameResSound * pSndRes = m_vSndByWeaponKill[nIndex];

	return _StartSound(pSndRes, vPos, bListener, false, false);
}

SOUND_INFO * CGameSoundManager::StartLobbyAmbientSnd( GSND_LOBBY_AMBIENT_TYPE nType, MATRIX * pMat, INT32 nVolume, bool bListener, bool bLoop)
{
#if !defined( ENABLE_LOBBY_AMBIENT_SOUND)
	return nullptr;
#else

	I3ASSERT( nType < GSND_LOBBY_AMBIENT_COUNT);

	i3GameResSound * pSndRes = m_pSndByLobbyAmbient[ nType];

	if( m_pSndCtx == nullptr)
		return nullptr;

	if( m_LobbyAmbientSndInfo[ nType]._playState != nullptr)
	{
		m_pSndCtx->Stop( m_LobbyAmbientSndInfo[ nType]._playState, true);

		I3_SAFE_RELEASE( m_LobbyAmbientSndInfo[ nType]._playState );
		I3_SAFE_RELEASE( m_LobbyAmbientSndInfo[ nType]._playInfo );
	}

	i3SoundPlayInfo * pInfo = nullptr;
	if(m_LobbyAmbientSndInfo[ nType]._playInfo != nullptr)
	{
		pInfo = m_LobbyAmbientSndInfo[ nType]._playInfo;
	}
	else
	{
		I3ASSERT( pSndRes->getSound() != nullptr);

		pInfo = i3SoundPlayInfo::new_object();
		I3ASSERT( pInfo != nullptr);

		pSndRes->getSound()->CopyTo( pInfo, I3_COPY_INSTANCE);
	}

	pInfo->AddStyle( I3SND_PROP_TYPE_EFFECT);
	pInfo->SetDefaultVolume( MIN( nVolume, 100));
	pInfo->setListenerVolume( MIN( nVolume, 100));
	pInfo->setForceStopAtStageEnd( true);
	pInfo->SetDistanceInfo( 10.0f, 3.0f, 0);

	if( bListener )
	{
		pInfo->set3DMode( I3SND_RT_3DMODE_DISABLE);
	}
	else
	{
		pInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
	}

	if( bLoop)
	{
		pInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
	}		

	i3SoundPlayState * pState = m_pSndCtx->Play( pInfo, pMat);
	if( pState != nullptr)
	{
		pState->AddRef();

		m_LobbyAmbientSndInfo[ nType]._playInfo = pInfo;
		m_LobbyAmbientSndInfo[ nType]._playState = pState;

		return &m_LobbyAmbientSndInfo[ nType];
	}

	return nullptr;

#endif
}

i3SoundPlayInfo * CGameSoundManager::_PrepareStartSound( i3GameResSound * pSound, bool bListener, bool bLoop, bool bStopAtAllStop)
{
	if( pSound == nullptr)
	{
		return nullptr;
	}

	i3SoundPlayInfo * pInfo = pSound->getSound();
	if( pInfo == nullptr)
	{
		return nullptr;
	}

	INT32 nCnt = 0;

	i3SoundPlayInfo * pInstance = _GetFreeSndInfo( &nCnt);
	if( pInstance == nullptr)
	{
		return nullptr;
	}

	pInfo->CopyTo( pInstance, I3_COPY_INSTANCE);

	if( bListener )
	{
		pInstance->set3DMode( I3SND_RT_3DMODE_DISABLE);
	}
	else
	{
		pInstance->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
	}

	if( bLoop)
	{
		pInstance->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
	}

	if( !bStopAtAllStop)
	{
		pInstance->setForceStopAtStageEnd( false);
	}
	else
	{
		pInstance->setForceStopAtStageEnd( true);
	}


	return pInstance;
}

i3SoundPlayState * CGameSoundManager::_StartSound( i3GameResSound * pSound, VEC3D * vPos, bool bListener, bool bLoop, bool bStopAtAllStop)
{
	if( m_pSndCtx == nullptr)
		return nullptr;

	i3SoundPlayInfo * pInfo = _PrepareStartSound( pSound, bListener, bLoop, bStopAtAllStop);

	if( pInfo != nullptr)
	{
		return m_pSndCtx->Play( pInfo, vPos);
	}

	return nullptr;
}

i3SoundPlayState * CGameSoundManager::_StartSound( i3GameResSound * pSound, MATRIX * pMat, bool bListener, bool bLoop, bool bStopAtAllStop)
{
	if( m_pSndCtx == nullptr)
		return nullptr;

	i3SoundPlayInfo * pInfo = _PrepareStartSound( pSound, bListener, bLoop, bStopAtAllStop);
	if( pInfo != nullptr)
	{
		return m_pSndCtx->Play( pInfo, pMat);
	}

	return nullptr;
}

INT32 CGameSoundManager::ReserveSoundPlay( INFO_TYPE nType, UINT32 nParam1, UINT32 nParam2, VEC3D * vPos, bool bListener)
{
	I3ASSERT( m_nCurReserveIndex < 32);

	if( m_pSndCtx == nullptr)
		return 0;

	MATRIX * pMat = m_pSndCtx->GetListnerMatrix();
	VEC3D vDiff;

	i3Vector::Sub( &vDiff, vPos, i3Matrix::GetPos( pMat));

	if( m_nCurReserveIndex == 0)
	{
		GSND_TERRAIN_RESERVE_INFO * pInfo = &m_ReserveInfo[0];

		pInfo->nInfoType    = nType;
		pInfo->Param_1		= nParam1;
		pInfo->Param_2		= nParam2;

		i3Vector::Copy( &pInfo->vPos, &vDiff);
		pInfo->bListener = bListener;

		m_nCurReserveIndex = 1;

		return 0;
	}

	//현재로서는 타입과 , 액션또는 재질이 같다면 같다라고 간주합니다.
	for( INT32 i = 0; i < m_nCurReserveIndex; ++i)
	{
		GSND_TERRAIN_RESERVE_INFO * pInfo = &m_ReserveInfo[i];

		if( (pInfo->nInfoType == nType) && (pInfo->Param_1 == nParam1) && (pInfo->bListener == bListener))
		{
			if( i3Vector::LengthSq( &vDiff) < i3Vector::LengthSq( &pInfo->vPos))
			{
				i3Vector::Copy( &pInfo->vPos, &vDiff);

				pInfo->Param_2 = nParam2;
			}

			return i;
		}
	}

	GSND_TERRAIN_RESERVE_INFO * pInfo = &m_ReserveInfo[m_nCurReserveIndex];

	pInfo->nInfoType    = nType;
	pInfo->Param_1		= nParam1;
	pInfo->Param_2		= nParam2;

	i3Vector::Copy( &pInfo->vPos, &vDiff);
	pInfo->bListener = bListener;

	m_nCurReserveIndex++;

	return m_nCurReserveIndex;
}

void CGameSoundManager::FlushTerrainSound(bool bForce)
{
	if( m_pSndCtx == nullptr)
		return ;

	MATRIX * pMat = m_pSndCtx->GetListnerMatrix();
	VEC3D vOriginal;

	if( bForce)
	{
		m_nCurReserveIndex = 0;
		return;
	}

	for( INT32 i = 0; i < m_nCurReserveIndex; ++i)
	{
		GSND_TERRAIN_RESERVE_INFO * pInfo = &m_ReserveInfo[i];

		i3Vector::Add( &vOriginal, &pInfo->vPos, i3Matrix::GetPos( pMat));

		if( (INFO_TYPE) pInfo->nInfoType == IT_TERRAIN_TYPE)
		{
			StartTerrainTypeSnd( pInfo->Param_1, pInfo->Param_2, &vOriginal, pInfo->bListener);
		}
		else if( (INFO_TYPE) pInfo->nInfoType == IT_CHARACTER_ACTION)
		{
			StartCharacterActionSnd( pInfo->Param_1, pInfo->Param_2, &vOriginal, pInfo->bListener);
		}
		else if( (INFO_TYPE) pInfo->nInfoType == IT_WEAPON_STATE)
		{
			StartWeaponStateSnd( pInfo->Param_1, pInfo->Param_2, &vOriginal, pInfo->bListener);
		}
		else if( (INFO_TYPE) pInfo->nInfoType == IT_ANNOUNCE)
		{
			StartAnnounceMessage( pInfo->Param_1);
		}
		else if( (INFO_TYPE) pInfo->nInfoType == IT_DINO_ACTION)
		{
			StartDinoActionSound( pInfo->Param_1, pInfo->Param_2, &vOriginal, pInfo->bListener);
		}
		else if( (INFO_TYPE) pInfo->nInfoType == IT_DINO_TERRAIN_TYPE)
		{
			StartDinoTerrainTypeSnd( pInfo->Param_1, pInfo->Param_2, &vOriginal, pInfo->bListener);
		}
		/*else if( (INFO_TYPE) pInfo->nInfoType == IT_RADIO)
		{
			StartRadioMessage( pInfo->Param_1, pInfo->Param_2);
		}*/
	}

	m_nCurReserveIndex = 0;
}

void	CGameSoundManager::Drive( REAL32 rDeltaTime, bool bForce)
{
	if( m_pSndCtx == nullptr)
		return;

	//Reserved Sound Play를 처리
	FlushTerrainSound( bForce);

	_ProcessRadioMessage( rDeltaTime, bForce);

	_UpdateBGM( rDeltaTime);

	_ProcessOutstandingSound( rDeltaTime);
}

void CGameSoundManager::setOutStandingEffect( bool bEnable, REAL32 time)
{
	if( bEnable != m_bEnableOutStandingSound)
	{
		m_rElapsedOutStandingTime	= 0.0f;
		m_rCurrentPitch				= 1.0f;
		m_rOutStandingTime			= time;
	}

	m_bEnableOutStandingSound = bEnable;
}

void CGameSoundManager::_ProcessOutstandingSound( REAL32 rDeltaSeconds)
{
	if( m_pSndCtx == nullptr)
		return;

	INT32 numChannels = m_pSndCtx->ChannelGroup_GetNumChannels( PBSND_CHANNEL_OUTSTANDING);
	if( m_bEnableOutStandingSound )
	{
		if( m_bInnerEnableOutStanding)
		{
			m_rElapsedOutStandingTime += rDeltaSeconds;
			if( numChannels <= 0 || m_rElapsedOutStandingTime >= m_rOutStandingTime)
			{
				m_pSndCtx->ChannelGroup_SetVolume( PBSND_CHANNEL_NORMAL, 100);
				//m_pSndCtx->ChannelGroup_SetPitch( PBSND_CHANNEL_NORMAL, 1.0f);

				m_bInnerEnableOutStanding = false;
				m_bEnableOutStandingSound = false;
			}
			else
			{
				REAL32 ratio = m_rElapsedOutStandingTime / m_rOutStandingTime;
				//REAL32 pitch = MINMAX( 0.1f, 1.0f - (i3Math::sin( I3_DEG2RAD( ratio * 180.0f))), 1.0f);
				REAL32 pitch = MINMAX( 0.3f, i3Math::cos( I3_DEG2RAD( ratio * 360.0f)), 1.0f);

				//if( pitch < 0.8f)
				{
					//m_pSndCtx->ChannelGroup_SetPitch( PBSND_CHANNEL_NORMAL, pitch);
					m_pSndCtx->ChannelGroup_SetVolume( PBSND_CHANNEL_NORMAL, (INT32)(pitch * 100.0f));
				}

				//I3TRACE("PITCH: %f, value: %f\n", pitch, i3Math::cos( I3_DEG2RAD( ratio * 360.0f)));
			}
		}
		else
		{
			m_pSndCtx->ChannelGroup_SetVolume( PBSND_CHANNEL_NORMAL, 20);
			//m_pSndCtx->ChannelGroup_SetPitch( PBSND_CHANNEL_NORMAL, 0.5f);

			m_bInnerEnableOutStanding = true;
		}
	}
	else
	{
		if( m_bInnerEnableOutStanding)
		{
			m_pSndCtx->ChannelGroup_SetVolume( PBSND_CHANNEL_NORMAL, 100);
			//m_pSndCtx->ChannelGroup_SetPitch( PBSND_CHANNEL_NORMAL, 1.0f);

			m_bInnerEnableOutStanding = false;
		}
	}
}

void CGameSoundManager::_RegisterNextBGM( const char * pszName, bool bFadeIn, bool bLoop, REAL32 fadetime, FMOD_CHANNEL_CALLBACK callback)
{
	if((i3Pack::isFile( pszName) != nullptr) || i3System::IsFile( pszName))
	{
		i3::string_ncopy_nullpad( m_pBackGroundMusic[0]._szName, pszName, MAX_PATH -1);

		m_pBackGroundMusic[0]._bSuspendStart	 = true;
		m_pBackGroundMusic[0]._bFadeEnable		 = bFadeIn;
		m_pBackGroundMusic[0]._bFadeOut			 = false;
		m_pBackGroundMusic[0]._rFadeElapsedTime	 = 0.0f;
		m_pBackGroundMusic[0]._rFadeTime		 = fadetime;
		m_pBackGroundMusic[0]._rUpdateElapse	 = 0.0f;
		m_pBackGroundMusic[0]._rUpdateInterval	 = 0.1f;
		m_pBackGroundMusic[0]._Loop				 = bLoop;
		m_pBackGroundMusic[0]._Callback			 = callback;
			
	}

	//StopBGM(fadetime);
}

void CGameSoundManager::_StartBGM()
{
	if((i3Pack::isFile( m_pBackGroundMusic[0]._szName) != nullptr) || i3System::IsFile( m_pBackGroundMusic[0]._szName))
	{
		BGM_INFO * pBGMInfo = &m_pBackGroundMusic[1];

		i3::string_ncopy_nullpad( pBGMInfo->_szName, m_pBackGroundMusic[0]._szName, MAX_PATH -1);

		pBGMInfo->_bSuspendStart		= false;
		pBGMInfo->_bFadeEnable			= m_pBackGroundMusic[0]._bFadeEnable;
		pBGMInfo->_bFadeOut				= m_pBackGroundMusic[0]._bFadeOut;
		pBGMInfo->_rFadeElapsedTime		= m_pBackGroundMusic[0]._rFadeElapsedTime;
		pBGMInfo->_rFadeTime			= m_pBackGroundMusic[0]._rFadeTime;
		pBGMInfo->_rUpdateElapse		= m_pBackGroundMusic[0]._rUpdateElapse;
		pBGMInfo->_rUpdateInterval		= m_pBackGroundMusic[0]._rUpdateInterval;
		pBGMInfo->_Loop					= m_pBackGroundMusic[0]._Loop;
		pBGMInfo->_Callback				= m_pBackGroundMusic[0]._Callback;

		i3SoundStream * pSnd	= i3SoundStream::new_object_ref();
		i3SoundPlayInfo * pInfo = i3SoundPlayInfo::new_object();

		pSnd->Create( pBGMInfo->_szName,  176400, 44100, 44100);

		m_pBackGroundMusic[1]._playInfo	= pInfo;

		pInfo->SetStyle( I3SND_PROP_TYPE_BGM);
		pInfo->set3DMode( I3SND_RT_3DMODE_DISABLE);
		if (	pBGMInfo->_Loop	== true	)
		{
			pInfo->AddStyle( I3SND_RT_LOOPMODE_LOOPING);
		}
		else
		{
			pInfo->AddStyle( I3SND_RT_LOOPMODE_ONESHOT);
		}
		
		pInfo->AddStyle( I3SND_RT_BUFFMODE_STREAM);
		pInfo->SetDefaultVolume( 100);
		pInfo->setListenerVolume( (pBGMInfo->_bFadeEnable && ( pBGMInfo->_rFadeTime > I3_EPS) ) ? 0 : 100 );
		pInfo->setSound( pSnd);

		pBGMInfo->_playState = m_pSndCtx->Play( pInfo, (VEC3D *)nullptr, 0, pBGMInfo->_Callback);
		if( pBGMInfo->_playState != nullptr)
		{
			I3_MUST_ADDREF(pBGMInfo->_playState);
		}
		else
		{
			_StopBGM( pBGMInfo);
		}

		//1번채널에 플레이 하고 0번은 다시 리셋
		i3mem::FillZero( &m_pBackGroundMusic[0], sizeof( BGM_INFO));
	}
}

void CGameSoundManager::_UpdateBGM( REAL32 rDeltaSeconds)
{
	if( m_pSndCtx == nullptr)
		return;	

	BGM_INFO *pInfo = &m_pBackGroundMusic[1];

	if( (pInfo->_bFadeEnable) && (pInfo->_playState != nullptr) && (pInfo->_playInfo != nullptr))
	{
		pInfo->_rFadeElapsedTime += rDeltaSeconds;
		pInfo->_rUpdateElapse += rDeltaSeconds;

		if( pInfo->_rUpdateInterval < pInfo->_rUpdateElapse)
		{
			pInfo->_rUpdateElapse -= pInfo->_rUpdateInterval;

			INT32	nVolume = 0;
			REAL32 rRatio = pInfo->_rFadeElapsedTime / pInfo->_rFadeTime;

			if( pInfo->_bFadeOut)
			{
				nVolume = (INT32)(100 * ( 1.0f - rRatio));
				if( nVolume <= 0)
				{
					I3TRACE("StopBGM Called\n");
					_StopBGM( pInfo);

					return;
				}
			}
			else
			{
				nVolume =(INT32)(100 * rRatio);
				if( nVolume > 100)
				{
					pInfo->_bFadeEnable = false;

					nVolume = 100;
				}
			}

			if( !pInfo->_playState->isFree())
			{
				pInfo->_playInfo->setListenerVolume( nVolume);
				pInfo->_playState->SetVolume( nVolume);
				//I3TRACE("VOLUME: %d\n", nVolume);
			}
		}
	}

	if( m_pBackGroundMusic[0]._bSuspendStart && pInfo->_playInfo == nullptr)
	{
		_StartBGM();
	}
}

void CGameSoundManager::_StopBGM(BGM_INFO * pInfo)
{
	if( m_pSndCtx == nullptr)
		return;

	if( pInfo == nullptr)
		return;

	if( pInfo->_playState != nullptr)
	{
		m_pSndCtx->Stop( pInfo->_playState, true);
	}

	I3_SAFE_RELEASE( pInfo->_playState );
	I3_SAFE_RELEASE( pInfo->_playInfo);

	i3mem::FillZero( pInfo, sizeof( BGM_INFO));
}

void CGameSoundManager::_StopLobbyAmbient(void)
{	
#if !defined( ENABLE_LOBBY_AMBIENT_SOUND)
	return;

#else

	if( m_pSndCtx == nullptr)
		return;

	for(INT32 i=0; i<GSND_LOBBY_AMBIENT_COUNT; i++)
	{
		SOUND_INFO * pSound = &m_LobbyAmbientSndInfo[i];

		if( pSound->_playState != nullptr)
		{
			m_pSndCtx->Stop( pSound->_playState, true);
		}

		I3_SAFE_RELEASE( pSound->_playState );
		I3_SAFE_RELEASE( pSound->_playInfo);
	}	

#endif
}

void CGameSoundManager::StartBGM( const char * pBGMPath, bool bFadeIn, bool bLoop, REAL32 fadetime, FMOD_CHANNEL_CALLBACK callback)
{
	if( pBGMPath != nullptr)
	{
		_RegisterNextBGM( pBGMPath, bFadeIn, bLoop, fadetime, callback);

		m_bPlayBGM = true;
	}
}
void CGameSoundManager::StopDinoBGM( REAL32 fadetime)
{

	m_bPlayBGM = false;

	if( m_pBackGroundMusic[1]._playState != nullptr &&
		i3::generic_is_iequal(m_pBackGroundMusic[1]._szName ,"Sound/BackGround/BreedingNest.ogg"))
	{
		_StopBGM( &m_pBackGroundMusic[1]);	
	}
	else if( m_pBackGroundMusic[1]._playState != nullptr &&
		i3::generic_is_iequal(m_pBackGroundMusic[1]._szName ,"Sound/BackGround/BloodyHoliday_BackGround.ogg"))
	{
		_StopBGM( &m_pBackGroundMusic[1]);	
	}


}
void CGameSoundManager::StopBGM( REAL32 fadetime)
{
	m_bPlayBGM = false;

	if( m_pBackGroundMusic[1]._playState != nullptr)
	{
		if( fadetime > I3_EPS)
		{
			m_pBackGroundMusic[1]._bFadeEnable		 = true;
			m_pBackGroundMusic[1]._bFadeOut			 = true;
			m_pBackGroundMusic[1]._rFadeElapsedTime	 = 0.0f;
			m_pBackGroundMusic[1]._rFadeTime		 = fadetime;
		}
		else
		{
			_StopBGM( &m_pBackGroundMusic[1]);
		}
	}

	//	BGM 환경음도 같이
	_StopLobbyAmbient();	
}

i3SoundPlayState * CGameSoundManager::tellPlaySound( i3SoundPlayInfo * pInfo)
{
	if( m_pSndCtx == nullptr)
		return nullptr;

	if( pInfo == nullptr)
		return nullptr;

	INT32 nCnt = 0;

	i3SoundPlayInfo * pInstance = _GetFreeSndInfo( &nCnt);
	if( pInstance == nullptr)
	{
		I3TRACE("푸뤼한 사운드가 없습니다. 버퍼갯수를 늘려야한다고 말해주세요\n");
		return nullptr;
	}

	pInfo->CopyTo( pInstance, I3_COPY_INSTANCE);


	return m_pSndCtx->Play( pInstance);

}

void CGameSoundManager::EnterStage( i3Stage * pStage)
{
	if( g_pFramework->GetStageBattle() != nullptr)
	{
		ROOM_INFO_BASIC	roomInfo;
		MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

		// 스테이지에서 사용하는 Object Sound를 로드합니다.
		InitObjectSnd( roomInfo._StageID );

		STAGE_MODE stageType = StageID::GetStageMode( roomInfo._StageID);
		if( stageType == STAGE_MODE_ESCAPE || 
			stageType == STAGE_MODE_CROSSCOUNT )
		{
			// 공룡모드라면 공룡이 사용하는 사운드를 로드합니다.
			InitDinoActionSound();
			InitDinoTerrainTypeSound();
		}
		else if( stageType == STAGE_MODE_BOMB || stageType == STAGE_MODE_TUTORIAL)
		{
			// 폭파 미션 C4 사운드 로드
			InitWeapon_C4StateSnd();
		}

		/*InitTerrainTypeSnd();*/
		InitCharacterActionSnd();
		InitWeaponStateSnd();
		InitAnnounceSnd();
		InitWeaponKillSound();
		InitRadioMessage();
	}
	else
	{	// Lobby UI입니다.
		InitRouletteShopSound();
		InitLotterySound();
	}
}

void CGameSoundManager::LeaveStage( void)
{
	ReleaseObjectSnd();
	ReleaseDinoActionSound();
	ReleaseDinoTerrainTypeSound();
	ReleaseWeapon_C4StateSnd();

	ReleaseRouletteShopSound();
	//ReleaseTerrainTypeSnd();
	ReleaseCharacterActionSnd();
	ReleaseWeaponStateSnd();
	ReleaseAnnounceSnd();
	ReleaseWeaponKillSound();
	ReleaseRadioMessage();
}

#ifdef DOMI_NEW_FLOW
void CGameSoundManager::LeaveStageRoundJump( void)
{
	ReleaseObjectSnd();
}
#endif
#ifndef _CEFFECT_MANAGER_H__
#define _CEFFECT_MANAGER_H__

//#include "Weapon.h"
#include "GameCharaEquipDef.h"


#define CHARA_EFFECT_TYPE_COUNT			9 //(WEAPON_EFFECT_TYPE_COUNT + 1)   
#define RANDOM_SOUND					3

#define HITBOXSHAPE_COUNT				128

#define EFFECT_BULLET_TYPE				2		
//	È¯°æ ÀÌÆåÆ®
enum ENV_EFFECT_TYPE
{
	ENV_EFFECT_CHARA_DOWN_COMMON = 0,
	ENV_EFFECT_CHARA_WALK_COMMON,
	ENV_EFFECT_WEAPON_DOWN_COMMON,
	ENV_EFFECT_HELMET_DOWN_COMMON,
	ENV_EFFECT_BOMB_LIGHT,
	ENV_EFFECT_UH60_DUST_COMMON,		// Çï±â ¸ÕÁöÈ¿°ú
	ENV_EFFECT_RANGE_GOLEM_FIRE,		// ½ÃÁî°ñ·½ ¹ß»ç ÀÔÈÒÆ®
	ENV_EFFECT_SIEGE_GOLEM_FIRE,		// µ¹°ñ·½ ¹Ù´Ú ÆÐ´ë±â È¿°ú
	ENV_EFFECT_RANGE_GOLEM_IMPACT,		// °ñ·½ ÅºÂø È¿°ú
	ENV_EFFECT_RPG7_SMK_COMMON,			// RPG7 ¿¬±â È¿°ú
	ENV_EFEECT_TREX_WALK,				// TREX¹ßÀÚ±¹

	ENV_EFEECT_DUMMY,					// ÀÓ½Ã ´õ¹Ì
	ENV_EFEECT_SUPPLY,					// ÀÓ½Ã ¼­ÇÃ¶óÀÌ
	ENV_EFEECT_SUPPLY_IDLE,					// ÀÓ½Ã ¼­ÇÃ¶óÀÌ

	ENV_EFFECT_TYPE_COUNT
};

enum WEAPON_EFFECT_TYPE
{
	WEAPON_EFFECT_TYPE_UNKNOWN = -1,
	WEAPON_EFFECT_TYPE_KNIFE,
	WEAPON_EFFECT_TYPE_HANDGUN,
	WEAPON_EFFECT_TYPE_ASSAULT,
	WEAPON_EFFECT_TYPE_SMG,
	WEAPON_EFFECT_TYPE_SNIFER,
	WEAPON_EFFECT_TYPE_SHOTGUN,
	WEAPON_EFFECT_TYPE_RAPTOR,
	WEAPON_EFFECT_TYPE_RAPTOR_LEFT_SCRATCH,
	WEAPON_EFFECT_TYPE_TREX,
	WEAPON_EFFECT_TYPE_STING,
	WEAPON_EFFECT_TYPE_ELITE,
	WEAPON_EFFECT_TYPE_ELITE_LEFT_SCRATCH,
	WEAPON_EFFECT_TYPE_WATER,
	WEAPON_EFFECT_TYPE_COUNT
};

enum BULLET_EFFECT_TYPE
{
	BULLET_EFFECT_DEFAULT = 0,
	BULLET_EFFECT_WATER,
};


enum BLOOD_TERRAIN_TYPE
{
	BLOOD_TERRAIN_WALL = 0,
	BLOOD_TERRAIN_FLOOR,

	BLOOD_TERRAIN_TYPE_COUNT
};

enum BLOOD_EFFECT_TYPE
{
	BLOOD_EFFECT_ASSAULT = 0,
	BLOOD_EFFECT_SMG,
	BLOOD_EFFECT_HANDGUN,
	BLOOD_EFFECT_SHOTGUN,
	BLOOD_EFFECT_KNIFE,
	BLOOD_EFFECT_GRENADE,
	BLOOD_EFFECT_RIFLE,
	BLOOD_EFFECT_WATER,

	BLOOD_EFFECT_TYPE_COUNT,
};

//////////////////////////////////////////////////////////////////////////////////////////////
class CEffectManager :	public i3EffectManager
{
	I3_CONCRETE_CLASS_DEFINE( CEffectManager, i3EffectManager );

protected:
	//	Ä³¸¯ÅÍ HIt
	INT32				m_CharaHitIndex[2][ CHARA_EFFECT_TYPE_COUNT];		// [initialize at constructor]

	//	Ä³¸¯ÅÍ Water HIt 
	INT32				m_CharaWaterHitIndex[2][ CHARA_EFFECT_TYPE_COUNT];	// [initialize at constructor]

	// °ø·æ Hit
	INT32				m_DinoHitIndex[2][ CHARA_EFFECT_TYPE_COUNT];		// [initialize at constructor]

	//	ÁöÇü Hit
	INT32				m_TerrainEffIndex[ I3_TERRAIN_TYPE_COUNT][ WEAPON_EFFECT_TYPE_COUNT];	// [initialize at constructor]

	//	ÅºÈç
	INT32				m_BulletHoleEffIndex[ I3_TERRAIN_TYPE_COUNT][ WEAPON_EFFECT_TYPE_COUNT];// [initialize at constructor]

	//	È¯°æ
	INT32				m_EnvironmentEffIndex[ ENV_EFFECT_TYPE_COUNT];		// [initialize at constructor]

	//  Ç÷Èç
	INT32				m_BloodEffIndex[ BLOOD_TERRAIN_TYPE_COUNT ][ BLOOD_EFFECT_TYPE_COUNT];	// [initialize at constructor]

	i3::vector<i3Node*>	m_TransformList;

	INT32			m_pEnvEffect[4] = { 0, 0, 0, 0 };
	
	i3::vector<i3TimeStamp*> m_ObjectEffectList;
	i3::vector<i3Object*>	m_DeleteObjectList;
	
	bool			m_bInitedBloodEffect = false;

private :
	void			__SetTerrainTypeToDecal( i3TimeStamp * pEffect, I3_TERRAIN_TYPE type);

public:
	CEffectManager(void);

	virtual void	OnCreate(void) override;

	i3TimeStamp *	AddTerrainEffect( I3_TERRAIN_TYPE type, WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal = 0.f, i3Object * pObj = nullptr, bool UnBlowFlag = false, bool UnDecalFlag = false);
	void			AddBloodEffect( BLOOD_TERRAIN_TYPE TerrainType, BLOOD_EFFECT_TYPE EffectType, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal = 0.f, i3Object * pObj = nullptr );
	void			AddBlowEffect( I3_TERRAIN_TYPE type, WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal);
	void			AddEquipHitEffect( WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, bool bListener);
	void			AddCharaHitEffect( WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, REAL32 dist);
	void			AddDinoBuffEffect(VEC3D * pPos, VEC3D * pNormal);
	void			AddDinoHitEffect( WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, REAL32 dist);
	i3TimeStamp *	AddEnvironmentEffect( ENV_EFFECT_TYPE effType, VEC3D * pPos, VEC3D * pNormal, MATRIX * pObjMat = nullptr);
	i3TimeStamp *	AddDecal( I3_TERRAIN_TYPE type, WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal = 0.f, i3Object * pObj = nullptr);
	i3TimeStamp *	AddDecal( INT32 EffIdx, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal = 0.f, i3Object * pObj = nullptr);


	INT32			GetEnvEffectTypeIndex(INT32 nIndex){	return m_pEnvEffect[nIndex];}
	INT32			GetEnvironmentEffectIdx( INT32 nidx)	{	return m_EnvironmentEffIndex[nidx];}


	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual	void	Reset() override;

	void			InitAgeLimitEffect( void);

	i3::vector<i3TimeStamp*>&		getObjectEffectList( void)				{ return m_ObjectEffectList; }
	void			NeetToInitAgeLimitEffect()				{ m_bInitedBloodEffect = false; }
};

#endif //ifndef _CEFFECT_MANAGER_H__
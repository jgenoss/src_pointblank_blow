#if !defined( _I3_GAMERES_DEFINE_H__)
#define _I3_GAMERES_DEFINE_H__

enum	I3_GAMERES_TYPE
{
	I3_GAMERES_ANIMATION = 0,
	I3_GAMERES_ANIMATIONPACK,
	I3_GAMERES_TEXTURE,
	I3_GAMERES_BINARY,
	I3_GAMERES_SOUND,
	I3_GAMERES_WORLD,
	I3_GAMERES_CHARA,
	I3_GAMERES_SCENE,
	I3_GAMERES_OBJECT,
	I3_GAMERES_LIGHT,
	I3_GAMERES_RESPAWN,
	I3_GAMERES_PATH,
	I3_GAMERES_ACTION_EVENT,

	I3_GAMERES_TYPE_COUNT,
};

enum I3GAME_DYNAMIC_STATE
{
	I3GAME_DYNAMIC_STATE_NONE = 0,
	I3GAME_DYNAMIC_STATE_DYNAMIC,
	I3GAME_DYNAMIC_STATE_KINEMATIC,
	I3GAME_DYNAMIC_STATE_STATIC,
	I3GAME_DYNAMIC_STATE_TRIGGER,
};



#if defined ( I3_COMPILER_VC)
#pragma pack (push, 1)
#endif

typedef struct PACKED _tagi3PersistRSCHeader
{
	UINT8	m_ID[4];
	INT32	m_nType;
	INT32	m_nNameBufSize;
	INT32	m_nFilePathSize;
	OBJREF	m_pParent;
}I3_PERSIST_RSC_HEADER;

typedef struct PACKED _tagi3PersistRSCHeader_2
{
	UINT8	m_ID[4];
	INT32	m_nType;
	INT32	m_nNameBufSize;
	INT32	m_nFilePathSize;
	UINT32	m_nDataOffSet;
}I3_PERSIST_RSC_HEADER_2;

typedef struct PACKED _tagi3PersistRSCScene
{
	INT32	m_nLayer;
	char	m_szClass[64];
	char	m_szCommon[64];
}I3_PERSIST_RSC_SCENE;

typedef struct PACKED _tagi3PersistRSCScene_2
{
	INT32	m_nLayer;
	char	m_szClass[64];
	INT32	m_bCommon;
}I3_PERSIST_RSC_SCENE_2;

typedef struct PACKED _tagi3PersistRSCSound
{
	//INT32 m_nSoundStyle;
	REAL32  m_rMaxDist;
	REAL32	m_rMinDist;

	INT32	m_nDefaultVolume;
	INT32	m_nListenerVolume;
}I3_PERSIST_RSC_SOUND;

typedef struct PACKED _tagi3PersistRSCSound2
{
	REAL32  m_rMaxDist;
	REAL32	m_rMinDist;
}I3_PERSIST_RSC_SOUND1;

typedef struct PACKED _tagi3PersistGameInstance
{
	UINT32	nHashCodeResPath;
	UINT32	nHashCodeInstClass;

	PERSIST_VEC3D		vPos;
	PERSIST_VEC4D		qRot;
	PERSIST_VEC3D		vScale;

	UINT32	bInitEnable;
	UINT32	nIDPortal;

	UINT32	nDynamicState;
	UINT32	nParam1;
	UINT32	nParam2;
	UINT32	nParam3;
	UINT32	nNameHash;
	UINT8	nNetSyncMode;
	UINT8	nTriggerEnable;
	UINT8	nTriggerMatchGroup;
	UINT8	nTriggerType;

	UINT32	HashCode[3];

	//PERSIST_VEC3D		vJumpPos;		// Jumper Ãß°¡ by dfly 20080226

	UINT32	nGameDataSize;
}I3_PERSIST_GAME_INSTANCE;


typedef struct PACKED _tagi3PersistGameInstancePack
{
	UINT8	m_ID[4];
	UINT32	m_Version;

	UINT32	m_nInstanceCount;
	UINT32	pad[5];

	inline 
	void	Init(void)
	{
		i3mem::FillZero( this, sizeof(_tagi3PersistGameInstancePack));
		i3mem::Copy( m_ID, "GINS", 4);

		m_Version			= 2;
	}
}I3_PERSIST_GAME_INSTANCE_PACK;

#if defined ( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
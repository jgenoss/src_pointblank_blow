#if !defined( _I3_GAMERES_DEFINE_H__)
#define _I3_GAMERES_DEFINE_H__

enum	I3_GAMERES_TYPE
{
	I3_GAMERES_NONE = -1,
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

namespace i3
{
	namespace pack
	{
		struct PACKED RSC_HEADER
		{
			UINT8	m_ID[4] = { 'R', 'S', 'C', '1' };
			INT32	m_nType = 0;
			INT32	m_nNameBufSize = 0;
			INT32	m_nFilePathSize = 0;
			OBJREF	m_pParent = 0;
		};

		struct PACKED RSC_HEADER_2
		{
			UINT8	m_ID[4] = { 'R', 'S', 'C', '2' };
			INT32	m_nType = 0;
			INT32	m_nNameBufSize = 0;
			INT32	m_nFilePathSize = 0;
			UINT32	m_nDataOffSet = 0;
		};

		struct PACKED RSC_HEADER_3
		{
			UINT8	m_ID[4] = { 'R', 'S', 'C', '3' };
			INT32	m_nType = 0;
			INT32	m_nNameBufSize = 0;
			INT32	m_nFilePathSize = 0;
			OBJREF	m_pParent = 0;
			UINT32	m_Style = 0;
		};

		struct PACKED RSC_SCENE
		{
			INT32	m_nLayer = 0;
			char	m_szClass[64] = { 0 };
			char	m_szCommon[64] = { 0 };
		};

		struct PACKED RSC_SCENE_2
		{
			INT32	m_nLayer = 0;
			char	m_szClass[64] = { 0 };
			INT32	m_bCommon = 0;
		};

		struct PACKED RSC_SOUND
		{
			REAL32  m_rMaxDist = 0.0f;
			REAL32	m_rMinDist = 0.0f;

			INT32	m_nDefaultVolume = 0;
			INT32	m_nListenerVolume = 0;
		};

		struct PACKED RSC_SOUND1
		{
			REAL32  m_rMaxDist = 0.0f;
			REAL32	m_rMinDist = 0.0f;;
		};

		struct PACKED GAME_INSTANCE
		{
			UINT32	nHashCodeResPath = 0;
			UINT32	nHashCodeInstClass = 0;

			i3::pack::VEC3D		vPos;
			i3::pack::VEC4D		qRot;
			i3::pack::VEC3D		vScale;

			UINT32	bInitEnable = 0;
			UINT32	nIDPortal = 0;

			UINT32	nDynamicState = 0;
			UINT32	nParam1 = 0;
			UINT32	nParam2 = 0;
			UINT32	nParam3 = 0;
			UINT32	nNameHash = 0;
			UINT8	nNetSyncMode = 0;
			UINT8	nTriggerEnable = 0;
			UINT8	nTriggerMatchGroup = 0;
			UINT8	nTriggerType = 0;

			UINT32	HashCode[3] = { 0 };

			UINT32	nGameDataSize = 0;
			UINT32	nNameSize = 0;
		};

		// xgkick
		// 이름과 소속 Layer 정보를 추가하기 위한 수정.
		struct PACKED GAME_INSTANCE_2
		{
			UINT32	nHashCodeResPath = 0;
			UINT32	nHashCodeInstClass = 0;

			i3::pack::VEC3D		vPos;
			i3::pack::VEC4D		qRot;
			i3::pack::VEC3D		vScale;

			UINT32	bInitEnable = 0;
			UINT32	nIDPortal = 0;

			UINT32	nDynamicState = 0;
			UINT32	nParam1 = 0;
			UINT32	nParam2 = 0;
			UINT32	nParam3 = 0;
			UINT32	nNameHash = 0;
			UINT8	nNetSyncMode = 0;
			UINT8	nTriggerEnable = 0;
			UINT8	nTriggerMatchGroup = 0;
			UINT8	nTriggerType = 0;

			UINT32	HashCode[3] = { 0 };

			char	m_szName[64] = { 0 };
			INT32	m_Layer = 0;

			UINT32	nGameDataSize = 0;
			UINT32	nNameSize = 0;				// <- 호환성을 위해 남겨둠.
			UINT32	pad[8] = { 0 };

			void	CopyFrom(GAME_INSTANCE * pOld)
			{
				nHashCodeResPath = pOld->nHashCodeResPath;
				nHashCodeInstClass = pOld->nHashCodeInstClass;

				vPos.x = pOld->vPos.x;		vPos.y = pOld->vPos.y;		vPos.z = pOld->vPos.z;
				qRot.x = pOld->qRot.x;		qRot.y = pOld->qRot.y;		qRot.z = pOld->qRot.z;		qRot.w = pOld->qRot.w;
				vScale.x = pOld->vScale.x;	vScale.y = pOld->vScale.y;	vScale.z = pOld->vScale.z;

				bInitEnable = pOld->bInitEnable;
				nIDPortal = pOld->nIDPortal;

				nDynamicState = pOld->nDynamicState;
				nParam1 = pOld->nParam1;
				nParam2 = pOld->nParam2;
				nParam3 = pOld->nParam3;
				nNameHash = pOld->nNameHash;
				nNetSyncMode = pOld->nNetSyncMode;
				nTriggerEnable = pOld->nTriggerEnable;
				nTriggerMatchGroup = pOld->nTriggerMatchGroup;
				nTriggerType = pOld->nTriggerType;

				HashCode[0] = pOld->HashCode[0];
				HashCode[1] = pOld->HashCode[1];
				HashCode[2] = pOld->HashCode[2];

				nGameDataSize = pOld->nGameDataSize;
				nNameSize = pOld->nNameSize;

				m_szName[0] = 0;
				m_Layer = 0;
			}
		};

		struct PACKED GAME_INSTANCE_PACK
		{
			UINT8	m_ID[4] = { 'G', 'I', 'N', 'S' };
			UINT32	m_Version = 4;

			UINT32	m_nInstanceCount = 0;
			UINT32	m_LayerCount = 0;
			UINT32	pad[4] = { 0 };
/*
			inline
				void	Init(void)
			{
				i3mem::FillZero(this, sizeof(i3::pack::GAME_INSTANCE_PACK));
				i3mem::Copy(m_ID, "GINS", 4);

				m_Version = 4;
			}*/
		};

		// *.i3INS 파일 안에 저장되는 구조체
		// Layer에 대한 정보를 가지고 있다.
		struct PACKED GAME_LAYER
		{
			char	m_szName[64] = { 0 };
			UINT32	pad[4] = { 0 };
		};
	}
}

#if defined ( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
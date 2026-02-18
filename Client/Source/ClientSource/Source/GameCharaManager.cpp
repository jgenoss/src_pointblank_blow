#include "pch.h"
#include "GameCharaManager.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GameCharaDino.h"
#include "GameCharaDinoRaptor.h"
#include "DinoInfo.h"
#include "BattleHud.h"
#include "UtilBattle.h"
#include "UtilBattle.h"
#include "ObserverBattle.h"
#include "GameStateMgr.h"
#include "BGLoadThread.h"
#include "GameStateMgr.h"

#include "Avatar/AvatarManager.h"
#include "../../Avatar/AvatarGen.h"
#include "../Character/InputCtrlAI_ForHuman.h"

#include "Weapon/WeaponUtil.h"
#include "GameMissionManager.h"
#include "GameCharaMoveContext.h"

#include "Character/Fullbody/GameCharaFullbody.h"
#include "Character/Fullbody/GameCharaDomiRaptor.h"
#include "Character/Fullbody/GameCharaDomiAcidItem.h"
#include "Character/Fullbody/GameCharaDomiAcid.h"
#include "Character/Fullbody/GameCharaDomiAcidNotBomb.h"
#include "Character/Fullbody/GameCharaDomiSting.h"
#include "Character/Fullbody/GameCharaDomiMutantRex.h"
#include "Character/Fullbody/GameCharaDomiTank.h"
#include "Character/Fullbody/GameCharaDomiRaptorMercury.h"
#include "Character/Fullbody/GameCharaDomiStingMars.h"
#include "Character/Fullbody/GameCharaDomiAcidVulcan.h"

#include "UI/HUD/DomiRevivalController.h"
#include "UI/HUD/UIHUD_Domination_Manager.h"
#include "UI/UIHudMultiWeapon.h"
#include "UI/UIBattleFrame.h"
#include "Ui/UIHudManager.h"
#include "UI/UIMainFrame.h"
#include "UI/UICharaView.h"
#include "UI/UIUtil.h"
#include "UI/UIFloatCharacterView.h"

#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/algorithm/to_upper.h"

#include "./StageObject/ObjectRespawnHelicopter.h"
#include "./StageBattle/ScoreContext.h"
#include "./StageBattle/UserContext.h"

#include "BattleServerContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "HackContext.h"
#include "GameContextUtil.h"
#include "Cheatkey.h"
#include "Weapon/WeaponItemID.h"
#include "CommunityContext.h"
#include "i3Scene/i3XrayScene.h"
#include "i3Scene/i3XrayNode.h"


#include "i3Base/string/ext/utf16_to_mb.h"

#include "ClientStageInfoUtil.h"


#if defined( BUILD_RELEASE_QA_VERSION) && 0
	#define RESERVE_TRACE		I3TRACE
#else
	#define RESERVE_TRACE		__noop
#endif


static REAL32 s_rStartPos[ ][ 3 ] = {
	{0, -1010, 0}, {0, -1020, 0}, {0, -1030, 0}, {0, -1040, 0},
	{0, -1050, 0}, {0, -1060, 0}, {0, -1070, 0}, {0, -1080, 0},
	{0, -1090, 0}, {0, -1100, 0}, {0, -1110, 0}, {0, -1120, 0},
	{0, -1130, 0}, {0, -1140, 0}, {0, -1150, 0}, {0, -1160, 0}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
EquipResSceneGraph::EquipResSceneGraph(EQUIP::ePART part)
{
	_nCount = TEAM_COUNT;

	if( part == EQUIP::ePART_BERET )
		_nCount = TEAM_COUNT * EQUIP::eBERET_COUNT;

	size_t alloc_size = sizeof(i3GameResSceneGraph*) * _nCount;

	_pRes = (i3GameResSceneGraph **)i3MemAlloc(alloc_size);
	i3mem::FillZero( _pRes, alloc_size);
}

EquipResSceneGraph::~EquipResSceneGraph(void)
{
	I3MEM_SAFE_FREE( _pRes);
}

void EquipResSceneGraph::SetEquipRes(INT32 team, INT32 res_subtype, i3GameResSceneGraph * pRes)
{
	UINT32 offset = _nCount / TEAM_COUNT;
	UINT32 idx = static_cast<UINT32>( (team * offset) + res_subtype);
	I3ASSERT(idx < _nCount);

	I3_REF_CHANGE( _pRes[idx], pRes);
}

const i3GameResSceneGraph * EquipResSceneGraph::GetEquipRes(INT32 team, INT32 res_subtype)
{
	UINT32 offset = _nCount / TEAM_COUNT;
	UINT32 idx = static_cast<UINT32>( (team * offset) + res_subtype);
	I3ASSERT(idx < _nCount);

	return _pRes[idx];
}

void EquipResSceneGraph::ReleaseGameRes(bool is_remove_all /* = false */)
{
	for( UINT32 idx = 0 ; idx < _nCount ; ++idx)
	{
		if( _pRes[idx] != nullptr && ( !_pRes[idx]->isCommonRes() || is_remove_all) )
		{
			I3_SAFE_RELEASE(_pRes[idx]);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE( CGameCharaManager);//, i3ElementBase );
I3_CLASS_INSTANCE( CGameRespawn);//, i3ElementBase );
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CGameRespawn::CGameRespawn()
{

}

CGameRespawn::~CGameRespawn()
{
	i3::cu::for_each_safe_release_clear(m_RedTeamList, &i3RespawnObj::Release); 		//한쌍
	i3::cu::for_each_safe_release_clear(m_BlueTeamList, &i3RespawnObj::Release); 	//한쌍
	i3::cu::for_each_safe_release_clear(m_RedTeamNPCList, &i3RespawnObj::Release); 	//한쌍
	i3::cu::for_each_safe_release_clear(m_BlueTeamNPCList, &i3RespawnObj::Release); 	//한쌍
}

void CGameRespawn::Create( const i3::vector<i3RespawnObj*>& RespawnObjList)
{
	size_t nCount = RespawnObjList.size();

	//레벨툴에 있는 값을 게임값으로 세팅합니다.
	for(size_t i = 0; i < nCount; ++i)
	{
		i3RespawnObj * pObj = RespawnObjList[i];
		switch(pObj->getRespawnStyle())
		{
		case 0	: m_RedTeamList.push_back( pObj);		break; //CHARACTER_TEAM_RED
		case 1	: m_BlueTeamList.push_back( pObj);	break; //CHARACTER_TEAM_BLUE
		case 2	: m_RedTeamNPCList.push_back( pObj);	break; //NPC_TEAM_RED
		case 3	: m_BlueTeamNPCList.push_back( pObj);	break; //NPC_TEAM_BLUE
		default: I3ASSERT_0; break;
		}
		I3_MUST_ADDREF(pObj);		//한쌍
	}


}

UINT32	CGameRespawn::GetRespawnCount( CHARACTER_TEAM_TYPE nTeam)
{
	i3::vector<i3RespawnObj*>* pValidList = nullptr;

	switch(nTeam)
	{
	case CHARACTER_TEAM_RED	: pValidList = &m_RedTeamList;		break;
	case CHARACTER_TEAM_BLUE: pValidList = &m_BlueTeamList;		break;
	case NPC_TEAM_RED		: pValidList = &m_RedTeamNPCList;	break;
	case NPC_TEAM_BLUE		: pValidList = &m_BlueTeamNPCList;	break;
	default: I3ASSERT_0; break;
	}

	if (pValidList)
		return pValidList->size();
	
	return 0;
}

i3RespawnObj	*	CGameRespawn::GetRespawnObject( INT32 nTeam, INT32 nidx)
{
	i3::vector<i3RespawnObj*>* pValidList = nullptr;

	switch(nTeam)
	{
	case CHARACTER_TEAM_RED	: pValidList = &m_RedTeamList;break;
	case CHARACTER_TEAM_BLUE: pValidList = &m_BlueTeamList;break;
	case NPC_TEAM_RED		: pValidList = &m_RedTeamNPCList;break;
	case NPC_TEAM_BLUE		: pValidList = &m_BlueTeamNPCList;break;
	default: I3ASSERT_0; break;
	}

	nidx = nidx % (INT32)pValidList->size();	//참 거시한 코드 입니다. 성백 전체 구조변경해야함...
	i3RespawnObj * pObj = (*pValidList)[nidx];
	return pObj;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TCPDEATHTIME	0.1f

//#define CHARA_RED_TYPE_B
#define ENABLED_RESTORE_SNDPLAYINFO

static bool _SetSoundNodeProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( !i3::same_of<i3AIContext*>(pNode))
		return true;

	i3AIContext * pCtx = (i3AIContext *) pNode;
	i3AI * pAI = pCtx->getAI();
	INT32 i, j;

	for( i = 0; i < pAI->getAIStateCount(); i++)
	{
		i3AIState * pState = pAI->getAIState( i);
		i3TimeSequence * pTimeSeq = pState->getTimeSeq();

		if( pTimeSeq == nullptr)
			continue;

		for( j = 0; j < pTimeSeq->GetSequenceCount(); j++)
		{
			i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( j);

			if( i3::same_of<i3SoundNode*>(pInfo->GetNode()))
			{
				i3SoundNode * pSnd = (i3SoundNode *) pInfo->GetNode();
				if( pSnd == nullptr)
				{
					continue;
				}

				i3SoundPlayInfo * pPlayInfo = pSnd->getSoundPlayInfo();
				if( pPlayInfo == nullptr)
				{
					continue;
				}

				char szTemp[MAX_PATH];

				i3Sound * pSound = pPlayInfo->getSound();
				if( pSound != nullptr && (pSound->hasName()))
				{
					i3::extract_filename(pSound->GetName(), szTemp);

					i3GameResSound * pRes = (i3GameResSound *) g_pFramework->QuaryResource( szTemp);
					if( pRes)
					{
						i3SoundPlayInfo * pSndResInfo = pRes->getSound();
						if( pSndResInfo != nullptr)
						{
							pPlayInfo->RemoveStyle( I3SND_PROP_TYPE_BGM | I3SND_PROP_TYPE_VOICE);
							pPlayInfo->AddStyle( I3SND_PROP_TYPE_EFFECT);

							pPlayInfo->SetDistanceInfo( pSndResInfo->getMaxDistance(),
														pSndResInfo->getMinDistance(),
														pSndResInfo->getVolumeType());

							pPlayInfo->SetDefaultVolume( pSndResInfo->GetDefaultVolume());
							pPlayInfo->setListenerVolume( pSndResInfo->getListenerVolume());
						}
					}
					else
					{
						I3TRACE("no res == skip: %s\n", szTemp);
					}
				}
				else
				{
					I3TRACE("no res name == skip: %s\n", szTemp);
				}
			}
		}
	}

	return true;
}


namespace AICharacter_Util
{
	T_ItemID ReverveTypeToItemID( UINT8 nType)
	{
		switch( nType)
		{
		case RESERVE_DOMI_RAPTOR:			return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR);
		case RESERVE_DOMI_RAPTOR_BLUE:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE);
		case RESERVE_DOMI_RAPTOR_GREEN:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN);
		case RESERVE_DOMI_STING:			return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_STING);
		case RESERVE_DOMI_ACID:				return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_ACID);
		case RESERVE_DOMI_MUTANT:			return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_MUTANTREX);
		case RESERVE_DOMI_ACID_NOT_BOMB:	return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB);
		case RESERVE_DOMI_RAPTOR_MERCURY:	return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY);
		case RESERVE_DOMI_STING_MARS:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_STING_MARS);
		case RESERVE_DOMI_ACID_VULCAN:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_ACID_VULCAN);
		default:							return 0xFFFFFFFF;
		}
	}

	INT8 ItemIDToReserveType( T_ItemID nItemID)
	{
		CHARA_RES_ID nResID = CHARA::ItemID2ResID( nItemID);

		switch(nResID)
		{
		case CHARA_RES_ID_DINO_DOMI_RAPTOR:			return RESERVE_DOMI_RAPTOR;
		case CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE:	return RESERVE_DOMI_RAPTOR_BLUE;
		case CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN:	return RESERVE_DOMI_RAPTOR_GREEN;
		case CHARA_RES_ID_DINO_DOMI_STING:			return RESERVE_DOMI_STING;
		case CHARA_RES_ID_DINO_DOMI_ACID:			return RESERVE_DOMI_ACID;
		case CHARA_RES_ID_DINO_DOMI_MUTANTREX:		return RESERVE_DOMI_MUTANT;
		case CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB:	return RESERVE_DOMI_ACID_NOT_BOMB;
		case CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY: return RESERVE_DOMI_RAPTOR_MERCURY;
		case CHARA_RES_ID_DINO_DOMI_STING_MARS:		return RESERVE_DOMI_STING_MARS;
		case CHARA_RES_ID_DINO_DOMI_ACID_VULCAN:	return RESERVE_DOMI_ACID_VULCAN;
		default:									return RESERVE_NONE;
		}
	}

	UINT16 GetReserveCount( UINT8 nType)
	{
		switch( nType)
		{
		case RESERVE_DOMI_RAPTOR:			return 2;
		case RESERVE_DOMI_RAPTOR_BLUE:		return 2;
		case RESERVE_DOMI_RAPTOR_GREEN:		return 2;
		case RESERVE_DOMI_STING:			return 2;
		case RESERVE_DOMI_ACID:				return 2;
		case RESERVE_DOMI_MUTANT:			return 0;
		case RESERVE_DOMI_ACID_NOT_BOMB:	return 2;
		default:							return 0;
		}
	}

	const UINT8	AVERAGE_RESERVE_COUNT = 15;		//평균 저장 되는 카운드
	const UINT32 LIMITED_CHARACTER_COUNT = 36;	//MAX_COUNT_DOMI_DINO + Player(4)
	const char g_szReserveTypeName[RESERVE_MAX][MAX_STRING_COUNT] = { "DomiRaptor", "DomiRaptor_Blue", "DomiRaptor_Green", "DomiSting", "DomiAcid", "DomiMutant", "DomiAcidNoUseBomb"};

	//.......................................................................
	//제압 미션 enum DINO_KIND -> enum CHARA_RES_ID
	CHARA_RES_ID DinoKind2CharaResId( DINO_KIND e)
	{
		switch( e)
		{
		case DINO_KIND_RAPTOR_RED:		return CHARA_RES_ID_DINO_DOMI_RAPTOR;
		case DINO_KIND_RAPTOR_GREEN:	return CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN;
		case DINO_KIND_RAPTOR_BLUE:		return CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE;
		case DINO_KIND_STING:			return CHARA_RES_ID_DINO_DOMI_STING;
		case DINO_KIND_ACID:			return CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB;
		case DINO_KIND_ACID_BOMB:		return CHARA_RES_ID_DINO_DOMI_ACID;
		case DINO_KIND_MUTANTTREX:		return CHARA_RES_ID_DINO_DOMI_MUTANTREX;
		case DINO_KIND_TANK:			return CHARA_RES_ID_DINO_DOMI_TANK;
		case DINO_KIND_ACID_ITEM:		return CHARA_RES_ID_DINO_DOMI_ACID_ITEM;
		case DINO_KIND_RAPTOR_REINFORCE:return CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY;
		case DINO_KIND_STING_REINFORCE:	return CHARA_RES_ID_DINO_DOMI_STING_MARS;
		case DINO_KIND_ACID_REINFORCE:	return CHARA_RES_ID_DINO_DOMI_ACID_VULCAN;
		}
		return CHARA_RES_ID_UNKNOWN;
	}

	UINT32 DinoKind2ItemID( DINO_KIND e)
	{
		switch( e)
		{
		case DINO_KIND_RAPTOR_RED:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR);
		case DINO_KIND_RAPTOR_GREEN:	return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN);
		case DINO_KIND_RAPTOR_BLUE:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE);
		case DINO_KIND_STING:			return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_STING);
		case DINO_KIND_ACID:			return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB);
		case DINO_KIND_ACID_BOMB:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_ACID);
		case DINO_KIND_MUTANTTREX:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_MUTANTREX);
		case DINO_KIND_TANK:			return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_TANK);
		case DINO_KIND_ACID_ITEM:		return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_ACID_ITEM);
		case DINO_KIND_RAPTOR_REINFORCE:return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY);
		case DINO_KIND_STING_REINFORCE:	return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_STING_MARS);
		case DINO_KIND_ACID_REINFORCE:	return MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DINO, CHARACTER_CLASS_UNKNOWN, CHARA_RES_ID_DINO_DOMI_ACID_VULCAN);
		}
		return 0;
	}
	//.......................................................................
};


CGameCharaManager::CGameCharaManager()
{
	m_pBGLoadInfoPool = new i3FixedMemoryPool;
	MEMDUMP_NEW( m_pBGLoadInfoPool, sizeof(i3FixedMemoryPool));
	m_pBGLoadInfoPool->Create( "EquipLoadInfoPool", sizeof( sBGEquipLoadInfo), 128);
	m_pBGLoadInfoPool->SetAutoCompactEnable( false);

	m_pPreloadAnimPool = new i3FixedMemoryPool;
	MEMDUMP_NEW( m_pPreloadAnimPool, sizeof(i3FixedMemoryPool));
	m_pPreloadAnimPool->Create( "PreloadAnimPool", sizeof(tagPRELOADANIMATION), 1024);
	m_pPreloadAnimPool->SetAutoCompactEnable( false);
}

CGameCharaManager::~CGameCharaManager()
{
	Destroy();

	I3_SAFE_RELEASE( m_pAvatarManager);

	I3_SAFE_RELEASE(m_pIOManager);	// 아바타 메니저에서 Thread를 종료하고 IO를 종료해야 RefCount를 측정할 수 있다.

	I3_SAFE_RELEASE( CGameCharaBase::m_spBlendEnableAttr);
	I3_SAFE_RELEASE( CGameCharaBase::m_spBlendModeAttr);

	__ReleasePhysXShape();

	for( UINT32 i = 0; i < m_vDummyCharaInfo.size(); i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( i);
		I3_MUST_RELEASE(pInfo->_resTeamRed._pBody);
		I3_MUST_RELEASE(pInfo->_resTeamBlue._pBody);

		I3MEM_SAFE_FREE( pInfo);
	}
	m_vDummyCharaInfo.clear();

	I3_SAFE_RELEASE( m_pAvatarGen);

	m_MaskResourceMap.clear();

	ClearAIContextList();

	for( UINT32 i = 0; i < m_PhysXControllerList.size(); i++)
	{
		PlayerController * pCtrl = m_PhysXControllerList.at(i);
		I3_SAFE_RELEASE( pCtrl);
	}
	m_PhysXControllerList.clear();

	_FreePreloadAnimQueue();

	I3_SAFE_DELETE( m_pPreloadAnimPool);
	I3_SAFE_DELETE( m_pAIUpperID);
	I3_SAFE_DELETE( m_pAIDualUpperID);

	I3_SAFE_DELETE( m_pBGLoadInfoPool);
}


void CGameCharaManager::ReleaseRespawnPoint()
{
	I3_SAFE_RELEASE( m_pRespawnInfo);
}

void CGameCharaManager::PreCreate( void)
{
	m_pIOManager = i3SceneIOManager::new_object();
	m_pIOManager->Create();
	m_pIOManager->SetCommonResMode( true);

	m_pAvatarManager = cAvatarManager::new_object();
	m_pAvatarManager->Create();

	// 머리 (19MB)
	CreateFaceResource();
	
	// 105MB
	m_pAvatarGen = AvatarGen::new_object();

	m_pAvatarGen->setThreadMode( false);
	m_pAvatarGen->setLoadStyle( AVATAR_LOAD_STYLE_MERGE);
	m_pAvatarGen->setEnableTexture( AVATAR::TEX_NORM, g_pFramework->GetEnableNormalMap());
	m_pAvatarGen->setEnableTexture( AVATAR::TEX_SPEC, g_pFramework->GetEnableSpecularMap());
	m_pAvatarGen->setTextureQuality( g_pViewer->GetRenderContext()->getGfxOption()->getTextureQuality() );
	m_pAvatarGen->setInstanceMode( false);

	CreateDummyCharaInfo( CDI_RED_BULLS, true);
	CreateDummyCharaInfo( CDI_ACID_POLE, true);
	CreateDummyCharaInfo( CDI_TARANTULA, true);
	CreateDummyCharaInfo( CDI_KEENEYES, true);
	CreateDummyCharaInfo( CDI_GRAY, true);
	CreateDummyCharaInfo( CDI_WOLF, true);
}

void CGameCharaManager::PostCreate( void)
{
	//	리소스 미리 로딩 (20MB)
	// 미리 공간 확보
	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		UINT32 maxCnt = 0;
		
		// 모든 아이템의 최대 ResID를 검사하여 mapping한다.
		const INT32 cntInfo = g_pCharaInfoDataBase->GetInfoCount( (EQUIP::ePART) i);
		for( INT32 j = 0; j < cntInfo; j++)
		{
			const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfoByIndex( (EQUIP::ePART) i, j);
			if( maxCnt < pInfo->GetResID())
				maxCnt = pInfo->GetResID();
		}

		m_pEquipRes[i].clear();

		for( UINT32 cnt = 0 ; cnt < maxCnt + 1 ; ++cnt)
			m_pEquipRes[i].push_back( new EquipResSceneGraph( static_cast<EQUIP::ePART>(i) ) );

		for( INT32 j = 0; j < TEAM_COUNT; j++)
		{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			m_ResCheckList[i][j].resize( maxCnt + 1);
#endif
		}
	}
	

	m_DinoRes.resize( CHARA_RES_ID_DINO_E - CHARA_RES_ID_DINO_S);

	//	클랜 텍스처 로드
	LoadClanTexture();

	//	캐릭터 리소스 로드
	LoadCharaResource();

	// AI Index를 찾아두자!!
	__PreCreateAIID();

	// AI Context를 미리 메모리에 로드해놓자 (60MB)
	__CreateAIContext( CHARA_RES_ID_ACID_POLE, SLOT_MAX_COUNT);
	__CreateAIContext( CHARA_RES_ID_SWAT_FEMALE, SLOT_MAX_COUNT);

	m_pIOManager->SetCommonResMode( false);
}

void CGameCharaManager::Destroy( void )
{
	LeaveStage();

	ReleaseCharaResource();
	ReleaseEquipResource();
	ReleaseClanTexture();

}

void CGameCharaManager::ReleaseCharaResource( void)
{
	m_CharaRes.clear();
}

void CGameCharaManager::ReleaseEquipResource( void)
{
	for( INT32 part = 0; part < EQUIP::ePART_COUNT; part++)
	{
		for( UINT32 num = 0 ; num < m_pEquipRes[part].size() ; ++num)
		{
			m_pEquipRes[part][num]->ReleaseGameRes(true);
			I3_SAFE_DELETE(m_pEquipRes[part][num]);
		}

		m_pEquipRes[part].clear();
	}
}

void CGameCharaManager::__BGLoading( void)
{
	sBGEquipLoadInfo * pInfo;

	if( !m_EquipLoadQueue.empty())
	{
		pInfo = m_EquipLoadQueue.front();

		switch( pInfo->_state)
		{
		case eBGLOAD_STATE_PUSH :
			{
				BGLoadThread * pThread = g_pFramework->GetBGLoadThread();

				if( pThread != nullptr )
				{
					char szPath[ MAX_PATH] = {0,};

					pInfo->_pThread = pThread;
					pInfo->_state = eBGLOAD_STATE_LOADING;

					GetEquipPath( szPath, pInfo->_ItemID, pInfo->_team, pInfo->_subtype);

					if( szPath[0] != 0)
					{
						pThread->SetLoadCmd( szPath);
						return;
					}
					else
					{
						I3PRINTLOG(I3LOG_WARN,  "장비 리소스 ID가 잘 못 되었다. 확인 바람 %d", pInfo->_ItemID);
					}
				}
			}
			break;

		case eBGLOAD_STATE_LOADING :
			{
				// 로드 완료를 확인
				i3GameResSceneGraph * pRes = static_cast<i3GameResSceneGraph*>(pInfo->_pThread->GetRes());

				if( pRes != nullptr)
				{
					EQUIP::ePART part = EQUIP::ItemID2PartsType( pInfo->_ItemID);
					EQUIP::RES_ID num = EQUIP::ItemID2ResID( pInfo->_ItemID );

					// 로드 되었지만 Main에서 로드했을 수도 있다. 체크
					if (part < 0 || part >= EQUIP::ePART_COUNT)
					{
						I3_BOUNDCHK(part, EQUIP::ePART_COUNT);
					}
					else
					if( m_pEquipRes[part][num]->GetEquipRes(pInfo->_team, pInfo->_subtype) == nullptr )
					{
						this->SetEquipResource( pInfo->_ItemID, pInfo->_team, pInfo->_subtype, pRes);
					}

					I3_MUST_RELEASE(pRes);	// Thread에서 AddRef한것에 대한 Count를 맞춘다.

					m_pBGLoadInfoPool->Free( pInfo);
					m_EquipLoadQueue.pop();
				}
			}
			break;

		case eBGLOAD_STATE_FLUSH :
			{
				// 로드 완료를 확인
				i3GameResSceneGraph * pRes = static_cast<i3GameResSceneGraph*>(pInfo->_pThread->GetRes());

				if( pRes != nullptr)
				{
					I3_MUST_RELEASE(pRes);
					m_pBGLoadInfoPool->Free( pInfo);
					m_EquipLoadQueue.pop();
				}
			}
			break;
		}
	}
}

bool CGameCharaManager::__IsLoadPushed( T_ItemID itemID)
{

	UINT32 cnt = m_EquipLoadQueue.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		sBGEquipLoadInfo * pInfo = m_EquipLoadQueue.front();

		m_EquipLoadQueue.pop();
		m_EquipLoadQueue.push( pInfo);

		if( pInfo->_ItemID == itemID)
			return true;
	}
	
	return false;
}

void CGameCharaManager::PushLoadEquip( CGameCharaBase * pOwner, CHARA_RES_ID CharaType, T_ItemID itemID)
{
	if( itemID == 0)
		return;

	TEAM_TYPE teamType = CHARA::CharaTeamType2TeamType(pOwner->getCharaTeam());

	EQUIP::eBERET res_subtype = EQUIP::eBERET_NONE;
	if( EQUIP::ItemID2PartsType(itemID) == EQUIP::ePART_BERET )
	{
		const NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->getNetCharaInfo( pOwner->getCharaNetIndex(), pOwner->getBufferType() );
		T_ItemID head_id = pNetCharaInfo->getCharaParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HEAD);

		if( head_id != itemID )
		{
			const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo(head_id);
			res_subtype = static_cast<EQUIP::eBERET>(pEquipInfo->GetCombinationType());
		}
	}

	const i3GameResSceneGraph * pRes = QueryEquipResource( itemID, teamType, res_subtype, false);

	if( pRes == nullptr)
	{
		//if( __IsLoadPushed( itemID) == false)
		{
			sBGEquipLoadInfo * pNewLoadCmd = reinterpret_cast<sBGEquipLoadInfo*>(m_pBGLoadInfoPool->Alloc( sizeof( sBGEquipLoadInfo), __FILE__, __LINE__));

			pNewLoadCmd->_pOwner = pOwner;
			pNewLoadCmd->_state = eBGLOAD_STATE_PUSH;
			pNewLoadCmd->_charaID = CharaType;
			pNewLoadCmd->_ItemID = itemID;
			pNewLoadCmd->_team = teamType;
			pNewLoadCmd->_subtype = res_subtype;

			m_EquipLoadQueue.push( pNewLoadCmd);

			I3TRACE( "PreLoad Equip %d\n", itemID);
		}
	}
}

void CGameCharaManager::LoadCharaResource( void)
{
	i3::rc_wstring strPath;

	INT32	Count = g_pCharaInfoDataBase->GetInfoCount( EQUIP::ePART_CHARA);
	T_ItemID nItemIDMale = static_cast<T_ItemID>(GetItemIndex( "AcidPole"));
	T_ItemID nItemIDFemale = static_cast<T_ItemID>(GetItemIndex( "SWAT_Female"));

	m_CharaRes.resize( CHARA_RES_ID_HUMAN_E);

	for( INT32 i = 0 ; i < Count ; i++ )
	{
		const CCharaInfo * pInfo = (const CCharaInfo*) g_pCharaInfoDataBase->GetInfo( EQUIP::ePART_CHARA, i );
		if( pInfo == nullptr)
			continue;

		if( pInfo->GetITEMID() == (unsigned)nItemIDMale ||
			pInfo->GetITEMID() == (unsigned)nItemIDFemale )
		{
			pInfo->getResPath( strPath);

			if( !strPath.empty() )		// ResPath 가 지정된 캐릭터만 로드
			{
				// char szTemp[ MAX_PATH];
				i3::stack_wstring	wstrFileName;
				i3::extract_filename( strPath, wstrFileName);
				i3::string strFileName;		i3::utf16_to_mb(wstrFileName, strFileName);

				i3GameResChara * pRes = (i3GameResChara *) g_pFramework->QuaryResource( strFileName.c_str());
				I3ASSERT( pRes != nullptr);

				CHARA_RES_ID resID = (CHARA_RES_ID) pInfo->GetResID();

				I3ASSERT( m_CharaRes.at( resID) == nullptr);
				m_CharaRes[ resID] = pRes;
			}
		}
	}

	// 캐릭터 컬리젼 쉐입을 16개만큼 만들어 놓자!!
	__FindBoneName( CHARA_RES_ID_ACID_POLE);
	__FindBoneName( CHARA_RES_ID_SWAT_FEMALE);
}

void CGameCharaManager::__FindBoneName( CHARA_RES_ID resID)
{
	INT32 i;
	CHARA::PHYSX_SHAPE_TYPE phsxType = (resID == CHARA_RES_ID_ACID_POLE)? CHARA::PHYSX_SHAPE_MALE : CHARA::PHYSX_SHAPE_FEMALE;
	const i3GameResChara * pRes = QueryCharaResource( resID, true);

	if( pRes != nullptr)
	{
		i3Body * pSrcBody = (i3Body*) i3Scene::FindNodeByExactType( ((i3GameResChara *)pRes)->getScene(), i3Body::static_meta());
		I3ASSERT( pSrcBody != nullptr);

		i3LOD * pLOD = pSrcBody->getFrameLOD();
		I3ASSERT( pLOD != nullptr);

		i3Skeleton * pSrcSkel = pLOD->getSkeleton();

		for( i = 0; i < pSrcSkel->getBoneCount(); i++)
		{
			i3Bone * pBone = pSrcSkel->getBone( i);
			i3::safe_string_copy( m_szShapeBoneName[phsxType][ i], pBone->getName(), MAX_NAME);
		}

		for( i = pSrcSkel->getBoneCount(); i < 128; i++)
			m_szShapeBoneName[phsxType][ i][0] = 0;
	}
}

i3GameResChara * CGameCharaManager::_LoadCharaRes( const char * pszCHR, const char * pszPath)
{
	i3GameResChara * pRes= (i3GameResChara *) g_pFramework->FindResourceByPath( pszPath);

	if( !i3::same_of<i3GameResChara*>(pRes))
	{
#ifdef BREAK_LOAD_CHARARES
		I3PRINTLOG(I3LOG_FATAL, "Cannot find resource %s %s", pszPath, pszCHR);		
#else
		I3PRINTLOG(I3LOG_NOTICE,"Cannot find resource %s %s", pszPath, pszCHR);
#endif
	}

	return pRes;
}


void CGameCharaManager::RemoveAllChara( void )
{
	INT32 i;
	for( i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		RemoveChara(i);
	}

	RemoveAllAIChara();

	DestroyAIIReserveAI();
	I3ASSERT( m_CharaList.size() == 0);	//케릭터가 있으면 안됨.
}

void CGameCharaManager::RemoveAllAIChara( void )
{
	for( INT32 i = 0; i < MAX_COUNT_DOMI_DINO; i++ )
		RemoveAiChara( i);
}

void CGameCharaManager::_FirstSetChara( INT32 idx, CGameCharaBase * pChara, CHARACTER_TEAM_TYPE team)
{
	{
 		VEC3D vPos;
 
 		REAL32	fTheta = GetRespawnPosition( &vPos, team, idx);
 
 		pChara->setDynamicActiveState( true, false );
 		pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );
 		
 		if (pChara->getMoveContext())
 		{
 			pChara->forceSetPos( &vPos);
  			pChara->getMoveContext()->setTakeOffPos( &vPos);			// 캐릭터 생성시 월드가 높은 곳에 있으면 낙하처리가 됩니다.
 		}
 
 		if (pChara->getBoneContext())
  			pChara->getBoneContext()->setTheta( fTheta );
  
  		// Init network information
 		if (pChara->getMoveContext())
 		{
  			pChara->getMoveContext()->setPostPosition( &vPos);
  			pChara->getMoveContext()->setRecvPosition( &vPos);
  			pChara->getMoveContext()->setRecvPosTime( 1.f);
 		}
		
		setCharaTask( idx, CHARA_TASK_CREATE);
		setCharaResState( idx, RESSTATE_LOADED);
	}
	

	// Sound Owner설정
	if (pChara->getSoundContext())
		pChara->getSoundContext()->SetSoundOwner();

	setCharaByNetIdx( idx, pChara);

	// 캐릭터 마크 설정
	if( pChara->isNetwork() )
	{
		if (pChara->getMaterialContext())
			pChara->getMaterialContext()->SetTeamMarkEnable();
	}
	
	pChara->setDynamicActiveState( true, false);
	pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );
	pChara->SetVisible( false );
	
	// 이전 사용자의 발사 패킷을 초기화합니다. 라운드가 새로시작하면서 캐릭터가 교체되는 경우가 발생합니다.(연합미션)
	GAMEINFO_CHARACTER * pCharaGameInfo = BattleSlotContext::i()->setGameInfo_Chara( idx);
	pCharaGameInfo->Reset();

	//	무조건 HP를 내부에서 100으로 셋팅하기 때문에 HP 증가 캐시템 사용시 적용해야 한다.
	pCharaGameInfo->_tNetData._tHP.setHP( (UINT16) pChara->getFullHP());

	I3TRACE( "First Set Chara %d %d!!!!!!!!!!!\n", idx, i3::same_of<Avatar*>(pChara)?1:0);
}

CGameCharaBase * CGameCharaManager::_AddChara( INT32 idx, GLOBAL_CHARA_INFO * pInfo)
{
	CGameCharaBase * pChara = AttachCharaToStage( pInfo, m_pStage );

	if( pChara == nullptr )
	{
		I3PRINTLOG(I3LOG_FATAL,  "알 수 없는 캐릭터입니다." );
		return nullptr;
	}

	if (UserInfoContext::i()->IsGM_Observer())
	{
		CGameFramework * pFramework = (CGameFramework*)i3Framework::getCurrentFramework();

		pChara->addGNodeStyle(I3_GAMENODE_STYLE_XRAY);
		pChara->AttachScene();

		SetXrayState(pChara, true);
		pFramework->getSgContext()->GetXrayScene()->SetXrayState(true);
	}
	else
	{
		pChara->AttachScene();
	}

	_FirstSetChara( idx, pChara, pInfo->GetTeam());

	return pChara;
}

CGameCharaBase * CGameCharaManager::AddChara( INT32 idx, CHARA_RES_ID ResType,
											 bool bRespawnPos, CHARA::TYPE charaType, bool bRecord /*= true*/)
{
	if( idx >= SLOT_MAX_COUNT || idx < 0)
	{
		#if !defined( DEF_OBSERVER_MODE)
			I3TRACE("잘못된 ADD 입니다.\n");
		#endif
		return nullptr;
	}

	GLOBAL_CHARA_INFO * pCharaInfo = setCharaInfo( idx, (charaType==CHARA::TYPE_DOMI_FULLBODY_AI)?true:false);

	MakeCharaInfo( pCharaInfo, idx, ResType, charaType);

	return _AddChara( idx, pCharaInfo);
}


CGameCharaBase * CGameCharaManager::AddAiChara( INT32 idx, CHARA_RES_ID ResType, bool bRespawnPos, CHARA::TYPE charaType, bool bRecord)
{
	GLOBAL_CHARA_INFO *	pCharaInfo = setCharaInfo( idx, (charaType==CHARA::TYPE_DOMI_FULLBODY_AI) ? true : false);

	MakeCharaInfo( pCharaInfo, idx, ResType, charaType);

	CGameCharaBase * pChara = AttachCharaToStage( pCharaInfo, m_pStage );

	if( pChara == nullptr )
	{
		I3PRINTLOG(I3LOG_FATAL,  "알 수 없는 캐릭터입니다." );
		return nullptr;
	}

	pChara->AttachScene();
	
	UINT32	nRand = 0;
	INT32 nRandConst = m_pRespawnInfo->GetRespawnCount( pCharaInfo->GetTeam());
	
	if( nRandConst <= 0)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%d 팀의 Respawn Position이 없습니다.\n 1:Red 2:Blue 3:R_NPC 4:B_NPC", pCharaInfo->GetTeam());
		nRandConst = 1;
	}

	INT32 roomIdx = MyRoomInfoContext::i()->getMyRoomIndex();
	INT32 roundCount = ScoreContext::i()->getTotalRoundCount();

	nRand		= ( roomIdx + roundCount + idx/2)	% nRandConst;


	i3RespawnObj * pObj = m_pRespawnInfo->GetRespawnObject( pCharaInfo->GetTeam(), nRand);

	REAL32	fTheta = pObj->GetTheta();

	pChara->setDynamicActiveState( true, false );
	pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );

	VEC3D vPos;
	i3Vector::Copy( &vPos, pObj->GetPos());
	vPos.y += 0.2f;
	pChara->forceSetPos( &vPos);
	pChara->getMoveContext()->setTakeOffPos( &vPos);			// 캐릭터 생성시 월드가 높은 곳에 있으면 낙하처리가 됩니다.
	pChara->getBoneContext()->setTheta( fTheta );

	pChara->getMoveContext()->setPostPosition( &vPos);
	pChara->getMoveContext()->setRecvPosition( &vPos);
	pChara->getMoveContext()->setRecvPosTime( 1.f);

	pChara->getSoundContext()->SetSoundOwner();

	{//첼린지 모드
		setCharaByNetIdx( idx, pChara);

		setCharaTask( idx, CHARA_TASK_CREATE);
		setCharaResState( idx, RESSTATE_LOADED);
	}

	GAMEINFO_CHARACTER * pCharaGameInfo = BattleSlotContext::i()->setGameInfo_Chara( idx, pChara->getBufferType());

	I3ASSERT( pCharaGameInfo != nullptr);
	pCharaGameInfo->Reset();
	pCharaGameInfo->_tNetData._tHP.setHP( (UINT16) pChara->getFullHP());

	return pChara;
}

void CGameCharaManager::ChangeToDino(DINO_ROOM_INFO* pDinoRoomInfo)
{
	I3ASSERT(pDinoRoomInfo);

#ifdef I3_DEBUG
	GlobalFunc::PB_TRACE("AddDino");
	GlobalFunc::PB_TRACE("_uiTRexSlotIdx %d", pDinoRoomInfo->_uiTRexSlotIdx);
#endif
	for(INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++ )
	{
		if(pDinoRoomInfo->_uiRaptorSlotIdx[i] != 255 )
		{
			UINT8 SlotIdx = pDinoRoomInfo->_uiRaptorSlotIdx[i];
			if(getCharaByNetIdx(SlotIdx) != nullptr)
			{
				const COMMON_PARTS_INFO * pPartsInfo = BattleSlotContext::i()->getNetCharaInfo(SlotIdx)->getCommonParts();
				// vv3 - check
				T_ItemID dino_id = pPartsInfo->m_Info[CHAR_EQUIPMENT_COMMON_DINO].m_TItemID;

				const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( dino_id);
				CHARA_RES_ID userEquipDino = (CHARA_RES_ID)pCharaDBInfo->GetResID();

				if(getCharaByNetIdx(SlotIdx)->getCharaInfo()->GetResType() != userEquipDino /*장비한 공룡 캐릭터의 리소스*/) 
				{
					RemoveCharaAddDinoChara(SlotIdx, userEquipDino /*장비한 공룡 캐릭터의 리소스*/);
				}
			}
		}
	}
}

void CGameCharaManager::ChangeToTRex(DINO_ROOM_INFO* pDinoRoomInfo, INT32 iMySlotIdx)
{
	I3ASSERT(pDinoRoomInfo);

#ifdef I3_DEBUG
	GlobalFunc::PB_TRACE("AddTRex");
	GlobalFunc::PB_TRACE("_uiTRexSlotIdx %d", pDinoRoomInfo->_uiTRexSlotIdx);
#endif
	if(pDinoRoomInfo->_uiTRexSlotIdx != 255 )
	{
		if(getCharaByNetIdx(pDinoRoomInfo->_uiTRexSlotIdx) != nullptr)
		{
			if(getCharaByNetIdx(pDinoRoomInfo->_uiTRexSlotIdx)->getCharaInfo()->GetResType() != CHARA_RES_ID_DINO_TREX) 
			{
				RemoveCharaAddDinoChara(pDinoRoomInfo->_uiTRexSlotIdx, CHARA_RES_ID_DINO_TREX);
			}
		}
	}
}

void CGameCharaManager::RemoveCharaAddDinoChara( INT32 idx, CHARA_RES_ID ResType)
{
	if(idx == WRONG_SLOT_INDEX) {
		return;
	}

#if defined( DEF_OBSERVER_MODE)
	I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
#endif

	if(idx == BattleSlotContext::i()->getMySlotIdx())
	{
		//if( ResType > CHARA_RES_ID_HUMAN_E)
		//if( getCharaByNetIdx( idx) != nullptr)
		//{	// 캐릭터에서 공룡으로 바뀌면 저장
		//	if( i3::same_of( getCharaByNetIdx( idx), Avatar::static_meta()) )
		//	{
		//		I3_REF_CHANGE( m_pMyChara, getCharaByNetIdx( idx));
		//	}
		//}

		RemoveChara(idx);
		CGameCharaBase* pMyChara = AddChara(idx, ResType);
		CGameCameraManager::i()->setTargetCharacter( pMyChara);

		//캐릭터 생성 후 리스폰 위치로 이동 hansoft.4555
		VEC3D pos;
		REAL32 theta = GetRespawnPosition( &pos, pMyChara->getCharaInfo()->GetTeam(), idx, -1, false);
		pMyChara->Cmd_Respawn( &pos, theta, false);
		pMyChara->addCharaStateMask( CHARA_STATE_DEATH);
		ExecuteRespawnCheatkey();
	}
	else
	{
		RemoveChara(idx);
		AddChara(idx, ResType);
	}
}

void CGameCharaManager::SetVisibleAtPlayingChara()
{
	INT32 i = 0;
	CGameCharaBase* pChara = nullptr;

	for(i=0; i<SLOT_MAX_COUNT; i++) {
		if( UserContext::i()->IsPlayingSlot(i)) {
			pChara = getCharaByNetIdx(i);
			if(pChara != nullptr) {
				pChara->SetVisible(true);
			}
		}
	}
}

void CGameCharaManager::RemoveAiChara( INT32 idx, bool bRecord /*= true*/)
{
	if( idx < 0 || idx >= MAX_COUNT_DOMI_DINO) 
		return;

	_RemoveChara( m_AICharaData[ idx]._pChara);
	m_AICharaData[ idx]._pChara = nullptr;
	m_AICharaData[ idx]._CharaInfo.Reset();
}

void CGameCharaManager::RemoveChara( INT32 idx, bool bRecord /*= true*/ )
{
	if( idx < 0 || idx >= SLOT_MAX_COUNT)
	{
		I3PRINTLOG(I3LOG_FATAL,  "RemoveChara Index Invalid!!! %d", idx);
		return;
	}

	Avatar * pChara = (Avatar*)getCharaByNetIdx( idx);

	if( pChara != nullptr)
	{
		I3TRACE( "Remove Chara(1) %d - %d\n", idx, pChara->GetRefCount() );

		_RemoveChara( pChara);
		setCharaByNetIdx( idx, nullptr);
		setCharaInfo(idx)->Reset();
		setCharaResState( idx, RESSTATE_NONE);
		setCharaTeamAutoChange( idx, false);
	}
}

void CGameCharaManager::SetCharaWeaponSet( CGameCharaBase * pChara, const ITEM_INFO * pList)
{
	// 해당 함수는 무기 정보만 필요한 함수이고, NET_CHAR_INFO, UI_CHARA_INFO 가 공통적으로 쓰이는 함수 이므로
	// 매개 변수를 바꾼다.

	// 50ms 
	I3ASSERT(pChara != nullptr && pList != nullptr);
	
	INT32 i;

	GameContextUtil::CheckWeaponList( pList );

	UINT8 WeaponFlag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();
	
	T_ItemID  ui32WeaponItemID = 0;

	for( i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++ )
	{
		WEAPON_SLOT_TYPE weaponSlot = static_cast<WEAPON_SLOT_TYPE>(i);
		ui32WeaponItemID = pList[weaponSlot].m_TItemID;

		//탈취모드 예외처리
		if( ui32WeaponItemID == 0 )	// <-- 무기가 설정되지 않았을때 들어옵니다.	
		{// revision 47814, 59375 주무기 없는 상태에서 RPG전 시작시, 무기 설정 안되는 버그 수정 - hansoft 3406
			// 주무기의 경우, RPG or Shotgun Mode인 경우에는 삭제하지 않습니다.
			// 삭제 조건: 1. 주무기가 아닌 경우 2. 주무기 Lock 해제 및 RPG 사용 3. 주무기 Lock 사용 및 ShotGun 사용 
			if( (weaponSlot != WEAPON_SLOT_PRIMARY) || 
				( ((WeaponFlag & LOCK_PRIMARY) == 0) && ((WeaponFlag & LOCK_RPG7) == 0) ) ||
				( ((WeaponFlag & LOCK_PRIMARY) != 0) && ((WeaponFlag & LOCK_SHOTGUN) == 0) ) )
			{
				pChara->Cmd_DeleteWeapon( weaponSlot);
				continue;	// 해당 무기는 Lock되어 있습니다.
			}
		}
		else if( ((WeaponFlag & LOCK_PRIMARY) == 0) &&
			(weaponSlot == WEAPON_SLOT_PRIMARY) &&
			((WeaponFlag & LOCK_RPG7) == 0) &&
			((WeaponFlag & LOCK_SHOTGUN) == 0))
		{
			pChara->Cmd_DeleteWeapon( weaponSlot);
			continue;	// 해당 무기는 Lock되어 있습니다.
		}
		else if( ((WeaponFlag & LOCK_SECONDARY) == 0) &&
			(weaponSlot == WEAPON_SLOT_SECONDARY) )
		{
			pChara->Cmd_DeleteWeapon( weaponSlot);
			continue;	// 해당 무기는 Lock되어 있습니다.
		}
		else if( ((WeaponFlag & LOCK_MELEE) == 0) &&
			(weaponSlot == WEAPON_SLOT_MELEE) )
		{
			if( (WeaponFlag & LOCK_MELEE) == 0 &&
				(WeaponFlag & LOCK_KNUCKLE) == 0 )
			{
				pChara->Cmd_DeleteWeapon( weaponSlot);
				continue;	// 해당 무기는 Lock되어 있습니다.
			}
		}
		else if( ((WeaponFlag & LOCK_THROW) == 0) &&
			(weaponSlot == WEAPON_SLOT_THROWING1) )
		{
			pChara->Cmd_DeleteWeapon( weaponSlot);
			continue;	// 해당 무기는 Lock되어 있습니다.
		}

		// 특수전(저격전, 샷건전, 너클전..)의 무기 ItemID 를 받아옵니다. 
		ui32WeaponItemID = g_pFramework->FindWeaponIDForGameMode(ui32WeaponItemID);

		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)
			&& MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
		{
			if( weaponSlot == WEAPON_SLOT_MELEE)
			{
				WEAPON_CLASS_TYPE classType = WEAPON_CLASS_KNIFE;
				WEAPON::RES_ID iWeaponNumber = WEAPON::ItemID2ResID( ui32WeaponItemID);

				if( (iWeaponNumber != WEAPON::getItemIndex(WEAPON::CHINESECLEAVER)) 
					&& ( iWeaponNumber != WEAPON::getItemIndex(WEAPON::CHINESECLEAVER_SEIZED)) )
				{
					if( pChara->getTeam() ==  TEAM_RED)
						iWeaponNumber = WEAPON::getItemIndex( WEAPON::HAIRDRYER);
					else
						iWeaponNumber = WEAPON::getItemIndex( WEAPON::FRYINGPAN);
				}

				ui32WeaponItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE,	classType, iWeaponNumber);
			}
			else
			{
				pChara->Cmd_DeleteWeapon( weaponSlot);
				continue;
			}
		}

		//겨울 탈출 모드
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY)
			&& MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
		{
			WEAPON_CLASS_TYPE classType;
			WEAPON::RES_ID iWeaponNumber = WEAPON::ItemID2ResID( ui32WeaponItemID);

			if( weaponSlot == WEAPON_SLOT_PRIMARY )
			{
				classType = WEAPON_CLASS_ASSAULT;

				if( iWeaponNumber != WEAPON::getItemIndex(WEAPON::SNOW_BALL_BLASTER) )
				{
					iWeaponNumber = WEAPON::getItemIndex(WEAPON::SNOW_BALL_BLASTER);
				}

				ui32WeaponItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY,	classType, iWeaponNumber);
			}
			else if( weaponSlot == WEAPON_SLOT_SECONDARY )
			{
				classType = WEAPON_CLASS_HANDGUN;

				if( iWeaponNumber != WEAPON::getItemIndex(WEAPON::HANDGUN_MGL32) )
				{
					iWeaponNumber = WEAPON::getItemIndex(WEAPON::HANDGUN_MGL32);
				}

				ui32WeaponItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY,	classType, iWeaponNumber);
			}
			else if( weaponSlot == WEAPON_SLOT_MELEE )
			{
				classType = WEAPON_CLASS_KNIFE;

				if( iWeaponNumber != WEAPON::getItemIndex(WEAPON::KNIFE_ICICLE)
					&& iWeaponNumber != WEAPON::getItemIndex(WEAPON::KNIFE_ICE_FORK))
				{
					iWeaponNumber = WEAPON::getItemIndex(WEAPON::KNIFE_ICICLE);
				}

				ui32WeaponItemID = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE,	classType, iWeaponNumber);
			}
			else
			{
				pChara->Cmd_DeleteWeapon( weaponSlot);
				continue;
			}
		}

		if( !pChara->Cmd_CreateWeapon( weaponSlot, ui32WeaponItemID))
		{
			//I3TRACE( "Error - WeaponList is invalid.. idx : %d slot : %d id : %d \n", pChara->getCharaInfo()->GetNetIdx(), i, ui32WeaponItemID );
		}
	}
	I3ASSERT( pChara->getCharaInfo() != nullptr);


	//C4 제공합니다.
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) )
	{
		if( pChara->getCharaTeam() & CHARACTER_TEAM_RED)
		{
			T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex( WEAPON::C4));
			pChara->Cmd_CreateWeapon( WEAPON_SLOT_MISSION, Itemid);
		}
	}
	//C4 제공합니다.
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex( WEAPON::C4));
		pChara->Cmd_CreateWeapon( WEAPON_SLOT_MISSION, Itemid);
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	else if( !g_pConfig->m_bNetworking )	// Local Mode
	{
		UINT32	Class = 0;
		Class = Weapon::GetItemTypeFromClass( (WEAPON_CLASS_TYPE)g_DebugWeaponSet[5][0]);
		T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class, g_DebugWeaponSet[5][0], g_DebugWeaponSet[5][1]);
		pChara->Cmd_CreateWeapon( WEAPON_SLOT_MISSION, Itemid);
	}
#endif
}

void CGameCharaManager::SetCharaWeaponSetWithoutLock( CGameCharaBase * pChara, NET_CHARA_INFO * pNetCharaInfo)
{
	I3ASSERT(pChara != nullptr && pNetCharaInfo != nullptr);

	// 인벤토리에서는 무기제한을 적용하지 않는다
	GameContextUtil::CheckWeaponList( pNetCharaInfo->getWeaponList( WEAPON_SLOT_PRIMARY));


	INT32 i = 0;

	T_ItemID ui32WeaponItemID = 0;
	for( i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++ )
	{
		WEAPON_SLOT_TYPE slot = static_cast<WEAPON_SLOT_TYPE>(i);
		// 1.5 Check Weapon
		ui32WeaponItemID = pNetCharaInfo->getWeaponItemID( slot);
		if( !pChara->Cmd_CreateWeapon( slot, ui32WeaponItemID))
		{
			I3TRACE( "Error - 2 WeaponList is invalid.. idx : %d slot : %d id : %d \n", pChara->getCharaInfo()->GetNetIdx(), i, ui32WeaponItemID );
		}
	}

	I3ASSERT( pChara->getCharaInfo() != nullptr);

	//폭탄지급  호출하는 곳 확인후. 로비이면 삭제 합니다.
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) )
	{
		if( pChara->getCharaTeam() & CHARACTER_TEAM_RED)
		{
			T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
			pChara->Cmd_CreateWeapon( WEAPON_SLOT_MISSION, Itemid);
		}
	}
}

void CGameCharaManager::SetCharaEquipSet(CGameCharaBase * pChara, const GLOBAL_CHARA_INFO * pCharaInfo)
{
	// vv3 - check
	const CHARA_PARTS_INFO * pPartsInfo = pCharaInfo->GetParts();
	T_ItemID itemID = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HEAD);
	T_ItemID subItemID = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_BERET);

	pChara->Cmd_CreateEquip( (CHARA_RES_ID)pCharaInfo->GetResType(), itemID, subItemID);
}

void CGameCharaManager::RespawnCharaForAI( INT32 idx, bool bRecord /*= true*/ )
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3PRINTLOG(I3LOG_NOTICE, BattleSlotContext::i()->getNickForSlot( idx));
#endif

	// 초기화
	VEC3D	vRespawnPos;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	//제압 미션 AI일 경우
	const GLOBAL_CHARA_INFO * pInfo = getCharaInfo( idx, false);

	CGameCharaBase * pChara = getCharaByNetIdx( idx, pInfo->GetType() == CHARA::TYPE_DOMI_FULLBODY_AI);

	if( pChara == nullptr )
	{
		//I3PRINTLOG(I3LOG_FATAL,  "아직 캐릭터를 생성하지 못한 녀석이 리스폰한다.!!!!%d", idx);
		return;
	}
	GAMEINFO_CHARACTER * pCharaInfo	= BattleSlotContext::i()->setGameInfo_Chara( idx, pChara->getBufferType());

	setCharaTask( idx, CHARA_TASK_RESPAWN);

	// 리스폰 무기 설정
	const NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->getNetCharaInfo( idx, pChara->getBufferType());

	if(pChara->IsDino() == false)
	{
		// 주무기만 바꾸는게 아니라 해당 리스트 배열 시작점을 넘기는 겁니다.
		SetCharaWeaponSet( pChara, pNetCharaInfo->getWeaponList(WEAPON_SLOT_PRIMARY));
	}

	REAL32 theta = 0.f;
	if( pInfo->GetType() == CHARA::TYPE_DOMI_FULLBODY_AI)
	{
		theta = GetRespawnPosition( &vRespawnPos, pInfo->GetTeam(), idx, pNetCharaInfo->getRespawnObjIndex(), true);
	}
	else
	{
		theta = GetRespawnPosition( &vRespawnPos, pInfo->GetTeam(), idx );
		UserContext::i()->Death[idx] = false;
	}


	pChara->setDynamicActiveState( true, false );
	pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );
	
	pChara->Cmd_Respawn( &vRespawnPos, theta );

	if( pChara->IsDino() == false)
	{
		if( !pChara->isAttachedProtectParts() )
		{
			CGameCharaEquipContext * pEquipCtx = pChara->getEquipContext();
			EQUIP::ePART part_type = pChara->getProtectPartsType();

			CGameCharaEquip * pEquip = pEquipCtx->getEquip(part_type);
			if( pEquip != nullptr )
			{
				I3ASSERT( i3::kind_of<CGameCharaEquip*>(pEquip));

				// 월드에서 띄어냅니다.
				pEquipCtx->Detach( part_type);

				// 캐릭터에 붙입니다.
				if( !pEquipCtx->Attach( part_type))
				{
					I3TRACE( "헬멧이 안 붙었어요~\n" );
				}
			}
		}
	}

	// 오브젝트 위에서 리스폰
	_RespawnOnObject( pChara, pNetCharaInfo);

	// 통신용 데이터 초기화
	pCharaInfo->_tNetData.Reset();

	if(pChara->IsAcid())
		pCharaInfo->_tNetData._tHP.setHP( (UINT16) pChara->getCurHP());
	else if(pChara->IsCCAcid() || pChara->IsAcidVulcan())
		pCharaInfo->_tNetData._tHP.setHP( (UINT16) pChara->getCurHP());
	else
		pCharaInfo->_tNetData._tHP.setHP( (UINT16) pChara->getFullHP());
	pCharaInfo->_tNetData._tPosRotData.setPos( &vRespawnPos );

	pChara->getMoveContext()->setRecvPosition( &vRespawnPos);
	pChara->getMoveContext()->setPostPosition( &vRespawnPos);

	BattleServerContext::i()->InitMulticastInfo( idx );

	pChara->getMoveContext()->EnablePushingRandomDir();

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3PRINTLOG(I3LOG_NOTICE, BattleSlotContext::i()->getNickForSlot( idx));
#endif
}

void CGameCharaManager::RespawnChara( INT32 idx, bool bRecord /*= true*/, bool bFirstRespawn /*=false*/, bool bInvincible /* = true */ )
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	/*I3TRACE(" 리스폰 인덱스 정보 : %d\n", idx);
	I3PRINTLOG(I3LOG_NOTICE, BattleSlotContext::i()->getNickForSlot( idx));*/
#endif

	// 초기화
	VEC3D	vRespawnPos;
	REAL32 theta;
	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	GAMEINFO_CHARACTER * pCharaInfo	= BattleSlotContext::i()->setGameInfo_Chara( idx );

	const GLOBAL_CHARA_INFO * pInfo = getCharaInfo( idx);

	if( pInfo->GetType() == CHARA::TYPE_NA )
	{
		I3TRACE( "잘 못된 리스폰입니다. %d\n", idx );
		return;
	}

	CGameCharaBase * pChara = getCharaByNetIdx( idx);
	I3ASSERT( pChara != nullptr);
	if( pChara == nullptr )
		return;

	if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && pChara->isAI())
	{
		return;
	}

	pChara->SetVisible(true);

	// 캐릭터 마크 설정
	if( pChara->isNetwork() )
	{
		if (pChara->getMaterialContext())
			pChara->getMaterialContext()->SetTeamMarkEnable();
	}

	setCharaTask( idx, CHARA_TASK_RESPAWN);

	pMissionMng->ProcessCharaRespawn( idx);
	UserContext::i()->Death[idx] = false;

	// 리스폰 무기 설정
	NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( idx );

	//panelty reset
	HackContext::i()->resetHackUserSlot( idx);

	pNetCharaInfo->setHP( (UINT8) MINMAX( 100, pChara->getFullHP(), 200 ) );
	pChara->setCharaInfo()->SetHP( (REAL32) MINMAX( 100, pChara->getFullHP(), 200 ));

	if(pChara->IsDino() == false)
	{
		if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && !pChara->IsDino())
		{
			pChara->getMultiWeaponContext()->InitMultiWeaponList();
		}

		// 주무기만 바꾸는게 아니라 해당 무기 리스트 배열 시작점을 넘깁니다.
		SetCharaWeaponSet( pChara, pNetCharaInfo->getWeaponList(WEAPON_SLOT_PRIMARY));

		if( !pChara->isAttachedProtectParts() )
		{
			CGameCharaEquipContext * pEquipCtx = pChara->getEquipContext();
			EQUIP::ePART part_type = pChara->getProtectPartsType();

			CGameCharaEquip * pEquip = pEquipCtx->getEquip( part_type);
			if( pEquip != nullptr )
			{
				I3ASSERT( i3::kind_of<CGameCharaEquip*>(pEquip));

				// 월드에서 띄어냅니다.
				pEquipCtx->Detach( part_type);

				// 캐릭터에 붙입니다.
				if( !pEquipCtx->Attach( part_type))
				{
					I3TRACE( "헬멧이 안 붙었어요~\n" );
				}
			}
		}
	}

	// 리스폰 위치 설정
	char szString[MAX_STRING_COUNT];
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || 
		MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION) )
	{	
		theta = GetRespawnPosition( &vRespawnPos, pInfo->GetTeam(), idx);
		sprintf_s( szString, "RESPAWN CHARA %d", idx);
	}
	else
	{
		INT32 nRespawnObjectIndex = -1;
		theta = GetRespawnPosition( &vRespawnPos, pInfo->GetTeam(), (INT32) pNetCharaInfo->getRespawnIndex(), nRespawnObjectIndex);
		sprintf_s( szString, "RESPAWN CHARA %d",  pNetCharaInfo->getRespawnIndex());
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if (pChara->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
		I3PRINTLOG(I3LOG_DEFALUT, szString);
	GlobalFunc::PB_TRACE( "Chara Respawn ( %d, %s, respawnIdx %d )", idx, BattleSlotContext::i()->getNickForSlot( idx ), pNetCharaInfo->getRespawnIndex() );
#endif

	pChara->setDynamicActiveState( true, false );
	pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );

	if( (bFirstRespawn == true) && (pChara->isLocal() == false) )
		bInvincible = false;

	pChara->Cmd_Respawn( &vRespawnPos, theta , bInvincible);

	// 오브젝트 위에서 리스폰
	_RespawnOnObject( pChara, pNetCharaInfo);

	// UDP 통신용 데이터 초기화
	pCharaInfo->_tNetData.Reset();
	pCharaInfo->_tNetData._tHP.setHP( (UINT16) pChara->getFullHP());

	pCharaInfo->_tNetData._tPosRotData.setPos( &vRespawnPos );

	if (pChara->getMoveContext())
	{
		pChara->getMoveContext()->setRecvPosition( &vRespawnPos);
		pChara->getMoveContext()->setPostPosition( &vRespawnPos);
	}

	BattleServerContext::i()->InitMulticastInfo( idx );


	if (idx == BattleSlotContext::i()->getMySlotIdx() )
	{
		// 리스폰시 직전의 죽음이 본인의 애시드자폭이었다면 자폭직전의 체력으로 설정한다.
		CGameCharaBase * pChara2 = g_pCharaManager->getCharaByNetIdx( idx);
		pChara2->setCharaInfo()->LoadHP();

		pNetCharaInfo->setMyRespawnCount( (UINT8)UserInfoContext::i()->GetMyRespawnCnt()); 
		I3TRACE("내 리스폰 인덱스 %d \n", pNetCharaInfo->getRespawnIndex() );
		I3TRACE("X : %f   Y: %f   Z: %f  \n",vRespawnPos.x,vRespawnPos.y,vRespawnPos.z);
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3PRINTLOG(I3LOG_NOTICE, BattleSlotContext::i()->getNickForSlot( idx));
#endif

	if( bFirstRespawn == true)
		pChara->addCharaStateMask( CHARA_STATE_FIRSTRESPAWN);

	if (idx == BattleSlotContext::i()->getMySlotIdx())
		ExecuteRespawnCheatkey();

	pChara->getMoveContext()->EnablePushingRandomDir();
}

void CGameCharaManager::MoveRespawnPoint()
{
	INT32 idx = BattleSlotContext::i()->getMySlotIdx();

	const GLOBAL_CHARA_INFO * pInfo = getCharaInfo( idx );
	const NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->getNetCharaInfo( idx );
	CGameCharaBase * pChara = getCharaByNetIdx( idx);

	VEC3D vRespawnPos;
	REAL32 theta;

	// 리스폰 위치 설정
	theta = GetRespawnPosition( &vRespawnPos, pInfo->GetTeam(), idx);

	GlobalFunc::PB_TRACE( "Chara Respawn ( %d, %s, respawnIdx %d )", idx, BattleSlotContext::i()->getNickForSlot( idx ), pNetCharaInfo->getRespawnIndex() );

	pChara->setDynamicActiveState( true, false );
	pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );

	pChara->Cmd_Respawn( &vRespawnPos, theta , false);

	// 오브젝트 위에서 리스폰
	_RespawnOnObject( pChara, pNetCharaInfo);

	if (idx == BattleSlotContext::i()->getMySlotIdx())
		ExecuteRespawnCheatkey();
}

void CGameCharaManager::_ReverseSetPhysixValue( i3Skeleton * pSkel, i3Bone * pBone, VEC3D * pVec )
{
	if( pBone->getParentIndex() == -1 )
		return;

	i3PhysixShapeSet * pParentShape = pSkel->getShapeSet( pBone->getParentIndex() );

	if( pParentShape != nullptr )
	{
		pParentShape->setFBLinearVelocity( pVec );

		i3Vector::Scale( pVec, pVec, 0.5f );
		_ReverseSetPhysixValue( pSkel, pSkel->getBone( pBone->getParentIndex() ), pVec );
	}
}

void CGameCharaManager::_ProcessPhysixValueByExplosion( CGameCharaBase * pChara, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon, VEC3D * pIKPos)
{

	INT32 i;
	MATRIX mat;
	VEC3D vAddLin, vLin, vRoll, vRollDir, vR, vTorque;

	REAL32 rAngForce = 0.f;
	REAL32 rRelation = 1.f;

	VEC3D * pHitPos = nullptr;

	PHYSIXINFODATA_WEAPON * pPhysixData = &g_pPhysixInfoDataBase->getInfo()->m_WeaponDeath;

	i3Body * pBody = pChara->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	I3ASSERT( pSkel != nullptr );

	I3ASSERT( pHitDir != nullptr );
	i3Vector::Normalize( &vLin, pHitDir);

	if( BattleServerContext::i()->IsP2PHost() )	{ i3Vector::Scale( &vLin, &vLin, pPhysixData->m_rWeapon_Grenade ); }
	else								{ i3Vector::Scale( &vLin, &vLin, -pPhysixData->m_rWeapon_Grenade ); }

	if( pChara->isLocal() )				{ i3Vector::Add( &vLin, 0.f, pPhysixData->m_rWeapon_Grenade * 2.f, 0.f ); }
	else								{ i3Vector::Add( &vLin, 0.f, pPhysixData->m_rWeapon_Grenade * 1.4f, 0.f ); }

	I3TRACE( "%d, lin : %.4f,\n", hitpart, i3Vector::Length( &vLin) );

	for( i = 0; i < pSkel->getBoneCount(); i++ )
	{
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );

		if( pShapeSet == nullptr )		continue;

		CHARA_HIT_PART part = pChara->getCollisionContext()->GetHitPart( pShapeSet );

		rRelation = 1.f;
		if( part == hitpart )
		{
			i3Vector::Scale( &vAddLin, &vLin, rRelation * 1.7f );
			pShapeSet->setFBLinearVelocity( &vAddLin );
		}
		else if( part == CHARA_HIT_UNKNOWN )
		{
		}
		else
		{
			i3Vector::Scale( &vAddLin, &vLin, rRelation );
			pShapeSet->setFBLinearVelocity( &vAddLin );
		}
		if( part == CHARA_HIT_HEAD )
		{
			pShapeSet->getShapeGlobalPose( &mat );
			pHitPos = i3Matrix::GetPos( &mat );

			I3ASSERT(pIKPos != nullptr);

			i3Vector::Sub( &vR, pIKPos, pHitPos );
			i3Vector::Cross( &vTorque, &vR, pHitDir );
		}
	}

	// 캐릭터를 회전 시킵니다.
	rAngForce = pPhysixData->m_rWeapon_Grenade * 3.f;

	for( i = 0; i < pSkel->getBoneCount(); i++ )
	{
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );

		if( pShapeSet == nullptr )		continue;

		CHARA_HIT_PART part = pChara->getCollisionContext()->GetHitPart( pShapeSet );

		if( (part != hitpart) && (part != CHARA_HIT_ROOT) && (part != CHARA_HIT_UNKNOWN))
		{
			// 맞은 부위에 따른 다른 부위의 로테이션 값
			pShapeSet->getShapeGlobalPose( &mat );

			I3ASSERT(pIKPos != nullptr);
			i3Vector::Sub( &vRoll, i3Matrix::GetPos( &mat ), pIKPos );
			i3Vector::Cross(  &vRollDir, &vRoll, &vTorque );

			i3Vector::Scale( &vRollDir, &vRollDir, rAngForce );
			pShapeSet->addFBLinearVelocity( &vRollDir );
		}
	}
}


void CGameCharaManager::_ProcessPhysixValueByDefault( CGameCharaBase * pChara, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon, VEC3D * pIKPos)
{
	INT32 i;

	REAL32 rForce = 10.f;
	MATRIX mat;
	VEC3D vR, vTorque, vHitForce;

//	PHYSIXINFODATA_WEAPON * pPhysixData = &g_pPhysixInfoDataBase->getInfo()->m_WeaponDeath;

	CWeaponInfo * pWeaponInfo = nullptr;
	if( pWeapon != nullptr )
		pWeaponInfo = pWeapon->getWeaponInfo();

	i3Body * pBody = pChara->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	I3ASSERT( pSkel != nullptr );

#if defined( PHYSIX_CHARA_FORCE)
	{
		REAL32 rSlerp = 100.f;

		switch( hitpart )
		{
		case CHARA_HIT_HEAD :		rSlerp = 15.f;		break;
		case CHARA_HIT_NECK :		rSlerp = 20.f;		break;
		case CHARA_HIT_LCLAVICLE :
		case CHARA_HIT_RCLAVICLE :	rSlerp = 10.f;		break;
		case CHARA_HIT_LUARM :
		case CHARA_HIT_RUARM :		rSlerp = 0.05f;		break;
		case CHARA_HIT_LDARM :
		case CHARA_HIT_RDARM :		rSlerp = 0.003f;		break;
		case CHARA_HIT_LHAND :
		case CHARA_HIT_RHAND :		rSlerp = 0.0005f;	break;
		case CHARA_HIT_LULEG :
		case CHARA_HIT_RULEG :		rSlerp = 100.f;		break;
		case CHARA_HIT_RDLEG :
		case CHARA_HIT_LDLEG :		rSlerp = 10.f;		break;
		case CHARA_HIT_LFOOT :
		case CHARA_HIT_RFOOT :		rSlerp = 0.5f;		break;
		case CHARA_HIT_BODY1 :		rSlerp = 40.f;		break;
		case CHARA_HIT_BODY2 :		rSlerp = 80.f;		break;
		case CHARA_HIT_BODY3 :		rSlerp = 100.f;		break;
		case CHARA_HIT_ROOT :		rSlerp = 170.f;		break;
		default :					rSlerp = 10.f;		break;
		}

		if( pWeapon != nullptr )
			rForce = (REAL32) pWeapon->getAP() * rSlerp + 5000.f;
	}
#else
	{
		REAL32 rSlerp = 1.f;

		switch( hitpart )
		{
		case CHARA_HIT_LUARM :
		case CHARA_HIT_RUARM :
		case CHARA_HIT_LDARM :
		case CHARA_HIT_RDARM :
		case CHARA_HIT_LHAND :
		case CHARA_HIT_RHAND :		rSlerp = 0.9f;		break;
		case CHARA_HIT_LULEG :
		case CHARA_HIT_RULEG :		rSlerp = 0.71f;		break;
		case CHARA_HIT_HEAD :		rSlerp = 1.2f;		break;
		case CHARA_HIT_NECK :
		case CHARA_HIT_RDLEG :
		case CHARA_HIT_LDLEG :		rSlerp = 0.51f;		break;
		case CHARA_HIT_LFOOT :
		case CHARA_HIT_RFOOT :		rSlerp = 0.051f;	break;
		case CHARA_HIT_BODY1 :		rSlerp = 0.67f;		break;
		case CHARA_HIT_BODY2 :		rSlerp = 0.77f;		break;
		case CHARA_HIT_BODY3 :		rSlerp = 0.8f;		break;
		case CHARA_HIT_ROOT :		rSlerp = 0.61f;		break;
		case CHARA_HIT_LCLAVICLE :
		case CHARA_HIT_RCLAVICLE :	rSlerp = 0.8f;		break;
		default :					rSlerp = 0.8f;		break;
		}

		if( pWeapon != nullptr)
			rForce = (REAL32) pWeapon->getAP() * rSlerp * 0.3f + pWeapon->getAP() * 0.1f + 30.f;
	}
#endif

	if( pWeapon != nullptr && pWeaponInfo != nullptr )
	{
		rForce *= pWeapon->GetPhysixForce( pWeaponInfo->GetTypeClass());

		rForce *= pWeaponInfo->GetPhysixForceRate();
	}

	REAL32	rLinForce = rForce * g_pPhysixInfoDataBase->getInfo()->m_rCharaLinSlop;
	REAL32	rAngForce = 0.f;

	rAngForce = rForce * g_pPhysixInfoDataBase->getInfo()->m_rCharaAngSlop;

	I3TRACE( "%d, lin : %.4f, ang : %.4f\n", hitpart, rLinForce, rAngForce);

	for( i = 0; i < pSkel->getBoneCount(); i++ )
	{
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );

		if( pShapeSet == nullptr )		continue;
		if( pChara->getCollisionContext()->GetHitPart( pShapeSet ) != hitpart )	continue;

		if( hitpart != CHARA_HIT_ROOT )
		{
			pShapeSet->getShapeGlobalPose( &mat );
			I3ASSERT(pIKPos != nullptr);
			i3Vector::Sub( &vR, pIKPos, i3Matrix::GetPos( &mat ) );
			i3Vector::Cross( &vTorque, &vR, pHitDir );
		}
		i3Vector::Scale( &vHitForce, pHitDir, rLinForce);

		pShapeSet->setFBLinearVelocity( &vHitForce );

		if( hitpart == CHARA_HIT_ROOT)	continue;

		i3Vector::Scale( &vHitForce, &vHitForce, 0.5f );
		_ReverseSetPhysixValue( pSkel, pSkel->getBone( i ), &vHitForce );
		break;
	}

	if( hitpart != CHARA_HIT_ROOT )
	{
		// 캐릭터를 회전 시킵니다.
		VEC3D vRoll, vRollDir;
		for( i = 0; i < pSkel->getBoneCount(); i++ )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );

			if( pShapeSet == nullptr )		continue;

			CHARA_HIT_PART part = pChara->getCollisionContext()->GetHitPart( pShapeSet );
			if( (part != hitpart) && (part != CHARA_HIT_ROOT) && (part != CHARA_HIT_UNKNOWN))
			{
				// 맞은 부위에 따른 다른 부위의 로테이션 값
				pShapeSet->getShapeGlobalPose( &mat );
				I3ASSERT(pIKPos != nullptr);
				i3Vector::Sub( &vRoll, i3Matrix::GetPos( &mat ), pIKPos );
				i3Vector::Cross(  &vRollDir, &vRoll, &vTorque );
				i3Vector::Scale( &vRollDir, &vRollDir, rAngForce);
				pShapeSet->addFBLinearVelocity( &vRollDir );
			}
		}
	}
}

REAL32 CGameCharaManager::GetRespawnPosition( VEC3D * pOut, CHARACTER_TEAM_TYPE team, INT32 idxChara, 
											 INT32 idxObj /*= -1*/, bool bIsAI /*= false*/ )
{
	UINT32	nRand;
	INT32 nRandConst = m_pRespawnInfo->GetRespawnCount( team);

	if( nRandConst <= 0)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%d 팀의 Respawn Position이 없습니다.\n 1:Red 2:Blue 3:R_NPC 4:B_NPC", team);
		return 0.f;
	}

	I3ASSERT( idxChara > -1);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || 
		MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION) )
	{	// 랜덤 기능 추가 2007.11.16 by dfly79
		INT32 roomIdx = MyRoomInfoContext::i()->getMyRoomIndex();

		INT32 roundCount = (INT32)(ScoreContext::i()->getCurrentRound( BattleServerContext::i()->IsUseDedicationMode()));

		// 카운트 다운 ack과 리스폰 ack을 받았을때 이 루틴을 오게 되는데, 카운트 다운 ack을 받았을때는 roundCount가 타이밍에 0일수도 1일수도 있습니다.
		// 0인 경우에 카운트다운 전과 후에 순간이동 현상이 생기므로 일단은 라운드가 0이라고 설정되어 있을경우 1로 강제로 맞춰줍니다.
		if (roundCount == 0) roundCount = 1;

		nRand		= ( roomIdx + roundCount + idxChara/2)	% nRandConst;
	}
	else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && team != 1 )
	{
		nRand = (i3Math::Rand()%(nRandConst*2))%nRandConst;
	}
	else
	{
		nRand	= (idxChara/2) % nRandConst;
	}

	i3RespawnObj* pObj = nullptr;
	bool bRandPosition	= (idxObj == -1);

	if( bRandPosition)
	{
		pObj = m_pRespawnInfo->GetRespawnObject( team, nRand);
	}
	else
	{
		i3Stage* pstage	= g_pFramework->GetCurrentStage();
		if( pstage)
		{
			i3::vector<i3RespawnObj*>& pobj_list = pstage->getRespawnObjList();
			INT32 size = static_cast<INT32>(pobj_list.size());
			
			I3_BOUNDCHK( idxObj, size);
			if( idxObj >= 0 && idxObj < size)
			{
				pObj = pobj_list[idxObj];
			}
		}
	}

	I3ASSERT_RETURN( pObj != nullptr, 0.f);
	I3ASSERT( pOut != nullptr);
	REAL32 fTemp = 0.2f;

	if( bIsAI)
	{
		const NET_CHARA_INFO*	pnet_chara_info	= BattleSlotContext::i()->getNetCharaInfo( idxChara, CHARA::BUFFER_NEW_AI);
		i3Vector::Copy( pOut, const_cast<VEC3D*>(pnet_chara_info->getRespawnPos()) );
	}
	else
	{
		i3Vector::Copy( pOut, pObj->GetPos());
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) )	// revision 29013
		fTemp = 0.6f;

	// 월드에 붙여서 리스폰시 캐릭터가 월드에 뚫리는 문제가 발생합니다.
	// 월드에서 20센티 띄워서 리스폰 시킵니다.
	setY( pOut, getY(pOut) + fTemp);

	return pObj->GetTheta();
}

void CGameCharaManager::SwapMyCharaToLoaded( INT32 idx)
{
	I3_REF_CHANGE( m_pTempChara[idx], m_pMyChara);

	GLOBAL_CHARA_INFO * pCharaInfo = setCharaInfo( idx);
	I3ASSERT( pCharaInfo != nullptr);
	pCharaInfo->Reset();

	g_pCharaManager->MakeCharaInfo( pCharaInfo, idx,
		(CHARA_RES_ID) BattleSlotContext::i()->setNetCharaInfo(idx)->getCharaParts()->GetPartsResID(CHAR_EQUIPMENT_PARTS_CHARA), CHARA::TYPE_PLAYER);

	BattleSlotContext::i()->setNetCharaInfo(idx)->getCharaParts()->CopyTo( pCharaInfo->SetParts());

	_AddChara( idx, pCharaInfo);
}

void CGameCharaManager::_RemoveChara( CGameCharaBase * pChara)
{
	if( RemoveCharacter_Impl( pChara))
	{
		i3::vu::remove(m_CharaList, pChara );
		pChara->ReturnInstance();
	}
}

bool CGameCharaManager::RemoveCharacter_Impl( CGameCharaBase * pChara)
{
	if( pChara == nullptr || pChara->isReturnedInstance()) // 이미 반환된 경우
		return false;

	I3ASSERT( i3::kind_of<CGameCharaBase*>(pChara));

	if(UIBattleFrame::i() != nullptr)
	{
		UIHudManager * pHUDManager = static_cast<UIHudManager*>(UIBattleFrame::i()->GetHUD( UIHUD_MANAGER));

		if( pHUDManager != nullptr)
		{
			if( pHUDManager->GetHud( HUD::PLAYER_STATE) != nullptr &&
				pHUDManager->GetHud( HUD::PLAYER_STATE)->GetMyChara() == pChara)
			{
				pHUDManager->GetHud( HUD::PLAYER_STATE)->SetMyChara( nullptr);
			}
		}
	}
	// vv3 - check
	const NET_CHARA_INFO* pNetCharaInfo = BattleSlotContext::i()->getNetCharaInfo( pChara->getCharaNetIndex(), pChara->getBufferType());
	const ITEM_INFO* pItemInfo = pNetCharaInfo->getCommonParts()->getParts(CHAR_EQUIPMENT_COMMON_DINO);
	if( ReturnToReserve( pItemInfo->m_TItemID, pChara ) )
		return false;

	if( m_pStage != nullptr )
	{
		// ref--
		if( pChara->getParent() != nullptr)
			pChara->getParent()->RemoveChild( pChara);
	}

	pChara->DetachScene();

	// 캐릭터 삭제시 스테이지의 Object 관계를 끊어야 한다.
	BattleObjContext::i()->ResetEventHandler( pChara);

	FreePhysXShape( pChara);

	// 월드에 떨어진 무기를 찾아 오너 상태를 nullptr로 한다.
	for( INT32 i = 0; i < g_pWeaponManager->GetDropListCount(); i++)
	{
		WeaponBase * pWeapon = g_pWeaponManager->GetDropWeapon( i);
		I3ASSERT( pWeapon != nullptr);
		if( pWeapon->getOwner() == pChara || pWeapon->getCreater() == pChara )
		{
			pWeapon->SetOwner( nullptr);
			pWeapon->setCreater( nullptr);
		}
	}

	// 유저가 사용중인 무기들의 create 상태를 nullptr로 한다.
	for( UINT32 i = 0; i < g_pWeaponManager->GetCharaWeaponListCount(); i++)
	{
		WeaponBase * pWeapon = g_pWeaponManager->GetCharaWeapon( i);
		I3ASSERT( pWeapon != nullptr);
		if( pWeapon->getOwner() == pChara)
		{
			pWeapon->SetOwner( nullptr);
			pWeapon->setCreater( nullptr);
		}
	}

	if (CGameCameraManager::i())
		CGameCameraManager::i()->RemoveTargetChara(pChara);

	return true;
}

void CGameCharaManager::_SetPhysixValue( CGameCharaBase * pChara, CHARA_DEATH_TYPE deathType, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon )
{
	MATRIX	IKMat;
	VEC3D	vLin;
	INT32	i;
	i3PhysixShapeSet * pShapeSet;
	VEC3D * pIKPos = nullptr;

	VEC3D vZero;
	
	i3Body * pBody = pChara->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	I3ASSERT( pSkel != nullptr );

	I3ASSERT( pChara != nullptr );
	pChara->getMoveContext()->EnableController( false );	// 컨트롤러의 컬리젼을 끕니다.

	for( i = 0; i < pSkel->getBoneCount(); i++ )
	{
		pShapeSet = pSkel->getShapeSet( i );

		if( pShapeSet == nullptr )		continue;

		pShapeSet->setFBLinearVelocity( &vZero );
		pShapeSet->setFBAngularVelocity( &vZero );

		if( pChara->getCollisionContext()->GetHitPart( pShapeSet ) == CHARA_HIT_ROOT )
		{
			pShapeSet->getShapeGlobalPose( &IKMat );
			pIKPos = i3Matrix::GetPos( &IKMat );
		}
	}

	switch( deathType )
	{
	case CHARA_DEATH_O :
	case CHARA_DEATH_B :	_ProcessPhysixValueByExplosion( pChara, hitpart, pHitDir, pWeapon, pIKPos);		break;

	case CHARA_DEATH_POISON:
	case CHARA_DEATH_F :
		for( i = 0; i < pSkel->getBoneCount(); i++ )
		{
			pShapeSet = pSkel->getShapeSet( i );

			if( pShapeSet == nullptr )		continue;
			if( pChara->getCollisionContext()->GetHitPart( pShapeSet ) == CHARA_HIT_HEAD )
			{
				i3Vector::Scale( &vLin, pChara->getMoveContext()->getOldVelocity(), 1.f/g_pViewer->getDeltaTime() * 0.3f);
				i3Vector::SetY( &vLin, 0.f );
				pShapeSet->setFBLinearVelocity( &vLin );
			}
		}

		break;

	default :	_ProcessPhysixValueByDefault( pChara, hitpart, pHitDir, pWeapon, pIKPos);		break;
	}	// switch( deathType)

	if(pChara->IsRaptorBase()
#ifdef DOMI_DINO
		|| pChara->IsDomiRaptorBoneBase()
#endif
		)
	{
		i3Vector::Copy( pChara->getCollisionContext()->getBonesForceDir( CHARA_CONTACT_BONE_ROOT), pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_ROOT) )->getFBLinearVelocity() );
		i3Vector::Copy( pChara->getCollisionContext()->getBonesForceDir( CHARA_CONTACT_BONE_SPINE3), pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_SPINE1) )->getFBLinearVelocity() );
	}
	else if(pChara->IsRex() 
#ifdef DOMI_DINO
		|| pChara->IsDomiMutantRex()
#endif
		)
	{
		//Rex는 death를 simulate하지 않...
	}
	else
	{
		if( pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_ROOT) ) != nullptr)
		{
			i3Vector::Copy( pChara->getCollisionContext()->getBonesForceDir( CHARA_CONTACT_BONE_ROOT), pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_ROOT) )->getFBLinearVelocity() );
		}
		
		if( pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_SPINE3) ) != nullptr)
		{
			i3Vector::Copy( pChara->getCollisionContext()->getBonesForceDir( CHARA_CONTACT_BONE_SPINE3), pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_SPINE3) )->getFBLinearVelocity() );
		}
	}


	// Chara의 죽음, simulation 시작
	pChara->SetDynamicUpdateEnable( true );
	pChara->setDynamicActiveState( true, false );

	pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_DYNAMIC, true);
	i3Scene::ModifyDynamicState( pChara->getCollisionContext()->getRoughBound(), I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE, 0);

	// 이팩트용 컨택트 박스를 Dynamic으로 변경해야 합니다.
	pChara->getCollisionContext()->SetContactShapeSetEnable( true);
}

void CGameCharaManager::_SetDeathAnimation( CGameCharaBase * pChara, CHARA_DEATH_TYPE deathType, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon)
{
	I3ASSERT( pChara != nullptr );
	pChara->getMoveContext()->EnableController( false );	// 컨트롤러의 컬리젼을 끕니다.

	// 이팩트용 컨택트 박스를 Dynamic으로 변경해야 합니다.
	pChara->getCollisionContext()->SetContactShapeSetEnable( true);

	// 공중에서 죽었다면..
	if( pChara->getCollisionContext()->getFloorHitResult()->m_T > 0.5f)
	{
		pChara->getUpdateContext()->setDeathOriginalHeight( getY( pChara->GetPos()));

		REAL32 rOffset = -(pChara->getCollisionContext()->getFloorHitResult()->m_T - 0.5f);
		addY( pChara->GetPos(), rOffset );

		pChara->getUpdateContext()->setDeathBlendHeight( getY( pChara->GetPos() ) );
	}
	else
	{
		pChara->getUpdateContext()->setDeathOriginalHeight( getY( pChara->GetPos()));
		pChara->getUpdateContext()->setDeathBlendHeight( getY( pChara->GetPos() ) );
	}

	// 일정 크기의 바운드 체크로 월드컬리젼에 대한 밀어내기를 합니다.
}

void CGameCharaManager::DeathDino( CGameCharaBase* pChara, INT32 nShooter, VEC3D * pHitDir, CHARA_DEATH_TYPE deathType, CHARA_HIT_PART hitpart )
{
	if (pChara == nullptr) return;

	CGameCharaBase * pShooter = nullptr;
	WeaponBase * pShooterWeapon = nullptr;

	INT32 idx = pChara->getCharaNetIndex();
	//Network 입니다. 추후 이동합니다.  성백
	if( pChara->GetType() == CHARA::TYPE_NA )
	{
		I3TRACE( "잘 못된 데스입니다. %d\n", idx );
		return;
	}

	//CGameCharaBase * pChara = m_pChara[ idx ];
	

	if( nShooter != -1 && nShooter < SLOT_MAX_COUNT )
	{
		pShooter = getCharaByNetIdx( nShooter);								///현제 shooter가 AI인지 USER인지 모른다..

		if( pShooter != nullptr && pShooter->IsDino() == false)
		{
			pShooterWeapon = pShooter->GetCurrentCharaWeapon();
		}
	}

	I3TRACE( "Dino Death ( %d, %s )\n", idx, BattleSlotContext::i()->getNickForSlot( idx));

	if( !pChara->isCharaStateMask( CHARA_STATE_DEATH))
	{
		pChara->Cmd_Death( nShooter, deathType, pHitDir);		//	Action Death

		setCharaTask( idx, CHARA_TASK_DEATH );
	}

	//헤드샷으로 죽은경우 헤드샷 사운드 출력
	{
		INT32 nidx = 0;
		if( pChara->IsRaptor())
		{
			nidx = 1;
		}
		else if (pChara->IsSting())
		{
			nidx = 2;
		}
		else if (pChara->IsTank())
		{
			nidx = 3;
		}
		else if (pChara->IsAcid())
		{
			nidx = 4;
		}
		else if (pChara->IsCCRaptor() || pChara->IsRaptorMercury())
		{
			nidx = 1;
		}
		else if (pChara->IsCCSting() || pChara->IsStingMars())
		{
			nidx = 2;
		}
		else if (pChara->IsCCAcid() || pChara->IsAcidVulcan())
		{
			nidx = 4;
		}

		g_pSndMng->StartDinoActionSound( nidx, GTBDA_ON_DEATH_HOWL, pChara->GetPos(), pChara->is1PV());
	}

	// Physix On
	if( pChara->IsOnObject() == false)
	{
		/*if( g_pEnvSet->m_bEnablePhysX && (pChara->IsRex() == false))
		{
			_SetPhysixValue( pChara, deathType, hitpart, pHitDir, pShooterWeapon );
		}
		else*/
		{
			_SetDeathAnimation( pChara, deathType, hitpart, pHitDir, pShooterWeapon );
		}
	}
}

void CGameCharaManager::DeathChara( CGameCharaBase* pChara, INT32 nShooter, VEC3D * pHitDir, CHARA_DEATH_TYPE deathType, CHARA_HIT_PART hitpart  )
{ 
	if( pChara->IsDino() )
	{
		DeathDino( pChara, nShooter, pHitDir, deathType, hitpart);
	}
	else
	{
		DeathHuman( pChara, nShooter, pHitDir, deathType, hitpart);
	}
}
void CGameCharaManager::DeathHuman( CGameCharaBase* pChara, INT32 nShooter, VEC3D * pHitDir, CHARA_DEATH_TYPE deathType, CHARA_HIT_PART hitpart )
{
	//Network 입니다. 추후 이동합니다.  성백

	if (pChara == nullptr) return;

	if( pChara->GetType() == CHARA::TYPE_NA )
	{
		I3TRACE( "잘 못된 데스입니다. %d\n", pChara->getCharaNetIndex() );
		return;
	}
	
	int idx = pChara->getCharaNetIndex();

	WeaponBase * pHiterWeapon = pChara->GetCurrentCharaWeapon();

	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && pChara->isLocal() && pHiterWeapon != nullptr)
	{
		pChara->getWeaponContext()->NoUseWeapon(pHiterWeapon);
		
		if ( HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsVisible())
		{
			HUD::instance()->GetHud(HUD::MULTIWEAPON)->SetEnable(false);
		}
	}

	CGameCharaBase * pShooter = nullptr;
	WeaponBase * pShooterWeapon = nullptr;

	if( nShooter != -1 && nShooter < SLOT_MAX_COUNT )
	{
		pShooter = getCharaByNetIdx( nShooter);
		if( pShooter != nullptr)
		{
			if(pShooter->IsDino())
			{
				pShooterWeapon = nullptr;
			}
			else
			{
				pShooterWeapon = pShooter->GetCurrentCharaWeapon();
			}
		}
	}

	bool bHost = BattleServerContext::i()->IsP2PHost();
	if( BattleServerContext::i()->IsUseDedicationMode() )
	{
		if( pChara->isLocal())
			bHost = true;
	}

	I3TRACE( "Chara Death ( %d, %s )\n", idx, BattleSlotContext::i()->getNickForSlot( idx));

	if( !pChara->isCharaStateMask( CHARA_STATE_DEATH))
	{
		pChara->Cmd_Death( nShooter, deathType, pHitDir);		//	Action Death

		setCharaTask( idx, CHARA_TASK_DEATH);

		// Weapon attach to world
		if( (pChara->IsOnObject() == false) && ( pHiterWeapon != nullptr) )
		{
			if( bHost )
			{
				//bool bCheckDropWeapon = false;
				WEAPON_SLOT_TYPE nWeaponSlot = pChara->getWeaponContext()->getCurrentSlotNum();
				I3_BOUNDCHK( nWeaponSlot, WEAPON_SLOT_COUNT);
				if( nWeaponSlot < WEAPON_SLOT_MELEE )
				{
					// 무기를 버리고 패킷을 세팅합니다.
					GAMEEVENT::Write_Chara_DropWeapon( pChara, nWeaponSlot, pHiterWeapon->getLoadedBulletCount(), pHiterWeapon->getLoadedBulletDualCount(), pHiterWeapon->getTotalBulletCount());
				}

			}
		}
	}

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) )
	{
		CGameMissionManager * pMissionManager = CGameMissionManager::i();

		if( pMissionManager->WhoInstall( idx ) )
		{
			pChara->Cmd_Install( false);

			pMissionManager->Install( idx, pMissionManager->WhoInstallingArea( idx ), false );
		}

		if( pMissionManager->WhoUninstall( idx ) )
		{
			pChara->Cmd_Uninstall( false);

			pMissionManager->Uninstall( idx, false );
		}
	}

	// 캐릭터 마크 설정
	pChara->getMaterialContext()->SetTeamMarkEnable( false);

	//헤드샷으로 죽은경우 헤드샷 사운드 출력
	if( CHARA_DEATH_H == deathType)
	{
		if( pShooterWeapon != nullptr && (pShooterWeapon->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_KNIFE || pShooterWeapon->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_KNUCKLE))
		{
			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_KNIFEHIT_HEAD, 0, pChara->GetPos(), pChara->is1PV());
		}
		else
		{
			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_HIT_HEAD, i3Math::Rand() %2, pChara->GetPos(), pChara->is1PV());
		}
	}

	if( (UserInfoContext::i()->IsGM_Observer() == true && pChara->isLocal()) == false)
	{
		// revision 29616
		if( BattleSlotContext::i()->IsGMUserForClient( idx) && BattleSlotContext::i()->IsGMObserverForClient( idx))
		{
			I3TRACE( " [CGameCharaManager::DeathHuman]GM Death Sound is not play..\n");
		}
		else if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && pChara->isAIToClient())
			g_pSndMng->StartCharacterActionSnd( GTBCA_MADNESS_DEATH, pChara->getDBInfo()->GetCharaSexTypeDISPLAY(), pChara->GetPos(), pChara->is1PV());
		else	
			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_DEATH_CHARA, pChara->getDBInfo()->GetCharaSexTypeDISPLAY(), pChara->GetPos(), pChara->is1PV());
	}

	// Physix On
	if( pChara->IsOnObject() == false && hitpart != CHARA_HIT_DEATH_TRAMPLE)
	{
		if( g_pEnvSet->m_bEnablePhysX)		_SetPhysixValue( pChara, deathType, hitpart, pHitDir, pShooterWeapon );
		else								_SetDeathAnimation( pChara, deathType, hitpart, pHitDir, pShooterWeapon );

		if( pChara->isAttachedProtectParts() )
		{
			// Equip의 상태가 붙어있는 상태라면 Collision과 dynamic을 끕니다.
			if( (deathType == CHARA_DEATH_B) || (deathType == CHARA_DEATH_O) || (hitpart == CHARA_HIT_HEAD) )
			{
				// HeadShot or BoomShot
				//	머리에 장비된 헬멧을 월드에 붙인다.
				pChara->getEquipContext()->DropToWorld( pChara->getProtectPartsType() , pHitDir);
			}
		}

		pChara->Cmd_SetPhysics();
	}

	WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
	if( (pChara->getWeaponContext()->getCurrentSlotNum() >= 2) &&
		(pWeapon != nullptr) )		// <-- 임시 땜빵. C4를 설치하자마자 죽게되면 나올수 있습니다.
	{
		pWeapon->DetachNode();
		pWeapon->SetEnable( false);
	}
}

void CGameCharaManager::VisibleCharaAndWarpSavedPos( INT32 idx )
{
	CGameCharaBase * pChara = getCharaByNetIdx( idx);
	if( pChara == nullptr ) return;

	pChara->LoadSavedPos();
	if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == false)
		pChara->SetVisible( true );
}


void CGameCharaManager::InvisibleCharaAndWarp( INT32 idx )
{
	CGameCharaBase * pChara = getCharaByNetIdx( idx);
	if( pChara == nullptr ) return;

	pChara->SaveNowPos();

	VEC3D pos;
	i3Vector::Set( &pos, 0.f, g_pConfigEx->GetPhysix().HeightChara, 0.f );

	pChara->forceSetPos( &pos);
	pChara->SetVisible( false );

	GAMEINFO_CHARACTER * pCharaInfo = BattleSlotContext::i()->setGameInfo_Chara( idx );
	pCharaInfo->_tNetData._tPosRotData.setPos( &pos );
	pChara->getMoveContext()->setRecvPosition( &pos);
	pChara->getMoveContext()->setPostPosition( &pos);
}

void CGameCharaManager::DeathPosition( INT32 idx, bool bRespawn )
{
	CGameCharaBase * pChara = getCharaByNetIdx( idx);
	if( pChara == nullptr) return;

	VEC3D pos;
	i3Vector::Set( &pos, s_rStartPos[ idx ][ 0 ], g_pConfigEx->GetPhysix().HeightChara, s_rStartPos[ idx ][ 2 ] );

	pChara->forceSetPos( &pos);
	pChara->SetVisible( false );

	if( bRespawn )
	{
		MATRIX mtx;
		VEC3D vec;

		i3SceneObject * pSceneObj = pChara->getSceneObject();
		i3Body * pBody = pSceneObj->GetBody();
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != nullptr )
			{
				pShapeSet->getShapeGlobalPose( &mtx );
				i3Vector::Sub( &vec, i3Matrix::GetPos( &mtx ), i3Matrix::GetPos( pChara->getBoneContext()->getBoneMatrix(BONE_IK) ) );
				i3Vector::Add( &vec, &vec, &pos );
				i3Vector::Add( &vec, (REAL32)i, 0.f, 0.f );

				i3Matrix::SetPos( &mtx, &vec );

				pShapeSet->setShapeGlobalPose( &mtx );
			}
		}

		pChara->setDynamicActiveState( true, false );
		pChara->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
	}

	GAMEINFO_CHARACTER * pCharaInfo = BattleSlotContext::i()->setGameInfo_Chara( idx );
	pCharaInfo->_tNetData._tPosRotData.setPos( &pos );
	pChara->getMoveContext()->setRecvPosition( &pos);
	pChara->getMoveContext()->setPostPosition( &pos);

	pChara->addCharaStateMask( CHARA_STATE_DEATHPOSITION);
}

void CGameCharaManager::UpdateReserveDeath( REAL32 fDeltatime )
{
	RESERVEDEATHCHARA * pReserveDeath;

	for( size_t i = 0; i < m_ReserveDeathList.size(); i++ )
	{
		bool bDelete = false;
		pReserveDeath	= m_ReserveDeathList[i];
		I3ASSERT( pReserveDeath != nullptr );

		// 캐릭터가 나간 경우입니다. 또는 아직 로딩이 안된 경우.
		if( pReserveDeath->_pChara == nullptr )	bDelete = true;
		else
		{
			if( pReserveDeath->_pChara->isCharaStateMask( CHARA_STATE_DEATH))	bDelete = true;
			else
			{
				pReserveDeath->_Time += fDeltatime;
				if( pReserveDeath->_Time > TCPDEATHTIME )
				{
					if( pReserveDeath->_pChara->getCharaNetIndex() != -1 )
					{
						DeathChara( pReserveDeath->_pChara, pReserveDeath->_nShooterIdx, pReserveDeath->_vDir, pReserveDeath->_DeathType );
						UserContext::i()->Death[pReserveDeath->_pChara->getCharaNetIndex()] = true;
					}

					bDelete = true;
				}
			}
		}

		if( bDelete)
		{
			pReserveDeath->_pChara		= nullptr;
			pReserveDeath->_DeathType	= CHARA_DEATH_UN;
			pReserveDeath->_Time		= 0.f;
			pReserveDeath->_vDir		= nullptr;

			i3::vu::erase_index(m_ReserveDeathList, i);
			i--;
		}
	}
}

void CGameCharaManager::ReserveDeath( INT32 idx, INT32 nShooter, const VEC3D * pHitDir, CHARA_DEATH_TYPE deathType, CHARA::BUFFER_TYPE buffer_type)
{
	CGameCharaBase * pChara = getCharaByNetIdx( idx, buffer_type == CHARA::BUFFER_NEW_AI);
	if( pChara == nullptr )return;

	#ifdef DOMI_USER_REVIVAL
	//다른 유저 Death 시 update
	if( pChara->getCharaNetIndex() != BattleSlotContext::i()->getMySlotIdx())
	{
		RevivalController::i()->Show();
	}
	#endif

	if( pChara->isCharaStateMask( CHARA_STATE_DEATH))return;

	m_ReserveDeathSlot[ idx ]._nShooterIdx	= nShooter;
	m_ReserveDeathSlot[ idx ]._pChara		= pChara;
	m_ReserveDeathSlot[ idx ]._vDir			= const_cast<VEC3D*>(pHitDir);
	m_ReserveDeathSlot[ idx ]._DeathType	= deathType;
	m_ReserveDeathSlot[ idx ]._Time			= 0.f;
	m_ReserveDeathList.push_back( &m_ReserveDeathSlot[ idx ] );
}


void CGameCharaManager::EnterStage( i3Stage* pStage )
{
	m_pStage = pStage;

	if( i3::same_of<CStageBattle*>(pStage))
	{
		//Create RespawnPoint
		i3::vector<i3RespawnObj*>& List = pStage->getRespawnObjList();

		m_pRespawnInfo = CGameRespawn::new_object();
		m_pRespawnInfo->Create( List );

		m_ReserveDeathList.clear();

		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			GLOBAL_CHARA_INFO * pInfo = setCharaInfo( i);
			pInfo->SetAIToClient( false);
		}

		for( INT32 j = 0; j < SLOT_MAX_COUNT; j++)
		{
			__PreCreateCharaShapes( (CHARA_RES_ID) CHARA_RES_ID_ACID_POLE, j);
			__PreCreateCharaShapes( (CHARA_RES_ID) CHARA_RES_ID_SWAT_FEMALE, j);
		}
	}
}

void CGameCharaManager::LeaveStage( void )
{
	// 로딩중인 Thread들을 중지
	if( m_pAvatarManager != nullptr)
	{
		m_pAvatarManager->LeaveStage();
	}

	// 캐릭터 로드중일수 있다..
	// 하지만 PhysX를 썼다면 반환시키자
	{
		for( UINT32 i = 0; i < m_vPhysxShapeList_Male.size(); i++)
		{
			tagPRELOADEDPHYSICSHAPE *  pFindInfo = m_vPhysxShapeList_Male.at( i);
			for( INT32 j = 0; j < pFindInfo->_shapeCount; j++)
				if( pFindInfo->_pShapes[ j] != nullptr)
					pFindInfo->_pShapes[ j]->setEventReceiver( nullptr);

			pFindInfo->_pOwner = nullptr;
			pFindInfo->_state = eAPS_NONE;
		}

		for( UINT32 i = 0; i < m_vPhysxShapeList_Female.size(); i++)
		{
			tagPRELOADEDPHYSICSHAPE *  pFindInfo = m_vPhysxShapeList_Female.at( i);
			for( INT32 j = 0; j < pFindInfo->_shapeCount; j++)
				if( pFindInfo->_pShapes[ j] != nullptr)
					pFindInfo->_pShapes[ j]->setEventReceiver( nullptr);
			pFindInfo->_pOwner = nullptr;
			pFindInfo->_state = eAPS_NONE;
		}

		__ReleasePhysXShape();
	}

	I3_SAFE_RELEASE( m_pMyChara);
	SetLobbyAvatar( nullptr);

	ReleaseRespawnPoint();	//리스폰 삭제
	RemoveAllChara();		//난입한 캐릭터 삭제

	m_pStage	  = nullptr;
	m_pCharaLayer = nullptr;
	m_ReserveDeathList.clear();
	for( UINT32 i = 0; i < m_DinoRes.size(); i++)
	{
		m_DinoRes[i] = nullptr;
	}

	// 장비 리소스가 해제된다.
	// 등록된 것을 삭제
	for( INT32 part = 0; part < EQUIP::ePART_COUNT; part++)
	{
		for( UINT32 num = 0 ; num < m_pEquipRes[part].size() ; ++num)
		{
			m_pEquipRes[part][num]->ReleaseGameRes();
		}
	}

	while( !m_EquipLoadQueue.empty())
	{
		sBGEquipLoadInfo * pInfo = m_EquipLoadQueue.front();
		if( pInfo->_state == eBGLOAD_STATE_LOADING)
		{
			pInfo->_state = eBGLOAD_STATE_FLUSH;
			m_EquipLoadQueue.pop();
			m_EquipLoadQueue.push( pInfo);
		}
		else if( pInfo->_state != eBGLOAD_STATE_FLUSH)
		{
			m_pBGLoadInfoPool->Free( pInfo);
			m_EquipLoadQueue.pop();
		}
		else
			break;
	}

	m_pIOManager->TrashRes( false);

	i3Framework * pFramework = i3Framework::getCurrentFramework();
	pFramework->getSgContext()->GetXrayScene()->SetXrayState(false);
}

#ifdef DOMI_NEW_FLOW
void CGameCharaManager::EnterStageRoundJump(i3Stage* pStage)
{
	// 새로운 리스폰 포인트
	g_pCharaManager->m_pRespawnInfo = CGameRespawn::new_object();
	i3::vector<i3RespawnObj*>& list = pStage->getRespawnObjList();
	g_pCharaManager->m_pRespawnInfo->Create(list);

	// 첫 라운드 생성했던 캐릭터를 새로운 스테이지에 붙임.
	for (size_t i = 0; i < m_CharaList.size(); i++)
	{
		if (g_pCharaManager->m_pCharaLayer)
			g_pCharaManager->m_pCharaLayer->AddChild((i3GameNode*)m_CharaList[i]);
		else
			pStage->AddChild((i3GameNode*)m_CharaList[i]);

		m_CharaList[i]->AttachScene();
	}

	g_pCharaManager->m_pStage = pStage;
}

void CGameCharaManager::LeaveStageRoundJump()
{
	// 캐릭터를 삭제하지 않기 위해 노드에서 떼어낸다.
	for (size_t i = 0; i < m_CharaList.size(); i++)
	{
		if (g_pCharaManager->m_pCharaLayer)
			g_pCharaManager->m_pCharaLayer->RemoveChild((i3GameNode*)m_CharaList[i]);
		else
			g_pCharaManager->m_pStage->RemoveChild((i3GameNode*)m_CharaList[i]);

		m_CharaList[i]->DetachScene();
	}

	// 리스폰 포인트, death list 삭제(사이즈가 작기 때문에)
	g_pCharaManager->ReleaseRespawnPoint();
	g_pCharaManager->m_ReserveDeathList.clear();
}
#endif

void CGameCharaManager::SetCharaLayer(i3GameNode* pNode)
{
	m_pCharaLayer = pNode;
}

CGameCharaBase * CGameCharaManager::CreateViewChara(VIEW_CHARA_INFO * pInfo)
{
	// 해당 함수는 UI에서만 쓰이는 함수입니다.
	// 따라서 사용하는 정보를 정리 합니다.

	I3ASSERT(pInfo != nullptr);

	UI_CHARA_INFO  * pUICharaInfo = nullptr;
	INT32 netIdx = pInfo->_charaInfo.GetNetIdx();

	// vv3 - check
	T_ItemID item_id = pInfo->_charaInfo.GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
 	const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( item_id);
 	if( pCharaDBInfo == nullptr)
 		return nullptr;

 	pInfo->_charaInfo.SetResType((CHARA_RES_ID) pCharaDBInfo->GetResID());

	if( netIdx != MAX(0, BattleSlotContext::i()->GetMySlotIdxEx()) )
		pUICharaInfo = CCommunityContext::i()->SetAnotherUserInfo( netIdx );
	else
		pUICharaInfo = UserInfoContext::i()->SetMyUICharaInfo();

	//	무기 리스트 설정
	for(INT32 i=0; i<WEAPON_SLOT_COUNT; i++)
	{
		if( pInfo->_weapon[i] != 0 && pUICharaInfo->getWeaponItemID( (WEAPON_SLOT_TYPE)i) != pInfo->_weapon[i])
		{
			pUICharaInfo->setWeaponItemID( (WEAPON_SLOT_TYPE)i, pInfo->_weapon[i]);
		}
	}

	return nullptr;
}

#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
#include "UnitTest/GameUnitTest.h"
#endif

void CGameCharaManager::MakeCharaTeamInfo( GLOBAL_CHARA_INFO * pCharaInfo, INT32 idx, CHARA::TYPE charaType)
{
	if( charaType != CHARA::TYPE_PROFILE)
	{
		if (pCharaInfo->GetTeam() == CHARACTER_TEAM_NONE)
		{
			if ((idx % 2) == 0)	pCharaInfo->SetTeam(CHARACTER_TEAM_RED);
			else				pCharaInfo->SetTeam(CHARACTER_TEAM_BLUE);
		}
	}

	if (pCharaInfo->GetTeam() != CHARACTER_TEAM_RED && pCharaInfo->GetTeam() != CHARACTER_TEAM_BLUE)
		pCharaInfo->SetTeam(CHARACTER_TEAM_RED);
		
	ISTEST_S
	{
		//pCharaInfo->SetTeam( CHARACTER_TEAM_BLUE);
	}
	ISTEST_E
}

void CGameCharaManager::MakeCharaInfo( GLOBAL_CHARA_INFO * pCharaInfo, INT32 idx, CHARA_RES_ID ResType, CHARA::TYPE charaType )
{
	I3ASSERT( pCharaInfo != nullptr);

	pCharaInfo->SetResType( ResType);

	//	네크워크 아이디
	pCharaInfo->SetNetIdx(idx);


	//1. Create Player Character
	// 캐릭터 타입
	if( idx == BattleSlotContext::i()->getMySlotIdx() )
	{
		if( charaType == CHARA::TYPE_NA)
			pCharaInfo->SetType( CHARA::TYPE_PLAYER);
		else
			pCharaInfo->SetType( charaType);
	}
	else
	{
		// 훈련전이라면, NPC(AI) 캐릭터로 추가
#if ENABLE_UNIT_TEST
		if( UnitTest::get_unit_test() != nullptr )
		{
			if( UnitTest::get_unit_test()->m_UnitTestFlag.is(UNIT_TEST_AI) )
			{
				pCharaInfo->SetType(CHARA::TYPE_AI);
			}
			else
			{
				pCharaInfo->SetType(CHARA::TYPE_PLAYER_NET);
			}
		}
		else
#endif
		{
			if( charaType != CHARA::TYPE_NA)
			{
				if( (charaType == CHARA::TYPE_AI) && (BattleServerContext::i()->IsP2PHost() == false) )
				{
					pCharaInfo->SetType( CHARA::TYPE_PLAYER_NET);
				}
				else
				{
					pCharaInfo->SetType( charaType);
				}
			}
			else
			{
				pCharaInfo->SetType(CHARA::TYPE_PLAYER_NET);
			}

			// 배틀에서만 동작하도록 수정
			if(g_pFramework->IsBattleStage() && MyRoomInfoContext::i()->getStage()->IsAiMode())
			{
				pCharaInfo->SetAIToClient( (idx % 2) != (BattleSlotContext::i()->getMySlotIdx() % 2) );			// AI 캐릭터인지 구분하기 위해서 설정

				//PBBUG-17250 버그때문에 주석 처리
				//BattleSlotContext::i()->setSlotState( idx, SLOT_STATE_BATTLE);	// Ai들은 항시 게임중으로 처리
			}
		}

#if defined( DEF_OBSERVER_MODE)
		ISTEST_S
		if( BattleSlotContext::i()->isObserverMe())
			pCharaInfo->SetType( CHARA::TYPE_AI);
		ISTEST_E
#endif
	}

	//	진행 팀
	MakeCharaTeamInfo( pCharaInfo, idx, charaType);
}

void CGameCharaManager::MakeAiCharaInfoForDomination( GLOBAL_CHARA_INFO * pCharaInfo, INT32 idx, CHARA_RES_ID ResType, CHARA::TYPE charaType)
{
	I3ASSERT( pCharaInfo != nullptr);
	pCharaInfo->SetResType( ResType);

	//	네크워크 아이디
	pCharaInfo->SetNetIdx(idx);

	if( charaType != CHARA::TYPE_NA)
		pCharaInfo->SetType( charaType);
	else
		pCharaInfo->SetType( CHARA::TYPE_DOMI_FULLBODY_AI);

	pCharaInfo->SetAIToClient(true);
	//진행 팀은 서버에서 받은 값으로 설정 됩니다.
}

CGameCharaBase * CGameCharaManager::AttachCharaToStage( GLOBAL_CHARA_INFO * pCharaInfo, i3Stage * pStage )
{
	CGameCharaBase * pChara = nullptr;

	I3ASSERT( pCharaInfo != nullptr);

	if( m_pTempChara[pCharaInfo->GetNetIdx()] == nullptr)
	{	// 아바타가 아니라면
		i3ClassMeta * pInstanceMeta = nullptr;
		i3InstanceManager * pManager = pStage->GetInstanceManager( CGameCharaBase::static_meta() );

		switch( pCharaInfo->GetResType())
		{
		case CHARA_RES_ID_DINO_RAPTOR :				pInstanceMeta = CGameCharaDinoRaptor::static_meta();			break;
		case CHARA_RES_ID_DINO_TREX :				pInstanceMeta = CGameCharaDinoTRex::static_meta();				break;
		case CHARA_RES_ID_DINO_STING :				pInstanceMeta = CGameCharaDinoSting::static_meta();			break;
		case CHARA_RES_ID_DINO_TANK :				pInstanceMeta = CGameCharaDinoTank::static_meta();				break;
		case CHARA_RES_ID_DINO_ACID :				pInstanceMeta = CGameCharaDinoAcid::static_meta();				break;
		case CHARA_RES_ID_DINO_ELITE :				pInstanceMeta = CGameCharaDinoElite::static_meta();			break;
		case CHARA_RES_ID_DINO_CC_RAPTOR :			pInstanceMeta = CGameCharaDinoCCRaptor::static_meta();			break;
		case CHARA_RES_ID_DINO_CC_STING :			pInstanceMeta = CGameCharaDinoCCSting::static_meta();			break;
		case CHARA_RES_ID_DINO_CC_ACID :			pInstanceMeta = CGameCharaDinoCCAcid::static_meta();			break;
		case CHARA_RES_ID_DINO_RAPTOR_MERCURY :		pInstanceMeta = CGameCharaDinoRaptorMercury::static_meta();	break;
		case CHARA_RES_ID_DINO_STING_MARS :			pInstanceMeta = CGameCharaDinoStingMars::static_meta();		break;
		case CHARA_RES_ID_DINO_ACID_VULCAN :		pInstanceMeta = CGameCharaDinoAcidVulcan::static_meta();		break;
#ifdef DOMI_DINO
		case CHARA_RES_ID_DINO_DOMI_RAPTOR:
		case CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE:
		case CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN:	pInstanceMeta = CGameCharaDomiRaptor::static_meta();			break;
		case CHARA_RES_ID_DINO_DOMI_ACID:			pInstanceMeta = CGameCharaDomiAcid::static_meta();				break;
		case CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB:	pInstanceMeta = CGameCharaDomiAcidNotBomb::static_meta();		break;
		case CHARA_RES_ID_DINO_DOMI_STING:			pInstanceMeta = CGameCharaDomiSting::static_meta();				break;
		case CHARA_RES_ID_DINO_DOMI_MUTANTREX:		pInstanceMeta = CGameCharaDomiMutantRex::static_meta();			break;
		case CHARA_RES_ID_DINO_DOMI_ACID_ITEM:		pInstanceMeta = CGameCharaDomiAcidItem::static_meta();			break;
		case CHARA_RES_ID_DINO_DOMI_TANK:			pInstanceMeta = CGameCharaDomiTank::static_meta();				break;
		case CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY:	pInstanceMeta = CGameCharaDomiRaptorMercury::static_meta();		break;
		case CHARA_RES_ID_DINO_DOMI_STING_MARS:		pInstanceMeta = CGameCharaDomiStingMars::static_meta();			break;
		case CHARA_RES_ID_DINO_DOMI_ACID_VULCAN:	pInstanceMeta = CGameCharaDomiAcidVulcan::static_meta();		break;
#endif
		default:
			I3PRINTLOG(I3LOG_FATAL,  "Invalid Chara Type");
			break;
		}

		pManager->setInstanceMeta( pInstanceMeta);
		pChara	= (CGameCharaBase *) pManager->GetFreeInstance();
		pManager->setInstanceMeta(CGameCharaBase::static_meta());
 
		const CCharaInfo * pCharaDBInfo = (const CCharaInfo*) g_pCharaInfoDataBase->GetInfo( EQUIP::ePART_CHARA, pCharaInfo->GetResType());
		pChara->Create( pCharaInfo, pCharaDBInfo);
	}
	else
	{
		pChara = (Avatar*) m_pTempChara[pCharaInfo->GetNetIdx()];
		m_pTempChara[ pCharaInfo->GetNetIdx()] = nullptr;

		if( pChara->isAIToClient() == false)
		{
			pChara->CharaChangeClanMark( BattleSlotContext::i()->getSlotInfo( pCharaInfo->GetNetIdx())->_clanMark);
		}
	}

	if( m_pCharaLayer )
	{
		I3_GAMENODE_ADDCHILD( m_pCharaLayer, pChara );
	}
	else
	{
		I3_GAMENODE_ADDCHILD( pStage, pChara );
	}

	if (GetCharaIndex(pChara) == -1)
	{
		m_CharaList.push_back( pChara );
	}

	return pChara;
}

bool CGameCharaManager::DetachCharaFromStage( INT32 idx, i3GameNode * pStage )
{
	I3ASSERT( pStage != nullptr );

	CGameCharaBase * pChara = getChara( idx );
	if( pChara == nullptr )return false;

	_RemoveChara( pChara);

	return true;
}

INT32 CGameCharaManager::GetCharaIndex(const CGameCharaBase* pChara)
{
	return i3::vu::int_index_of(m_CharaList, pChara);
		//m_CharaList.Find((void*)pChara);
}

CGameCharaDinoTRex	*	CGameCharaManager::GetTRex(void)
{
	
	const size_t CharaCount = m_CharaList.size();

	for(size_t i = 0;i < CharaCount;++i)
	{
		CGameCharaBase * pChara = getChara( i );
		if( pChara != nullptr)
		{
			if( i3::same_of<CGameCharaDinoTRex*>(pChara))
			{
				return (CGameCharaDinoTRex*)pChara;
			}
		}
	}

	return nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////
//	Chara Texture Resource
///////////////////////////////////////////////////////////////////////////////////////////

i3Texture * CGameCharaManager::_LoadCommonTexture( const char * pszPath)
{
	i3GameRes * pRes = (i3GameRes *) g_pFramework->QuaryResource( pszPath);
	I3ASSERT( pRes != nullptr);

	i3GameResTexture * pResTex = nullptr;
	if( i3::same_of<i3GameResTexture*>(pRes))
	{	//	텍스처인지 확인
		pResTex = (i3GameResTexture *) pRes;
	}
	else
	{
		I3ASSERT_0;
	}

	return pResTex->getTexture();
}

bool CGameCharaManager::_IsSameTexture( i3Texture * pSrcTex, i3Texture * pDstTex)
{	//	두 텍스처가 같은지 비교
	UINT32 srcCRC = 0, dstCRC = 0;

	pSrcTex->Lock(0);
	srcCRC = CRC32( 0xFFFFFFFF, (UINT8 *) pSrcTex->GetLockedBuffer(0), pSrcTex->GetDataSize());
	pSrcTex->Unlock(0);

	pSrcTex->Lock(0);
	dstCRC = CRC32( 0xFFFFFFFF, (UINT8 *) pDstTex->GetLockedBuffer(0), pDstTex->GetDataSize());
	pSrcTex->Unlock(0);

	return (srcCRC == dstCRC);
}

INT32 CGameCharaManager::GetCharaClanTexCount( void)
{
	return (INT32)m_TexClan._ClanList.size();
}

const i3Texture * CGameCharaManager::QueryCharaClanTex( INT32 idx)
{
	if( GetCharaClanTexCount() <= idx || idx < 0)
		return nullptr;

	i3Texture * pTex = m_TexClan._ClanList[idx];
	I3ASSERT( i3::kind_of<i3Texture*>(pTex));

	return pTex;
}

void CGameCharaManager::ReleaseClanTexture( void)
{
	i3::cu::for_each_safe_release_clear( m_TexClan._ClanList, &i3Texture::Release);
}

bool CGameCharaManager::LoadClanTexture( void)
{
	ReleaseClanTexture();

	//	Clan Mark Texture
	//	임시 Zepetto 하나만...나중에 추가 komet
	i3Texture * pTex = _LoadCommonTexture( "Clan_PointBlank_Diff.i3i");
//	I3ASSERT( pTex != nullptr );
	I3ASSERT( i3::kind_of<i3Texture*>(pTex));

	if( pTex != nullptr)
	{
		m_TexClan._ClanList.push_back(  pTex); 
	}

	return true;
}


//i3Texture* CGameCharaManager::GetBeretEmblemTexture(ITEM_RES_ID eItemResID, ETextureKind eTextureKind)
//{
//	I3_BOUNDCHK(eItemResID, ITEM_RES_ID_BERET_COUNT);
//	I3_BOUNDCHK(eTextureKind, ETK_COUNT);
//
//	return m_arBeretEmblemTexture[eItemResID-1][eTextureKind];
//}

void CGameCharaManager::_RespawnOnObject( CGameCharaBase * pChara, const NET_CHARA_INFO * pNetCharaInfo)
{
	INT32 i;

	I3ASSERT( pChara != nullptr);
	I3ASSERT( pNetCharaInfo != nullptr);

	if( MyRoomInfoContext::i()->getStage()->GetStageUID() != STAGE_UID_HELISPOT )
		return;

	INT32 iUserCount = 0;
	INT32 isBlueTeam = (pChara->getCharaNetIndex() & 1);

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( ((i % 2) == isBlueTeam) &&
			UserContext::i()->IsPlayingSlot( i) &&
			(BattleSlotContext::i()->getSlotInfo( i )->_State == SLOT_STATE_BATTLE) )
			iUserCount++;
	}

	ISTEST_S
	{
		iUserCount = 1;
	}
	ISTEST_E

	if( ( iUserCount > 0 ) && ((pNetCharaInfo->getRespawnIndex() % iUserCount) == 0))
	{
		i3Object		* pObj	= nullptr;
		tagCharaData	* pData = __getCharaData(pChara->getCharaNetIndex(), pChara->isAI());

		if (pData)
		{
			if (pData->_CharaInfo.GetTeam() == CHARACTER_TEAM_BLUE)	pObj = CGameMissionManager::i()->getBlueObject();
			else													pObj = CGameMissionManager::i()->getRedObject();
		}
		
		I3ASSERT_RETURN(pObj);

		MATRIX * pMat = ((CGameObjectRespawnHelicopter*)pObj)->GetRespawnPose();
		if( pMat != nullptr)
		{
			pChara->getMoveContext()->SetPosition( i3Matrix::GetPos( pMat));

			if( pChara->isLocal() )
			{
				// 올라 탄 상태를 설정.
				pChara->AddOnObjectState();
				pChara->getMoveContext()->SetRideObject( pObj);
				pChara->getMoveContext()->setRideObjectElapseTime( 0.5f);
			}
		}
	}
}

void CGameCharaManager::LoadAiBindData(INT32 nLevel, INT32 nIndex, TEAM_TYPE team)
{
	i3RegKey * pRoot = nullptr;

	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) || 
		MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) )
	{
		pRoot = Config::_GetRegRoot( "AICharacter/AICharacter");
	}
	else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
	{
		if(nLevel >= 4)
		{
			nLevel = 4;
		}
		pRoot = Config::_GetRegRoot( "AI_Madness/AI_Madness");
	}

	I3ASSERT( pRoot != nullptr);

	char conv[MAX_PATH];
	sprintf_s( conv, "Level_%d", nLevel);

	i3RegKey * pLevelRoot = Config::FindKey( conv, pRoot);
	I3ASSERT( pLevelRoot != nullptr);

	// 등록된 NPC 안에서 선택해야한다.
	I3ASSERT(0 <= nIndex && nIndex < pLevelRoot->getChildCount());
	i3RegKey * pCharaKey = (i3RegKey*)pLevelRoot->getChild(nIndex);
	I3ASSERT( pCharaKey != nullptr);

	// 출력 정보 (닉네임, 성별, 계급, 사용무기, 사용장비)
	i3::rc_wstring wstr_Name;
	FIND_REG_DATA( pCharaKey, "_Name", wstr_Name );
	i3::string str_Name;	i3::utf16_to_mb(wstr_Name, str_Name);
	m_AiBindData[nIndex]._wstr_nickname = GAME_PEF_RCSTRING(str_Name);

	FIND_REG_DATA(pCharaKey, "Info_Rank", &m_AiBindData[nIndex]._rank);

	FIND_REG_DATA(pCharaKey, "Weapon_Primary",	(INT32*)&m_AiBindData[nIndex]._primaryWeaponName);
	FIND_REG_DATA(pCharaKey, "Weapon_Secondary",(INT32*)&m_AiBindData[nIndex]._secondaryWeaponName);
	FIND_REG_DATA(pCharaKey, "Weapon_Melee",	(INT32*)&m_AiBindData[nIndex]._meleeWeaponName);
	FIND_REG_DATA(pCharaKey, "Weapon_Throw",	(INT32*)&m_AiBindData[nIndex]._throwWeaponName);
	FIND_REG_DATA(pCharaKey, "Weapon_Item",		(INT32*)&m_AiBindData[nIndex]._itemWeaponName);
	FIND_REG_DATA(pCharaKey, "Hp", &m_AiBindData[nIndex]._hp);
	FIND_REG_DATA(pCharaKey, "RespawnTime", &m_AiBindData[nIndex]._respawnTime);

	//Attack Time
	FIND_REG_DATA( pCharaKey, "AttackTime_Pre",		&m_AiBindData[nIndex]._AIData._PreAttack);
	FIND_REG_DATA( pCharaKey, "AttackTime",			&m_AiBindData[nIndex]._AIData._Attack);
	m_AiBindData[nIndex]._AIData._Attack += m_AiBindData[nIndex]._AIData._PreAttack;

	FIND_REG_DATA( pCharaKey, "AttackTime_Post",	&m_AiBindData[nIndex]._AIData._AfterAttack);
	m_AiBindData[nIndex]._AIData._AfterAttack += m_AiBindData[nIndex]._AIData._Attack;

	FIND_REG_DATA( pCharaKey, "AttackTime_Total",	&m_AiBindData[nIndex]._AIData._TotalTime);

	//SeeSight Angle
	FIND_REG_DATA( pCharaKey, "SeeSight_Angle",		&m_AiBindData[nIndex]._AIData._Angle);
	FIND_REG_DATA( pCharaKey, "SeeSight_Range",		&m_AiBindData[nIndex]._AIData._Range);

	//Targeting Level
	FIND_REG_DATA( pCharaKey, "Deviation",			&m_AiBindData[nIndex]._AIData._Deviation);

	FIND_REG_DATA( pCharaKey, "KeepWatchTime",		&m_AiBindData[nIndex]._AIData._KeepWatchTime);
	FIND_REG_DATA( pCharaKey, "MinPriority",		&m_AiBindData[nIndex]._AIData._Priority);
	FIND_REG_DATA( pCharaKey, "FastMoveDistance",	&m_AiBindData[nIndex]._AIData._SwapWeaponDistance);

	FIND_REG_DATA( pCharaKey, "SwapRatio_Handgun",	&m_AiBindData[nIndex]._AIData._SwapRatioHandgun);
	FIND_REG_DATA( pCharaKey, "SwapRatio_Knife",	&m_AiBindData[nIndex]._AIData._SwapRatioKnife);

	// 수류탄 투척 패턴
	FIND_REG_DATA( pCharaKey, "GrenadePattern",		(INT32 *)&m_AiBindData[nIndex]._GrenadePattern);
	FIND_REG_DATA( pCharaKey, "SmokeProbability",	(INT32 *)&m_AiBindData[nIndex]._SmokeProbability);

	// 기본 조준 위치보다 아래로 조준하여 사격하는 정도
	FIND_REG_DATA( pCharaKey, "TargetPosition",		&m_AiBindData[nIndex]._AIData._TargetHeightUnerNeck);
	
	i3RegKey * pEvadeKey = Config::FindKey( "Evasion", pCharaKey);
	I3ASSERT( pEvadeKey != nullptr);

	{
		FIND_REG_DATA( pEvadeKey, "0_SideCrawl",				&m_AiBindData[nIndex]._EvasionRate[AIET_SIDECRAWL]);
		FIND_REG_DATA( pEvadeKey, "1_Move",						&m_AiBindData[nIndex]._EvasionRate[AIET_MOVE]);
		FIND_REG_DATA( pEvadeKey, "2_SideCrawlCrouch",			&m_AiBindData[nIndex]._EvasionRate[AIET_SIDECRAWLCROUCH]);
		FIND_REG_DATA( pEvadeKey, "3_MoveCrouch",				&m_AiBindData[nIndex]._EvasionRate[AIET_MOVECROUCH]);
		FIND_REG_DATA( pEvadeKey, "4_SideCrawlFootStep",		&m_AiBindData[nIndex]._EvasionRate[AIET_SIDECRAWLFOOTSTEP]);
		FIND_REG_DATA( pEvadeKey, "5_MoveFootStep",				&m_AiBindData[nIndex]._EvasionRate[AIET_MOVEFOOTSTEP]);
		FIND_REG_DATA( pEvadeKey, "6_SideCrawlCrouchFootStep",	&m_AiBindData[nIndex]._EvasionRate[AIET_SIDECRAWLCROUCHFOOTSTEP]);
		FIND_REG_DATA( pEvadeKey, "7_MoveCrouchFootStep",		&m_AiBindData[nIndex]._EvasionRate[AIET_MOVECROUCHFOOTSTEP]);
		FIND_REG_DATA( pEvadeKey, "8_Crouch",					&m_AiBindData[nIndex]._EvasionRate[AIET_CROUCH]);
		FIND_REG_DATA( pEvadeKey, "9_Stand",					&m_AiBindData[nIndex]._EvasionRate[AIET_STAND]);
	}

	// 1.5 Parts
	{
		i3RegKey * pPartsKey = Config::FindKey( "Parts", pCharaKey);
		I3ASSERT( pPartsKey != nullptr);

		INT32 chara, upper, lower, glove, head, face, belt, holster;

		if( i3::contain_string( ::GetCommandLine(), "OLD_EQUIPMENT") >= 0 )
		{
			FIND_REG_DATA( pPartsKey, "Chara",				&chara);
			FIND_REG_DATA( pPartsKey, "Upper",				&upper);
			FIND_REG_DATA( pPartsKey, "Lower",				&lower);
			FIND_REG_DATA( pPartsKey, "Glove",				&glove);
			FIND_REG_DATA( pPartsKey, "Head",				&head);
			FIND_REG_DATA( pPartsKey, "Face",				&face);
			FIND_REG_DATA( pPartsKey, "Belt",				&belt);
			FIND_REG_DATA( pPartsKey, "Holster",			&holster);
		}
		else
		{
			i3::rc_string str_key;

			if( team == TEAM_RED )	str_key = "Chara_Red";
			else					str_key = "Chara_Blue";

			FIND_REG_DATA( pPartsKey, str_key.c_str(),		&chara);

			const CCharaInfo * pCharaInfo = g_pCharaInfoDataBase->GetCharaDBInfo(static_cast<T_ItemID>(chara) );

			upper	= static_cast<INT32>( pCharaInfo->GetPartsItemID(EQUIP::ePART_VEST) );
			lower	= static_cast<INT32>( pCharaInfo->GetPartsItemID(EQUIP::ePART_PANT) );
			glove	= static_cast<INT32>( pCharaInfo->GetPartsItemID(EQUIP::ePART_GLOVE) );
			head	= static_cast<INT32>( pCharaInfo->GetPartsItemID(EQUIP::ePART_HEADGEAR) );
			face	= static_cast<INT32>( pCharaInfo->GetPartsItemID(EQUIP::ePART_FACEGEAR) );
			belt	= static_cast<INT32>( pCharaInfo->GetPartsItemID(EQUIP::ePART_BELT) );
			holster = static_cast<INT32>( pCharaInfo->GetPartsItemID(EQUIP::ePART_HOLSTER) );
		}

		// vv3 - check
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA, chara);
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_UPPER, upper);
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_LOWER, lower);
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_GLOVE, glove);
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_HEAD, head);
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_FACE, face);
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_BELT, belt);
		m_AiBindData[nIndex]._Parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_HOLSTER, holster);
	}

	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
	{
		//매드니스 모드 추가 작업
		FIND_REG_DATA( pEvadeKey, "10_Jump",								&m_AiBindData[nIndex]._EvasionRate[AIET_JUMP]);

		i3RegKey * pReinforceStatus = Config::_FindKey( "ReinforceStatus", pCharaKey );
		I3ASSERT( pReinforceStatus != nullptr);

		FIND_REG_DATA( pReinforceStatus, "Jump_Height",						&m_AiBindData[nIndex]._Jump_Height);
		FIND_REG_DATA( pReinforceStatus, "Jump_Time",						&m_AiBindData[nIndex]._Jump_Time);
		FIND_REG_DATA( pReinforceStatus, "ReduceDamage",					&m_AiBindData[nIndex]._ReduceDamage);
		FIND_REG_DATA( pReinforceStatus, "ReduceDamage_PlayerCount1",		&m_AiBindData[nIndex]._ReduceDamage_PlayerCount[AIPLY_1]);
		FIND_REG_DATA( pReinforceStatus, "ReduceDamage_PlayerCount2",		&m_AiBindData[nIndex]._ReduceDamage_PlayerCount[AIPLY_2]);
		FIND_REG_DATA( pReinforceStatus, "ReduceDamage_PlayerCount3",		&m_AiBindData[nIndex]._ReduceDamage_PlayerCount[AIPLY_3]);
		FIND_REG_DATA( pReinforceStatus, "ReduceDamage_PlayerCount4",		&m_AiBindData[nIndex]._ReduceDamage_PlayerCount[AIPLY_4]);
		FIND_REG_DATA( pReinforceStatus, "Speed",							&m_AiBindData[nIndex]._Speed);
		FIND_REG_DATA( pReinforceStatus, "InvincibleTime",					&m_AiBindData[nIndex]._InvincibleTime);

		//특성값
		i3RegKey * pFeature = Config::_FindKey( "feature", pCharaKey );
		I3ASSERT( pFeature != nullptr);

		FIND_REG_DATA( pFeature, "Attack_Death_Player",						&m_AiBindData[nIndex]._AIFeature._Attack_Death_Player);
	}
}

void CGameCharaManager::RaptorHP_Recovery(INT32 iKillerIdx, INT32 iDeathIdx)
{
	I3_BOUNDCHK(iKillerIdx, SLOT_MAX_COUNT);

	CGameCharaBase* pKiller = getCharaByNetIdx( iKillerIdx);

	if(IsHP_Recovery(iKillerIdx, iDeathIdx)) {
		pKiller->setFullHP();
		if(iKillerIdx == BattleSlotContext::i()->getMySlotIdx()) {
			if( CHud::i() != nullptr)
				CHud::i()->AddHP_Recovery();
		}
	}
}

void CGameCharaManager::Assist_Kill(INT32 iKillerIdx, INT32 iDeathIdx)
{
	I3_BOUNDCHK(iKillerIdx, SLOT_MAX_COUNT);

	if(IsAssist_Kill(iKillerIdx, iDeathIdx)) {
		if(iKillerIdx == BattleSlotContext::i()->getMySlotIdx()) {
			if( CHud::i() != nullptr)
				CHud::i()->AddAssist_Kill();
		}

	}
}
//탈출미션 새로운 맵이 생길대마다 직접 섹터값을 추가 해줘야 합니다.
bool CGameCharaManager::IsEscapeNearKill(INT32 DeathIdx)
{
	// 커버리티 : 라운드 시작 초반에 잠시 nullptr이 들어올 수 있습니다. 2011.11.29. ugeun.ji
	CGameCharaBase * pVictims = nullptr;
	
	pVictims = g_pCharaManager->getCharaByNetIdx( DeathIdx);
	if (pVictims == nullptr) return false;

	//피해자가 공룡이면 리턴 시킵니다.
	if(pVictims->IsDino())
		return false;

	//피해자의 포탈번호를 가져옵니다.
	INT32 Sector =  pVictims->getPortalID();

	//스테이지 정보를 가져옵니다.
	STAGE_ID StageUID = MyRoomInfoContext::i()->getStage()->GetStageUID();
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
	{
		switch (StageUID)
		{
		case STAGE_UID_BREEDING_NEST:
			if (Sector == 3)
			{
				return true;
			}
			else
				return false;
			break;
		case STAGE_UID_DUPTOWN:
			if (Sector == 12)
			{
				return true;
			}
			else
				return false;

			break;

		case STAGE_UID_DINOBREAKDOWN:
			if (Sector == 8)
			{
				return true;
			}
			else
				return false;

			break;
		}
	}

	return false;
}

bool CGameCharaManager::IsAssist_Kill(INT32 iKillerIdx, INT32 iDeathIdx)
{

	I3_BOUNDCHK(iKillerIdx, SLOT_MAX_COUNT);
	I3_BOUNDCHK(iDeathIdx, SLOT_MAX_COUNT);

	CGameCharaBase* pKiller = getCharaByNetIdx(iKillerIdx);
	CGameCharaBase* pDeath = getCharaByNetIdx(iDeathIdx);

	if(pDeath != nullptr && pKiller != nullptr)
	{
		if(pDeath->getDamagedByHowl() && pKiller->IsRaptorBase())
		{
			return true;
		}
	}

	return false;

}
bool CGameCharaManager::IsHP_Recovery(INT32 iKillerIdx, INT32 iDeathIdx)
{
	I3_BOUNDCHK(iKillerIdx, SLOT_MAX_COUNT);
	I3_BOUNDCHK(iDeathIdx, SLOT_MAX_COUNT);

	CGameCharaBase* pKiller = getCharaByNetIdx( iKillerIdx);
	CGameCharaBase* pDeath = getCharaByNetIdx( iDeathIdx);

	if(pDeath != nullptr && pKiller != nullptr)
	{
		if(pDeath->getDamagedByHowl() && pKiller->IsRaptorBase())
		{
			//Key:EscapeMission 어시스트킬 HP회복을 막습니다.
			//HP회복막음
			
		return true;
			/*
			if(((CGameCharaDino*)pKiller)->GetDinoWeaponUsageType() == WEAPON_SLOT_PRIMARY)
			{
				return true;
			}
			*/
		}
	}

	return false;
}

const i3GameResChara * CGameCharaManager::QueryCharaResource( CHARA_RES_ID type, bool bCommonLoad )
{

	i3GameResChara * pRes = nullptr;
	size_t Count = 0;

	if( type < CHARA_RES_ID_HUMAN_E)
	{
		Count = m_CharaRes.size();
		I3ASSERT( type < (INT32) Count);
		pRes = (i3GameResChara*) m_CharaRes[ type];
	}
	else if( type < CHARA_RES_ID_DINO_E)
	{
		Count = m_DinoRes.size();
		I3ASSERT( type - CHARA_RES_ID_DINO_S < (INT32) Count);
		pRes = (i3GameResChara*) m_DinoRes[ type - CHARA_RES_ID_DINO_S];
	}
	else
	{
		I3ASSERT_0;
	}

	if( pRes == nullptr)
	{
		const CCharaInfo * pInfo = (const CCharaInfo*) g_pCharaInfoDataBase->GetInfo( EQUIP::ePART_CHARA, type );

		if( pInfo != nullptr )
		{
			i3::rc_wstring wstrPath;
			pInfo->getResPath( wstrPath);
			I3ASSERT( !(wstrPath.empty()) );
			i3::wstring wstrFilename;	
			i3::extract_filename( wstrPath, wstrFilename);
			i3::string strFilename;		i3::utf16_to_mb(wstrFilename, strFilename);

			pRes = (i3GameResChara *) g_pFramework->QuaryResource( strFilename.c_str(), bCommonLoad);
			I3ASSERT( pRes != nullptr);
			if( pRes == nullptr)
			{
				I3PRINTLOG(I3LOG_NOTICE, "Query Chara Resource File Name : %s", strFilename);
			}

			if( type < CHARA_RES_ID_HUMAN_E)
			{
				I3ASSERT( m_CharaRes[ pInfo->GetResID()] == nullptr);
				m_CharaRes[ pInfo->GetResID()] = pRes;
			}
			else
			{
				I3ASSERT( m_DinoRes[ pInfo->GetResID() - CHARA_RES_ID_DINO_S] == nullptr);
				m_DinoRes[ pInfo->GetResID() - CHARA_RES_ID_DINO_S] = pRes;
			}
		}
	}

	return pRes;
}

void CGameCharaManager::SetEquipResource( T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype, const i3GameResSceneGraph * pRes)
{
	EQUIP::ePART part = EQUIP::ItemID2PartsType( ItemID);
	EQUIP::RES_ID resID = EQUIP::ItemID2ResID( ItemID);

	if (part < 0 || part >= EQUIP::ePART_COUNT)
	{
		I3_BOUNDCHK(part, EQUIP::ePART_COUNT);
		return;
	}

	if ( resID >= m_pEquipRes[part].size() )
	{
		I3ASSERT(resID < m_pEquipRes[part].size());
		return;
	}

	m_pEquipRes[part][resID]->SetEquipRes(team, res_subtype, const_cast<i3GameResSceneGraph*>(pRes) );
}

void CGameCharaManager::GetEquipPath( char * pszOut, EQUIP::ePART part, EQUIP::RES_ID ResID, TEAM_TYPE team, INT32 res_subtype)
{
	const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( part, ResID);

	if( pInfo != nullptr)
	{
		i3::rc_wstring wstrName;

		pInfo->GetResPostName( wstrName, team , res_subtype);

		if( !wstrName.empty() )
		{
			i3::string strName;		i3::utf16_to_mb(wstrName, strName);
			i3::snprintf( pszOut, MAX_PATH, "Avatar/%s/%s/%s.i3s", AVATAR::getPartName(part), strName, strName);
		}
		else
			pszOut[0] = 0;
	}
	else
		pszOut[0] = 0;
}

void CGameCharaManager::GetEquipPath( char * pszOut, T_ItemID itemID, TEAM_TYPE team, INT32 res_subtype)
{
	GetEquipPath( pszOut, EQUIP::ItemID2PartsType( itemID),  EQUIP::ItemID2ResID( itemID), team, res_subtype);
}

const i3GameResSceneGraph *	CGameCharaManager::QueryEquipResource( T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype, bool bDirectLoad )
{
	char szFullPath[ MAX_PATH];

	const i3GameResSceneGraph * pRes = GetEquipResource( EQUIP::ItemID2PartsType( ItemID), EQUIP::ItemID2ResID( ItemID), team, res_subtype);

	if( pRes == nullptr && bDirectLoad)
	{
		GetEquipPath( szFullPath, ItemID, team, res_subtype);

		if( szFullPath[0] != 0)
		{
			if( i3FileStream::isFile( szFullPath) )
			{
				pRes = (i3GameResSceneGraph*) g_pFramework->LoadResource( szFullPath, 0);

				CheckDiffuseTexture(ItemID, pRes);

				SetEquipResource(ItemID, team, res_subtype, pRes);
			}
		}
	}

	return pRes;
}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
bool CGameCharaManager::IsExistEquipResource(EQUIP::ePART part, EQUIP::RES_ID ResID, TEAM_TYPE team)
{
	I3ASSERT_RETURN(part != EQUIP::ePART_UNKNOWN, false);

	i3::vector<bool>& 	resCheckList = m_ResCheckList[part][team];

	if (ResID >= resCheckList.size() )
		return false;

	if(	 resCheckList[ ResID] == false)
	{
		char szFullPath[ MAX_PATH];

		GetEquipPath( szFullPath, part, ResID, team);
			
		if( szFullPath[0] != 0)
		{
			resCheckList[ ResID] = i3FileStream::isFile( szFullPath);
		}
	}
	
	return resCheckList[ ResID];
}
#endif

void CGameCharaManager::CheckDiffuseTexture( T_ItemID ItemID, const i3GameResSceneGraph * pRes )
{
	i3::rc_wstring wstr_diffuse;

	const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(ItemID);
	if( pInfo != nullptr )
		pInfo->getDiffuseTexture(wstr_diffuse);
	
	if(wstr_diffuse.length() > 0)
	{
		i3::string diffuse;		i3::utf16_to_mb(wstr_diffuse, diffuse);

		if( g_pFramework->GetResourceManager()->IsExist( diffuse.c_str()))
		{
			i3Body* pBody = (i3Body*)const_cast<i3GameResSceneGraph*>(pRes)->getSceneGraphInfo()->getInstanceSg();
			I3ASSERT(pBody);

			i3LOD* pLOD = nullptr;

			for(INT32 lod = 0; lod < (INT32)pBody->getLODCount(); lod++)
			{
				pLOD = pBody->getLOD( lod);

				i3Texture * pChangeDiff = nullptr;
				i3TextureBindAttr * pTex = (i3TextureBindAttr*) i3Scene::FindAttrByType( 
					pLOD->getShapeNode(), i3TextureBindAttr::static_meta());

				if (pTex == nullptr)
				{
					I3ASSERT(pTex);		continue;
				}
				// 텍스처 로드
				pChangeDiff = g_pFramework->GetResourceManager()->LoadTexture( diffuse.c_str());

				if (pChangeDiff == nullptr)
				{
					I3ASSERT(pChangeDiff);	continue;
				}

				pTex->SetTexture( pChangeDiff);
				pChangeDiff->Release();
			}
		}
	}
}

const i3GameResSceneGraph * CGameCharaManager::GetEquipResource( EQUIP::ePART part, EQUIP::RES_ID ResID, TEAM_TYPE team, INT32 res_subtype)
{
	if( part <= EQUIP::ePART_UNKNOWN || EQUIP::ePART_COUNT <= part )
	{
		I3PRINTLOG(I3LOG_NOTICE, "invalid arg... Part = %d", part );
		return nullptr;
	}

	tagEquipRes & List = m_pEquipRes[part];

	if( ResID > 0 && List.size() > ResID )
	{
		const i3GameResSceneGraph * pRes = List[ResID]->GetEquipRes(team, res_subtype);
		if( pRes != nullptr)
			return pRes;
	}

	return nullptr;
}

bool CGameCharaManager::IsAvailableCharaResourceFromManager( T_ItemID nItemID)
{
	if( nItemID == 0)
		return false;

	const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( nItemID);
	
	if( (i3::same_of<CCharaInfo*>(pInfo) || i3::same_of<CDinoInfo*>(pInfo)) == false)
		return false;

	i3::rc_wstring wstrPath;
	pInfo->getResPath( wstrPath);

	// Path는 제거하고 파일이름만
	i3::wstring wstrFilename;	
	i3::extract_filename(wstrPath, wstrFilename);
	i3::string strFilename;		i3::utf16_to_mb(wstrFilename, strFilename);
	
	i3::change_file_ext(strFilename, "I3CHR");
	// 무기 I3CHR 리소스가 실제로 로딩되었는지 확인
	i3GameRes * pRes = g_pFramework->QuaryResource( strFilename.c_str() );
	if( pRes == nullptr)
	{
		return false;
	}

	return true;
}

CHARA_RES_ID CGameCharaManager::GetCharaResType( const i3GameResChara * pCharaRes )
{
	for ( size_t i = 0 ; i < m_CharaRes.size() ; ++i )
	{
		i3GameResChara * pRes = m_CharaRes[i];
		if ( pRes == pCharaRes )
			return (CHARA_RES_ID) i;
	}

	return CHARA_RES_ID_UNKNOWN;
}
 
void CGameCharaManager::OnUpdate(REAL32 tm)
{
	Avatar* pAvatar = nullptr;

	// 로드가 완료된 캐릭터
	I3ASSERT( m_pAvatarManager != nullptr);

	m_pAvatarManager->OnUpdate();

	// ref = 2
	pAvatar = m_pAvatarManager->GetLoadedAvatar(); 

	if( pAvatar != nullptr)
	{
		if( g_pFramework->IsBattleStage() == false)
		{
			//대표 캐릭터 리소스(Shape정보를 복사할 기존 캐릭터 1개만 있으면 된다.)
			CHARA_RES_ID Res = pAvatar->getCharaInfo()->GetResType();
			I3ASSERT(Res != CHARA_RES_ID_UNKNOWN);
			I3ASSERT( pAvatar->getCharaInfo()->GetResType() != CHARA_RES_ID_UNKNOWN );
			I3ASSERT( pAvatar->getCharaInfo()->GetResType() < CHARA_RES_ID_HUMAN_E);

			if( pAvatar->GetCallBack() == UICharaView::sAfterLoadChara)
			{
				I3ASSERT( BattleServerContext::i()->IsP2PActivate() == false);
				
				CharaViewMgr::getView()->RemoveViewChara();

				SetLobbyAvatar( pAvatar);
				m_pTempChara[0] = nullptr;

				pAvatar->CreateLaterInLobby();

				//해당 클래스의 콜백함수로 들어간다..
				pAvatar->ProcessCallback();	// ref =3
				pAvatar->LoadFinish();
				//I3_MUST_RELEASE(pAvatar);	// ref = 2
			}
		}
		else
		{
			pAvatar->LoadFinish();
			m_pIOManager->TrashRes( false);//true);
		}
	}
	else
	{
		//UIFloatCharacterView * pCharaView = static_cast<UIFloatCharacterView *>(GameUI::GetFloating(UFW_CHARACTER));
		//if(pCharaView) pCharaView->DoUpdateCharacter();
	}
	
	// Animation Backround load
	//UpdatePreloadAnim();

	__BGLoading();

	if( g_pFramework->IsBattleStage() == false)
	{
		m_IOTrashResTimer += tm;
		if( m_IOTrashResTimer > 10.f)
		{
			m_pIOManager->TrashRes( false);
			m_IOTrashResTimer = 0.f;
		}
	}

	OutLineUpdate();
}

//--------------------------------------------------------------------------------------
//기존 addChara를 호출하는 클래스의 메타를 지정해 클래스값및 콜백을 등록해 주어야 한다.
//--------------------------------------------------------------------------------------
bool CGameCharaManager::CreateAvatarForView( AvatarLoadedCallback callback, INT32 idx, GLOBAL_CHARA_INFO* pInfo,
										  CHARA_RES_ID ResType, CHARA::TYPE charaType)
{
	I3TRACE("캐릭터 생성 요청. InLobby \n");

	if( pInfo->GetParts()->IsValid() == false)
		return false;

	MakeCharaInfo( pInfo, idx, ResType, charaType);

	if( getCharaByNetIdx( 0) == nullptr)
	{
		INT32 loadSlot = m_pAvatarManager->LoadAvatarQue( callback, pInfo, 0, true);
		if( loadSlot != -1)
		{

			I3TRACE( "Parts -----------------------------------------------\n");
			I3TRACE( "Chara : %d,		Head : %d\n", pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA), pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HEAD) );
			I3TRACE( "Body : %d,		Pants : %d\n", pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_UPPER), pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_LOWER) );
			I3TRACE( "Glove : %d,		Facegear : %d\n", pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_GLOVE), pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_FACE) );
			I3TRACE( "Belt : %d,		Holster : %d\n", pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_BELT), pInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HOLSTER) );
			pInfo->setLoadSlotIndex( loadSlot);
			setCharaResState( 0, CGameCharaManager::RESSTATE_LOADING);

			// 모델링 로딩 정보를 주고 다시 원래의 파츠 정보로 설정한다.
			Avatar * pAvatar = m_pAvatarManager->GetLoadAvatarQue( loadSlot, true);
			pAvatar->SetPartsEquipInfo( pInfo->GetParts());

			if( m_pTempChara[0] != nullptr)
			{// 이전 캐릭터를 로딩 하기 전에 들어오면 이전 것은 삭제
				m_pTempChara[0]->ReturnInstance();
			}

			m_pTempChara[0] = pAvatar;

			return true;
		}
	}
	
	return false;
}

Avatar * CGameCharaManager::ChangeTeamAvatar( AvatarLoadedCallback callback, INT32 idx, CHARA::TYPE charaType)
{
	SLOT_STATE slotState = (SLOT_STATE)BattleSlotContext::i()->getSlotInfo(idx)->_State;

	if( BattleSlotContext::i()->getMySlotIdx() != idx)
	{
		if(slotState < SLOT_STATE_LOAD) // 플레이어 슬롯상태가 로비이하이면 (슬롯이 빈경우)
			return nullptr;
	}

	NET_CHARA_INFO* pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( idx);
	Avatar* pChara = (Avatar*)getCharaByNetIdx(idx);

	if(pChara != nullptr)
	{
		_RemoveChara(pChara);
		setCharaByNetIdx(idx, nullptr);
	}	

	// 로딩은안되었어도 팀데이터는 있어야한다.
	GLOBAL_CHARA_INFO* pCharaInfo = setCharaInfo(idx, (charaType==CHARA::TYPE_DOMI_FULLBODY_AI)? true:false); // 함수명에속지말길
	g_pCharaManager->MakeCharaInfo(pCharaInfo, idx, (CHARA_RES_ID)pNetCharaInfo->getCharaParts()->GetPartsResID(CHAR_EQUIPMENT_PARTS_CHARA), charaType);
	pNetCharaInfo->getCharaParts()->CopyTo(pCharaInfo->SetParts());
	setCharaTeamAutoChange(idx, true);

	I3ASSERT(getCharaByNetIdx(idx) == nullptr);
	
	return CreateAvatarPlayer( (AvatarLoadedCallback) callback, idx, pNetCharaInfo, BattleSlotContext::i()->getCharaType(idx), false, true);
}

Avatar * CGameCharaManager::ChangeTeamAvatarAI( AvatarLoadedCallback callback, INT32 idx, CHARA::TYPE charaType, AI_BIND_DATA * pBindData)
{
	BattleSlotContext::i()->setSlotInfo(idx)->_State = SLOT_STATE_BATTLE;

	Avatar * pAvatar = ChangeTeamAvatar( nullptr, idx, charaType);
	I3ASSERT( pAvatar != nullptr);

	GLOBAL_CHARA_INFO* pCharaInfo = setCharaInfo(idx, (charaType==CHARA::TYPE_DOMI_FULLBODY_AI)? true:false); // 함수명에속지말길
	pCharaInfo->SetAIBind( pBindData);

	AfterLoadAIChara( idx, pBindData);

	pAvatar->SetLoadFinishCallback( callback);

	return pAvatar;
}

void CGameCharaManager::SetLobbyAvatar( Avatar * pAvatar)
{
	if( m_pLobbyAvatar != nullptr)
		m_pLobbyAvatar->ReturnInstance(); 
	m_pLobbyAvatar = pAvatar;
}

Avatar * CGameCharaManager::CreateAvatarPlayer( AvatarLoadedCallback callback, INT32 idx, NET_CHARA_INFO * pNetInfo
											   , CHARA::TYPE charaType, bool bResetInfo /*= true*/, bool bForceAvatar) //, AvatarLoadedCallback callback_finish_for_AI)
{
	//g_pFramework->GetViewer()->startProfile( I3_VIEWER_PROFILE_USERDEF0);

	Avatar * pAvatar = nullptr;

	I3TRACE("캐릭터 생성 요청. InBattle Index = %d\n",idx);
	if (m_pTempChara[idx] != nullptr)
	{
		//이미 생성된 캐릭터 입니다.
		return nullptr;
	}

	GLOBAL_CHARA_INFO * pCharaInfo = setCharaInfo( idx, (charaType==CHARA::TYPE_DOMI_FULLBODY_AI)?true:false);
	I3ASSERT( pCharaInfo != nullptr);
	if( bResetInfo)
	{
		// 0.01ms
		pCharaInfo->Reset();
		g_pCharaManager->MakeCharaInfo( pCharaInfo, idx, (CHARA_RES_ID) pNetInfo->getCharaParts()->GetPartsResID(CHAR_EQUIPMENT_PARTS_CHARA), charaType);
		pNetInfo->getCharaParts()->CopyTo( pCharaInfo->SetParts());
	}

	if( getCharaByNetIdx( idx) == nullptr)
	{
		// 0.008ms
		bool bUseAvatar = true;
		bool bFreezeTeam = false;
		bool bAnotherParts = true;

		// 보이는 모델링 파츠 정보와 다르게 설정될 수 있기때문에 원래의 정보를 백업!!
		CHARA_PARTS_INFO	backupPartsInfo;
		pCharaInfo->GetParts()->CopyTo( &backupPartsInfo);

		if( g_pFramework->IsBattleStage() )
		{
			// vv3 - check
			T_ItemID changeItemID = pCharaInfo->GetParts()->GetPartsItemID((CHAR_EQUIPMENT_PARTS_CHARA));
			const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( changeItemID);
			I3ASSERT( pCharaDBInfo != nullptr);

			if (!(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && charaType == CHARA::TYPE_AI) )
			{
				if (idx != BattleSlotContext::i()->getMySlotIdx())
				{
					if (g_pEnvSet->m_bDisplayOtherParts == false &&
						MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEATHMATCH))
					{
						// 상대편 파츠 보기가 Off
						bAnotherParts = false;
					}

					if (g_pEnvSet->isFixedViewCharacter() == true)
					{	// 상대방 캐릭터 고정
						bFreezeTeam = true;

						TEAM_TYPE team = (pCharaInfo->GetTeam() == CHARACTER_TEAM_RED) ? TEAM_RED : TEAM_BLUE;

						if (team == TEAM_RED)
						{
							if (pCharaDBInfo->GetTeamRed_ItemID() != 0)
							{
								changeItemID = pCharaDBInfo->GetTeamRed_ItemID();
								pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo(changeItemID);
								I3ASSERT(pCharaDBInfo != nullptr);
							}
						}
						else
						{
							if (pCharaDBInfo->GetTeamBlue_ItemID() != 0)
							{
								changeItemID = pCharaDBInfo->GetTeamBlue_ItemID();
								pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo(changeItemID);
								I3ASSERT(pCharaDBInfo != nullptr);
							}
						}
					}

					if (!bAnotherParts || bFreezeTeam)
					{	// 파츠는 캐릭터용 전용으로 고정
						pCharaInfo->SetParts()->SetItemID(changeItemID);
						for (INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
						{
							T_ItemID itemID = static_cast<T_ItemID>(pCharaDBInfo->GetPartsItemID((EQUIP::ePART) i));
							if (itemID != 0)
								pCharaInfo->SetParts()->SetItemID(itemID);
						}
						pNetInfo->setCharaParts(pCharaInfo->GetParts());
					}
				}
			}
			
			// 더미 캐릭터와 비교
			{
				if( IsDummyCharaID( changeItemID) )
				{	// vv3 - check
					if( pCharaInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_SKIN) == 0)
					{
						bool bSame = true;
						for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
						{
							T_ItemID default_item_id = static_cast<T_ItemID>(pCharaDBInfo->GetPartsItemID( (EQUIP::ePART) i));
							T_ItemID equip_parts_id = pCharaInfo->GetParts()->GetPartsItemID( EQUIP::Parts2CommonParts( (EQUIP::ePART)i));

							if (EQUIP::ePART_BERET == (EQUIP::ePART)i)
							{
								if ((default_item_id != equip_parts_id))
								{
									bSame = false;
									break;
								}
							}
							else if( ( default_item_id != 0 ) && ( default_item_id != equip_parts_id ) )
							{
								bSame = false;
								break;
							}
						}
						bUseAvatar = !bSame;
					}
				}
			}
		}

		// 5.9ms
		INT32 loadSlot = m_pAvatarManager->LoadAvatarQue( callback, pCharaInfo, idx, bUseAvatar && bForceAvatar);

		if( loadSlot != -1)
		{	// loadSlot이 -1이라면 이미 로딩중이다.
			pCharaInfo->setLoadSlotIndex( loadSlot);
			setCharaResState( idx, CGameCharaManager::RESSTATE_LOADING);

			// vv3 - check
			const CCharaInfo* pCharaDBInfo = (const CCharaInfo *)g_pCharaInfoDataBase->GetInfo( pCharaInfo->GetParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));
			pAvatar = m_pAvatarManager->GetLoadAvatarQue( loadSlot, (bUseAvatar && bForceAvatar));
		
			I3ASSERT( pAvatar != nullptr);

			if( bUseAvatar == false || bForceAvatar == false)
			{
				I3TRACE( "Avatar is Dummy!!!");
				pAvatar->SetLoadedParts( true);
			}
			
			// vv3 - check
			// 무기를 bg 로드한다.
			// 인게임, UI에서 같이 쓴다. 그런데 어차피 무기 정보라 이 정도만 처리해도 된다.
			CHARA_WEAPON_INFO CharaInfo_WeaponList;
			if( idx != MAX(0, BattleSlotContext::i()->GetMySlotIdxEx()) )
			{
				NET_CHARA_INFO * pNetCharaInfo = const_cast<NET_CHARA_INFO*>(BattleSlotContext::i()->getNetCharaInfo( idx));
				pNetCharaInfo->GetWeaponList(&CharaInfo_WeaponList);
			}	
			else
			{
				UI_CHARA_INFO  * pUICharaInfo = const_cast<UI_CHARA_INFO*>(UserInfoContext::i()->GetMyUICharaInfo());
				pUICharaInfo->GetWeaponList(&CharaInfo_WeaponList);
			}
			
			for( UINT32 i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++ )
			{
				T_ItemID weaponItemID = 0;
				weaponItemID = CharaInfo_WeaponList.m_Info[i].m_TItemID;
				g_pWeaponManager->PushLoadWeapon( weaponItemID, true);
			}

			// 36.9ms
			pAvatar->CreateLater( pCharaDBInfo, false);
			I3_REF_CHANGE( m_pTempChara[pAvatar->getCharaNetIndex()], pAvatar);	// ref = 5

			// 모델링 로딩 정보를 주고 다시 원래의 파츠 정보로 설정한다.
			pAvatar->SetPartsEquipInfo( &backupPartsInfo);

			//해당 클래스의 콜백함수로 들어간다..
			pAvatar->ProcessCallback();	// ref =5
			I3_MUST_RELEASE(pAvatar);	// ref = 4

			if( bUseAvatar == false || bForceAvatar == false)	// 기본 더미 파츠 사용이라면 바로 LoadFinish
			{
				pAvatar->LoadFinish();
			}
		}
		else
		{
			I3ASSERT( loadSlot != -1);
		}
	}
	else
	{
		return nullptr;
	}

	//RESET_PROFILE();
//	REPORT_PROFILE();

	//g_pFramework->GetViewer()->endProfile( I3_VIEWER_PROFILE_USERDEF0);
	
	return pAvatar;
}

Avatar * CGameCharaManager::CreateAvatarAI( AvatarLoadedCallback finish, INT32 idx, NET_CHARA_INFO * pNetInfo,
		CHARA::TYPE charaType, AI_BIND_DATA * pBindData)
{
	Avatar * pAvatar = CreateAvatarPlayer( nullptr, idx, pNetInfo, charaType, true, true);

	GLOBAL_CHARA_INFO * pCharaInfo = setCharaInfo( idx, (charaType==CHARA::TYPE_DOMI_FULLBODY_AI)?true:false);
	I3ASSERT( pCharaInfo != nullptr);
	pCharaInfo->SetAIBind( pBindData);

	AfterLoadAIChara( idx, pBindData);

	pAvatar->SetLoadFinishCallback( finish);

	return pAvatar;
}

void CGameCharaManager::PreStartStage( void)
{
	INT32 i;

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_CharaData[ i]._CharaResState = RESSTATE_NONE;
		m_CharaData[ i]._TeamChange = false;
	}

	for( i = 0; i < MAX_COUNT_DOMI_DINO; i++)
	{
		m_AICharaData[ i]._CharaResState = RESSTATE_NONE;
		m_AICharaData[ i]._TeamChange = false;
	}
}

void CGameCharaManager::__PreCreateCharaShapes( CHARA_RES_ID type, INT32 idx)
{
	INT32 i;

	const i3GameResChara * pRes = QueryCharaResource( type, true);

	if( pRes == nullptr)	return;

	i3Body * pSrcBody = (i3Body*) i3Scene::FindNodeByExactType( ((i3GameResChara *)pRes)->getScene(), i3Body::static_meta());
	I3ASSERT( pSrcBody != nullptr);

	i3LOD * pLOD = pSrcBody->getFrameLOD();
	I3ASSERT( pLOD != nullptr);

	i3Skeleton * pSrcSkel = pLOD->getSkeleton();

	i3Skeleton * pDestSkel = i3Skeleton::new_object();
	pSrcSkel->CopyTo( pDestSkel, I3_COPY_INSTANCE);

	i3MatrixStack mtxStack;
	I3_ONINSTANCING_INFO info;
	info._pBase			= nullptr;
	info._pMatrixStack	= &mtxStack;

	MATRIX mtx,temp;
	
	i3Matrix::SetPos( &mtx, (type==CHARA_RES_ID_ACID_POLE)?10.f:30.f, g_pConfigEx->GetPhysix().HeightChara, (REAL32) idx * 10.f, 1.f);
	mtxStack.Push( &temp, &mtx);
	
	// 여기서 PhysX Shape를 생성합니다
	pDestSkel->OnInstancing( &info);
	
	// 생성된 Shape을 보관하자.
	CHARA::PHYSX_SHAPE_TYPE physxType = (type==CHARA_RES_ID_ACID_POLE)?CHARA::PHYSX_SHAPE_MALE : CHARA::PHYSX_SHAPE_FEMALE;
	tagPRELOADEDPHYSICSHAPE * pShapeInfo = &m_PhysixShapeArray[physxType][ idx];

	pShapeInfo->_shapeCount = pSrcSkel->getBoneCount();
	pShapeInfo->_pShapes = (i3PhysixShapeSet**) i3MemAlloc( sizeof(i3PhysixShapeSet*) * pShapeInfo->_shapeCount);
	pShapeInfo->_state = eAPS_NONE;

	for( i = 0; i < pDestSkel->getBoneCount(); i++)
	{
		if( pDestSkel->isShapeSet( i) == true)
		{
			i3PhysixShapeSet * pShapeSet = pDestSkel->getShapeSet( i);
			I3_MUST_ADDREF(pShapeSet);
			pShapeInfo->_pShapes[ i] = pShapeSet;
			pShapeInfo->_pShapes[ i]->modifyDynamicState( I3_PHYSIX_STATE_DISABLE, 0);
		}
		else
		{
			pShapeInfo->_pShapes[ i] = nullptr; 
		}
	}

	I3_SAFE_RELEASE( pDestSkel);

	if( type == CHARA_RES_ID_ACID_POLE)
		m_vPhysxShapeList_Male.push_back( pShapeInfo);
	else
		m_vPhysxShapeList_Female.push_back( pShapeInfo);
}

CGameCharaManager::tagPRELOADEDPHYSICSHAPE * CGameCharaManager::__FindNotUsedPhysXShape( CHARA::PHYSX_SHAPE_TYPE type)
{
	i3::vector<tagPRELOADEDPHYSICSHAPE*> * pList;

	if( type == CHARA::PHYSX_SHAPE_MALE)
		pList = &m_vPhysxShapeList_Male;
	else if( type == CHARA::PHYSX_SHAPE_FEMALE)
		pList = &m_vPhysxShapeList_Female;
	else
		return nullptr;

	for( UINT32 i = 0; i < pList->size(); i++)
	{
		tagPRELOADEDPHYSICSHAPE * pFindInfo = pList->at( i);
		if( pFindInfo->_state == eAPS_NONE)
			return pFindInfo;
	}

	return nullptr;
}

CGameCharaManager::tagPRELOADEDPHYSICSHAPE * CGameCharaManager::__FindOwnedPhysXShape( CGameCharaBase * pOwner)
{
	I3ASSERT( pOwner != nullptr);

	i3::vector<tagPRELOADEDPHYSICSHAPE*> *pList;
	CHARA::PHYSX_SHAPE_TYPE shapeType = static_cast<CHARA::PHYSX_SHAPE_TYPE>(pOwner->getDBInfo()->GetCharaSexTypePHYSIX());

	if( shapeType == CHARA::PHYSX_SHAPE_MALE)
		pList = &m_vPhysxShapeList_Male;
	else if( shapeType == CHARA::PHYSX_SHAPE_FEMALE)
		pList = &m_vPhysxShapeList_Female;
	else
		return nullptr;

	for( UINT32 i = 0; i < pList->size(); i++)
	{
		tagPRELOADEDPHYSICSHAPE * pFindInfo = pList->at( i);
		if( pFindInfo->_pOwner == pOwner)
			return pFindInfo;
	}

	return nullptr;
}

void CGameCharaManager::__ReleasePhysXShape( void)
{
	for( UINT32 i = 0; i < m_vPhysxShapeList_Male.size(); i++)
	{
		tagPRELOADEDPHYSICSHAPE * pInfo = m_vPhysxShapeList_Male.at( i);

		for(INT32 j = 0; j < pInfo->_shapeCount; j++)
			I3_SAFE_RELEASE( pInfo->_pShapes[ j]);

		I3MEM_SAFE_FREE( pInfo->_pShapes);
	}
	m_vPhysxShapeList_Male.clear();

	for( UINT32 i = 0; i < m_vPhysxShapeList_Female.size(); i++)
	{
		tagPRELOADEDPHYSICSHAPE * pInfo = m_vPhysxShapeList_Female.at( i);

		for(INT32 j = 0; j < pInfo->_shapeCount; j++)
			I3_SAFE_RELEASE( pInfo->_pShapes[ j]);

		I3MEM_SAFE_FREE( pInfo->_pShapes);
	}
	m_vPhysxShapeList_Female.clear();
}

void CGameCharaManager::SetPhysXShape( CGameCharaBase * pOwner, CHARA_RES_ID type)
{
	INT32 i;

	I3ASSERT( pOwner != nullptr);

	//CHARA::PHYSX_SHAPE_TYPE physxType = CHARA::ResIDToShape( type);
	CHARA::PHYSX_SHAPE_TYPE physxType = static_cast<CHARA::PHYSX_SHAPE_TYPE>(pOwner->getDBInfo()->GetCharaSexTypePHYSIX());

	// 이전에 이미 등록을 했다. 재사용
	tagPRELOADEDPHYSICSHAPE * pFindInfo = __FindOwnedPhysXShape( pOwner);

	if( pFindInfo == nullptr)	// 사용된 적이 없으면 빈 슬롯에서 가져 오자.
		pFindInfo = __FindNotUsedPhysXShape( physxType);

	if( pFindInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "not enough physx shape!!");
		return;
	}

	i3Skeleton * pSkel = pOwner->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();

	for( i = 0; i < pFindInfo->_shapeCount; i++)
	{
		if( pFindInfo->_pShapes[ i] != nullptr)
		{	
			INT32 destIdx = pSkel->FindBoneByName( m_szShapeBoneName[physxType][ i]);
			if( destIdx != -1)
			{
				i3PhysixShapeSet * pShapeSet = pFindInfo->_pShapes[ i];
				pSkel->setShapeSet( destIdx, pShapeSet);
				pShapeSet->setEventReceiver( pOwner);
			}
		}
	}

	pFindInfo->_pOwner = pOwner;
	pFindInfo->_state = eAPS_USE;
}

void CGameCharaManager::FreePhysXShape( CGameCharaBase * pOwner)
{
	tagPRELOADEDPHYSICSHAPE * pFindInfo = __FindOwnedPhysXShape( pOwner);
	if( pFindInfo == nullptr)
		return;

	INT32 i;
	VEC3D pos;
	REAL32 x = (pOwner->getCharaInfo()->GetResType() == CHARA_RES_ID_ACID_POLE)?10.f:30.f;
	i3Vector::Set( &pos, x, g_pConfigEx->GetPhysix().HeightChara, (REAL32) pOwner->getCharaNetIndex() * 10.f );

	MATRIX mtx, mtxRoot;
	VEC3D dist, newPos;

	i3PhysixShapeSet * pRoot = nullptr;

	for( i = 0; i < pFindInfo->_shapeCount; i++)
	{
		if( pFindInfo->_pShapes[i] != nullptr)
		{
			if( pRoot == nullptr)
			{
				pRoot = pFindInfo->_pShapes[i];
				pRoot->getShapeGlobalPose( &mtxRoot);
			}

			pFindInfo->_pShapes[i]->getShapeGlobalPose( &mtx);

			i3Vector::Sub( &dist, i3Matrix::GetPos( &mtx), i3Matrix::GetPos( &mtxRoot));
			i3Vector::Add( &newPos, &pos, &dist);
			i3Matrix::SetPos( &mtx, &newPos);

			pFindInfo->_pShapes[i]->modifyDynamicState( I3_PHYSIX_STATE_KINEMATIC, 0);
			pFindInfo->_pShapes[i]->setShapeGlobalPose( &mtx);
			pFindInfo->_pShapes[i]->setEventReceiver( nullptr);
		}
	}

	pFindInfo->_pOwner = nullptr;
	pFindInfo->_state = eAPS_NONE;

	I3TRACE( "Free PhysX Shape %d\n", pOwner->getCharaNetIndex());
}

void CGameCharaManager::CreateDummyCharaInfo( T_ItemID charaID, bool bKeepResource)
{
	for( UINT32 i = 0; i < m_vDummyCharaInfo.size(); i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( i);
		if( pInfo->_TItemID[EQUIP::ePART_CHARA] == charaID)
		{	// 이미 등록되었다.
			return;
		}
	}

	{
		tagDUMMYCHARAINFO * pNewInfo = (tagDUMMYCHARAINFO*) i3MemAlloc( sizeof( tagDUMMYCHARAINFO));

		const CCharaInfo * pCharaInfo = g_pCharaInfoDataBase->GetCharaDBInfo( charaID);
		I3ASSERT( pCharaInfo != nullptr);

		for( INT32 parts = 0 ; parts < EQUIP::ePART_COUNT ; ++parts)
		{
			if( parts == EQUIP::ePART_CHARA)
				pNewInfo->_TItemID[EQUIP::ePART_CHARA] = charaID;
			else
				pNewInfo->_TItemID[parts] = pCharaInfo->GetPartsItemID(static_cast<EQUIP::ePART>(parts));
		}

		pNewInfo->_bKeepOriginal = bKeepResource;

		i3mem::FillZero( &pNewInfo->_resTeamRed, sizeof( pNewInfo->_resTeamRed));
		i3mem::FillZero( &pNewInfo->_resTeamBlue, sizeof( pNewInfo->_resTeamBlue));
		
		m_vDummyCharaInfo.push_back( pNewInfo);
	}
}

bool CGameCharaManager::UpdateLoadDummyChara( void)
{
	for( UINT32 i = 0; i < m_vDummyCharaInfo.size(); i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( i);

		if( pInfo->_resTeamRed._pBody != nullptr && pInfo->_resTeamBlue._pBody != nullptr )
			continue;

		if( m_pAvatarGen->isReadyToLoad() )
		{	// Start load
			m_pAvatarGen->Destroy();	// 초기화
			m_pAvatarGen->SetTeam( (pInfo->_resTeamRed._pBody == nullptr)?TEAM_RED:TEAM_BLUE );

			for(INT32 idx = 0 ; idx < EQUIP::ePART_COUNT; ++idx )
			{
				// itemid check
				if( idx == EQUIP::ePART_SKIN || idx == EQUIP::ePART_BERET )
					continue;

				m_pAvatarGen->SetPart( pInfo->_TItemID[idx], false);
			}
		}
		else
		{
			m_pAvatarGen->UpdateInThread( true);

			if( m_pAvatarGen->isReadyScene() )
			{
				// Resource를 Keep하자!!
				if( pInfo->_bKeepOriginal)
				{
					i3SceneGraphInfo * pSgInfo;
					for( UINT32 j = 0; j < EQUIP::ePART_COUNT; j++)
					{
						for( UINT32 k = 0; k < AVT_LOD_COUNT; k++)
						{
							pSgInfo = m_pAvatarGen->getSgInfo( (EQUIP::ePART) j, k);
							m_pAvatarGen->KeepDummyCharaResource( pSgInfo);
						}
					}
					for( INT32 j = 0; j < AVATAR::HEAD_TYPE_MAX; j++)
					{
						for( UINT32 k = 1; k < AVT_LOD_COUNT; k++)
						{
							pSgInfo = m_pAvatarGen->getHeadSgInfo( (AVATAR::HEAD_TYPE) j, k);
							m_pAvatarGen->KeepDummyCharaResource( pSgInfo);
						}
					}
				}

				// load complate
				i3Body * pBody = m_pAvatarGen->GetGeneratedBody( true);
				I3_MUST_ADDREF(pBody);
				m_pAvatarGen->BindMacroTexture( pBody, AvatarGen::TEX_LOAD_HIGH, true);

				if( m_pAvatarGen->GetTeam() == TEAM_RED)
					pInfo->_resTeamRed._pBody = pBody;
				else
					pInfo->_resTeamBlue._pBody = pBody;
			}
		}

		return false;
	}

	if( m_pAvatarGen != nullptr)
		m_pAvatarGen->Destroy();	// 초기화

	i3NamedResourceManager * pManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
	pManager->DeleteUnusedObjects();

	I3_SAFE_RELEASE( m_pAvatarGen);

	return true;
}

void CGameCharaManager::ReleaseDummyCharaInfo( T_ItemID charaID)
{
	for( UINT32 i = 0; i < m_vDummyCharaInfo.size(); i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( i);
		if( pInfo->_TItemID[EQUIP::ePART_CHARA] == charaID)
		{
			I3_MUST_RELEASE(pInfo->_resTeamRed._pBody);
			I3_MUST_RELEASE(pInfo->_resTeamBlue._pBody);
			I3MEM_SAFE_FREE( pInfo);

			m_vDummyCharaInfo.erase( m_vDummyCharaInfo.begin() + i);
			break;
		}
	}
}

i3Body * CGameCharaManager::GetDummyCharacterBody( T_ItemID charaID, TEAM_TYPE team)
{
	for( UINT32 i = 0; i < m_vDummyCharaInfo.size(); i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( i);
		if( pInfo->_TItemID[EQUIP::ePART_CHARA] == charaID)
		{
			if( team == TEAM_RED)
				return pInfo->_resTeamRed._pBody;
			else
				return pInfo->_resTeamBlue._pBody;
		}
	}

	I3PRINTLOG(I3LOG_FATAL,  "no instance character...%d", charaID);
	tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( 0);
	return pInfo->_resTeamRed._pBody;
}

void CGameCharaManager::SetDummyCharacterOption( bool bNormalMap, bool bSpecular)
{
	i3::vector<i3RenderAttr*> normalList;
	i3::vector<i3RenderAttr*> specularList;

	for( UINT32 i = 0; i <	m_vDummyCharaInfo.size(); i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( i);
		
		for( UINT32 j = 0; j < pInfo->_resTeamRed._pBody->getLODCount(); j++)
		{
			i3LOD * pLOD = pInfo->_resTeamRed._pBody->getLOD( j);
			i3Scene::FindAttrByExactType( pLOD->getShapeNode(), i3NormalMapEnableAttr::static_meta(), normalList);
			i3Scene::FindAttrByExactType( pLOD->getShapeNode(), i3SpecularMapEnableAttr::static_meta(), specularList);
		}

		for( UINT32 j = 0; j < pInfo->_resTeamBlue._pBody->getLODCount(); j++)
		{
			i3LOD * pLOD = pInfo->_resTeamBlue._pBody->getLOD( j);
			i3Scene::FindAttrByExactType( pLOD->getShapeNode(), i3NormalMapEnableAttr::static_meta(), normalList);
			i3Scene::FindAttrByExactType( pLOD->getShapeNode(), i3SpecularMapEnableAttr::static_meta(), specularList);
		}
	}

	for( UINT32 i = 0; i < normalList.size(); i++)
	{
		i3NormalMapEnableAttr * pAttr = (i3NormalMapEnableAttr*) normalList.at( i);
		pAttr->Set( bNormalMap);
	}

	for( UINT32 i = 0; i < specularList.size(); i++)
	{
		i3SpecularMapEnableAttr * pAttr = (i3SpecularMapEnableAttr*) specularList.at( i);
		pAttr->Set( bSpecular);
	}
}

INT32 CGameCharaManager::GetDummyCharaLoadProg( void)
{
	INT32 prog = 0;
	INT32 cnt = 0;

	for( UINT32 i = 0; i < m_vDummyCharaInfo.size(); i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at( i);

		if( pInfo->_resTeamRed._pBody != nullptr && pInfo->_resTeamBlue._pBody != nullptr)
			cnt++;
	}

	if( m_vDummyCharaInfo.size() > 0)
		prog = (INT32)((REAL32) cnt / (REAL32) m_vDummyCharaInfo.size() * 100.f);

	return prog;
}

bool CGameCharaManager::IsDummyCharaID( T_ItemID charaID)
{
	UINT32 cnt = m_vDummyCharaInfo.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		tagDUMMYCHARAINFO * pInfo = m_vDummyCharaInfo.at(i);
		if( pInfo->_TItemID[EQUIP::ePART_CHARA] == charaID)
		{
			return true;
		}
	}

	return false;
}

tagPRELOADANIMATION * CGameCharaManager::_FindPreloadAnim( const char * pszPath)
{

	UINT32 cnt = m_PreloadAnimQueue.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		tagPRELOADANIMATION * pInfo = m_PreloadAnimQueue.front();

		m_PreloadAnimQueue.pop();
		m_PreloadAnimQueue.push( pInfo);

		if( i3::generic_is_iequal( pInfo->_szPath, pszPath) )
			return pInfo;
	}

	return nullptr;
}

void CGameCharaManager::_FreePreloadAnimQueue( void)
{
	while( !m_PreloadAnimQueue.empty() )
	{
		tagPRELOADANIMATION * pInfo = m_PreloadAnimQueue.front();

		m_pPreloadAnimPool->Free( pInfo);

		m_PreloadAnimQueue.pop();
	}
}

void CGameCharaManager::LoadWeaponAnimation( i3ElementBase * pOwner, const char * pszPath)
{
	tagPRELOADANIMATION * pFindInfo = _FindPreloadAnim( pszPath);

	I3_MUST_ADDREF(pOwner);

	if( pFindInfo == nullptr)
	{
		tagPRELOADANIMATION * pNewInfo = reinterpret_cast<tagPRELOADANIMATION*>(m_pPreloadAnimPool->Alloc( sizeof( tagPRELOADANIMATION), __FILE__, __LINE__));

		I3ASSERT( pszPath != nullptr);
		I3ASSERT( i3::generic_string_size( pszPath) > 0);
		i3::safe_string_copy( pNewInfo->_szPath, pszPath, sizeof( pNewInfo->_szPath));

		pNewInfo->_OwnList.push_back( pOwner);

		pNewInfo->_State = PRELOAD_STATE_NONE;

		m_PreloadAnimQueue.push( pNewInfo);

		I3TRACE( "Preload Anim Queing - %s\n", pszPath);
	}
	else
	{
		pFindInfo->_OwnList.push_back( pOwner);

		I3TRACE( "Preload Anim reloading - %s\n", pszPath);
	}
}

void CGameCharaManager::UpdatePreloadAnim( void)
{
	if( !m_PreloadAnimQueue.empty())
	{
		BGLoadThread * pLoadThread = g_pFramework->GetBGLoadThread();
		if( pLoadThread != nullptr)
		{
			i3AnimationResManager * pManager = (i3AnimationResManager *) i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta());

			tagPRELOADANIMATION * pInfo = m_PreloadAnimQueue.front();
			switch( pInfo->_State)
			{
			case PRELOAD_STATE_NONE :
				{
					pLoadThread->SetLoadCmd( pInfo->_szPath);
					pInfo->_State = PRELOAD_STATE_LOADING;
				}
				break;

			case PRELOAD_STATE_LOADING :
				{
					i3GameRes * pRes = pLoadThread->GetRes();
					if( pRes != nullptr)
					{
						I3TRACE( "Preload Anim Complate - %s\n", pInfo->_szPath);
						I3ASSERT( i3::same_of<i3GameResAnimation*>(pRes));

						i3GameResAnimation * pAnimRes = static_cast<i3GameResAnimation*>(pRes);

						pManager->AddResource( pRes, pInfo->_szPath);
					
						// 참조하는 Obj에 Animation을 등록해주자.
						for( size_t j = 0; j < pInfo->_OwnList.size(); j++)
						{
							i3ElementBase * pObj = pInfo->_OwnList.at( j);
							if( i3::same_of<i3AIState*>(pObj))
							{
								((i3AIState*)pObj)->SetAnim( pAnimRes->getAnimation() );
							}

							I3_MUST_RELEASE(pObj);
						}

						I3_MUST_RELEASE(pRes);

						m_pPreloadAnimPool->Free( pInfo);

						m_PreloadAnimQueue.pop();
					}
				}
				break;
			}
		}
	}
}


i3SceneGraphInfo * CGameCharaManager::__FindFaceResource( UINT128 hash)
{
	i3::vector_map<UINT128,i3SceneGraphInfo*>::iterator it = m_MaskResourceMap.find(hash);
	if( it != m_MaskResourceMap.end())
		return it->second;

	return nullptr;
}

void CGameCharaManager::CreateFaceResource( void)
{
	INT32 i, j, t;
	i3::rc_wstring	wstrPostName;
	i3::string		strPostName;
	char szPath[ MAX_PATH];
	const char * pszAHT;

	INT32 count = g_pCharaInfoDataBase->GetInfoCount( EQUIP::ePART_CHARA);

	for( i = 0; i < count; i++)
	{
		CCharaInfo * pCharaInfo = (CCharaInfo*) g_pCharaInfoDataBase->GetInfo( EQUIP::ePART_CHARA, i);
		if( pCharaInfo == nullptr) continue;

		pCharaInfo->GetResPostName( wstrPostName);
		i3::utf16_to_mb(wstrPostName, strPostName);

		bool bDummyChara = false;
		if( CDI_RED_BULLS == pCharaInfo->GetITEMID() || CDI_ACID_POLE == pCharaInfo->GetITEMID()
			|| CDI_TARANTULA == pCharaInfo->GetITEMID() || CDI_KEENEYES == pCharaInfo->GetITEMID()
			|| CDI_GRAY == pCharaInfo->GetITEMID() || CDI_WOLF == pCharaInfo->GetITEMID())
		{
			bDummyChara = true;
		}

		const i3::fixed_string& strPart = AVATAR::getPartName( EQUIP::ePART_CHARA);

		for( t = 0; t < AVATAR::HEAD_TYPE_MAX; t++)
		{
			switch( t)
			{
			case 1 :  pszAHT = "Y_";	break;
			case 2 :  pszAHT = "Z_";	break;
			default : pszAHT = "X_";	break;
			}

			// face는 LOD1번부터
			for( j = 1; j < AVT_LOD_COUNT; j++)
			{
				i3::snprintf( szPath, sizeof(szPath), "Avatar/%s/%s/%s_%sLOD%d.i3s", strPart, strPostName, strPostName, pszAHT, j);

				UINT128 hash;
				
				i3::to_upper( szPath);
				i3MD5::Gen( (char*)szPath, i3::generic_string_size( szPath), (char *) &hash);

				if( __FindFaceResource( hash) != nullptr)
				{
					continue;
				}
								
				i3GameResSceneGraph * pRes = (i3GameResSceneGraph*) g_pFramework->GetResourceManager()->LoadResource( szPath, 0, true, false, false); 
			
				if(pRes == nullptr)
				{
					I3ASSERT(pRes != nullptr);
					I3PRINTLOG(I3LOG_NOTICE, "Face Resource File name : %s", szPath);
					continue;
				}

				i3SceneGraphInfo * pSgInfo = pRes->getSceneGraphInfo();

				// 아바타 폴더 내 없는 캐릭터는 SgInfo가 없습니다.
				if (pSgInfo == nullptr)
				{
					// M_M_ , F_M_ 4글자니까 상수로 합니다. 
					i3::string str_chara_name = strPostName.substr(4);
					I3PRINTLOG(I3LOG_NOTICE, "Check Chara File - %s, Type = %d, Num = %d", str_chara_name, t, j);
					continue;
				}

				m_MaskResourceMap.insert( i3::vector_map<UINT128,i3SceneGraphInfo*>::value_type( hash, pSgInfo));

				if( bDummyChara == false)
				{
					i3::vector<i3Node*> list;
					i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3AttrSet::static_meta(), list);
					for( UINT32 k = 0; k < list.size(); k++)
					{
						i3Node * pNode = list.at(k);
						pNode->AddFlag( I3_NODEFLAG_USER);
					}
				}
			}
		}
	}
}

i3SceneGraphInfo * CGameCharaManager::FindFaceResource( const char * pszName)
{
	UINT128 hash;
	char szPath[MAX_PATH];
	i3::safe_string_copy( szPath, pszName, sizeof( szPath));
	i3::to_upper( szPath);
	i3MD5::Gen( (char*)szPath, i3::generic_string_size( szPath), (char *) &hash);

	return __FindFaceResource( hash);
}

void CGameCharaManager::__CreateUpperAI( i3GameResChara * pRes, INT8 * pUpperAI, INT8 * pUpperDualAI)
{
	// 남캐
	i3Chara * pChara = pRes->getChara();

	i3AIContext * pAICtx = nullptr;
	char * pszAnimSet = "";
	const char * pszBody2Name = "";
	INT32 nWeaponNumMax;

	i3AIContext * pAIUpper = static_cast<i3AIContext*>(pChara->FindChildByName( "Upper", false));
	I3ASSERT( pAIUpper != nullptr);

	//	AnimSet
	for( INT32 nAnimCnt = 0; nAnimCnt < CHARA_VIEW_MAX; nAnimCnt++)
	{
		if( nAnimCnt == CHARA_VIEW_1PV)		pszAnimSet = "1PV";
		else								pszAnimSet = "3PV";

		i3AIContext * pAIAnimSet = static_cast<i3AIContext*>(pAIUpper->FindChildByName( pszAnimSet, false));
		I3ASSERT( pAIAnimSet != nullptr);

		//	Weapon Class
		for( INT32 nWeaponType = 1; nWeaponType < WEAPON_CLASS_COUNT; nWeaponType++)
		{
			const char * pszWeaponType = WEAPON::GetClassName( (WEAPON_CLASS_TYPE) nWeaponType);

			if( pszWeaponType == nullptr)
				continue;

			nWeaponNumMax = WEAPON::getItemCount( (WEAPON_CLASS_TYPE) nWeaponType);

			i3AIContext * pWeaponTypeCtx = static_cast<i3AIContext*>(pAIAnimSet->FindChildByName( pszWeaponType, false));
			if( pWeaponTypeCtx == nullptr)	continue;

			//	Weapon Num
			i3::rc_wstring	wstrFindWeapon;
			i3::string		strFindWeapon;

			for( INT32 nWeaponNum = 1; nWeaponNum < nWeaponNumMax; nWeaponNum++)
			{
				CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( (WEAPON_CLASS_TYPE) nWeaponType, nWeaponNum);
				if( pWeaponInfo == nullptr)
					continue;

				if( pWeaponInfo->AttachedSubWeapon() == true)	
					continue;		//	독립될 수 없는 LSS와 같은 익스텐션 전용 무기는 주무기 자체에 애니메이션이 있기 때문에 스킵한다.

			//	char szFindWeapon[ 256] = {0,};

				//	GetLinkedToCharaAI 항목을 무기 정보에 추가하여 FindWeapon을 대신합니다. komet
				 pWeaponInfo->GetLinkedToCharaAI(wstrFindWeapon);

				 i3::utf16_to_mb(wstrFindWeapon, strFindWeapon);

				 i3AIContext * pWeaponCtx = static_cast<i3AIContext*>(pWeaponTypeCtx->FindChildByName( strFindWeapon.c_str(), false));

				 if( pWeaponCtx == nullptr)
					 continue;

				//	Body2
				for( INT32 nBodyState = 0; nBodyState < CHARA_BODYUPPER_MAXCOUNT; nBodyState++)
				{
					pszBody2Name = WEAPON::GetBodyUpperName( (CHARA_ACTION_BODYUPPER) nBodyState, pWeaponInfo);
					if( pszBody2Name == nullptr)
						continue;

					pAICtx = static_cast<i3AIContext*>(pWeaponCtx->FindChildByName( pszBody2Name, false));
					if( pAICtx == nullptr) continue;
					
					//	AI Id
					for( INT32 id = 0; id < ID_UPPER_AI_MAX; id++)
					{	
						INT32 nRet = pAICtx->FindAIStateByName( g_szNameUpperAI[ id] );
						INT32 idx = getAIUpperIDIndex( (CHARA_VIEW_TYPE)nAnimCnt,
							(WEAPON_CLASS_TYPE)nWeaponType, nWeaponNum, (CHARA_ACTION_BODYUPPER)nBodyState, (ID_UPPER_AI)id);
						I3ASSERT( -2 < nRet && nRet < 128);
						pUpperAI[ idx] = static_cast<INT8>(nRet);
					}

					switch( nWeaponType )
					{
					case WEAPON_CLASS_DUALHANDGUN	:
					case WEAPON_CLASS_DUALSMG		:
					case WEAPON_CLASS_DUALSHOTGUN	:
						{
							for( INT32 id = 0; id < ID_UPPER_DUAL_AI_MAX; id++)
							{
								INT32 nRet = pAICtx->FindAIStateByName( g_szNameUpperDualAI[ id]);
								INT32 idx = getAIUpperDualIDIndex( (CHARA_VIEW_TYPE) nAnimCnt, nWeaponNum, (ID_UPPER_DUAL_AI) id);
								if( nRet != -1 && idx != -1)
								{
									I3ASSERT( -2 < nRet && nRet < 128);
									pUpperDualAI[ idx] = static_cast<INT8>(nRet);
								}
							}
						}
						break;
					default							: break;
					}
				}
			}
		}
	}
}

void CGameCharaManager::__CreateLowerAI( i3GameResChara * pRes)
{
	i3AIContext * pAICtx;
	char szAIName[256];

	i3Chara * pChara = pRes->getChara();

	for( INT32 i = 0; i < CHARA_BODYLOWER_MAXCOUNT; i++)
	{
		i3::safe_string_copy( szAIName, "Lower:", sizeof(szAIName) );

		switch( (CHARA_ACTION_BODYLOWER) i)
		{
		case CHARA_BODYLOWER_STAND:			i3::generic_string_cat( szAIName, "Stand");		break;
		case CHARA_BODYLOWER_CROUCH:		i3::generic_string_cat( szAIName, "Sit");			break;
		}

		//	AI Context
		pAICtx = pChara->FindAI( szAIName);
		
		if( pAICtx == nullptr)	continue;

		//	AI Id
		for( INT32 id = 0; id < ID_LOWER_AI_MAX; id++)
		{
			INT32 Idx = pAICtx->FindAIStateByName( g_szNameLowerAI[ id] );
			//EscapeMission : 인간인 경우는 하체 34번 JumpStartMove와 하체 35번 JumpEndMove값을 Assert처리 하지 않습니다.
			if(id !=34 && id!= 35)
			{
				I3ASSERT(Idx != -1);
			}

			I3ASSERT( Idx < 128);

			m_nAILowerId[ i][ id] = static_cast<INT8>(Idx);
		}
	}
}

void CGameCharaManager::__PreCreateAIID( void)
{
	//	게임에서 사용될 AIState의 ID를 찾아 미리 저장해둡니다. 나중에 AIState 변경은 이곳 배열에 저장된 ID를 사용하게 됩니다.

	m_uMaxWeaponCount = WEAPON::getItemCount(WEAPON_CLASS_ASSAULT);	// Assault가 가장 많다
	if( m_uMaxWeaponCount < WEAPON::getItemCount(WEAPON_CLASS_SMG))
		m_uMaxWeaponCount = WEAPON::getItemCount(WEAPON_CLASS_SMG);		// 혹시 모르니 SMG도 체크

	m_uDualWeaponCount = WEAPON::getItemCount(WEAPON_CLASS_DUALSMG);

	UINT32 iUpperIDCount = CHARA_VIEW_MAX * WEAPON_CLASS_COUNT * m_uMaxWeaponCount * CHARA_BODYUPPER_MAXCOUNT * ID_UPPER_AI_MAX;
	UINT32 iUpperDualIDCount = CHARA_VIEW_MAX * m_uDualWeaponCount * ID_UPPER_DUAL_AI_MAX;

	if( iUpperIDCount > 0)
	{
		m_pAIUpperID = new INT8 [iUpperIDCount];
		MEMDUMP_NEW( m_pAIUpperID, sizeof(INT8) * iUpperIDCount);
	}

	if( iUpperDualIDCount > 0)
	{
		m_pAIDualUpperID = new INT8 [iUpperDualIDCount];
		MEMDUMP_NEW( m_pAIDualUpperID, sizeof(INT8) * iUpperDualIDCount);
	}

	i3GameResChara * pRes = (i3GameResChara*)g_pCharaManager->QueryCharaResource( CHARA_RES_ID_ACID_POLE, true);
	I3ASSERT( pRes != nullptr);
	__CreateUpperAI( pRes, m_pAIUpperID, m_pAIDualUpperID);
	__CreateLowerAI( pRes);
}

void CGameCharaManager::__CreateAIContext( CHARA_RES_ID resID, UINT32 count)
{
	UINT32 i;

	i3GameResChara * pRes = (i3GameResChara*) QueryCharaResource( resID, true);
	I3ASSERT( pRes != nullptr);

	i3Chara * pSrcChara = pRes->getChara();
	I3ASSERT( pSrcChara != nullptr);

	for( i = 0; i < count; i++)
	{
		i3GameNode * pChild = pSrcChara->getFirstChild();

		while( pChild != nullptr)
		{
			i3GameNode * pNewChild = (i3GameNode *) i3GameNode::CreateClone( pChild, I3_COPY_INSTANCE);

			AddAIContext( pNewChild, resID);

			pChild = pChild->getNext();
		}
	}
}

INT32 CGameCharaManager::_ResID2AICtxSlot( CHARA_RES_ID resID)
{
	switch( resID)
	{
	case CHARA_RES_ID_D_FOX :
	case CHARA_RES_ID_LEOPARD :
	case CHARA_RES_ID_RED_BULLS :
	case CHARA_RES_ID_ACID_POLE :	return 0;
	case CHARA_RES_ID_TARANTULA :
	case CHARA_RES_ID_SWAT_FEMALE :
	case CHARA_RES_ID_CHOU :
	case CHARA_RES_ID_RICA :
	case CHARA_RES_ID_HIDE :
	case CHARA_RES_ID_VIPERRED : return 1;
	default :
		I3PRINTLOG(I3LOG_FATAL,  "지원하지 않습니다. 리스트 버퍼를 증가시켜서 작업진행해야 함.");
		break;
	}

	return 0;
}

i3GameNode * CGameCharaManager::FindFreeAIContext( const char * pszAIName, CHARA_RES_ID resID)
{
	i3GameNode * pNode;

	INT32 slot = _ResID2AICtxSlot( resID);

	for( UINT32 i = 0; i < m_listAIContext[slot].size(); i++)
	{
		pNode = m_listAIContext[slot].at( i);

		if( (pNode->isUsed() == false) )
		{
			if( i3::generic_is_iequal( pNode->GetName(), pszAIName) )
			{
				//I3TRACE( "Finded FreeAI %s\n", pNode->GetName());
				return pNode;
			}
		}
	}

	return nullptr;
}

void CGameCharaManager::AddAIContext( i3GameNode * pNode, CHARA_RES_ID resID)
{
	I3_MUST_ADDREF(pNode);

	INT32 slot = _ResID2AICtxSlot( resID);

	m_listAIContext[slot].push_back( pNode);
}

static void sRec_UnbindResourceAtAIContext( i3GameNode * pNode)
{
	if( i3::same_of<i3AIContext*>(pNode))
	{
		i3AIContext * pCtx = static_cast<i3AIContext*>(pNode);

		pCtx->SetEnable( false);
		pCtx->ResetCurrentAI();

		for( INT32 i = 0; i < pCtx->getAIStateCount(); i++)
		{
			pCtx->getAIState( i)->SetAnim( nullptr);
		}
	}

	i3GameNode * pChild = pNode->getFirstChild();
	while( pChild != nullptr)
	{
		sRec_UnbindResourceAtAIContext( pChild);

		pChild = pChild->getNext();
	}
}

void CGameCharaManager::UnuseAIContext( i3GameNode * pNode)
{
	if( i3::same_of<i3AIContext*>(pNode))
	{
		pNode->removeGNodeState( I3_GAMENODE_STATE_USED);

		i3GameNode * pParent = pNode->getParent();

		sRec_UnbindResourceAtAIContext( pNode);

		if( pParent != nullptr)
		{
			pParent->RemoveChild( pNode);
		}
	}
}

void CGameCharaManager::ClearAIContextList( void)
{
	UINT32 i, j;
	i3GameNode * pNode;

	for( i = 0; i < 2; i++)
	{
		for( j = 0; j < m_listAIContext[i].size(); j++)
		{
			pNode = m_listAIContext[i].at( j);
			I3_MUST_RELEASE(pNode);
		}

		m_listAIContext[i].clear();
	}
}

void CGameCharaManager::DumpAIContextList( CHARA_RES_ID resID)
{
	INT32 aiType = _ResID2AICtxSlot( resID);
	UINT32 cnt = m_listAIContext[ aiType].size();
	UINT32 charaCnt = m_CharaList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		bool bSame = false;
		i3GameNode * pNode = m_listAIContext[ aiType].at(i);
		
		for( UINT32 j = 0; j < charaCnt; j++)
		{
			CGameCharaBase * pChara = m_CharaList.at(j);
			i3GameNode * pChild = pChara->getFirstChild();
			if( pChild == pNode)
			{
				bSame = true;
				break;
			}
		}

		if( bSame == false)
		{
			I3PRINTLOG(I3LOG_NOTICE, "ai context(%s) attached to invalid chara.", pNode->GetName());
		}
	}
}

void CGameCharaManager::SetAiCharaData( INT32 idx, AI_CREATE_DATA* pAi_Create_Data)
{
	//서버에서 받은 데이터를 Set하기 전 케릭터를 지워야 하는지 먼저 검사 합니다.
	CGameCharaBase * pChara = getCharaByNetIdx( idx, CHARA::BUFFER_NEW_AI);
	
	//제압 모드 공룡(Dino) AI 만 사용 하는 함수 입니다.
	CHARA_RES_ID eNew_Id = CHARA::ItemID2ResID( pAi_Create_Data->_chara_item_id);

	if( pChara != nullptr && (pChara->getCharaInfo()->GetResType() != eNew_Id))
	{
		RemoveAiChara(idx);
	}


	///Ai케릭터 net info --------------------------------------------------------------
	NET_CHARA_INFO* pnet_chara_info = BattleSlotContext::i()->setNetCharaInfo( idx, CHARA::BUFFER_NEW_AI);
	I3ASSERT( pnet_chara_info != nullptr);

	//Ai케릭터 아이템 ID

	COMMON_PARTS_INFO commonInfo;
	// vv3 - check
	commonInfo.SetPartsItemID(pAi_Create_Data->_chara_item_id);
	pnet_chara_info->setCommonParts( &commonInfo);

	pnet_chara_info->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_ASSAULT);
	pnet_chara_info->setWeaponItemID( WEAPON_SLOT_SECONDARY, DEFAULT_WEAPON_HANDGUN);
	pnet_chara_info->setWeaponItemID( WEAPON_SLOT_MELEE, DEFAULT_WEAPON_KNIFE);
	pnet_chara_info->setWeaponItemID( WEAPON_SLOT_THROWING1, DEFAULT_WEAPON_THROWING1);
	pnet_chara_info->setWeaponItemID( WEAPON_SLOT_THROWING2, DEFAULT_WEAPON_THROWING2);


	//respawn pos 설정.
	pnet_chara_info->setRespawnPos( &pAi_Create_Data->_pos );
	pnet_chara_info->setRespawnIndex(idx);


	///Ai케릭터 info --------------------------------------------------------------
	GLOBAL_CHARA_INFO*	pchara_info = setCharaInfo( idx, CHARA::BUFFER_NEW_AI);
	CGameCharaBase*		pPlayer		= getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx() );

	if( pchara_info == nullptr || pPlayer == nullptr )
	{
		I3ASSERT( pchara_info == nullptr || pPlayer == nullptr );
	}
	else
	{					
		pchara_info->SetTeam( (pPlayer->getCharaTeam() & CHARACTER_TEAM_RED) ? CHARACTER_TEAM_BLUE : CHARACTER_TEAM_RED);
		pchara_info->SetHP( pAi_Create_Data->_hp);
	}

	//Ai케릭터 닉네임ad
#if defined(I3_DEBUG)

	UINT8 ui8Red = 255, ui8Green = 255, ui8Blue = 255;
	switch(pAi_Create_Data->grade_for_server)
	{
	case 1:	// Red
		ui8Red		= 255;
		ui8Green	= 0;
		ui8Blue		= 0;
		break;
	case 2:	// Green
		ui8Red		= 0;
		ui8Green	= 255;
		ui8Blue		= 0;
		break;
	case 3:	// Blue
		ui8Red		= 0;
		ui8Green	= 0;
		ui8Blue		= 255;
		break;
	case 4:	// Purple
		ui8Red		= 169;
		ui8Green	= 48;
		ui8Blue		= 194;
		break;
	case 5:	// Brown
		ui8Red		= 138;
		ui8Green	= 65;
		ui8Blue		= 21;
		break;
	}

	i3::rc_wstring wstrCharaName;
	i3::sprintf( wstrCharaName, L"Idx:%02d, {col:%d,%d,%d,255}Grade:%d{/col}", idx, ui8Red, ui8Green, ui8Blue, pAi_Create_Data->grade_for_server );

	BattleSlotContext::i()->setNickForSlot_BufferNewAI(idx, wstrCharaName);


#endif
}


void CGameCharaManager::CreateReserveAI( void)
{
#if defined( BUILD_RELEASE_QA_VERSION)
	for( UINT8 nType = 0; nType < aiu::RESERVE_MAX; ++nType)
		I3ASSERT( m_vecReserve[nType].size() == 0);

	I3TRACE( " >>>>>>>> Pre Create AI\n");
#endif

	m_FrequencyOfUse.reserve( aiu::RESERVE_MAX);
	for( UINT8 nType = 0; nType < aiu::RESERVE_MAX; ++nType)
	{
		m_vecReserve[nType].reserve( aiu::AVERAGE_RESERVE_COUNT);

		T_ItemID nItemID = aiu::ReverveTypeToItemID( nType);

		for( UINT8 cnt = 0; cnt < aiu::GetReserveCount( nType); ++cnt)
		{			
			CreateReserveAI( nItemID);
		}
		m_FrequencyOfUse.push_back( aiu::RESERVE_TYPE(nType));
	}

#if defined( BUILD_RELEASE_QA_VERSION)
	for( UINT8 nType = 0; nType < aiu::RESERVE_MAX; ++nType)
		I3TRACE( "  - Reserve Character(%s)	Create	Count  : %d\n", aiu::g_szReserveTypeName[nType], m_vecReserve[nType].size());

	I3TRACE( " <<<<<<<< Pre Create AI\n");
#endif
}

CGameCharaBase* CGameCharaManager::CreateReserveAI( T_ItemID nItemID)
{
	INT8 nType = aiu::ItemIDToReserveType( nItemID);

	//임시 SlotIdx
	static UINT8 ntemp_slot_idx = 0;	
	if( ++ntemp_slot_idx >= SLOT_MAX_COUNT) ntemp_slot_idx = 0;

	INT32 nMainSlot = BattleSlotContext::i()->getMainSlotIdx();
	setCharaInfo( ntemp_slot_idx, true)->SetTeam( !(nMainSlot%2) ? CHARACTER_TEAM_BLUE : CHARACTER_TEAM_RED);

	CHARA_RES_ID	ResID		= CHARA::ItemID2ResID( nItemID);
	CGameCharaBase*	pReserveAI	= AddAiChara( ntemp_slot_idx, ResID, true, CHARA::TYPE_DOMI_FULLBODY_AI);

	pReserveAI->setCharaInfo()->Reset();
	pReserveAI->setCharaInfo( nullptr );
	setCharaByNetIdx(ntemp_slot_idx, nullptr, true);

	pReserveAI->SetVisible( false);									//안보이게 함.
	pReserveAI->DetachScene();
	pReserveAI->setPaused( true);


	//저장.
	if (nType < 0 || nType >= aiu::RESERVE_MAX)
	{
		I3_BOUNDCHK(nType, aiu::RESERVE_MAX);
	}
	else
	{
		m_vecReserve[nType].push_back(pReserveAI);
	}

	//빼기.
	i3::vu::remove( m_CharaList, pReserveAI);

	return pReserveAI;
}

void CGameCharaManager::DestroyAIIReserveAI( void)
{//Free한 애들을 지워야 함.

	I3TRACE( " >>>>>>>> Destroy Pre AI Character\n");

	for( UINT32 nType = 0; nType < aiu::RESERVE_MAX; ++nType)
	{
		for( UINT32 i = 0; i < m_vecReserve[nType].size(); ++i)
		{
			CGameCharaBase * pReserveAI = m_vecReserve[nType].at(i);
			_DestroyReserveAI( pReserveAI);
		}

		if( m_vecReserve[nType].size() != 0)
		{
			I3TRACE( "  - Reserve Character(%s) Destroy	Count : %d\n", aiu::g_szReserveTypeName[nType], m_vecReserve[nType].size());
		}

		m_vecReserve[nType].clear();
	}

	I3TRACE( " <<<<<<<< Destroy Pre AI Character\n");
}

void CGameCharaManager::_DestroyReserveAI( CGameCharaBase* pReserveAI)
{
	//지우기 위해서 임시로 만듬.
	GLOBAL_CHARA_INFO sCharaInfo;

	CHARA_RES_ID	ResID	= CHARA::ItemID2ResID( aiu::ReverveTypeToItemID(aiu::RESERVE_DOMI_RAPTOR) );
	MakeAiCharaInfoForDomination( &sCharaInfo, -7, ResID, CHARA::TYPE_DOMI_FULLBODY_AI);

	if( m_pStage != nullptr)
	{
		if( m_pCharaLayer && m_pCharaLayer->getFirstChild())
		{
			m_pCharaLayer->RemoveChild( (i3GameNode*)pReserveAI);
		}
		else if( m_pStage->getFirstChild())
			m_pStage->RemoveChild( (i3GameNode *)pReserveAI);
	}

	pReserveAI->setCharaInfo( &sCharaInfo );

	pReserveAI->DetachScene();
	pReserveAI->ReturnInstance();

	pReserveAI->setCharaInfo( nullptr );
}

bool CGameCharaManager::ReturnToReserve( T_ItemID nItemID, CGameCharaBase* pReturn)
{
	INT8 nType = aiu::ItemIDToReserveType( nItemID);

	if( nType == aiu::RESERVE_NONE)
	{
		return false;
	}

	UINT32 nReserveTotal = 0;
	for( UINT8 type = 0; type < aiu::RESERVE_MAX; ++type)
		nReserveTotal += m_vecReserve[type].size();

	if( (nReserveTotal + m_CharaList.size()) > aiu::LIMITED_CHARACTER_COUNT)
	{
		for( UINT32 i = 0; i < m_FrequencyOfUse.size(); i++)
		{
			aiu::RESERVE_TYPE nLowType = m_FrequencyOfUse.at(i);

			if( m_vecReserve[nLowType].size() > 0)
			{
				CGameCharaBase* pReserveAI = m_vecReserve[nLowType].at(0);
				_DestroyReserveAI( pReserveAI);
				m_vecReserve[nLowType].erase(m_vecReserve[nLowType].begin());

				RESERVE_TRACE( "    -- Remove  AI(%s) ItemID : %d\n", aiu::g_szReserveTypeName[nLowType], nItemID);
				break;
			}
		}
	}

	RESERVE_TRACE( "  - Return AI(%s) To Reserve List... ItemID : %d\n", aiu::g_szReserveTypeName[nType], nItemID);

	pReturn->getMoveContext()->EnableController( false );

	//안보이도록 함.
	(pReturn)->SetVisible( false);
	(pReturn)->DetachScene();
	(pReturn)->setPaused( true);

	setCharaByNetIdx( pReturn->getCharaNetIndex(), nullptr, true );
	setCharaInfo( pReturn->getCharaNetIndex(), true )->Reset();
	pReturn->setCharaInfo(nullptr);


	//저장.
	m_vecReserve[nType].push_back(pReturn);

	//빼기.
	i3::vu::remove( m_CharaList, pReturn);

	return true;
}

CGameCharaBase* CGameCharaManager::RequestFreeAI( INT32 nSlotIdx, T_ItemID nItemID)
{
#if defined( BUILD_RELEASE_QA_VERSION)
	//ReserveDump();
#endif

	INT8 nType = aiu::ItemIDToReserveType( nItemID);

	//사용 한 AI Type을 뒤에 넣는다.
	for( UINT32 i = 0; i < m_FrequencyOfUse.size(); i++)
	{
		if( m_FrequencyOfUse.at(i) == aiu::RESERVE_TYPE(nType))
		{
			m_FrequencyOfUse.erase( m_FrequencyOfUse.begin() + i);
			break;
		}
	}
	m_FrequencyOfUse.push_back( aiu::RESERVE_TYPE(nType));

	RESERVE_TRACE("  -- Request Free AI(%s) Start\n", aiu::g_szReserveTypeName[nType]);

	if( nType == aiu::RESERVE_NONE)
	{
		RESERVE_TRACE( "  -- not Reserve Type....\n");

		return nullptr;
	}

	if( m_vecReserve[nType].size() > 0)
	{
		CGameCharaBase*	pReserveAI	= m_vecReserve[nType].at( 0);

		RESERVE_TRACE( "  -- Request AI ... ItemID : %d\n", nItemID);

		_PrepareAI( pReserveAI, nSlotIdx, nItemID);
 
		//빼기.
		m_vecReserve[nType].erase( m_vecReserve[nType].begin() );

		//저장.
		m_CharaList.push_back(pReserveAI);

		return (pReserveAI);
	}

	I3ASSERT( BattleSlotContext::i()->getMySlotIdx() != -1);
	CHARACTER_TEAM_TYPE teamType = getCharaInfo( BattleSlotContext::i()->getMySlotIdx())->GetTeam();
	setCharaInfo(nSlotIdx, true)->SetTeam( (teamType & CHARACTER_TEAM_RED) ? CHARACTER_TEAM_BLUE : CHARACTER_TEAM_RED);


	RESERVE_TRACE( "  -- ReserveList(%s) is empty ... Create AI ItemID : %d\n", aiu::g_szReserveTypeName[nType], nItemID);

	//Free 한 AI가 없음.
	CHARA_RES_ID	ResID		= CHARA::ItemID2ResID( nItemID);
	CGameCharaBase*	pNewAI		= AddAiChara( nSlotIdx, ResID, true, CHARA::TYPE_DOMI_FULLBODY_AI);

	return pNewAI;
}

void CGameCharaManager::_PrepareAI( CGameCharaBase* pReserveAI, INT32 nSlotIdx, T_ItemID nItemID)
{
	//생성 한것 처럼 세팅.		
	GLOBAL_CHARA_INFO*	pReserveCharaInfo = setCharaInfo(nSlotIdx, true);

	pReserveCharaInfo->SetAIToClient( true);
	CHARA_RES_ID ResID = CHARA::ItemID2ResID( nItemID);
	MakeAiCharaInfoForDomination( pReserveCharaInfo, nSlotIdx, ResID, CHARA::TYPE_DOMI_FULLBODY_AI);

	//Create시 해줘야 하는 것들.
	pReserveAI->setCharaInfo( pReserveCharaInfo );

	pReserveAI->SetVisible( true);			
	pReserveAI->AttachScene();
	pReserveAI->setPaused( false);

	setCharaByNetIdx( nSlotIdx, pReserveAI, true );
}


void CGameCharaManager::ReserveDump()
{
	I3TRACE("AI Character Reserve Dump >>>>>>>>>>>>>>>>>>>>>\n");

	UINT32 nTotal = 0;
	for( UINT8 type = 0; type < aiu::RESERVE_MAX; ++type)
	{
		I3TRACE( "  %s		Reserve Count : %d\n", aiu::g_szReserveTypeName[type], m_vecReserve[type].size());

		nTotal += m_vecReserve[type].size();
	}
	I3TRACE("     -- Total Reserve Character Count : %d\n", nTotal);
	I3TRACE("     -- Total Active Character Count : %d\n", m_CharaList.size());

	I3TRACE("AI Character Reserve Dump <<<<<<<<<<<<<<<<<<<<<\n");
}

void CGameCharaManager::LostDevice( void)
{
	if( m_pAvatarManager != nullptr)
		m_pAvatarManager->LostDevice();
}

void CGameCharaManager::Revive( i3RenderContext * pCtx)
{
	if( m_pAvatarManager != nullptr)
		m_pAvatarManager->Revive( pCtx);
}

PlayerController * CGameCharaManager::__FindFreePhysXController( void)
{
	PlayerController * pCtrl;
	for( UINT32 i = 0; i < m_PhysXControllerList.size(); i++)
	{
		pCtrl = m_PhysXControllerList.at(i);
		
		if( pCtrl->getOwner() == nullptr)
			return pCtrl;
	}

	return nullptr;
}

INT32 CGameCharaManager::__FindPhysXController( PlayerController * pCtrl)
{
	PlayerController * pSrc;
	for( UINT32 i = 0; i < m_PhysXControllerList.size(); i++)
	{
		pSrc = m_PhysXControllerList.at(i);
		if( pSrc == pCtrl)
			return i;
	}

	return -1;
}

PlayerController * CGameCharaManager::AllocPhysXController( CGameCharaBase * pOwner)
{
	PlayerController * pCtrl = __FindFreePhysXController();

	if( pCtrl == nullptr)
	{
		pCtrl = PlayerController::new_object();
		m_PhysXControllerList.push_back( pCtrl);
	}

	return pCtrl;
}

void CGameCharaManager::FreePhysXController( PlayerController * pCtrl)
{
	INT32 findIdx = __FindPhysXController( pCtrl);
	if( findIdx != -1)
	{
		pCtrl->setOwner( nullptr);
	}
}

void CGameCharaManager::AfterLoadAIChara( INT32 idx, AI_BIND_DATA * pAIBind)
{
	if( (g_pFramework == nullptr) ||
		(g_pFramework->IsBattleStage() == false) )
		return;

	if( BattleServerContext::i()->IsP2PActivate() == false)
		return;

	I3ASSERT( idx != BattleSlotContext::i()->GetMySlotIdxEx() );

	// vv3 - check
	const NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->getNetCharaInfo( idx, CHARA::Type2BufferType( BattleSlotContext::i()->getCharaType( idx)));
	CHARA_RES_ID CharaResID = CHARA::ItemID2ResID( pCharaInfo->getCharaParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));

	CGameCharaBase * pChara = nullptr;
	I3ASSERT( BattleSlotContext::i()->getCharaType( idx) == CHARA::TYPE_AI);

	pChara = g_pCharaManager->AddAiChara( idx, CharaResID, true, CHARA::TYPE_AI); 
	I3ASSERT( pChara != nullptr);
	pChara->BindAI( pAIBind);

	BattleSlotContext::i()->setSlotState( idx, SLOT_STATE_BATTLE);
}

void CGameCharaManager::setCharaAutoTeamChange( INT32 idx, bool bAI)
{
	tagCharaData * pData = __getCharaData( idx, bAI);

	if( pData != nullptr)
	{
		CHARACTER_TEAM_TYPE team = getCharaChangeTeam(idx, pData->_CharaInfo);
		pData->_CharaInfo.SetTeam(team);
	}
}

void CGameCharaManager::setCharaTeamAutoChange( INT32 idx, bool change, bool bAI)
{
	tagCharaData * pData = __getCharaData( idx, bAI);
	
	if( pData != nullptr)
	{
		CHARACTER_TEAM_TYPE team = getCharaChangeTeam(idx, pData->_CharaInfo);
		pData->_TeamChange = change;
		pData->_CharaInfo.SetTeam(team);
	}
}

CHARACTER_TEAM_TYPE CGameCharaManager::getCharaChangeTeam(INT32 idx, const GLOBAL_CHARA_INFO & info)
{
	CHARACTER_TEAM_TYPE team = CHARACTER_TEAM_NONE;

	if ((idx % 2) == 0)	team = CHARACTER_TEAM_RED;
	else				team = CHARACTER_TEAM_BLUE;

	// 공수교대가 진행되있으면, 팀을 전환시켜줍니다.
	if(MyRoomInfoContext::i()->IsAutoTeamChangeableRoomState())
	{
		if(ScoreContext::i()->IsAutoTeamChangeExecuted())
		{
			if((idx % 2) == 0) team = CHARACTER_TEAM_BLUE;
			else			   team = CHARACTER_TEAM_RED;
		}
	}

	return team;
}

void CGameCharaManager::ExecuteRespawnCheatkey()
{
	// 리스폰 치트키
	if (!m_respawnCheatkey.empty())
	{
		i3::string keyword;
		i3::vector<i3::string> params;

		if (Cheatkey::ParseCommand(m_respawnCheatkey.c_str(), keyword, params))
		{
			i3::shared_ptr<Cheatkey::ICommand> command = 
				Cheatkey::CreateCommand(keyword);

			command->operator()(params);
		}

		m_respawnCheatkey.clear();
	}
}

void CGameCharaManager::SetXrayState(CGameCharaBase* pCharaBase, bool bState)
{
	i3XrayScene* pXrayScene = g_pFramework->getSgContext()->GetXrayScene();
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if (!UserInfoContext::i()->IsGM_Observer()) return;
	if (!pFramework) return;
	if (!pXrayScene) return;

	if (pCharaBase)
	{
		i3XrayNode* pXrayNode = pXrayScene->FindXrayNode(pCharaBase->GetNode());

		//죽었을때는 Xray를 켜도 켜지지 않아야한다
		if (pCharaBase->isCharaStateMask(CHARA_STATE_DEATH) && bState)
			return;

		if (pXrayNode)
		{
			if (!bState) pCharaBase->getMaterialContext()->GetZWriteAttr()->Set(true);
			else		 pCharaBase->getMaterialContext()->GetZWriteAttr()->Set(false);

			pXrayNode->setTeam(pCharaBase->getCharaTeam());
			pXrayNode->SetEnable(bState);
		}
	}
	else
	{
		if (pFramework->getSgContext()->GetXrayScene()->IsXrayState())
		{
			for (INT32 i = 0; i < (INT32)m_CharaList.size(); i++)
			{
				CGameCharaBase* pChara = m_CharaList[i];
				if (pChara == nullptr) return;

				
				i3XrayNode* pXrayNode = pXrayScene->FindXrayNode(pChara->GetNode());

				if (!pXrayNode) continue;

				pChara->getMaterialContext()->GetZWriteAttr()->Set(true);

				pXrayNode->SetEnable(false);
			}
		}
		else
		{
			for (INT32 i = 0; i < (INT32)m_CharaList.size(); i++)
			{
				CGameCharaBase* pChara = m_CharaList[i];
				if (pChara == nullptr) return;

				i3XrayNode* pXrayNode = pXrayScene->FindXrayNode(pChara->GetNode());
				if (!pXrayNode ||
					pChara->isCharaStateMask(CHARA_STATE_DEATH)) continue;

				if (CGameCameraManager::i()->getTargetCharacter() == pChara &&
					!i3::same_of<CGameCamera_FLY*>(CGameCameraManager::i()->GetCurrentCamera()))
					continue;

				pChara->getMaterialContext()->GetZWriteAttr()->Set(false);

				
				pXrayNode->setTeam(pChara->getCharaTeam());
				pXrayNode->SetEnable(true);
			}
		}

		pFramework->getSgContext()->GetXrayScene()->ToggleXrayState();
	}
}

void CGameCharaManager::OutLineUpdate()
{
	if (CGameCameraManager::i())
	{
		CGameCamera* pCam = CGameCameraManager::i()->GetCurrentCamera();
		if (!pCam) return;

		i3Framework * pFramework = i3Framework::getCurrentFramework();
		if (!pFramework) return;

		i3XrayScene* pXrayScene = g_pFramework->getSgContext()->GetXrayScene();
		if (!pXrayScene) return;

		if (!UserInfoContext::i()->IsGM_Observer()) return;

		for (INT32 i = 0; i < (INT32)m_CharaList.size(); i++)
		{
			CGameCharaBase* pChara = m_CharaList[i];

			if (pChara == nullptr) return;

			VEC3D vec;
			i3Vector::Sub(&vec, i3Matrix::GetPos(pChara->GetMatrix()), i3Matrix::GetPos(pCam->getCamMatrix()));

			REAL32 dist = i3Vector::Length(&vec);

			i3XrayNode* pXrayNode = pXrayScene->FindXrayNode(pChara->GetNode());
			if (pXrayNode)
			{
				pXrayNode->setWindowSize((REAL32)pFramework->GetViewer()->GetViewWidth(), (REAL32)pFramework->GetViewer()->GetViewHeight());
				pXrayNode->setTeam(pChara->getCharaTeam());
				pXrayNode->OnUpdate(dist);
			}
		}
	}
}
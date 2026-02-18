#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaBase.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoContexts.h"
#include "PBLog.h"
#include "GlobalFunc.h"
#include "StageObject/ObjectWithWeapon.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "Camera/GameCamera_OnObject.h"
#include "Weapon/WeaponOnObject.h"
#include "Character/InputCtrlAI_ForHuman.h"
#include "GameStateMgr.h"
#include "BattleHud.h"
#include "CommunityContext.h"

#include "../Mode/Domination/DomiUserSkillMgr.h"
#include "../Mode/Domination/DomiSkillProperties.h"

#include "StageBattle/AIModeContext.h"
#include "BattleServerContext.h"
#include "UserInfoContext.h"
#include "AssistContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "Weapon/WeaponItemID.h"

#include "i3Base/string/ext/format_string.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "UI/UIBattleFrame.h"
#include "UI/UIHudObserver.h"

#include "Designation/DesigApplyInfo.h"

#define CHARA_BASE_LOG

#if 0
#define CASHITEM_TRACE	I3TRACE
#else
#define CASHITEM_TRACE	__noop
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//					static Controls
//	활성화할 경우 캐릭터의 State가 DebugOutput됩니다.

// static
i3BlendEnableAttr* CGameCharaBase::m_spBlendEnableAttr = nullptr;
i3BlendModeAttr* CGameCharaBase::m_spBlendModeAttr = nullptr;

I3_CLASS_INSTANCE( CGameCharaBase);

CGameCharaBase::CGameCharaBase( void)
{
	_ResetCharaStateMask();
	
	for( INT32 i = 0; i < AVT_LOD_COUNT; i++)
	{
		m_arClanMarkAttrSet[i] = nullptr;
		m_arClanMarkTexBindAttr[i] = nullptr;
	}

	for( INT32 i = 0; i < CHARA::eABILITY_COUNT; i++)
		m_rTotalAbility[ i] = 0.f;

	for (INT32 i = 0; i < CHARA::eDAMAGEPART_COUNT; i++)
		m_rDefenceRate[i] = 0.f;

	addCharaFlag( CHARA_FLAG_ENABLE_EFFECT);

	CreateClanMarkTex();
}

CGameCharaBase::~CGameCharaBase( void)
{
	Destroy();

	// texture
	I3_SAFE_RELEASE( m_pPBMarkTexture);
	I3_SAFE_RELEASE( m_pClanMarkTexture);
}

void CGameCharaBase::Destroy( void)
{
	i3GameNode * pChild = getFirstChild();
	i3GameNode * pNext;

	while( pChild )
	{
		pNext = pChild->getNext();
		g_pCharaManager->UnuseAIContext( pChild);
		pChild = pNext;
	}

	I3_SAFE_RELEASE( m_pWeaponContext);
	I3_SAFE_RELEASE( m_pUpdateContext);
	I3_SAFE_RELEASE( m_pInputContext);
	I3_SAFE_RELEASE( m_pActionContext);
	I3_SAFE_RELEASE( m_pAnimContext);
	I3_SAFE_RELEASE( m_pMoveContext);
	I3_SAFE_RELEASE( m_pMaterialContext);
	I3_SAFE_RELEASE( m_pEquipContext);
	I3_SAFE_RELEASE( m_pCameraContext);
	I3_SAFE_RELEASE( m_pBoneContext);
	I3_SAFE_RELEASE( m_pSoundContext);
	I3_SAFE_RELEASE( m_pCollisionContext);
	I3_SAFE_RELEASE( m_pHUDContext);
	I3_SAFE_RELEASE( m_pMultiWeaponContext);
}

void CGameCharaBase::OnInitVariable()
{	
}

REAL32 CGameCharaBase::getDamageElapseTime( void) const
{
	return getHUDContext()->getDamageElapseTime();
}

REAL32 CGameCharaBase::getDeathTime( void ) const
{
	CGameCharaUpdateContext*	pUpdateContext	= getUpdateContext();

	if( pUpdateContext )
		return pUpdateContext->getDeathTime();

	return 0.0f;
}


void CGameCharaBase::WarpChara(VEC3D* pPos)
{
	// 점퍼 기능 해당 위치로 옮깁니다.
	getHUDContext()->ScreenEffect_Jumper( 0.3f);
	forceSetPos(pPos);
}

void CGameCharaBase::ChangeControl_View( CHARA_VIEW_TYPE viewType)
{
	m_oldViewType = m_ViewType;
	m_ViewType = viewType;

	if( m_ViewType == CHARA_VIEW_1PV )
	{
		if(m_pBoneContext)		m_pBoneContext->OnChange1PV();
		if(m_pWeaponContext)	m_pWeaponContext->OnChange1PV();
		if(m_pUpdateContext)	m_pUpdateContext->OnChange1PV();
		if(m_pActionContext)	m_pActionContext->OnChange1PV();
		if(m_pAnimContext)		m_pAnimContext->OnChange1PV();
		if(m_pMaterialContext)	m_pMaterialContext->OnChange1PV();
		if(m_pInputContext)		m_pInputContext->OnChange1PV();
		if(m_pMoveContext)		m_pMoveContext->OnChange1PV();
		if(m_pEquipContext)		m_pEquipContext->OnChange1PV();
		if(m_pSoundContext)		m_pSoundContext->OnChange1PV();
		if(m_pCollisionContext) m_pCollisionContext->OnChange1PV();
		if(m_pCameraContext)	m_pCameraContext->OnChange1PV();
		if(m_pHUDContext)		m_pHUDContext->OnChange1PV();
	
		// LOD 단계를 0(1인칭용)으로 세팅
		getSceneObject()->GetBody()->setCurrentLevel( 0);	
//		getSceneObject()->GetBody()->AddFlag(I3_NODEFLAG_MUSTBEPROCESS, true);
	}
	else
	{
		if(m_pBoneContext)		m_pBoneContext->OnChange3PV();
		if(m_pWeaponContext)	m_pWeaponContext->OnChange3PV();
		if(m_pUpdateContext)	m_pUpdateContext->OnChange3PV();
		if(m_pActionContext)	m_pActionContext->OnChange3PV();
		if(m_pAnimContext)		m_pAnimContext->OnChange3PV();
		if(m_pMaterialContext)	m_pMaterialContext->OnChange3PV();
		if(m_pInputContext)		m_pInputContext->OnChange3PV();
		if(m_pMoveContext)		m_pMoveContext->OnChange3PV();
		if(m_pEquipContext)		m_pEquipContext->OnChange3PV();
		if(m_pSoundContext)		m_pSoundContext->OnChange3PV();
		if(m_pCollisionContext) m_pCollisionContext->OnChange3PV();
		if(m_pCameraContext)	m_pCameraContext->OnChange3PV();
		if(m_pHUDContext)		m_pHUDContext->OnChange3PV();
	}
}

void CGameCharaBase::_SetControlsByCharaType( void)
{
	I3ASSERT( m_pCharaInfo != nullptr);

	switch( m_pCharaInfo->GetType())
	{
	case CHARA::TYPE_PLAYER :
	case CHARA::TYPE_FIRST_VIEW_PLAYER :
		{
			ChangeControl_View( CHARA_VIEW_1PV);
			ChangeControl_Local();
		}
		break;

	case CHARA::TYPE_DOMI_FULLBODY_AI:		//제압 미션 AI 케릭터 타입은 네트워크 케릭터로 동작
		{
			ChangeControl_View( CHARA_VIEW_3PV);
			ChangeControl_Fullbody();
		}
		break;

	case CHARA::TYPE_PLAYER_NET :
		{
			ChangeControl_View( CHARA_VIEW_3PV);
			ChangeControl_Network();
		}
		break;

	case CHARA::TYPE_NPC :
		{
			ChangeControl_View( CHARA_VIEW_3PV);
			ChangeControl_NPC();
		}
		break;

	case CHARA::TYPE_PROFILE :
		{
			ChangeControl_View( CHARA_VIEW_3PV);
		}
		break;

	case CHARA::TYPE_AI :
		{
			ChangeControl_View( CHARA_VIEW_3PV);
			ChangeControl_AI();
		}
		break;

	default :
		I3PRINTLOG(I3LOG_FATAL,  "Unknown chara type!!!");
		break;
	}
}

const char * CGameCharaBase::_GetWeaponBodyUpperName( CHARA_ACTION_BODYUPPER bodyUpper, const CWeaponInfo * pWeaponInfo)
{
	switch( bodyUpper)
	{
	case CHARA_BODYUPPER_ATTACKIDLE:		return "AttackIdle";
	case CHARA_BODYUPPER_ZOOM:
		if( pWeaponInfo->isAnimZoomType() ) //	Zoom 가능한 무기만 찾도록 한다.
			return "Zoom";
		break;
	case CHARA_BODYUPPER_GRENADE :
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_GLAUNCHER ||
			pWeaponInfo->getExtensionType(1) == WEAPON::EXT_GLAUNCHER)
		{
			return "Grenade";
		}
		break;
	case CHARA_BODYUPPER_SHOTGUN:
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_SHOTGUN_LAUNCHER ||
			pWeaponInfo->getExtensionType(1) == WEAPON::EXT_SHOTGUN_LAUNCHER)
		{
			return "ShotGun";
		}
		break;
	case CHARA_BODYUPPER_KNIFE :
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_BAYONET ||
			pWeaponInfo->getExtensionType(1) == WEAPON::EXT_BAYONET)
		{
			return "Knife";
		}
		break;

	case	CHARA_BODYUPPER_CROSSFIRE :
		if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_CROSSFIRE )
		{
			return "CrossIdle";
		}
		break;
	}

	return nullptr;
}

void CGameCharaBase::_PrepareCommonAI( void)
{
	i3AIContext * pAICtx = FindAI( "Common");

	if( pAICtx != nullptr)
	{
		m_pAnimContext->setAICommon( pAICtx);

		for( INT32 i = 0; i < ID_COMMON_AI_MAX; i++)
		{
			INT32 iRet = pAICtx->FindAIStateByName( g_szNameCommonAI[ i] );

			m_pAnimContext->setAICommonID( i, iRet);
		}
	}
}


#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
#define _SET_RESNAME(s)		SetName(s)
#else
#define _SET_RESNAME(s)		__noop
#endif

void CGameCharaBase::Create( GLOBAL_CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
{
	m_pCharaInfo = pCharaInfo;

	CreateFirst( pDBInfo);
	CreateLater( pDBInfo);
}

static void sRecCopyGameNode( i3GameNode * pSrc, i3GameNode * pDest)
{
	i3GameNode * pNew;
	i3GameNode * pChild = pSrc->getFirstChild();
	
	pNew = pDest->getFirstChild();

	while( pChild != nullptr)
	{
		if( ( pChild->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
		{
			pChild->CopyTo( pNew, I3_COPY_INSTANCE);
			sRecCopyGameNode( pChild, pNew);
		}

		pChild = pChild->getNext();

		if( pChild != nullptr && pNew->getNext() == nullptr)
		{
			i3GameNode * pCreatedNode = (i3GameNode *) i3GameNode::CreateClone( pChild, I3_COPY_INSTANCE);
			I3_GAMENODE_ADDCHILD( pNew, pCreatedNode);
			return;
		}

		pNew = pNew->getNext();
	}
}

void CGameCharaBase::CreateFirst( const CCharaInfo * pDBInfo, bool bInLobby)
{
	m_EnableStateMask = 0;
	m_CharaFlag = 0;
	m_bReturnedInstance = false;

	Destroy();
	
	I3ASSERT( pDBInfo != nullptr);
	m_pDBInfo = const_cast<CCharaInfo*>(pDBInfo);

	// AnimContext는 먼저 생성해주어야 합니다.
	{
		// 2ms
		m_pAnimContext = CGameCharaAnimContext::new_object();
		I3ASSERT( m_pAnimContext != nullptr);
		m_pAnimContext->OnCreate( this);
	}

	I3ASSERT( m_pCharaInfo != nullptr);

	i3GameResChara * pRes = (i3GameResChara*)g_pCharaManager->QueryCharaResource( getCharaInfo()->GetResType());
	I3ASSERT( pRes != nullptr);
	
	{
		i3GameNode * pChild, * pNewChild;

		RemoveAllChild();

		{ // 18.5ms
			i3Chara * pSrcChara = pRes->getChara();
			I3ASSERT( pSrcChara != nullptr);

			pChild = pSrcChara->getFirstChild();

			while( pChild != nullptr)
			{
				pNewChild = g_pCharaManager->FindFreeAIContext( pChild->GetName(), getCharaInfo()->GetResType());	// 버퍼에서 가져온다.
				if( pNewChild == nullptr)
				{	// 없다면 클론을 만들자. 하지만 이경우가 나올수는 없다..
					// 나온다면 버퍼가 모질라는 경우!!!!!!!
					pNewChild = (i3GameNode *) i3GameNode::CreateClone( pChild, I3_COPY_INSTANCE);
					g_pCharaManager->AddAIContext( pNewChild, getCharaInfo()->GetResType());
					I3PRINTLOG(I3LOG_FATAL,  "not enough buf!!");	// 이 경우 발생 원인 찾아봐야 한다!!
				}

				pNewChild->addGNodeState( I3_GAMENODE_STATE_USED);

				I3_GAMENODE_ADDCHILD( this, pNewChild);

				pChild = pChild->getNext();
			}

			pSrcChara->CopyTo( this, I3_COPY_INSTANCE);
		}
		
		// 0.1ms
		i3Chara::OnInitAfterLoad();
	}

	// 0.06ms
	OnBindResource();

	// 성별 체크
	// 해당 플래그로 판별하는 여자 남자는 전부 본, 컬리젼, 애니메이션에 관련된것만 사용해야한다.
	if( m_pDBInfo->GetCharaSexTypePHYSIX() == CHARA::eSEX_TYPE_FEMALE)
		addCharaFlag(CHARA_FLAG_IS_FEMALE);
	else
		removeCharaFlag( CHARA_FLAG_IS_FEMALE);

	// 해당 모델링의 AttrSet을 찾아 리스트에 넣습니다.
	// 0.1ms
	//if( bInLobby == false)
	{
		m_pMaterialContext = CGameCharaMaterialContext::new_object();
		m_pMaterialContext->OnCreate(this);
	}

	// 본의 인덱스 리스트 작성. 리소스 로딩 후에 호출
	m_pAnimContext->BindBoneList();

	{
		// 0.39ms
		InitClanMark();

		// 0.28ms
		m_pWeaponContext = CGameCharaWeaponContext::new_object();
		I3ASSERT( m_pWeaponContext != nullptr);
		m_pWeaponContext->OnCreate( this);

		// 0.01ms
		m_pEquipContext = CGameCharaEquipContext::new_object();
		I3ASSERT( m_pEquipContext != nullptr);
		m_pEquipContext->OnCreate(  this);

	}

	//	임의로 변경하기 쉽게 하기 위해 강제로 LOD Slope값을 지정합니다. (기존에 CharaTool에서 지정한 Slope값은 사용하지 않습니다. komet)
	getSceneObject()->GetBody()->setSlope( CHARA_LOD_SLOPE);

	// LOD(0)은 shadow가 나오지 않아야 한다.
	i3LOD * pLOD = getSceneObject()->GetBody()->getLOD( 0);
	if( bInLobby)
		pLOD->getShapeNode()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	else
		pLOD->getShapeNode()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER );
//	getSceneObject()->GetBody()->AddFlag( I3_NODEFLAG_MUSTBEPROCESS, true);
	
	m_pUpdateContext = CGameCharaUpdateContext::new_object();
	I3ASSERT( m_pUpdateContext != nullptr);
	m_pUpdateContext->OnCreate( this);

	m_pActionContext = CGameCharaActionContext::new_object();
	I3ASSERT( m_pActionContext != nullptr);
	m_pActionContext->OnCreate( this);

	m_pMoveContext = CGameCharaMoveContext::new_object();
	I3ASSERT( m_pMoveContext != nullptr);
	m_pMoveContext->OnCreate(  this);

	m_pInputContext = CGameCharaInputContext::new_object();
	I3ASSERT( m_pInputContext != nullptr);
	m_pInputContext->OnCreate(  this);

	m_pHUDContext = CGameCharaHUDContext::new_object();
	I3ASSERT( m_pHUDContext != nullptr);
	m_pHUDContext->OnCreate( this);

	m_pCameraContext = CGameCharaCameraContext::new_object();
	I3ASSERT( m_pCameraContext != nullptr);
	m_pCameraContext->OnCreate(  this);

	m_pBoneContext = CGameCharaBoneContext::new_object();
	I3ASSERT( m_pBoneContext != nullptr);
	m_pBoneContext->OnCreate(  this);

	m_pSoundContext = CGameCharaSoundContext::new_object();
	I3ASSERT( m_pSoundContext != nullptr);
	m_pSoundContext->OnCreate(  this);

	m_pCollisionContext = CGameCharaCollisionContext::new_object();
	I3ASSERT( m_pCollisionContext != nullptr);
	m_pCollisionContext->OnCreate(  this);

	m_pMultiWeaponContext = CGameCharaMultiWeaponContext::new_object();
	I3ASSERT( m_pMultiWeaponContext != nullptr);
	m_pMultiWeaponContext->OnCreate( this);

	// 0.5ms
	{
		_SetControlsByCharaType();

		// Update Timing을 조정하기 위해, 수동으로 Animation, World-Transform 등을 하도록 한다.
		{
			// Animation Update와 World-Space Cache Matrix에 대한 시간차를
			// 없애기 위해, 수동으로 Update를 한다.
			if( BattleServerContext::i()->IsP2PActivate() )
			{
				getSceneObject()->GetBody()->addStyle( I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM);
				getSceneObject()->AddFlag( I3_NODEFLAG_MANUAL_UPDATE);
			}

			// 이 때, Animation Update와 World-Space Transform 사이에 LOD 계산에서
			// Current Level이 다를 수 있기 때문에...
			// 수작업으로 LOD를 계산한다.
			getSceneObject()->GetBody()->removeStyle( I3BS_LOD_AUTO);
		}

		ResetChara();		//	최종 클래스에서 불러야 한다.
	}

	_PresetResName();
}

void CGameCharaBase::_PresetResName( void)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	switch( m_pCharaInfo->GetResType())
	{ 
	case CHARA_RES_ID_RED_BULLS		:
	case CHARA_RES_ID_ACID_POLE		:	_SET_RESNAME("CHARA_RES_ID_ACID_POLE");		break;
	case CHARA_RES_ID_SWAT_FEMALE	:	_SET_RESNAME("CHARA_RES_ID_SWAT_FEMALE");	break;

	case CHARA_RES_ID_DINO_RAPTOR		:	_SET_RESNAME("CHARA_RES_ID_DINO_RAPTOR");	break;
	case CHARA_RES_ID_DINO_TREX			:	_SET_RESNAME("CHARA_RES_ID_DINO_TREX");		break;
	case CHARA_RES_ID_DINO_STING		:	_SET_RESNAME("CHARA_RES_ID_DINO_STING");	break;
	case CHARA_RES_ID_DINO_TANK			:	_SET_RESNAME("CHARA_RES_ID_DINO_TANK");		break;
	case CHARA_RES_ID_DINO_ACID			:	_SET_RESNAME("CHARA_RES_ID_DINO_ACID");		break;
	case CHARA_RES_ID_DINO_ELITE		:	_SET_RESNAME("CHARA_RES_ID_DINO_ELITE");	break;

	case CHARA_RES_ID_DINO_CC_RAPTOR :		_SET_RESNAME("CHARA_RES_ID_DINO_CC_RAPTOR");		break;
	case CHARA_RES_ID_DINO_CC_STING	 :		_SET_RESNAME("CHARA_RES_ID_DINO_CC_STING");			break;
	case CHARA_RES_ID_DINO_CC_ACID   :		_SET_RESNAME("CHARA_RES_ID_DINO_CC_ACID");			break;
	case CHARA_RES_ID_DINO_RAPTOR_MERCURY:	_SET_RESNAME("CHARA_RES_ID_DINO_RAPTOR_MERCURY");	break;
	case CHARA_RES_ID_DINO_STING_MARS:		_SET_RESNAME("CHARA_RES_ID_DINO_STING_MARS");		break;
	case CHARA_RES_ID_DINO_ACID_VULCAN:		_SET_RESNAME("CHARA_RES_ID_DINO_ACID_VULCAN");		break;
#ifdef DOMI_DINO
	case CHARA_RES_ID_DINO_DOMI_RAPTOR:			_SET_RESNAME("CHARARES_DINO_DOMI_RAPTOR");			break;
	case CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN:	_SET_RESNAME("CHARARES_DINO_DOMI_RAPTOR_GREEN");	break;
	case CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE:	_SET_RESNAME("CHARARES_DINO_DOMI_RAPTOR_BLUE");		break;
	case CHARA_RES_ID_DINO_DOMI_ACID:			_SET_RESNAME("CHARARES_DINO_DOMI_ACID");			break;
	case CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB:	_SET_RESNAME("CHARARES_DINO_DOMI_ACID_NOT_BOMB");	break;
	case CHARA_RES_ID_DINO_DOMI_STING:			_SET_RESNAME("CHARARES_DINO_DOMI_STING");			break;
	case CHARA_RES_ID_DINO_DOMI_MUTANTREX:		_SET_RESNAME("CHARARES_DINO_DOMI_MUTANTREX");		break;
	case CHARA_RES_ID_DINO_DOMI_ACID_ITEM:		_SET_RESNAME("CHARARES_DINO_DOMI_ACID_ITEM");		break;
	case CHARA_RES_ID_DINO_DOMI_TANK:			_SET_RESNAME("CHARARES_DINO_DOMI_TANK");			break;

	case CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY:	_SET_RESNAME("CHARARES_DINO_DOMI_RAPTOR_MERCURY");	break;
	case CHARA_RES_ID_DINO_DOMI_STING_MARS:		_SET_RESNAME("CHARARES_DINO_DOMI_STING_MARS");		break;
	case CHARA_RES_ID_DINO_DOMI_ACID_VULCAN:	_SET_RESNAME("CHARARES_DINO_DOMI_ACID_VULCAN");		break;
#endif
	default :
		I3PRINTLOG(I3LOG_FATAL,  "Invalid Character type.");
		break;
	}
#endif
}

void CGameCharaBase::_PlayDownfallSound( void)
{
	g_pSndMng->StartCharacterActionSnd( GTBCA_ON_DOWNFALL, m_pDBInfo->GetCharaSexTypeDISPLAY(), nullptr, is1PV());
}

void CGameCharaBase::__CreateEquipment( bool bInBattle, bool bLoadWeapon)
{
	// vv3 - check
	// 고민이 많은 부분. 해당 함수는 공통적으로 많이 쓴다. 일단 구조에 알맞은 정보로 생성하도록 한다.
	// 해당 캐릭터가 팀 별로 다른 정보를 가져 오도록 만들어야 한다. 그런데 이 함수가 어디 어디서 나오는지 파악이 되질 않음.
	// 후에 체크해야된다.
	const NET_CHARA_INFO * pNetCharaInfo = nullptr;
	const UI_CHARA_INFO  * pUICharaInfo = nullptr;
	I3ASSERT( m_pCharaInfo != nullptr);
	
	if( m_pCharaInfo->GetNetIdx() != MAX(0, BattleSlotContext::i()->GetMySlotIdxEx()) )
	{
		if(bInBattle)
			pNetCharaInfo  = BattleSlotContext::i()->getNetCharaInfo( m_pCharaInfo->GetNetIdx() );
		else
			pUICharaInfo   = CCommunityContext::i()->GetAnotherUserInfo( m_pCharaInfo->GetNetIdx() );
	}
	else
		pUICharaInfo   = UserInfoContext::i()->GetMyUICharaInfo();

	// 장비 등록
	// vv3 - check
	const CHARA_PARTS_INFO* pPartsInfo = nullptr;
	
	if( pNetCharaInfo )
		pPartsInfo = pNetCharaInfo->getCharaParts();
	if( pUICharaInfo )
		pPartsInfo = pUICharaInfo->getCharaParts( CCharaInfoContext::i()->GetTeamSlot()  );
	
	I3ASSERT_RETURN(pPartsInfo);

	T_ItemID chara_id = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
	CHARA_RES_ID charaID = CHARA::ItemID2ResID( chara_id);
	
	// 장착해야할 장비는 background load하자
	const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( chara_id);

	// 5ms
	if( pCharaDBInfo != nullptr)
	{	// 더미 캐릭터용
		Cmd_CreateEquip( charaID, pCharaDBInfo->GetPartsItemID( EQUIP::ePART_HEADGEAR), pCharaDBInfo->GetPartsItemID( EQUIP::ePART_BERET) );
		Cmd_CreateEquip( charaID, pCharaDBInfo->GetPartsItemID( EQUIP::ePART_FACEGEAR));
		Cmd_CreateEquip( charaID, pCharaDBInfo->GetPartsItemID( EQUIP::ePART_HOLSTER));
		Cmd_CreateEquip( charaID, pCharaDBInfo->GetPartsItemID( EQUIP::ePART_BELT));
	}

	// 무기 등록
	// ! praptor - getMySlotIdx -> GetMySlotIdxEx 교체했습니다.
	// 방 이외에 장소에서도 호출될때 idx를 0값으로 설정해 놓지만,
	// 몇몇 경우(RoomInfo초기화 등)에 -1로 설정될지도릅니다.
	// 50ms
	if( bLoadWeapon)
	{	
		if( pNetCharaInfo )
			g_pCharaManager->SetCharaWeaponSet( this, pNetCharaInfo->getWeaponList(WEAPON_SLOT_PRIMARY));

		if( pUICharaInfo )
			g_pCharaManager->SetCharaWeaponSet( this, pUICharaInfo->getWeaponList(WEAPON_SLOT_PRIMARY));
	}

	//	클랜 마크 설정	// 0.004ms
	Cmd_SetClanTexture( 0);
}

void CGameCharaBase::CreateLater( const CCharaInfo * pDBInfo, bool bLoadWeapon)
{
	// PhysX는 Thread에서 처리할 수 없다. Main에서 처리
	// 0.07ms ~ 0.14ms
	if( g_pFramework->IsBattleStage())
	{
		if( IsDino() == false )
			g_pCharaManager->SetPhysXShape( this, (CHARA_RES_ID) pDBInfo->GetResID());

		//Kinemetic로 셋팅 0.06ms
		SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);
	}

	// 0.03ms
	__Instantiate();
	
	// Physix 임의 처리입니다.
	// 물리량을 설정합니다. - 순구	// 0.33ms ~0.68ms
	if( g_pFramework->IsBattleStage())
	{
		_PHYSIX_SetDefaultVariable();
		getCollisionContext()->AttachCollisionNode( getSceneObject(), 0);	// ref++ = 4
	}
	
	// 장비 설정 12ms
	if( !isNPC() && !IsDino() )
	{
		__CreateEquipment( true, bLoadWeapon);
	}

	ResetChara();
	
	// 0.1ms
	Cmd_Visible( false);

	//	EnableRenderBone(true);
	this->EnableUpdateAABB(true);
	this->EnableOccludee(true);
	this->EnableRenderAABB(true);

}

void CGameCharaBase::CreateInLobby( const CCharaInfo * pDBInfo)
{
	CreateFirst( pDBInfo, true);

	ChangeControl_View(CHARA_VIEW_3PV);
	ChangeControl_Profile();

	VEC3D vPos;
	SetMatrix( &I3_IDENTITYMATRIX);
	i3Vector::Set( &vPos, 0.f, 0.0f, 0.0f);
    forceSetPos( &vPos);

	SetEnable( false);
}

void CGameCharaBase::CreateLaterInLobby( void)
{
	//Kinemetic로 셋팅
	SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);

	__Instantiate();

	if( !isNPC() && !IsDino() )
		__CreateEquipment(false, true);

	SetViewerChara();

	getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	getActionContext()->setBodyUpper( CHARA_BODYUPPER_ATTACKIDLE);
	getActionContext()->setLower( CHARA_LOWER_NONE);
	getActionContext()->setUpper( CHARA_UPPER_NONE);

	getAnimContext()->ChangeAnimationContext();

	//	로비 애니메이션으로 플레이
	Cmd_Profile_Idle_Stand();

	//	강제로 적용
	getAnimContext()->SetAIStateImmediately( 0.f);

	SetEnable( true);

	this->EnableUpdateAABB(true);
	this->EnableOccludee(true);
	this->EnableRenderAABB(true);
}

void CGameCharaBase::_PHYSIX_SetActorPairs( i3PhysixShapeSet * pShapeSet, UINT32 nFlag )
{
	if(g_pFramework == nullptr)
		return;

	CStageBattle * pStage = g_pFramework->GetStageBattle();
	if( pStage == nullptr)
		return;

	i3World * pWorld = pStage->getWorld();

	if( pWorld != nullptr )
	{
		if( pWorld->getWorldCollision() != nullptr)
		{
			i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), pShapeSet->getNxActor(), nFlag );
			pShapeSet->getNxShape( 0 )->setFlag( NX_SF_POINT_CONTACT_FORCE, true );
		}

		if( pStage->GetStageObjectList() != nullptr )
		{	
			UINT32 objCnt = pStage->GetStageObjectList()->size();
			for( UINT32 j = 0; j < objCnt; j++ )
			{
				i3Object * pObj = (*pStage->GetStageObjectList())[j];
				i3Scene::SetActorPairFlag( pObj->getSceneObject(), pShapeSet->getNxActor(), nFlag );
			}
		}
	}
}

#define PHYSIX_CHARA_LINEAR_DAMPING			0.0f
#define PHYSIX_CHARA_ANGULAR_DAMPING		1.5f //3.5f
#define PHYSIX_CHARA_FXFORCE				1.7f

void CGameCharaBase::_PHYSIX_SetDefaultVariable(void)
{
	INT32 i;
	MATRIX Mtx, ParentMtx;
	i3Skeleton * pSkel = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();

	for (i = 0; i < CHARA_HIT_MAXCOUNT; i++)
	{
		m_PhysixShapeSetInfo[i]._iBoneIndex = -1;
		m_PhysixShapeSetInfo[i]._pShapeSet = nullptr;
		m_PhysixShapeSetInfo[i]._rDistanceToParentBone = 0.f;
		m_PhysixShapeSetInfo[i]._iParentShapeIdx = CHARA_HIT_UNKNOWN;
	}

	// 캐릭터 피직스 actor 충돌깊이값 조정. 깊이 값이 클수록 떨림현상이 줄어듬. LocaleValue.pef 의 조정값으로 수정.
	// PhysX SDK 디폴트 값으로 0.01 설정되어 있으며(NX_SKIN_WIDTH 검색) 
	// i3s 파일에는 각 actor 별로 값이 설정되어 있음(0.01 ~ 0.04). 
	const REAL32 skinWidthIncr = LocaleValue::GetReal("Physix_CharaActorCollDepth_Incr");

	INT32 boneCnt = pSkel->getBoneCount();
	for (i = 0; i < boneCnt; i++)
	{
		if (!pSkel->isShapeSet(i))
			continue;

		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet(i);
		if (pShapeSet == nullptr)
		{
			continue;
		}

		CHARA_HIT_PART part = getCollisionContext()->GetHitPart(pShapeSet);
		if (part != CHARA_HIT_UNKNOWN)
		{
			m_PhysixShapeSetInfo[part]._iBoneIndex = i;
			m_PhysixShapeSetInfo[part]._pShapeSet = pShapeSet;
		}

		if (skinWidthIncr != 0.0f)
		{
			for (INT32 idx = 0; idx < pShapeSet->getNxShapeCount(); idx++)
			{
				NxShape* pShape = pShapeSet->getNxShape(idx);
				if (pShape == nullptr)
					continue;

				pShape->setSkinWidth(pShape->getSkinWidth() + skinWidthIncr);
			}
		}
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if(IsDino()) 
	{
		if(m_PhysixShapeSetInfo[CHARA_HIT_NECK2]._pShapeSet == nullptr) 
			I3PRINTLOG(I3LOG_FATAL, "_PHYSIX_SetDefaultVariable _pShapeSet is nullptr");
	}
#endif


	VEC3D vDist;
	for (i = 0; i < CHARA_HIT_MAXCOUNT; i++)
	{
		if (m_PhysixShapeSetInfo[i]._pShapeSet == nullptr)
			continue;

		i3Bone * pBone = pSkel->getBone(m_PhysixShapeSetInfo[i]._iBoneIndex);
		if (pBone->getParentIndex() != -1)
		{
			if (pSkel->isShapeSet(pBone->getParentIndex()) == false)	continue;

			i3PhysixShapeSet * pParentShapeSet = pSkel->getShapeSet(pBone->getParentIndex());
			if (pParentShapeSet == nullptr) continue;

			CHARA_HIT_PART part = getCollisionContext()->GetHitPart(pParentShapeSet);

			if (part != CHARA_HIT_UNKNOWN)
			{
				pParentShapeSet->getShapeGlobalPose(&ParentMtx);

				m_PhysixShapeSetInfo[i]._pShapeSet->getShapeGlobalPose(&Mtx);
				i3Vector::Sub(&vDist, i3Matrix::GetPos(&ParentMtx), i3Matrix::GetPos(&Mtx));
				REAL32 rDist = i3Vector::Length(&vDist);

				m_PhysixShapeSetInfo[i]._rDistanceToParentBone = rDist;
				m_PhysixShapeSetInfo[i]._iParentShapeIdx = (INT32)part;
			}
		}
	}

	if (IsDino() == false)
	{
		_PHYSIX_InitActorPairs();
	}

	// xgkick
	setDynamicActiveState(true, false);
	SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC, true);
}

void CGameCharaBase::_PHYSIX_InitActorPairs( void)
{
	INT32 i, j;
	i3PhysixContext * pCtx = g_pFramework->getPhysixContext();

	for( i = 1; i < CHARA_HIT_MAXCOUNT; i++)
	{
		if (m_PhysixShapeSetInfo[ i]._pShapeSet == nullptr)
			continue;

		switch( i )
		{
		case CHARA_HIT_ROOT :					// B_Root
			for( j = CHARA_HIT_BODY2; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_BODY3 :				// B_Spine1
			for( j = CHARA_HIT_BODY2; j < CHARA_HIT_LUARM; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			for( j = CHARA_HIT_LULEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_BODY2 :				// B_Spine2
			for( j = CHARA_HIT_BODY1; j < CHARA_HIT_LUARM; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			for( j = CHARA_HIT_LULEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_BODY1 :				// B_Spine3
			for( j = CHARA_HIT_NECK; j < CHARA_HIT_LUARM; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			for( j = CHARA_HIT_LDLEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_NECK :					// B_Neck
			for( j = CHARA_HIT_HEAD; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_HEAD :					// H_Head
			for( j = CHARA_HIT_LCLAVICLE; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_LCLAVICLE :			// B_L Clavicle
			for( j = CHARA_HIT_RCLAVICLE; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_RCLAVICLE :			// B_R Clavicle
			for( j = CHARA_HIT_LUARM; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_LUARM :				// AU_L UpperArm
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 2.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_LDARM]._pShapeSet, NX_IGNORE_PAIR);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_LHAND]._pShapeSet, NX_IGNORE_PAIR);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RUARM]._pShapeSet, NX_IGNORE_PAIR);
			for( j = CHARA_HIT_LULEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_LDARM :				// AD_L ForeArm
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 4.f);
			for( j = CHARA_HIT_LDLEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_LHAND :				// HN_L Hand
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 8.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RHAND]._pShapeSet, NX_IGNORE_PAIR);
			for( j = CHARA_HIT_LDLEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_RUARM :				// AU_R UpperArm
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 2.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RHAND]._pShapeSet, NX_IGNORE_PAIR);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RDARM]._pShapeSet, NX_IGNORE_PAIR);
			for( j = CHARA_HIT_LULEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_RDARM :				// AD_R ForeArm
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 4.f);
			for( j = CHARA_HIT_LDLEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_RHAND :				// HN_R Hand
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 8.f);
			for( j = CHARA_HIT_LDLEG; j < CHARA_HIT_MAXCOUNT; j++)
			{
				pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ j]._pShapeSet, NX_IGNORE_PAIR);
			}
			break;
		case CHARA_HIT_LULEG :				// LU_L Thigh
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 4.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_LDLEG]._pShapeSet, NX_IGNORE_PAIR);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_LFOOT]._pShapeSet, NX_IGNORE_PAIR);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RFOOT]._pShapeSet, NX_IGNORE_PAIR);
			break;
		case CHARA_HIT_LDLEG :				// LD_L Calf
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 8.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_LFOOT]._pShapeSet, NX_IGNORE_PAIR);
			break;
		case CHARA_HIT_LFOOT :				// F_L Foot
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 12.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RFOOT]._pShapeSet, NX_IGNORE_PAIR);
			break;
		case CHARA_HIT_RULEG :				// LU_R Thigh
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 4.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RDLEG]._pShapeSet, NX_IGNORE_PAIR);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_LFOOT]._pShapeSet, NX_IGNORE_PAIR);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RFOOT]._pShapeSet, NX_IGNORE_PAIR);
			break;
		case CHARA_HIT_RDLEG :				// LD_R Calf
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 8.f);
			pCtx->setActorPairFlags( m_PhysixShapeSetInfo[ i]._pShapeSet, m_PhysixShapeSetInfo[ CHARA_HIT_RFOOT]._pShapeSet, NX_IGNORE_PAIR);
			break;
		case CHARA_HIT_RFOOT :				// F_R Foot
			m_PhysixShapeSetInfo[ i]._pShapeSet->setFxForce( 12.f);
			break;
		}
	}
}

void CGameCharaBase::_PHYSIX_SetLinearVelocity( CHARA_DAMAGEINFO * pInfo )
{
	VEC3D vForce;
	VEC3D vDir;

	i3Vector::Normalize( &vDir, pInfo->_pVecDir );

	if( pInfo->_DeathType == CHARA_DEATH_B || pInfo->_DeathType == CHARA_DEATH_O )
	{
		i3PhysixShapeSet * pShapeSet;
		i3Body * pBody = getSceneObject()->GetBody();
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

		i3Vector::Scale( &vForce, &vDir, (REAL32)(pInfo->_rDamage) * 0.05f );
		
		INT32 boneCnt = pSkel->getBoneCount();
		for( INT32 i = 0; i < boneCnt; i++ )
		{
			pShapeSet = pSkel->getShapeSet( i );

			if( pShapeSet == nullptr )		continue;

			
			CHARA_HIT_PART part = (CHARA_HIT_PART)getCollisionContext()->GetHitPart( pShapeSet );

			if( part == CHARA_HIT_BODY1 )
			{
				VEC3D vAddLin;
				i3Vector::Scale( &vAddLin, &vForce, 1.5f );
				pShapeSet->setLinearVelocity( &vForce );
			}
			else if( part != CHARA_HIT_UNKNOWN )
			{
				pShapeSet->setLinearVelocity( &vForce );
			}
		}
		return;
	}

	if( pInfo->_pHitResult != nullptr )
	{	
		i3Vector::Scale( &vForce, &vDir, (REAL32)(pInfo->_rDamage) * 0.5f );
		pInfo->_pHitResult->m_pDestShapeSet->setLinearVelocity( &vForce );
	}
}

REAL32 CGameCharaBase::_CalcDamageLength( const CHARA_DAMAGEINFO * pDamageInfo)
{	
	I3ASSERT( pDamageInfo != nullptr);

	VEC3D InvVecDir, vShapePos;

	I3_PHYSIX_HIT_RESULT *	pHitResult = pDamageInfo->_pHitResult;
	if( pHitResult != nullptr )
	{
		i3PhysixShapeSet*		pDestShapeSet = pHitResult->m_pDestShapeSet;
		
		*((NxVec3 *) &vShapePos) = pDestShapeSet->getNxActor()->getGlobalPosition();

		i3Vector::Sub( &InvVecDir, &vShapePos, pDamageInfo->_pVecPos );

		return i3Vector::Length( &InvVecDir);
	}
	else
	{
		return pDamageInfo->_rDistance;
	}
}

REAL32 CGameCharaBase::_CalcDistanceRate( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength, CGameCharaBase* pKiller)
{
	I3ASSERT( pDamageInfo != nullptr);
	
	// DPD_Rate의 (Damage per Distance) 적용
	VEC3D vLen;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pDamageInfo->_ui32FireWeaponID, true );

	REAL32  rValidDamageRate = 1.0f;
	REAL32	rDistRate = 1.f;

	// 거리에 따른 데미지의 증감율( 1.0f가 Default입니다.)
	if( pDamageInfo->_rExplosionRange > I3_EPS)
	{
		rValidDamageRate	= 1.0f - ((1.0f / pDamageInfo->_rExplosionRange) * rDamagedLength) * pDamageInfo->_rExplosionSlope;
	}

	if( !pDamageInfo->_IsNPC )
	{
		CGameCharaBase * pHitUser = pKiller;
		if( pHitUser == nullptr)
		{
			pHitUser = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);
		}

		if( pHitUser != nullptr )
		{
			REAL32 rDistance = pDamageInfo->_rDistance;
			if( pDamageInfo->_rDistance == 0.f)
			{
				i3Vector::Sub( &vLen, GetPos(), pHitUser->GetPos() );
				rDistance = i3Vector::Length( &vLen);
			}
			
			if( pDamageInfo->_pWeapon != nullptr)
			{
				I3ASSERT( pDamageInfo->_pWeapon->GetRange() > I3_EPS);
				rDistRate = rDistance / pDamageInfo->_pWeapon->GetRange();
			} 
			else
			{
				I3ASSERT( pWeaponInfo->GetRange() > I3_EPS);
				rDistRate = rDistance / pWeaponInfo->GetRange();
			}
		}
	}
	else
	{	// NPC
		I3ASSERT( pWeaponInfo->GetRange() > I3_EPS);
		rDistRate = pDamageInfo->_rDistance / pWeaponInfo->GetRange();
		pDamageInfo->_rDamage = 5.f;
		pDamageInfo->SetHitPart( CHARA_HIT_BODY1 );
	}

	if( rDistRate > 1.0f)	rDistRate = 1.0f;
		
	// 호칭 수치 계산 (DPD_Rate : 원거리 데미지)
	REAL32 fDPD_Value = 0.0f;
	REAL32 fRate = 0.0f;

	if( pDamageInfo->_pWeapon != nullptr)
		fDPD_Value = pDamageInfo->_pWeapon->GetDPD_Rate();
	else
		fDPD_Value = pWeaponInfo->GetDPD_Rate();

	// DPD Rate 호칭 적용..
	if (  pDamageInfo->_pWeapon != nullptr )
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( pDamageInfo->_nKillerIdx);
		
		if (pNetInfo->GetDesigApplyInfo())
			fRate += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponDPDRate * 0.01f;
	}

	fDPD_Value = i3Math::Min(1.0f, (fDPD_Value + (fDPD_Value * fRate)));	
	rValidDamageRate *= (1.0f - (rDistRate * fDPD_Value));

	return rValidDamageRate;
}

void CGameCharaBase::ResetChara( void)
{	

	bool bFirstRespawn = isCharaStateMask(CHARA_STATE_FIRSTRESPAWN);
	bool bDeath = isCharaStateMask(CHARA_STATE_DEATH);

	removeCharaFlag( CHARA_FLAG_RESETABLE);

	_ResetCharaStateMask();

	// 다음 스테이트는 지속되어야 합니다.
	if ( bFirstRespawn ) addCharaStateMask(CHARA_STATE_FIRSTRESPAWN);
	if ( bDeath ) addCharaStateMask(CHARA_STATE_DEATH);

	removeEnableStateMask( 0xFFFFFFFF);

	m_fJumpDelayElapsedTime = 0.f;
	m_rRespawnElapseTime	= 0.f;
	m_rInvincibleElapseTime	= 0.f;
	m_rContactTime			= 0.f;
	m_rAttackIdleTime		= 0.f;
	m_rAttackIdleTimeLeft	= 0.f;
	m_rPendingActionTime	= 0.f;
	m_rDamageHowlEffectiveTime	= 0.0f;
	m_bLoadMagChg = true;

	m_iNetworkAttackCount = 0.f;

	if(m_pWeaponContext)	m_pWeaponContext->OnReset();
	if(m_pUpdateContext)	m_pUpdateContext->OnReset();
	if(m_pActionContext)	m_pActionContext->OnReset();
	if(m_pAnimContext)		m_pAnimContext->OnReset();
	if(m_pMaterialContext)	m_pMaterialContext->OnReset();
	if(m_pInputContext)		m_pInputContext->OnReset();
	if(m_pMoveContext)		m_pMoveContext->OnReset();
	if(m_pEquipContext)		m_pEquipContext->OnReset();
	if(m_pSoundContext)		m_pSoundContext->OnReset();
	if(m_pCollisionContext) m_pCollisionContext->OnReset();
	if(m_pCameraContext)	m_pCameraContext->OnReset();
	if(m_pHUDContext)		m_pHUDContext->OnReset();
	if(m_pBoneContext)		m_pBoneContext->OnReset();

	if(IsDino() == false)
	{
		//	캐릭터에 붙이기 위해 현재 붙어 있는 모든 무기를 떼어낸다
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT; ++i)
			Cmd_DetachWeapon( static_cast<WEAPON_SLOT_TYPE>(i));

		//	무기 초기화
		if (getWeaponContext())
			getWeaponContext()->_Reset_Weapons();
		else
			TRACE("CGameCharaBase::ResetChara getWeaponContext() = nullptr\n");
	}

	setFullHP();
	
	getActionContext()->_All_Stop();

	if( isLocal() )
	{
		if (getHUDContext())
		{
			getHUDContext()->ClearScreenEffect();
			if (g_pFramework->GetHazePtr()) g_pFramework->GetHazePtr()->InitEffect();
		}
		else
			TRACE("CGameCharaBase::ResetChara getHUDContext() = nullptr\n");
	}

	// 각종 능력치 초기값 계산
	__ResetDesigOffsetDatas();

	__ResetInvincibleTime();
	__ResetJumpHeightRate();
	__ResetThrowSpeed();
	__ResetFullHP();
	__ResetMovementSpeed();
	__ResetDefenceRate();
	__ResetWeaponSwapTimeRate();

	if( getEquipContext() != nullptr)
	{
		m_rTotalAbility[ CHARA::eABILITY_FALLDAMAGE] = getEquipContext()->GetAbility( EQUIP::eABILITY_FALLDAMAGE );
	}
	

	addCharaFlag( CHARA_FLAG_ENABLE_EFFECT);
}

void CGameCharaBase::_UpdateOnlyAI( REAL32 tm)
{
	i3GameNode * pChild = getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild->isEnable() && (! pChild->isPaused() ) )
		{
			if( i3::same_of<i3AIContext*>(pChild))
			{
				pChild->OnUpdate( tm);
			}
		}

		pChild = pChild->getNext();
	}
}

void CGameCharaBase::_UpdateAllExceptAI( REAL32 tm)
{
	i3GameNode * pChild = getFirstChild();

#if defined( I3_DEBUG)
	i3GameNode* pPrevNode = pChild;
#endif

	while( pChild != nullptr)
	{
		if( pChild->isEnable() && (! pChild->isPaused() ) )
		{
			if( i3::same_of<i3AIContext*>(pChild) == false)
			{
				pChild->OnUpdate( tm);
			}
		}

		pChild = pChild->getNext();

#if defined( I3_DEBUG)
		//같은 child가 있는 지 검사 합니다.
		I3ASSERT( pPrevNode != pChild ); 
		pPrevNode = pChild;
#endif

	}
}

void CGameCharaBase::AddOnObjectState()
{
	// 관전자, 자기 자신 플레이어 모두 다 포함
	// 카메라가 보는 캐릭이 헬기에 타고, 카메라가 보간을 한다면
	if( g_pCamera != nullptr && g_pCamera->getCurTarget() == this && g_pCamera->isEnabledInterpolate())
	{
		// 보간을 하지 않게 한다.
		g_pCamera->SetInterPolate(0.f);
	}

	addCharaStateMask( CHARA_STATE_ONOBJECT);
	//GlobalFunc::PB_TRACE("AddOnObjectState \n");
}

void CGameCharaBase::RemoveOnObjectState()
{
	if(isCharaStateMask(CHARA_STATE_ONOBJECT) == true)
	{
		// 관전자, 자기 자신 플레이어 모두 다 포함
		// 카메라가 보는 캐릭이 헬기에 내리고, 카메라가 보간을 하지않고, 현재 카메라가 fps 카메라가 아니라면
		if( g_pCamera != nullptr && g_pCamera->getCurTarget() == this && g_pCamera->isEnabledInterpolate() == false && 
			i3::same_of<CGameCamera_FPS*>(g_pCamera) == false)
		{
			// 보간을 한다.
			g_pCamera->SetInterPolate(0.1f);
			//GlobalFunc::PB_TRACE("RemoveOnObjectState \n");
		}
	}

	removeCharaStateMask( CHARA_STATE_ONOBJECT);
	//GlobalFunc::PB_TRACE("RemoveOnObjectState \n");
}

void CGameCharaBase::_UpdateDamagedByHowl( REAL32 rDeltaSeconds)
{
	if( getDamagedByHowl() == false)
		return;

	m_rDamageHowlEffectiveTime -= rDeltaSeconds;

	if( m_rDamageHowlEffectiveTime <= 0.0f)
	{
		setDamagedByHowl( false);
		setDamagedByHowlTime( 0.0f);
	}
}

void CGameCharaBase::_UpdateDeathBlowState( REAL32 rDeltaSeconds)
{
	if( isDeathblowState() == false)	return;

	m_rElapsedDeathBlow -= rDeltaSeconds;

	if( m_rElapsedDeathBlow <= 0.0f)
	{
		SetDeathblowState( false);
	}
}


void CGameCharaBase::OnUpdate( REAL32 tm)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	// 이하의 처리 순서는 매우 중요하고, 변경할 때는 숙고가 필요하다.
	//	Add code here
	m_rAttackIdleTime -= tm;
	m_rAttackIdleTimeLeft -= tm;
	m_rDropWeaponTimer -= tm;
	m_rPendingActionTime -= tm;

	if(m_pInputContext) m_pInputContext->OnFirstUpdate( tm);

	if(m_pWeaponContext) m_pWeaponContext->OnUpdate( tm);

	// Input 관련 처리가 끝나면, 무엇보다 바로 AIContext를 Update한다.
	// 이것은 AIContext에서 어떤 Animation을 재생할지가 결정되기 때문에
	// 앞서 처리될 필요가 있기 때문이다.
	_UpdateOnlyAI( tm);

	// UpdateContext::onFirstUpdate 안에서 Animation에 대한 Update 및
	// Bone의 World-Space Matrix를 모두 구해두어야 한다.
	// Animation의 Update는 독립적인 계산이기 때문에 우선해도 무방하며,
	// 이후의 대부분의 계산에서 1-Frame 오차를 발생하지 않도록 하기
	// 위해서 반드시 앞서 처리되어야 한다.
	if(m_pUpdateContext)	m_pUpdateContext->OnFirstUpdate( tm);

	if(m_pCameraContext)	m_pCameraContext->ProcessChangePose( tm);
	
	//m_pWeaponContext->OnUpdate( tm);
	
	//i3Chara::OnUpdate( tm);
	// AIContext를 제외한 나머지(Weapon 등)들을 모두 Update한다.
	// 이것은 i3Chara::OnUpdate의 대체용이다.
	_UpdateAllExceptAI( tm);

	if(m_pMaterialContext)	m_pMaterialContext->OnUpdate( tm);

	if(m_pInputContext)		m_pInputContext->OnUpdate( tm);
	if(m_pMoveContext)		m_pMoveContext->OnUpdate( tm);
	if(m_pUpdateContext)	m_pUpdateContext->OnUpdate( tm);
	if(m_pActionContext)	m_pActionContext->OnUpdate( tm);
	if(m_pAnimContext)		m_pAnimContext->OnUpdate( tm);
	if(m_pHUDContext)		m_pHUDContext->OnUpdate( tm);
	if(m_pUpdateContext)	m_pUpdateContext->OnLastUpdate( tm);	

	_UpdateDamagedByHowl( tm);
	_UpdateDeathBlowState( tm);

	if (m_rCrouchParalysis > 0)
	{
		m_rCrouchParalysis -= tm;
	}
}

// AI State가 변경될 때마다 호출되는 함수.
void CGameCharaBase::OnChangeAIState( i3AIContext * pCtx)
{
	i3AIState * pState = pCtx->getCurrentAIState();
	i3Animation * pAnim;

	I3ASSERT( pState != nullptr);

	if( pState->getAnim() == nullptr)
	{
		return;
	}

	pAnim = pState->getAnim();

	// 상체 Animation들은 하체의 것과 방향이 안맞을 수 있다.
	// 이것을 보정하기 위해, Pivot Bone이 삽입되어 있으며,
	// Pivot은 항상 전방(Z축)으로 향해야 한다는 가정을 이용해
	// 상체가 비틀어진 것을 보정한다.

	if (m_pBoneContext)
	{
		m_pBoneContext->setTimeTargetYawBlend( pState->getBlendTime());
		m_pBoneContext->setTimeYawBlend( 0.f);
	}
	else
		TRACE("CGameCharaBase::OnChangeAIState m_pBoneContext = nullptr\n");
	

	

	INT32 idxIK = -1;
	INT32 idxPivot = -1;

	INT32 trackCnt = pAnim->GetTrackCount();
	for(INT32 i=0; i < trackCnt; i++)
	{
		if( i3::generic_is_iequal( pAnim->GetTrackBoneName( i), "IK") )
		{
			idxIK = i;
		}

		if( i3::generic_is_iequal( pAnim->GetTrackBoneName( i), "Pivot") )
		{
			idxPivot = i;
		}
	}

	// GlobalFunc::PB_TRACE("OnChangeAIState 1 %s %d %x idxPivot %d", pAnim->GetName(), pCtx->getCurrnetAIStateIndex(), pState, idxPivot);

	if(idxPivot != -1)
	{
		if (m_pBoneContext)
		{
			pAnim->GetKeyframe( idxPivot, 0, m_pBoneContext->getPivotMatrix());
			m_pBoneContext->setValidPivot( true);
		}
		return;
	}

		
	if (m_pBoneContext)
		m_pBoneContext->setPivotYawTarget( 0.f);
}

void CGameCharaBase::ReturnInstance( void)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( getCharaInfo()->GetNetIdx() > -1 &&
		getCharaInfo()->GetNetIdx() < USER_SLOT_COUNT )
	{
		char szTemp[256] = "";
		i3::snprintf( szTemp, _countof(szTemp), "Remove Chara (%s)", BattleSlotContext::i()->getNickForSlot(  getCharaInfo()->GetNetIdx()));
		I3PRINTLOG(I3LOG_NOTICE, szTemp);
	}
#endif

	// Context들을 삭제
	Destroy();

	// SceneGraph 삭제
	getAttachNode()->RemoveAllChild();

	// 나머지 GameNode를 삭제
	RemoveAllChild();

	m_bReturnedInstance = true;	// 이것을 통해 인스턴 반환 여부를 체크하도록 한다.

	i3Chara::ReturnInstance();
}

void CGameCharaBase::OnBindResource( void)
{
	if( m_pCharaInfo == nullptr || m_pCharaInfo->GetType() == CHARA::TYPE_NPC)
	{
		return;
	}

	i3Chara::OnBindResource();

	// Common ID
	_PrepareCommonAI();
	
	return;
}

void CGameCharaBase::ChangeControl_Local( void)
{
	if(m_pWeaponContext)	m_pWeaponContext->OnChangeLocal();
	if(m_pUpdateContext)	m_pUpdateContext->OnChangeLocal();
	if(m_pActionContext)	m_pActionContext->OnChangeLocal();
	if(m_pAnimContext)		m_pAnimContext->OnChangeLocal();
	if(m_pMaterialContext)	m_pMaterialContext->OnChangeLocal();
	if(m_pInputContext)		m_pInputContext->OnChangeLocal();
	if(m_pMoveContext)		m_pMoveContext->OnChangeLocal();
	if(m_pEquipContext)		m_pEquipContext->OnChangeLocal();
	if(m_pSoundContext)		m_pSoundContext->OnChangeLocal();
	if(m_pCollisionContext) m_pCollisionContext->OnChangeLocal();
	if(m_pCameraContext)	m_pCameraContext->OnChangeLocal();
}

void CGameCharaBase::ChangeControl_Network( void)
{
//	if(m_pWeaponContext) m_pWeaponContext->OnChangeNetwork();
// 	m_pUpdateContext->OnChangeNetwork();
// 	m_pActionContext->OnChangeNetwork();
// 	m_pAnimContext->OnChangeNetwork();
// 	m_pMaterialContext->OnChangeNetwork();
// 	m_pInputContext->OnChangeNetwork();
// 	m_pMoveContext->OnChangeNetwork();
// 	if(m_pEquipContext) m_pEquipContext->OnChangeNetwork();
// 	m_pSoundContext->OnChangeNetwork();
// 	m_pCollisionContext->OnChangeNetwork();
// 	m_pCameraContext->OnChangeNetwork();
// 	m_pAIContext->OnChangeNetwork();

	if(m_pWeaponContext)	m_pWeaponContext->OnChangeNetwork();
	if(m_pUpdateContext)	m_pUpdateContext->OnChangeNetwork();
	if(m_pActionContext)	m_pActionContext->OnChangeNetwork();
	if(m_pAnimContext)		m_pAnimContext->OnChangeNetwork();
	if(m_pMaterialContext)	m_pMaterialContext->OnChangeNetwork();
	if(m_pInputContext)		m_pInputContext->OnChangeNetwork();
	if(m_pMoveContext)		m_pMoveContext->OnChangeNetwork();
	if(m_pEquipContext)		m_pEquipContext->OnChangeNetwork();
	if(m_pSoundContext)		m_pSoundContext->OnChangeNetwork();
	if(m_pCollisionContext) m_pCollisionContext->OnChangeNetwork();
	if(m_pCameraContext)	m_pCameraContext->OnChangeNetwork();
}

void CGameCharaBase::ChangeControl_Fullbody()
{
	if(m_pWeaponContext)	m_pWeaponContext->OnChangeNetwork();
	if(m_pUpdateContext)	m_pUpdateContext->OnChangeFullbody();
	if(m_pActionContext)	m_pActionContext->OnChangeNetwork();
	if(m_pAnimContext)		m_pAnimContext->OnChangeNetwork();
	if(m_pMaterialContext)	m_pMaterialContext->OnChangeNetwork();
	if(m_pInputContext)		m_pInputContext->OnChangeFullbody();
	if(m_pMoveContext)		m_pMoveContext->OnChangeFullbody();
	if(m_pEquipContext)		m_pEquipContext->OnChangeNetwork();
	if(m_pSoundContext)		m_pSoundContext->OnChangeNetwork();
	if(m_pCollisionContext) m_pCollisionContext->OnChangeNetwork();
	if(m_pCameraContext)	m_pCameraContext->OnChangeNetwork();
}


void CGameCharaBase::ChangeControl_NPC( void)
{
// 	if(m_pWeaponContext) m_pWeaponContext->OnChangeNPC();
// 	m_pUpdateContext->OnChangeNPC();
// 	m_pActionContext->OnChangeNPC();
// 	m_pAnimContext->OnChangeNPC();
// 	m_pMaterialContext->OnChangeNPC();
// 	m_pInputContext->OnChangeNPC();
// 	m_pMoveContext->OnChangeNPC();
// 	if(m_pEquipContext) m_pEquipContext->OnChangeNPC();
// 	m_pSoundContext->OnChangeNPC();
// 	m_pCollisionContext->OnChangeNPC();
// 	m_pCameraContext->OnChangeNPC();
// 	m_pAIContext->OnChangeNPC();

	if(m_pWeaponContext)	m_pWeaponContext->OnChangeNPC();
	if(m_pUpdateContext)	m_pUpdateContext->OnChangeNPC();
	if(m_pActionContext)	m_pActionContext->OnChangeNPC();
	if(m_pAnimContext)		m_pAnimContext->OnChangeNPC();
	if(m_pMaterialContext)	m_pMaterialContext->OnChangeNPC();
	if(m_pInputContext)		m_pInputContext->OnChangeNPC();
	if(m_pMoveContext)		m_pMoveContext->OnChangeNPC();
	if(m_pEquipContext)		m_pEquipContext->OnChangeNPC();
	if(m_pSoundContext)		m_pSoundContext->OnChangeNPC();
	if(m_pCollisionContext) m_pCollisionContext->OnChangeNPC();
	if(m_pCameraContext)	m_pCameraContext->OnChangeNPC();
}

void CGameCharaBase::ChangeControl_AI(void)
{
	if (m_pActionContext)	m_pActionContext->OnChangeLocal();
	if (m_pWeaponContext)	m_pWeaponContext->OnChangeLocal();
	if (m_pUpdateContext)	m_pUpdateContext->OnChangeLocal();
	if (m_pAnimContext)		m_pAnimContext->OnChangeLocal();
	if (m_pMaterialContext) m_pMaterialContext->OnChangeLocal();

	if(IsDino())
	{
		if (m_pInputContext)	m_pInputContext->OnChangeAI_Dino();
	}
	else
	{
		if (m_pInputContext)	m_pInputContext->OnChangeAI();
	}

	if (m_pMoveContext)		m_pMoveContext->OnChangeAI();
	if (m_pEquipContext)	m_pEquipContext->OnChangeLocal();
	if (m_pSoundContext)	m_pSoundContext->OnChangeLocal();
	if (m_pCollisionContext)	m_pCollisionContext->OnChangeLocal();
	if (m_pCameraContext)	m_pCameraContext->OnChangeLocal();
}


void CGameCharaBase::ChangeControl_Profile( void)
{
	if (m_pUpdateContext)
		m_pUpdateContext->OnChangeProfile();
}

bool CGameCharaBase::OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event )
		{
		case I3_EVT_CONTACT :
			if( i3::same_of<i3PhysixShapeSet*>(pObj))
			{
				i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pObj;
				
				INT32 cnt = pShapeSet->getContactCount();
				for( INT32 i = 0; i < cnt; i++ )
				{
					const I3_PHYSIX_CONTACT_REPORT_PTR& spReport = pShapeSet->getContactReport( i );

					if (getCollisionContext())
						getCollisionContext()->QueContactReport( spReport );
				}

				return true;
			}
			break;
		}
	}

	return i3GameObj::OnEvent( event, pObj, param, code);
}


void CGameCharaBase::ProcessTimer( REAL32 rDeltatime)
{
	if( g_pFramework->IsBattleStage() == false)
		return;

	//	리스폰 직후 무적시간을 계산해 준다.
	if( m_rInvincibleElapseTime > 0.0f)
	{
		m_rInvincibleElapseTime = m_rInvincibleElapseTime - rDeltatime;
		
		REAL32 InvTime = getAbility( CHARA::eABILITY_INVINCIBLE_TIME);

		if( InvTime  - 0.5f > m_rInvincibleElapseTime)	
		{	
			removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
		}

		m_pMaterialContext->SetBlendEnable( true);
	}		
	else if( m_rInvincibleElapseTime < 0.0f)		
	{	
		removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);

		m_rInvincibleElapseTime = 0.0f;

		m_pMaterialContext->SetBlendEnable( false);

		m_rInvincibleElapseTime = m_rInvincibleElapseTime - rDeltatime;

		
	}

	/*
	//	점프 딜레이 체크
	if( m_fJumpDelayElapsedTime > 0.0f)
	{
		m_fJumpDelayElapsedTime -= tm;
	}
	else if( m_fJumpDelayElapsedTime < 0.0f)		m_fJumpDelayElapsedTime = 0.0f;
	*/

	m_fJumpDelayElapsedTime = m_fJumpDelayElapsedTime + rDeltatime;
}

void CGameCharaBase::MoveStop( void)
{
	if (m_pMoveContext)
	{
		i3Vector::Zero( m_pMoveContext->getMoveDir());

		m_pMoveContext->setAccel( 0.0f, 0.0f, 0.0f);
		m_pMoveContext->setVelocity( 0.0f, 0.0f, 0.0f);	
	}
}

void CGameCharaBase::forceSetPos( const VEC3D * pPos)
{
	i3GameObj::SetPos( const_cast<VEC3D*>(pPos) );
	if (m_pMoveContext)
		m_pMoveContext->SetPosition( pPos);
}


void CGameCharaBase::SetViewerChara( bool bEnable)
{	//	뷰어 또는 GUI용 3인칭 캐릭터로 고정	
	if( bEnable)
	{	
		getSceneObject()->GetBody()->removeStyle( I3BS_LOD_AUTO);
		getSceneObject()->GetBody()->setCurrentLevel( 1);

		//	무기 LOD
		if( getWeaponContext() != nullptr)
		{
			for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
			{
				WeaponBase * pWeapon = getWeaponContext()->getWeapon( static_cast<WEAPON_SLOT_TYPE>(i));

				if( pWeapon != nullptr)
				{
					i3Body * pBody = pWeapon->getSceneObject()->GetBody();
					I3ASSERT( pBody != nullptr);

					pBody->removeStyle( I3BS_LOD_AUTO);
					pWeapon->setCurLOD( 0);
				}
			}
		}

		//	장비 LOD
		if( m_pEquipContext != nullptr)
		{
			for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)		
			{
				EQUIP::ePART part = (EQUIP::ePART) i;

				CGameCharaEquip * pEquip = m_pEquipContext->getEquip( part);

				if( pEquip != nullptr)
				{
					i3Body * pBody = pEquip->getSceneObject()->GetBody();
					I3ASSERT( pBody != nullptr);

					pBody->removeStyle( I3BS_LOD_AUTO);
					pEquip->setCurLOD( 0);
				}
			}
		}	

		//	텍스처 깍뚜기 현상을 막아요.
		if (m_pMaterialContext)
		{
			m_pMaterialContext->getTextureFilterAttr()->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
			m_pMaterialContext->getTextureFilterAttr()->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		}
	}
	else
	{
		getSceneObject()->GetBody()->addStyle( I3BS_LOD_AUTO);

		//	무기 LOD
		if( getWeaponContext() != nullptr)
		{
			for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
			{
				WeaponBase * pWeapon = getWeaponContext()->getWeapon( static_cast<WEAPON_SLOT_TYPE>(i));

				if( pWeapon != nullptr)
				{
					i3Body * pBody = pWeapon->getSceneObject()->GetBody();
					I3ASSERT( pBody != nullptr);

					pBody->addStyle( I3BS_LOD_AUTO);
				}
			}
		}

		//	장비 LOD
		if( m_pEquipContext != nullptr)
		{
			for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)		
			{
				EQUIP::ePART part = (EQUIP::ePART) i;
		
				CGameCharaEquip * pEquip = m_pEquipContext->getEquip( part);

				if( pEquip != nullptr)
				{
					i3Body * pBody = pEquip->getSceneObject()->GetBody();
					I3ASSERT( pBody != nullptr);

					pBody->addStyle( I3BS_LOD_AUTO);
				}
			}
		}

		if (m_pMaterialContext)
		{
			m_pMaterialContext->getTextureFilterAttr()->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
			m_pMaterialContext->getTextureFilterAttr()->SetMinFilter( I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR);
		}
	}
}

bool CGameCharaBase::CalcHelmetPenetrate( const CHARA_DAMAGEINFO * pDamageInfo ) const
{
	if(IsDino()) return false;
	
	// 장비 방어력
	REAL32 HeadDefenseRate = GetHelmetProtectionRate();

	if( HeadDefenseRate == 0.0f )
		return true;

	// 무기 관통력
	REAL32 rPenetrateHead = pDamageInfo->_pWeapon->GetWeaponPenetrate( false);

	if( pDamageInfo->_DeathType == CHARA_DEATH_O )
		rPenetrateHead = 0.5f;

	// 헬멧 방어율 계산
	if( rPenetrateHead > 0.f && HeadDefenseRate > 0.f)
	{
		rPenetrateHead /= HeadDefenseRate;
	}

	if( rPenetrateHead < i3Math::Randf() )
	{
		return false;
	}

	return true;	// 관통됨
}

void CGameCharaBase::SetHelmetProtection( bool bEnable)
{
	REAL32 HeadProtectionTime = m_pEquipContext->GetHeadProtectionTime();

	m_rEndHelmProtectionTime = g_pFramework->GetAccTime() + HeadProtectionTime;
	getEquipContext()->setHelmetProtect( true);

	if (bEnable)
		observer_hud::Context::i()->Notify(observer_hud::EVT_HELMETPROTECTION, getCharaNetIndex());
}

void CGameCharaBase::DropEquip( VEC3D * pDir)
{
	if( isAttachedProtectParts() )
	{
		VEC3D vDir;

		if( pDir != nullptr)
			i3Vector::Copy( &vDir, pDir);
		else
			i3Vector::Scale( &vDir, i3Matrix::GetAt( GetMatrix()), -1.f);

		getEquipContext()->DropToWorld( getProtectPartsType(), &vDir );
	}
}

void CGameCharaBase::HelmetEffect( CHARA_DAMAGEINFO * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	BULLET_EFFECT_TYPE effectType = BULLET_EFFECT_DEFAULT;
	if( pInfo->_pWeaponInfo != nullptr)
		effectType = static_cast<BULLET_EFFECT_TYPE>(pInfo->_pWeaponInfo->GetLoadBulletType());
	g_pEffectManager->AddEquipHitEffect( pInfo->GetFireWeaponClass(), effectType, 
		i3Matrix::GetPos( getBoneContext()->getHeadMatrix()), pInfo->_pVecDir, is1PV() );
}

void CGameCharaBase::_CheckHelmetProtection( CHARA_DAMAGEINFO * pDamageInfo, bool &bHelmetDrop, bool &bDamaged)
{
	if(IsDino()) return;
	if (MyRoomInfoContext::i()->getStage()->IsOneShotOneKill()) return;

	// Dedicated Server가 아니고 Client라면 처리하지 않는다.
	if( BattleServerContext::i()->IsUseDedicationMode() == false)
	{
		if( BattleServerContext::i()->IsP2PHost() == false) return;
	}

	if( !isInvincible() &&
		(pDamageInfo->GetHitPart() == CHARA_HIT_HEAD) )
	{
		if( g_pFramework->GetAccTime() < m_rEndHelmProtectionTime) {
			bDamaged = false;
			SetHelmProtectionFlag(true);
		}
		else {
			m_rEndHelmProtectionTime = 0.f;
		}

		if( m_pEquipContext && bDamaged)
		{
			if( isAttachedProtectParts() && !getEquipContext()->getHelmetProtect() )
			{
				//	헬멧을 쓰고 있다면
				bHelmetDrop = true;		// 차후 MultiProtection을 계산후에 설정하도록 기능 변경 예정

				if( pDamageInfo->isHelmProtectDamage())
				{
					// 헬멧의 관통 여부 계산
					if( !CalcHelmetPenetrate( pDamageInfo ) )
					{	// 관통이 안 되었습니다. 데미지는 계산하지 않습니다.
						bDamaged = false;

						SetHelmetProtection( true);
						SetHelmProtectionFlag(true);
					}
				}
			}
		}

		if( bHelmetDrop)
			pDamageInfo->_HelmetFlag = NET_HELMET_FLAG_DROP;

		if( !bDamaged )
		{
			pDamageInfo->_rDamage = 0.f;
			pDamageInfo->_HelmetFlag |= NET_HELMET_FLAG_PROTECT;
		}
	}
}

REAL32 CGameCharaBase::GetDinoFullHP()
{
	return (REAL32)getDBInfo()->GetHP();
}

REAL32 CGameCharaBase::GetDinoDamage(REAL32 rDamage)
{
	REAL32 rRatio = 0.f;

	rRatio = ((REAL32) getFullHP()) / GetDinoFullHP();

	return rDamage * rRatio;
}

void CGameCharaBase::CalcHitPoint( REAL32 rDamage, CHARA_DAMAGEINFO* pDamageInfo)
{
	//	HP 감소	
	if(IsDino()) {
		m_pCharaInfo->DecreaseHP(GetDinoDamage(rDamage));
	}
	else {
		m_pCharaInfo->DecreaseHP(rDamage);
	}
	
	//I3TRACE("NetID: %d HP %f / Damage %f\n", getCharaInfo()->GetNetIdx(), getCharaInfo()->GetHP(), rDamage);

	REAL32 rHP = m_pCharaInfo->GetHP();

	// hp가 0.5일때 ui에서 0으로 표기 되며, 캐릭터는 죽지 않는다.
	// 방장이 아닌 캐릭은 이동 불가 상태로 빠지기도 한다.
	// 그래서. 1미만이면 죽음 처리를 한다. - 박명진 2010. 5. 11
	//if( m_pCharaInfo->GetHP() <= 0)		m_pCharaInfo->SetHP(0.f);
	if( rHP < 1.f)
	{
		rHP = 0.f;
		m_pCharaInfo->SetHP( rHP);
	}
	else if( !IsDino() && rHP > ENCRYPTION_MAX_HP_VALUE)		rHP = ENCRYPTION_MAX_HP_VALUE;		///< 사람의 경우 최대 150까지
	else if (IsDino() && rHP > GetDinoFullHP())
	{
		rHP =GetDinoFullHP();
		m_pCharaInfo->SetHP( rHP);
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		if( !isNPC() )
		{
			GAMEINFO_CHARACTER * pCharaInfo	= BattleSlotContext::i()->setGameInfo_Chara( getCharaNetIndex());
			
			UINT16 hp = pCharaInfo->_tNetData._tHP.getHP();
			UINT16 curHP = (UINT16) rHP;

			//Hit된경우 이동속도 느려지는 효과
			if(isLocal() && ( hp > curHP))
			{
				Cmd_SpeedReduction( pDamageInfo->GetHitPart(), hp, curHP);
				
				g_pCamera->SetTremble( 2.0f, 2.0f, 0.0f, 0.1f);
			}

			pCharaInfo->_tNetData._tHP.setHP( curHP);
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "BattleServerContext::isNPC() is true..it must be false..");
		}
	}
}

// Medical Kit 추가  - 2011.06.21
bool CGameCharaBase::RecoveryHP(CHARA_DAMAGEINFO* pDamageInfo)
{
	// 릴레이일 경우 방장이 아니면 계산 안함
	if(!BattleServerContext::i()->IsUseDedicationMode())
	{
		if(!BattleServerContext::i()->IsP2PHost())
			return false;
	}

	OnHit(pDamageInfo);

	return true;
}

void CGameCharaBase::CalcDamageByPart( CHARA_DAMAGEINFO * pDamageInfo, REAL32& rDamage)
{
	REAL32 rRateDamage = 0.f;

	//범위 데미지가 머리에 맞은경우 데미지계산만 ROOT에 맞은 데미지로 계산을 합니다. --1214기획팀요청, RAJA수정
	if( pDamageInfo->_ByObjectIndex == 0xFFFF	// 오브젝트의한 데미지일 경우는 제외 - #2512 : 이윤호
		&& pDamageInfo->isDamageTypeRange() )	//범위 데미지를 맞았다.(수류탄, 폭탄, 오브젝트 등..)
	{
		//일반 모드에서 범위 데미지를 맞은 경우, CHARA_HIT_ROOT(몸통)으로 처리한다.
		if( pDamageInfo->GetHitPart() == CHARA_HIT_HEAD )	//머리에 맞았다.
		{
			pDamageInfo->SetHitPart( CHARA_HIT_ROOT );
		}
	}
	//	각부위에 따른 데미지 연산을 합니다.
	CCharaGlobal * pGlobalInfo = g_pCharaInfoDataBase->getGlobal();
	if (IsDino())
		pGlobalInfo = g_pCharaInfoDataBase->getDinoGlobal();
	CHARA::eDAMAGEPART damagePart = CHARA::GetDamagePartByHit(pDamageInfo->GetHitPart());

	if ((MyRoomInfoContext::i()->getStage()->IsHeadShotOnly())
		&& pDamageInfo->_DeathType == CHARA_DEATH_A)
	{
		rRateDamage = pGlobalInfo->GetDamageRateByMode(damagePart);
	}
	else
		rRateDamage = getDBInfo()->GetPartDamageRate(damagePart);

	
	rDamage *= rRateDamage;
	
	if(rDamage < 1 && rDamage > 0)
		rDamage =1;

	pGlobalInfo->CalcDamageByPart(rDamage);
}


REAL32 CGameCharaBase::GetThrowSpeed( WEAPON_SLOT_TYPE nSlot )
{
	REAL32 speed = 0.f;
	if( GetCurrentCharaWeapon() == nullptr)
		return speed;

	WeaponBase * pWeapon = getWeaponContext()->getWeapon(nSlot)->getUsedWeapon();
	if( pWeapon != nullptr)
		speed = pWeapon->GetThrowSpeed();

	speed += speed * m_rTotalAbility[ CHARA::eABILITY_THROWSPEED];

	return speed;
}

REAL32 CGameCharaBase::GetExThrowSpeed(WEAPON_SLOT_TYPE nSlot)
{
	REAL32 speed = 0.f;
	if (GetCurrentCharaWeapon() == nullptr)
		return speed;

	WeaponBase * pWeapon = getWeaponContext()->getWeapon(nSlot)->getUsedWeapon();
	if (pWeapon != nullptr)
		speed = pWeapon->GetExThrowSpeed();

	speed += speed * m_rTotalAbility[CHARA::eABILITY_THROWSPEED];

	return speed;
}

bool CGameCharaBase::NET_HitForHost( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamage)
{
	I3ASSERT( BattleServerContext::i()->IsP2PHost() == true);

	if( g_pFramework->IsBattleStage())
	{	
		GAMESTAGE_STATE stageState = gstatemgr::i()->getStageState();
		if( stageState != GAMESTAGE_STATE_BATTLE)
		{
			return false;
		}
		// 전투 상태이면 처리합니다.
	}

	// Medical Kit - 체력 회복후 리턴
	if(pDamageInfo->_DamageType == WEAPON::DAMAGE_BUFF)
	{
		REAL32 rHP = m_pCharaInfo->GetHP() + pDamageInfo->_rDamage;
		REAL32 maxHP = (REAL32) getFullHP();

		if(rHP > maxHP)
			rHP = maxHP;

		// 통신용 HP 설정
		GAMEINFO_CHARACTER * pCharaInfo	= BattleSlotContext::i()->setGameInfo_Chara( getCharaNetIndex() );

		pCharaInfo->_tNetData._tHP.setHP( (UINT16)rHP);

		// 실제 HP 설정
		m_pCharaInfo->SetHP( rHP);		

		return false;
	}

	bool	bDamaged = true;
	bool	bDeath = false;
	bool	bHelmetDrop = false;

	CalcDamageByPart(pDamageInfo, rDamage);

	I3ASSERT( !pDamageInfo->isFireWeaponSlot(WEAPON_SLOT_UNKNOWN) );
	I3ASSERT( !pDamageInfo->isFireWeaponNumber(-1) );

	SetHelmProtectionFlag(false);

	if( !isCharaStateMask( CHARA_STATE_DEATH) )
	{
		// 헤드샷 계산. 헬멧 프로텍션을 처리합니다.
		_CheckHelmetProtection( pDamageInfo, bHelmetDrop, bDamaged);

		if( bDamaged )
		{
			if(pDamageInfo->_pWeapon && pDamageInfo->_pWeapon->GetWeaponDamage()->IsCritical())
			{
				rDamage = pDamageInfo->_pWeapon->GetWeaponDamage()->GetCriticalDamage(rDamage);
			}

			CalcHitPoint( rDamage, pDamageInfo);
			
			if( pDamageInfo->_IsNPC)
			{
				if( m_pCharaInfo->GetHP() < 1.f)	m_pCharaInfo->SetHP(1.f);
			}

			if( m_pCharaInfo->GetHP() == 0.f && (isCharaStateMask( CHARA_STATE_DEATH) == false) )	//	죽으면
			{
				bDeath = true;
				bDamaged = false;
			}
		}
	}
	else
	{
		Cmd_HitEffect( pDamageInfo );
		return false;

		// 죽은 캐릭터면 물리 영향을 준다.
		//_PHYSIX_SetLinearVelocity( pDamageInfo );
	}

	// 피격입니다.
	if( bDamaged && !isNPC() && !pDamageInfo->isFireWeaponType(WEAPON_CLASS_MISSION) )
	{
		bool bFlag = pDamageInfo->IsCalculateOwned( BattleServerContext::i()->IsUseDedicationMode());
		GAMEEVENT::Write_Chara_Suffering( this, pDamageInfo->_DeathType, pDamageInfo->_nKillerIdx, pDamageInfo->GetHitPart(), bFlag);
	}

	// AI전에서는 방장이 어시스트를 계산합니다
	if (MyRoomInfoContext::i()->getStage()->IsAiMode() && BattleServerContext::i()->IsP2PHost())		AssistContext::i()->SetAssistor(this->getCharaNetIndex(), pDamageInfo->_nKillerIdx);

	//	상체가 데미지 동작이 가능한 상태이면
	Cmd_Damage( pDamageInfo);

	if( bDeath )
	{
		if( !isNPC() )
		{
			// Death chara & Physix set
			g_pCharaManager->DeathChara( this, pDamageInfo->_nKillerIdx, pDamageInfo->_pVecDir, pDamageInfo->_DeathType, pDamageInfo->GetHitPart() );

			// Network Sync
			if( !pDamageInfo->isFireWeaponType(WEAPON_CLASS_MISSION) )
			{	// C4는 네트워크 싱크를 맞추지 않습니다.
				GAMETOCONTEXT_INFO_CHARA_DEATH deathInfo;

				deathInfo.Reset();
				deathInfo._iKillidx			= pDamageInfo->_nKillerIdx;
				deathInfo._iDeathidx		= getCharaNetIndex();

				// KEYWORD : WEAPON_DUALHANDGUN
				/*if( pDamageInfo->_pWeapon != nullptr && pDamageInfo->_pWeapon->getPairWeapon() != nullptr)
				{
					if(i3::same_of<WeaponMiniAxe*>(pDamageInfo->_pWeapon->getPairWeapon()))
					{
						deathInfo._ui32WeaponID		= pDamageInfo->_ui32BaseWeaponID;
					}
					else
					{
						CWeaponInfo * pWeaponInfo = pDamageInfo->_pWeapon->getPairWeapon()->getWeaponInfo();
						I3ASSERT( pWeaponInfo != nullptr);			
						deathInfo._ui32WeaponID = pWeaponInfo->GetItemID();								
					}
				}
				else*/
				{
					deathInfo._ui32WeaponID		= pDamageInfo->_ui32BaseWeaponID;
				}

				deathInfo._nDeathType			= pDamageInfo->_DeathType;
				deathInfo._nHitPart				= pDamageInfo->GetHitPart();
				deathInfo._pHitDir				= pDamageInfo->_pVecDir;

				GAMEEVENT::Write_Chara_Death( &deathInfo);
			}
		}
		else
		{
			getActionContext()->_Play_Death( pDamageInfo->_nKillerIdx, pDamageInfo->_DeathType, pDamageInfo->_pVecDir);
		}
	}

	return bDeath;
}

void CGameCharaBase::NET_HitForClient( CHARA_DAMAGEINFO * pDamageInfo)
{
	MATRIX mat;

	I3ASSERT( BattleServerContext::i()->IsP2PHost() == false );

	if(IsDino())
	{
		NET_HitForClientNoHelmet(pDamageInfo);
	}
	else
	{	
		if(m_pEquipContext)
		{
			//	장비 체크
			if( (pDamageInfo->GetHitPart() == CHARA_HIT_HEAD) &&
				isAttachedProtectParts() &&	!isCharaStateMask( CHARA_STATE_DEATH) )
			{
				if( pDamageInfo->_pHitResult != nullptr )
				{
					//	불꽃이 튄다.
					HelmetEffect( pDamageInfo);
				}
				else
				{	
					if( pDamageInfo->GetHitPart() != CHARA_HIT_UNKNOWN)
					{
						if( m_PhysixShapeSetInfo[ pDamageInfo->GetHitPart()]._pShapeSet != nullptr)
						{
							BULLET_EFFECT_TYPE effectType = BULLET_EFFECT_DEFAULT;
							if( pDamageInfo->_pWeaponInfo != nullptr)
								effectType = static_cast<BULLET_EFFECT_TYPE>(pDamageInfo->_pWeaponInfo->GetLoadBulletType());

							m_PhysixShapeSetInfo[ pDamageInfo->GetHitPart()]._pShapeSet->getShapeGlobalPose( &mat);
							g_pEffectManager->AddEquipHitEffect( pDamageInfo->GetFireWeaponClass(),	effectType,
								i3Matrix::GetPos( &mat), pDamageInfo->_pVecDir, is1PV() );
						}
					}
				}
			}
			else
			{
				NET_HitForClientNoHelmet(pDamageInfo);
			}
		}
	}
}

void CGameCharaBase::NET_HitForClientNoHelmet( CHARA_DAMAGEINFO * pDamageInfo)
{
	if (getActionContext())
		getActionContext()->_Play_Damage( pDamageInfo->GetHitPart(), pDamageInfo->_pVecDir);
	Cmd_HitEffect( pDamageInfo );
}

void CGameCharaBase::DeathCharaByFastObject(VEC3D* pDir)
{
	CHARA_DAMAGEINFO damageInfo;
	
	damageInfo._DeathType = CHARA_DEATH_F;
	damageInfo._pVecDir = pDir;
	damageInfo.SetHitPart( CHARA_HIT_HEAD);

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, WEAPON::getItemIndex( WEAPON::KNIFE_M7) );
	damageInfo.SetWeaponItemID(pWeaponInfo, pWeaponInfo);

	DeathCharaAndWritePacket(&damageInfo);
}

void CGameCharaBase::DeathCharaAndWritePacket( const CHARA_DAMAGEINFO * pDamageInfo)
{
	GAMETOCONTEXT_INFO_CHARA_DEATH deathInfo;
	deathInfo.Reset();

	CHARA_HIT_PART part = pDamageInfo->GetHitPart();

	// Death chara & Physix set
	g_pCharaManager->DeathChara( this, getCharaNetIndex(), pDamageInfo->_pVecDir, pDamageInfo->_DeathType, part );

	// Network Sync
	deathInfo._iKillidx			= getCharaNetIndex();
	deathInfo._iDeathidx		= getCharaNetIndex();
	deathInfo._ui32WeaponID		= pDamageInfo->_ui32BaseWeaponID;
	deathInfo._nDeathType		= pDamageInfo->_DeathType;
	deathInfo._nHitPart			= part;
	deathInfo._pHitDir			= pDamageInfo->_pVecDir;
	
	GAMEEVENT::Write_Chara_Death( &deathInfo);
}

void CGameCharaBase::SetDeathblowState(bool bEnable)
{ 
	if( bEnable == isCharaFlag(CHARA_FLAG_DEATHBLOW))	return;

	if( bEnable)
	{
		addCharaFlag( CHARA_FLAG_DEATHBLOW);
		m_rElapsedDeathBlow = 5.0f;
	}
	else
	{
		removeCharaFlag( CHARA_FLAG_DEATHBLOW);
		m_rElapsedDeathBlow = 0.0f;	
	}
}

namespace
{
	bool IsSentrygunDamageInfo(CHARA_DAMAGEINFO* info)
	{
		if( info->isFireWeaponType(WEAPON_CLASS_MISSION))
		{
			if (info->isRealWeaponNumber(WEAPON::SENTRYGUN_LV1) ) return true;
			if (info->isRealWeaponNumber(WEAPON::SENTRYGUN_LV2) ) return true;
			if (info->isRealWeaponNumber(WEAPON::SENTRYGUN_LV3) ) return true;
			if (info->isRealWeaponNumber(WEAPON::SENTRYGUN_LV4) ) return true;
		}
		return false;
	}

	bool IsDummyDamageInfo(CHARA_DAMAGEINFO* info)
	{
		if( info->isFireWeaponType(WEAPON_CLASS_MISSION))
		{
			if (info->isRealWeaponNumber(WEAPON::DUMMY_LV1) ) return true;
			if (info->isRealWeaponNumber(WEAPON::DUMMY_LV2) ) return true;
			if (info->isRealWeaponNumber(WEAPON::DUMMY_LV3) ) return true;
			if (info->isRealWeaponNumber(WEAPON::DUMMY_LV4) ) return true;
		}
		return false;
	}

}
INT32 CGameCharaBase::OnHit( CHARA_DAMAGEINFO * pDamageInfo, CGameCharaBase* pKiller)
{
	I3ASSERT( pDamageInfo != nullptr);

#if defined( I3_DEBUG) && 0
	pDamageInfo->DumpDamageInfo();
#endif

	if( isInvincible())		return 0;		// 무적 상태이다.

	if( gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE )return 0;

	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	
	REAL32	rDamagedLength		= _CalcDamageLength( pDamageInfo);

	I3ASSERT( (pDamageInfo->GetHitPart() != CHARA_HIT_UNKNOWN) && (pDamageInfo->GetHitPart() < CHARA_HIT_MAXCOUNT));

	switch( pDamageInfo->_DamageType)
	{
	////////////////////////////////////////////////////////	섬광
	case WEAPON::DAMAGE_FLASH:
		{	
			getHUDContext()->ScreenEffect_Flashbang( pDamageInfo->_pVecPos,
							pDamageInfo->_rEffectiveTime, pDamageInfo->_rExplosionRange,
							pDamageInfo->_rExplosionSlope, rDamagedLength);
		}
		break;	

	case  WEAPON::DAMAGE_BUFF:
		{
			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);
			if( pChara == nullptr)
				return 1;

			if(pChara->IsDino())
			{
				if( pDamageInfo->_IsOwner )
				{	
					if( !isCharaStateMask( CHARA_STATE_DEATH) &&
						!pDamageInfo->_IsNPC && !isNPC()
#if defined( DEF_OBSERVER_MODE)
						&& !BattleSlotContext::i()->isObserverMe()
#endif
						)
					{
						I3ASSERT( pMyChara != nullptr);
						GAMEEVENT::Write_Chara_Hit( pMyChara, pDamageInfo);
					}
				}

				if( BattleServerContext::i()->IsP2PHost() )
					NET_HitForHost( const_cast<CHARA_DAMAGEINFO*>( pDamageInfo), pDamageInfo->_rDamage);
				else
					NET_HitForClient( const_cast<CHARA_DAMAGEINFO*>( pDamageInfo));

				//Effect및 부가 효과

				if (pDamageInfo->_nVictimIdx == BattleSlotContext::i()->getMySlotIdx())
				{
					CGameCharaDinoElite * pElite = (CGameCharaDinoElite*)pChara;

					// 스킬 지속시간 / 이동 속도율 
					REAL32 rSkillDuration = pElite->GetDinoSkillDuration(WEAPON::SECONDARY_FIRE);
					REAL32 rSkillMovementRate = pElite->GetDinoSkillMovementRate(WEAPON::SECONDARY_FIRE);

					//blur
					getHUDContext()->ScreenEffect_Howl( pDamageInfo->_rEffectiveTime);

					// howl에 의한 디버프 활성화
					setDamagedByHowl( true);
					setDamagedByHowlTime( rSkillDuration);

					// 이속 감소 효과
					getMoveContext()->EnableSpeedReduction(rSkillMovementRate, rSkillDuration);

					if(CHud::i())
						CHud::i()->AddMobilityUp();
				}
			}
			else
			{
				if( !isCharaStateMask( CHARA_STATE_DEATH) )
				{
					I3ASSERT( pMyChara != nullptr);
					GAMEEVENT::Write_Chara_Hit( pMyChara, pDamageInfo);

					// P2P는 방장이 계산
					if( BattleServerContext::i()->IsP2PHost() )	
						NET_HitForHost( const_cast<CHARA_DAMAGEINFO*>( pDamageInfo), pDamageInfo->_rDamage);
				}
			}
		}
		break;
	////////////////////////////////////////////////////////	HP	감소
	case WEAPON::DAMAGE_HP_LOSS:
		{				
			REAL32 rDamage = (REAL32) CalcDamage( pDamageInfo, rDamagedLength, pKiller);

			pDamageInfo->_rDamage = rDamage;

			if( pDamageInfo->IsCalculateOwned( BattleServerContext::i()->IsUseDedicationMode()) )
			{
				// 수류탄, 던지는 무기류
				if( !isCharaStateMask( CHARA_STATE_DEATH) )
				{
					if( BattleServerContext::i()->IsP2PHost() )
					{	
						// 수류탄 / 던지는 무기류는  방장이 계산
						NET_HitForHost( pDamageInfo, rDamage);

						// AI Character가 맞으면 여기에 대한 reaction을 한다.
						if ( isAI() )
						{
							CGameCharaInputControlAI * pAi = (CGameCharaInputControlAI*)getInputContext()->getControl();
							pAi->OnHit(pDamageInfo->_nKillerIdx);
						}
					}
					else
						NET_HitForClient( pDamageInfo);
				}
				else
				{
					Cmd_HitEffect( pDamageInfo );
	
					// 죽은 캐릭터면 물리 영향을 준다.
					//_PHYSIX_SetLinearVelocity( pDamageInfo );
				}

				return true;
			}
			else if( pDamageInfo->isFireWeaponType(WEAPON_CLASS_MISSION) &&
				 (pDamageInfo->isRealWeaponNumber(WEAPON::C4) ||  
				 pDamageInfo->isRealWeaponNumber(WEAPON::SHUTTLEX) ) )

			{// C4는 모든 클라이언트도 계산
				// Death chara & Physix set
				g_pCharaManager->DeathChara( this, pDamageInfo->_nKillerIdx, pDamageInfo->_pVecDir, pDamageInfo->_DeathType );
				return true;
			}

			////////////////////////////////////////////////////////////////////////
			// for Network  --- GameContext에도 코드가 있습니다.
			// desc : 자신이 발사한 것은 네트워크로 CS에 보냅니다.
			if( pDamageInfo->_IsOwner )
			{
				if( pDamageInfo->_pWeapon && pDamageInfo->_pWeapon->GetWeaponDamage()->CheckCriticalDamage())
				{
					g_pSndMng->StartCharacterActionSnd( GTBCA_ON_HIT_CRITICAL, m_pDBInfo->GetCharaSexTypeDISPLAY(), this->GetPos(), this->is1PV());
					I3TRACE("critical damage!!!!!!!!!!!!!!!!!!!!!!!\n");
				}

				if( BattleServerContext::i()->IsP2PHost() )	NET_HitForHost( pDamageInfo, rDamage);
				else								NET_HitForClient( pDamageInfo);

				if( !isCharaStateMask( CHARA_STATE_DEATH) &&
					!pDamageInfo->_IsNPC && !isNPC()
#if defined( DEF_OBSERVER_MODE)
					&& !BattleSlotContext::i()->isObserverMe()
#endif
					)
				{
					if( BattleServerContext::i()->IsUseDedicationMode())
					{
						CalcDamageByPart( pDamageInfo, pDamageInfo->_rDamage);

						I3ASSERT( !pDamageInfo->isFireWeaponSlot(WEAPON_SLOT_UNKNOWN) );
						I3ASSERT( !pDamageInfo->isFireWeaponNumber(-1) );

						if( !isCharaStateMask( CHARA_STATE_DEATH) )
						{
							bool bHelmetDrop = false;
							bool bDamaged = true;

							// 헤드샷 계산. 헬멧 프로텍션을 처리합니다.
							_CheckHelmetProtection( pDamageInfo, bHelmetDrop, bDamaged);
						}

						// revision 59406 acid(Vulcan) 자폭 시 자신에게 주는 데미지 수정 hansoft.5342
						if( getCharaInfo()->IsAcid() || getCharaInfo()->IsCCAcid() || getCharaInfo()->IsAcidVulcan())
						{
							if( pDamageInfo->_nKillerIdx == pDamageInfo->_nVictimIdx)
								pDamageInfo->_rDamage = GetDinoFullHP() * 2.f;
						}
					}

					if( pDamageInfo->_pWeapon && pDamageInfo->_pWeapon->GetWeaponDamage()->IsCritical() && pDamageInfo->GetHitPart() != CHARA_HIT_HEAD)
					{
						pDamageInfo->_rDamage = pDamageInfo->_pWeapon->GetWeaponDamage()->GetCriticalDamage( pDamageInfo->_rDamage);
					}

					I3ASSERT( pMyChara != nullptr);

					GAMEEVENT::Write_Chara_Hit( pMyChara, pDamageInfo, true);		
				}

			}
			else if( BattleServerContext::i()->IsP2PHost())
			{
				// NPC를 맞춘경우
				if( isNPC())
					NET_HitForHost( pDamageInfo, rDamage);

				// AI Character가 맞춘경우
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);

				if( pChara != nullptr && pChara->isAI())
					NET_HitForHost( pDamageInfo, rDamage);
			}
		}
		break;
	case WEAPON::DAMAGE_TRAMPLE:
		{
			pDamageInfo->_nVictimIdx = getCharaNetIndex();

			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);

			if( pChara == nullptr || (pChara->IsRex() == false && pChara->IsDomiMutantRex() == false && pChara->IsTank() == false))
				return 1;

			if( pDamageInfo->_IsOwner )
			{
				if( !isCharaStateMask( CHARA_STATE_DEATH) &&
					!pDamageInfo->_IsNPC && !isNPC()
#if defined( DEF_OBSERVER_MODE)
					&& !BattleSlotContext::i()->isObserverMe()
#endif
					)
				{
					I3ASSERT( pMyChara != nullptr);
					GAMEEVENT::Write_Chara_Hit( pMyChara, pDamageInfo);
				}
			}

			if( BattleServerContext::i()->IsP2PHost() )	NET_HitForHost( pDamageInfo, pDamageInfo->_rDamage);
			else
				NET_HitForClient( pDamageInfo);
		}break;
	case WEAPON::DAMAGE_HOWL:
		{
			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);

			if( pChara == nullptr || pChara->IsDino() == false)
			{
				return 1;
			}
			pDamageInfo->_nVictimIdx = getCharaNetIndex();

			if( pDamageInfo->_IsOwner )
			{	
				if( !isCharaStateMask( CHARA_STATE_DEATH) &&
					!pDamageInfo->_IsNPC && !isNPC()
#if defined( DEF_OBSERVER_MODE)
					&& !BattleSlotContext::i()->isObserverMe()
#endif
					)
				{
					I3ASSERT( pMyChara != nullptr);
					GAMEEVENT::Write_Chara_Hit( pMyChara, pDamageInfo);
				}
			}

			if( BattleServerContext::i()->IsP2PHost() )	NET_HitForHost( pDamageInfo, pDamageInfo->_rDamage);
			else
				NET_HitForClient( pDamageInfo);

			//Effect및 부가 효과
			{
				CGameCharaDinoTRex * pRex = (CGameCharaDinoTRex*)pChara;

				// 스킬 지속시간 / 이동 속도율 
				REAL32 rSkillDuration = pRex->GetDinoSkillDuration(WEAPON::SECONDARY_FIRE);
				REAL32 rSkillMovementRate = pRex->GetDinoSkillMovementRate(WEAPON::SECONDARY_FIRE);

				//blur
				getHUDContext()->ScreenEffect_Howl( pDamageInfo->_rEffectiveTime);

				// howl에 의한 디버프 활성화
				setDamagedByHowl( true);
				setDamagedByHowlTime( rSkillDuration );

				// 이속 감소 효과
				getMoveContext()->EnableSpeedReduction( rSkillMovementRate , pDamageInfo->_rEffectiveTime);
			}
		}break;
	default:
		{
			I3TRACE("[%s] WARNING! Unknown damage type! (%d)\n", __FUNCTION__, (INT32) pDamageInfo->_DamageType);
		}
	}	//	End of switch

	return 1;
}


#define LOD_SLOPE_VALUE		3.5f

void CGameCharaBase::calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio)
{
	INT32 i;
	
	static INT32 nSelectTemp = 4;
	
	if (nSelectTemp == 4)
		getSceneObject()->GetBody()->CalcLOD( pPos, pCamPos, fFOVRatio);
	else
		getSceneObject()->GetBody()->setCurrentLevel( nSelectTemp);

	if(IsDino() == false)
	{
		//	무기 LOD
		WeaponBase * pWeapon = GetCurrentCharaWeapon();
		if( pWeapon != nullptr)
			pWeapon->calcLOD( pPos, pCamPos, fFOVRatio);

		// 등에 맨 무기 LOD도 처리
		if( getWeaponContext()->getBackpack() != nullptr)
			getWeaponContext()->getBackpack()->calcLOD( pPos, pCamPos, fFOVRatio);
		
		//	장비 LOD
		if(m_pEquipContext)
		{
			for( i = 1; i < EQUIP::ePART_COUNT; i++)		
			{
				EQUIP::ePART type = (EQUIP::ePART) i;

				CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
				if( pEquip == nullptr) continue;

				if( m_pEquipContext->isAttachOnChara( type))
					pEquip->calcLOD( pPos, pCamPos, fFOVRatio);
			}
		}	
	}
}

bool CGameCharaBase::isAttachedProtectParts(void)
{
	if( m_pEquipContext == nullptr )
		return false;

	return (m_pEquipContext->isAttachOnChara( getProtectPartsType() ) == true);
}

EQUIP::ePART CGameCharaBase::getProtectPartsType(void)
{
	if( g_pEnvSet->IsV2Version() == false )
	{
		if( m_pEquipContext->getEquipInfo(EQUIP::ePART_HEADGEAR) != nullptr )
			return EQUIP::ePART_HEADGEAR;
		else
			return EQUIP::ePART_FACEGEAR;
	}

	return EQUIP::ePART_HEADGEAR;
}

bool	CGameCharaBase::isEnableTargetChase(void) const
{
	if(m_pEquipContext)
		return (m_pEquipContext->getTargetChaseC5() > I3_EPS);

	return false;
}

REAL32 CGameCharaBase::getTargetChaseRange( void) const
{
	if(m_pEquipContext)
		return m_pEquipContext->getTargetChaseC5();
	return 0.f;
}

void CGameCharaBase::SetMoveSpeedAddOffset(REAL32 rMoveOffset)
{
	m_rMoveOffset = rMoveOffset; 
} 


REAL32 CGameCharaBase::GetHelmetProtectionRate() const
{
	if(IsDomiDino() == false)
	{
		const NET_CHARA_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( getCharaNetIndex());
		REAL32 HeadDefenseRate = pInfo->getCharaSkillValue(SKILLLIST_HELMETDEFENSE);

		if(m_pEquipContext != nullptr)
			HeadDefenseRate += m_pEquipContext->GetHeadDefenseRate();

		return HeadDefenseRate;
	}

	return 0.f;
}

REAL32 CGameCharaBase::GetWeaponSwapTimeScale( void) const
{
	REAL32 rTimeScale = 1.f + m_rTotalAbility[ CHARA::eABILITY_SWAPTIME];
	
	//스킬 빠른 무기 변경 시간감소
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon != nullptr)
	{
		WeaponBase * pUsedWeapon = pWeapon->getUsedWeapon();
		if( pUsedWeapon != nullptr)
		{
			const NET_CHARA_INFO * pNetChara = BattleSlotContext::i()->getNetCharaInfo( getCharaNetIndex());
			REAL32 ratio = pNetChara->getWeaponSkillValue( pUsedWeapon->getWeaponInfo()->GetTypeClass(), SKILLLIST_CHANGE);
			rTimeScale += ratio;
		}
	}

	if( rTimeScale < I3_EPS)
		rTimeScale = 1.f;

	return rTimeScale;
}

REAL32 CGameCharaBase::GetDamageRateByDefence(CHARA_HIT_PART hitpart) const
{	
	REAL32 rRateDamage = 1.f;

	if( IsDomiDino() == false)
	{
		rRateDamage -= m_rDefenceRate[ CHARA::GetDamagePartByHit( hitpart)];
	}

	rRateDamage = MINMAX( I3_EPS, rRateDamage, 1.f);

	return rRateDamage; 
}

namespace
{
	REAL32 GetLadderSpeed(const EMoveSpeedType mst, const EMoveDirectionType mdt, const CGameCharaBase* climber)
	{
		REAL32 speed = 0.0f;
		const CCharaInfo* info = climber->getDBInfo();

		if (mst == MST_WALK)
		{
			switch (mdt)
			{
			case MDT_FRONT:		speed = info->GetLadderWalkFrontSpeed();	break;
			case MDT_SIDE:		speed = info->GetLadderWalkSideSpeed();		break;
			case MDT_DIAGONAL:	speed = info->GetLadderWalkDiagonalSpeed(); break;
			case MDT_BACK:		speed = info->GetLadderWalkBackSpeed();		break;
			}
		}
		else
		{
			switch (mdt)
			{
			case MDT_FRONT:		speed = info->GetLadderRunFrontSpeed();		break;
			case MDT_SIDE:		speed = info->GetLadderRunSideSpeed();		break;
			case MDT_DIAGONAL:	speed = info->GetLadderRunDiagonalSpeed();	break;
			case MDT_BACK:		speed = info->GetLadderRunBackSpeed();		break;
			}
		}

		return speed;
	}
}

REAL32 CGameCharaBase::GetMoveSpeedAddOffset(EMoveSpeedType eMST, EMoveDirectionType eMDT) const
{
	I3ASSERT(m_pDBInfo != nullptr);
	I3ASSERT(eMST < MST_COUNT);
	I3ASSERT(eMDT < MDT_COUNT);

	REAL32 rMoveSpeed = 0.0f;

	if (isCharaStateMask(CHARA_STATE_CLIMB))
		rMoveSpeed = GetLadderSpeed(eMST, eMDT, this);
	else
		rMoveSpeed = getDBInfo()->GetMoveSpeed(eMST, eMDT);
	
	if ( CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) ) 
		rMoveSpeed += getDBInfo()->GetMoveSpeed(eMST, eMDT, MMT_SPACE);

	// m_arMoveOffset[][] 혹은 m_rMoveOffset쪽은 계산식이 틀려서 변경함. 12월 버전 호칭 계산에 맞춘다.(2015.05.22.수빈)
	REAL32 rAddSpeed = m_rMoveOffset + rMoveSpeed * m_rMovementSpeedRate[ eMST][ eMDT];
	
	REAL32 Result = rMoveSpeed + rAddSpeed;

	CStageBattle* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	if (pStage != nullptr && CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) )
		Result += pStage->GetStageAddMoveSpeed(eMST, eMDT);
	
	return Result;
}

REAL32 CGameCharaBase::GetAccelerationTimeAddOffset() const 
{
	I3ASSERT(m_pDBInfo != nullptr);
	
	REAL32 Result = i3Math::Max(0.001f, (m_rAccelerationTimeOffset + m_rAccelerationTimeOffset_RunSkill + m_pDBInfo->GetAccelerationTime()));

	CStageBattle* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	if (pStage != nullptr && CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) )
		Result *= pStage->GetStageAddAccelerationTime();

	return Result;
}

void CGameCharaBase::PostUpdate(REAL32 rDeltaTime)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	// 이하의 처리 순서는 매우 중요하고, 변경할 때는 숙고가 필요하다.
	//	Add code here
	m_pMoveContext->PostUpdate(rDeltaTime);

}

void CGameCharaBase::SaveNowPos()
{
	i3Vector::Copy(&m_vSavedPos, getMoveContext()->getRideObjectLocalPos());
	//GlobalFunc::PB_TRACE("SaveNowPos m_vSavedPos %f %f %f \n", m_vSavedPos.x, m_vSavedPos.y, m_vSavedPos.z);
}

VEC3D* CGameCharaBase::LoadSavedPos()
{
	getMoveContext()->setRideObjectLocalPos(&m_vSavedPos);
	getMoveContext()->ResetJumping();

	//GlobalFunc::PB_TRACE("LoadSavedPos m_vSavedPos %f %f %f \n", m_vSavedPos.x, m_vSavedPos.y, m_vSavedPos.z);

	return &m_vSavedPos;
}	

void CGameCharaBase::OnObject(CGameObjectWithWeapon* pObj)
{ 
	if( isCharaFlag( CHARA_FLAG_ON_OBJECT)) return;

	addCharaFlag( CHARA_FLAG_ON_OBJECT);

	i3Scene::ModifyDynamicState( getSceneObject(), I3_PHYSIX_STATE_DISABLE, 0);
	SetEnable(false);
	m_pObjectForControlWeapon = pObj;
	
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( i3::same_of<WeaponDotLaser*>(pWeapon))
	{
		((WeaponDotLaser*)pWeapon)->DotStampDisable();
	}
}

void CGameCharaBase::OffObject()
{ 
	removeCharaFlag( CHARA_FLAG_ON_OBJECT);

	SetEnable(true);

	// 캐릭터가 살아 있는 경우에만 키도록 합니다.
	if( isCharaStateMask( CHARA_STATE_DEATH) == false)
		SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );
}

INT32 CGameCharaBase::CalcDamage( const CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength, CGameCharaBase* pKiller)
{
	REAL32 rValidDamageRate = 1.f;
	// 월샷에 대한 두께 기능 추가 by dfly79 20071227
	if( pDamageInfo->_WallShotReduceRate < 100 && pDamageInfo->_WallShotReduceRate > 0)
	{
		rValidDamageRate = 1.f - ((REAL32)pDamageInfo->_WallShotReduceRate * 0.01f);
	}

	// 거리에 따른 감소율 처리
	rValidDamageRate *= _CalcDistanceRate( const_cast<CHARA_DAMAGEINFO*>(pDamageInfo), rDamagedLength, pKiller);

	rValidDamageRate = MINMAX( 0.f, rValidDamageRate, 1.f);

	I3ASSERT( g_pCharaInfoDataBase->getGlobal() != nullptr);
	I3ASSERT( m_pCharaInfo != nullptr );

	REAL32 rDamage = _CalcDamageToVersion( pDamageInfo, rValidDamageRate);

	if(MyRoomInfoContext::i()->getStage()->IsAiMode() )
	{
		if( BattleSlotContext::i()->getMyTeam() != getCharaTeam() )
		{	
			INT32 i = 0;
			INT32 nPlayerCount = 0;
			if( getCharaTeam() & CHARACTER_TEAM_RED)	
			{
				for( i=1; i<SLOT_MAX_COUNT; i+=2)
				{
					// AI가 RedTeam이므로.. Blue팀의 플레이 인원을 구합니다.						
					const SLOT_INFO * pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);
					if(pSlotInfo->_State == SLOT_STATE_BATTLE) nPlayerCount++;
				}
			}
			else
			{
				for( i=0; i<SLOT_MAX_COUNT; i+=2)
				{
					// AI가 BlueTeam이므로.. Red팀의 플레이 인원을 구합니다.
					const SLOT_INFO * pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);
					if(pSlotInfo->_State == SLOT_STATE_BATTLE) nPlayerCount++;
				}
			}

			if( nPlayerCount > 0)
			{
				AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[getCharaNetIndex()];
				rDamage = rDamage * ( g_pTempConfig->m_Damage[pAiEntry->_level] - g_pTempConfig->m_PlayerCountPerDamage[nPlayerCount-1]);
			}
			else
			{
				I3_BOUNDCHK(getCharaNetIndex(),SLOT_MAX_COUNT);
				AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[getCharaNetIndex()];
				AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData( pAiEntry->_number );
				rDamage = rDamage * ( pBindData->_ReduceDamage - pBindData->_ReduceDamage_PlayerCount[nPlayerCount-1] );
			}
		}
	}

	// 반올림
	rDamage += 0.5f;

	
	if(rDamage < 1.f)
	{
		rDamage = 1.f;
	}

	// 절삭처리해야 됩니다.
	return (INT32) rDamage;
}

void CGameCharaBase::setLeftHand( bool bFlag)
{
	if( bFlag)	addCharaFlag( CHARA_FLAG_LEFT_HAND);
	else		removeCharaFlag( CHARA_FLAG_LEFT_HAND);

	if( m_pMaterialContext != nullptr)
		m_pMaterialContext->setLeftMode( bFlag);
}


bool CGameCharaBase::IsPBMark(UINT32 uiMark)
{
	if(uiMark == 0 || uiMark == 0xFFFFFFFF)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CGameCharaBase::CharaChangeClanMark(UINT32 iClanMark)
{
	INT32 i;

	// PB 마크
	if(IsPBMark(iClanMark))
	{
		for( i = 0; i < AVT_LOD_COUNT; i++)
		{
			if (m_arClanMarkTexBindAttr[i])
				m_arClanMarkTexBindAttr[i]->SetTexture( m_pPBMarkTexture);
		}
	}
	else 
	{
		if( (INT32)iClanMark != m_iClanMark)
		{
			MakeClanMarkTex( m_pClanMarkTexture, CLAN_MARK_MEDIUM, iClanMark, 6, 6);
		}

		for( i = 0; i < AVT_LOD_COUNT; i++)
		{
			if( m_arClanMarkTexBindAttr[ i])
				m_arClanMarkTexBindAttr[ i]->SetTexture(m_pClanMarkTexture);
		}
	}
}

void CGameCharaBase::CreateClanMarkTex()
{
	if(m_pClanMarkTexture != nullptr)
	{
		return;
	}

	m_pClanMarkTexture = i3Texture::new_object(); 
	I3ASSERT( m_pClanMarkTexture);
#if defined( I3_DEBUG)
	m_pClanMarkTexture->SetName( "ClanMark");
#endif

	m_pClanMarkTexture->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

	m_pPBMarkTexture = g_pFramework->GetResourceManager()->LoadTexture("Avatar/Clan_PointBlank_Diff.i3i");
}

void CGameCharaBase::InitClanMark()
{
	INT32 i;

	if(m_spBlendEnableAttr == nullptr)
	{
		m_spBlendEnableAttr = i3BlendEnableAttr::new_object();
	}

	if(m_spBlendModeAttr == nullptr)
	{
		m_spBlendModeAttr = i3BlendModeAttr::new_object();
	}

	m_spBlendEnableAttr->Set(true);
	m_spBlendModeAttr->SetSource( I3G_BLEND_SRCALPHA);
	m_spBlendModeAttr->SetDestination(I3G_BLEND_INVSRCALPHA);

	i3LOD * pLOD;

	for( i = 0; i < AVT_LOD_COUNT; i++)
	{
		pLOD = getSceneObject()->GetBody()->getLOD( i);

		if( pLOD != nullptr)
		{
			m_arClanMarkAttrSet[i] = (i3AttrSet*) CharaTextureUtil::FindNodeByContainName( pLOD->getShapeNode(), "MODEL_CLAN");
			if( m_arClanMarkAttrSet[i] != nullptr)
			{
				m_arClanMarkTexBindAttr[i] = static_cast<i3TextureBindAttr*>(i3Scene::FindAttrByExactType( m_arClanMarkAttrSet[i], i3TextureBindAttr::static_meta()));
				I3ASSERT( m_arClanMarkTexBindAttr[i] != nullptr);

				m_arClanMarkAttrSet[i]->AddAttr(m_spBlendEnableAttr);
				m_arClanMarkAttrSet[i]->AddAttr(m_spBlendModeAttr);

				// 나머지 텍스쳐는 삭제
				i3RenderAttr * pAttr;
				{
					pAttr = i3Scene::FindAttrByExactType( m_arClanMarkAttrSet[i], i3NormalMapEnableAttr::static_meta());
					if( pAttr != nullptr)
						m_arClanMarkAttrSet[i]->RemoveAttr( pAttr);

					pAttr = i3Scene::FindAttrByExactType( m_arClanMarkAttrSet[i], i3NormalMapBindAttr::static_meta());
					if( pAttr != nullptr)
						m_arClanMarkAttrSet[i]->RemoveAttr( pAttr);
				}

				{
					pAttr = i3Scene::FindAttrByExactType( m_arClanMarkAttrSet[i], i3SpecularMapEnableAttr::static_meta());
					if( pAttr != nullptr)
						m_arClanMarkAttrSet[i]->RemoveAttr( pAttr);

					pAttr = i3Scene::FindAttrByExactType( m_arClanMarkAttrSet[i], i3SpecularMapBindAttr::static_meta());
					if( pAttr != nullptr)
						m_arClanMarkAttrSet[i]->RemoveAttr( pAttr);
				}

				{
					pAttr = i3Scene::FindAttrByExactType( m_arClanMarkAttrSet[i], i3ReflectMaskMapEnableAttr::static_meta());
					if( pAttr != nullptr)
						m_arClanMarkAttrSet[i]->RemoveAttr( pAttr);

					pAttr = i3Scene::FindAttrByExactType( m_arClanMarkAttrSet[i], i3ReflectMaskMapBindAttr::static_meta());
					if( pAttr != nullptr)
						m_arClanMarkAttrSet[i]->RemoveAttr( pAttr);
				}
			}
			else
			{	// 해당 LOD에 클랜마크가 없다..?
				m_arClanMarkTexBindAttr[i] = nullptr;
			}
		}
		else
		{
			m_arClanMarkAttrSet[i] = nullptr;
			m_arClanMarkTexBindAttr[i] = nullptr;
		}
	}

}

void	CGameCharaBase::BindAI(AI_BIND_DATA* pData)
{
	m_pInputContext->BindAI(pData);
}

void CGameCharaBase::__Instantiate( void)
{
	MATRIX mat;
	I3_ONINSTANCING_INFO info;
	info._pBase = this;
	info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
	
	/*캐릭터 생성시 겹치지 않도록 합니다.*/
	i3Matrix::SetPos( &mat, (REAL32)getCharaNetIndex(), g_pConfigEx->GetPhysix().HeightChara, (REAL32)-getCharaNetIndex());
	
	info._pMatrixStack->Push( &mat, nullptr);
	OnInstancing( &info);
	info._pMatrixStack->Pop();
}

void CGameCharaBase::__ResetInvincibleTime( void)
{
	m_rTotalAbility[ CHARA::eABILITY_INVINCIBLE_TIME] = MyRoomInfoContext::i()->GetInvincibleTime();

	////-----------------	무적시간 +1sec권
	//	사용자가 사용중이면 무적시간 증가. (자신 사용)
	REAL32 rItemSkillValue = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_INVINCIBLE_TIME);
	CASHITEM_TRACE("CASHITEMSKILL_INVINCIBLE_TIME = %f\n", rItemSkillValue);
	m_rTotalAbility[ CHARA::eABILITY_INVINCIBLE_TIME] += rItemSkillValue;

	// 장비에 의한 무적시간 증가
	if( getEquipContext() != nullptr)
	{
		m_rTotalAbility[ CHARA::eABILITY_INVINCIBLE_TIME] += getEquipContext()->GetAbility( EQUIP::eABILITY_INVINCIBLE_TIME);
	}
}

void CGameCharaBase::__ResetJumpHeightRate( void)
{
	const NET_CHARA_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( getCharaNetIndex());

	// 스킬 점프력
	// m_rTotalAbility[ CHARA::eABILITY_JUMPHEIGHT] = m_rJumpHeightOffset;		// 여기다 넣으면 안된다. 버그임. (2015.05.22.수빈)
	m_rTotalAbility[ CHARA::eABILITY_JUMPHEIGHT] = pInfo->getCharaSkillValue(SKILLLIST_JUMP);
	
	// 장비 점프력 증가를 설정한다.
	if( getEquipContext() != nullptr)
		m_rTotalAbility[ CHARA::eABILITY_JUMPHEIGHT] += getEquipContext()->GetAbility( EQUIP::eABILITY_JUMPHEIGHT);
}

void CGameCharaBase::__ResetThrowSpeed( void)
{
	m_rTotalAbility[ CHARA::eABILITY_THROWSPEED] = 0.f;

	if( getEquipContext() != nullptr)
	{
		m_rTotalAbility[ CHARA::eABILITY_THROWSPEED] = getEquipContext()->GetAbility( EQUIP::eABILITY_THROWSPEED);
	}
}

void CGameCharaBase::__ResetFullHP( void)
{
	I3ASSERT( m_pCharaInfo != nullptr );

	m_iFullHP = 100;		//	default
	INT32 nMax = m_iFullHP;

	if( m_pDBInfo != nullptr)
	{
		m_iFullHP = nMax = getDBInfo()->GetHP(); 			// HP
	}

	if( m_pCharaInfo->GetAIToClient())
	{
		return;
	}

	// 장비에 따른 HP 증가
	if( getEquipContext() != nullptr)
	{
		m_iFullHP += static_cast<INT32>( (nMax * getEquipContext()->GetAbility( EQUIP::eABILITY_HPINCREASE)) );
	}


	//	유저가 Mega HP 10% 캐시 아이템을 사용하고 있다면 HP 증가.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_HP_RATE);
	m_iFullHP += (INT32)((REAL32)nMax * ratio);
	CASHITEM_TRACE("CASHITEMSKILL_HP_RATE = %f\n", ratio);

	INT32 addHP = (INT32)BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_HP_ADD);
	m_iFullHP += addHP;
	CASHITEM_TRACE("CASHITEMSKILL_HP_ADD = %d\n", addHP);
}

void	CGameCharaBase::__ResetDesigOffsetDatas(void)
{
	
	const NET_CHARA_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( getCharaNetIndex());	
	
	const DesigApplyInfo* desig_apply_info = pInfo->GetDesigApplyInfo();

	if ( desig_apply_info )		// 없을수 있다.
	{
		this->SetMoveSpeedAddOffset(desig_apply_info->desigChara.charaMoveSpeed);
		this->SetJumpHeightOffset(desig_apply_info->desigChara.charaJump);
		this->SetAccelerationTimeOffset(desig_apply_info->desigChara.charaAddSpeed);
		this->SetFallDamageMinOffset(desig_apply_info->desigChara.charaFallDamageReduce);
	}
}

void CGameCharaBase::__ResetMovementSpeed( void)
{
	// (제압공룡은 적용 안합니다.)
	for (INT32 i = 0; i < MDT_COUNT; i++)
	{
		m_rMovementSpeedRate[MST_WALK][i] = 0.f;
		m_rMovementSpeedRate[MST_RUN][i]  = 0.f;
		m_rMovementSpeedRate[MST_JUMP_RUN][i] = 0.f;
		m_rMovementSpeedRate[MST_JUMP_WALK][i] = 0.f;
	}
	if( IsDomiDino() || IsDino())
		return;

	//스킬시스템 증가값
	const NET_CHARA_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( getCharaNetIndex());
	REAL32 iItemlAdd = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_MOVE_RATE);
	CASHITEM_TRACE("CASHITEMSKILL_MOVE_RATE = %f\n", iItemlAdd);

	for( INT32 i = 0; i < MDT_COUNT; i++)
	{
		m_rMovementSpeedRate[ MST_WALK][ i] = pInfo->getCharaSkillValue(SKILLLIST_WALK);
		m_rMovementSpeedRate[ MST_RUN][ i]  = pInfo->getCharaSkillValue(SKILLLIST_RUN);
	}
	
	// Acceleration Time이 Run skill이 아무 레벨이라도 장착 되어 있으면 최소치로 낮춘다.
	if(pInfo->getCharaSkillValue(SKILLLIST_RUN) > 0)
		m_rAccelerationTimeOffset_RunSkill = -0.3f;
	else
		m_rAccelerationTimeOffset_RunSkill = 0.0f;

	for( INT32 i = 0; i < MDT_COUNT; i++)
	{
		m_rMovementSpeedRate[ MST_WALK][ i]    +=  iItemlAdd;
		m_rMovementSpeedRate[ MST_RUN][ i]     +=  iItemlAdd;
	}

	// 장비
	if( getEquipContext() != nullptr)
	{
		for( INT32 i = 0; i < MST_COUNT; i++)
		{
			//점프 워크 이상의 무브 스타일 탑입은 안먹히도록 일단 고정 시켜 놓음.
			if (i >= MST_JUMP_WALK) continue;

			for( INT32 j = 0; j < MDT_COUNT; j++)
			{
				switch( j)
				{
				case MDT_FRONT :	m_rMovementSpeedRate[ i][ j] += getEquipContext()->GetAbility( EQUIP::eABILITY_MOVESPEED_FRONT);	break;
				case MDT_SIDE :		m_rMovementSpeedRate[ i][ j] += getEquipContext()->GetAbility( EQUIP::eABILITY_MOVESPEED_SIDE);	break;
				case MDT_DIAGONAL :	m_rMovementSpeedRate[ i][ j] += getEquipContext()->GetAbility( EQUIP::eABILITY_MOVESPEED_DIAGONAL);	break;
				case MDT_BACK :		m_rMovementSpeedRate[ i][ j] += getEquipContext()->GetAbility( EQUIP::eABILITY_MOVESPEED_BACK);	break;
				}
			}
		}
	}

}

void CGameCharaBase::__ResetDefenceRate( void)
{
	REAL32 defaultRate = 0.f;

	if( IsDomiDino() == false)
	{
		//스킬 능력치 레이팅 값
		const NET_CHARA_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( getCharaNetIndex());
		defaultRate +=  pInfo->getCharaSkillValue(SKILLLIST_DEFENSE);

		REAL32	proofRate = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_DEFENSE_RATE);
		REAL32	proofOverlap = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_DEFENSE_OVERLAP);
		CASHITEM_TRACE("CASHITEMSKILL_DEFENSE_RATE = %f\n", proofRate);
		CASHITEM_TRACE("CASHITEMSKILL_DEFENSE_OVERLAP = %f\n", proofOverlap);
		defaultRate += proofRate+proofOverlap;



		if( getEquipContext() != nullptr)
		{
			for( INT32 i = 0; i < CHARA::eDAMAGEPART_COUNT; i++)
			{
				m_rDefenceRate[i] = defaultRate;

				switch( i)
				{
				case CHARA::eDAMAGEPART_BODY :	m_rDefenceRate[i] += getEquipContext()->GetAbility( EQUIP::eABILITY_DEFENSE_BODY);	break;
				case CHARA::eDAMAGEPART_ARM:	m_rDefenceRate[i] += getEquipContext()->GetAbility( EQUIP::eABILITY_DEFENSE_ARM);	break;
				case CHARA::eDAMAGEPART_HAND:	m_rDefenceRate[i] += getEquipContext()->GetAbility( EQUIP::eABILITY_DEFENSE_HAND);	break;
				case CHARA::eDAMAGEPART_LEG:	m_rDefenceRate[i] += getEquipContext()->GetAbility( EQUIP::eABILITY_DEFENSE_LEG);	break;
				case CHARA::eDAMAGEPART_FOOT:	m_rDefenceRate[i] += getEquipContext()->GetAbility( EQUIP::eABILITY_DEFENSE_FOOT);	break;
				}
			}
		}
	}
	else
	{
		for( INT32 i = 0; i < CHARA::eDAMAGEPART_COUNT; i++)
		{
			m_rDefenceRate[i] = 0.f;
		}
	}
}

void CGameCharaBase::__ResetWeaponSwapTimeRate( void)
{
	m_rTotalAbility[ CHARA::eABILITY_SWAPTIME] = 0.f;

	//	빠른 무기 변경 아이템 사용시 시간 감소.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_SWAPWEAPON_RATE);
	CASHITEM_TRACE("CASHITEMSKILL_SWAPWEAPON_RATE = %f\n", ratio);

	m_rTotalAbility[ CHARA::eABILITY_SWAPTIME] = ratio;

	if( getEquipContext() != nullptr)
	{
		m_rTotalAbility[ CHARA::eABILITY_SWAPTIME] += getEquipContext()->GetAbility( EQUIP::eABILITY_SWAPTIME);
	}
}

WeaponBase*	CGameCharaBase::GetCharaWeapon( WEAPON_SLOT_TYPE slot) const
{ 
	if( getWeaponContext() != nullptr)
		return getWeaponContext()->getWeapon( slot);
	return nullptr;
}

WeaponBase*	CGameCharaBase::GetCurrentCharaWeapon() const
{ 

	if ( getWeaponContext() != nullptr)
	{
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			// 다중무기 시스템 사용 중일 경우 
			// 주무기 - 다중무기에서, 보조무기 - 기존 루틴
			if ( getWeaponContext()->getCurrentSlotNum() == WEAPON_SLOT_PRIMARY
				&& getMultiWeaponContext()->GetCurrSlotInfo() != nullptr)
			{
				return getMultiWeaponContext()->GetCurrSlotInfo()->pWeapon;
			}
			else
			{
				return getWeaponContext()->getCurrent();
			}
		}
		else
		{
			return getWeaponContext()->getCurrent();
		}
	}
	return nullptr;
}

void CGameCharaBase::SetGravityRecovery(bool Enable)
{
	if (getMoveContext() != nullptr)
		getMoveContext()->SetGravityRecovery(Enable);
}

INT32 CGameCharaBase::GetFallMinDamage() const
{ 
	if(IsDino()) 
	{
		return g_pCharaInfoDataBase->getDinoGlobal()->GetFallMinDamage(); 
	}
	else
	{
		return g_pCharaInfoDataBase->getGlobal()->GetFallMinDamage(); 
	}
	
}

REAL32 CGameCharaBase::GetFallDamageMinHeight() const
{ 
	if(IsDino()) 
	{
		return g_pCharaInfoDataBase->getDinoGlobal()->GetFallDamageMinHeight(); 
	}
	else
	{
		return g_pCharaInfoDataBase->getGlobal()->GetFallDamageMinHeight(); 
	}
}

REAL32 CGameCharaBase::GetFallDamageSlope() const
{ 
	if(IsDino()) 
	{
		return g_pCharaInfoDataBase->getDinoGlobal()->GetFallDamageSlope(); 
	}
	else
	{
		return g_pCharaInfoDataBase->getGlobal()->GetFallDamageSlope(); 
	}
}

INT32 CGameCharaBase::getCurHP_Percent( void) const
{ 
	REAL32 curHP = (REAL32)getCurHP();
	REAL32 rMaxHP = (REAL32)getFullHP();

	if( curHP * 100.0f < rMaxHP ) {
		if( curHP > 0.f) {
			return 1;
		}
		else {
			return 0;
		}
	}
	
	return (INT32)( curHP * 100.0f / rMaxHP); 
}

INT32 CGameCharaBase::getFullHP( void) const
{
	I3ASSERT( m_pCharaInfo != nullptr);

	INT32 hp = m_iFullHP;

	return hp;
}

INT32 CGameCharaBase::getCurHPAppliedCashItem( void) const
{ 
	I3ASSERT( m_pCharaInfo != nullptr );

	INT32 nDefaultHp = 100;		//	default

	if( m_pDBInfo != nullptr)
	{
		nDefaultHp = getDBInfo()->GetHP(); 			// HP
	}

	INT32 nCurHP = getCurHP();

	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_HP_RATE);
	REAL32 HpAdd = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_HP_ADD);
	CASHITEM_TRACE("CASHITEMSKILL_HP_RATE = %f\n", ratio);
	CASHITEM_TRACE("CASHITEMSKILL_HP_ADD = %f\n", HpAdd);
	nCurHP += (INT32)((REAL32)nDefaultHp * ratio);
	nCurHP += (INT32)HpAdd;

	return nCurHP;
}

void CGameCharaBase::addEnableStateMask( UINT32 mask)
{ 
	m_EnableStateMask |= mask; 

	// GlobalFunc::PB_TRACE("addEnableStateMask m_EnableStateMask %x mask %x", m_EnableStateMask, mask);
}

void CGameCharaBase::removeEnableStateMask( UINT32 mask)
{
	m_EnableStateMask &= ~mask;

	//if( is1PV())
	//{
	//	if( mask & CHARA_STATE_DISABLE_SHOT)
	//	{
	//		mask |= mask;
	//	}
	//}
	// GlobalFunc::PB_TRACE("removeEnableStateMask m_EnableStateMask %x mask %x", m_EnableStateMask, mask);
}

void CGameCharaBase::PreloadAnimationByWeapon( T_ItemID itemID)
{
	getAnimContext()->_PreloadAnimation( itemID);
}

bool CGameCharaBase::IsNetworkAttackReturnUpper()
{
	REAL32 duration = getAnimContext()->getUpperActualDuration();

	//네트워크 싱크 문제로 offset 적용. (참조 UDPSENDTIME_NOMOVE )
	return m_iNetworkAttackCount > (duration + 1.0f) ;
}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)

void CGameCharaBase::DumpChara( void)
{
	UINT32 nCharaState;	
	char szAILowerAI[256] = "(null)";
	char szAIUpperAI[256] = "(null)";

	I3ASSERT( m_pCharaInfo != nullptr );
	INT32 nNetIdx = getCharaNetIndex();

	i3::rc_wstring wstrNickName = BattleSlotContext::i()->getNickForSlot( nNetIdx);
	if( wstrNickName.empty() )
		wstrNickName = L"null";

	I3PRINTLOG(I3LOG_NOTICE,"---------------------------------------------------");
	I3PRINTLOG(I3LOG_NOTICE,"<< Dump Character >>");	
	I3PRINTLOG(I3LOG_NOTICE, i3::format_string("- Chara ID : %s (%d)", wstrNickName, nNetIdx ) );
	I3PRINTLOG(I3LOG_NOTICE, "- this : 0x%p", this );
	I3PRINTLOG(I3LOG_NOTICE, "- InvincibleElapseTime = %.5f", m_rInvincibleElapseTime );
	I3PRINTLOG(I3LOG_NOTICE, "- CharaTask = %d", g_pCharaManager->getCharaTask( nNetIdx ) );
	I3PRINTLOG(I3LOG_NOTICE, "- HP : %f", m_pCharaInfo->GetHP());

	
	I3PRINTLOG(I3LOG_NOTICE, "- Visible : %d", isVisible());
	I3PRINTLOG(I3LOG_NOTICE, "- Enable : %d", isEnable());
	I3PRINTLOG(I3LOG_NOTICE, "- Current Pos : %.4f, %.4f, %.4f", GetPos()->x, GetPos()->y, GetPos()->z);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	switch( getActionContext()->getBodyLower())
	{
	case CHARA_BODYLOWER_STAND:		I3PRINTLOG(I3LOG_NOTICE, "- Body State : CHARA_BODYLOWER_STAND");		break;
	case CHARA_BODYLOWER_CROUCH:	I3PRINTLOG(I3LOG_NOTICE, "- Body State : CHARA_BODYLOWER_CROUCH");		break;
	default:						I3PRINTLOG(I3LOG_NOTICE, "- Body State : Unknown");					break;
	}

	switch( getActionContext()->getBodyUpper())
	{
	case CHARA_BODYUPPER_NONE:			I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : CHARA_BODYUPPER_NONE");			break;
	case CHARA_BODYUPPER_ATTACKIDLE:	I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : CHARA_BODYUPPER_ATTACKIDLE");	break;	
	case CHARA_BODYUPPER_ZOOM:			I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : CHARA_BODYUPPER_ZOOM");			break;
	case CHARA_BODYUPPER_GRENADE:		I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : CHARA_BODYUPPER_GRENADE");		break;
	case CHARA_BODYUPPER_SHOTGUN:		I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : CHARA_BODYUPPER_SHOTGUN");		break;
	case CHARA_BODYUPPER_KNIFE:			I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : CHARA_BODYUPPER_KNIFE");			break;
	case CHARA_BODYUPPER_CROSSFIRE:		I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : CHARA_BODYUPPER_CROSSFIRE");		break;
	default:							I3PRINTLOG(I3LOG_NOTICE, "- Body2 State : Unknown");						break;
	}

	switch( getActionContext()->getLower())
	{
	case CHARA_LOWER_NONE:		I3PRINTLOG(I3LOG_NOTICE, "- Lower State : CHARA_LOWER_NONE");		break;
	case CHARA_LOWER_IDLE:		I3PRINTLOG(I3LOG_NOTICE, "- Lower State : CHARA_LOWER_IDLE");		break;
	case CHARA_LOWER_RUN:		I3PRINTLOG(I3LOG_NOTICE, "- Lower State : CHARA_LOWER_RUN");		break;
	case CHARA_LOWER_JUMP:		I3PRINTLOG(I3LOG_NOTICE, "- Lower State : CHARA_LOWER_JUMP");		break;
	case CHARA_LOWER_DEATH:		I3PRINTLOG(I3LOG_NOTICE, "- Lower State : CHARA_LOWER_DEATH");		break;
	case CHARA_LOWER_WALK:		I3PRINTLOG(I3LOG_NOTICE, "- Lower State : CHARA_LOWER_WALK");		break;
	default:					I3PRINTLOG(I3LOG_NOTICE, "- Lower State : Unknown");				break;
	}

	switch( getActionContext()->getUpper())
	{
	case CHARA_UPPER_NONE:						I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_NONE");						break;
	case CHARA_UPPER_IDLE:						I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_IDLE");						break;
	case CHARA_UPPER_MOVE:						I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_MOVE");						break;
	case CHARA_UPPER_FIRE:						I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_FIRE");						break;
	case CHARA_UPPER_FIRE_READY:				I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_FIRE_READY");				break;
	case CHARA_UPPER_SECONDARY_FIRE:			I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_SECONDARY_FIRE");			break;
	case CHARA_UPPER_LOADMAGAZINE:				I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_LOADMAGAZINE");				break;
	case CHARA_UPPER_DAMAGE:					I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_DAMAGE");					break;
	case CHARA_UPPER_SWAPWEAPON:				I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_SWAPWEAPON");				break;
	case CHARA_UPPER_JUMP:						I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_JUMP");						break;
	case CHARA_UPPER_EXTENSION_ACTIVATE:		I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_EXTENSION_ACTIVATE");		break;
	case CHARA_UPPER_EXTENSION_DEACTIVATE:		I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_EXTENSION_DEACTIVATE");		break;
	case CHARA_UPPER_REMOVESHELL:				I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_REMOVESHELL");				break;
	case CHARA_UPPER_LOADMAG_READY:				I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_LOADMAG_READY");				break;
	case CHARA_UPPER_FIREBULLET:				I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_FIREBULLET");				break;
	case CHARA_UPPER_NONEBATTLE_IDLE:			I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_NONEBATTLE_IDLE");			break;
	case CHARA_UPPER_NONEBATTLE_WEAPONSHOW:		I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_NONEBATTLE_WEAPONSHOW");		break;
	case CHARA_UPPER_NONEBATTLE_IDLE_CHANGE:	I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_NONEBATTLE_IDLE_CHANGE");	break;
	case CHARA_UPPER_NONEBATTLE_IDLE_STAND:		I3PRINTLOG(I3LOG_NOTICE, "- Upper State : CHARA_UPPER_NONEBATTLE_IDLE_STAND");		break;
	default:									I3PRINTLOG(I3LOG_NOTICE, "- Upper State : Unknown");								break;
	}

	switch( getViewType())
	{
	case CHARA_VIEW_1PV:			I3PRINTLOG(I3LOG_NOTICE, "- AnimSet : CHARA_VIEW_1PV");		break;
	case CHARA_VIEW_3PV:			I3PRINTLOG(I3LOG_NOTICE, "- AnimSet : CHARA_VIEW_3PV");		break;
	default:						I3PRINTLOG(I3LOG_NOTICE, "- AnimSet : Unknown");				break;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	현재 하체 동작을 저장
	if( m_pAnimContext->isAILower())
	{
		if( m_pAnimContext->getAILower()->getCurrentAIState() != nullptr)
		{
			i3::safe_string_copy( szAILowerAI, m_pAnimContext->getAILower()->getCurrentAIState()->GetName(), 256 );
		}
	}

	//	현재 상체 동작을 저장
	if( m_pAnimContext->isAIUpper())
	{
		if( m_pAnimContext->getAIUpper()->getCurrentAIState() != nullptr)
		{
			i3::safe_string_copy( szAIUpperAI, m_pAnimContext->getAIUpper()->getCurrentAIState()->GetName(), 256 );
		}
	}

	I3PRINTLOG(I3LOG_NOTICE, "- Cur Lower AI : %s", szAILowerAI);
	I3PRINTLOG(I3LOG_NOTICE, "- Cur Upper AI : %s", szAIUpperAI);

	if( m_pAnimContext->getOldLowerID() == ID_LOWER_UNKNOWN)
	{
		I3PRINTLOG(I3LOG_NOTICE, "- Old Lower AI : Unknown (ID: %d)", m_pAnimContext->getOldLowerID());
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "- Old Lower AI : %s (ID: %d)", g_szNameLowerAI [ m_pAnimContext->getOldLowerID()], m_pAnimContext->getOldLowerID());
	}

	if( m_pAnimContext->getOldLowerID() == ID_LOWER_UNKNOWN)
	{
		I3PRINTLOG(I3LOG_NOTICE, "- Old Upper AI : Unknown (ID: %d)", m_pAnimContext->getOldUpperID());	
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "- Old Upper AI : %s (ID: %d)", g_szNameUpperAI [ m_pAnimContext->getOldUpperID()], m_pAnimContext->getOldUpperID());	
	}

	switch( getWeaponContext()->getAttachHand())
	{
	case CHARA_WEAPON_GRIP_RIGHT_HAND:		I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Hand : CHARA_WEAPON_GRIP_RIGHT_HAND");	break;
	case CHARA_WEAPON_BARREL_LEFT_HAND:		I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Hand : CHARA_WEAPON_BARREL_LEFT_HAND");	break;
	case CHARA_WEAPON_GRIP_LEFT_HAND :		I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Hand : CHARA_WEAPON_GRIP_LEFT_HAND");	break;
	case CHARA_WEAPON_BARREL_RIGHT_HAND:	I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Hand : CHARA_WEAPON_BARREL_RIGHT_HAND");	break;
	}

	nCharaState	= getEnableStateMask();
	I3PRINTLOG(I3LOG_NOTICE, "- Enable State Mask : 0x%x", nCharaState);

	if( nCharaState & CHARA_STATE_DISABLE_CONTROL)
	{
		I3PRINTLOG(I3LOG_NOTICE, "On : CHARA_STATE_DISABLE_CONTROL");	
	}

	if( nCharaState & CHARA_STATE_DISABLE_SHOT)
	{
		I3PRINTLOG(I3LOG_NOTICE, "On : CHARA_STATE_DISABLE_SHOT");		
	}

	if( nCharaState & CHARA_STATE_DISABLE_MOVE)
	{
		I3PRINTLOG(I3LOG_NOTICE, "On : CHARA_STATE_DISABLE_MOVE");		
	}

	if( nCharaState & CHARA_STATE_DISABLE_CROUCH)
	{
		I3PRINTLOG(I3LOG_NOTICE, "On : CHARA_STATE_DISABLE_BODY");
	}

	if( nCharaState & CHARA_STATE_DISABLE_WEAPONCHANGE)
	{
		I3PRINTLOG(I3LOG_NOTICE, "On : CHARA_STATE_DISABLE_WEAPONCHANGE");	
	}

	if( nCharaState & CHARA_STATE_USED_COMBINE_ANIM)
	{
		I3PRINTLOG(I3LOG_NOTICE, "On : CHARA_STATE_USED_COMBINE_ANIM");	
	}

	//	현재 무기 덤프
	WeaponBase * pWeapon = GetCurrentCharaWeapon();
	if( pWeapon != nullptr)
	{
		pWeapon->DumpState();
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "NO Weapon");
	}

	//	장비품 덤프
	if(m_pEquipContext)
	{
		for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
		{
			CGameCharaEquip * pEquip = m_pEquipContext->getEquip( (EQUIP::ePART) i);

			if( pEquip != nullptr)
				pEquip->DumpState();
		}
	}

	I3PRINTLOG(I3LOG_NOTICE,"---------------------------------------------------");
}

#endif

REAL32 CGameCharaBase::_CalcDamageToVersion(const CHARA_DAMAGEINFO * pDamageInfo, REAL32 rValidDamageRate)
{
	REAL32 rDamage = 0.0f;

	if (g_pEnvSet->IsV2Version())
	{
		//방어 아이템 장비 스킬 계산
		REAL32 rDamageRate = GetDamageRateByDefence(pDamageInfo->GetHitPart());

		// 최종 데미지량 연산  (방어력에 의한 비율 * 무기 공격력 * 관통 및 거리에 대한 비율)
		rDamage = (rDamageRate) * pDamageInfo->_rDamage * rValidDamageRate;
	}
	else
	{
		EQUIP::eABILITY defense_ability = EQUIP::eABILITY_DEFENSE_BODY;
		CHARA::eDAMAGEPART hit_parts = CHARA::GetDamagePartByHit( pDamageInfo->GetHitPart() );
		
		switch ( hit_parts )
		{
		case CHARA::eDAMAGEPART_BODY:	defense_ability = EQUIP::eABILITY_DEFENSE_BODY;	break;
		case CHARA::eDAMAGEPART_ARM:	defense_ability = EQUIP::eABILITY_DEFENSE_ARM;	break;
		case CHARA::eDAMAGEPART_HAND:	defense_ability = EQUIP::eABILITY_DEFENSE_HAND;	break;
		case CHARA::eDAMAGEPART_LEG:	defense_ability = EQUIP::eABILITY_DEFENSE_LEG;	break;
		case CHARA::eDAMAGEPART_FOOT:	defense_ability = EQUIP::eABILITY_DEFENSE_FOOT;	break;
		}

		REAL32 parts_defense_rate = 0.0f;
		
		if( !IsDino() && m_pEquipContext)
			parts_defense_rate = m_pEquipContext->GetAbility(defense_ability);
		
		//이거 음수로 pef에서 써서 +로 해야됩니다!
		REAL32 rDamageRate = 1.0f + parts_defense_rate;

		// 파츠 / 거리별 데미지 감소 적용
		rDamage = (rDamageRate) * pDamageInfo->_rDamage * rValidDamageRate;

		// 아이템 데미지 감소 적용
		REAL32	item_defense_rate = BattleSlotContext::i()->GetUseCashItemValue(getCharaNetIndex(), CASHITEMSKILL_DEFENSE_OVERLAP);
		rDamage = rDamage * (1.0f - item_defense_rate);
	}

	return rDamage;
}
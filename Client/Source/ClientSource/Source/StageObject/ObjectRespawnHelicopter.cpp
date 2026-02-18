#include "pch.h"
#include "ObjectRespawnHelicopter.h"

#include "GameCharaInputContext.h"
#include "Camera/GameCamera_OnObject.h"
#include "WeaponM197.h"
#include "StageBattle.h"
#include "./StageBattle/ScoreContext.h"

#include "BattleHud_Helicopter.h"

#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"

const char * s_PathName_Breakdown2_Helicopter[ RESPAWN_HELICOPTER_PATH_COUNT] =
{
	"",
	"Object/cgame/Helispot/UH60_R_Path1.i3a",	// 0 group
	"Object/cgame/Helispot/UH60_R_Path2.i3a",
	"Object/cgame/Helispot/UH60_B_Path1.i3a",
	"Object/cgame/Helispot/UH60_B_Path2.i3a",

	"Object/cgame/Helispot/UH60_R_Path3.i3a",	// 1 group
	"Object/cgame/Helispot/UH60_R_Path4.i3a",
	"Object/cgame/Helispot/UH60_B_Path3.i3a",
	"Object/cgame/Helispot/UH60_B_Path4.i3a",
};

const char * s_SoundName_Breakdown2_Helicopter =
{	
	"OB_Dust_Uh60_Ambient.wav",
};

static INT32 s_nRespawnHelicopterPathKey = -1;

I3_CLASS_INSTANCE( CGameObjectRespawnHelicopter);//, CGameControledObject);

CGameObjectRespawnHelicopter::CGameObjectRespawnHelicopter() : m_bUseObject_Flag(false)
															, m_iUseObject_Count(0)
															, m_bLeaveWeapon_Flag(false)
															, m_iLeaveWeapon_Count(0)
															, m_bEnterWeapon_Flag(false)
															, m_iEnterWeapon_Count(0)
{
	m_PathKey			= RESPAWN_HELICOPTER_PATH_NONE;
	i3mem::FillZero( &m_pPathAnim, sizeof( i3Animation*) * RESPAWN_HELICOPTER_PATH_COUNT);
	m_rPathLocalTime	= 0.f;
	
	m_bMyEnterObject	= false;
	m_bDestroyed		= false;

	m_rRespawnTime		= 0.f;
	m_rSendCountTime	= 0.f;

	m_pRespawnDummy		= nullptr;

	m_nTeam				= 0;
	m_bFirstAnimSet		= false;

	m_bWarningNotice = false;

	m_pHitShapeSetTrans	= nullptr;
	m_pRootDummy = nullptr;
	m_pRespawnDummy2 = nullptr;
}

CGameObjectRespawnHelicopter::~CGameObjectRespawnHelicopter()
{
	INT32 i;

	for( i = 0; i < RESPAWN_HELICOPTER_PATH_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pPathAnim[ i]);
	}

	s_nRespawnHelicopterPathKey = -1;
}

void CGameObjectRespawnHelicopter::_SetPathAnimKey( RESPAWN_HELICOPTER_PATH pathKey, REAL32 rStartTime)
{
	I3ASSERT( pathKey < RESPAWN_HELICOPTER_PATH_COUNT);

	if( m_pPathAnim[ pathKey] != nullptr)
	{
		m_pSceneObj->PlayAnim( m_pPathAnim[ pathKey], I3_ANIM_BLEND_MODE_REPLACE, rStartTime, 0.f, 1.0f, 1.0f, I3_ANIM_PLAY_MODE_CLAMP, I3_ANIM_PLAY_MODE_STOP);
	}

	m_rPathLocalTime	= rStartTime;
	m_PathKey			= pathKey;
	m_rSendCountTime	= 0.f;

	I3TRACE( "play obj anim.. %d  pathKey : %d %.4f\n", m_bFirstAnimSet, pathKey, rStartTime);
}


static I3SG_TRAVERSAL_RESULT __ProcHitShapeSetChangeForMovement( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
#if 0
	if( i3::same_of<i3Transform2*>(pNode))
	{
		i3Transform2 * pTrans2 = (i3Transform2*)pNode;
		if( pTrans2->getShapeSet() != nullptr)
		{
			if( OBJECT_COLLISION_BY_WEAPON_HIT( pTrans2->getShapeSet()->getShapeGroup() ))
				pTrans2->getShapeSet()->addShapeSetFlag( I3PHYSIX_SHAPESET_FLAG_SETPOS);
		}
	}
	else if( i3::same_of<i3ShapeSetContainer*>(pNode))
	{
		i3ShapeSetContainer * pShapeSetContainer = (i3ShapeSetContainer*) pNode;
		if( pShapeSetContainer->getShapeSet() != nullptr)
		{
			if( OBJECT_COLLISION_BY_WEAPON_HIT( pShapeSetContainer->getShapeSet()->getShapeGroup() ))
				pShapeSetContainer->getShapeSet()->addShapeSetFlag( I3PHYSIX_SHAPESET_FLAG_SETPOS);
		}
	}
	else if( i3::same_of<i3Body*>(pNode))
	{
		INT32 i;
		i3Body * pBody = (i3Body*) pNode;
		if( pBody->getFrameLOD() != nullptr)
		{
			if( pBody->getFrameLOD()->getSkeleton() != nullptr)
			{
				for( i = 0; i < pBody->getFrameLOD()->getSkeleton()->getBoneCount(); i++)
				{
					if( pBody->getFrameLOD()->getSkeleton()->isShapeSet( i))
					{
						i3PhysixShapeSet * pShapeSet = pBody->getFrameLOD()->getSkeleton()->getShapeSet( i);
						if( OBJECT_COLLISION_BY_WEAPON_HIT( pShapeSet->getShapeGroup() ) )
							pShapeSet->addShapeSetFlag( I3PHYSIX_SHAPESET_FLAG_SETPOS);
					}
				}
			}
		}
	}
#endif

	return I3SG_TRAVERSAL_CONTINUE;
}

void CGameObjectRespawnHelicopter::_FirstUpdateAnim( void)
{
	if( s_nRespawnHelicopterPathKey == -1)
	{
		m_bFirstAnimSet = true;
	}
/*
	if( m_bFirstAnimSet )
	{
		INT32 iGroup = g_pRand->GetRand( ScoreContext::i()->RedWinRound + ScoreContext::i()->BlueWinRound);

		s_nRespawnHelicopterPathKey = (i3Math::Rand() % 2) + ((iGroup % 2) * 4);
	}

	if( m_nTeam == 0)
	{
		INT32 nPath = s_nRespawnHelicopterPathKey + 1;
		_SetPathAnimKey( (RESPAWN_HELICOPTER_PATH) nPath);

		I3TRACE( "anim first %d\n", s_nRespawnHelicopterPathKey);
	}
	else
	{
		INT32 nPath = s_nRespawnHelicopterPathKey + 3;
		_SetPathAnimKey( (RESPAWN_HELICOPTER_PATH) nPath);

		I3TRACE( "anim sec %d\n", s_nRespawnHelicopterPathKey);
	}
	*/


	INT32 nPath = CalcAnimObjPath(HitPartContext::i()->HitPartIndex, ScoreContext::i()->getCurrentDediRound(), m_nTeam, s_nRespawnHelicopterPathKey);
	_SetPathAnimKey( (RESPAWN_HELICOPTER_PATH) nPath);


}


void CGameObjectRespawnHelicopter::InitObject()
{
	m_bManualUpdate = true;

	m_bWarningNotice = true;

	// load animation
	InitAnimation();

	m_pRespawnDummy = (i3Transform*)i3Scene::FindNodeByName( getSceneObject(), "RespawnPointDummy");
	I3ASSERT( m_pRespawnDummy != nullptr);

	i3Transform* pTrans = getSceneObject()->FindBoneByName("WeaponPointDummy");
	I3ASSERT( pTrans != nullptr);

	T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY,		WEAPON_CLASS_OBJECT_GUN, 1);
	m_pWeaponOnObject = (WeaponM197*)g_pWeaponManager->QueryWeapon( nullptr, Itemid);

	I3ASSERT(m_pWeaponOnObject);
	m_pWeaponOnObject->InitWeaponOnObject();
	m_pWeaponOnObject->AttachNode( pTrans, this);

	m_pRootDummy = getSceneObject()->FindBoneByName( "Root");
	I3ASSERT( m_pRootDummy != nullptr);

	m_pRespawnDummy2 = getSceneObject()->FindBoneByName("RespawnPointDummy2");

	//i3Scene::Traverse( getSceneObject(), __ProcHitShapeSetChangeForMovement, nullptr);

	_FirstUpdateAnim();
	//EnableObject();

	// 최초 리스폰시 헬기의 위치를 한번 업데이트해야 합니다.
	i3SceneTracer * pTracer;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();

	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( g_pFramework->GetViewer()->getDeltaTime() );

	getSceneObject()->OnUpdate( pTracer);
}

void CGameObjectRespawnHelicopter::ResetObject()
{
	CGameObjectWithWeapon::ResetObject();

	m_rPathLocalTime	= 0.f;
	m_rRespawnTime		= 0.f;
	m_rSendCountTime	= 0.f;

	m_bDestroyed		= false;

	m_bWarningNotice	= true;

	//EnableObject();
	_FirstUpdateAnim();
}

void CGameObjectRespawnHelicopter::InitFlag( void)
{
	m_bUseObject_Flag = false;
	m_iUseObject_Count = 0;

	m_bLeaveWeapon_Flag = false;
	m_iLeaveWeapon_Count = 0;

	m_bEnterWeapon_Flag = false;
	m_iEnterWeapon_Count = 0;
}

void CGameObjectRespawnHelicopter::EnableObject()
{
	/*
	if( m_bFirstAnimSet )
	{
		INT32 iGroup = g_pRand->GetRand( ScoreContext::i()->RedWinRound + ScoreContext::i()->BlueWinRound);

		s_nRespawnHelicopterPathKey = (i3Math::Rand() % 2) + ((iGroup % 2) * 4);
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		if( m_nTeam == 0)
		{
			INT32 nPath = s_nRespawnHelicopterPathKey + 1;
			_SetPathAnimKey( (RESPAWN_HELICOPTER_PATH) nPath);

			I3TRACE( "anim first %d\n", s_nRespawnHelicopterPathKey);
		}
		else
		{
			INT32 nPath = s_nRespawnHelicopterPathKey + 3;
			_SetPathAnimKey( (RESPAWN_HELICOPTER_PATH) nPath);

			I3TRACE( "anim sec %d\n", s_nRespawnHelicopterPathKey);
		}
	}
	*/
}

void CGameObjectRespawnHelicopter::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameObjectWithWeapon::ProcessObject( rDeltaSeconds);

	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object( getNetIndex());

	if( getCurrentHP() == 0.f)
	{	// 파괴되면 보내줘야 합니다.
		m_bDestroyed = true;
	}

	m_rPathLocalTime += rDeltaSeconds;

	if( BattleServerContext::i()->IsP2PHost() )
	{	
		if( m_pPathAnim[ m_PathKey] != nullptr)
		{	
			if( m_rPathLocalTime > m_pPathAnim[ m_PathKey]->GetDuration())
			{	
				EnableObject();

				m_rPathLocalTime = rDeltaSeconds;
			}
		}
	}
	else
	{
		if( (pObjectInfo->_tNetData.getObjectType() & UDP_OBJECT_TYPE_ANIMPATH) ||
			(pObjectInfo->_tNetData.getObjectType() & UDP_OBJECT_TYPE_ANIMKEY) )
		{
			UINT8 path = pObjectInfo->_tNetData.getPathKey();
			REAL32 keyframe = pObjectInfo->_tNetData.getKeyFrame();
			if( (path != m_PathKey) && (m_bDestroyed == false) )
			{
				_SetPathAnimKey( (RESPAWN_HELICOPTER_PATH) path, keyframe);
			}
			else if( m_pPathAnim[ m_PathKey] != nullptr)
			{
				if (m_PathKey == path )
				{
					REAL32 rTerm = m_rPathLocalTime - keyframe;

					if( i3Math::abs( rTerm) > 0.1f)
					{
						_SetPathAnimKey( (RESPAWN_HELICOPTER_PATH) path, keyframe);

					}
				}
				else
				{
					I3TRACE("Server Path : %d ServerTime : %f \n", path, keyframe);
				}
			}
		}
	}

#if defined( DEF_OBSERVER_MODE)
	// Observer에서는 이하 처리를 하지 않는다.
	if( BattleSlotContext::i()->isObserverMe() )
	{
		return;
	}
#endif

	UINT64 keyStroke = g_pFramework->getKeyStroke();

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	
	if( (pPlayer != nullptr) && (pPlayer->getAttachedObject() == this))
	{
		if( pPlayer->isCharaStateMask( CHARA_STATE_ONOBJECT) &&
			(pPlayer->isCharaStateMask( CHARA_STATE_DEATH) == false) &&
			(static_cast<UINT32>(m_nTeam + 1) == pPlayer->getCharaTeam()) )
		{	// 탑승하고 있는 경우 처리

			if ((getMaxHP() > 0.f && (getCurrentHP() / getMaxHP()*100.f <= 30.f)) && 
				(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST))
			{
				if ( m_bWarningNotice)
				{
					m_bWarningNotice = false;
					CHud_Helicopter::instance()->UpdateWarningNotice();
				}
			}

			if( EnableOnOffObject(keyStroke) )
			{
				if(GetWeaponOnObject()->IsControlUser())
				{
					if( BattleServerContext::i()->IsP2PHost())
					{
						LeaveObject();
					}

					GAMEEVENT::Write_Chara_UseObject( pPlayer, this, NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT );
					m_bLeaveWeapon_Flag = true;
				}
				else if( IsEnableEnter( pPlayer) )
				{
					if( BattleServerContext::i()->IsP2PHost())
					{
						EnterObject( pPlayer);
					}

					GAMEEVENT::Write_Chara_UseObject( pPlayer, this, NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT );
					GAMEEVENT::Write_Chara_SyncUseObject( pPlayer, this, NET_SYNCKEY_ON_OBJECT );
					m_bEnterWeapon_Flag = true;
				}
			}

			if( IsEnableEnter( pPlayer)) 
			{
				CHud_Helicopter::instance()->SetRidingMark( ECM_CONTROL_M197);

			}
			else if( pPlayer->getCharaNetIndex() == GetEnterCharaIdx())
			{
				CHud_Helicopter::instance()->SetRidingMark( ECM_UNCONTROL_M197);
			}
			else
			{
				CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
			}
		}
	}

	if(IsEnableTime() == false)
	{
		CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
	}
	
	// 네트워크 싱크를 맞춥니다.
	if( BattleServerContext::i()->IsP2PHost() )
	{
		bool bSend = false;

		m_rSendCountTime+= rDeltaSeconds;
		if( m_rSendCountTime < 0.5f)
			bSend = true;

		// EventSender 추가 2011.04.20 swoongoo
		INT32 EventSender = BattleSlotContext::i()->getMySlotIdx();
		if( i3::kind_of<CGameCharaBase*>(getEventSender()))
		{
			CGameCharaBase * pES = (CGameCharaBase*) getEventSender();
			EventSender = pES->getCharaNetIndex();
		}

		GAMEEVENT::Write_Object_AnimateForOutpost( getNetIndex(), EventSender, (UINT8) m_PathKey, (UINT16) getCurrentHP(), m_rPathLocalTime, bSend);
	}

	SendLeaveSyncUH60();
}

void CGameObjectRespawnHelicopter::SetControledObjectState( INT32 state)
{
}


void CGameObjectRespawnHelicopter::InitAnimation( void)
{
	INT32 i;

	for( i = 0; i < RESPAWN_HELICOPTER_PATH_COUNT; i++)
	{
		if( (s_PathName_Breakdown2_Helicopter[ i] != nullptr) && (i3::generic_string_size( s_PathName_Breakdown2_Helicopter[ i]) > 0) )
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_PathName_Breakdown2_Helicopter[ i]);
			if( pAnimation == nullptr)
				continue;

			I3_REF_CHANGE( m_pPathAnim[ i], pAnimation);
			I3_MUST_RELEASE(pAnimation);  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	m_pSceneObj->BuildBoneList();
}

void CGameObjectRespawnHelicopter::Destroy(void)
{
}

void CGameObjectRespawnHelicopter::EnableDustEffect( bool bEnable)
{
}


bool CGameObjectRespawnHelicopter::IsEnableObject()
{
	return true;
}

MATRIX * CGameObjectRespawnHelicopter::GetRespawnPose( void)
{
	if( m_pRespawnDummy == nullptr)
		return nullptr;

	return m_pRespawnDummy->GetCacheMatrix();
}


void CGameObjectRespawnHelicopter::SendLeaveSyncUH60()
{
	if( GetEnterCharaIdx() != BattleSlotContext::i()->getMySlotIdx() )
	{
		CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		GAMEEVENT::Write_Chara_SyncUseObject( pPlayer, this, NET_SYNCKEY_OFF_OBJECT);
	}
}


struct PERSIST_OBJ_RESPAWNHELICOPTER
{
	UINT32			_iVer;
	INT32			_iTeamIdx;
	UINT32			pad[4];
} ;

UINT32 CGameObjectRespawnHelicopter::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	(*ppData) = i3MemAlloc( sizeof( PERSIST_OBJ_RESPAWNHELICOPTER));

	i3mem::FillZero( *ppData, sizeof(PERSIST_OBJ_RESPAWNHELICOPTER));

	PERSIST_OBJ_RESPAWNHELICOPTER * pData = (PERSIST_OBJ_RESPAWNHELICOPTER*) (*ppData);

	pData->_iVer		= 101;
	pData->_iTeamIdx	= m_nTeam;

	return sizeof( PERSIST_OBJ_RESPAWNHELICOPTER);
}

UINT32 CGameObjectRespawnHelicopter::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	UINT32 Rc;
	i3Stream * pStream = pInfo->getStream();

	PERSIST_OBJ_RESPAWNHELICOPTER info;
	
	Rc = pStream->Read( &info, sizeof( PERSIST_OBJ_RESPAWNHELICOPTER));
	I3_CHKIO( Rc);

	m_nTeam				= info._iTeamIdx;

	return Rc;
}

struct ALIGN4 I3_PERSIST_RESPAWNHELI
{
	INT32		nTeam;
	UINT32		pad[4];
} ;

UINT32 CGameObjectRespawnHelicopter::OnSave(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	result = CGameObjectWithWeapon::OnSave(pResFile);
	I3_CHKIO(result);

	I3_PERSIST_RESPAWNHELI data;
	data.nTeam = m_nTeam;

	rc = pResFile->Write(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	return result;
}

UINT32 CGameObjectRespawnHelicopter::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	I3_PERSIST_RESPAWNHELI data;

	result = CGameObjectWithWeapon::OnLoad(pResFile);
	I3_CHKIO(result);

	rc = pResFile->Read(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	m_nTeam = data.nTeam;

	return result;
}

#if defined(I3_DEBUG)//XML
bool CGameObjectRespawnHelicopter::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = new i3XMLElement( "RespawnHelicopter");
	pXML->LinkEndChild( pChild);

	pChild->addAttr( "TeamIdx",	m_nTeam);

	return true;
}

bool CGameObjectRespawnHelicopter::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( "RespawnHelicopter");
	if( pChild == nullptr)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		return false;
	}

	pChild->getAttr( "TeamIdx", &m_nTeam);

	return true;
}
#endif



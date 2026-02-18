#include "pch.h"
#include "Domi_Flow_Defense.h"

#include "./GameMode_Domination.h"
#include "../../Source/ui/hud/UIHUD_Domination_Manager.h"

#include "../ModeScriptManager.h"
#include "../DomiScript.h"

CDomi_Flow_Defense::CDomi_Flow_Defense( gmode_domination* pmode) : CDominationFlowBase( pmode),
m_pDefenceObj(NULL), m_bEnableCrossHitOnObjecct(false), m_bObjectActivated(false), m_bPressedKey(false), m_rPressedKeyTime(0.f), m_iDefenceCount(0)
{
}

CDomi_Flow_Defense::~CDomi_Flow_Defense(void)
{
}

void CDomi_Flow_Defense::_ResetKeyState()
{	
	if( m_bPressedKey)
	{
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
		myPlayer->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE);
	}

	m_bPressedKey = false;
	m_rPressedKeyTime = 0.f;
}

void CDomi_Flow_Defense::_Activate()
{
	m_bObjectActivated = true;				//방어 시작.

	_ResetKeyState();

	m_bEnableCrossHitOnObjecct = false;
	g_pGameContext->getMissionManager()->setEnableCrossHit( FALSE);	//HUD를 끄고, Input을 못받도록 함.

//#ifdef DOMI_HUD
//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_SCOREBOARD_OBJECT_HP_ENABLE, m_pDefenceObj->getNetIndex());
//#endif

	//Not Used Object 끄기.
	std::map<INT32, INT32>::iterator itr;
	itr = m_mapObjectIdx.find( m_pDefenceObj->getNetIndex());
	if( itr != m_mapObjectIdx.end())
	{
		i3Object* pNotUesd = m_pmode->GetWaveMissionObject( itr->second);
		pNotUesd->SetEnable( FALSE);
		pNotUesd->SetDynamicState(I3GAME_DYNAMIC_STATE_NONE);

		m_pDefenceObj->SetEnable( TRUE);
		m_pDefenceObj->SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);
	}
	else
	{
		I3NOTICE("  -- Can not find Not Uesd Defence Object(%d)\n", itr->second);
	}

	I3TRACE(" -- Defence Mission -> Start Defence  \n");
}

void CDomi_Flow_Defense::_Deactivate()
{
	m_bObjectActivated = false;				//방어 끝.

	I3TRACE(" -- Defence Mission -> End Defence  \n");
}

void CDomi_Flow_Defense::ProcessModeData( INT32 nIndex, INT32 nData)
{
	//switch( nIndex)
	//{
	//case 110:	//사용 할 오브젝트 설정(난이도에 따라 사용하는 오브젝트 수가 다름)
	//	{
	//		UINT8 offset = 5;
	//		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData);

	//		// 오브젝트 인덱스 저장.
	//		m_mapObjectIdx.clear();
	//		IModeScript*	pScript	= tinst<ModeScriptMgr>()->GetScript();
	//		for( INT8 iCnt = 0; iCnt < nData; ++iCnt)
	//		{
	//			INT32 nUsedObjectIdx = pScript->get_modedata(nIndex + offset + iCnt);
	//			INT32 nNotUsedObjectIdx = pScript->get_modedata(nIndex + offset + DEFENCE_OBJECT_MAX +iCnt);

	//			std::map<INT32, INT32>::value_type item( nUsedObjectIdx, nNotUsedObjectIdx);
	//			m_mapObjectIdx.insert( item);
	//		}
	//	}
	//	break;
	//case 111:	//Defence Object Total Count
	//	{
	//		UINT8 offset = 4;
	//		m_pmode->RegisterWaveMissionObjects( nIndex + offset, nData);									//불 투명.
	//		m_pmode->RegisterWaveMissionObjects( nIndex + offset + DEFENCE_OBJECT_MAX, nData, false);		//투명.


	//		m_iDefenceCount = 0;
	//		m_pDefenceObj = NULL;			
	//	}
	//	break;
	//case 112:			// 방어 할 오브젝트 인덱스, 여기서는 UI만 표시 함.
	//	{
	//		if( m_pDefenceObj != NULL)	// 새롭게 업데이트 하기 전 이전에 있던 Indication UI를 지움.
	//			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_DELETE_STAGE_OBJECT, m_pDefenceObj->getNetIndex());

	//		m_pmode->SetCurrentMissionObjectIndex( nData);		//한번에 하나의 오브젝트만 
	//		m_pDefenceObj = m_pmode->GetCurrentMissionObject();

	//		I3ASSERT( m_pDefenceObj != NULL);
	//		if( m_pDefenceObj != NULL)
	//		{
	//			I3TRACE("  -- Recv Defence Object Net_Index(%d) Name(%s)\n", m_pDefenceObj->getNetIndex(), m_pDefenceObj->GetName());
	//		}
	//		else
	//		{
	//			I3TRACE("  -- It is invalid Defence object Idx(%d)\n", nData);
	//			return;
	//		}
	//		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_SET_STAGE_OBJECT, WAVE_MISSION_DEFENCE);
	//	}
	//	break;

	//case 113:		// 방어 한 오브젝트 갯수	
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData);
	//	break;

	//case 114:		//서버에서 오브젝트 활성화 신호를 받음.
	//	if( nData)
	//	{
	//		_Activate();
	//	}
	//	else
	//	{
	//		_Deactivate();
	//	}
	//	break;
	//}
}

/*virtual*/	void CDomi_Flow_Defense::ResetObject()
{
	//모든 방어 미션 오브젝트 리셋(끄기).
	std::vector<i3Object*>& pMissionObjects = m_pmode->GetWaveMissionObjects();

	std::vector<i3Object*>::iterator itr;
	std::map<INT32, INT32>::iterator itr_idx;

	itr = pMissionObjects.begin();
	while( itr != pMissionObjects.end())
	{
		bool bUse = false;
		i3Object* pObject = *itr;

		for( itr_idx = m_mapObjectIdx.begin(); itr_idx != m_mapObjectIdx.end(); ++itr_idx)
		{
			if( itr_idx->second == pObject->getNetIndex()) { bUse = true; break;}
		}

		if( !bUse)
		{
			pObject->SetEnable(FALSE);
			pObject->SetDynamicState(I3GAME_DYNAMIC_STATE_NONE);
		}
		++itr;
	}
}

void CDomi_Flow_Defense::ProcessInput( REAL32 rDeltaSeconds)
{
	if( m_bEnableCrossHitOnObjecct)
	{
		if( g_pFramework->getKeyStroke() & GAME_KEY_ACTION_SPECIAL) m_bPressedKey = true;

		if( (g_pFramework->getKeyPress() & GAME_KEY_ACTION_SPECIAL) && m_bPressedKey)
		{
			CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
			myPlayer->addEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE);

			m_rPressedKeyTime += rDeltaSeconds;
			if( m_rPressedKeyTime >= DEFENCE_OBJECT_ACTIVATE_DELAY)
			{
				g_pGameContext->PushKeyNetwork( GAME_KEY_MAP_SPECIALACTION);
				g_pGameContext->SetEvent( EVENT_KEY_PROCESS_NET );

				_ResetKeyState();
			}
		}
		else
		{
			_ResetKeyState();
		}
	}
}

bool CDomi_Flow_Defense::CheckCrossHitMissionObject( CGameCharaBase* pCharacter)
{
	if( m_pDefenceObj == NULL || m_bObjectActivated) return false;

	MATRIX matObject;
	GlobalFunc::GetNowFrameMatrix( &matObject, m_pDefenceObj->getSceneObject());

	VEC3D vDir;

	i3Vector::Sub( &vDir, i3Matrix::GetPos( &matObject), pCharacter->GetPos());
	REAL32 rDist = i3Vector::Length( &vDir);

	//대충 3m 내에 들어오면 검사.
	if( (rDist < 3.f && (i3Math::abs( getY( &vDir)) < 0.05f)) == false)
	{
		_ResetKeyState();

		m_bEnableCrossHitOnObjecct = false;
		return false;
	}

	i3Vector::Sub( &vDir, i3Matrix::GetPos( &matObject), i3Matrix::GetPos( g_pCamera->getCamMatrix()));

	REAL32 rLength_FromCamera = i3Vector::Length( &vDir);

	i3CollideeLine cLine;
	cLine.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	cLine.SetDir( &vDir);

	I3_PHYSIX_HIT_RESULT * pResultBomb = g_pFramework->raycastClosest( &cLine, CGRP_MASKGROUP_ATTACK);
	REAL32 rCollisionLen = 1000000.f;
	if( pResultBomb != NULL)
	{	
		rCollisionLen = pResultBomb->m_T;
	}

	REAL32 rRadius_Object = rLength_FromCamera - rCollisionLen;

	if( rDist <= (rRadius_Object + RADIUS /*+ 0.1f*/))
	{
		// 화면에 대한 각도 계산은 캐릭터 포지션으로 합니다.
		// 카메라 포지션으로 하면 캐릭터가 앉으면서 해체가 풀리게 됩니다.
		i3Vector::Sub( &vDir, i3Matrix::GetPos( &matObject), pCharacter->GetPos());
		i3Vector::Normalize( &vDir, &vDir );
		REAL32 rDot = i3Vector::Dot( &vDir, i3Matrix::GetAt( pCharacter->GetMatrix()));
		if( rDot > 0.6f)
		{
			m_bEnableCrossHitOnObjecct = true;
			return true;
		}
	}

	_ResetKeyState();

	m_bEnableCrossHitOnObjecct = false;
	return false;
}
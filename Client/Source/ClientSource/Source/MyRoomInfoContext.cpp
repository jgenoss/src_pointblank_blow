#include "pch.h"
#include "MyRoomInfoContext.h"
#include "UserInfoContext.h"
#include "GameMissionManager.h"
#include "GameGUI.h"
#include "StageBattle/ScoreContext.h"

#include "BattleSlotContext.h"
#include "TextSet.h"

#include "i3Base/RemoveColorString.h"

#include "ClientStageInfoUtil.h"

MyRoomInfoContext::MyRoomInfoContext() : m_bManualReadyFlag(false), m_bTeamBalanceMsgFlag(false), m_bTeamChangeFlag(false), m_bQuickJoin(false), m_bSoloPlay(false)
	, m_CountDown(0), m_bInReadyRoom(false)
{
	
	i3mem::FillZero( &m_MyRoomInfo,		sizeof(ROOM_INFO_BASIC));
	i3mem::FillZero( &m_MyRoomInfoAdd,	sizeof(ROOM_INFO_ADD));
	i3mem::FillZero( m_RoomPasswd,		sizeof(m_RoomPasswd));

	ROOM_INFO_BASIC	roomInfo;
	roomInfo.Reset();
	setMyRoomInfo(&roomInfo);
	
	m_EmptyStage = CSI_STAGE();
	m_DummyStage = nullptr;
}

MyRoomInfoContext::~MyRoomInfoContext()
{
	delete m_DummyStage;
}

void	MyRoomInfoContext::ClearMyRoomPW()
{
	i3mem::FillZero( m_RoomPasswd, sizeof(m_RoomPasswd));
}

void	MyRoomInfoContext::SetMyRoomPW(const char* pw, INT32 len)
{
	i3mem::FillZero( m_RoomPasswd, sizeof(m_RoomPasswd));
	i3mem::Copy( m_RoomPasswd, pw, sizeof(char) * len);
}



void MyRoomInfoContext::getMyRoomInfo( ROOM_INFO_BASIC * pInfo ) const
{
	*pInfo = m_MyRoomInfo;
	GlobalFunc::DecryptValue(pInfo, sizeof(m_MyRoomInfo), ENC_SEED_ROOMINFO);
}

void MyRoomInfoContext::setMyRoomInfo( const ROOM_INFO_BASIC * pInfo )
{
	m_MyRoomInfo = *pInfo;

	// revision 61128 컬러 키워드 루틴 강화 커밋 ( RemoveColorKeyword -> RemoveColorString 으로 함수 변경 )
	i3::stack_wstring result;

	::RemoveColorString(m_MyRoomInfo._Title,result);

	i3::safe_string_copy(m_MyRoomInfo._Title,result,_countof(m_MyRoomInfo._Title));

	GlobalFunc::EncryptValue(&m_MyRoomInfo, sizeof(m_MyRoomInfo), ENC_SEED_ROOMINFO);

	m_bSoloPlay = false;
}

INT32 MyRoomInfoContext::getMyRoomIndex( void ) const
{
	ROOM_INFO_BASIC	roomInfo;
	getMyRoomInfo(&roomInfo);
	return roomInfo._Idx;
}

void MyRoomInfoContext::setMyRoomIndex( INT32 idx )
{
	ROOM_INFO_BASIC	roomInfo;
	getMyRoomInfo(&roomInfo);
	roomInfo._Idx = idx;
	setMyRoomInfo(&roomInfo);
}


void	MyRoomInfoContext::InitMyRoomInfo()
{
	ROOM_INFO_BASIC		MyRoomInfo;

	getMyRoomInfo(&MyRoomInfo);
	
	MyRoomInfo._StageID		= 0;
	MyRoomInfo._WeaponFlag	= LOCK_BASIC;

	m_bQuickJoin = false;

	setMyRoomInfo(&MyRoomInfo);
}


UINT8 MyRoomInfoContext::GetMyRoomWeaponFlag() const
{
	ROOM_INFO_BASIC	roomInfo;
	getMyRoomInfo(&roomInfo);
	return roomInfo._WeaponFlag;
}

UINT32 MyRoomInfoContext::GetMyRoomStageID() const
{
	ROOM_INFO_BASIC	roomInfo;
	getMyRoomInfo(&roomInfo);

	return roomInfo._StageID;
}

UINT8 MyRoomInfoContext::GetMyRoomInfoFlag() const
{
	ROOM_INFO_BASIC	roomInfo;
	getMyRoomInfo(&roomInfo);
	return roomInfo._InfoFlag;
}


// 자동공수교대가 세팅된 방인가?
bool MyRoomInfoContext::IsAutoTeamChangeableRoomState()
{
	bool autoTeamChangeEnable = UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE);

	if(!autoTeamChangeEnable)
		return false; // 자동공수교대 컨텐츠가 활성화되지않았습니다.(웹툴설정)

	return getStage()->IsAtkDefChangeStage();
}

bool MyRoomInfoContext::IsAutoTeamChanged() const
{
	return ScoreContext::i()->IsAutoTeamChangeExecuted();
}


CSI_STAGE* MyRoomInfoContext::getStage(void)
{
	UINT32 StageID = GetMyRoomStageID();
	
	if (m_DummyStage != nullptr)
		return m_DummyStage;
	else if (StageID == 0)
		return &m_EmptyStage;
	else
		return CStageInfoUtil::GetStageData(StageID);
}

void MyRoomInfoContext::setDummyStage(STAGE_ID stage)
{
	m_DummyStage					= new CSI_STAGE();
	m_DummyStage->m_ui32StageID		= stage;
	m_DummyStage->m_i32RuleIdx		= 1;
	m_DummyStage->m_ui8Event		= STAGE_EVENT_NORMAL;
	m_DummyStage->m_ui8MaxPerson	= 16;
	m_DummyStage->m_ui8Mark			= 0;
}

void MyRoomInfoContext::getMyRoomInfoAdd( ROOM_INFO_ADD * pInfo ) const
{
	*pInfo = m_MyRoomInfoAdd;
	GlobalFunc::DecryptValue(pInfo, sizeof(m_MyRoomInfoAdd), ENC_SEED_ROOMINFO);
}

void MyRoomInfoContext::setMyRoomInfoAdd( const ROOM_INFO_ADD * pInfo )
{
	m_MyRoomInfoAdd = *pInfo;
	GlobalFunc::EncryptValue(&m_MyRoomInfoAdd, sizeof(m_MyRoomInfoAdd), ENC_SEED_ROOMINFO);
}


UINT8 MyRoomInfoContext::GetMyRoomSubType() const
{
	ROOM_INFO_ADD roomAddInfo;
	getMyRoomInfoAdd(&roomAddInfo);
	return roomAddInfo._SubType;
}

UINT8 MyRoomInfoContext::GetMyRoomLockObserver() const
{
	ROOM_INFO_ADD	roomAddInfo;
	getMyRoomInfoAdd(&roomAddInfo);
	return roomAddInfo._LockObserver;
}


REAL32 MyRoomInfoContext::GetInvincibleTime()
{
	return CGameMissionManager::i()->GetInvincibleTime(getStage());
}

REAL32 MyRoomInfoContext::GetRespawnTime( INT32 userIdx)
{
	return CGameMissionManager::i()->GetRespawnTime(getStage(), userIdx);
}


bool MyRoomInfoContext::IsLockedWeaponSlot(UINT32 idxSlot) const
{
	// 주무기, 보조무기, 근접무기, 투척무기 슬롯만 검사
	if (idxSlot < 4)
	{
		if (g_pFramework->IsBattleStage())
		{
			UINT8 WeaponFlag = MyRoomInfoContext::GetMyRoomWeaponFlag();

			// RPG7전이고 0번 무기슬롯이면 제한된 무기가 아니다.
			if(idxSlot==0 && TESTBIT(WeaponFlag, LOCK_RPG7))
			{
				return false;
			}
			// 제한된 무기다
			else if (!GlobalFunc::CheckAvailableWeaponUsage(WeaponFlag, (WEAPON_SLOT_TYPE)idxSlot))
			{
				return true;
			}
		}
	}

	// 제한되지 않았다
	return false;
}

// 이 함수 하나도 이해가 안간다....pInfo->_Idx외엔 쓰는게 없다??????
bool MyRoomInfoContext::JoinRoom( const ROOM_INFO_BASIC* pInfo )
{
	ROOM_INFO_BASIC	roomInfo;
	getMyRoomInfo(&roomInfo);

	roomInfo._Idx = pInfo->_Idx;
	setMyRoomInfo(&roomInfo);

	bool rv = GameEventSender::i()->SetEventOnce(EVENT_JOIN_ROOM );
	return rv;
} 

//----------------------------------------------------------------------------------//
//									keyword : Star Player							//
//----------------------------------------------------------------------------------//

void MyRoomInfoContext::PrintStarPlayerEntranceMsg(const INT32 Idx)
{
	const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(Idx);

	if (g_pFramework->IsBattleStage())
	{
		// 배틀중인 경우, 레디룸 유저 입장에 대한 메세지는 출력하지 않습니다.
		if (pSlot->_State <= SLOT_STATE_READY)
			return;
	}

	i3::rc_wstring wstr_msg;
	TextSet::StarPlayerEntranceMessage( wstr_msg, pSlot->_Rank, BattleSlotContext::i()->getNickForSlot(Idx) );

	if ( wstr_msg.length() > 0)
		g_pFramework->PutSystemChatting(wstr_msg);
}


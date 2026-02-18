#include "pch.h"
#include "UserSession.h"
#include "ModuleTrans.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingSkill( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{

	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_SKILL_SET_REQ:	// 주특기 셋팅 ( = 스킬 세팅 )
		{			
			CHECKVALID_CHANNEL;

			PACKET_SKILL_SET_REQ		stSpReq;
			PACKET_SKILL_SET_ACK		stSpAck;

			pPacket->ReadData(&stSpReq, sizeof(PACKET_SKILL_SET_REQ));

			if(! CheckSkillSetting( &stSpReq ) )			
			{
				stSpAck.m_i32Rv = EVENT_ERROR_PBV15_SKILL;
			}
			else
			{
				S2MO_CHAR_SKILL_INFO Skill_Info;
				m_MultiSlot.GetCharSkillInfo( stSpReq.m_ui8SlotIdx, &Skill_Info );

				// 스킬 레벨이 0으로 온 것들은 장착 해제 혹은 빈 스킬란으로 처리
				if( stSpReq.m_ui8MainLv == 0 )		stSpReq.m_MainSkill		= MAIN_SKILL_NONE;
				if( stSpReq.m_ui8AssistLv == 0 )	stSpReq.m_AssistSkill	= ASSIST_SKILL_NONE;
				if( stSpReq.m_ui8CommonLv == 0 )	stSpReq.m_CommonSkill	= COMMON_SKILL_NONE;

				Skill_Info.m_ui8MainSkill		= (UINT8)stSpReq.m_MainSkill;
				Skill_Info.m_ui8AssistSkill		= (UINT8)stSpReq.m_AssistSkill;
				Skill_Info.m_ui8CommonSkill		= (UINT8)stSpReq.m_CommonSkill;

				Skill_Info.m_ui8MainSkillLv		= stSpReq.m_ui8MainLv;
				Skill_Info.m_ui8AssistSkillLv	= stSpReq.m_ui8AssistLv;
				Skill_Info.m_ui8CommonSkillLv	= stSpReq.m_ui8CommonLv;

				m_MultiSlot.SetCharSkillInfo( stSpReq.m_ui8SlotIdx, &Skill_Info);

				stSpAck.m_i32Rv = EVENT_ERROR_SUCCESS;

				// 모든 체크에 통과했다면 DB 저장 플래그를 킨다. 로그 아웃시 저장.
				ChangeCharaDataBaseInfo( ( (UINT64)0x01 << stSpReq.m_ui8SlotIdx) );
			}
			
			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_SKILL_SET_ACK ); 
			pSendPacket->WriteData(&stSpAck, sizeof(PACKET_SKILL_SET_ACK));
		}
		break;

	case PROTOCOL_SKILL_RESET_REQ:	// 추후 구현. 2012.02.15 노준영
		{
			CHECKVALID_CHANNEL;

			PACKET_SKILL_RESET_REQ		stSpeciality;
			pPacket->ReadData(&stSpeciality, sizeof(PACKET_SKILL_RESET_REQ));

			// 모든 체크에 통과했다면 DB 저장 플래그를 킨다. 로그 아웃시 저장.
			//ChangeDataBaseInfo( SAVE_DATABASE_CHARA_SKILL );
		}
		break;
	
	case PROTOCOL_SKILL_CLASS_SET_REQ:	// 스킬 시스템 병과 셋팅. ( Main 최초 설정 )
		{
			CHECKVALID_CHANNEL;

			bSendMessage = TRUE;
			PACKET_SKILL_CLASS_SET_ACK		stSpAck; 
			stSpAck.m_i32Rv = EVENT_ERROR_SUCCESS;

			pSendPacket->SetProtocol( PROTOCOL_SKILL_CLASS_SET_ACK );

			// Packet Read.
			PACKET_SKILL_CLASS_SET_REQ		stSpReq;
			pPacket->ReadData(&stSpReq, sizeof(PACKET_SKILL_CLASS_SET_REQ));

			stSpAck.m_ui8Class	= stSpReq.m_ui8Class;
			stSpAck.m_ui8Slot	= stSpReq.m_ui8Slot;

			// 값이 유효한지 체크
			if( !SetCharaSkillClass( stSpReq.m_ui8Slot, stSpReq.m_ui8Class, FALSE ) )
			{
				stSpAck.m_i32Rv = EVENT_ERROR_PBV15_SKILL_HACK_PACKET;
				pSendPacket->WriteData( &stSpAck,	sizeof(PACKET_SKILL_CLASS_SET_ACK) );
				break;
			}
			 
			// DB 플래그만 켭니다. 로그아웃시 저장. 
			ChangeCharaDataBaseInfo( ( (UINT64)0x01 << stSpReq.m_ui8Slot) );

			// ZLog 기록. 
			g_pModuleContextNC->LogSendMessage(m_WorkThreadIdx, LOG_MESSAGE_U_CLASS_SETTING, this, &stSpReq.m_ui8Slot, &stSpReq.m_ui8Class  );

			
			// Packet Send
			stSpAck.m_i32Rv = EVENT_ERROR_SUCCESS;
			pSendPacket->WriteData( &stSpAck,	sizeof(PACKET_SKILL_CLASS_SET_ACK) );
		}
		break;

	}
	return bSendMessage;
}

//////////////////////////////////////////////////////////////////////////
// 캐릭터 주특기 Protected
//////////////////////////////////////////////////////////////////////////
BOOL CUserSession::CheckSkillSetting( PACKET_SKILL_SET_REQ* pInfo )
{
	// 1. 캐릭터 슬롯 체크
	if( !m_MultiSlot.IsActiveChar( pInfo->m_ui8SlotIdx, MULTI_SLOT_CHECK_USING  ) )
	{
		return FALSE;
	}

	S2MO_CHAR_SKILL_INFO	Skill_Info;
	S2MO_CHAR_BASE_INFO		Base_Info;
	bool					bRestrictionEnable;

	m_MultiSlot.GetCharSkillInfo( pInfo->m_ui8SlotIdx, &Skill_Info );
	m_MultiSlot.GetCharBaseInfo( pInfo->m_ui8SlotIdx, &Base_Info );

	bRestrictionEnable = g_pContextMain->IsContentsEnabled(CONTENTS_RESTRICT_SKILL);

	if( !CheckGetMainSkill( static_cast<CHARA_CLASS>(Skill_Info.m_ui8Class), pInfo->m_MainSkill, pInfo->m_ui8MainLv, Base_Info.m_ui32SkillRelTime, bRestrictionEnable ) ) return FALSE;
	if( !CheckGetAssistSkill( pInfo->m_AssistSkill, pInfo->m_ui8AssistLv, (UINT8)m_UserInfoBasic.m_ui32Rank, bRestrictionEnable ) ) return FALSE;
	if( !CheckGetCommonSkill( pInfo->m_CommonSkill, pInfo->m_ui8CommonLv, (UINT8)m_UserInfoBasic.m_ui32Rank, bRestrictionEnable ) ) return FALSE;

	return TRUE;
}

BOOL CUserSession::SetCharaSkillClass( UINT8 ui8SlotIdx, UINT8 ui8SkillClass, BOOL bFirst )
{
	char	strErrorLog[MAX_PATH];

	// 클라이언트
	if( ui8SlotIdx >= S2MO_MULTI_SLOT_COUNT || ui8SkillClass >= CHARA_CLASS_COUNT )
	{
		i3String::Format( strErrorLog, MAX_PATH, "[PROTOCOL_SET_SPECIALITY_CLASS_REQ] HackPacket Slot Type Class / UID : %I64d ", m_i64UID ) ;
		//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_PBV15_SKILL_HACK_PACKET, PROTOCOL_SKILL_CLASS_SET_REQ, KILL_REASON_HACK_PACKET,  strErrorLog);
		return FALSE;
	}

	// 주의 : 차후에 하나의 타입당 2개이상의 클래스(병과)를 보유하는 것이 가능해지면 수정 되어야 합니다. 2012.02.13 노준영
	S2MO_CHAR_SKILL_INFO Skill_Info;
	m_MultiSlot.GetCharSkillInfo( ui8SlotIdx, &Skill_Info );

	// 이미 클래스가 셋팅 되었는지 체크
	if( CHARA_CLASS_NONE != Skill_Info.m_ui8Class ) 
	{
		i3String::Format( strErrorLog, MAX_PATH, "[PROTOCOL_SET_SPECIALITY_CLASS_REQ] HackPacket Aleady Class Setting / UID : %I64d ", m_i64UID ) ;
		//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_PBV15_SKILL_HACK_PACKET, PROTOCOL_SKILL_CLASS_SET_REQ, KILL_REASON_HACK_PACKET,  strErrorLog);
		return FALSE;
	}

	Skill_Info.Reset();
	Skill_Info.m_ui8Class	= ui8SkillClass;	
	m_MultiSlot.SetCharSkillInfo( ui8SlotIdx, &Skill_Info);

	return TRUE;
}


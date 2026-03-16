#ifndef __S2_MO_STRUCT_BATTLE_H__
#define __S2_MO_STRUCT_BATTLE_H__

struct PACKET_BATTLE_READYBATTLE_ACK : public S2MOPacketBaseT<PROTOCOL_BATTLE_READYBATTLE_ACK>
{
	S2MOValue<T_RESULT>									m_TResult;
	S2MOValue<UINT8>									m_ui8State;

	PACKET_BATTLE_READYBATTLE_ACK()
	{
		// Last Update : 2015-10-12 14:22:22
		_SetValue( m_TResult );
		_SetValue( m_ui8State );
	}
};

struct PACKET_BATTLE_START_GAME_ACK :public S2MOPacketBaseT<PROTOCOL_BATTLE_START_GAME_ACK>
{
	S2MOValue<UINT32>									m_ui32StageID;
	S2MOValue<S2_CHAR_BATTLE_INFO, SLOT_MAX_COUNT>		m_stBattleInfo;
	S2MOValue<UINT8, SLOT_MAX_COUNT>					m_aui8UsersScopeType;

	PACKET_BATTLE_START_GAME_ACK()
	{
		// Last Update : 2015-10-15 16:23:17
		_SetValue( m_ui32StageID );
		_SetValue( m_stBattleInfo );
		_SetValue( m_aui8UsersScopeType );
	}
};

struct PACKET_BATTLE_START_GAME_TRANS_ACK :public S2MOPacketBaseT<PROTOCOL_BATTLE_START_GAME_TRANS_ACK>
{
	S2MOValue<S2_CHAR_BATTLE_INFO>						m_stBattleInfo;
	S2MOValue<UINT8>									m_ui8UserScopeType;

	PACKET_BATTLE_START_GAME_TRANS_ACK()
	{
		// Last Update : 2015-10-20 16:22:55
		_SetValue( m_stBattleInfo );
		_SetValue( m_ui8UserScopeType );
	}
};

struct PACKET_BATTLE_STARTBATTLE_ACK :public S2MOPacketBaseResultT<PROTOCOL_BATTLE_STARTBATTLE_ACK>
{
	S2MOValue<UINT8>									m_ui8SlotIdx;
	S2MOValue<UINT8>									m_ui8InterEnter;										// 난입인지
	S2MOValue<UINT16>									m_ui16BattleUser;
	S2MOValue<UINT16, TEAM_COUNT>						m_ui16TotalRoundCount;									// 폭파, 섬멸전, 헤드킬러, 서든데스, 크로스카운트, 디펜스, 파괴
	S2MOValue<UINT16>									m_ui16RoundStartUser;									// 폭파, 섬멸전, 헤드킬러, 서든데스, 크로스카운트, 
	S2MOValue<UINT8>									m_ui8NowRoundCount;										// 크로스카운트, 탈출
	S2MOValue<DINO_ROOM_INFO>							m_MiDinoInfo;											// 크로스카운트, 탈출
	S2MOValue<UINT8>									m_ui8BlockBonusRemain;									// 탈출

	PACKET_BATTLE_STARTBATTLE_ACK()
	{
		// Last Update : 2015-10-20 19:20:11
		_SetValue( m_ui8SlotIdx );
		_SetValue( m_ui8InterEnter );
		_SetValue( m_ui16BattleUser );
		_SetValue( m_ui16TotalRoundCount );
		_SetValue( m_ui16RoundStartUser );
		_SetValue( m_ui8NowRoundCount );
		_SetValue( m_MiDinoInfo );
		_SetValue( m_ui8BlockBonusRemain );
	}
};

struct PACKET_BATTLE_ENDBATTLE_ACK :public S2MOPacketBaseT<PROTOCOL_BATTLE_ENDBATTLE_ACK>
{
	S2MOValue<UINT8>									m_ui8Win;
	S2MOValue<UINT16>									m_ui16BattleEndUserFlag;
	S2MOValue<UINT16, TEAM_COUNT>						m_aui16TotalRoundCount;

	S2MOValue<UINT16, SLOT_MAX_COUNT>					m_ui16AccExp;
	S2MOValue<UINT16, SLOT_MAX_COUNT>					m_ui16AccPoint;
	S2MOValue<UINT8, SLOT_MAX_COUNT>					m_ui8ResultIcon;
	S2MOValue<UINT8, SLOT_MAX_COUNT>					m_ui8AccMissionCount;
	S2MOValue<UINT16, SLOT_MAX_COUNT>					m_ui16AccChallengeScore;
	S2MOValue<UINT16, SLOT_MAX_COUNT*TYPE_BATTLE_RESULT_EVENT_COUNT>	m_ui16BonusExp;
	S2MOValue<UINT16, SLOT_MAX_COUNT*TYPE_BATTLE_RESULT_EVENT_COUNT>	m_ui16BonusPoint;

	S2MOValue<UINT8>									m_ui8MySlotIdx;

	S2MOValue<USER_INFO_BASIC>							m_UserInfoBasic;
	S2MOValue<USER_INFO_RECORD>							m_UserInfoRecord;

	S2MOValue<UINT16>									m_ui16QuestEndFlag;

	S2MOValue<UINT8>									m_ui8AbusingLevel;									// 어뷰징 레벨
	S2MOValue<UINT32>									m_ui32AbusingPopupTime;								// 어뷰징 팝업 시간

	S2MOValue<UINT8>									m_ui8AvatarSlot;
	S2MOValue<UINT32>									m_ui32BattleTime;
	S2MOValue<UINT32>									m_ui32SkillRelTime;
	S2MOValue<USER_INFO_DAILY>							m_UserInfoDaily;
	S2MOValue<MULTI_KILL_INFO>							m_MultiKillInfo;

	PACKET_BATTLE_ENDBATTLE_ACK()
	{
		// Last Update : 2015-10-20 19:35:13
		_SetValue( m_ui8Win );
		_SetValue( m_ui16BattleEndUserFlag );
		_SetValue( m_aui16TotalRoundCount );
		_SetValue( m_ui16AccExp );
		_SetValue( m_ui16AccPoint );
		_SetValue( m_ui8ResultIcon );
		_SetValue( m_ui8AccMissionCount );
		_SetValue( m_ui16AccChallengeScore );
		_SetValue( m_ui16BonusExp );
		_SetValue( m_ui16BonusPoint );
		_SetValue( m_ui8MySlotIdx );
		_SetValue( m_UserInfoBasic );
		_SetValue( m_UserInfoRecord );
		_SetValue( m_ui16QuestEndFlag );
		_SetValue( m_ui8AbusingLevel );
		_SetValue( m_ui32AbusingPopupTime );
		_SetValue( m_ui8AvatarSlot );
		_SetValue( m_ui32BattleTime );
		_SetValue( m_ui32SkillRelTime );
		_SetValue( m_UserInfoDaily );
		_SetValue( m_MultiKillInfo );
	}
};

#endif	
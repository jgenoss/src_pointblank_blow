#ifndef __S2_MO_STRUCT_CLAN_WAR_H__
#define __S2_MO_STRUCT_CLAN_WAR_H__

struct PACKET_CLAN_WAR_MATCH_TEAM_LIST_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_REQ>
{	
};

struct PACKET_CLAN_WAR_MATCH_TEAM_LIST_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK>
{
	S2MOValue<UINT8>											m_ui8MyClanTeam;		// 자기 클랜 매치팀 수. 최대 MAX_CLAN_MATCH_TEAM_COUNT 
	S2MOValue<MATCH_TEAM_INFO, MAX_CLAN_MATCH_TEAM_COUNT>		m_aTeamInfo;

	PACKET_CLAN_WAR_MATCH_TEAM_LIST_ACK()
	{
		_SetValue( m_ui8MyClanTeam );
		_SetValue( m_aTeamInfo ); 
	}
};


struct PACKET_CLAN_WAR_MATCH_TEAM_COUNT_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_MATCH_TEAM_COUNT_ACK>
{
	S2MOValue<INT32>											m_i32MatchedTeamCount;	// 매칭된  팀 개수.( 레디방 이동된 ). 전체 서버
	S2MOValue<INT32>											m_i32MatchingTeamCount;	// 매칭중인  팀 개수. 전체 서버
	
	PACKET_CLAN_WAR_MATCH_TEAM_COUNT_ACK()
	{
		_SetValue( m_i32MatchedTeamCount );
		_SetValue( m_i32MatchingTeamCount ); 
	}
};
 
struct PACKET_CLAN_WAR_PRESEASON_RANKING_INFO_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_PRESEASON_RANKING_ACK>
{
	S2MOValue<INT32>												m_i32SeasonRankingCount;		// 클랜 갯수. 0~3.
	S2MOValue<CLAN_BASIC_INFO, MAX_SEASON_RANKING_TEAM_COUNT>		m_aSeasonRankingClan;			// 클랜 정보.
	S2MOStringW<SEASON_NAME_LENGTH>									m_strSeasonName;				// 시즌명

	PACKET_CLAN_WAR_PRESEASON_RANKING_INFO_ACK()
	{
		_SetValue( m_i32SeasonRankingCount );
		_SetValue( m_aSeasonRankingClan );
		_SetValue( m_strSeasonName );
	}
};

struct PACKET_CLAN_WAR_CREATE_TEAM_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_CREATE_TEAM_REQ>
{
	S2MOValue<UINT8>							m_ui8MaxPer;	// 팀 최대 인원수
	
	PACKET_CLAN_WAR_CREATE_TEAM_REQ()
	{
		_SetValue( m_ui8MaxPer ); 
	}
};

struct PACKET_CLAN_WAR_CREATE_TEAM_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK>
{
	S2MOValue<MATCH_TEAM_INFO>					m_TeamlInfo;		// 매치팀 정보.
	
	PACKET_CLAN_WAR_CREATE_TEAM_ACK()
	{
		_SetValue( m_TeamlInfo );
	}
};


struct PACKET_CLAN_WAR_JOIN_TEAM_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_JOIN_TEAM_REQ>
{
	S2MOValue<T_TeamID>							m_TTeamID;				// 매치팀 ID.
	
	PACKET_CLAN_WAR_JOIN_TEAM_REQ()
	{
		_SetValue( m_TTeamID ); 
	}
};

struct PACKET_CLAN_WAR_JOIN_TEAM_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK>
{
	S2MOValue<bool>												m_bIsMyEnter;			// 입장하는 유저가 본인인지.
	S2INT8														m_i8JoinSlot;			// 입장하는 유저 슬롯.
	S2MOValue<MATCH_TEAM_DETAIL_INFO>							m_TeamDetailInfo;		// 매치팀 정보.
	S2MOValue<MATCH_MEMBER_INFO, MAX_CLAN_MATCH_MEMBER>			m_aMemberInfo;			// 매치팀원 정보.

	PACKET_CLAN_WAR_JOIN_TEAM_ACK()
	{
		_SetValue( m_bIsMyEnter );
		_SetValue( m_i8JoinSlot );
		_SetValue( m_TeamDetailInfo );
		_SetValue( m_aMemberInfo );
	}
};

struct PACKET_CLAN_WAR_LEAVE_TEAM_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_LEAVE_TEAM_REQ >
{
};

struct PACKET_CLAN_WAR_LEAVE_TEAM_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK>
{
	S2MOValue<INT8>									m_i8LeaderSlot;			// 매치팀 리더 슬롯 인덱스. 리더가 나가면 가장 빠른 슬롯이 리더가됨.
	S2MOValue<INT8>									m_i8LeaveSlot;			// 나간 유저 슬롯 인덱스.
	S2MOValue<bool>									m_bIsMyLeave;			// 나가는 유저가 본인인지.

	//m_TResult 값에 따라 다른 처리필요.
	//EVENT_ERROR_SUCCESS : 유저 나가기 성공. m_i8LeaveSlot가 나간 유저.
	//EVENT_ERROR_WRONG_SLOT_IDX : 유저 나가기 실패. 매치팀 슬롯 인덱스 오류
	//EVENT_ERROR_CLAN_BATTLE_TEAM_CLOSE : 매치팀 해체. 모든 팀원들이 나감 or  방에 용병들만 남은 경우. 

	PACKET_CLAN_WAR_LEAVE_TEAM_ACK()
	{
		_SetValue( m_i8LeaderSlot ); 
		_SetValue( m_i8LeaveSlot ); 
		_SetValue( m_bIsMyLeave ); 
	}
};

struct PACKET_CLAN_WAR_CHANGE_OPERATION_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_CHANGE_OPERATION_REQ>
{
	S2MOStringW<NET_CLAN_OPERATION_SIZE>			m_strOperation;				// 작전명.
	
	PACKET_CLAN_WAR_CHANGE_OPERATION_REQ()
	{
		_SetValue( m_strOperation ); 
	}
};

struct PACKET_CLAN_WAR_CHANGE_OPERATION_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK>
{
	S2MOStringW<NET_CLAN_OPERATION_SIZE>			m_strOperation;				// 작전명.
 
	PACKET_CLAN_WAR_CHANGE_OPERATION_ACK()
	{
		_SetValue( m_strOperation ); 
	}
};

struct PACKET_CLAN_WAR_CHANGE_MAX_PER_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_REQ>
{
	S2MOValue<UINT8>								m_ui8MaxMemberCount;		// 팀 최대 인원수
	
	PACKET_CLAN_WAR_CHANGE_MAX_PER_REQ()
	{
		_SetValue( m_ui8MaxMemberCount ); 
	}
};

struct PACKET_CLAN_WAR_CHANGE_MAX_PER_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK>
{
	S2MOValue<UINT8>								m_ui8MaxMemberCount;		// 팀 최대 인원수
 
	PACKET_CLAN_WAR_CHANGE_MAX_PER_ACK()
	{
		_SetValue( m_ui8MaxMemberCount ); 
	}
};

struct PACKET_CLAN_WAR_MATCHMAKING_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_MATCHMAKING_REQ >
{
};

struct PACKET_CLAN_WAR_MATCHMAKING_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_MATCHMAKING_ACK>
{
};


struct PACKET_CLAN_WAR_MERCENARY_LIST_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_MERCENARY_LIST_REQ >
{
};

struct PACKET_CLAN_WAR_CANCEL_MATCHMAKING_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_REQ >
{
};

struct PACKET_CLAN_WAR_CANCEL_MATCHMAKING_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK>
{
};

struct PACKET_CLAN_WAR_MERCENARY_LIST_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK>
{
	S2MOValue<UINT16>												m_ui16TotalCount;	// 총 용병수.
	S2MOValue<UINT16>												m_ui16CurCount;		// 이번패킷에서 용병수.
	S2MOValue<UINT16>												m_ui16StartIdx;		// 이번패킷에서 배열 시작 인덱스.
	S2MOValue<MERCENARY_INFO, MAX_MERCENARY_LIST_PER_PACKET>		m_aMerInfo;			// 용병 정보.

	PACKET_CLAN_WAR_MERCENARY_LIST_ACK()
	{
		_SetValue( m_ui16TotalCount ); 
		_SetValue( m_ui16CurCount ); 
		_SetValue( m_ui16StartIdx ); 
		_SetValue( m_aMerInfo ); 
	}
};



struct PACKET_CLAN_WAR_REGIST_MERCENARY_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_REGIST_MERCENARY_REQ>
{
};

struct PACKET_CLAN_WAR_REGIST_MERCENARY_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK>
{
};

struct PACKET_CLAN_WAR_REMOVE_MERCENARY_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_REQ>
{

};
struct PACKET_CLAN_WAR_REMOVE_MERCENARY_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK>
{
	
};


struct PACKET_CLAN_WAR_INVITE_MERCENARY_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_INVITE_MERCENARY_REQ>
{
	S2MOValue<T_MerID>										m_TMerID;	// 용병 ID.
	PACKET_CLAN_WAR_INVITE_MERCENARY_REQ()
	{
		_SetValue( m_TMerID ); 
	}
};

struct PACKET_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK>
{

};
struct PACKET_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK>
{
	S2MOStringW<NET_CLAN_NAME_SIZE>							m_strClanName;				// 초대한 팀의 클랜 명.
	PACKET_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK()
	{
		_SetValue( m_strClanName ); 
	}
};


struct PACKET_CLAN_WAR_INVITE_ACCEPT_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_INVITE_ACCEPT_REQ>
{
};
struct PACKET_CLAN_WAR_INVITE_ACCEPT_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK>
{
};


struct PACKET_CLAN_WAR_INVITE_DENIAL_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_INVITE_DENIAL_REQ>
{
};
struct PACKET_CLAN_WAR_INVITE_DENIAL_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK>
{
	S2MOValue<T_MerID>										m_TMerID;	// 거절한 용병 ID.
	PACKET_CLAN_WAR_INVITE_DENIAL_ACK()
	{
		_SetValue( m_TMerID ); 
	}
};

struct PACKET_CLAN_WAR_TEAM_CHATTING_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_TEAM_CHATTING_REQ>
{
	S2MOStringW<NET_CLAN_CHATING_SIZE>						m_strChatting;			 
	PACKET_CLAN_WAR_TEAM_CHATTING_REQ()
	{
		_SetValue( m_strChatting ); 
	}
};
struct PACKET_CLAN_WAR_TEAM_CHATTING_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK>
{
	// 결과값이 EVENT_ERROR_CHATTING_LIMIT 면 아래 타입에 따라 제재.
	S2MOValue<UINT8>										m_ui8LimitType;	// CHAT_LIMIT.

	S2MOValue<UINT8>										m_ui8NickColor;	
	S2MOStringW<NET_NICK_NAME_SIZE>							m_strNick;			 
	S2MOStringW<NET_CLAN_CHATING_SIZE>						m_strChatting;			
	PACKET_CLAN_WAR_TEAM_CHATTING_ACK()
	{
		_SetValue( m_ui8LimitType ); 
		_SetValue( m_ui8NickColor ); 
		_SetValue( m_strNick ); 
		_SetValue( m_strChatting ); 
	}
};


struct PACKET_CLAN_WAR_MATCH_TEAM_INFO_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_MATCH_TEAM_INFO_ACK>
{
	S2MOValue<CLAN_MATCHING_TEAM_INFO, TEAM_COUNT>					m_aClanInfo;
	S2MOValue<MATCHMAKING_RECORD, SLOT_MAX_COUNT>					m_aMatchMakingRecord;

	PACKET_CLAN_WAR_MATCH_TEAM_INFO_ACK()
	{
		_SetValue( m_aClanInfo ); 
		_SetValue(m_aMatchMakingRecord);
	}
};

struct PACKET_CLAN_WAR_CHANGE_MEMBER_INFO_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_CHANGE_MEMBER_INFO_ACK>
{
	S2MOValue<UINT8>										m_ui8Type;		// CHANGE_INFO. 

	// 주의 : 해당 타입만 변경. ex : CHANGE_INFO_NICKNAME이면 m_stMemberInfo.m_strNick만 변경.
	S2MOValue<MATCH_MEMBER_INFO>							m_stMemberInfo;
	
	PACKET_CLAN_WAR_CHANGE_MEMBER_INFO_ACK()
	{
		_SetValue( m_ui8Type ); 
		_SetValue( m_stMemberInfo ); 
	}
};

struct PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ>
{
	S2MOValue<T_MerID>										m_TMerID;		// 용병 ID

	PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ()
	{
		_SetValue( m_TMerID ); 
	}
};

struct PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK>
{
	S2MOValue<T_MerID>										m_TMerID;			// 용병 ID
	S2MOValue<MERCENARY_DETAIL_INFO>						m_DetailInfo;		// 상세 정보
	S2MOValue<S2_USER_DETAIL_INFO>							m_UserDetailInfo;	// 유저 상세 정보

	PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK()
	{
		// Last Update : 2015-10-12 14:04:58
		_SetValue( m_TMerID );
		_SetValue( m_DetailInfo );
		_SetValue( m_UserDetailInfo );
	}
};

struct PACKET_CLAN_WAR_MATCH_MAKING_RESULT_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_MATCH_MAKING_RESULT_ACK>
{
	S2MOValue<CLAN_MATCH_MAKING_RESULT>						m_MatchMakingResult;			// 매치메이킹 결과
	 
	PACKET_CLAN_WAR_MATCH_MAKING_RESULT_ACK()
	{
		_SetValue( m_MatchMakingResult ); 
	}
};

struct PACKET_CLAN_WAR_RESULT_ACK : public S2MOPacketBaseResultT<PROTOCOL_CLAN_WAR_RESULT_ACK>
{
	S2MOValue<CLAN_WAR_RESULT, MAX_MATCH_RESULT_COUNT>		m_aClanWarResult;		// 클랜 매치 메이킹 결과 정보. 최근 15개. 

	PACKET_CLAN_WAR_RESULT_ACK()
	{
		_SetValue(m_aClanWarResult);
	}
};

struct PACKET_CLAN_WAR_REPLACE_CLAN_NAME_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_REPLACE_CLAN_NAME_ACK>
{
	S2MOValue<T_TeamID>										m_TTeamID;				// 변경되는 매치팀 ID
	S2MOStringW<NET_CLAN_NAME_SIZE>							m_strClanName;			// 클랜명

	PACKET_CLAN_WAR_REPLACE_CLAN_NAME_ACK()
	{
		_SetValue(m_TTeamID);
		_SetValue(m_strClanName);
	}
};

struct PACKET_CLAN_WAR_REPLACE_CLAN_MARK_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_REPLACE_CLAN_MARK_ACK>
{
	S2MOValue<T_TeamID>										m_TTeamID;				// 변경되는 매치팀 ID
	S2MOValue<UINT32>										m_ui32Mark;				// 클랜 마크

	PACKET_CLAN_WAR_REPLACE_CLAN_MARK_ACK()
	{
		_SetValue(m_TTeamID);
		_SetValue(m_ui32Mark);
	}
};

struct PACKET_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK : public S2MOPacketBaseT<PROTOCOL_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK>
{
	S2MOValue<T_TeamID>										m_TTeamID;				// 변경되는 매치팀 ID
	S2MOValue<UINT8>										m_ui8Color;				// 컬러 

	PACKET_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK()
	{
		_SetValue(m_TTeamID);
		_SetValue(m_ui8Color);
	}
};


#endif


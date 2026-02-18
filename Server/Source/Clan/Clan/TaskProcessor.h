#ifndef _TaskProcessor_h
#define _TaskProcessor_h

class CPacketLocker;
class CGateway;

#include "../../../../Common/CommonSource/ClanMatch/Ranking.h"
#include "RingBuffer.h"
#include "BattleTeam.h"

#include "MercenaryMgr.h"
#include "MatchManager.h"
#include "MatchTeam.h"


struct MATCHING_RESULT;

class CTaskProcessor: public i3Thread
{
	I3_CLASS_DEFINE(CTaskProcessor);

	BOOL						m_bIsRunning;
	UINT8*						m_pui8RecvBuffer;
	UINT8*						m_pui8SendBuffer;
	UINT32						m_ui32DataSize;
	UINT32						m_ui32ReadedLength;
	CPacketLocker*				m_pPacketLocker;
	CGateway*					m_pGateway;

	CLAN_CONTEXT				m_ClanListContext;
	CLAN_DETAIL_INFO_SERVER*	m_pClanDetailInfo;

	CBattleTeam*				m_pBattleTeam;

	i3BinListEx*				m_pListClan;

	i3List*						m_pListClanTemp;

	i3List*						m_pListMemberTemp;
	i3List*						m_pListRequestTemp;

	UINT8	*					m_pui8SendUserCount;
	INT32	**					m_ppi32SendSession;
	T_UID	**					m_ppi64SendDBIdx;

	UINT32						m_ui32ClanOrderIdx;

	INT32						m_i32ClanTempCount;
	//CHashTableUser*				m_pUserNode;

	//CRankingManager				m_RankingManager;

	UINT32						m_ui32TeamOrderTime;

	INT32						m_i32SeasonRankingCount;
	CLAN_BASIC_INFO				m_aSeasonRankingClan[MAX_SEASON_RANKING_TEAM_COUNT];
	UINT32						m_ui32dMatchTeamSendTime;

	void						_DoTaskForGame( UINT32 ui32idxBuffer );
	BOOL						_DoTaskForGameGlobal( UINT32 ui32idxBuffer, i3NetworkPacket* pPacket );
	BOOL						_DoTaskForGameUser( UINT32 ui32idxBuffer, i3NetworkPacket* pPacket );
	BOOL						_DoTaskForGameBattle( UINT32 ui32idxBuffer, i3NetworkPacket* pPacket );
	void						_DoTaskForRecursive(void);

	BOOL						_DoTaskForClanWar(UINT32 ui32BufferIdx, i3NetworkPacket* pPacket);
	
	void						_DoTaskForDB( RING_OUT_DB* pOutDB );

private:
	INT32						m_i32LogIdx;

public:
	CTaskProcessor();
	virtual ~CTaskProcessor();

	BOOL						Create(UINT32 ui32DataSize,CPacketLocker* pPacketLocker,CGateway* pGateway);
	BOOL						_ClanCreate(void);

	virtual UINT32				OnRunning(void* pUserData);		
	BOOL						SendPacketMessage(UINT32 idx,i3NetworkPacket* pPacket);
	BOOL						SendRecursivePacket(i3NetworkPacket* pPacket);

	void						PushTempClan( CLAN_DETAIL_INFO_SERVER* pstClan );
	CLAN_DETAIL_INFO_SERVER*	PopTempClan();

	void						PushTempMember( CLAN_MEMBER_DETAIL_INFO* pstMember );
	CLAN_MEMBER_DETAIL_INFO*	PopTempMember();

	void						PushTempRequest( CLAN_REQUEST_SERVER_INFO* pstReauest );
	CLAN_REQUEST_SERVER_INFO*	PopTempRequest();

	const UINT32				GetClanCount(void) const			{	return m_ClanListContext._totalCount;	}
	const CLAN_CONTEXT*			GetClanContext(void) const			{	return &m_ClanListContext;				}	

	UINT8						GetClanPersonRank( UINT8 ui8Person ); 
	UINT8						GetClanExpRank( UINT32 ui32Exp );
	UINT32						GetClanExp( UINT8 ui8Rank );

	void						SendPacket( PROTOCOL ProtocolID, CLAN_USER_INFO* pUserInfo, INT32 i32Success );
	void						SendPacket( PROTOCOL ProtocolID, UINT32 ui32ServerIdx, INT32 i32SessionIdx, T_UID i64UID, INT32 i32Success );
	void						SendPacketInvitedResult( PROTOCOL ProtocolID, UINT32 ui32ServerIdx, INT32 i32SessionIdx, T_UID i64UID, INT64 i64SUID );

	void						SetClanListContect();

	void						SetMemberListContect( CLAN_DETAIL_INFO_SERVER* pstClan );
	void						SetRequestListContect( CLAN_DETAIL_INFO_SERVER* pstClan );	

	void						SetClanChangeStateDate( CLAN_DETAIL_INFO_SERVER* pstClan );

	void						DeleteRequest( CLAN_DETAIL_INFO_SERVER* pstClan, T_UID i64UID );

	BOOL						GetMenagement( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstMember, CLAN_MENAGE_AUTHORITY_TYPE eMenageType );
	void						ClanChangePerson( CLAN_DETAIL_INFO_SERVER* pstClan );
	
	void						SendMemberInfo( CLAN_USER_INFO* pstUserInfo, CLAN_DETAIL_INFO_SERVER* pstClan );

	void						SendClanMemberStateChange( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser );
	void						SendMemberInfo_Insert( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser );	
	void						SendMemberInfo_Delete( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser );

	void						CheckHaveItem( CLAN_DETAIL_INFO_SERVER* pstClan );
	BOOL						CheckUseItem( CLAN_DETAIL_INFO_SERVER* pstClan );
	void						UserLogIn( CLAN_USER_INFO* pUserInfo );																	// 로그인 처리
	void						UserLogOut( CLAN_WAR_INFO* pUserInfo );																// 로그아웃 처리

	void						UserStateChange( CLAN_USER_INFO* pUserInfo, UINT64 ui64State );											// 유저 정보 갱신
	void						UserServerEnter( CLAN_USER_INFO* pUserInfo );															// 서버 입장
	void						UserServerLeave( CLAN_WAR_INFO* pUserInfo );															// 서버 퇴장
	void						UserChannelEnter( CLAN_USER_INFO* pUserInfo, UINT32 ui32Channel );										// 채널 입장
	void						UserChannelLeave( CLAN_WAR_INFO* pUserInfo );															// 채널 퇴장
	void						UserRoomEnter( CLAN_USER_INFO* pUserInfo, UINT32 ui32RoomIdx, bool bIsMatchMaking );											// 방 입장
	void						UserRoomLeave( CLAN_USER_INFO* pUserInfo );																// 방 퇴장
	void						UserClanPageEnter( CLAN_USER_INFO* pUserInfo );															// 클랜 페이지 입장
	void						UserClanPageLeave( CLAN_USER_INFO* pUserInfo );															// 클랜 페이지 퇴장

	void						StreamingClanList( UINT32 ui32ServerIdx, UINT32 ui32StartIdx );

	void						SendClanBasicInfo( CLAN_DETAIL_INFO_SERVER* pstClan );

	void						SendMemberContext( CLAN_USER_INFO* pUserInfo );
	void						SendRequestContext( CLAN_USER_INFO* pUserInfo );
	void						SendMemberList( CLAN_USER_INFO* pUserInfo, UINT8 ui8SectionIdx );
	void						SendRequestList( CLAN_USER_INFO* pUserInfo, UINT8 ui8SectionIdx );

	// Clan
	void						ClanSendConnectMemberChangeInfo( PROTOCOL ProtocolID, CLAN_DETAIL_INFO_SERVER* pstClan, void* pArg, UINT16 ui16Size, PROTOCOL TeamProtocolID = 0 );

	void						ClanListDetailInfo( CLAN_USER_INFO* pUserInfo, UINT8 cType );											// 클랜 리스트에서 특정 클랜 선택시 해당 클랜의 상세정보 요청
	void						ClanDetailInfo( CLAN_USER_INFO* pUserInfo, UINT8 ui8Type );												// 클랜 상세정보_클랜정보
	void						ClanDetailInfoMember( CLAN_USER_INFO* pUserInfo, T_UID wMemberDBIdx );									// 클랜 상세정보_멤버 정보
	void						ClanDetailInfoRequest( CLAN_USER_INFO* pUserInfo, T_UID i64UID );										// 클랜 상세정보_가입신청서

	void						ClanCreate( CLAN_USER_INFO* pUserInfo, CLAN_CS_CREATE_INFO* pClanCreateInfo );							// 클랜 생성
	void						ClanClose( CLAN_USER_INFO* pUserInfo );																	// 클랜 해채

	void						ClanRequestJoin( CLAN_USER_INFO* pUserInfo, CLAN_CS_JOIN_INFO* pClanJoinInfo );							// 클랜 가입 신청서			-- DB Thread
	void						ClanRequestAccept( CLAN_USER_INFO* pUserInfo, UINT8 ui8Count, T_UID* pi64UID );							// 클랜 가입 신청 수락
	void						ClanRequestCancel( CLAN_USER_INFO* pUserInfo );															// 클랜 가입 신청 취소
	void						ClanRequestDenial( CLAN_USER_INFO* pUserInfo, UINT8 ui8Count, T_UID* pi64UID );							// 클랜 가입 신청 거절

	void						ClanInvite( CLAN_USER_INFO* pUserInfo, T_UID i64UID );				
	void						ClanInviteAccept( CLAN_USER_INFO* pUserInfo, UINT32 ui32State, UINT8 ui8Accept, UINT32 ui32Rank, TTCHAR* pstrNick, UINT8 ui8NickColor );	// 클랜 초대 신청 수락

	void						ClanCommissionMaster( CLAN_USER_INFO* pUserInfo, T_UID i64UID, UINT8 ui8CommisionRank);											// 클랜 마스터 위임
	void						ClanCommissionStaff( CLAN_USER_INFO* pUserInfo, UINT8 ui8Count, T_UID* pi64UID );						// 클랜 스태프 임명
	void						ClanCommissionRegular( CLAN_USER_INFO* pUserInfo, UINT8 ui8Count, T_UID* pi64UID );						// 클랜 일반 임명

	void						ClanSecession( CLAN_USER_INFO* pUserInfo );																// 클랜 탈퇴
	void						ClanDeportation( CLAN_USER_INFO* pUserInfo, UINT8 ui8Count, T_UID* pi64MemberDBIdx );					// 클랜 제명

	void						ClanMemChatting( CLAN_USER_INFO* pUserInfo, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor );	// 클랜 맴버 체팅
	void						ClanPageChatting( CLAN_USER_INFO* pUserInfo, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser,  UINT8 ui8NickColor );	// 클랜 페이지 체팅

	void						ClanCheckMark( CLAN_USER_INFO* pUserInfo, UINT32 ui32Mark );											// 클랜 체크_마크 중복		-- Not Need Thread
	INT32						_ClanCheckMark( UINT32 ui32Mark );
	void						ClanCheckJoinAuthority( CLAN_USER_INFO* pUserInfo, UINT8 ui8Age, UINT8 ui8Rank );						// 클랜 체크_가입 조건		-- Not Need Thread

	void						ClanReplaceNotice( CLAN_USER_INFO* pUserInfo, TTCHAR* pstrNotice );										// 클랜 교체_공지사항		-- DB Thread
	void						ClanReplaceIntro( CLAN_USER_INFO* pUserInfo, TTCHAR* pstrIntro );											// 클랜 교체_소개			-- DB Thread
	void						ClanReplaceClanName( CLAN_USER_INFO* pUserInfo, TTCHAR* pstrName );										// 클랜 교체_이름			-- DB Thread
	void						ClanReplaceMark( CLAN_USER_INFO* pUserInfo, UINT32 ui32Mark );											// 클랜 교체_마크			-- DB Thread
	void						ClanReplacePersonMax( CLAN_USER_INFO* pUserInfo, UINT16 ui16AddPersonCount );							// 클랜 교체_최대인원		-- DB Thread
	void						ClanReplaceManagement( CLAN_USER_INFO* pUserInfo, CLAN_MANAGEMENT* pManagement );						// 클랜 교체_가입조건		-- DB Thread
	void						ClanReplaceMemberNick( CLAN_WAR_INFO* pUserInfo, MATCH_MEMBER_INFO* pMemberInfo );										// 클랜 교체_멤버 닉네임	-- DB Thread
	void						ClanReplaceMemberColorNick( CLAN_WAR_INFO* pUserInfo, MATCH_MEMBER_INFO* pMemberInfo );								// 클랜 교체_멤버 칼라 닉네임
	void						ClanReplaceMemberRank( CLAN_WAR_INFO* pUserInfo, MATCH_MEMBER_INFO* pMemberInfo );										// 클랜 교체_멤버 랭크		-- DB Thread

	void						ClanMemberInvitedRoom( CLAN_USER_INFO* pUserInfo, T_UID i64UID );										// 클랜 멤버 방에 초대

	void						ClanNote( CLAN_USER_INFO* pUserInfo, UINT8 ui8Type, UINT8 ui8NoteSize, TTCHAR* pstrNote );				// 클랜 전체 쪽지	

	void						ClanCheckName( CLAN_USER_INFO* pUserInfo, TTCHAR* pstrClanName );											// 클랜 이름 중복 체크		-- DB Thread
	void						ClanCheckAzit( CLAN_USER_INFO* pUserInfo, TTCHAR* pstrAzit );												// 클랜 아지트 중복 체크	-- DB Thread

	void						ClanBattleRecordReset( CLAN_USER_INFO* pUserInfo );														// 클랜 전적 초기화			-- DB Thread
	void						ClanBattlePointReset( CLAN_USER_INFO* pUserInfo );														// 클랜 포인트 초기화		-- DB Thread
	void						ClanBattleExpUp( CLAN_USER_INFO* pUserInfo, UINT32 ui32ItemID, UINT32 ui32Date );						// 클랜 경험치 증가권
	void						ClanBattleReplaceColorName( CLAN_USER_INFO* pUserInfo, UINT32 ui32ItemID, UINT32 ui32Date, UINT8 ui8Color);	// 클랜 컬러명

	void						ClanItemDelete( CLAN_USER_INFO* pUserInfo, UINT32 ui32DeleteItemID );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CLAN CS
	void						ClanRequestAutoAcceptOption( UINT32 ui32ServerIdx, PACKET_GC_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ* pPacketReqGC );	// 클랜 가입신청서 (자동,수동) 승인 설정 요청(클랜마스터만 가능)
	void						SendClanRequestAutoAcceptOption( UINT32 ui32ServerIdx, PACKET_GC_H_CLAN_ACK* pPacketAckGCH );								// 클랜 가입신청서 (자동,수동) 승인 설정 응답

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLAN GameServer -> ClanServer
	void						ClanFindUserInfo( CLAN_USER_INFO* pstUserInfo, T_UID i64FindUID );											// 유저 찾기 기본정보 중 클랜 정보 요청
	void						SendClanFindUserInfo( UINT32 ui32ServerIdx, PACKET_GC_NEW_CLAN_USER_INFO_ACK* pPacketAckGC );				// 유저 찾기 기본정보 중 클랜 정보 응답

	void						SetMatchResultLastCount( MATCH_RESULT_CONTEXT_SERVER* pstClanMatchResultContext );																					// 클랜 매치 결과 저장 마지 카운트 갱신
	void						SendMatchResult_Insert( CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser );				// 클랜 매치 결과 업데이트

	//Battle 
	void						BattleResultUser( UINT8 ui8Count, CLAN_RESULT_USER* pResultUserList, CMercenaryManager*	pMercenaryMgr  );
	void						BattleResult(  UINT8 ui8ClanCount, CLAN_RESULT* pClanResult, UINT8 ui8UserCount, CLAN_RESULT_USER* pClanResultUser, CTeamManager* pTeamManager, UINT8 ui8IsMatchMaking );
	
	void						BattleMatchTeamLeaveRoom( CLAN_RESULT* pClanResult, BATTLE_INFO* pBattleInfo, CLAN_DETAIL_INFO_SERVER* pstClan );

	void						BattleMatchTeamContext( CLAN_USER_INFO* pUserInfo );
	void						BattleMatchGetClanRoomInfo( CLAN_USER_INFO* pstUserInfo, UINT32 ui32Channel );
	void						BattleMatchAllTeamContext( CLAN_USER_INFO* pUserInfo, UINT32 ui32Channel );
	void						BattleMatchAllTeamList( CLAN_USER_INFO* pUserInfo, UINT32 ui32Channel, UINT16 ui16Section );
	void						BattleStreamingMatchClanList( CLAN_USER_INFO* pUserInfo, UINT8 ui8Section );
	void						BattleTeamDetailInfo( CLAN_USER_INFO* pUserInfo, UINT32 ui32TeamIdx );

	void						BattleTeamCreate( CLAN_USER_INFO* pUserInfo, UINT32 ui32Channel, INT8 i8PerCount);
	void						BattleTeamJoin( CLAN_USER_INFO* pUserInfo, UINT32 ui32TeamIdx );
	void						BattleTeamJoinNaming( CLAN_USER_INFO* pUserInfo, UINT8 ui8Naming );
	void						BattleTeamLeave( CLAN_USER_INFO* pUserInfo );
	INT32						BattleTeamLeaveUser(CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MEMBER_DETAIL_INFO* pstUser, INT32 i32ServerIdx = INVALID_INDEX);
	void						BattleTeamChangePer( CLAN_USER_INFO* pUserInfo, UINT8 ui8PerMax );
	void						BattleClanRequset( CLAN_USER_INFO* pUserInfo, UINT32 ui32RequsetTeamIdx );
	void						BattleClanAccept( CLAN_USER_INFO* pUserInfo, UINT32 ui32SpIdx, UINT32 ui32TeamIdx, UINT8 ui8Accept );
	void						BattleClanRoomJoin( CLAN_USER_INFO* pUserInfo, UINT32 ui32TeamIdx, UINT32 ui32RequsetTeamIdx, UINT32 ui32RoomIdx );
	
	void						BattleClanRoomLeave( CLAN_USER_INFO* pUserInfo );

	void						MatchUserChatting( CLAN_USER_INFO* pUserInfo, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor );

	void						BattleClanMatchClanInfo( CLAN_USER_INFO* pUserInfo, PACKET_GC_D_CS_MATCH_NEW_CLAN_INFO_REQ* pPacketReqGCD );
	void						BattleClanOperation( CLAN_USER_INFO* pUserInfo, PACKET_GC_D_CS_MATCH_NEW_OPERATION* pPacketReqGCD );

	CLAN_DETAIL_INFO_SERVER* FindClan2( UINT32 ui32ClanDBIdx )
	{
		return (CLAN_DETAIL_INFO_SERVER*)m_pListClan->FindItemEx( ui32ClanDBIdx );

	};

	CLAN_DETAIL_INFO_SERVER* FindClanIdx( UINT32 ui32Count )
	{
		return (CLAN_DETAIL_INFO_SERVER*)m_pListClan->GetItem(ui32Count);
	}


	// Clan Match(1.5)
	CMatching*			GetEmptyMatchBuffer(INT32 i32ServerIdx, T_MatchingID& TMatchingID)			{ return m_pMatchManager[i32ServerIdx]->GetEmptyMatching(TMatchingID); }
	
	void				RequestMatchTeamList(CTeamManager* pTeamManager, CLAN_USER_INFO* pClanUser);

	T_RESULT			CreateClanMatchTeam(CTeamManager* pTeamManager, CLAN_USER_INFO* pClanUser, UINT8 ui8MaxPlayer, INT32 i32KDRate, T_TeamID& TTeamID);
	T_RESULT			JoinClanMatchTeam(CTeamManager* pTeamManager, CLAN_USER_INFO* pClanUser, T_TeamID TTeamID, INT32 i32KillDeathRate, INT8& i8LeaveSlot);
	T_RESULT			LeaveClanMatchTeam(CTeamManager* pTeamManager, CLAN_USER_INFO *pUserInfo , T_TeamID TTeamID, INT8& i8JoinSlot );
	T_RESULT			ChangeOperationClanMatchTeam(CTeamManager* pTeamManager, T_UID TUID, T_TeamID TTeamID, TTCHAR* pOperation);
	T_RESULT			ChangeMaxPerCountClanMatchTeam(CTeamManager* pTeamManager, T_UID TUID, T_TeamID TTeamID, UINT8 ui8MaxPerCount );

	T_RESULT			InsertMatchmakingRequest(CTeamManager* pTeamManager, INT32 i32ServerIdx, T_TeamID TTeamID);
	T_RESULT			CancelMatchmaking(CTeamManager* pTeamManager, T_TeamID TTeamID);

	void				JoinRoomMatchTeams(CLAN_USER_INFO* pUserInfo, T_TeamID TTeamID, UINT32 ui32RequsetTeamIdx, UINT32 ui32RoomIdx);

	
	T_RESULT			InviteMercenary(CMercenaryManager* pMercenaryManager, CLAN_USER_INFO* pUserInfo, T_MerID TMerID, T_TeamID TTeamID);

	CMatchManager*		m_pMatchManager[MAX_GAME_SERVER_COUNT];
	CMercenaryManager*	m_pMercenaryMgr[MAX_GAME_SERVER_COUNT];

	CTeamManager*		m_pTeamManager[MAX_GAME_SERVER_COUNT];

	bool				SendMatchingResult(INT32 i32ServerIdx, MATCHING_RESULT* pMatchingResult);

	// Private
	//void				SendMatchTeamChanges(INT32 i32ServerIdx, BATTLE_INFO* pMatchTeam);
//	void				SendMatchTeamChanges(INT32 i32ServerIdx, CMatchTeam* pMatchTeam);

	INT32				GetSeasonRankingCount( void )	{ return m_i32SeasonRankingCount; }
	CLAN_BASIC_INFO*	GetSeasonRankingClan( void )	{ return m_aSeasonRankingClan; }

	void				SendAllMatchTeamCount( INT32 i32GameServerCount );
	void				SendInviteAccept( T_RESULT TResult, CLAN_USER_INFO *pClanUser );
	void				SendInviteDenial( T_RESULT TResult, CLAN_USER_INFO *pClanUser, T_MerID TMerID, T_UID TLeaderUID = INVALID_IDX );
	void				ClanWarLogout( CLAN_WAR_INFO* pWarInfo );
	void				ClanWarChangeMemberInfo( CHANGE_INFO	eType , CLAN_WAR_INFO* pWarInfo, MATCH_MEMBER_INFO* pMemberInfo );
	void				SendLeaveMatchTeam(CTeamManager* pTeamManager, CLAN_WAR_INFO *pWarInfo , INT8 i8LeaveSlot, T_RESULT TResult );

	INT32				GetMatchingPoint(CTeamManager* pTeamManager, T_TeamID TTeamID);

	CTeamManager*		GetTeamManager(UINT32 ui32ServerIdx ) 
	{
		if( ui32ServerIdx >= MAX_GAME_SERVER_COUNT ) return NULL;
		return m_pTeamManager[ui32ServerIdx];
	}

	// 클랜 매치메이킹 결과 전달.
	void				SendClanWarResult(CLAN_DETAIL_INFO_SERVER* pstClan, CLAN_MATCH_MAKING_RESULT* pResult);		// 실시간 1경기.
	void				MakePacketClanWarResult(CLAN_DETAIL_INFO_SERVER* pstClan, i3NetworkPacket* pPakcet);									// 누적 15경기.
};

extern CTaskProcessor* g_pTaskProcessor;

#endif //_TaskProcessor_h

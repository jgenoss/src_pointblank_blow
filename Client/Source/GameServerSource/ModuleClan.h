#ifndef __MODULE_CLAN_H__
#define __MODULE_CLAN_H__ 

class CUserSession;
class CModuleClanSocket;

class CModuleClan : public i3Thread
{
	I3_CLASS_DEFINE( CModuleClan );
private:
	BOOL						m_bRunning;
	UINT32						m_ui32ThreadCount;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLAN	
	CModuleClanSocket	*		m_pClanSocket;
	UINT32						m_ui32ClanConnectTime; 
	UINT32						m_ui32ClanContextSendTime;

	SERVER_NET_BUFFER				m_sClanMainMessage;
	SERVER_NET_BUFFER				m_sClanAuthMessage;
	SERVER_NET_BUFFER				m_sClanClanMessage;
	SERVER_NET_BUFFER				m_sClanMessMessage;
	SERVER_NET_BUFFER		*		m_pClanUserMessage;

protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLAN
	INT32						_ClanConnect(void);
	INT32						_ClanUpdate(void); 	

public: 
	CModuleClan(void); 
	virtual ~CModuleClan(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	INT32						OnCreate( UINT32 ui32ThreadCount );
	void						OnDestroy(void);
	virtual UINT32				OnRunning( void * pUserData);

	void						SetClanContextTime( UINT32 ui32Time )						{	m_ui32ClanContextSendTime = ui32Time;	};
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLAN
	BOOL	ClanSendTestNCQA(			INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Message, UINT32 ui32Value, UINT32 ui32Value1, UINT32 ui32Value2 );			// QA 요청 메시지
	BOOL	ClanSendLogIn( 				INT32 i32WorkIdx, CUserSession * pSession );																				// 로그인 요청
	BOOL	ClanSendLogOut(				INT32 i32WorkIdx, CUserSession * pSession );																				// 로그아웃 요청
	BOOL	ClanSendServerEnter(		INT32 i32WorkIdx, CUserSession * pSession );																				// 서버 입장 요청
	BOOL	ClanSendServerLeave(		INT32 i32WorkIdx, CUserSession * pSession );																				// 서버 퇴장 요청
	BOOL	ClanSendChannelEnter(		INT32 i32WorkIdx, CUserSession * pSession, INT32 i32ChnnelIdx );											// 체널 입장 요청
	BOOL	ClanSendChannelLeave(		INT32 i32WorkIdx, CUserSession * pSession );																// 체널 퇴장 요청
	BOOL	ClanSendRoomEnter(			INT32 i32WorkIdx, CUserSession * pSession, INT32 i32RoomIdx );												// 방 입장 요청
	BOOL	ClanSendRoomLeave(			INT32 i32WorkIdx, CUserSession * pSession );																// 방 퇴장 요청
	BOOL	ClanSendPageEnter(			INT32 i32WorkIdx, CUserSession * pSession );																// 클랜 페이지 입장 요청
	BOOL	ClanSendPageLeave(			INT32 i32WorkIdx, CUserSession * pSession );																// 클랜 페이지 퇴장 요청
	BOOL	ClanSendCreate(				INT32 i32WorkIdx, CUserSession * pSession, char * pstrClanName, char * pClanIntro, char * pClanAzit, UINT32 ui32Mark );			// 클랜 생성 요청
	BOOL	ClanSendJoinRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, char * pContents, UINT8 ui8Age, UINT8 ui8Rank );					// 클랜 가입 신청서 요청
	BOOL	ClanSendCancelRequest(		INT32 i32WorkIdx, CUserSession * pSession );																				//가입취소	
	BOOL	ClanSendDetailInfo(			INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Type );													//클랜 상세정보 요청
	BOOL	ClanSendMemberContext(		INT32 i32WorkIdx, CUserSession * pSession );																				//유저 정보 요청 
	BOOL	ClanSendMemberList(			INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8section );												//유저 정보 요청 
	BOOL	ClanSendMemberDetailInfo(	INT32 i32WorkIdx, CUserSession * pSession, TUID ClanMemberUID);												//유저 디테일 정보 요청 
	BOOL	ClanSendRequestContext(		INT32 i32WorkIdx, CUserSession * pSession );																//가입신청서 요청
	BOOL	ClanSendRequestList(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8section );												//유저 정보 요청 
	BOOL	ClanSendRequestInfo(		INT32 i32WorkIdx, CUserSession * pSession, TUID RequestIdx);											//유저 상세정보 요청
	BOOL	ClanSendAcceptRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8count, TUID * pList);									//스탭만 권한이 있음
	BOOL	ClanSendDenialRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8count, TUID * pList);									//스탭만 권한이 있음
	BOOL	ClanSendInviteAcceptRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT32 ui32State, UINT8 ui8Accept );
	BOOL	ClanSendChatingRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, char* pstrChating );		//클랜 체팅
	BOOL	ClanSendPageChatingRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, char* pstrChating );
	BOOL	ClanSendSecessionRequest(	INT32 i32WorkIdx, CUserSession * pSession );															//클랜 탈퇴
	BOOL	ClanSendCloseRequest(		INT32 i32WorkIdx, CUserSession * pSession );															//클랜 해체
	BOOL	ClanSendCommissionMasterRequest( INT32 i32WorkIdx, CUserSession * pSession, TUID i64UserIdx );									//클랜장 위임
	BOOL	ClanSendCommissionStaffRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID* pIdx );			//스탭 위임	
	BOOL	ClanSendCommissionRegularRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID* pIdx );

	BOOL	ClanSendDuplicateName( INT32 i32WorkIdx, CUserSession * pSession, char* pszClanName );							// 클랜 이름 중복 검사
	BOOL	ClanSendDuplicateAzit( INT32 i32WorkIdx, CUserSession * pSession, char* pszAzit );								// 클랜 아지트 중복 검사

	BOOL	ClanSendDeportationRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID* pIdx );				//클랜원 제명
	BOOL	ClanSendNoticeRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8NoticeSize, char* pstrNotice );		//공지사항
	BOOL	ClanSendIntroRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8IntroSize, char* pstrIntro );		//소개글 수정
	BOOL	ClanSendNoteRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Tpye, UINT8 ui8NoteSize, char* pstrNote );
	BOOL	ClanSendNameRequest(		INT32 i32WorkIdx, CUserSession * pSession, char* pstrName );							//클랜명 변경
	BOOL	ClanSendCheckMarkRequest(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Mark );							//마크 체크
	BOOL	ClanSendMarkRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Mark );							//마크 변경
	BOOL	ClanSendPersonMaxRequest(	INT32 i32WorkIdx, CUserSession * pSession );											//클랜 인원 추가
	BOOL	ClanSendManagementRequest(	INT32 i32WorkIdx, CUserSession * pSession, CLAN_MANAGEMENT* pManagement );			//기타관리
	BOOL	ClanSendRoomInvitedRequest(	INT32 i32WorkIdx, CUserSession * pSession, TUID wUserIdx );							//클랜초대요청
	BOOL	ClanSendCheckJoinAuthorityRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Age, UINT8 ui8Rank );			//가입조건 체크		
	BOOL	ClanSendRecordResetRequest( INT32 i32WorkIdx, CUserSession * pSession );											// 클랜 전적 초기화
	BOOL	ClanSendPointResetRequest(	INT32 i32WorkIdx, CUserSession * pSession );											// 클랜 포인트 초기화
	BOOL	ClanSendClanColorRequest(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32Date, UINT8 ui8Color );		// 컬러 클랜 변경
	BOOL	ClanSendExpUpRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32Date );						// 클랜 경험치 150%	
	BOOL	ClanSendChangeNickname(		INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName );										// 유저 닉네임 변경
	BOOL	ClanSendChangeRank(			INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Rank );								// 유저 랭킹 변경
	BOOL	ClanSendChangeColorNick(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Color );
	UINT8	GetClanPerRank(				INT32 i32Curper );
	
	BOOL	ClanSendClanInfoRankUpdate( INT32 i32WorkIdx, CUserSession * pSession );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLAN BATTLE SERVER
	BOOL	ClanSendBattleResult(				INT32 i32WorkIdx, UINT8 ui8Match, UINT8 ui8ClanCount, CLAN_RESULT* pClanResult, UINT8 ui8UserCount, CLAN_RESULT_USER* pClanResultUser ); // 클랜 배틀 결과
	BOOL	ClanSendMatchTeamContextRequest(	INT32 i32WorkIdx, CUserSession * pSession );
	BOOL	ClanSendMatchAllTeamContextRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8PerCount );
	BOOL	ClanSendMatchTeamListRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Section );
	BOOL	ClanSendMatchAllTeamListRequest(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Channel, UINT16 ui16Section );
	BOOL	ClanSendMatchTeamDetailRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx );
	BOOL	ClanSendMatchTeamCreateRequest(		INT32 i32WorkIdx, CUserSession * pSession, INT8 i8Channel, INT8 i8PerCount );
	BOOL	ClanSendMatchTeamJoinRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx, UINT8 ui8Type );
	BOOL	ClanSendMatchTeamLeaveRequest(		INT32 i32WorkIdx, CUserSession * pSession );
	BOOL	ClanSendMatchTeamChangePerRequest(	INT32 i32WorkIdx, CUserSession * pSession,  UINT8 ui8PerMax );
	BOOL	ClanSendMatchFightQuickRequestRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count );
	BOOL	ClanSendMatchFightRequestRequest(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32RequsetTeamIdx );	
	BOOL	ClanSendMatchFightAccectRequest(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32SpIdx, UINT32 ui32RequsetTeamIdx, UINT8 ui8Accept );
	BOOL	ClanSendMatchChattingRequest(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, char* pstrChating );

	BOOL	ClanSendRoomLeaveResult(			INT32 i32WorkIdx, CUserSession * pSession );
	BOOL	ClanSendBattleRoomJoin(				INT32 i32WorkIdx, CUserSession * pSession,  UINT32 ui32TeamIdx, UINT32 ui32ReqTeamIdx, INT32 i32RoomIdx );
	BOOL	ClanSendInvite(						INT32 i32WorkIdx, CUserSession * pSession, TUID i64UID );

	BOOL	ClanSendItemDelete(					INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ItemID );
};

extern CModuleClan * g_pModuleClan; 

#endif

#pragma once

#include "CSI_Stage.h"

//
//  로비 위치에 넣기에 전혀 마땅치 않아보이므로, MyRoomInfo위주로 또 따로 뺀다..
//

class MyRoomInfoContext : public i3::shared_ginst<MyRoomInfoContext>
{
public:
	MyRoomInfoContext();
	~MyRoomInfoContext();

	void			InitMyRoomInfo();											// 로드된것에서 입력받는 부분이 있어 불가피하게 추가한다.

	const char *	getMyRoomPW()	const	{ return m_RoomPasswd; }			// 패스워드는 특이하게도 널값이 없는듯 하다..(거지같은 부분임)
	void			ClearMyRoomPW();
	void			SetMyRoomPW(const char* pw, INT32 len);

	void		getMyRoomInfo( ROOM_INFO_BASIC * pInfo ) const;
	void		setMyRoomInfo( const ROOM_INFO_BASIC * pInfo );
	INT32		getMyRoomIndex( void ) const;
	void		setMyRoomIndex( INT32 idx );

	UINT8		GetMyRoomWeaponFlag() const;
	UINT32		GetMyRoomStageID() const;
	UINT8		GetMyRoomInfoFlag() const;
	
	bool		IsAutoTeamChangeableRoomState();		// 자동공수교대가 세팅된 방인가.(웹툴설정, 맵모드에 영향받습니다)
	bool		IsAutoTeamChanged() const;			    // 자동공수교대가 실행되어 팀이바뀐상태인가

	void		setMyRoomScore(const ROOM_NOW_SCORE& pScore)	{ m_RoomNowScore = pScore; }
	const ROOM_NOW_SCORE* getMyRoomScore() const				{ return &m_RoomNowScore; }	

	CSI_STAGE*	getStage( void);
	void		setDummyStage(STAGE_ID stage);			// 싱글모드용 스테이지를 셋팅한다. 해당 스테이지가 등록되면 getStage에서 해당 스테이지 정보를 불러오브로 주의 요망.

	void		getMyRoomInfoAdd( ROOM_INFO_ADD * pInfo ) const;
	void		setMyRoomInfoAdd( const ROOM_INFO_ADD * pInfo );

	UINT8		GetMyRoomSubType() const;
	UINT8		GetMyRoomLockObserver() const;

	// Room
	REAL32		GetInvincibleTime();
	REAL32		GetRespawnTime( INT32 userIdx);

	void		SetManualReadyFlag(bool bEnable)				{ m_bManualReadyFlag = bEnable; }
	bool		IsManualReady(void) const						{ return m_bManualReadyFlag; }

	bool		IsTeamBalanceMessageFlag(void) const			{ return m_bTeamBalanceMsgFlag; }
	void		SetTeamBalanceMessageFlag(bool bEnable)			{ m_bTeamBalanceMsgFlag = bEnable; }
	
	bool		IsTeamChangeFlag(void) const					{ return m_bTeamChangeFlag; }
	void		SetTeamChangeFlag(bool bEnable)					{ m_bTeamChangeFlag = bEnable; }

	bool		IsTeamChangeByHostFlag(void) const { return m_bTeamChangeByHostFlag; }
	void		SetTeamChangeByHostFlag(bool bEnable) { m_bTeamChangeByHostFlag = bEnable; }

	bool		IsSoloPlayFlag(void) const						{ return m_bSoloPlay; }
	void		SetSoloPlayFlag(bool bEnable)					{ m_bSoloPlay = bEnable; }

	// 무기 제한 확인
	bool		IsLockedWeaponSlot(UINT32 idxSlot) const;

	bool		JoinRoom(const ROOM_INFO_BASIC* pInfo);

	INT32		GetCountDown( void)	const						{ return m_CountDown; }
	void		SetCountDown( INT32 cnt)						{ m_CountDown = cnt; }

	void		SetQuickJoin(bool enable)						{ m_bQuickJoin = enable; }
	bool		IsQuickJoin() const								{ return m_bQuickJoin;   }

	bool		IsRandomMap(void) const { return ((GetMyRoomInfoFlag() & ROOM_INFO_FLAG_RANDOM_MODE) == ROOM_INFO_FLAG_RANDOM_MODE); }

	//----------------------------------------------------------------------------------//
	//									keyword : Star Player							//
	//----------------------------------------------------------------------------------//
	void		setInReadyRoom(const bool bFlag)				{ m_bInReadyRoom = bFlag; }
	const bool	getInReadyRoom(void)							{ return m_bInReadyRoom; }

	void		PrintStarPlayerEntranceMsg(const INT32 Idx);

private:
	enum {	ENC_SEED_ROOMINFO		= 3, };

	ROOM_INFO_BASIC	m_MyRoomInfo;							//Room				(C->G)//자신이 들어가 있는 방정보
	ROOM_INFO_ADD	m_MyRoomInfoAdd;						//Room				(C->G)//자신이 들어가 있는 방정보
	char			m_RoomPasswd[NET_ROOM_PW];				//Room				(G->C)//저장할 비밀번호

	ROOM_NOW_SCORE		m_RoomNowScore;					// RoomNowScore
	
	bool			m_bManualReadyFlag;	// 룸에서 수동으로 레디를 했는지 확인용 - 서버에의해 강제로 변경하는 것과 구별하기 위해 필요
	bool			m_bTeamBalanceMsgFlag; //
	bool			m_bTeamChangeFlag;  // 
	bool			m_bTeamChangeByHostFlag;	//방장에 의해서 일괄 팀 교체가 되었는가

	INT32			m_CountDown;		// 카운트 다운 값 (방에 입장시 이값이 있으면 카운트 다운을 한다)
	bool			m_bQuickJoin;		// 빠른입장
	bool			m_bSoloPlay;		// 솔로플레이 가능여부

	bool			m_bInReadyRoom;		// 현재 방에 입장 중인지 체크 (keyword : Star Player)

	CSI_STAGE		m_EmptyStage;
	CSI_STAGE*		m_DummyStage;
};


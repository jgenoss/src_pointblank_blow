#pragma once



class UserInfoContext : public i3::shared_ginst<UserInfoContext>
{
public:
	UserInfoContext();
	~UserInfoContext();

	USER_INFO_RECORD *			SetMyUserInfoRecord(void)						{return m_MyUserInfoRecord; }
	const USER_INFO_RECORD *	GetMyUserInfoRecord(void) const					{return m_MyUserInfoRecord; }

	UI_CHARA_INFO 	*			SetMyUICharaInfo(void)			{ return m_MyUICharaInfo; }
	const UI_CHARA_INFO *		GetMyUICharaInfo(void) const	{ return m_MyUICharaInfo; }

	void		ResetForSingleMode( void);

	UINT32		GetMyRespawnCnt(void)const						{ return m_nMyRespawnCnt;	}
	void		SetMyRespawnCnt(UINT32 Arg)						{ m_nMyRespawnCnt = Arg;}

	UINT8		GetMyTouchDownCnt(void)const					{ return m_nMyTouchDownCnt;	}
	void		SetMyTouchDownCnt(UINT8 Arg)					{ m_nMyTouchDownCnt = Arg;}

	UINT8		GetMyUsurpationIdx(void)const					{ return m_nMyUsurpationIdx;	}
	void		SetMyUsurpationIdx(UINT8 Arg)					{ m_nMyUsurpationIdx = Arg;}

	void		SetMyUserInfoBasic(const USER_INFO_BASIC & UserInfoBasic);
	void		GetMyUserInfoBasic(USER_INFO_BASIC * UserInfoBasic) const;

	void		SetMyUserInfoInvItemData(const USER_INFO_INVITEM_DATA & Data);
	void		GetMyUserInfoInvItemData(USER_INFO_INVITEM_DATA * pData) const;

	void		SetMyUserInfoEquipmentWeapon(const USER_INFO_EQUIPMENT_WEAPON & InfoEquipWeapon);
	void		GetMyUserInfoEquipmentWeapon(USER_INFO_EQUIPMENT_WEAPON & InfoEquipWeapon) const;
	
	UINT32		GetInvenServerTime(void) const						{ return m_InvenServerTime; }
	void		SetInvenServerTime(UINT32 Time)						{ m_InvenServerTime = Time; }
	UINT32		GetInventorySecTime( UINT32 ExpireTime ) const; 

	UINT8		getAge( void)			{ return m_iAge; }
	void		setAge( UINT8 age)		{ m_iAge = age; }
	bool		IsAdultUser(void) const;

	void			setLoginCount(UINT32 ui32Cnt) { m_ui32LoginCnt = ui32Cnt; }
	UINT32			getLoginCount(void) const	  { return m_ui32LoginCnt; }

	void			SetNationalCode(UINT8 ui8Code)	{ m_ui8NationalCode = ui8Code; }
	UINT8			GetNationalCode(void) const		{ return m_ui8NationalCode; }

	void			SetRankUp(UINT32 rank,UINT32 point)		{ m_rankUpRank = rank;	m_rankUpPoint = point;	}
	void			ResetRankUp(void)						{ m_rankUpRank = 0;		m_rankUpPoint = 0;		}
	UINT32			GetRankUpRank(void)						{ return m_rankUpRank;							}
	UINT32			GetRankUpPoint(void)					{ return m_rankUpPoint;							}

	bool			GetReceivedMyInfo() const						{ return m_ReceivedMyInfo; }
	void			SetReceivedMyInfo(bool val)						{ m_ReceivedMyInfo = val; }

	void			SetPCCafePoint( UINT32 ui32Point )	{ m_PCCafePoint = ui32Point; }
	UINT32			GetPCCafePoint()					{ return m_PCCafePoint; }
	void			SetPCCafeExp( UINT32 ui32Exp )		{ m_PCCafeExp = ui32Exp; }
	UINT32			GetPCCafeExp()						{ return m_PCCafeExp; }

	void			SetLastGameWeapon(T_ItemID itemID)	{ m_LastGameWeapon = itemID; }
	T_ItemID		GetLastGameWeapon() const			{ return m_LastGameWeapon; }

	i3::rc_wstring	GetMyNickName() const;

	UINT32			GetMyRank() const;
	UINT32			GetMyClanState() const;
	UINT32			GetMyPoint() const;
	UINT32			GetMyCash() const;

	void			SetMyNickName(const i3::wliteral_range& wstrName);
	void			SetMyNickName(const char* szName);				// TODO : 이 함수는 나중에 삭제되어야한다.(2014.07.14.수빈)
	void			SetMyPoint(UINT32 val);
	void			SetMyCash(UINT32 val);
	void			SetMyNCSACoin(UINT32 ui32NCCoin,UINT32 ui32HappyCoin);
	void			GetMyNCSACoin(UINT32 * ui32NCCoin,UINT32 * ui32HappyCoin);

	bool			IsMyNickName(const i3::wliteral_range& Name) const;
	bool			HaveNickName() const;							

	UINT32			GetRealRank() const;
	INT32			GetMaxExpByRank( INT32 nRank );

	void			SetAuthLevel()
	{
		USER_INFO_BASIC stUserInfoBasic;
		GetMyUserInfoBasic( &stUserInfoBasic );
		m_eAuthLevel		= (AUTH_LEVEL)stUserInfoBasic.m_ui8AuthLevel;
		m_eSpecialAbility	= (SPECIAL_ABILITY)stUserInfoBasic.m_ui8SpecialAbility;
	}

	bool			CheckAuthLevel( AUTH_LEVEL eLevel = AUTH_LEVEL_MOD ) const
	{	
		if( m_eAuthLevel < eLevel ) return false;	
		return true;
	}

	bool			CheckSpecialAbility( SPECIAL_ABILITY eAbility )
	{	
		INT32 i32Check = 1 << (eAbility-1);
		if( i32Check & m_eSpecialAbility ) return true;	
		return false;
	}

	//
	// GM-Observer 관련
	//
	
	bool			IsGM_Observer() const;
	void			SetEnable_GMObserver(bool bEnable) { m_bEnable_GMObserver = bEnable; }
	
	//GM 옵저버화면 표시플랙
	enum GMOBSERVER_VIEWFLAG
	{ 
		GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER		= 0x00000001,
	};

	DWORD			GetGMObserverViewFlag() const; 
	void			AddGMObserverViewFlag(GMOBSERVER_VIEWFLAG flag) { m_gmObserverViewFlag |= flag; }
	void			RemoveGMObserverViewFlag(GMOBSERVER_VIEWFLAG flag) { m_gmObserverViewFlag &= ~flag; }
	bool			IsGMObserverViewFlag(GMOBSERVER_VIEWFLAG flag) const 
	{ 
		return (m_gmObserverViewFlag & flag) ? true : false; 
	}
	
	bool			IsInitialRankUp(void) const { return m_bInitialRankUpFlag; }
	bool			IsGeneralRankUp(void) const { return m_bGeneralRankUpFlag; }
	
	void			SetInitialRankUpFlag(bool bEnable) { m_bInitialRankUpFlag = bEnable; }
	void			SetGeneralRankUpFlag(bool bEnable) { m_bGeneralRankUpFlag = bEnable; }
	
	bool		IsViewHudNick() const {	return m_bViewHUDNick; }
	void		ToggleViewHudNick() {	m_bViewHUDNick = !m_bViewHUDNick;  }

	void					SetUserInfoDaily(const USER_INFO_DAILY& data) { m_UserInfoDaily = data; receive_user_info_daily = true; }
	const USER_INFO_DAILY *	GetUserInfoDaily() const { return &m_UserInfoDaily; }
	USER_INFO_DAILY *		SetUserInfoDaily()       { return &m_UserInfoDaily; }
	bool					IsUserInfoDaily() const { return receive_user_info_daily; }

	void					resetRankUpItemList()								{ memset(m_nRankUpItemList, 0, sizeof(RANKUP_ITEM) * MAX_RANK_COUNT); }
	const RANKUP_ITEM*		GetUserRankUpItemList(INT32 idx)		{ return &m_nRankUpItemList[idx];	}
	void					SetUserRankUpItemList(INT32 idx, RANKUP_ITEM* src)	{ m_nRankUpItemList[idx] = *src;	}	
		
	void					SetDailyGiftGaugeArray(UINT32* src);
	UINT32					GetDailyGiftGauge(INT32 idx) const { return m_ui32DailyGiftGauge[idx]; }
	bool					IsDailyGiftGauge() const { return receive_daily_gift_gauge; }
	

	void					SetTSRewardItemData( T_ItemID itemID, UINT32 itemArg) { m_tsReward_ItemID = itemID; m_tsReward_ItemArg = itemArg; m_enableTsReward = true; }
	UINT32					GetTSRewardItemID() const { return m_tsReward_ItemID;  }
	UINT32					GetTSRewardItemArg() const { return m_tsReward_ItemArg; }

	void					SetTSRewardFlag(bool bEnable)	{ m_enableTsReward = bEnable; }
	bool					IsTSRewardFlag() const			{ return m_enableTsReward; }

	void					SetShowTSRewardMsg(bool bEnable) { m_showTsRewardMsg = bEnable; }
	bool					IsShowTSRewardMsg() const { return m_showTsRewardMsg; }

	void					SetEventReawardFlag(bool bEnable) { m_enableEventReward = bEnable; }
	bool					isEventReawardFlage() const { return m_enableEventReward; }

	bool					IsNewRepair(void) const		{ return m_bEnableNewRepair;		}
	void					SetNewRepair(bool bEnable)	{ m_bEnableNewRepair	= bEnable;	}

	bool					IsDefaultItemChanged(void) const		{ return m_bDefaultItemChanged;		}
	void					SetDefaultItemChanged(bool bChanged)	{ m_bDefaultItemChanged	= bChanged;	}

	void					SetContentsFlag(ContentsEnableFlag ContentsFlag)	{ m_ContentsFlag = ContentsFlag; }
	bool					IsContentsEndabled(CONTENTS eContents)				{ return m_ContentsFlag.IsContentsEnabled(eContents); }

	bool					IsTeamChangeContentEnable() { return IsContentsEndabled(CONTENTS_TEAM_CHANGE); }

	void					SetBillingURL(const i3::rc_wstring& wstrBillingURL);
	const i3::rc_wstring&	GetBillingURL(void)	const								{ return m_wstrBillingURL; }
	
	bool					IsChargeCashEnable(void);

	bool					IsAprilFoolsDay();

	void					SetMaxMultiWeapon( UINT8 count)	{ m_nMaxMultiWeapon = count;	}
	UINT8					GetMaxMultiWeapon()				{ return m_nMaxMultiWeapon;		}

	// 장성급 베레모 관련
	void					SetGeneralRankInfo( INVEN_BUFFER* buf);
	INVEN_BUFFER			GetGeneralRankInfo()		{	return m_GeneralRank_ItemInfo;		}
	bool					IsGeneralMsgBox()			{	return m_bGeneralRank_MsgBoxFlag;	}
	void					SetGeneralMsgBox()			{	m_bGeneralRank_MsgBoxFlag = false;	}
	bool					IsGeneralRecvData()			{	return m_bGeneralRank_RecvDataFlag;}

	void					SetReceivedUserInfo(bool flag)	{ m_bReceivedUserInfo = flag; }
	bool					GetReceivedUserInfo()			{ return m_bReceivedUserInfo; }
	
	void					SetCreateNickName(bool flag)	{ m_bCreateNickName = flag; }
	bool					GetCreateNickName()				{ return m_bCreateNickName; }

private:
	void					CreateGMObserverCallSignNumSprite();

private:

	enum
	{
		ENC_SEED_USERINFOBASIC	= 4,	ENC_SEED_USERINFOINVEN	= 5,
		ENC_SEED_USERINFOWEAPON	= 6,	ENC_SEED_USERINFOCHARA	= 7,
	};
	//----------------------------------------------------------------------------------//
	//자신의 정보를 저장하는 저장소 입니다. 
	USER_INFO_BASIC				*	m_MyUserInfoBasic; 
	USER_INFO_RECORD			*	m_MyUserInfoRecord;
	USER_INFO_EQUIPMENT_WEAPON	*	m_MyUserInfoEquipmentWeapon; 

	// User 실제 정보   
	USER_INFO_INVITEM_DATA 		*	m_MyUserInfoInvItemData;     
	// 임시 저장용 서버에 보내기 전에 저장 하거나 임시로 받아서 사용
	UI_CHARA_INFO				*   m_MyUICharaInfo;

	UINT32							m_InvenServerTime;	// 구입한 물건의 날자를 계산하기 위해 필요 (보여주기 위한 용도)
	UINT8							m_iAge;									// 생년월일
	UINT32							m_ui32LoginCnt;						// 로그인 횟수

	UINT8							m_ui8NationalCode;

 	// 진급 포인트 시스템
	UINT32							m_rankUpRank;
	UINT32							m_rankUpPoint;

	// PC Cafe Bonus
	UINT32							m_PCCafePoint;
	UINT32							m_PCCafeExp;

	// 자신의 리스폰 카운트
	UINT32							m_nMyRespawnCnt;
	// 미션 관련 정보
	UINT8							m_nMyTouchDownCnt;
	UINT8							m_nMyUsurpationIdx;

	// 유저 오늘의 전적
	USER_INFO_DAILY					m_UserInfoDaily;
	bool							receive_user_info_daily;

	T_ItemID						m_LastGameWeapon;

	bool							m_ReceivedMyInfo;	// 이 멤버변수는 썩 좋지 않아 나중에 제거해야한다..
	// GM 권한, 일반 유저 
	AUTH_LEVEL						m_eAuthLevel;
	bool							m_bEnable_GMObserver;
	DWORD							m_gmObserverViewFlag;


	SPECIAL_ABILITY					m_eSpecialAbility;

	bool 							m_bInitialRankUpFlag;
	bool							m_bGeneralRankUpFlag;

	bool							m_bViewHUDNick;

	// 진급시 지급받을 아이템 리스트
	RANKUP_ITEM		m_nRankUpItemList[MAX_RANK_COUNT];

	//판수보상
	UINT32			m_ui32DailyGiftGauge[DAILY_GIFT_TYPE_MAX];	
	bool			receive_daily_gift_gauge;

	UINT32			m_tsReward_ItemID;
	UINT32			m_tsReward_ItemArg;

	bool			m_enableTsReward;
	bool			m_enableEventReward;
	bool			m_showTsRewardMsg;

	bool			m_bEnableNewRepair;
	bool			m_bDefaultItemChanged; //인 게임후에 무기/파츠 등에 장비가 변경될 경우

	UINT8			m_nMaxMultiWeapon;

	ContentsEnableFlag m_ContentsFlag;

	i3::rc_wstring  m_wstrBillingURL;
	//is_change_default_item

	// 장성 계급 여부
	bool			m_bGeneralRank_MsgBoxFlag;
	bool			m_bGeneralRank_RecvDataFlag;
	INVEN_BUFFER	m_GeneralRank_ItemInfo;

	bool			m_bReceivedUserInfo;

	bool			m_bCreateNickName;
};

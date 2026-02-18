#ifndef __CHARA_INFO_CONTEXT_H
#define __CHARA_INFO_CONTEXT_H


class CCharaInfoContext : public i3::shared_ginst<CCharaInfoContext>
{
private:
	//공통
	struct tagCommon
	{
		COMMON_PARTS_INFO		parts;
		COMMON_PARTS_INFO		activeParts;	// 인게임에서는 이 정보를 사용. 인벤 UI 와 분리하기 위해.
	};
	tagCommon m_common;			// 공통

	eCharaState			m_CharaSlotState[S2MO_MULTI_SLOT_COUNT] = { CHARA_STATE_NONE };
	
	//개인
	struct	tagChara
	{
		bool		changed = false;
		TTCHAR		changedNick[NET_CHARA_NICK_NAME_SIZE] = { 0 };

		CHARA_INFO	info;

		tagChara( const CHARA_INFO& chara )
		{
			info = chara;
		}
	};


	// 보유 아바타
	i3::vector< tagChara* > m_vecChara;		


	INT32				m_nMainChara[TEAM_COUNT] = { 0 };							// 현재 사용중인 아바타 Index/ 서버의 인덱스하고는 다르다.
	bool				m_bChangedMultiSlot = false;
	TEAM_TYPE			m_eTeamSlot = TEAM_RED;

	tagChara*			SetChara( int nSlot );
	const tagChara*		GetChara( int nSlot ) const;

public:
	bool		HasBasicChara() const			{ return 0 < GetHasCharacterCount();}
	INT32		GetHasCharacterCount() const	{ return static_cast<INT32>(m_vecChara.size());}

	INT32		GetEmptyCharacterIdx() const;

	INT32		find_slot_idx(UINT8 idx) const;
	UINT8		find_slot_idx_by_DBIdx(T_ItemDBIdx chara_db_idx); 
	INT32		GetMainCharaIdx( void) const								{ return m_nMainChara[m_eTeamSlot]; }
	INT32		GetMainCharaIdx(TEAM_TYPE team) const						{ return m_nMainChara[team]; }
	void		SetMainCharaIdx( TEAM_TYPE team, INT32 idx);
	void		SetMainCharaIdx( INT32 idx );

	void		SetMainCharaBySlotIdx( UINT8 slot_idx);
	UINT8		GetMainCharaSlotIdx( TEAM_TYPE team = TEAM_NONE ) const;
	T_ItemDBIdx GetMainCharaDBIdx(TEAM_TYPE team = TEAM_NONE) const;

	INT32		FindCharaSlotIdxByItemID(T_ItemID chara_id);
	UINT32		FindCharaSlotIdxByItemInfo(::ITEM_INFO chara_item_info) const;

	TEAM_TYPE	GetTeamSlot(void)											{ return m_eTeamSlot; }
	void		SetTeamSlotBySlotIdx(INT32 slot_idx);
	void		SetTeamSlotByCharaID(T_ItemID chara_id);

	const COMMON_PARTS_INFO*	GetCommonPartsInfo() const		{ return &m_common.parts; }	
	COMMON_PARTS_INFO*			SetCommonPartsInfo()			{ return &m_common.parts; }
	void						SetCommonPartsInfo( const COMMON_PARTS_INFO& info ) { m_common.parts = info;}

	const COMMON_PARTS_INFO*	GetActiveCommonPartsInfo() const	{ return &m_common.activeParts; }
	COMMON_PARTS_INFO*			GetActiveCommonPartsInfo()			{ return &m_common.activeParts; }

	eCharaState					GetPlayType( int nSlot ) const						{ I3ASSERT( nSlot < S2MO_MULTI_SLOT_COUNT); return m_CharaSlotState[ nSlot];}
	void						SetPlayType( int nSlot, eCharaState playType )		
	{ 
		if (nSlot >= S2MO_MULTI_SLOT_COUNT)
		{
			I3ASSERT(nSlot < S2MO_MULTI_SLOT_COUNT);
			return;
		}
		m_CharaSlotState[ nSlot] = playType;
	}

	const CHARA_PARTS_INFO *	GetMainCharaPartsInfo( TEAM_TYPE team = TEAM_NONE ) const 
	{
		if( team == TEAM_NONE) 
			team = m_eTeamSlot;

		return GetCharaPartsInfo( m_nMainChara[team] ); 
	}
	void						SetMainCharaPartsInfo( CHARA_PARTS_INFO * pPartsInfo)	{ SetCharaPartsInfo( m_nMainChara[m_eTeamSlot], pPartsInfo); }

	const CHARA_PARTS_INFO*		GetCharaPartsInfo( int nSlot ) const;
	const CHARA_PARTS_INFO*		GetNetCharaPartsInfo(int nNetSlot) const;

		
	const CHARA_PARTS_INFO *	GetCharaPartsInfoBySlotIdx( UINT32 idx) const;
	void						SetCharaPartsInfo( INT32 nSlot, CHARA_PARTS_INFO * pPartsInfo);

	const CHARA_WEAPON_INFO*	GetMainCharaWeaponInfo(TEAM_TYPE team = TEAM_NONE) const
	{
		if (team == TEAM_NONE)
			team = m_eTeamSlot;

		return GetCharaWeaponInfo(m_nMainChara[team]);
	}

	const CHARA_WEAPON_INFO*	GetCharaWeaponInfo( int nSlot ) const;
	const CHARA_WEAPON_INFO*	GetNetCharaWeaponInfo(int nNetSlot) const;
	const CHARA_WEAPON_INFO*	GetCharaWeaponInfoByIdx( UINT32 idx) const;
	void						SetCharaWeaponInfo( INT32 nSlot, CHARA_WEAPON_INFO * pWeaponInfo);

	void						SetCharaEquipInfo( INT32 nSlot, ITEM_INFO* pCharEquipment);

	INT32						GetEmptyPlayType() const;

	const CHARA_INFO *			GetMainCharaInfo( TEAM_TYPE team = TEAM_NONE )
	{
		if( team == TEAM_NONE) 
			team = m_eTeamSlot;

		return GetCharaInfo( m_nMainChara[team] );
	}
	
	void						SetCharaInfo( int nSlot, CHARA_INFO * pInfo);

	const CHARA_INFO*			GetCharaInfo( int nSlot ) const;
	const CHARA_INFO*			GetCharaInfoByList( int idx ) const;
	const CHARA_INFO*			GetCharaInfoBySlotIdx( UINT8 idx ) const;
	CHARA_INFO *				SetCharaInfo( INT32 nSlot);
	const CHARA_SLOT_SKILL*		GetMainCharaSkill( void) const			{ return GetCharaSkill( m_nMainChara[m_eTeamSlot]); }

	bool		AddCharacterInfo( const CHARA_INFO& info );
	INT32		FindCharaInfoIndexByItemID( T_ItemID itemID);
	void		DelCharacterInfo(size_t idx);
	void		SortCharacterInfo();

	const CHARA_SLOT_SKILL*			GetCharaSkill( int nSlot) const;


	const TTCHAR*	GetNickName( int nSlot ) const;
	const TTCHAR*	GetNickName( ::ITEM_INFO chara_item_info ) const;

	UINT32		GetCreateDate( int nSlot ) const;	
	int			GetMultiSlotID( int nMultiSlot ) const;
	

	bool		IsChangedNickName() const;
	bool		SetNickName( int nSlot, const TTCHAR* name );
	
	void		GetChangeCharaNickNameData( PACKET_H_NEW_CHANGE_CHARA_NICK_GROUP_REQ* header, PACKET_D_NEW_CHANGE_CHARA_NICK_GROUP* data ) const;
	void		SetChangeCharaNickName( int arg );

	void		SetChangeMultiSlotIdx();
	void		UndoMultiSlotIdx();
	
	void		GetCurrentSlotWeaponInfo( ITEM_INFO * pWeaponList);
	void		GetNetCharaInfo( NET_CHARA_INFO * pOut );
	void		GetUICharaInfo( UI_CHARA_INFO * pOut);
	void		SetNextChara( bool bRight );

	void		ClearAll();
	bool		CalculateMultiSlotIdx();
	
	//V11용 캐릭터 파츠 검사 함수. (V11용이긴 하지만 현재는 V15에서 써도 상관은 없습니다. - 나중에 수정해야됨.)
	bool		isUsedCharacter(INT32 flag);
public:
	struct LRInfo
	{
		bool	_Acvite = false;
		UINT8	_win = 0;
		UINT8	_lastMode = 0;
		UINT32 _lastStageID = 0;
		UINT16 _lastKill = 0;
		UINT16 _lastDeath = 0;
		INT32  _exp = 0;
		UINT32 _point = 0;
		UINT32  _addExp = 0;
		UINT32 _addPoint = 0;
	};

	void setMyLastRecord(bool active, UINT8 gResult, UINT8 mode,UINT32 stageID, UINT16 kill, UINT16 death, INT32 exp, UINT32 point, UINT32 addExp, UINT32 addPoint)
	{
		lastBattleInfo._Acvite	=	active;
		lastBattleInfo._win =		gResult;
		lastBattleInfo._lastMode =	mode; 
		lastBattleInfo._lastStageID=stageID;
		lastBattleInfo._lastKill =	kill;
		lastBattleInfo._lastDeath =	death;
		lastBattleInfo._exp =		exp;
		lastBattleInfo._point =		point;
		lastBattleInfo._addExp =	addExp;
		lastBattleInfo._addPoint =	addPoint;
	};
	const LRInfo* getMyLastRecord() {return (const LRInfo*)&lastBattleInfo;	};

	struct STCLevel
	{
		i3::vector<INT32>	SlotLevel[CATEGORYLIST_MAX];
		REAL32				rPlayTime = 0.0f;
	};

public:
	void			ResetCheckSkillLevel(void)				{ m_Checklevel.clear();				}
	void			AddCheckSkillLevel(STCLevel* level)		{ m_Checklevel.push_back(*level);	}
	const STCLevel*	GetCheckSkillLevel(INT32 idx)			{ return &m_Checklevel.at(idx);}
	STCLevel*		SetCheckSkillLevel(INT32 idx)			{ return &m_Checklevel.at(idx);}
	void			SetAllCharaData(void);
	bool			CheckAllCharaData(void);
	
	void			SetOneCheckLevel(bool blevel)			{ m_bOneCheckLevel = blevel;}
	bool			GetOneCheckLevel(void) const			{ return m_bOneCheckLevel;}


private:
	LRInfo lastBattleInfo;
	i3::vector<STCLevel>		m_Checklevel;
	bool						m_bOneCheckLevel = false;

public:
	CCharaInfoContext();
	~CCharaInfoContext();
};

#endif
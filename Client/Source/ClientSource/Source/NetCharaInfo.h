#pragma once

#include "UICharaInfo.h"

struct DesigApplyInfo;

class NET_CHARA_INFO
{
private:
	CHARA_PARTS_INFO			_CharacterParts;
	ITEM_INFO					_WeaponList[WEAPON_SLOT_COUNT];
	COMMON_PARTS_INFO			_CommonParts;

	UINT8						_WeaponEndurance[WEAPON_SLOT_COUNT];	// 기존 내구도와 같이 사용. 신규내구도 (0 ~ 100) / 구내구도 (0 or 100)

	UINT8						_Hp;
	INT32						_RespawnIdx;
	INT32						_RespawnObjIdx;
	UINT8						_MyRespawnCnt;
	VEC3D						_RespawnPos;						// 서버에서 받는 좌표로 리스폰 위치 결정 할 경우. 
	UINT8						_CurrentCharaIdx;					// 선택된 캐릭터
	bool						_bAIClient;							// Host, Client에 상관 없이 AI를 설정
	
	T_ItemID					_CharacterID[TEAM_COUNT];			// V3 캐릭터 레드팀, 블루팀 아이디 저장.
	
	UINT8						_aDesigSetType[MAX_EQUIP_USER_TITLE];	// 장비중 호칭..서버에서 주는 값 그대로셋팅..(DesigSet 배열인덱스로 받지 않는다.)

	DesigApplyInfo*				_pDesigApplyInfo;					// MAX_EQUIP_USER_TITLE만큼 만들지 말고 하나만 둔다.(개별적으로 쓸 이유가 없어보임)
	SKILLSYSTEM_APPLY_INFO*		_apSkillSystem_Apply_Info;			// 장비중인 스킬정보 SKILLSYSTEM_APPLY_INFO		_SkillSystem[CATEGORYLIST_MAX];	
																	// CATEGORYLIST_MAX만큼 동적생성하는 코드로 변경..(필요할때 생성)
		

public:
	NET_CHARA_INFO();
	~NET_CHARA_INFO();

	const CHARA_PARTS_INFO* getCharaParts( void) const						{ return &_CharacterParts;	}
	CHARA_PARTS_INFO *		setCharaParts( void)							{ return &_CharacterParts; }
	void					setCharaParts( const CHARA_PARTS_INFO * pInfo)	{ pInfo->CopyTo( &_CharacterParts); }
	void					setCharaParts( const CHARA_PARTS_INFO_OTHER * pInfo) { pInfo->CopyTo( &_CharacterParts); }


	const COMMON_PARTS_INFO * getCommonParts( void) const					{ return &_CommonParts; }
	void					setCommonParts( COMMON_PARTS_INFO * pInfo)		{ pInfo->CopyTo( &_CommonParts); }
	void					setCommonParts( COMMON_PARTS_INFO_OTHER * pInfo){ pInfo->CopyTo( &_CommonParts); }
	
	T_ItemID getWeaponItemID( WEAPON_SLOT_TYPE iSlot) const
	{
		if (iSlot < 0 || iSlot >= WEAPON_SLOT_COUNT)
		{
			I3_BOUNDCHK(iSlot, WEAPON_SLOT_COUNT);
			return 0;
		}
		return _WeaponList[ iSlot].m_TItemID;
	}

	void setWeaponItemID( WEAPON_SLOT_TYPE iSlot, T_ItemID ItemID, T_ItemDBIdx ItemIdx = -1, const char * pszFile = nullptr, INT32 line = 0);

	T_ItemDBIdx getWeaponItemDBIdx( WEAPON_SLOT_TYPE iSlot) const
	{
		if (iSlot < 0 || iSlot >= WEAPON_SLOT_COUNT)
		{
			I3_BOUNDCHK(iSlot, WEAPON_SLOT_COUNT);
			return T_ItemDBIdx(-1);
		}
		return _WeaponList[ iSlot].m_TItemDBIdx;
	}	

	void setWeapon( WEAPON_SLOT_TYPE idx, ITEM_INFO * pInfo)		{ pInfo->CopyTo( &_WeaponList[ idx]);}
	const ITEM_INFO* getWeaponList( WEAPON_SLOT_TYPE idx) const		{ return &_WeaponList[ idx];}

	void SetWeaponList( const ITEM_INFO * pWeaponList)
	{
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
			((ITEM_INFO*)pWeaponList)[i].CopyTo( &_WeaponList[i]);
	}

	void	SetWeaponList( const CHARA_WEAPON_INFO_OTHER * pInfo)
	{
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT - 1; i++)
			setWeaponItemID( (WEAPON_SLOT_TYPE) i, pInfo->m_Info[i]);
	}

	void SetWeaponList( const CHARA_WEAPON_INFO * pInfo)
	{
		i3mem::Copy( _WeaponList, pInfo->m_Info, sizeof( ITEM_INFO ) * CHAR_EQUIPMENT_WEAPON_COUNT );
	}

	void GetWeaponList( CHARA_WEAPON_INFO * pDest)
	{
		i3mem::Copy( pDest->m_Info, _WeaponList,  sizeof( ITEM_INFO ) * CHAR_EQUIPMENT_WEAPON_COUNT );
	}

	UINT8	getEndurance( UINT32 idx ) const	{ return _WeaponEndurance[idx];	}
	void	setEndurance( const UINT8* en)		{ i3mem::Copy( _WeaponEndurance, en, sizeof(UINT8)*CHAR_EQUIPMENT_WEAPON_COUNT ); }
	void	setEndurance( INT32 idx, UINT8 en)	{ _WeaponEndurance[idx] = en;	}

	UINT8	getHP( void) const					{ return _Hp; }
	void	setHP( UINT8 hp)					{ _Hp = hp; }

	INT32	getRespawnIndex( void) const		{ return _RespawnIdx; }
	void	setRespawnIndex( INT32 idx)			{ _RespawnIdx = idx; }

	INT32	getRespawnObjIndex(void) const		{ return _RespawnObjIdx;	}
	void	setRespawnObjIndex(INT32 idx)		{ _RespawnObjIdx	= idx;	}

	UINT8	getMyRespawnCount(void) const		{ return _MyRespawnCnt; }
	void	setMyRespawnCount( UINT8 count )	{ _MyRespawnCnt = count; }

	const VEC3D*	getRespawnPos(void) const	{ return &_RespawnPos;	}
	void	setRespawnPos( const VEC3D * pos)	{ i3Vector::Copy( &_RespawnPos, const_cast<VEC3D*>(pos) );	}

	UINT8	getCurrentCharaIndex( void) const	{ return _CurrentCharaIdx; }
	void	setCurrentCharaIndex( UINT8 idx)	{ _CurrentCharaIdx = idx; }

	bool	isAIClient( void) const				{ return _bAIClient; }
	void	setAIClient( bool bAI)				{ _bAIClient = bAI; }

	void	ApplyEquipUserTitle(const UINT8 (&ui8EquipUserTitle)[ MAX_EQUIP_USER_TITLE ]);
	void	ApplyEquipUserTitle(INT32 slotIdx, UINT8 desigSetType);
	void	DisableEquipUserTitle(INT32 slotIdx);
	const DesigApplyInfo* GetDesigApplyInfo() const {  return _pDesigApplyInfo; }		// nullptr값일수 있다.
	
	UINT8	GetDesigSetTypeBySlot(INT32 iSlot) const { return _aDesigSetType[iSlot]; }

	void							ResetSkill( INT32 slot);
	const SKILLSYSTEM_APPLY_INFO*	getSkillsystem( INT32 iSlot) const;
	SKILLSYSTEM_APPLY_INFO *		setSkillsystem( INT32 iSlot);
	
	void					SetSkillsystem( const CHARA_SLOT_SKILL& skill, const char * pszFile = nullptr, INT32 line = 0);

	REAL32 getWeaponSkillValue( WEAPON_CLASS_TYPE weapontype, SKILL_TYPE skilltype) const;
	REAL32 getCharaSkillValue( SKILL_TYPE skilltype) const;

	void	Reset( void);
	void	CopyTo( NET_CHARA_INFO * pDest);
	void	CopyTo( UI_CHARA_INFO * pDest, INT team = TEAM_RED );							// 하휘호환(UI_CHARA_INFO)를 위해 만듬. 실제로는 쓰면 안됨 디버깅용도로만 써줬으면....

	void		setCharIDTeam( int team, T_ItemID CharaID ) { _CharacterID[team] = CharaID; }
	T_ItemID	GetCharaIDTeam( int team = 0 )				{ return _CharacterID[team]; }
};
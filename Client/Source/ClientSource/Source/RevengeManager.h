#ifndef		__REVENGE_MANAGER_H__
#define		__REVENGE_MANAGER_H__

//	Revenge System(복수시스템)
//	하동익

struct RevengeUser {
	bool		m_bRevengeUserState;	//리벤지 유저 여부
	UINT8		m_nChainKill;			//연속킬
	INT16		m_nKillDeathCount;		//킬뎃 카운트
} ;

//	러브샷 구조체
struct LoveShotUser {
	REAL32		m_Time;
	bool		m_state;
} ;

enum REVENGE_TYPE{	// 복수시스템 킬 카운트 설정
	REVENGE_TYPE_NONE = -1,

	REVENGE_TYPE_HOTKILLER = 5,
	REVENGE_TYPE_NIGHTMARE = 6,
	REVENGE_TYPE_MEATCHOPPER = 7,

	REVENGE_TYPE_PAYBACK = -5,
	REVENGE_TYPE_AVENGER = -6,
	REVENGE_TYPE_STALKER = -7,

	REVENGE_TYPE_LOVESHOT = 0,
	REVENGE_TYPE_AWESOME = 1,

	REVENGE_TYPE_MAX,

};

#include "tinst.h"
template<> struct is_tinst_manual<class CRevengeManager> : std::tr1::true_type {};

class	CRevengeManager : public i3ElementBase
{
	I3_CLASS_DEFINE(CRevengeManager, i3ElementBase);

	CRevengeManager();
	virtual ~CRevengeManager();

public:
	static	CRevengeManager*	i() { return tinst<CRevengeManager>(); }

private:
	RevengeUser		m_RevengeUser[SLOT_MAX_COUNT];
	LoveShotUser	m_LoveShotUser[SLOT_MAX_COUNT];
	LoveShotUser	m_LoveShotUserAfter[SLOT_MAX_COUNT];
	INT8			m_CurrentKilledUserIdx;				// 방금전 나에게 죽은 유저
	INT8			m_nMyHotKillerCount;				// 내가 핫킬러로 등록된 유저수
	i3::rc_wstring 	m_DeathNick;						// 방금전 나에게 죽은 유저닉네임
//	char			HotKillerMsg[MAX_STRING_COUNT];
	i3::rc_wstring		m_DeathWeaponName;
	INT32			m_DeathWeaponNum;
	bool			m_bLoveShot;
	bool			m_bReservedLoveShot;

public:
	void		InitData();

	bool		isBefore(INT32,INT32,INT32);

	void		RegistRevengeUser(INT32 idx)		{ m_RevengeUser[idx].m_bRevengeUserState = true;	};
	void		ResetRevengeUser(INT32 idx);
	void		ResetRevengeUserAll();
	void		ResetChainKillUser();

	bool		isRevengeUser(INT32 idx)			{ return m_RevengeUser[idx].m_bRevengeUserState;	};
	void		SetDeathUserNick(const i3::rc_wstring& wstrNick);

	void		SetDeathWeaponNum(INT32 num)		{ m_DeathWeaponNum = num;		};
	INT32		GetDeathWeaponNum(void)				{ return m_DeathWeaponNum;		};
	void		SetDeathWeaponName(const i3::rc_wstring& strName)	{ m_DeathWeaponName = strName; };
	const i3::rc_wstring&	GetDeathWeaponName() const				{ return m_DeathWeaponName;		};

	void		AddKillDeath(INT32);
	void		SubKillDeath(INT32);

	void		SetMyHotKillerCount();
	INT32		GetMyHotKillerCount()				{ return m_nMyHotKillerCount;	};

	INT32		GetCurrentMessage();

	i3::rc_wstring	getRevengeMessage(INT32);

	/* LoveShot Function*/
	//LoveShotUser*	getLoveShotUserInfo()				{ return m_LoveShotUser;				};
	void			SetLoveShotIdx(INT32 idx)			{ m_LoveShotUser[idx].m_state = true;	};
	void			SetLoveShotIdxAfter(INT32 idx)		{ m_LoveShotUserAfter[idx].m_state = true;	};
	bool			isLoveShot(INT32);
	bool			isLoveShotAfter(INT32);

	void			OnUpdate(REAL32);


};

#endif
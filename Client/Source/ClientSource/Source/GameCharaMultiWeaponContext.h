#pragma once

#include "Weapon/WeaponBase.h"
#include "GameCharaContextBase.h"
#include "GameCharaBase.h"

/* 다중무기 슬롯 정보 - 서버와의 통신은 Sync로 처리하며 클라 내에서는 UI Idx로 처리*/
struct MULTIWEAPON_INFO
{
	WeaponBase *	pWeapon = nullptr;		// Weapon 정보
	UINT8			nSlotNumber = 0;	// HUD 표시용
	UINT8			nSyncNumber = 0;	// 네트워크 싱크용
};

class CGameCharaMultiWeaponContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaMultiWeaponContext, CGameCharaContextBase);

	friend class CGameCharaBase;

public:
	CGameCharaMultiWeaponContext();
	virtual ~CGameCharaMultiWeaponContext();

	virtual void	OnCreate(CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy(void) override;

	//	죽거나 배틀 시작 시 다중무기 슬롯을 초기화
	void		InitMultiWeaponList();

	// 다중무기를 삭제
	void		DeleteMultiWeapon( WeaponBase* pWeapon);

	// 다중무기를 추가
	void		AddMultiWeapon(WeaponBase* pWeapon, UINT8 nSyncNumber = 0);

	// 다중무기를 변경
	void		ChangeMultiWeapon(WeaponBase* pWeapon);

	MULTIWEAPON_INFO*	GetCurrSlotInfo()		{ return m_pCurrentWeaponSlot; }		// 현재 장착한 다중무기 슬롯
	MULTIWEAPON_INFO*	GetPrevSlotInfo()		{ return m_pLeftWeaponSlot; }			// 좌측에 위치한 다중무기 슬롯
	MULTIWEAPON_INFO*	GetNextSlotInfo()		{ return m_pRightWeaponSlot; }			// 우측에 위치한 다중무기 슬롯
	UINT8				GetMultiWeaponCount()	{ return static_cast<UINT8>( m_MultiWeaponList.size()); }

	void				HasPickupChange()		{ m_bPickupChange = true; }
	void				ResetPickupChange()		{ m_bPickupChange = false; }
	bool				IsPickupChange()		{ return m_bPickupChange; }

	void				HasDropedChange( UINT8 index = 0);
	bool				IsDropedChange()		{ return m_bDropedChange; }
	void				ResetDropedChange()		{ m_bDropedChange = false; m_nNextSlotIndex = 0; }
	UINT8				GetNextSlotIndex()		{ return m_nNextSlotIndex; }

	void				HasHUDSelectedChange()	{ m_bHUDSelectedChange = true; }
	void				ResetHUDSelectedChange(){ m_bHUDSelectedChange = false; }
	bool				IsHUDSelectedChange()	{ return m_bHUDSelectedChange; }

	void				HasQuickChange()		{ m_bQuickChange = true; }
	void				ResetQuickChange()		{ m_bQuickChange = false; }
	bool				IsQuickChange()			{ return m_bQuickChange; }
	INT32				GetQuickChangeIdx()		{ return m_nQuickChangeIdx; }
	void				EmptyQuickChangeIdx()	{ m_nQuickChangeIdx = -1; }

	void				SetDropProhibit(bool bProhibit)		{ m_bDropProhibit = bProhibit; }
	bool				IsDropProhibit()					{ return m_bDropProhibit; }
	void				SetPickupProhibit(bool bProhibit)	{ m_bPickupProhibit = bProhibit; }
	bool				IsPickupProhibit()					{ return m_bPickupProhibit; }

	void				ResetChange();

	// Drop 시 다중무기 리스트에서 제거
	void		DropedMultiWeapon();

	void		SetQuickChangeIdx();
	WeaponBase* GetQuickChangeWeapon();

	WeaponBase* GetMultiWeapon( UINT8 index);
	WeaponBase* GetSwapMultiWeapon( bool bReset);

	MULTIWEAPON_INFO*	SearchIndexSlot( UINT8 uiIdx);
	MULTIWEAPON_INFO*	GetPrevMultiWeaponInfo(UINT8 idx);
	MULTIWEAPON_INFO*	GetNextMultiWeaponInfo(UINT8 idx);

	// Sync Index로 UI Index를 찾음
	UINT8				GetMultiWeaponIdx(UINT8 nSyncNum);

	// ItemID로 UI Index를 찾음
	UINT8				SearchItemIDMultiWeapon(WeaponBase* pWeapon);

	// 해당 Index 기준으로 다중무기 셋팅하는 함수
	void				SetMultiWeaponSlot( UINT8 idx);

	// 내 캐릭터가 사용하는 다중무기인지 확인하는 함수
	bool				IsUsingMultiWeapon( WeaponBase* pWeapon);

	// 다중무기 빈 슬롯이 있는지 체크
	bool				CheckEmptySlot();

	void				ClearWeaponSlotList();

#if defined(I3_DEBUG)
	void				OutputMulitWeaponList();
#endif

protected:
	i3::vector<MULTIWEAPON_INFO*>	m_MultiWeaponList;

	MULTIWEAPON_INFO*	m_pLeftWeaponSlot;
	MULTIWEAPON_INFO*	m_pCurrentWeaponSlot;
	MULTIWEAPON_INFO*	m_pRightWeaponSlot;

	INT32				m_nQuickChangeIdx;				// 빠른 무기 교체용

	bool				m_bPickupChange;
	bool				m_bDropedChange;
	bool				m_bHUDSelectedChange;
	bool				m_bQuickChange;

	bool				m_bDropProhibit;
	bool				m_bPickupProhibit;

	UINT8				m_nNextSlotIndex;
};
#pragma once

#include "iHudBase.h"
#include "UIHudDefine.h"

#include "../GameCharaMultiWeaponContext.h"

class UIHudManager;
class UIImgSetCtrl;

class UIHudMultiWeapon : public iHudBase
{
public:
	UIHudMultiWeapon(UIHudManager* p);
	virtual ~UIHudMultiWeapon();

	virtual void InitializeAtLoad(i3UIScene * pScene) override;
	virtual void ClearAtUnload(i3UIScene * pScene) override;
	virtual void Update(REAL32 rDeltaSeconds) override;
	virtual void SetEnable(bool enable) override;

	// Context에서 변경된 정보로 UI를 변경
	void UpdateMultiWeaponSlot();

	// HUD를 On/Off 해준다
	void ShowMultiWeaponSlot( bool bAuto = false);

	// Slot을 변경(bForward가 TRUE일 경우 우측으로 이동)
	void SetMultiWeaponSlotImage(BOOL bForward = TRUE);
	
	void OffAutoClosed();

	void SetCurrentSlotIdx( UINT8 idx)		{ m_nCurrentSlotIdx = idx; }
	UINT8 GetCurrentSlotIdx()				{ return m_nCurrentSlotIdx; }

	void SetEnableFire( BOOL enable)		{ m_bEnableFire = enable; }
	BOOL GetEnableFire()					{ return m_bEnableFire; }

	bool GetAutoView()						{ return m_bAutoView; }

private:
	UIImgSetCtrl *			m_pCurrMultiWeapon;	// 현재 위치의 다중무기 Slot(현재 장착한 무기)
	i3UIButtonImageSet *	m_pCurrSlotNumber;	// 현재 위치의 Slot 번호

	UIImgSetCtrl *			m_pNextMultiWeapon;	// 다음 위치의 다중무기 Slot(이전에 장착했던 무기)
	i3UIButtonImageSet *	m_pNextSlotNumber;	// 다음 위치의 Slot 번호

	UIImgSetCtrl *			m_pPrevMultiWeapon;	// 이전 위치의 다중무기 Slot(다음에 장착할 다중무기)
	i3UIButtonImageSet *	m_pPrevSlotNumber;	// 이전 위치의 Slot 번호 

	bool		m_bAutoView;			// 무기 습득으로 인한 자동 HUD인지
	BOOL		m_bEnableFire;			// HUD off 이후 바로 발사 불가
	REAL32		m_fLifeTime;			// 무기 습득으로 인한 자동 HUD를 off 시키기 위한 시간
	UINT8		m_nCurrentSlotIdx;		// HUD On 상태일 경우에서 현재 다중무기

	// UI Slot 셋팅
	void _SetWeaponShapeImage(UIImgSetCtrl* ctrl, T_ItemID itemID);

	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
};
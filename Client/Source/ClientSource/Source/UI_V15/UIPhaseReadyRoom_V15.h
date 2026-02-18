/**
 * \file	UI\UIPhaseReadyRoom_V15.h
 *
 * Declares the user interface phase ready room class.
 */


#if !defined( __UI_PHASE_READY_ROOM_V10_H__)
#define __UI_PHASE_READY_ROOM_V10_H__

#include "UI/UIPhaseBase.h"
#include "UI/UIPhaseReadyRoom.h"
#include "UI/room_detail.h"
#include "UI/UIDefine.h"



class UIPhaseReadyRoom_V15 : public UIPhaseReadyRoom
{
	I3_CLASS_DEFINE( UIPhaseReadyRoom_V15, UIPhaseReadyRoom);

private:
	// 레디 버튼 딜레이
	bool			m_bReadyPushed;
	REAL32			m_rReadyDelay;

public:
	UIPhaseReadyRoom_V15();
	virtual ~UIPhaseReadyRoom_V15();

	virtual void		OnCreate( i3GameNode * pParent) override;
	virtual void		OnLoadAllScenes() override;
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		OnEntranceEnd(void) override;
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void		SetFrmWndPos() override;				// Frame window 정렬
	virtual void		UpdateCBBoxInfo(COMBO_TYPE BoxType) override;
	virtual void		ApplyLua_cbbox(COMBO_TYPE _cType, const i3::wliteral_range& wstrCondition, INT32 _cel) override;
	virtual void		ApplyLua_auto_team_change_enable(UINT32 stageID) override;

	virtual void		ChangeGameMode(INT32 _mode, INT32 _celidx) override;
	virtual void		SetLuaText() override;
	virtual void		CheckBeHost(void) override;

public:
	bool				IsReadyBtnPushed() const { return m_bReadyPushed; }
	void				SetReadyBtnOn() { m_bReadyPushed = true; }
	void				InitReadyDelayTime(REAL32 rReadyDelay) { m_rReadyDelay = rReadyDelay; }
};

#endif

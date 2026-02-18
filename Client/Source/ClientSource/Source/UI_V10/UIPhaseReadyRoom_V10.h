/**
 * \file	UI\UIPhaseReadyRoom_V10.h
 *
 * Declares the user interface phase ready room class.
 */


#if !defined( __UI_PHASE_READY_ROOM_V10_H__)
#define __UI_PHASE_READY_ROOM_V10_H__

#include "UI/UIPhaseBase.h"
#include "UI/UIPhaseReadyRoom.h"
#include "UI/room_detail.h"
#include "UI/UIDefine.h"

class ChangeMode
{
public:
	ChangeMode():rTime(0), iShapeIdx(0){}
	virtual ~ChangeMode(){}

	void		SetStateAni(INT32 state)		
	{ 
		ChangeAni[0]->EnableCtrl(false);
		ChangeAni[1]->EnableCtrl(false);
		Framewnd->EnableCtrl(true);
		if( state != -1)
		{
			ChangeAni[state]->EnableCtrl(true);
		}

		iShapeIdx = 0;
		iStateAni = state; 
		
	}
	void		SetControl(i3UIFrameWnd* frame_wnd)
	{ 
		Framewnd = frame_wnd;
		ChangeAni[0] = (i3UIButtonImageSet*)frame_wnd->FindChildByName( "ChangeMode1");
		ChangeAni[1] = (i3UIButtonImageSet*)frame_wnd->FindChildByName( "ChangeMode2");
	}
	void		OnUpdate(REAL32 rDeltaSeconds)
	{
		if( iStateAni == 1)
		{
			rTime += rDeltaSeconds;
			if( rTime > 0.15)
			{
				rTime = 0.f;
				iShapeIdx++;
				if( iShapeIdx > 5)
					iShapeIdx = 0;

				ChangeAni[iStateAni]->SetShapeIdx(iShapeIdx);
			}
		}
	}

	i3UIButtonImageSet*		ChangeAni[2];
	i3UIFrameWnd*			Framewnd;
	INT32					iStateAni;
	INT32					iShapeIdx;
	REAL32					rTime;
};


class UIPhaseReadyRoom_V10 : public UIPhaseReadyRoom
{
	I3_CLASS_DEFINE( UIPhaseReadyRoom_V10, UIPhaseReadyRoom);
private:
	INT32				m_nCurComboData[COMBO_MAX];	// 콤보박스 관련. 현재 위치한 셀의 Index
	ChangeMode			m_ChangMode;

	// 레디 버튼 딜레이
	bool			m_bReadyPushed;
	REAL32			m_rReadyDelay;

	i3UIComboBox*			m_pStageComboBox;

public:
	UIPhaseReadyRoom_V10();
	virtual ~UIPhaseReadyRoom_V10();

	virtual void		OnCreate( i3GameNode * pParent) override;
	virtual void		OnLoadAllScenes() override;
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		OnEntranceEnd( void) override;
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void		SetFrmWndPos() override;				// Frame window 정렬
	virtual void		UpdateCBBoxInfo(COMBO_TYPE BoxType) override;
	virtual void		ApplyLua_cbbox(COMBO_TYPE _cType, const i3::wliteral_range& wstrCondition, INT32 _cel) override;
	virtual void		ApplyLua_auto_team_change_enable(UINT32 stageID) override;

	virtual void		ChangeGameMode(INT32 _mode, INT32 _celidx) override;
	virtual void		SetLuaText() override;

	virtual void 		CheckBeHost(void) override;
	virtual bool		OnRevive(i3RenderContext * pCtx) override;

	void				Apply_Title();
	void				Apply_RandomMap(bool bChange = true);
	void				SetModeAni(INT32 state = 0);
	void				ChangeMode();
	void				FocusOnStageComboBox(bool enable);

public:
	void				CheckBattleChallengeComplate();
	bool				isTrainingMode( void);
	
	void				_GetUserInfo(INT32 idxGameSlot);

public:
	void				ClickSlot (INT32 idx);
	void				ClickRSlot(INT32 idx);

public:
	bool				IsReadyBtnPushed() const { return m_bReadyPushed; }
	void				SetReadyBtnOn() { m_bReadyPushed = true; }
	void				InitReadyDelayTime(REAL32 rReadyDelay) { m_rReadyDelay = rReadyDelay; }
};

#endif

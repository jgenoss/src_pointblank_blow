/**
 * \file	UI\UIPhaseNonClanMember.h
 *
 * Declares the user interface phase non clan member class.
 */


#if !defined( __UI_PHASE_NONCLANMEMBER_H__)
#define __UI_PHASE_NONCLANMEMBER_H__

#define	CLAN_SLOT_COUNT 8

#include "UIPhaseClanBase.h"
#include "UIFlowEditBox.h"
#include "UIClanSlot.h"


class CClanContext;

class UIPhaseNonClanMember : public UIPhaseClanBase
{
	I3_CLASS_DEFINE( UIPhaseNonClanMember, UIPhaseClanBase);

private:
	UIClanSlot			m_ClanSlot[CLAN_SLOT_COUNT];

	INT32				m_i32CurSelClanIdx;

private:
	i3UIScrollBar *		m_pClanListScroll;
	i3UIImageBoxEx *	m_pDetailClanMark;
	i3UIEditBox *		m_pSearchEdit;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void	OnLoadAllScenes(void) override;

	void			_InitScrollBar(void);
	void			_UpdateClanList(void);
	void			_UpdateClanDetailInfo( void);

	void			_UpdateClanIntroFlow(REAL32 rDeltaSeconds);
	void			_UpdateSlotPushFlag(void);

public:
	UIPhaseNonClanMember();
	virtual ~UIPhaseNonClanMember();

	virtual void	OnCreate(i3GameNode * pParent) override;

	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng ) override;
	virtual bool	OnKey_Enter( void) override;
	virtual void	Leave(void) override;
	
	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;

	void			ClickClanSlot(INT32 i32Slot);
	void			ClanListScroll( void);
	void			ClanListWheel(INT32 zDelta);
	void			SearchClan( void);
	void			JoinClan( void);
	void			CreateClan( void);
	void			VisitClan( INT32 i32Slot);

private:
	typedef void	(UIPhaseNonClanMember::*call_func)(INT32,const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;
	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_MemberInfo( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_CreationInfo( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_ClanList_DetailInfo( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_DetailInfo( INT32 arg, const i3::user_data&);
	
	
};

#endif

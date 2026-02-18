/**
 * \file	UI\UIBattleFrame.h
 *
 * Declares the user interface battle frame class.
 */

#if !defined( __UI_BATTLE_FRAME_H__)
#define __UI_BATTLE_FRAME_H__

#include <unordered_map>

#include "UIBattleDef.h"
#include "UIPopupFocus.h"

class UIBattlePopupBase;
class UIHUDBase;

template<> struct is_tinst_manual<UIBattleFrame> : std::tr1::true_type {};

class UIBattleFrame : public i3GameNode
{
	I3_CLASS_DEFINE( UIBattleFrame, i3GameNode);

public:
	static UIBattleFrame* i() { return tinst<UIBattleFrame>(); }

private:
	UIHUDBase *			m_pHUD[ UIHUD_MAX];
	UIBattlePopupBase *	m_pPopup[ UBP_MAX];
	
	UIPopupFocus		m_popup_focus;
	
	class igshop_ctrl*	m_shop_ctrl;
public:
	/** \brief Popup 이외의 창에 대해서 INput을 꺼줍니다.		*/
	void			SetGameShopValidTime(INT32 sec);
	void			SetRespawn();
	void			SetDeath();
	void			SetDeathDelay();
	void			SetRoundEnd();
	void			SetGameEnd();

	void			SetIntermissionStart();
	void			SetIntermissionEnd();

	void			FocusPopup(UIPopupBase* popup, bool bFocus);
	bool			IsPopupFocused() const { return m_popup_focus.IsAnyPopupOpened(); }
	bool			IsPopupFocused(UIPopupBase* popup) const { return m_popup_focus.IsActivePopup(popup); }
	bool			IsFocusedTopPopup(UIPopupBase* popup) const { return m_popup_focus.IsTopPopup(popup); }
	bool			IsPopupClosedAll() const { return ( m_numRenderPopup < 1 ); }
	void			InputDisableMainUI(bool bEnable);
	
	void			InputDisable(bool bEnable);
	//----------------------------------------------------------------------//
	//							inline function								//
	//----------------------------------------------------------------------//
public:
	UIHUDBase *		GetHUD( UIHUD hud)
	{
		I3_BOUNDCHK( hud, UIHUD_MAX);
		return m_pHUD[ hud];
	}

	UIBattlePopupBase *	GetPopup( UIBATTLE_POPUP type)
	{
		I3_BOUNDCHK( type, UBP_MAX);
		return m_pPopup[ type];
	}


public:
	UIBattleFrame();
	virtual ~UIBattleFrame();

	void	OnCreate( void);

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	void			ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData);

	void			EnableHUD( UIHUD hud, bool bEnable);

	void			UpdateChatBuffer();	//인게임채팅
	
	INT32			FindPopup( LuaState * L);
	UIBATTLE_POPUP	FindPopupByPointer( UIBattlePopupBase * pPopup);

	bool			IsOpenPopup( UIBATTLE_POPUP type );

	void			TogglePopup( UIBATTLE_POPUP type, void * pArg1 = nullptr, void * pArg2 = nullptr);
	void			OpenPopup(UIBATTLE_POPUP type, void * pArg1 = nullptr, void * pArg2 = nullptr);
	void			ClosePopup( UIBATTLE_POPUP type);

	void			CloseAll();		// 배틀 종료시에는 팝업된 창이 있다면 모두 꺼둡니다....

private:
	void			AddPopupCount(INT32 cnt);
	INT32			m_numRenderPopup;
	USER_INFO_EQUIPMENT_WEAPON	m_InfoEquipWeapon;

	friend class    UIBattlePopupBase;

private:
	typedef void	(UIBattleFrame::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Open_IGShop( INT32 arg, const i3::user_data&);

	void			ReceiveGameEvent_KickVote_Suggest_Result( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Start( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Update( INT32 arg, const i3::user_data&);
	
};

namespace CrossCounter_Util
{
	UIBATTLE_POPUP GetStyle();
};
namespace cc = CrossCounter_Util;

#endif

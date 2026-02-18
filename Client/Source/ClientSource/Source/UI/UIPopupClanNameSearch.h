#if !defined( __UI_POPUP_CLAN_SEARCH_H__)
#define __UI_POPUP_CLAN_SEARCH_H__

#include "UIPopupClanBase.h"

class UIPopupClanNameSearch : public UIPopupClanBase
{
	I3_CLASS_DEFINE( UIPopupClanNameSearch, UIPopupClanBase);
	i3UIEditBox * m_pSerach;
	
	bool m_bPopupTC;
	bool m_bCheckedName;
	bool m_bCheckedAddress;

	bool m_bCreateConfirm;
	bool m_bWaitingIsDuplicateName;
	bool m_bWaitingIsDuplicateAddress;

public:
//	bool m_bCheckingName;			// 쓰이지 않으므로 변수제거.(11.09.20.수빈)
//	bool m_bCheckingAddress;

	i3::rc_wstring	m_wstrClanName;
	i3::rc_wstring  m_wstrURL;			// 쓰이지는 않는것 같음..일단 형변환..(2014.07.14.수빈)

	UIPopupClanNameSearch(void);
	~UIPopupClanNameSearch(void);
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual void OnCreate( i3GameNode * pParent) override;
	virtual bool OnKey_Enter() override { _OverLappingOK(); return true; }

	void _OverLappingCheck( LuaState * L);
	void __OverLappingCheck();

	void   _OverLappingOK();
	void   __OverLappingOK();
	
	virtual bool	_CheckClanAddressOverLap(LuaState * L);
	virtual bool	_CheckClanAddressValidate(const char * pszText);

	bool	_OnPopupCheckOverlapping( const i3::rc_wstring& wstrName, bool bDuplicate);		// virtual 제거..

	/*
	param[in] pArg1 SearchInfo*
	*/
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

private:
	typedef void	(UIPopupClanNameSearch::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void		RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void		ReceiveGameEvent_DuplicateName( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_DuplicateAddress( INT32 arg, const i3::user_data&);
		
};
#endif
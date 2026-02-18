#if !defined( __UI_POPUP_CREATE_CLAN_H__)
#define __UI_POPUP_CREATE_CLAN_H__

#include "UIPopupClanBase.h"

class UIPopupCreateClan : public UIPopupClanBase
{
	I3_CLASS_DEFINE( UIPopupCreateClan, UIPopupClanBase);

private:
	i3::rc_wstring	m_wstrClanName;			//[NET_CLAN_NAME_SIZE];
	i3::rc_wstring 	m_wstrClanURL;			// [NET_CLAN_AZIT_URL_SIZE];

	// 추후 클랜소개가 추가 될지도 몰라 만들어둡니다.
	i3::rc_wstring 	m_wstrClanIntro;

	bool			m_bWaitRequestCheck;		// 서버로 요청을 보냈는지 체크

	bool			m_bCompleteNameCheck;
	bool			m_bCompleteURLCheck;

protected:
	void			_InitControl( void);
	void			_SetCheckResultText(const i3::wliteral_range& wTextRng, const I3COLOR * pColor);

	void			_SetCheckResultSuccess( const i3::wliteral_range& wTextRng);
	void			_SetCheckResultFail( const i3::wliteral_range& wTextRng);

	void			_GetClanNameText( void);
	void			_GetClanURLText( void);

	
public:
	
	UIPopupCreateClan();
	virtual ~UIPopupCreateClan();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr , void * pArg2 = nullptr ) override;
	virtual void	OnEntranceEnd(void) override;

	virtual bool	OnKey_Enter(void) override { CreateClan(); return true; }
	virtual bool	OnKey_Escape(void) override { Close(); return true; }

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	void			CheckClanName(void);
	void			CheckClanURL( void);
	void			CreateClan( void);
	void			Close( void);

	// CallBack Function
	static void 	cbSuccessCreateClan(void* pThis, INT32 nParam);

private:
	typedef void	(UIPopupCreateClan::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_DuplicateClanName( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_DuplicateAddress( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_Clan( INT32 arg, const i3::user_data&);

};
#endif

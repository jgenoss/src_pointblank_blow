#if !defined( __UI_FLOAT_MINIPROFILE_H__)
#define __UI_FLOAT_MINIPROFILE_H__

#include "UIFloatingBase.h"

#define PROFILE_MARK_SIZE	42

namespace
{
	class UIClanMark
	{
	private:
		i3UIImageBoxEx *	m_pClanMark;
		i3UIImageBox *		m_pDefaultClanMark;

	public:
		UIClanMark(void);
		~UIClanMark(void);

		void SetClanMarkCtrl( i3UIImageBoxEx * pClanMark, i3UIImageBox * pDefClanMark );
		void SetClanMarkFlag( UINT32 ui32MarkFlag );
	};
};

class UIFloatMiniProfile : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatMiniProfile, UIFloatingBase);

private:
	UIClanMark		m_ClanMark;

	void			__UpdateUserInfo( void);

public:
	UIFloatMiniProfile();
	virtual ~UIFloatMiniProfile();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual bool	OnExitStart( void) override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

protected:
	void			_SetDefaultInfo(const USER_INFO_BASIC & pInfo);
	void			_SetDetailInfo(const USER_INFO_BASIC & pInfo);
	void			_SetUserExp(const USER_INFO_BASIC & pInfo);
	void			_SetSeasonInfo(void);
	void			_ResetSeasonInfo(void);

public:
	void			OnModifyCurrentPosition(const i3::wliteral_range& wTextRng);
	void			ToggleUserInfo(bool bDetail, bool bClickBtn);
	void			CloseDetailInfo(i3UIControl* pCtrl);


	//----------------------------------------------------------------------//
	//						Received Game Eent Functions					//
	//----------------------------------------------------------------------//
private:
	typedef void	(UIFloatMiniProfile::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Update_UserInfo( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Update_SeasonInfo( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Reset_SeasonInfo(INT32, const i3::user_data&);
	void			ReceiveGameEvent_Clan_Deprotation( INT32, const i3::user_data&);
	
};

#endif

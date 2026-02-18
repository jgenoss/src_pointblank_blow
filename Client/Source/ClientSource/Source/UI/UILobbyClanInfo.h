/**
 * \file	UI\UILobbyClanInfo.h
 *
 * Declares the user interface lobby banner class.
 */


#if !defined( __UI_LOBBY_CLANINFO_H__)
#define __UI_LOBBY_CLANINFO_H__

#include "UIFloatingBase.h"

class UILobbyClanInfo : public UIFloatingBase
{
	I3_CLASS_DEFINE( UILobbyClanInfo, UIFloatingBase);

private:
	i3UIImageBoxEx *	m_pImageBox;

public:
	UILobbyClanInfo();
	virtual ~UILobbyClanInfo();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;

	virtual void	ProcessGameEvent( GAME_EVENT Event, INT32 Arg, const i3::user_data& UserData) override;

	void			SetLuaClanInfo(void);
};

#endif

/**
 * \file	UI\UILobbyBanner.h
 *
 * Declares the user interface lobby banner class.
 */


#if !defined( __UI_LOBBY_BANNER_H__)
#define __UI_LOBBY_BANNER_H__

#include "UIFloatingBase.h"

class UINewAndHotBanner;

class UILobbyBanner : public UIFloatingBase
{
	I3_CLASS_DEFINE( UILobbyBanner, UIFloatingBase);

private:
	i3UIImageBoxEx *	m_pImageBox;

	//UINewAndHotBanner *		m_pSubNewAndHotBanner;
	i3Texture *			m_pTex;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UILobbyBanner();
	virtual ~UILobbyBanner();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	
};

#endif

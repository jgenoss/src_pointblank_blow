/**
 * \file	UI\UILobbyBanner_V11.h
 *
 * Declares the user interface lobby banner class.
 */


#if !defined( __UI_LOBBY_BANNER_V11_H__)
#define __UI_LOBBY_BANNER_H__

#include "UI/UIFloatingBase.h"

class UINewAndHotBanner;

class UILobbyBanner_V11 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UILobbyBanner_V11, UIFloatingBase);

private:
	i3UIImageBoxEx *	m_pImageBox;

	i3Texture *			m_pTex;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UILobbyBanner_V11();
	virtual ~UILobbyBanner_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	/** \brief Sliding 명령을 줍니다.
		\return N/A */
	virtual void	OnSlideIn( void) override;

	/** \brief Sliding 명령을 줍니다.
		\return N/A */
	virtual void	OnSlideOut( void) override;
	
};

#endif

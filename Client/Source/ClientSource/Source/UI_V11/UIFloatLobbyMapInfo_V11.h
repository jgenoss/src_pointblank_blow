#if !defined( __UI_FLOAT_LOBBY_MAPINFO_V11_H__)
#define __UI_FLOAT_LOBBY_MAPINFO_V11_H__

#include "UI/UIFloatingBase.h"

class UIFloatLobbyMapInfo_V11 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatLobbyMapInfo_V11, UIFloatingBase);

private:
	bool			m_bRequestRoomInfoAdd;
	INT32			m_SelectedElement;

private:
	virtual void	_InitializeAtLoad(i3UIScene * pScene);

public:
	UIFloatLobbyMapInfo_V11();
	virtual ~UIFloatLobbyMapInfo_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 ROOM_INFO_BASIC*
	param[in] pArg2 ROOM_INFO_ADD*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;

	void			SetMapInfo( ROOM_INFO_BASIC * pInfo, ROOM_INFO_ADD * pAddInfo);
	
};

#endif

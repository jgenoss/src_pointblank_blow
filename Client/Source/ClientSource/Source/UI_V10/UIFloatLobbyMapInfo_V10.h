#pragma once

#include "../UI/UIFloatingBase.h"

class UIFloatLobbyMapInfo_V10 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatLobbyMapInfo_V10, UIFloatingBase);

	enum STATIC_TEXT
	{
		STATIC_MAPINFO = 0,
		STATIC_GAMEMODE,
		STATIC_GAMESETTING,
		STATIC_ROOMMASTER,
		STATIC_ROOMSTATE,
		STATIC_JOINLIMIT,
		STATIC_WEAPONLIMIT,
		STATIC_OBSERVERTEXT,
		STATIC_TEAMBALANCE,

		STATIC_TEXT_MAX,
	};

public:
	UIFloatLobbyMapInfo_V10();
	virtual ~UIFloatLobbyMapInfo_V10();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 ROOM_INFO_BASIC*
	param[in] pArg2 ROOM_INFO_ADD*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart(void) override;

private:
	void			_DefaultInfo();
	void			_SetMapInfo(const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo);

	void			_SetInfoText(const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo);
	void			_SetInfoImage(const ROOM_INFO_BASIC * pInfo);

	void			__GetMapName(const ROOM_INFO_BASIC * pInfo, i3::wstring& output);
	void			__GetGameSettingText(const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo, i3::wstring& output);
	void			__GetRoomStateText(const ROOM_INFO_BASIC * pInfo, i3::wstring& output);
	void			__GetJoinLimit(const ROOM_INFO_ADD * pAddInfo, i3::wstring& output);
	void			__GetWeaponLimit(const ROOM_INFO_BASIC * pInfo, i3::wstring& output);
	void			__GetObserverText(const ROOM_INFO_ADD * pAddInfo, i3::wstring& output);
	void			__GetTeamBalance(const ROOM_INFO_ADD * pAddInfo, i3::wstring& output);

public:
	void			EnableToolTip( const ROOM_INFO_BASIC * pArg1 = nullptr, const ROOM_INFO_ADD * pArg2 = nullptr);
	void			DisableToolTip(void);

	void			ChangeToolTip( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

private:
	i3::vector<i3UIStaticText*>	vecStaticText;
	
	i3UIButtonImageSet *		m_pMapImage;
	i3UIButtonImageSet *		m_pGameModeImage;

	i3UIImageBox *				m_pNewIcon;
	i3UIImageBox *				m_pHeadHunter;
	i3UIImageBox *				m_pImgRandMapEvent = nullptr;
};